#include <stdint.h>

#define STK_BASE 0xE000E010
#define STK_CTRL (*(volatile uint32_t *)(STK_BASE + 0x00))
#define STK_LOAD (*(volatile uint32_t *)(STK_BASE + 0x04))

#define RCC_BASE 0x40021000
#define RCC_AHB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x4C))

#define GPIOA_BASE 0x48000000
#define GPIOA_MODER (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define LED_PIN 6 // LED en PA6
#define LED2_PIN 5 // Otro LED en PA5

#define GPIOC_BASE 0x48000800
#define GPIOC_MODER (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_IDR (*(volatile uint32_t *)(GPIOC_BASE + 0x10))
#define BUTTON_PIN 13 // Botón en PC13

volatile uint32_t ms_counter = 0;
int control = 1; // Bandera para controlar el parpadeo del LED

void init_ms_counter(void) {
    ms_counter = 0;
}

void configure_systick_and_start(void) {
    STK_CTRL = 0x4; // Desactivar SysTick para configuración, usar reloj del procesador
    STK_LOAD = 3999; // Valor de recarga para 1 ms (suponiendo reloj de 4 MHz)
    STK_CTRL = 0x5; // Habilitar SysTick, reloj del procesador, sin interrupción
}

void configure_gpio(void) {
    RCC_AHB2ENR |= (1 << 0) | (1 << 2); // Habilitar reloj para GPIOA y GPIOC

    // Configurar GPIOA pin 5 como salida (01)
    GPIOA_MODER &= ~(0x3 << (LED_PIN * 2));
    GPIOA_MODER |= (0x1 << (LED_PIN * 2));

    // Configurar GPIOA pin 6 como salida (01) para el segundo LED
    GPIOA_MODER &= ~(0x3 << (LED2_PIN * 2));
    GPIOA_MODER |= (0x1 << (LED2_PIN * 2));

    // Configurar GPIOC pin 13 como entrada
    GPIOC_MODER &= ~(0x3 << (BUTTON_PIN * 2));
}

void toggle_led(void) {
    if (ms_counter >= 1000) { // Parpadea cada 500 ms
        GPIOA_ODR ^= (1 << LED_PIN); // Cambiar el estado del LED
        ms_counter = 0;
    }
}

int main(void) {
    init_ms_counter();
    configure_systick_and_start();
    configure_gpio();

    GPIOA_ODR &= ~(1 << LED_PIN); // Cambiar el estado del LED

    while (1) {
        if (!(GPIOC_IDR & (1 << BUTTON_PIN))) { // Botón presionado (activo bajo
            while (1){
                if ((GPIOC_IDR & (1 << BUTTON_PIN))){
                    break;
                }
            }
            control = control*(-1);
        }

        if(control == -1){
            toggle_led(); // Actualizar el estado del LED
            ms_counter++; // Incrementar contador de ms
        }
        else {
            ms_counter = 0;
        }
    }
    return 0; // Nunca debería llegar aquí
}
