// Access Control System Implementation

#include "gpio.h"
#include "systick.h"
#include "uart.h"

#define TEMP_UNLOCK_DURATION 5000 // Duration in ms for temporary unlock


typedef enum {
    LOCKED,
    TEMP_UNLOCK,
    PERM_UNLOCK
} DoorState_t;

DoorState_t current_state = LOCKED;
uint32_t unlock_timer = 0;

void run_state_machine(void) {  // evalua si la puerta debe estar bloqueada o desbloqueada mediante la varaible current_state
    switch (current_state) {
        case LOCKED:
            // No periodic action in locked state
            break;
        case TEMP_UNLOCK:
            if (systick_GetTick() - unlock_timer >= TEMP_UNLOCK_DURATION) {
                gpio_set_door_led_state(0); // Turn off door state LED
                current_state = LOCKED;
            }
            break;
        case PERM_UNLOCK:
            // No periodic action in permanent unlock state
            break;
    }
}

void handle_event(uint8_t event) { //mediante esta funcion se determian si el boton se presiono una vez o mas de una vez para asi definir el valor de la variable state y saber a que estado del switch pertenece
    if (event == 1) { // Single button press
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = TEMP_UNLOCK;
        unlock_timer = systick_GetTick();
    } else if (event == 2) { // Double button press
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = PERM_UNLOCK;
    } else if (event == 'O') { // UART OPEN command
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = TEMP_UNLOCK;
        unlock_timer = systick_GetTick();
    } else if (event == 'C') { // UART CLOSE command
        gpio_set_door_led_state(0); // Turn off door state LED
        current_state = LOCKED;
    }
}

int main(void) {
    configure_systick_and_start(); // inicializa el systick para poder usarlo en los contadoes
    configure_gpio(); // configura el GPIOA y GPIOC como salida o entrada y le asigna los pines, ademas define las funciones que vamos a usar mas adelante
    usart2_init();
    usart2_send_string("System Initialized\r\n");
    systick_reset(); // establece el el ms_counter en cero para que empiece a contar una vez entrado en el ciclo

    uint32_t heartbeat_tick = 0; 

    while (1) { // se inicializa la maquina

        if (systick_GetTick() - heartbeat_tick >= 500) { // si el ms counter alcanza los 500ms activa o desactiva el led y guarda el valor del ms_counter en la variable heartbeat
            heartbeat_tick = systick_GetTick();
            gpio_toggle_heartbeat_led();
            systick_reset();

        }

        uint8_t button_pressed = button_driver_get_event();
        if (button_pressed != 0) {
            handle_event(button_pressed);
            button_pressed = 0;
        }

        uint8_t rx_byte = usart2_get_command();
        if (rx_byte != 0) {
            handle_event(rx_byte);
            rx_byte = 0;
        }

        run_state_machine();
        SysTick_Handler(); // se actualiza el valor de ms_counter
    }
}
