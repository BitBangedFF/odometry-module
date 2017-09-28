/**
 * @file udpserver.c
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
#include "system.h"
#include "udpserver.h"

static bool is_init = false;

static void udpserver_init(void)
{
    if(is_init == false)
    {

        is_init = true;
    }

    debug_puts("udpserver_init");
}

static void udpserver_task(void *params)
{
    (void) params;

    system_wait_for_start();

    udpserver_init();

    // terminate this task
    while(1)
    {
        //vTaskDelete(NULL);
        vTaskSuspend(NULL);
    }
}

void udpserver_start(void)
{
    //tcpip_init(NULL, NULL);

    //Netif_Config();

    //udpecho_init();
    //http_server_netconn_init();

    //User_notification(&gnetif);

    (void) xTaskCreate(
            udpserver_task,
            UDPSERVER_TASK_NAME,
            UDPSERVER_TASK_STACKSIZE,
            NULL,
            UDPSERVER_TASK_PRI,
            NULL);
}
