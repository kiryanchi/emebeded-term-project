#include "led.h"

void led_init()
{
    fbfd = open(FILEPATH, O_RDWR);
    if (fbfd == -1)
    {
        perror("Error (call to 'open')");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1)
    {
        perror("Error (call to 'ioctl')");
        close(fbfd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(fix_info.id, "RPi-Sense FB") != 0)
    {
        printf("%s\n", "Error: RPi-Sense FB not found");
        close(fbfd);
        exit(EXIT_FAILURE);
    }

    led_map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (led_map == MAP_FAILED)
    {
        close(fbfd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    led_clear(led_map);
}

static void delay(int t)
{
  usleep(t * 1000);
}

void led_change_color(uint16_t *map, uint16_t color, uint16_t pos)
{
  *(map + pos) = color;
}

void led_clear(uint16_t *map)
{
  memset(map, 0, FILESIZE);
}

void led_number(uint16_t *map, const char num[], uint16_t pos, uint16_t color)
{
  int len = num[0];
  for (int i = 1; i <= len; i++)
  {
    *(map + num[i] + pos) = color;
  }
}

void led_draw_time(uint16_t *p, struct tm tm)
{
  led_number(p, V_ARR[tm.tm_hour / 10], 0, RGB565_YELLOW);
  led_number(p, V_ARR[tm.tm_hour % 10], 29, RGB565_YELLOW);
  led_number(p, H_ARR[tm.tm_min / 10], 37, RGB565_MAGENTA);
  led_number(p, H_ARR[tm.tm_min % 10], 0, RGB565_MAGENTA);
}

void led_close(uint16_t *map)
{
    // 0으로 초기화후(불을 다 끔)
    led_clear(map);

    if (munmap(map, FILESIZE) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(fbfd);
}


void *led_thread_clock(void *arg)
{
  printf("thread_function_enter\n");
  time_t now, before;
  struct tm nt;

  now = before = time(0);
  nt = *localtime(&now);
  led_draw_time(led_map, nt);

  while (!led_thr_exit)
  {
    now = time(0);
    if (now != before)
    {
      before = now;
      nt = *localtime(&now);
      led_clear(led_map);
      led_draw_time(led_map, nt);
    }
    delay(100);
  }
}

void led_thread_start()
{
  led_thr_exit = false;
  led_thr_id = pthread_create(&led_thread, NULL, led_thread_clock, NULL);
}

void led_thread_stop()
{
  led_thr_exit = true;
  led_thr_id = pthread_join(led_thread, &led_thread_return);
  led_clear(led_map);
}