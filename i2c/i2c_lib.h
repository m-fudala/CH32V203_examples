/*
    Configuration for I2C1 on CH32V203

    https://github.com/m-fudala
*/

#ifndef I2C_LIB_H_
#define I2C_LIB_H_

#include "ch32v20x.h"

typedef struct I2C {
    volatile unsigned char tx_bytes_sent;
    volatile unsigned char tx_bytes_to_send;
    volatile char tx_destination;
    volatile char *tx_bytes;
} I2C;

void i2c_master_init(void);
void i2c_slave_init(char address);

void i2c_master_send(char destination_address, char *bytes,
                        unsigned int bytes_length);

void I2C1_EV_IRQHandler(void) __attribute__((interrupt()));
void I2C1_ER_IRQHandler(void) __attribute__((interrupt()));

#endif
