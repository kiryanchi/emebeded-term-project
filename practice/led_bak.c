#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FILEPATH "/dev/fb0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_BLACK 0x0000
#define RGB565_YELLOW 0xFFE6
#define RGB565_MAGENTA 0xF81F
#define RGB565_CYAN 0x07FF
#define RGB565_PURPLE 0x801F

void delay(int);

int main(void)
{
  int i;
  int fbfd;
  uint16_t *map;
  uint16_t *p;
  struct fb_fix_screeninfo fix_info;

  fbfd = open(FILEPATH, O_RDWR);
  if (fbfd == -1) {
    perror("Error (call to 'open')");
    exit(EXIT_FAILURE);
  }
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
    perror("Error (call to 'ioctl')");
    close(fbfd);
    exit(EXIT_FAILURE);
  }

  if (strcmp(fix_info.id , "RPi-Sense FB") != 0) {
    printf("%s\n", "Error: RPi-Sense FB not found");
    close(fbfd);
    exit(EXIT_FAILURE);
  }

  map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if (map == MAP_FAILED) {
    close(fbfd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  p = map;

  memset(map, 0, FILESIZE);

  for (i = 0; i < NUM_WORDS; i++) {
    *(p + i) = RGB565_PURPLE;
    delay(25);
  }

  for (i = 0; i < 3; i++) {
    delay(250);
    memset(map, RGB565_YELLOW, FILESIZE);
    delay(250);
    memset(map, 0, FILESIZE);
  }
  delay(250);

  memset(map, 0, FILESIZE);

  if (munmap(map, FILESIZE) == -1) {
    perror("Error un-mmapping the file");
  }
  close(fbfd);

  return 0;
}

void delay(int t)
{
  usleep(t * 1000);
}
