/**
 * @file led.h
 * @brief TODO.
 *
 */


#ifndef LED_H
#define LED_H


#include <stdbool.h>


#define LED_GPIO_PERIF RCC_AHB1Periph_GPIOD
#define LED_GPIO_PORT GPIOD

#define LED_PIN_GREEN GPIO_Pin_12
#define LED_PIN_ORANGE GPIO_Pin_13
#define LED_PIN_RED GPIO_Pin_14
#define LED_PIN_BLUE GPIO_Pin_15


#define LED_COUNT (4)


typedef enum
{
    LED_GREEN = 0,
    LED_ORANGE,
    LED_RED,
    LED_BLUE
} led_kind;


void led_init(void);


void led_set(
        const led_kind led,
        const bool state);


void led_toggle(
        const led_kind led);


void led_set_all(
        const bool state);


#endif  /* LED_H */
