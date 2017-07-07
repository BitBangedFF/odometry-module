/**
 * @file uart1.h
 * @brief TODO.
 *
 */


#ifndef UART1_H
#define UART1_H


#include <stdint.h>
#include <stdbool.h>


#define UART1_BAUDRATE (115200)
#define UART1_RX_TIMEOUT_MS (50)
#define UART1_RX_TIMEOUT_TICKS (UART1_RX_TIMEOUT_MS / portTICK_RATE_MS)
#define UART1_RX_QUEUE_SIZE (64)

#define UART1_TYPE USART3
#define UART1_PERIF RCC_APB1Periph_USART3
#define UART1_RCC_ENABLE RCC_APB1PeriphClockCmd
#define UART1_IRQ USART3_IRQn

#define UART1_GPIO_PERIF RCC_AHB1Periph_GPIOB
#define UART1_GPIO_PORT GPIOB
#define UART1_GPIO_RX_PIN GPIO_Pin_11
#define UART1_GPIO_TX_PIN GPIO_Pin_10
#define UART1_GPIO_AF_RX_PIN GPIO_PinSource11
#define UART1_GPIO_AF_TX_PIN GPIO_PinSource10
#define UART1_GPIO_AF_RX GPIO_AF_USART3
#define UART1_GPIO_AF_TX GPIO_AF_USART3


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
