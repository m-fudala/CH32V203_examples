#include "usb_lib.h"
#include "usb_descriptors.h"

__attribute__((__aligned__(4))) unsigned char endpoint0_buffer
                                    [USBFS_DEFAULT_BUFFER_SIZE];
__attribute__((__aligned__(4))) unsigned char endpoint1_buffer
                                    [USBFS_DEFAULT_BUFFER_SIZE];

USB usb;
USBEndpoint1 endpoint1;

void usb_init(void) {
    // GPIO settings
    RCC->APB2PCENR |= RCC_IOPBEN;   // enable port B clock
    // DP - PB7, DM - PB6 -> output low level in push-pull mode before
    // initialization
    GPIOB->CFGLR |= 0x33000000;
    GPIOB->OUTDR |= 3 << 6;

    RCC->AHBPCENR |= RCC_USBFS;     // enable USB FS clock

    // enable device mode, internal pull-up and DMA
    USBFSD->BASE_CTRL |= USBD_EN_PULLUP_EN | RB_UC_DMA_EN;

    // enable interrupts
    PFIC->IENR[1] |= (3 << 3) | (3 << 27);  // enable USB HP/LP (35,36), USBFS
                                            // and USBFSWakeUp (59,60) 
                                            // Note: contary to what RM
                                            // says, USBFS and USBFSWakeUp are
                                            // interrupts 59 and 60, as stated
                                            // in the header

    USBFSD->INT_EN |= RB_UIE_TRANSFER | RB_UIE_BUS_RST;

    USBFSD->INT_FG |= 0x1F;     // clear interrupts

    // clear resets
    USBFSD->BASE_CTRL &= ~(RB_UC_RESET_SIE | RB_UC_CLR_ALL);

    // enable device physical port
    USBFSD->UDEV_CTRL |= RB_UD_PORT_EN;
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

    USBFSD->UEP0_TX_CTRL = TX_ANSWER_NAK;
    USBFSD->UEP0_RX_CTRL = 0;
}

void deconfigure_endpoint1(void) {
    USBFSD->UEP4_1_MOD = 0;
    USBFSD->UEP1_TX_CTRL = 0;
    USBFSD->UEP1_RX_CTRL = 0;
}

void configure_endpoint1(void) {
    USBFSD->UEP1_DMA = (int)endpoint1_buffer;

    // enable IN endpoint
    USBFSD->UEP4_1_MOD = RB_UEP1_TX_EN;

    USBFSD->UEP1_TX_CTRL = 0;
    USBFSD->UEP1_RX_CTRL = 0;
}

void write_bytes_endpoint0(void) {
    unsigned char packet_length;

    if (usb.tx_bytes_to_send > USBFS_DEFAULT_BUFFER_SIZE) {
        packet_length = USBFS_DEFAULT_BUFFER_SIZE;
    } else {
        packet_length = usb.tx_bytes_to_send;
    }

    for (unsigned char byte = 0; byte < packet_length; byte++) {
        *(endpoint0_buffer + byte) = *usb.tx_pointer;

        usb.tx_pointer++;                           
    }

    USBFSD->UEP0_TX_LEN = packet_length;
    usb.tx_bytes_to_send -= packet_length;

    USBFSD->UEP0_TX_CTRL &= TX_D0_D1_READY_ACK_EXPECTED;
    USBFSD->UEP0_TX_CTRL ^= RB_UEP_T_TOG;
}

void write_bytes_endpoint1(void) {
    unsigned char packet_length;

    if (endpoint1.tx_bytes_to_send > USBFS_DEFAULT_BUFFER_SIZE) {
        packet_length = USBFS_DEFAULT_BUFFER_SIZE;
    } else {
        packet_length = endpoint1.tx_bytes_to_send;
    }

    for (unsigned char byte = 0; byte < packet_length; byte++) {
        *(endpoint1_buffer + byte) = *endpoint1.tx_pointer;

        endpoint1.tx_pointer++;                           
    }

    USBFSD->UEP1_TX_LEN = packet_length;
    endpoint1.tx_bytes_to_send -= packet_length;

    USBFSD->UEP1_TX_CTRL &= TX_D0_D1_READY_ACK_EXPECTED;
    USBFSD->UEP1_TX_CTRL ^= RB_UEP_T_TOG;
}

