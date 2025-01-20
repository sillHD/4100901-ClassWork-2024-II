#include "uart.h"

void usart2_init(void)
{
    configure_gpio_for_usart(); // Inicializar los pines para el USART2
    *RCC_APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Habilitar reloj para USART2

    USART2->CR1 &= ~USART_CR1_UE; // Deshabilitar USART
    USART2->BRR = BAUD_9600_4MHZ; // Configurar baudrate a 9600
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Habilitar transmisor y receptor
    USART2->CR1 |= USART_CR1_UE; // Habilitar USART

    // Activar interrupción de RXNE
    USART2->CR1 |= USART_CR1_RXNEIE; 
    NVIC->ISER[1] |= (1 << 6);
}

void usart2_send_string(const char *str)
{
    while (*str) {
        while (!(USART2->ISR & USART_ISR_TXE));
        USART2->TDR = *str++;
    }
}

command_t usart2_get_command(void)
{
    command_t cmd = last_command;
    last_command = CMD_NONE;
    return cmd;
}


void USART2_IRQHandler(void)
{
    uint32_t isr = USART2->ISR;
    if (isr & USART_ISR_RXNE) {
        char command = USART2->RDR;
        if (command == 'O') {
            last_command = CMD_OPEN;
        } else if (command == 'C') {
            last_command = CMD_CLOSE;
        }
    }
}

