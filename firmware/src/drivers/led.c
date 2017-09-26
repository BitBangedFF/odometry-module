/**
 * @file led.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "debug.h"
#include "led.h"

static bool is_init = false;

static const uint16_t LED_PINS[] =
{
    [LED_GREEN] = LED_PIN_GREEN,
    [LED_BLUE] = LED_PIN_BLUE,
    [LED_RED] = LED_PIN_RED
};

void led_init(void)
{
    if(is_init == false)
    {
        GPIO_InitTypeDef gpio_init;

        LED_GPIO_CLK_ENABLE();

        gpio_init.Pin = (LED_PIN_GREEN | LED_PIN_BLUE | LED_PIN_RED);
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init.Alternate = 0;

        HAL_GPIO_Init(LED_GPIO_PORT, &gpio_init);

        is_init = true;

        led_set_all(false);
    }

    debug_puts("led_init");
}

bool led_is_init(void)
{
    return is_init;
}

void led_set(
        const led_kind led,
        const bool state)
{
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PINS[led], (GPIO_PinState) state);
}

void led_toggle(
        const led_kind led)
{
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PINS[led]);
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
