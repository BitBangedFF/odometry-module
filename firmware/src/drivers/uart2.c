/**
 * @file uart2.c
 * @brief TODO.
 *
 */


#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_conf.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "nvicconf.h"
#include "debug.h"
#include "uart2.h"


static xQueueHandle rx_queue;
static bool is_init = false;


static void __attribute__((used)) USART2_IRQHandler(void)
{
    uint8_t data;
    portBASE_TYPE higher_priority_task_woken = pdFALSE;

    if(USART_GetITStatus(UART2_TYPE, USART_IT_RXNE) != 0)
    {
        data = USART_ReceiveData(UART2_TYPE) & 0x00FF;
        (void) xQueueSendFromISR(
                rx_queue,
                &data,
                &higher_priority_task_woken);
    }
}


void uart2_init(
        const uint32_t baudrate)
{
    if(is_init == false)
    {
        USART_InitTypeDef usart_init;
        GPIO_InitTypeDef gpio_init;
        NVIC_InitTypeDef nvic_init;

        // enable GPIO and USART clock
        RCC_AHB1PeriphClockCmd(UART2_GPIO_PERIF, ENABLE);
        UART2_RCC_ENABLE(UART2_PERIF, ENABLE);

        // configure USART Rx as input floating
        GPIO_StructInit(&gpio_init);
        gpio_init.GPIO_Pin = UART2_GPIO_RX_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(UART2_GPIO_PORT, &gpio_init);

        // configure USART Tx as alternate function
        GPIO_StructInit(&gpio_init);
        gpio_init.GPIO_Pin = UART2_GPIO_TX_PIN;
        gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        GPIO_Init(UART2_GPIO_PORT, &gpio_init);

        // map to alternate functions
        GPIO_PinAFConfig(UART2_GPIO_PORT, UART2_GPIO_AF_TX_PIN, UART2_GPIO_AF_TX);
        GPIO_PinAFConfig(UART2_GPIO_PORT, UART2_GPIO_AF_RX_PIN, UART2_GPIO_AF_RX);

        USART_StructInit(&usart_init);
        usart_init.USART_BaudRate = baudrate;
        usart_init.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
        usart_init.USART_WordLength = USART_WordLength_8b;
        usart_init.USART_StopBits = USART_StopBits_1;
        usart_init.USART_Parity = USART_Parity_No ;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(UART2_TYPE, &usart_init);

        nvic_init.NVIC_IRQChannel = UART2_IRQ;
        nvic_init.NVIC_IRQChannelPreemptionPriority = NVIC_MID_PRI;
        nvic_init.NVIC_IRQChannelSubPriority = 0;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);

        rx_queue = xQueueCreate(UART2_RX_QUEUE_SIZE, sizeof(uint8_t));

        USART_ITConfig(UART2_TYPE, USART_IT_RXNE, ENABLE);

        USART_Cmd(UART2_TYPE, ENABLE);

        USART_ITConfig(UART2_TYPE, USART_IT_RXNE, ENABLE);

        is_init = true;
    }

    debug_puts("uart2_init\n");
}


bool uart2_is_init(void)
{
    return is_init;
}


bool uart2_get_char(
        uint8_t * const data)
{
    bool ret = false;

    if(is_init == true)
    {
        (void) xQueueReceive(rx_queue, data, portMAX_DELAY);
        ret = true;
    }

    return ret;
}


bool uart2_get_char_timeout(
        uint8_t * const data)
{
    bool ret = false;

    if(is_init == true)
    {
        if(xQueueReceive(rx_queue, data, UART2_RX_TIMEOUT_TICKS) == pdTRUE)
        {
            ret = true;
        }
    }

    if(ret == false)
    {
        *data = 0;
    }

    return ret;
}


void uart2_put_char(
        const uint8_t data)
{
    uart2_send(&data, 1);
}


void uart2_send(
        const uint8_t * const data,
        const uint32_t size)
{
    if(is_init == true)
    {
        uint32_t i;
        for(i = 0; i < size; i += 1)
        {
            while((UART2_TYPE->SR & USART_FLAG_TXE) == 0);

            UART2_TYPE->DR = (data[i] & 0x00FF);
        }
    }
}
