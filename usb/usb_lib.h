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

void usb_init(void);

void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((interrupt()));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt()));
void USBFS_IRQHandler(void) __attribute__((interrupt()));
void USBFSWakeUp_IRQHandler(void) __attribute__((interrupt()));

#endif
