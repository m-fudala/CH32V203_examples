#include "uart_lib.h"

UART uart;

void uart_init() {
    uart.tx_bytes_sent = 0;

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
    uart.tx_bytes_to_send = bytes_len;
    uart.tx_bytes = bytes;

    USART1->CTLR1 |= USART_CTLR1_TXEIE;    // enable TX buffer empty interrupt
}

void USART1_IRQHandler() {
    if (USART1->STATR & USART_STATR_TXE) {
        // put a byte into data register
        USART1->DATAR = uart.tx_bytes[uart.tx_bytes_sent];

        ++uart.tx_bytes_sent;

        if (uart.tx_bytes_sent == uart.tx_bytes_to_send) {
            USART1->CTLR1 &= ~USART_CTLR1_TXEIE;    // disable TX interrupt
                                                    // when all bytes are sent
            uart.tx_bytes_sent = 0;
        }
    }
}