#include "usb_lib.h"
#include "usb_descriptors.h"

USB usb;
USBEndpoint1 endpoint1;

unsigned char endpoint0_buffer[USB_DEFAULT_BUFFER_SIZE];

void usb_init(void) {
    // GPIO settings
    RCC->APB2PCENR |= RCC_IOPAEN;   // enable port A clock
    // DP - PA12, DM - PA11 -> output low level in push-pull mode before
    // initialization
    GPIOA->OUTDR = 0;
    GPIOA->CFGHR = GPIO_CFGHR_MODE12_1 | GPIO_CFGHR_MODE12_0 |
            GPIO_CFGHR_MODE11_1 | GPIO_CFGHR_MODE11_0;

    RCC->APB1PCENR |= RCC_USBEN;     // enable USBD clock

    // power up peripherial
    USBD_CNTR &= ~USBD_PDWN;

    // enable internal pull-up and set the speed
    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;

    // clear interrupt flags
    USBD_ISTR = 0;

    // clear reset bit
    USBD_CNTR &= ~USBD_FRES;

    // enable interrupts
    PFIC->IENR[1] |= (3 << 3);  // enable USB HP/LP (35,36)
    USBD_CNTR |= USBD_CTRM | USBD_RESETM;

    clear_sram();
}

void set_address(unsigned char address) {
    if (address) {
        USBD_DADDR |= address;
    } else {
        // set default address
        USBD_DADDR &= ~(USBD_ADD_MASK);
    }
}

void configure_endpoint_control(unsigned char endpoint) {
    USBD_COUNT_RX(endpoint) = USBD_NUMBLOCK_64;
    USBD_ADDR_TX(endpoint) = USBD_BUFF_TX_OFFSET(endpoint);
    USBD_ADDR_RX(endpoint) = USBD_BUFF_RX_OFFSET(endpoint);
    USBD_EPR(endpoint) |= USBD_STAT_RX_ACK | USBD_EPTYPE_CONTROL |
            USBD_STAT_TX_NAK;
}

void clear_sram(void) {
    for (unsigned int i = 0; i < 1024; i += 4) {
        SRAM_BYTE(i) = 0;
    }
}

void copy_rx_to_buffer(unsigned char endpoint, unsigned char* buffer, 
        unsigned char length) {
    for (unsigned char i = 0; i < length / 2; ++i) {
        unsigned short halfword = USBD_BUFF_RX_BYTE(endpoint, i * 2);
        *(buffer + 2 * i) = (unsigned char)(halfword & 0xFF);
        *(buffer + 2 * i + 1) = (unsigned char)((halfword >> 8) & 0xFF);
    }
}

