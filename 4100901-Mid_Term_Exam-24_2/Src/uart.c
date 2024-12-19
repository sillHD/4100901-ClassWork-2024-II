#include "uart.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"

static volatile command_t last_command = CMD_NONE;

void UART_Init (USART_TypeDef * UARTx) {
    UART_clock_enable(UARTx);
    // Disable USART
    UARTx->CR1 &= ~USART_CR1_UE;

    // Set data length to 8 bits (clear M bit)
    UARTx->CR1 &= ~USART_CR1_M;

    // Select 1 stop bit (clear STOP bits in CR2)
    UARTx->CR2 &= ~USART_CR2_STOP;

    // Set parity control as no parity (clear PCE bit)
    UARTx->CR1 &= ~USART_CR1_PCE;

    // Oversampling by 16 (clear OVER8 bit)
    UARTx->CR1 &= ~USART_CR1_OVER8;

    // Set Baud rate to 9600 using APB frequency (4 MHz)
    UARTx->BRR = BAUD_9600_4MHZ;

    // Enable transmission and reception
    UARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // Enable USART
    UARTx->CR1 |= USART_CR1_UE;

    // Verify that USART is ready for transmission
    while ((UARTx->ISR & USART_ISR_TEACK) == 0);

    // Verify that USART is ready for reception
    while ((UARTx->ISR & USART_ISR_REACK) == 0);

    UART_receive_it(UARTx);
}

void usart2_init(void)
{
    configure_gpio_for_usart();

    *RCC_APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    // TODO: Configurar UART2

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

