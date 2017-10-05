/**
 * @file udpserver.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
#include "udp_protocol.h"
#include "udpserver.h"

typedef struct
{
    uint16_t offset;
    uint32_t data;
} queue_msg;

#define DATA_QUEUE_SIZE_LEN (32)

static const TickType_t UDP_TX_FREQ = M2T(100);
static const TickType_t LINK_STATUS_FREQ = M2T(500);
static const TickType_t TX_MUTEX_IO_TASK_TIMEOUT = M2T(2);
static const TickType_t TX_MUTEX_DATA_TASK_TIMEOUT = portMAX_DELAY;
static const TickType_t DATA_QUEUE_PUSH_TIMEOUT = 0;
static const TickType_t DATA_QUEUE_POP_TIMEOUT = portMAX_DELAY;

static StaticTask_t init_task_tcb;
static StackType_t init_task_stack[UDPSERVER_INIT_TASK_STACKSIZE];
static StaticTask_t data_task_tcb;
static StackType_t data_task_stack[UDPSERVER_DATA_TASK_STACKSIZE];
static StaticTask_t io_task_tcb;
static StackType_t io_task_stack[UDPSERVER_IO_TASK_STACKSIZE];

static SemaphoreHandle_t tx_mutex = NULL;
static xQueueHandle data_queue = NULL;

static uint8_t tx_buffer[UP_HEADER_SIZE+UP_MSG_DATA_SIZE];
static up_header * const tx_header = (up_header*) &tx_buffer[0];
static up_msg_data * const tx_msg_data = (up_msg_data*) &tx_buffer[UP_HEADER_SIZE];

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

static void update_link(struct netif * const netif)
{
    const uint8_t link_status = ethernetif_link_status();

    if(link_status != 0)
    {
        if(netif_is_link_up(&gnetif) == 0)
        {
            netif_set_link_up(netif);
        }
    }
    else
    {
        if(netif_is_link_up(&gnetif) != 0)
        {
            netif_set_link_down(netif);
        }
    }
}

static void udpserver_init(void)
{
    if(is_init == false)
    {
        tx_mutex = xSemaphoreCreateMutex();
        data_queue = xQueueCreate(DATA_QUEUE_SIZE_LEN, sizeof(queue_msg));

        memset(&tx_buffer[0], 0, sizeof(tx_buffer));

        tx_header->version_major = UP_VERSION_MAJOR;
        tx_header->version_minor = UP_VERSION_MINOR;
        tx_header->msg_type = UP_MSG_TYPE_DATA;
        tx_header->msg_size = UP_MSG_DATA_SIZE;

        // create tcp_ip stack thread
        tcpip_init(NULL, NULL);

        // inialize the lwip stack
        netif_config();

        notify(&gnetif);

        is_init = true;
    }

    debug_printf(
            "IP %u.%u.%u.%u:%u\r\n",
            IP_ADDR0,
            IP_ADDR1,
            IP_ADDR2,
            IP_ADDR3,
            UDPSERVER_PORT);
}

static void data_task(void *params)
{
    (void) params;
    queue_msg msg;

    debug_puts(UDPSERVER_DATA_TASK_NAME" started");

    while(1)
    {
        if(xQueueReceive(data_queue, &msg, DATA_QUEUE_POP_TIMEOUT) == pdTRUE)
        {
            if(xSemaphoreTake(tx_mutex, TX_MUTEX_DATA_TASK_TIMEOUT) == pdTRUE)
            {
                tx_msg_data->data_items[msg.offset] = msg.data;

                xSemaphoreGive(tx_mutex);
            }
        }
    }
}

static void io_task(void *params)
{
    (void) params;
    TickType_t last_wake_time;
    err_t nc_err;
    struct netbuf *io_buff;

    debug_puts(UDPSERVER_IO_TASK_NAME" started");

    conn = netconn_new(NETCONN_UDP);

    if(conn != NULL)
    {
        nc_err = netconn_bind(conn, IP_ADDR_ANY, (uint16_t) UDPSERVER_PORT);

        if(nc_err != ERR_OK)
        {
            debug_puts("netconn_bind failed");
            netconn_delete(conn);
            conn = NULL;
        }
    }

    if(conn != NULL)
    {
        nc_err = netconn_connect(conn, IP_ADDR_BROADCAST, (uint16_t) UDPSERVER_PORT);

        if(nc_err != ERR_OK)
        {
            debug_puts("netconn_connect failed");
            netconn_delete(conn);
            conn = NULL;
        }
    }

    io_buff = netbuf_new();

    if(xSemaphoreTake(tx_mutex, portMAX_DELAY) == pdTRUE)
    {
        (void) netbuf_ref(
                io_buff,
                &tx_buffer[0],
                tx_header->msg_size+UP_HEADER_SIZE);

        xSemaphoreGive(tx_mutex);
    }

    last_wake_time = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&last_wake_time, UDP_TX_FREQ);

        if(conn != NULL)
        {
            if(xSemaphoreTake(tx_mutex, TX_MUTEX_IO_TASK_TIMEOUT) == pdTRUE)
            {
                nc_err = netconn_send(conn, io_buff);

                xSemaphoreGive(tx_mutex);

                if(nc_err != ERR_OK)
                {
                    debug_puts("netconn_send failed");
                }
            }
        }
    };
}

static void init_task(void *params)
{
    (void) params;
    TickType_t last_wake_time;

    led_off(LED_ETH_STATUS);

    system_wait_for_start();

    debug_puts(UDPSERVER_INIT_TASK_NAME" started");

    udpserver_init();

    (void) xTaskCreateStatic(
            &data_task,
            UDPSERVER_DATA_TASK_NAME,
            UDPSERVER_DATA_TASK_STACKSIZE,
            NULL,
            UDPSERVER_DATA_TASK_PRI,
            &data_task_stack[0],
            &data_task_tcb);

    (void) xTaskCreateStatic(
            &io_task,
            UDPSERVER_IO_TASK_NAME,
            UDPSERVER_IO_TASK_STACKSIZE,
            NULL,
            UDPSERVER_IO_TASK_PRI,
            &io_task_stack[0],
            &io_task_tcb);

    last_wake_time = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&last_wake_time, LINK_STATUS_FREQ);

        update_link(&gnetif);
    }
}

void udpserver_start(void)
{
    (void) xTaskCreateStatic(
            &init_task,
            UDPSERVER_INIT_TASK_NAME,
            UDPSERVER_INIT_TASK_STACKSIZE,
            NULL,
            UDPSERVER_INIT_TASK_PRI,
            &init_task_stack[0],
            &init_task_tcb);
}

bool udpserver_set_item(
        const uint16_t offset,
        const uint8_t * const data,
        const uint8_t size)
{
    bool ret = false;

    if(is_init == true)
    {
        queue_msg msg;

        msg.offset = offset;
        msg.data = 0;

        memcpy(&msg.data, &data, size);

        if(xQueueSendToBack(data_queue, &msg, DATA_QUEUE_PUSH_TIMEOUT) == pdTRUE)
        {
            ret = true;
        }
    }

    return ret;
}

// TODO - make this better
// it's currently part of the STM32 HAL implememted in ethernetif.c
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
