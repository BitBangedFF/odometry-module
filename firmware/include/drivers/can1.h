/**
 * @file can1.h
 * @brief TODO.
 *
 */


#ifndef CAN1_H
#define CAN1_H


#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_can.h"


#define CAN1_RX_TIMEOUT_MS (50)
#define CAN1_RX_TIMEOUT_TICKS (CAN1_RX_TIMEOUT_MS / portTICK_RATE_MS)
#define CAN1_RX_QUEUE_SIZE (16)

#define CAN1_CHANNEL CAN1
#define CAN1_FILTER_NUM (0)
#define CAN1_PERIF RCC_APB1Periph_CAN1
#define CAN1_RCC_ENABLE RCC_APB1PeriphClockCmd
#define CAN1_RX_IRQ CAN1_RX0_IRQn

#define CAN1_GPIO_PERIF RCC_AHB1Periph_GPIOD
#define CAN1_GPIO_PORT GPIOD
#define CAN1_GPIO_RX_PIN GPIO_Pin_0
#define CAN1_GPIO_TX_PIN GPIO_Pin_1
#define CAN1_GPIO_AF_RX_PIN GPIO_PinSource0
#define CAN1_GPIO_AF_TX_PIN GPIO_PinSource1
#define CAN1_GPIO_AF_RX GPIO_AF_CAN1
#define CAN1_GPIO_AF_TX GPIO_AF_CAN1


void can1_init(void);


bool can1_is_init(void);


bool can1_read(
        CanRxMsg * const msg);


bool can1_read_timeout(
        CanRxMsg * const msg);


bool can1_write(
        CanTxMsg * const msg);


#endif  /* CAN1_H */
