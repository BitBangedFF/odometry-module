/**
 * @file led.c
 * @brief TODO.
 *
 */


#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_conf.h"

#include "debug.h"
#include "led.h"


static bool is_init = false;


static const uint32_t LED_PINS[] =
{
    [LED_GREEN] = LED_PIN_GREEN,
    [LED_ORANGE] = LED_PIN_ORANGE,
    [LED_RED] = LED_PIN_RED,
    [LED_BLUE] = LED_PIN_BLUE
};


void led_init(void)
{
    if(is_init == false)
    {
        GPIO_InitTypeDef gpio_leds;

        RCC_AHB1PeriphClockCmd(LED_GPIO_PERIF, ENABLE);

        GPIO_StructInit(&gpio_leds);
        gpio_leds.GPIO_Mode = GPIO_Mode_OUT;
        gpio_leds.GPIO_Pin = (LED_PIN_GREEN | LED_PIN_ORANGE | LED_PIN_RED | LED_PIN_BLUE);
        gpio_leds.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio_leds.GPIO_Speed = GPIO_Speed_25MHz;
        gpio_leds.GPIO_OType = GPIO_OType_PP;

        GPIO_Init(GPIOD, &gpio_leds);

        led_set_all(false);

        is_init = true;
    }

    debug_puts("led_init\n");
}


bool led_is_init(void)
{
    return is_init;
}


void led_set(
        const led_kind led,
        const bool state)
{
    if(state == true)
    {
        GPIO_SetBits(LED_GPIO_PORT, LED_PINS[led]);
    }
    else
    {
        GPIO_ResetBits(LED_GPIO_PORT, LED_PINS[led]);
    }
}


void led_toggle(
        const led_kind led)
{
    GPIO_ToggleBits(LED_GPIO_PORT, LED_PINS[led]);
}


void led_set_all(
        const bool state)
{
    uint32_t i;
    for(i = 0; i < LED_COUNT; i += 1)
    {
        led_set((led_kind) i, state);
    }
}
