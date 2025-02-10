#include "usb_lib.h"

__attribute__((__aligned__(4))) unsigned char endpoint0_buffer
                                    [USBFS_DEFAULT_BUFFER_SIZE];
__attribute__((__aligned__(4))) unsigned char endpoint1_buffer
                                    [USBFS_DEFAULT_BUFFER_SIZE];

void usb_init(void) {
    // GPIO settings
    RCC->APB2PCENR |= RCC_IOPBEN;   // enable port B clock
    // DP - PB7, DM - PB6 -> output low level in push-pull mode before
    // initialization
    GPIOB->CFGLR |= 0x33000000;
    GPIOB->OUTDR |= 3 << 6;

    RCC->AHBPCENR |= RCC_USBFS;     // enable USB FS clock

    // enable device mode and internal pull-up 
    USBFSD->BASE_CTRL |= USBD_EN_PULLUP_EN | RB_UC_INT_BUSY;

    // enable interrupts
    PFIC->IENR[1] |= (3 << 3) | (3 << 27);  // enable USB HP/LP (35,36), USBFS
                                            // and USBFSWakeUp (59,60) 
                                            // Note: contary to what RM
                                            // says, USBFS and USBFSWakeUp are
                                            // interrupts 59 and 60, as stated
                                            // in the header

    USBFSD->INT_EN |= RB_UIE_DEV_NAK | RB_UIE_FIFO_OV |
                    RB_UIE_SUSPEND | RB_UIE_TRANSFER |
                    RB_UIE_BUS_RST;

    USBFSD->INT_FG |= 0x1F;     // clear interrupts

    // set default address
    USBFSD->DEV_ADDR &= ~(MASK_USB_ADDR);

    // enable device physical port
    USBFSD->UDEV_CTRL |= RB_UD_PORT_EN;

    // clear resets
    USBFSD->BASE_CTRL &= ~(RB_UC_RESET_SIE | RB_UC_CLR_ALL);
}

void configure_endpoint0(void) {
    USBFSD->UEP0_DMA = (int)endpoint0_buffer;

    USBFSD->UEP0_TX_CTRL |= RB_UEP_T_AUTO_TOG | TX_ANSWER_NAK;
    USBFSD->UEP0_RX_CTRL |= RB_UEP_R_AUTO_TOG | RX_ANSWER_NAK;
}

void deconfigure_endpoint1(void) {
    USBFSD->UEP4_1_MOD = 0;
    USBFSD->UEP1_TX_CTRL = 0;
    USBFSD->UEP1_RX_CTRL = 0;
}

void configure_endpoint1(void) {
    USBFSD->UEP1_DMA = (int)endpoint1_buffer;

    // enable IN endpoint
    USBFSD->UEP4_1_MOD |= RB_UEP1_TX_EN;

    USBFSD->UEP1_TX_CTRL |= RB_UEP_T_AUTO_TOG | TX_ANSWER_NAK;
    USBFSD->UEP1_RX_CTRL |= RB_UEP_R_AUTO_TOG | RX_ANSWER_NAK;
}

void USB_HP_CAN1_TX_IRQHandler(void) {

}

void USB_LP_CAN1_RX0_IRQHandler(void) {

}

void USBFS_IRQHandler(void) {
    volatile int usb_interrupt_flags = USBFSD->INT_FG;  // read the status

    if (usb_interrupt_flags & RB_UIF_BUS_RST) {
        // TODO: deconfigure more endpoints
        deconfigure_endpoint1();   
        configure_endpoint0();
        configure_endpoint1();     

        USBFSD->INT_FG |= RB_UIF_BUS_RST;   // clear interrupt
    } else if (usb_interrupt_flags & RB_UIF_TRANSFER) {
        configure_endpoint0();

        volatile int usb_interrupt_status = USBFSD->INT_ST;
        int current_token = (usb_interrupt_status & MASK_UIS_TOKEN) >> 4;
        int current_endpoint = usb_interrupt_status & MASK_UIS_ENDP;
        int no_of_bytes = USBFSD->RX_LEN;

        switch (current_token) {
            case UIS_TOKEN_OUT: {

                
                break;
            }

            case UIS_TOKEN_IN: {


                break;
            }

            case UIS_TOKEN_SETUP: {


                break;
            }
        }

        USBFSD->INT_FG |= RB_UIF_TRANSFER;   // clear interrupt
    }
}

void USBFSWakeUp_IRQHandler(void) {

}