void USBFS_IRQHandler(void) {
    volatile int usb_interrupt_flags = USBFSD->INT_FG;  // read the status
    volatile int usb_interrupt_status = USBFSD->INT_ST;

    if (usb_interrupt_flags & RB_UIF_BUS_RST) {        
        // TODO: deconfigure more endpoints
        deconfigure_endpoint1();  
        configure_endpoint0();
        configure_endpoint1();

        set_address(USBFS_DEFAULT_ADDRESS);

        usb.device_state = USB_DEVICE_STATE_DEFAULT;

        USBFSD->INT_FG |= RB_UIF_BUS_RST;   // clear interrupt
    } else if (usb_interrupt_flags & RB_UIF_TRANSFER) {
        int current_token = (usb_interrupt_status & MASK_UIS_TOKEN) >> 4;
        int current_endpoint = usb_interrupt_status & MASK_UIS_ENDP;
        int no_of_bytes = USBFSD->RX_LEN;

        switch (current_token) {
            case UIS_TOKEN_OUT: {
                // only ACK OUT packets for now
                USBFSD->UEP0_RX_CTRL ^= RB_UEP_R_TOG;

                break;
            }

            case UIS_TOKEN_IN: {
                switch (current_endpoint) {
                    case ENDPOINT0: {
                        if (usb.device_state == USB_DEVICE_STATE_DEFAULT) {
                            set_address(usb.device_address);
                        }

                        break;
                    }

                    case ENDPOINT1: {
                        USBHIDReport hid_report = {
                            .x = 10,
                            .y = 0,
                            .buttons = 0
                        };

                        endpoint1.tx_pointer = (unsigned char *)&hid_report;
                        endpoint1.tx_bytes_to_send = sizeof(hid_report);

                        // write_bytes_endpoint1();

                        break;
                    }
                }

                break;
            }

            case UIS_TOKEN_SETUP: {
                USBFSD->UEP0_RX_CTRL |= RX_ANSWER_NAK;
                
                usb.request.bmRequestType.transfer_direction = 
                        GET_USB_SETUP_REQUEST_TYPE_DIR(endpoint0_buffer[0]);
                usb.request.bmRequestType.type = 
                        GET_USB_SETUP_REQUEST_TYPE_TYPE(endpoint0_buffer[0]);
                usb.request.bmRequestType.recipent = 
                        GET_USB_SETUP_REQUEST_TYPE_REC(endpoint0_buffer[0]);

                usb.request.bRequest = endpoint0_buffer[1];
                usb.request.wIndex = (unsigned short)
                        ((endpoint0_buffer[4] << 8) | endpoint0_buffer[5]);
                usb.request.wLength = (unsigned short)
                        ((endpoint0_buffer[7] << 8) | endpoint0_buffer[6]);

                switch (usb.request.bmRequestType.type) {
                    case USB_SETUP_REQUEST_TYPE_TYPE_STANDARD: {
                        switch (usb.request.bRequest) {
                            case SETUP_DEVICE_REQS_GET_DESCRIPTOR: {
                                usb.request.wValue = (unsigned short)
                                        ((endpoint0_buffer[2] << 8) |
                                        endpoint0_buffer[3]);

                                switch (usb.request.wValue) {
                                    case DESC_TYPE_DEVICE: {
                                        usb.tx_pointer = 
                                                (unsigned char *)
                                                &device_descriptor;
                                        usb.tx_bytes_to_send = 
                                                sizeof(device_descriptor);
                                        
                                        break;
                                    }

                                    case DESC_TYPE_CONFIGURATION: {
                                        usb.tx_pointer = (unsigned char *)
                                                &full_configuration_descriptor;
                                        usb.tx_bytes_to_send = 
                                                usb.request.wLength;

                                        break;
                                    }

                                    case DESC_TYPE_HID_REPORT: {
                                        usb.tx_pointer = 
                                                (unsigned char *)
                                                &hid_report_descriptor;
                                        usb.tx_bytes_to_send = 
                                                sizeof(hid_report_descriptor);

                                        break;
                                    }

                                    // case DESC_TYPE_STRING: {
                                    //     switch(usb.request.wIndex) {
                                    //         case STRING_DESCRIPTOR0: {
                                    //             usb.tx_pointer = 
                                    //                 (unsigned char *)
                                    //                 &string_descriptor0;
                                    //             usb.tx_bytes_to_send = 
                                    //                 string_descriptor0.bLength;

                                    //                 string_req++;
                                                
                                    //             break;
                                    //         }

                                    //         case STRING_DESCRIPTOR_MANUFACTURER: {
                                    //             usb.tx_pointer = 
                                    //                 (unsigned char *)
                                    //                 &string_descriptor_manufacturer;
                                    //             usb.tx_bytes_to_send = 
                                    //                 string_descriptor_manufacturer.bLength;
                                                
                                    //             break;
                                    //         }

                                    //         case STRING_DESCRIPTOR_PRODUCT: {
                                    //             usb.tx_pointer = 
                                    //                 (unsigned char *)
                                    //                 &string_descriptor_product;
                                    //             usb.tx_bytes_to_send = 
                                    //                 string_descriptor_product.bLength;
                                                
                                    //             break;
                                    //         }

                                    //         case STRING_DESCRIPTOR_SERIAL_NUMBER: {
                                    //             usb.tx_pointer = 
                                    //                 (unsigned char *)
                                    //                 &string_descriptor_serial_number;
                                    //             usb.tx_bytes_to_send = 
                                    //                 string_descriptor_serial_number.bLength;
                                                
                                    //             break;
                                    //         }
                                    //     }

                                    //     break;
                                    // }
                                }
                                
                                break;
                            }

                            case SETUP_DEVICE_REQS_SET_ADDRESS: {
                                usb.request.wValue = (unsigned short)
                                        ((endpoint0_buffer[3] << 8) |
                                        endpoint0_buffer[2]);

                                // save received address
                                usb.device_address = usb.request.wValue;

                                usb.tx_bytes_to_send = 0;

                                usb.device_state = USB_DEVICE_STATE_ADDRESSED;

                                break;
                            }

                            case SETUP_DEVICE_REQS_SET_CONFIGURATION: {
                                usb.request.wValue = (unsigned short)
                                        ((endpoint0_buffer[3] << 8) |
                                        endpoint0_buffer[2]);

                                usb.tx_bytes_to_send = 0;

                                usb.device_state = USB_DEVICE_STATE_CONFIGURED;

                                break;
                            }

                            default: {
                                usb.device_error = REQ_NOT_IMPLEMENTED;

                                break;
                            }
                        }

                        break;
                    }

                    case USB_SETUP_REQUEST_TYPE_TYPE_CLASS: {
                        switch (usb.request.bRequest) {
                            case HID_REQS_SET_IDLE: {
                                usb.tx_bytes_to_send = 0;
                            }
                        
                            default: {
                                usb.device_error = REQ_NOT_IMPLEMENTED;

                                break;
                            }
                        }

                        break;
                    }
                }

                USBFSD->UEP0_RX_CTRL &= RX_ANSWER_ACK;

                if (usb.device_error) {
                    USBFSD->UEP0_TX_LEN = 0;
                    USBFSD->UEP0_TX_CTRL |= TX_ANSWER_STALL;
                    USBFSD->UEP0_TX_CTRL ^= RB_UEP_T_TOG;

                    usb.device_error = NO_ERROR;
                } else {
                    write_bytes_endpoint0();
                }

                break;
            }
        }

        USBFSD->INT_FG |= RB_UIF_TRANSFER;   // clear interrupt
    }
}
