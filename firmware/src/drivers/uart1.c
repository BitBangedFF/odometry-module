/**
 * @file uart1.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "nvicconf.h"
#include "debug.h"
#include "led.h"
#include "uart1.h"

static xQueueHandle rx_queue;
static bool is_init = false;

void __attribute__((used)) UART1_IRQ_HANDLER(void)
{
    portBASE_TYPE higher_priority_task_woken = pdFALSE;

    if(LL_USART_IsActiveFlag_RXNE(UART1_TYPE) != 0)
    {
        if(LL_USART_IsEnabledIT_RXNE(UART1_TYPE) != 0)
        {
            led_toggle(LED_UART1_STATUS);
            
            const uint8_t data = LL_USART_ReceiveData8(UART1_TYPE);

            (void) xQueueSendFromISR(
                    rx_queue,
                    &data,
                    &higher_priority_task_woken);
        }
    }
}

void uart1_init(
        const uint32_t baudrate)
{
    if(is_init == false)
    {
        LL_USART_InitTypeDef usart_init;
        GPIO_InitTypeDef gpio_init;
        RCC_PeriphCLKInitTypeDef pclk_init;

        LL_USART_DeInit(UART1_TYPE);
        LL_USART_Disable(UART1_TYPE);
        LL_USART_DisableIT_RXNE(UART1_TYPE);
        LL_USART_DisableIT_ERROR(UART1_TYPE);

        memset(&usart_init, 0, sizeof(usart_init));
        memset(&gpio_init, 0, sizeof(gpio_init));
        memset(&pclk_init, 0, sizeof(pclk_init));

        rx_queue = xQueueCreate(UART1_RX_QUEUE_SIZE, sizeof(uint8_t));

        LL_USART_StructInit(&usart_init);

        UART1_RX_GPIO_CLK_ENABLE();
        UART1_TX_GPIO_CLK_ENABLE();

        pclk_init.PeriphClockSelection = RCC_PERIPHCLK_USART3;
        pclk_init.Usart3ClockSelection = RCC_USART3CLKSOURCE_SYSCLK;
        HAL_RCCEx_PeriphCLKConfig(&pclk_init);

        UART1_CLK_ENABLE();

        gpio_init.Pin = UART1_GPIO_RX_PIN;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init.Alternate = UART1_GPIO_AF_RX;

        HAL_GPIO_Init(UART1_GPIO_PORT, &gpio_init);

        gpio_init.Pin = UART1_GPIO_TX_PIN;
        gpio_init.Alternate = UART1_GPIO_AF_TX;

        HAL_GPIO_Init(UART1_GPIO_PORT, &gpio_init);

        NVIC_SetPriority(UART1_IRQ, NVIC_MID_PRI);
        NVIC_EnableIRQ(UART1_IRQ);

        usart_init.BaudRate = baudrate;
        usart_init.DataWidth = LL_USART_DATAWIDTH_8B;
        usart_init.StopBits = LL_USART_STOPBITS_1;
        usart_init.Parity = LL_USART_PARITY_NONE;
        usart_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
        usart_init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        usart_init.OverSampling = LL_USART_OVERSAMPLING_16;

        LL_USART_Init(UART1_TYPE, &usart_init);

        LL_USART_Enable(UART1_TYPE);

        LL_USART_ClearFlag_ORE(UART1_TYPE);

        LL_USART_EnableIT_RXNE(UART1_TYPE);
        LL_USART_EnableIT_ERROR(UART1_TYPE);

        is_init = true;
    }

    debug_puts("uart1_init\n");
}

bool uart1_is_init(void)
{
    return is_init;
}

bool uart1_get_char(
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

bool uart1_get_char_timeout(
        uint8_t * const data)
{
    bool ret = false;

    if(is_init == true)
    {
        if(xQueueReceive(rx_queue, data, UART1_RX_TIMEOUT_TICKS) == pdTRUE)
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

void uart1_put_char(
        const uint8_t data)
{
    uart1_send(&data, 1);
}

void uart1_send(
        const uint8_t * const data,
        const uint32_t size)
{
    if(is_init == true)
    {
        uint32_t i;
        for(i = 0; i < size; i += 1)
        {
            while(LL_USART_IsActiveFlag_TXE(UART1_TYPE) == 0);

            LL_USART_TransmitData8(UART1_TYPE, data[i]);
        }

        while(LL_USART_IsActiveFlag_TC(UART1_TYPE) == 0);
    }
}
