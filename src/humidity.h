#ifndef HUMIDITY_H
#define HUMIDITY_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdbool.h>

#define DEV_PATH "/dev/i2c-1"
#define DEV_ID 0x5F
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21

#define T0_OUT_L 0x3C
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F
#define T0_degC_x8 0x32
#define T1_degC_x8 0x33
#define T1_T0_MSB 0x35

#define TEMP_OUT_L 0x2A
#define TEMP_OUT_H 0x2B

#define H0_T0_OUT_L 0X36
#define H0_T0_OUT_H 0x37
#define H1_T0_OUT_L 0x3A
#define H1_T0_OUT_H 0x3B
#define H0_rH_x2 0x30
#define H1_rH_x2 0x31

#define H_T_OUT_L 0x28
#define H_T_OUT_H 0x29

static int humi_fd = 0;

static pthread_t humi_thread;
static void* humi_thread_return;
static bool humi_thr_exit = true;
static int humi_thr_id;

static void delay(int);

void get_humi_temp(int, double*);
void power_off_humi_temp(int);

void *humi_thread_renewal(void*);
void humi_thread_start();
void humi_thread_stop();

#endif