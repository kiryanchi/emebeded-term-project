#ifndef LED_H
#define LED_H
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
#include <stdbool.h>

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

// If down, + 29
const char NUM_V_ZERO[] = {12, 0, 1, 2, 8, 10, 16, 18, 24, 26, 32, 33, 34};
const char NUM_V_ONE[] = {5, 2, 10, 18, 26, 34};
const char NUM_V_TWO[] = {11, 0, 1, 2, 10, 16, 17, 18, 24, 32, 33, 34};
const char NUM_V_THREE[] = {11, 0, 1, 2, 10, 16, 17, 18, 26, 32, 33, 34};
const char NUM_V_FOUR[] = {9, 0, 2, 8, 10, 16, 17, 18, 26, 34};
const char NUM_V_FIVE[] = {11, 0, 1, 2, 8, 16, 17, 18, 26, 32, 33, 34};
const char NUM_V_SIX[] = {12, 0, 1, 2, 8, 16, 17, 18, 24, 26, 32, 33, 34};
const char NUM_V_SEVEN[] = {9, 0, 1, 2, 8, 10, 16, 18, 26, 34};
const char NUM_V_EIGHT[] = {13, 0, 1, 2, 8, 10, 16, 17, 18, 24, 26, 32, 33, 34};
const char NUM_V_NINE[] = {10, 0, 1, 2, 8, 10, 16, 17, 18, 26, 34};

// If down, + 37
const char NUM_H_ZERO[] = {12, 3, 4, 5, 6, 7, 11, 15, 19, 20, 21, 22, 23};
const char NUM_H_ONE[] = {5, 3, 4, 5, 6, 7};
const char NUM_H_TWO[] = {11, 3, 4, 5, 7, 11, 13, 15, 19, 21, 22, 23};
const char NUM_H_THREE[] = {11, 3, 4, 5, 6, 7, 11, 13, 15, 19, 21, 23};
const char NUM_H_FOUR[] = {9, 3, 4, 5, 6, 7, 13, 19, 20, 21};
const char NUM_H_FIVE[] = {11, 3, 5, 6, 7, 11, 13, 15, 19, 20, 21, 23};
const char NUM_H_SIX[] = {12, 3, 5, 6, 7, 11, 13, 15, 19, 20, 21, 22, 23};
const char NUM_H_SEVEN[] = {9,3, 4, 5, 6, 7, 11, 19, 20, 21};
const char NUM_H_EIGHT[] = {13, 3, 4, 5, 6, 7, 11, 13, 15, 19, 20, 21, 22, 23};
const char NUM_H_NINE[] = {10, 3, 4, 5, 6, 7, 11, 13, 15, 19, 20, 21};

const char* V_ARR[] = {NUM_V_ZERO, NUM_V_ONE, NUM_V_TWO, NUM_V_THREE, NUM_V_FOUR, NUM_V_FIVE, NUM_V_SIX, NUM_V_SEVEN, NUM_V_EIGHT, NUM_V_NINE};
const char* H_ARR[] = {NUM_H_ZERO, NUM_H_ONE, NUM_H_TWO, NUM_H_THREE, NUM_H_FOUR, NUM_H_FIVE, NUM_H_SIX, NUM_H_SEVEN, NUM_H_EIGHT, NUM_H_NINE};

void _change_color(uint16_t *map, uint16_t color, uint16_t pos);
void led_clear(uint16_t *map);
void led_number(uint16_t *map, const char num[], uint16_t pos, uint16_t color);
#endif
