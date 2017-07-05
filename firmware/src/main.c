/**
 * @file main.c
 * @brief Main.
 *
 */


#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"


void hw_init(void)
{
    GPIO_InitTypeDef gpio_led;

    GPIO_StructInit(&gpio_led);
    gpio_led.GPIO_Mode = GPIO_Mode_OUT;
    gpio_led.GPIO_Pin = GPIO_Pin_12;
    gpio_led.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_led.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_led.GPIO_OType = GPIO_OType_PP;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_Init(GPIOD, &gpio_led);

    debug_puts("hw_int\n");
}


void led_task(void *params)
{
    debug_puts("led_task\n");

    while(1)
    {
        GPIO_ToggleBits(GPIOD, GPIO_Pin_12);

        vTaskDelay(500 / portTICK_RATE_MS);
    }
}


int main(void)
{
    debug_init();

    hw_init();

    (void) SysTick_Config(SystemCoreClock / 1000);

    (void) xTaskCreate(
            led_task,
            "led_task",
            configMINIMAL_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + 2UL,
            NULL);

    vTaskStartScheduler();

    while(1);

    return 0;
}
