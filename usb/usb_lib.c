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
    PFIC->IENR[1] |= (3 << 3) | (3 << 27);  // enable USB HP/LP (35,36), USBFS
                                            // and USBFSWakeUp (59,60) 
                                            // Note: contary to what RM
                                            // says, USBFS and USBFSWakeUp are
                                            // interrupts 59 and 60, as stated
                                            // in the header

    USBFSD->INT_EN |= USBFSD_RB_UIE_DEV_NAK | USBFSD_RB_UIE_FIFO_OV |
                    USBFSD_RB_UIE_SUSPEND | USBFSD_RB_UIE_TRANSFER |
                    USBFSD_RB_UIE_BUS_RST;

    // set default address
    USBFSD->DEV_ADDR &= ~(MASK_USB_ADDR);

    // enable device physical port
    USBFSD->UDEV_CTRL |= USBFSD_RB_UD_PORT_EN;

    // clear resets
    USBFSD->BASE_CTRL &= ~(USBFSD_RB_UC_RESET_SIE | USBFSD_RB_UC_CLR_ALL);
}

void USB_HP_CAN1_TX_IRQHandler(void) {

}

void USB_LP_CAN1_RX0_IRQHandler(void) {

}

void USBFS_IRQHandler(void) {
    volatile int usb_interrupt_flags = USBFSD->INT_FG;  // read the status
}

void USBFSWakeUp_IRQHandler(void) {
    
}
