/**
 * @file platform.c
 * @brief TODO.
 *
 */

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#include "debug.h"
#include "led.h"
#include "platform.h"

static void cpu_cache_enable(void)
{
    SCB_EnableICache();

    SCB_EnableDCache();
}

static void system_clock_config(void)
{
    RCC_ClkInitTypeDef clk_init;
    RCC_OscInitTypeDef osc_init;

    // enable HSE oscillator and activate PLL with HSE as source
    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_init.HSEState = RCC_HSE_BYPASS;
    osc_init.HSIState = RCC_HSI_OFF;
    osc_init.PLL.PLLState = RCC_PLL_ON;
    osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc_init.PLL.PLLM = 8;
    osc_init.PLL.PLLN = 432;
    osc_init.PLL.PLLP = RCC_PLLP_DIV2;
    osc_init.PLL.PLLQ = 9;
    osc_init.PLL.PLLR = 7;

    if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
    {
        platform_error_handler();
    }

    // activate the OverDrive to reach the 216 MHz frequency
    if(HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        platform_error_handler();
    }

    // select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    // clocks dividers
    clk_init.ClockType =
            (RCC_CLOCKTYPE_SYSCLK |
             RCC_CLOCKTYPE_HCLK |
             RCC_CLOCKTYPE_PCLK1 |
             RCC_CLOCKTYPE_PCLK2);
    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
    clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    if(HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_7) != HAL_OK)
    {
        platform_error_handler();
    }
}

void platform_init(void)
{
    cpu_cache_enable();

    HAL_Init();

    system_clock_config();

    led_init();

    debug_init();
}

void platform_error_handler(void)
{
    while(1)
    {
        led_set_all(true);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    platform_error_handler();
}
#endif

#ifdef BUILD_TYPE_DEBUG
int __io_putchar(int ch)
{
    // TODO

    return ch;
}

int _write(int file, char *ptr, int len)
{
    int idx;

    for(idx = 0; idx < len; idx += 1)
    {
        __io_putchar(*ptr++);
    }

    return len;
}
#endif
