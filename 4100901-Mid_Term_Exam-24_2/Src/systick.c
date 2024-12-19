#include "systick.h"

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

} SysTick_t;


#define SysTick ((SysTick_t *)0xE000E010) // Base address of SysTick


volatile uint32_t ms_counter = 0; // Counter for milliseconds


void configure_systick_and_start(void)
{
    SysTick->CTRL = 0x4;     // Disable SysTick for configuration, use processor clock
    SysTick->LOAD = 3999;    // Reload value for 1 ms (assuming 4 MHz clock)
    SysTick->CTRL = 0x7;     // Enable SysTick, processor clock, no interrupt
}

uint32_t systick_GetTick(void)
{
    return ms_counter;
}

void systick_reset(void) // incializa la variable ms_counter
{
    ms_counter = 0;
}

void SysTick_Handler(void) // actua como el cronometro sumando un 1 a la variable ms_counter
{
    ms_counter++;
}
