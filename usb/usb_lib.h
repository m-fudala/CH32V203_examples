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
#define USBFS_DEFAULT_ADDRESS           0

// R8_USB_CTRL bits
#define USBD_EN_PULLUP_EN               (2 << 4)
#define RB_UC_INT_BUSY                  (1 << 3)
#define RB_UC_RESET_SIE                 (1 << 2)
#define RB_UC_CLR_ALL                   (1 << 1)
#define RB_UC_DMA_EN                    (1 << 0)

// R8_UDEV_CTRL bits
#define RB_UD_PORT_EN                   (1 << 0)

// R8_USB_INT_EN bits
#define RB_UIE_DEV_NAK                  (1 << 6)
#define RB_UIE_FIFO_OV                  (1 << 4)
#define RB_UIE_SUSPEND                  (1 << 2)
#define RB_UIE_TRANSFER                 (1 << 1)
#define RB_UIE_BUS_RST                  (1 << 0)

// R8_USB_DEV_AD
#define MASK_USB_ADDR                   0x7F

// R8_USB_INT_FG bits
#define RB_U_IS_NAK                     (1 << 7)
#define RB_U_TOG_OK                     (1 << 6)
#define RB_U_SIE_FREE                   (1 << 5)
#define RB_UIF_FIFO_OV                  (1 << 4)
#define RB_UIF_HST_SOF                  (1 << 3)
#define RB_UIF_SUSPEND                  (1 << 2)
#define RB_UIF_TRANSFER                 (1 << 1)
#define RB_UIF_BUS_RST                  (1 << 0)

// R8_USB_INT_ST bits
#define RB_UIS_IS_NAK                   (1 << 7)
#define RB_UIS_TOG_OK                   (1 << 6)
#define MASK_UIS_TOKEN                  (3 << 4)
#define MASK_UIS_ENDP                   0xF
#define UIS_TOKEN_OUT                   0x0
#define UIS_TOKEN_IN                    0x2
#define UIS_TOKEN_SETUP                 0x3

// R8_UEPn_TX_CTRL bits
#define RB_UEP_T_AUTO_TOG               (1 << 3)
#define RB_UEP_T_TOG                    (1 << 2)
#define TX_D0_D1_READY_ACK_EXPECTED     0x0
#define TX_D0_D1_REPLY_NO_RESP_EXPECTED 0x1
#define TX_ANSWER_NAK                   0x2
#define TX_ANSWER_STALL                 0x3

// R8_UEPn_RX_CTRL bits
#define RB_UEP_R_AUTO_TOG               (1 << 3)
#define RB_UEP_R_TOG                    (1 << 2)
#define RX_ANSWER_ACK                   0x0
#define RX_TIMEOUT_NO_RESPONSE          0x1
#define RX_ANSWER_NAK                   0x2
#define RX_ANSWER_STALL                 0x3

// R8_UEP4_1_MOD bits
#define RB_UEP1_RX_EN                   (1 << 7)
#define RB_UEP1_TX_EN                   (1 << 6)
#define RB_UEP1_BUF_MOD                 (1 << 4)

// structure of USB setup packet
typedef struct USBSetupRequest {
    struct {
        unsigned char transfer_direction : 1;
        unsigned char type : 2;
        unsigned char recipent : 5;
    } bmRequestType;

    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} USBSetupRequest;

// setup packet defines
#define USB_SETUP_REQUEST_TYPE_DIR                  (1 << 7)
#define GET_USB_SETUP_REQUEST_TYPE_DIR(X)           \
            (((X) & USB_SETUP_REQUEST_TYPE_DIR) >> 7)
#define USB_SETUP_REQUEST_TYPE_DIR_HOST_DEVICE      0
#define USB_SETUP_REQUEST_TYPE_DIR_DEVICE_HOST      1
#define USB_SETUP_REQUEST_TYPE_TYPE                 (3 << 5)
#define GET_USB_SETUP_REQUEST_TYPE_TYPE(X)          \
            (((X) & USB_SETUP_REQUEST_TYPE_TYPE) >> 5)
#define USB_SETUP_REQUEST_TYPE_TYPE_STANDARD        0
#define USB_SETUP_REQUEST_TYPE_TYPE_CLASS           1
#define USB_SETUP_REQUEST_TYPE_TYPE_VENDOR          2
#define USB_SETUP_REQUEST_TYPE_REC                  (0x1F << 0)
#define GET_USB_SETUP_REQUEST_TYPE_REC(X)           \
            ((X) & USB_SETUP_REQUEST_TYPE_REC)
#define USB_SETUP_REQUEST_TYPE_REC_DEVICE           0
#define USB_SETUP_REQUEST_TYPE_REC_INTERFACE        1
#define USB_SETUP_REQUEST_TYPE_REC_ENDPOINT         2
#define USB_SETUP_REQUEST_TYPE_REC_OTHER            3

// standard device requests
#define SETUP_DEVICE_REQS_GET_STATUS                0x0
#define SETUP_DEVICE_REQS_CLEAR_FEATURE             0x1
#define SETUP_DEVICE_REQS_SET_FEATURE               0x3
#define SETUP_DEVICE_REQS_SET_ADDRESS               0x5
#define SETUP_DEVICE_REQS_GET_DESCRIPTOR            0x6
#define SETUP_DEVICE_REQS_SET_DESCRIPTOR            0x7
#define SETUP_DEVICE_REQS_GET_CONFIGURATION         0x8
#define SETUP_DEVICE_REQS_SET_CONFIGURATION         0x9

void usb_init(void);

void set_address(unsigned char address);

void configure_endpoint0(void);
void deconfigure_endpoint1(void);
void configure_endpoint1(void);

void USBFS_IRQHandler(void) __attribute__((interrupt()));

#endif
