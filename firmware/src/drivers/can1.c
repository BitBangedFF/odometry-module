/**
 * @file can1.c
 * @brief TODO.
 *
 */


#include <stdint.h>
#include <stdbool.h>

#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "nvicconf.h"
#include "debug.h"
#include "led.h"
#include "can1.h"


static xQueueHandle rx_queue;
static bool is_init = false;


static void __attribute__((used)) CAN1_RX0_IRQHandler(void)
{
    portBASE_TYPE higher_priority_task_woken = pdFALSE;

    if(CAN_GetITStatus(CAN1_CHANNEL, CAN_IT_FMP0) != 0)
    {
        led_toggle(LED_CAN1_STATUS);

        CanRxMsg rx_data;
        CAN_Receive(CAN1_CHANNEL, CAN_FIFO0, &rx_data);

        (void) xQueueSendFromISR(
                rx_queue,
                &rx_data,
                &higher_priority_task_woken);
    }
}


void can1_init(void)
{
    if(is_init == false)
    {
        CAN_InitTypeDef can_init;
        CAN_FilterInitTypeDef can_filter;
        GPIO_InitTypeDef gpio_init;
        NVIC_InitTypeDef nvic_init;

        RCC_AHB1PeriphClockCmd(CAN1_GPIO_PERIF, ENABLE);
        CAN1_RCC_ENABLE(CAN1_PERIF, ENABLE);

        GPIO_StructInit(&gpio_init);
        gpio_init.GPIO_Pin = (CAN1_GPIO_RX_PIN | CAN1_GPIO_TX_PIN);
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(CAN1_GPIO_PORT, &gpio_init);

        CAN_DeInit(CAN1_CHANNEL);

        CAN_StructInit(&can_init);
        can_init.CAN_TTCM = DISABLE;
        can_init.CAN_ABOM = DISABLE;
        can_init.CAN_AWUM = DISABLE;
        can_init.CAN_NART = DISABLE;
        can_init.CAN_RFLM = DISABLE;
        can_init.CAN_TXFP = DISABLE;
        can_init.CAN_Mode = CAN_Mode_Normal;
        can_init.CAN_SJW = CAN_SJW_1tq;

        // 1 MBps
        can_init.CAN_BS1 = CAN_BS1_6tq;
        can_init.CAN_BS2 = CAN_BS2_8tq;
        can_init.CAN_Prescaler = 2;
        (void) CAN_Init(CAN1_CHANNEL, &can_init);

        can_filter.CAN_FilterNumber = CAN1_FILTER_NUM;
        can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;
        can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
        can_filter.CAN_FilterIdHigh = 0x0000;
        can_filter.CAN_FilterIdLow = 0x0000;
        can_filter.CAN_FilterMaskIdHigh = 0x0000;
        can_filter.CAN_FilterMaskIdLow = 0x0000;
        can_filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        can_filter.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&can_filter);

        nvic_init.NVIC_IRQChannel = CAN1_RX_IRQ;
        nvic_init.NVIC_IRQChannelPreemptionPriority = NVIC_MID_PRI;
        nvic_init.NVIC_IRQChannelSubPriority = 0;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);

        rx_queue = xQueueCreate(CAN1_RX_QUEUE_SIZE, sizeof(CanRxMsg));

        // enable FIFO 0 message pending interrupt
        CAN_ITConfig(CAN1_CHANNEL, CAN_IT_FMP0, ENABLE);

        is_init = true;
    }

    debug_puts("can1_init\n");
}


bool can1_is_init(void)
{
    return is_init;
}


bool can1_read(
        CanRxMsg * const msg)
{
    bool ret = false;

    if(is_init == true)
    {
        (void) xQueueReceive(rx_queue, msg, portMAX_DELAY);
        ret = true;
    }

    return ret;
}


bool can1_read_timeout(
        CanRxMsg * const msg)
{
    bool ret = false;

    if(is_init == true)
    {
        if(xQueueReceive(rx_queue, msg, CAN1_RX_TIMEOUT_TICKS) == pdTRUE)
        {
            ret = true;
        }
    }

    if(ret == false)
    {
        msg->StdId = 0x00;
        msg->ExtId = 0x00;
        msg->DLC = 0;
    }

    return ret;
}


bool can1_write(
        CanTxMsg * const msg)
{
    bool ret = false;

    if(is_init == true)
    {
        if(CAN_Transmit(CAN1_CHANNEL, msg) == CAN_TxStatus_Ok)
        {
            ret = true;
        }
    }

    return ret;
}
