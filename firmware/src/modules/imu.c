/**
 * @file imu.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"
#include "nvicconf.h"
#include "debugio.h"
#include "led.h"
#include "uart2.h"
#include "system.h"
#include "udp_protocol.h"
#include "udpserver.h"
#include "xsmessage.h"
#include "xsparser.h"
#include "imu.h"

static StaticTask_t rx_task_tcb;
static StackType_t rx_task_stack[IMU_RX_TASK_STACKSIZE];

static uint8_t rx_msg_buffer[XS_MAXGARBAGE];

static bool is_init = false;

static void rx_task(void *params)
{
    (void) params;
    uint8_t rx_data;

    system_wait_for_start();

    led_off(LED_UART2_STATUS);

    uart2_init(UART2_BAUDRATE);

    debug_puts(IMU_RX_TASK_NAME" started");

    while(1)
    {
        if(uart2_get_char(&rx_data) == true)
        {
            // TODO
            led_toggle(LED_UART2_STATUS);
        }
    }
}

static void imu_init(void)
{
    if(is_init == false)
    {
        (void) xTaskCreateStatic(
                &rx_task,
                IMU_RX_TASK_NAME,
                IMU_RX_TASK_STACKSIZE,
                NULL,
                IMU_RX_TASK_PRI,
                &rx_task_stack[0],
                &rx_task_tcb);

        is_init = true;
    }
}

void imu_start(void)
{
    imu_init();
}
