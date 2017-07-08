/**
 * @file uart2.h
 * @brief TODO.
 *
 */


#ifndef UART2_H
#define UART2_H


#include <stdint.h>
#include <stdbool.h>


#define UART2_BAUDRATE (115200)
#define UART2_RX_TIMEOUT_MS (50)
#define UART2_RX_TIMEOUT_TICKS (UART2_RX_TIMEOUT_MS / portTICK_RATE_MS)
#define UART2_RX_QUEUE_SIZE (64)

#define UART2_TYPE USART2
#define UART2_PERIF RCC_APB1Periph_USART2
#define UART2_RCC_ENABLE RCC_APB1PeriphClockCmd
#define UART2_IRQ USART2_IRQn

#define UART2_GPIO_PERIF RCC_AHB1Periph_GPIOD
#define UART2_GPIO_PORT GPIOD
#define UART2_GPIO_RX_PIN GPIO_Pin_6
#define UART2_GPIO_TX_PIN GPIO_Pin_5
#define UART2_GPIO_AF_RX_PIN GPIO_PinSource6
#define UART2_GPIO_AF_TX_PIN GPIO_PinSource5
#define UART2_GPIO_AF_RX GPIO_AF_USART2
#define UART2_GPIO_AF_TX GPIO_AF_USART2


void uart2_init(
        const uint32_t baudrate);


bool uart2_is_init(void);


bool uart2_get_char(
        uint8_t * const data);


bool uart2_get_char_timeout(
        uint8_t * const data);


void uart2_put_char(
        const uint8_t data);


void uart2_send(
        const uint8_t * const data,
        const uint32_t size);


#endif  /* UART2_H */
