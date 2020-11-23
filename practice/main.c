
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
