/*
    Example of how to use UART1 on CH32V203 microcontroller
    https://github.com/m-fudala

*/
#include "ch32v20x.h"
#include "uart_lib.h"

void clock_init(void);

void uart_parse(char *buffer, char buffer_length);

// react to incoming bytes here
void uart_parse(char *buffer, char buffer_length) {
    // just echo the message back
    uart_send(buffer, buffer_length);
}

int main(void) {
    clock_init();

    // initialize registers for UART to work and specify function that is
    // going to be executed on incoming bytes
    uart_init(uart_parse);

    // send a message
    char message[] = "Hello\r\n";
    uart_send(message, 7);

    while(1);
}

void clock_init(void) {
    // configure clock to 48MHz
    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;
    RCC->CTLR |= RCC_HSION;

    while (!(RCC->CTLR & RCC_HSIRDY));   // wait for HSI

    // CFGR0->HPRE = 0  SYSCLK not divided

    // CFGR0->PLLSRC = 0    HSI selected

    RCC->CFGR0 |= RCC_PLLMULL6;     // multiply PLL source *6

    RCC->CTLR |= RCC_PLLON;     // enable PLL

    while (!(RCC->CTLR & RCC_PLLRDY));   // wait for PLL to lock

    RCC->CFGR0 |= RCC_SW_PLL;    // set PLL as clock source

    while (!(RCC->CFGR0 & RCC_SWS_PLL));   // check if PLL is the source
}
