#include "led.h"
#include "humidity.h"

int main()
{
    int j;

    led_init();

    led_thread_start();
    scanf("%d", &j);

    led_thread_stop();

    return 0;
}