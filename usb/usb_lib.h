/*
    Configuration for USB CH32V203

    https://github.com/m-fudala

    Note: there are two USB modules, regular USB (registers at address starting
    with 0x4...) and OTG FS (registers starting at address 0x5...). It seems
    that regular USB connects to PA12/PA11 and is named U1/USB, while the OTG
    connects to PB7/PB6 (despite what diagram in reference manual says) and is
    named U2/USBFS. Some CH32V203 models have both, some have one or the other.
    This example shows use of the OTG FS USB module.
*/

#ifndef USB_LIB_H_
#define USB_LIB_H_

#include "ch32v20x.h"
#include "usb_hid_standards.h"
#include "usb_standards.h"

#define USB_DEFAULT_BUFFER_SIZE         64
#define USB_DEFAULT_ADDRESS             0

#define USBD_BTABLE_OFFSET              0

#define REG                         *(volatile uint32_t*)
#define BIT                         (uint32_t)

#define USBD_BASE                   (APB1PERIPH_BASE + 0x5C00)

#define USBD_CNTR                   REG (USBD_BASE + 0x40)
#define USBD_ISTR                   REG (USBD_BASE + 0x44)
#define USBD_FNR                    REG (USBD_BASE + 0x48)
#define USBD_DADDR                  REG (USBD_BASE + 0x4C)
#define USBD_BTABLE                 REG (USBD_BASE + 0x50)
#define SRAM_START                  (uint32_t)0x40006000

#define USBD_BUFF_TX_OFFSET(X)      0x40 + (X) * 0x40
#define USBD_BUFF_RX_OFFSET(X)      0x40 + 0x40 + (X) * 0x40
#define USBD_BUFF_TX(X)             REG (SRAM_START + USBD_BUFF_TX_OFFSET(X))
#define USBD_BUFF_RX(X)             REG (SRAM_START + USBD_BUFF_RX_OFFSET(X))

#define USBD_BUFF_TX_HALFWORD(X, Y) REG (SRAM_START + (USBD_BUFF_TX_OFFSET(X) \
                                            + (Y)) * 2)
#define USBD_BUFF_RX_HALFWORD(X, Y) REG (SRAM_START + (USBD_BUFF_RX_OFFSET(X) \
                                            + (Y)) * 2)

#define SRAM_HALFWORD(Y)            REG (SRAM_START + (Y))

// USBD_CNTR fields
#define USBD_CTRM                   BIT 0x8000
#define USBD_RESETM                 BIT 0x400
#define USBD_PDWN                   BIT 0x02
#define USBD_FRES                   BIT 0x01

// USBD_ISTR fields
#define USBD_CTR                    BIT 0x8000
#define USBD_RESET                  BIT 0x400
#define USBD_EP_ID_MASK             BIT 0xF

// USBD_DADDR fields
#define USBD_EF                     BIT 0x80
#define USBD_ADD_MASK               BIT 0x7F

#define USBD_EPR(X)                 REG (USBD_BASE + (X) * 0x4)

#define USBD_ADDR_TX(X)             REG(SRAM_START + USBD_BTABLE_OFFSET \
                                        + (X) * 0xF)
#define USBD_COUNT_TX(X)            REG(SRAM_START + USBD_BTABLE_OFFSET \
                                        + (X) * 0xF + 0x4)
#define USBD_ADDR_RX(X)             REG(SRAM_START + USBD_BTABLE_OFFSET \
                                        + (X) * 0xF + 0x8)
#define USBD_COUNT_RX(X)            REG(SRAM_START + USBD_BTABLE_OFFSET \
                                        + (X) * 0xF + 0xC)

// USBD_EPR fields
#define USBD_CTR_RX                 BIT 0x8000

#define USBD_STAT_RX_DISABLED       BIT 0x0 << 12
#define USBD_STAT_RX_STALL          BIT 0x1 << 12
#define USBD_STAT_RX_NAK            BIT 0x2 << 12
#define USBD_STAT_RX_ACK            BIT 0x3 << 12

#define USBD_STAT_RX_BIT1           BIT 0x2000
#define USBD_STAT_RX_BIT0           BIT 0x1000

#define USBD_SETUP                  BIT 0x800

#define USBD_EPTYPE_BULK            BIT 0x0 << 9
#define USBD_EPTYPE_CONTROL         BIT 0x1 << 9
#define USBD_EPTYPE_ISO             BIT 0x2 << 9
#define USBD_EPTYPE_INTERRUPT       BIT 0x3 << 9

#define USBD_EP_KIND                BIT 0x100

#define USBD_CTR_TX                 BIT 0x80

#define USBD_STAT_TX_DISABLED       BIT 0x0 << 4
#define USBD_STAT_TX_STALL          BIT 0x1 << 4
#define USBD_STAT_TX_NAK            BIT 0x2 << 4
#define USBD_STAT_TX_ACK            BIT 0x3 << 4

#define USBD_STAT_TX_BIT1           BIT 0x20
#define USBD_STAT_TX_BIT0           BIT 0x10

// USBD_COUNT_RX fields
#define USBD_BLSIZE                 BIT 0x8000
#define USBD_NUMBLOCK_64            USBD_BLSIZE | ((BIT 0x1) << 10)
#define USBD_COUNT_RX_MASK          BIT 0x3FF

typedef enum USBEndpoints {
    ENDPOINT0,
    ENDPOINT1
} USBEndpoints;

typedef enum USBErrors {
    NO_ERROR,
    REQ_NOT_IMPLEMENTED
} USBErrors;

typedef struct USB {
    volatile unsigned char device_address;

    volatile unsigned char device_error;
    volatile unsigned char device_state;

    volatile unsigned char control_stage;

    USBSetupRequest request;

    volatile unsigned char tx_bytes_to_send;
    volatile unsigned char *tx_pointer;
} USB;

typedef struct USBEndpoint1 {
    volatile unsigned char tx_bytes_to_send;
    volatile unsigned char *tx_pointer;
} USBEndpoint1;

typedef struct USBDebugs {
    volatile unsigned char reset_counter;
    volatile unsigned char control_counter;
    volatile unsigned char in_counter;
    volatile unsigned char out_counter;
} USBDebugs;

void usb_init(void);

void set_address(unsigned char address);

void configure_endpoint_control(unsigned char endpoint);

void clear_sram(void);
void copy_rx_to_buffer(unsigned char endpoint, unsigned char* buffer,
        unsigned char length);
void copy_buffer_to_tx(unsigned char endpoint);

void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt()));

#endif
