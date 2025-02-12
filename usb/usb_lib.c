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

    // enable device mode, internal pull-up and DMA
    USBFSD->BASE_CTRL |= USBD_EN_PULLUP_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN;

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

    // enable device physical port
    USBFSD->UDEV_CTRL |= RB_UD_PORT_EN;

    // clear resets
    USBFSD->BASE_CTRL &= ~(RB_UC_RESET_SIE | RB_UC_CLR_ALL);
}

void set_address(unsigned char address) {
    if (address) {
        USBFSD->DEV_ADDR |= address;
    } else {
        // set default address
        USBFSD->DEV_ADDR &= ~(MASK_USB_ADDR);
    }
}

void configure_endpoint0(void) {
    USBFSD->UEP0_DMA = (int)endpoint0_buffer;

    USBFSD->UEP0_TX_CTRL |= RB_UEP_T_AUTO_TOG | TX_D0_D1_READY_ACK_EXPECTED;
    USBFSD->UEP0_RX_CTRL |= RB_UEP_R_AUTO_TOG | RX_ANSWER_ACK;
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

    USBFSD->UEP1_TX_CTRL |= RB_UEP_T_AUTO_TOG | TX_D0_D1_READY_ACK_EXPECTED;
    USBFSD->UEP1_RX_CTRL |= RB_UEP_R_AUTO_TOG | RX_ANSWER_ACK;
}

void USBFS_IRQHandler(void) {
    volatile int usb_interrupt_flags = USBFSD->INT_FG;  // read the status

    if (usb_interrupt_flags & RB_UIF_BUS_RST) {
        set_address(USBFS_DEFAULT_ADDRESS);
        
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
                USBSetupRequest request;

                request.bmRequestType.transfer_direction = 
                        GET_USB_SETUP_REQUEST_TYPE_DIR(endpoint0_buffer[0]);
                request.bmRequestType.type = 
                        GET_USB_SETUP_REQUEST_TYPE_TYPE(endpoint0_buffer[0]);
                request.bmRequestType.recipent = 
                        GET_USB_SETUP_REQUEST_TYPE_REC(endpoint0_buffer[0]);

                request.bRequest = endpoint0_buffer[1];
                request.wValue = (unsigned short)((endpoint0_buffer[2] << 8) |
                        endpoint0_buffer[3]);
                request.wIndex = (unsigned short)((endpoint0_buffer[4] << 8) |
                        endpoint0_buffer[5]);
                request.wLength = (unsigned short)((endpoint0_buffer[6] << 8) |
                        endpoint0_buffer[7]);

                switch (request.bRequest) {
                    case SETUP_DEVICE_REQS_GET_DESCRIPTOR: {
                        switch (request.wValue) {
                            case DESC_TYPE_DEVICE: {
                                
                                
                                break;
                            } 
                        }
                        
                        break;
                    }
                }

                break;
            }
        }

        USBFSD->INT_FG |= RB_UIF_TRANSFER;   // clear interrupt
    }
}
