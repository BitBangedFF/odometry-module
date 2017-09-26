/**
 * @file led.h
 * @brief TODO.
 *
 */

#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include "stm32f7xx_hal_gpio.h"

#define LED_GPIO_PORT GPIOB

#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED_PIN_GREEN GPIO_PIN_0
#define LED_PIN_BLUE GPIO_PIN_7
#define LED_PIN_RED GPIO_PIN_14

#define LED_UART1_STATUS LED_GREEN
#define LED_CAN1_STATUS LED_BLUE
#define LED_SYSTEM_STATUS LED_RED
#define LED_COUNT (3)

typedef enum
{
    LED_GREEN = 0,
    LED_BLUE,
    LED_RED
} led_kind;

void led_init(void);

bool led_is_init(void);

void led_set(
        const led_kind led,
        const bool state);

void led_toggle(
        const led_kind led);

void led_set_all(
        const bool state);

#endif  /* LED_H */
