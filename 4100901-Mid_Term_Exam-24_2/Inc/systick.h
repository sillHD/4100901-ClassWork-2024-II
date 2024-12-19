
#include <stdint.h>

void configure_systick_and_start(void);
uint32_t systick_GetTick(void);
void systick_reset(void);
void SysTick_Handler(void);
