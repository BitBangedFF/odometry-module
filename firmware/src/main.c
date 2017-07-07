/**
 * @file main.c
 * @brief Main.
 *
 */


#include "stm32f4xx_conf.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"
#include "led.h"
#include "uart1.h"


void hw_init(void)
{
    led_init();
    uart1_init(UART1_BAUDRATE);
}


void led_task(void *params)
{
    debug_puts("led_task\n");
    
    led_set_all(true);
    led_set(LED_ORANGE, false);

    while(1)
    {
        led_toggle(LED_BLUE);
        led_toggle(LED_ORANGE);

        vTaskDelay(M2T(500));
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
