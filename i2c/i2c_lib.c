#include "i2c_lib.h"

void i2c_master_init(void) {
    // clock settings
    RCC->APB2PCENR |= RCC_IOPBEN;   // enable port B clock
    RCC->APB1PCENR |= RCC_I2C1EN;   // enable I2C1 clock

    // GPIO settings
    GPIOB->CFGLR |= 0xEE000000; // SCL - PB6 -> open-drain alternate output
                                // SDA - PB7 -> open-drain alternate output

    PFIC->IENR[1] |= 3 << 15;   // enable I2C1 global interrupts (47, 48)

    // I2C settings
    I2C1->CTLR2 |= I2C_CTLR2_ITBUFEN | I2C_CTLR2_ITEVTEN | I2C_CTLR2_ITERREN
                    | 48;   // enable interrupts, set input clock as 48 MHz
    I2C1->CKCFGR |= I2C_CKCFGR_FS | 120;    // set fast mode and 120 clock
                                            // divider

    I2C1->CTLR1 |= I2C_CTLR1_PE;    // enable peripheral
}

void i2c_slave_init(char address) {
    // clock settings
    RCC->APB2PCENR |= RCC_IOPBEN;   // enable port B clock
    RCC->APB1PCENR |= RCC_I2C1EN;   // enable I2C1 clock

    // GPIO settings
    GPIOB->CFGLR |= 0xEE000000; // SCL - PB6 -> open-drain alternate output
                                // SDA - PB7 -> open-drain alternate output

    PFIC->IENR[1] |= 3 << 15;   // enable I2C1 global interrupts (47, 48)

    // I2C settings
    I2C1->CTLR2 |= I2C_CTLR2_ITBUFEN | I2C_CTLR2_ITEVTEN | I2C_CTLR2_ITERREN
                    | 48;   // enable interrupts, set input clock as 48 MHz
    I2C1->CKCFGR |= I2C_CKCFGR_FS | 120;    // set fast mode and 120 clock
                                            // divider

    I2C1->OADDR1 |= address << 1;

    I2C1->CTLR1 |= I2C_CTLR1_PE;    // enable peripheral
}

void I2C1_EV_IRQHandler(void) {

}

void I2C1_ER_IRQHandler(void) {

}

