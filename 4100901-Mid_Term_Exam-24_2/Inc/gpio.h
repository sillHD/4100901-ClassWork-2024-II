#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t CFGR1;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CFGR2;
} SYSCFG_t;

typedef struct {
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t PR1;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t PR2;
} EXTI_t;


typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;

} GPIO_t;

// Funciones para configurar GPIOs
void configure_gpio(void);

// Control del LED de estado de la puerta (simula el estado de la puerta)
// state = 0: LED apagado (puerta bloqueada)
// state = 1: LED encendido (puerta desbloqueada)
void gpio_set_door_led_state(uint8_t state);

// LED de latido (heartbeat)
void gpio_toggle_heartbeat_led(void);

// Lectura de botones (simple/doble pulsación se maneja en otra capa)
uint8_t button_driver_get_event(void);


void configure_gpio_for_usart(void);

#endif // GPIO_H

