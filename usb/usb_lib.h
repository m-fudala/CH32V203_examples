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

#define USBFS_DEFAULT_BUFFER_SIZE       64

// R8_USB_CTRL bits
#define USBFSD_USBD_EN_PULLUP_EN        (2 << 4)
#define USBFSD_RB_UC_INT_BUSY           (1 << 3)
#define USBFSD_RB_UC_RESET_SIE          (1 << 2)
#define USBFSD_RB_UC_CLR_ALL            (1 << 1)

// R8_UDEV_CTRL bits
#define USBFSD_RB_UD_PORT_EN            (1 << 0)

// R8_USB_INT_EN bits
#define USBFSD_RB_UIE_DEV_NAK           (1 << 6)
#define USBFSD_RB_UIE_FIFO_OV           (1 << 4)
#define USBFSD_RB_UIE_SUSPEND           (1 << 2)
#define USBFSD_RB_UIE_TRANSFER          (1 << 1)
#define USBFSD_RB_UIE_BUS_RST           (1 << 0)

// R8_USB_DEV_AD
#define MASK_USB_ADDR                   (0x7F)

// R8_USB_INT_FG bits
#define RB_UIF_BUS_RST                  (1 << 0)

// R8_UEPn_TX_CTRL bits
#define RB_UEP_T_AUTO_TOG               (1 << 3)
#define RB_UEP_T_TOG                    (1 << 2)
#define TX_D0_D1_READY_ACK_EXPECTED     (0x0)
#define TX_D0_D1_REPLY_NO_RESP_EXPECTED (0x1)
#define TX_ANSWER_NAK                   (0x2)
#define TX_ANSWER_STALL                 (0x3)

// R8_UEPn_RX_CTRL bits
#define RB_UEP_R_AUTO_TOG               (1 << 3)
#define RB_UEP_R_TOG                    (1 << 2)
#define RX_ANSWER_ACK                   (0x0)
#define RX_TIMEOUT_NO_RESPONSE          (0x1)
#define RX_ANSWER_NAK                   (0x2)
#define RX_ANSWER_STALL                 (0x3)

// R8_UEP4_1_MOD bits
#define RB_UEP1_RX_EN                   (1 << 7)
#define RB_UEP1_TX_EN                   (1 << 6)
#define RB_UEP1_BUF_MOD                 (1 << 4)

void usb_init(void);

void configure_endpoint0(void);
void deconfigure_endpoint1(void);
void configure_endpoint1(void);

void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((interrupt()));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt()));
void USBFS_IRQHandler(void) __attribute__((interrupt()));
void USBFSWakeUp_IRQHandler(void) __attribute__((interrupt()));

#endif
