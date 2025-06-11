/*
    Example of how to use USB on CH32V203 microcontroller
    https://github.com/m-fudala

*/
#include "ch32v20x.h"
#include "usb_lib.h"

void clock_init(void);
void HardFault_Handler(void) __attribute__((interrupt()));

int main(void) {
    clock_init();

    usb_init();

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

void HardFault_Handler(void) {
    asm("nop");
}

