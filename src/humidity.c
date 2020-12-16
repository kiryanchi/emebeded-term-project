#include "humidity.h"

void get_humi_temp(int fd, double* result)
{
  uint8_t status = 0;

  if ((fd = open(DEV_PATH, O_RDWR)) < 0)
  {
    perror("Unable to open i2c device");
    exit(1);
  }

  if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0)
  {
    perror("Unable to configure i2c slave device");
    close(fd);
    exit(1);
  }

  if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC)
  {
    printf("%s\n", "who_am_i error");
    close(fd);
    exit(1);
  }

  // Power down the device (clean start)
  i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

  // Turn on the humidity sensor analog front end in single shot mode
  i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

  // Rune one-shot measurement. The set bit will be reset by the sensor itself after execution (self-clearing bit)
  i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

  // wait until measurement is completed
  do
  {
    delay(25);
    status = i2c_smbus_read_byte_data(fd, CTRL_REG2);
  } while (status != 0);

  // Read calibration temprature LSB data
  uint8_t t0_out_l = i2c_smbus_read_byte_data(fd, T0_OUT_L);
  uint8_t t0_out_h = i2c_smbus_read_byte_data(fd, T0_OUT_H);
  uint8_t t1_out_l = i2c_smbus_read_byte_data(fd, T1_OUT_L);
  uint8_t t1_out_h = i2c_smbus_read_byte_data(fd, T1_OUT_H);

  // Read calibration temprature dgreeC data
  uint8_t t0_degC_x8 = i2c_smbus_read_byte_data(fd, T0_degC_x8);
  uint8_t t1_degC_x8 = i2c_smbus_read_byte_data(fd, T1_degC_x8);
  uint8_t t1_t0_msb = i2c_smbus_read_byte_data(fd, T1_T0_MSB);

  // Read calibration relative humidity LSB data
  uint8_t h0_out_l = i2c_smbus_read_byte_data(fd, H0_T0_OUT_L);
  uint8_t h0_out_h = i2c_smbus_read_byte_data(fd, H0_T0_OUT_H);
  uint8_t h1_out_l = i2c_smbus_read_byte_data(fd, H1_T0_OUT_L);
  uint8_t h1_out_h = i2c_smbus_read_byte_data(fd, H1_T0_OUT_H);

  // Read relative humidity rH data
  uint8_t h0_rh_x2 = i2c_smbus_read_byte_data(fd, H0_rH_x2);
  uint8_t h1_rh_x2 = i2c_smbus_read_byte_data(fd, H1_rH_x2);

  // make 16 bit values (bit shift)
  int16_t T0_OUT = t0_out_h << 8 | t0_out_l;
  int16_t T1_OUT = t1_out_h << 8 | t1_out_l;

  // make 16 bit values (bit shift)
  int16_t H0_T0_OUT = h0_out_h << 8 | h0_out_l;
  int16_t H1_T0_OUT = h1_out_h << 8 | h1_out_l;

  // make 16 and 10 bit values ( bit mask and bit shift)
  uint16_t T0_DegC_x8 = (t1_t0_msb & 3) << 8 | t0_degC_x8;
  uint16_t T1_DegC_x8 = ((t1_t0_msb & 12) >> 2) << 8 | t1_degC_x8;

  // Calculate calibration value
  double T0_DegC = T0_DegC_x8 / 8.0;
  double T1_DegC = T1_DegC_x8 / 8.0;

  double H0_rH = h0_rh_x2 / 2.0;
  double H1_rH = h1_rh_x2 / 2.0;

  double t_gradient_m = (T1_DegC - T0_DegC) / (T1_OUT - T0_OUT);
  double t_intercept_c = T1_DegC - (t_gradient_m * T1_OUT);

  double h_gradient_m = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
  double h_intercept_c = H1_rH - (h_gradient_m * H1_T0_OUT);

  uint8_t t_out_l = i2c_smbus_read_byte_data(fd, TEMP_OUT_L);
  uint8_t t_out_h = i2c_smbus_read_byte_data(fd, TEMP_OUT_H);

  int16_t T_OUT = t_out_h << 8 | t_out_l;

  uint8_t h_t_out_l = i2c_smbus_read_byte_data(fd, H_T_OUT_L);
  uint8_t h_t_out_h = i2c_smbus_read_byte_data(fd, H_T_OUT_H);

  int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;

  double T_DegC = (t_gradient_m * T_OUT) + t_intercept_c;

  double H_rH = (h_gradient_m * H_T_OUT) + h_intercept_c;

  result[0] = T_DegC;
  result[1] = H_rH;
}

// power off
void power_off_humi_temp(int fd)
{
  i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);
}

static void delay(int t)
{
  usleep(t * 1000);
}

void *humi_thread_renewal(void* arg)
{
  double humi_result[2];
  int temp = 0, humi = 0;
  FILE* fp;

  while(!humi_thr_exit)
  {
    fp = fopen("humi_data", "w");
    get_humi_temp(humi_fd, humi_result);
    temp = humi_result[0];
    humi = humi_result[1];

    fprintf(fp, "%d&%d\n", temp, humi);

    fclose(fp);

    delay(1000);
  }
}

void humi_thread_start()
{
  humi_thr_exit = false;
  humi_thr_id = pthread_create(&humi_thread, NULL, humi_thread_renewal, NULL);
}

void humi_thread_stop()
{
  humi_thr_exit = true;
  humi_thr_id = pthread_join(humi_thread, &humi_thread_return);
}