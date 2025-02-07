#include "usb_lib.h"

void usb_init(void) {
    // GPIO settings
    RCC->APB2PCENR |= RCC_IOPBEN;   // enable port B clock
    // DP - PB7, DM - PB6 -> output low level in push-pull mode before
    // initialization
    GPIOB->CFGLR |= 0x33000000;
    GPIOB->OUTDR |= 3 << 6;

    RCC->AHBPCENR |= RCC_USBFS;     // enable USB FS clock

    // enable device mode and internal pull-up 
    USBFSD->BASE_CTRL |= USBFSD_USBD_EN_PULLUP_EN | USBFSD_RB_UC_INT_BUSY;

    // enable interrupts

    // set default address

    // enable device physical port
    USBFSD->UDEV_CTRL |= USBFSD_RB_UD_PORT_EN;

    // clear resets
    USBFSD->BASE_CTRL &= ~(USBFSD_RB_UC_RESET_SIE | USBFSD_RB_UC_CLR_ALL);
}
