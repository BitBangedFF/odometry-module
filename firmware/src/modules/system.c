/**
 * @file system.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "config.h"
#include "nvicconf.h"
#include "debugio.h"
#include "led.h"
#include "uart1.h"
#include "uart2.h"
//#include "can1.h"
#include "udpserver.h"
#include "system.h"

static xSemaphoreHandle system_ready_mutex;
static bool is_init = false;

static void signal_ready_to_start(void)
{
    xSemaphoreGive(system_ready_mutex);
}

static void system_init(void)
{
    if(is_init == false)
    {
        system_ready_mutex = xSemaphoreCreateMutex();
        xSemaphoreTake(system_ready_mutex, portMAX_DELAY);

        is_init = true;
    }

    debug_puts("system_init");
}

static void system_task(void *params)
{
    (void) params;
    led_init();
    led_set(LED_SYSTEM_STATUS, true);

    uart1_init(UART1_BAUDRATE);

    uart2_init(UART2_BAUDRATE);

    //can1_init();

    system_init();

    udpserver_start();

    signal_ready_to_start();

    while(1)
    {
        led_toggle(LED_SYSTEM_STATUS);
        //led_off(LED_UART2_STATUS);

        vTaskDelay(M2T(500));
    }

    // should never get here
    while(1)
    {
        led_set_all(true);
        vTaskDelay(portMAX_DELAY);
    }
}

void vApplicationIdleHook(void)
{
    // TODO - update watchdog
}

void system_start(void)
{
    (void) xTaskCreate(
            system_task,
            SYSTEM_TASK_NAME,
            SYSTEM_TASK_STACKSIZE,
            NULL,
            SYSTEM_TASK_PRI,
            NULL);
}

void system_wait_for_start(void)
{
    while(is_init == false)
    {
        vTaskDelay(5);
    }

    xSemaphoreTake(system_ready_mutex, portMAX_DELAY);
    xSemaphoreGive(system_ready_mutex);
}
