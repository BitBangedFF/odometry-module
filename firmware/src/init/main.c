/**
 * @file main.c
 * @brief Main.
 *
 */

#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#include "debug.h"
#include "platform.h"
#include "led.h"
#include "system.h"

int main(void)
{
    platform_init();

    system_start();

    vTaskStartScheduler();

    // should never get here
    while(1)
    {
        platform_error_handler();
    }

    return 0;
}