void copy_buffer_to_tx(unsigned char endpoint) {
    unsigned char packet_length;

    if (usb.tx_bytes_to_send > USB_DEFAULT_BUFFER_SIZE) {
        packet_length = USB_DEFAULT_BUFFER_SIZE;
    } else {
        packet_length = usb.tx_bytes_to_send;
    }

    for (unsigned char i = 0; i < packet_length / 2; ++i) {
        USBD_BUFF_TX_BYTE(endpoint, i * 2) = *(usb.tx_pointer + 2 * i) << 8 |
                *(usb.tx_pointer + 2 * i + 1);
    }

    USBD_COUNT_TX(endpoint) = packet_length;
    usb.tx_bytes_to_send -= packet_length;
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
    // read interrupt flags
    volatile unsigned int usb_interrupt_flags = USBD_ISTR;

    if (usb_interrupt_flags & USBD_RESET) {
        USBD_ISTR &= ~USBD_RESET;     // clear interrupt

        // initialize packet buffer description table
        USBD_BTABLE = USBD_BTABLE_OFFSET;

        configure_endpoint_control(0);

        set_address(USB_DEFAULT_ADDRESS);

        // enable endpoint transfer
        USBD_DADDR |= USBD_EF;

        usb.device_state = USB_DEVICE_STATE_DEFAULT;
    } else if (usb_interrupt_flags & USBD_CTR) {
        USBD_ISTR &= ~USBD_CTR;   // clear interrupt

        unsigned char current_endpoint = usb_interrupt_flags & USBD_EP_ID_MASK;
        unsigned char no_of_bytes = USBD_COUNT_RX(current_endpoint) &
                USBD_COUNT_RX_MASK;
        unsigned char current_token;

        if (USBD_EPR(current_endpoint) & USBD_CTR_TX) {
            current_token = USB_TOKEN_IN;

            USBD_EPR(current_endpoint) &= ~USBD_CTR_TX;
        } else if (USBD_EPR(current_endpoint) & USBD_CTR_RX) {
            if (USBD_EPR(current_endpoint) & USBD_SETUP) {
                current_token = USB_TOKEN_SETUP;
            } else {
                current_token = USB_TOKEN_OUT;
            }

            USBD_EPR(current_endpoint) &= ~USBD_CTR_RX;
        }

        switch (current_token) {
            // case UIS_TOKEN_OUT: {
            //     // only ACK OUT packets for now
            //     USBFSD->UEP0_RX_CTRL ^= RB_UEP_R_TOG;

            //     break;
            // }

            case USB_TOKEN_IN: {
                switch (current_endpoint) {
                    case ENDPOINT0: {
                        if (usb.device_state == USB_DEVICE_STATE_ADDRESSED) {
                            set_address(usb.device_address);
                        }

                        break;
                    }

                    // case ENDPOINT1: {
                    //     USBHIDReport hid_report = {
                    //         .x = 10,
                    //         .y = 0,
                    //         .buttons = 0
                    //     };

                    //     endpoint1.tx_pointer = (unsigned char *)&hid_report;
                    //     endpoint1.tx_bytes_to_send = sizeof(hid_report);

                    //     // write_bytes_endpoint1();

                    //     break;
                    // }
                }

                break;
            }

            case USB_TOKEN_SETUP: {
                usb.control_stage = USB_CONTROL_STAGE_SETUP;

                copy_rx_to_buffer(current_endpoint, endpoint0_buffer,
                        no_of_bytes);
                
                usb.request.bmRequestType.transfer_direction = 
                        GET_USB_SETUP_REQUEST_TYPE_DIR(endpoint0_buffer[0]);
                usb.request.bmRequestType.type = 
                        GET_USB_SETUP_REQUEST_TYPE_TYPE(endpoint0_buffer[0]);
                usb.request.bmRequestType.recipent = 
                        GET_USB_SETUP_REQUEST_TYPE_REC(endpoint0_buffer[0]);

                usb.request.bRequest = endpoint0_buffer[1];
                usb.request.wIndex = (unsigned short)(endpoint0_buffer[4] << 8
                        | endpoint0_buffer[5]);
                usb.request.wLength = 
                        (unsigned short)((endpoint0_buffer[7] << 8)
                        | USBD_BUFF_RX_BYTE(current_endpoint, 6));

                switch (usb.request.bmRequestType.type) {
                    case USB_SETUP_REQUEST_TYPE_TYPE_STANDARD: {
                        switch (usb.request.bRequest) {
                            case SETUP_DEVICE_REQS_GET_DESCRIPTOR: {
                                usb.request.wValue = (unsigned short)
                                        (endpoint0_buffer[2] << 8 |
                                        endpoint0_buffer[3]);

                                switch (usb.request.wValue) {
                                    case DESC_TYPE_DEVICE: {
                                        usb.tx_pointer = 
                                                (unsigned char *)
                                                &device_descriptor;
                                        usb.tx_bytes_to_send = 
                                                sizeof(device_descriptor);

                                        usb.control_stage =
                                                USB_CONTROL_STAGE_DATA_IN;

                                        copy_buffer_to_tx(current_endpoint);
                                        
                                        break;
                                    }

                                    // case DESC_TYPE_CONFIGURATION: {
                                    //     usb.tx_pointer = (unsigned char *)
                                    //             &full_configuration_descriptor;
                                    //     usb.tx_bytes_to_send = 
                                    //             usb.request.wLength;

                                    //     break;
                                    // }

                                    // case DESC_TYPE_HID_REPORT: {
                                    //     usb.tx_pointer = 
                                    //             (unsigned char *)
                                    //             &hid_report_descriptor;
                                    //     usb.tx_bytes_to_send = 
                                    //             sizeof(hid_report_descriptor);

                                    //     break;
                                    // }

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

                                usb.control_stage =
                                                USB_CONTROL_STAGE_STATUS_IN;

                                copy_buffer_to_tx(current_endpoint);

                                break;
                            }

                            // case SETUP_DEVICE_REQS_SET_CONFIGURATION: {
                            //     usb.request.wValue = (unsigned short)
                            //             ((endpoint0_buffer[3] << 8) |
                            //             endpoint0_buffer[2]);

                            //     usb.tx_bytes_to_send = 0;

                            //     usb.device_state = USB_DEVICE_STATE_CONFIGURED;

                            //     break;
                            // }

                            default: {
                                usb.device_error = REQ_NOT_IMPLEMENTED;

                                break;
                            }
                        }

                        break;
                    }

                    // case USB_SETUP_REQUEST_TYPE_TYPE_CLASS: {
                    //     switch (usb.request.bRequest) {
                    //         case HID_REQS_SET_IDLE: {
                    //             usb.tx_bytes_to_send = 0;
                    //         }
                        
                    //         default: {
                    //             usb.device_error = REQ_NOT_IMPLEMENTED;

                    //             break;
                    //         }
                    //     }

                    //     break;
                    // }
                }

                // set ACK
                USBD_EPR(current_endpoint) |= USBD_STAT_RX_BIT1 |
                        USBD_STAT_TX_BIT1;
                USBD_EPR(current_endpoint) |= USBD_STAT_RX_ACK;
                USBD_EPR(current_endpoint) |= USBD_STAT_TX_ACK;
                
                // if (usb.device_error) {
                //     USBFSD->UEP0_TX_LEN = 0;
                //     USBFSD->UEP0_TX_CTRL |= TX_ANSWER_STALL;
                //     USBFSD->UEP0_TX_CTRL ^= RB_UEP_T_TOG;

                //     usb.device_error = NO_ERROR;
                // } else {
                //     write_bytes_endpoint0();
                // }

                break;
            }
        }
    }
}
