/**
 * @file uart1.h
 * @brief TODO.
 *
 */

#ifndef UART1_H
#define UART1_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal_gpio.h"

#define UART1_BAUDRATE (115200)
#define UART1_RX_TIMEOUT_MS (50)
#define UART1_RX_TIMEOUT_TICKS (UART1_RX_TIMEOUT_MS / portTICK_RATE_MS)
#define UART1_RX_QUEUE_SIZE (64)

#define UART1_TYPE USART3
#define UART1_IRQ USART3_IRQn
#define UART1_IRQ_HANDLER USART3_IRQHandler
#define UART1_CLK_ENABLE() __HAL_RCC_USART3_CLK_ENABLE();
#define UART1_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART1_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART1_FORCE_RESET() __HAL_RCC_USART3_FORCE_RESET()
#define UART1_RELEASE_RESET() __HAL_RCC_USART3_RELEASE_RESET()

#define UART1_GPIO_PORT GPIOD
#define UART1_GPIO_RX_PIN GPIO_PIN_9
#define UART1_GPIO_TX_PIN GPIO_PIN_8
#define UART1_GPIO_AF_RX GPIO_AF7_USART3
#define UART1_GPIO_AF_TX GPIO_AF7_USART3

void uart1_init(
        const uint32_t baudrate);

bool uart1_is_init(void);

bool uart1_get_char(
        uint8_t * const data);

bool uart1_get_char_timeout(
        uint8_t * const data);

void uart1_put_char(
        const uint8_t data);

void uart1_send(
        const uint8_t * const data,
        const uint32_t size);

#endif  /* UART1_H */
