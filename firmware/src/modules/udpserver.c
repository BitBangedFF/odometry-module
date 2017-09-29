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
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "config.h"
#include "nvicconf.h"
#include "debugio.h"
#include "led.h"
#include "system.h"
#include "ethernetif.h"
#include "udpserver.h"

static struct netconn *conn;
static ip_addr_t ipaddr;
static ip_addr_t netmask;
static ip_addr_t gw;
static struct netif gnetif;
static bool is_init = false;

static void notify(struct netif * const netif)
{
    if(netif_is_up(netif) != 0)
    {
        led_on(LED_ETH_STATUS);
    }
    else
    {
        led_off(LED_ETH_STATUS);
    }
}

static void netif_config(void)
{
    IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
    IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
    IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);

    // add the network interface
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    // register default network interface
    netif_set_default(&gnetif);

    if(netif_is_link_up(&gnetif) != 0)
    {
        netif_set_up(&gnetif);
    }
    else
    {
        netif_set_down(&gnetif);
    }

    netif_set_link_callback(&gnetif, &ethernetif_update_config);
}

static void udpserver_init(void)
{
    if(is_init == false)
    {
        // create tcp_ip stack thread
        tcpip_init(NULL, NULL);

        // inialize the lwip stack
        netif_config();

        // start the UDP server IO task
        //udpserver_io_start?

        notify(&gnetif);

        is_init = true;
    }

    debug_puts("udpserver_init");
}

static void udpserver_init_task(void *params)
{
    (void) params;
    err_t nc_err;

    led_off(LED_ETH_STATUS);

    system_wait_for_start();

    udpserver_init();

    /*
    conn = netconn_new(NETCONN_UDP);

    if(conn != NULL)
    {
        nc_err = netconn_bind(conn, IP_ADDR_ANY, (uint16_t) UDPSERVER_PORT);

        if(nc_err != ERR_OK)
        {
            netconn_delete(conn);
            conn = NULL;
        }
    }
    */

    while(1)
    {
        vTaskDelay(M2T(500));
        notify(&gnetif);
    }
}

void ethernetif_notify_conn_changed(struct netif *netif)
{
    if(netif_is_link_up(netif) != 0)
    {
        debug_puts("network connected");

        IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
        IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
        IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);

        netif_set_addr(netif, &ipaddr , &netmask, &gw);

        netif_set_up(netif);
    }
    else
    {
        debug_puts("network disconnected");

        netif_set_down(netif);
    }

    notify(netif);
}

void udpserver_start(void)
{
    (void) xTaskCreate(
            udpserver_init_task,
            UDPSERVER_INIT_TASK_NAME,
            UDPSERVER_INIT_TASK_STACKSIZE,
            NULL,
            UDPSERVER_INIT_TASK_PRI,
            NULL);
}
