/*
    Configuration for USART1 on CH32V203 

    https://github.com/m-fudala
*/

#ifndef UART_LIB_H_
#define UART_LIB_H_

#include "ch32v20x.h"

typedef struct UART {
    volatile unsigned char tx_bytes_sent;
    volatile unsigned char tx_bytes_to_send;
    volatile char *tx_bytes;
} UART;

void uart_init();
void uart_send(char *bytes, unsigned char bytes_len);

void USART1_IRQHandler() __attribute__((interrupt()));

#endif
