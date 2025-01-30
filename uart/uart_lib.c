#include "uart_lib.h"

UART uart;

void uart_init(void (*function)(char *, char)) {
    uart.tx_bytes_sent = 0;
    uart.rx_bytes_read = 0;
    idle_function_pointer = function;

    RCC->APB2PCENR |= RCC_USART1EN | RCC_IOPAEN;   // enable USART1 clock,
                                                    // enable port A clock

    GPIOA->CFGHR |= 0x4A0; // PA10: floating input, PA9: 2 MHz alternate
                            // push-pull output

    USART1->BRR = 12 << 4;  // DIV = 12 -> DIV_Fraction = 0,
                            // DIV_Mantissa = 12 -> 250000 baud @ 48MHz

    // enable RX, TX
    USART1->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE;

    PFIC->IENR[1] |= 1 << 21;    // enable USART1 global interrupt (53)
    USART1->CTLR1 |= USART_CTLR1_RXNEIE;    // enable RX non-empty interrupt
}

void uart_send(char *bytes, unsigned char bytes_len) {
    uart.tx_bytes_sent = 0;

    uart.tx_bytes_to_send = bytes_len;
    uart.tx_bytes = bytes;

    USART1->CTLR1 |= USART_CTLR1_TXEIE;    // enable TX buffer empty interrupt
}

void USART1_IRQHandler() {
    // RX interrupt
    if (USART1->STATR & USART_STATR_RXNE) {
        uart.rx_buffer[uart.rx_bytes_read] = USART1->DATAR;

        ++uart.rx_bytes_read;

        USART1->STATR |= USART_STATR_RXNE;  // clear RXNE flag
        USART1->CTLR1 |= USART_CTLR1_IDLEIE;    // enable idle line interrupt
    // idle line (RX done) interrupt
    } else if (USART1->STATR & USART_STATR_IDLE) {
        // invoke function on the end of the message
        idle_function_pointer((char *)uart.rx_buffer, uart.rx_bytes_read);

        uart.rx_bytes_read = 0;

        USART1->DATAR;  // read data register to clear the interrupt
        USART1->CTLR1 &= ~USART_CTLR1_IDLEIE;   // disable idle line interrupt
    // TX interrupt
    // cannot seem to clear TXE flag, so TX has to be the last else if
    } else if (USART1->STATR & USART_STATR_TXE) {
        // put a byte into data register
        USART1->DATAR = *(uart.tx_bytes + uart.tx_bytes_sent);

        ++uart.tx_bytes_sent;

        if (uart.tx_bytes_sent == uart.tx_bytes_to_send) {  
            USART1->CTLR1 &= ~USART_CTLR1_TXEIE;    // disable TX interrupt
                                                    // when all bytes are sent
        }
    }
}