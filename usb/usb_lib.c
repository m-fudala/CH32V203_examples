#include "usb_lib.h"
#include "usb_descriptors.h"

USB usb;
USBEndpoint1 endpoint1;

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
}

void set_address(unsigned char address) {
    if (address) {
        USBD_DADDR |= address;
    } else {
        // set default address
        USBD_DADDR &= ~(USBD_ADD_MASK);
    }
}

void configure_endpoint0(void) {
    USBD_COUNT_RX(0) = USBD_NUMBLOCK_64;
    USBD_ADDR_RX(0) = 0x00C0;
    USBD_ADDR_TX(0) = 0x0080;
    USBD_EPR(0) = USBD_STAT_RX_ACK | USBD_EPTYPE_CONTROL | USBD_STAT_TX_NAK;
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
    // read interrupt flags
    volatile unsigned int usb_interrupt_flags = USBD_ISTR;

    if (usb_interrupt_flags & USBD_RESET) {
        // initialize packet buffer description table
        USBD_BTABLE = USBD_BTABLE_OFFSET;

        configure_endpoint0();

        set_address(USB_DEFAULT_ADDRESS);

        // enable endpoint transfer
        USBD_DADDR |= USBD_EF;

        usb.device_state = USB_DEVICE_STATE_DEFAULT;

        USBD_ISTR &= ~USBD_RESET;     // clear interrupt
    } else if (usb_interrupt_flags & USBD_CTR) {
        // int current_token = (usb_interrupt_status & MASK_UIS_TOKEN) >> 4;
        // int current_endpoint = usb_interrupt_status & MASK_UIS_ENDP;
        // int no_of_bytes = USBFSD->RX_LEN;

        // switch (current_token) {
        //     case UIS_TOKEN_OUT: {
        //         // only ACK OUT packets for now
        //         USBFSD->UEP0_RX_CTRL ^= RB_UEP_R_TOG;

        //         break;
        //     }

        //     case UIS_TOKEN_IN: {
        //         switch (current_endpoint) {
        //             case ENDPOINT0: {
        //                 if (usb.device_state == USB_DEVICE_STATE_DEFAULT) {
        //                     set_address(usb.device_address);
        //                 }

        //                 break;
        //             }

        //             case ENDPOINT1: {
        //                 USBHIDReport hid_report = {
        //                     .x = 10,
        //                     .y = 0,
        //                     .buttons = 0
        //                 };

        //                 endpoint1.tx_pointer = (unsigned char *)&hid_report;
        //                 endpoint1.tx_bytes_to_send = sizeof(hid_report);

        //                 // write_bytes_endpoint1();

        //                 break;
        //             }
        //         }

        //         break;
        //     }

        //     case UIS_TOKEN_SETUP: {
        //         USBFSD->UEP0_RX_CTRL |= RX_ANSWER_NAK;
                
        //         usb.request.bmRequestType.transfer_direction = 
        //                 GET_USB_SETUP_REQUEST_TYPE_DIR(endpoint0_buffer[0]);
        //         usb.request.bmRequestType.type = 
        //                 GET_USB_SETUP_REQUEST_TYPE_TYPE(endpoint0_buffer[0]);
        //         usb.request.bmRequestType.recipent = 
        //                 GET_USB_SETUP_REQUEST_TYPE_REC(endpoint0_buffer[0]);

        //         usb.request.bRequest = endpoint0_buffer[1];
        //         usb.request.wIndex = (unsigned short)
        //                 ((endpoint0_buffer[4] << 8) | endpoint0_buffer[5]);
        //         usb.request.wLength = (unsigned short)
        //                 ((endpoint0_buffer[7] << 8) | endpoint0_buffer[6]);

        //         switch (usb.request.bmRequestType.type) {
        //             case USB_SETUP_REQUEST_TYPE_TYPE_STANDARD: {
        //                 switch (usb.request.bRequest) {
        //                     case SETUP_DEVICE_REQS_GET_DESCRIPTOR: {
        //                         usb.request.wValue = (unsigned short)
        //                                 ((endpoint0_buffer[2] << 8) |
        //                                 endpoint0_buffer[3]);

        //                         switch (usb.request.wValue) {
        //                             case DESC_TYPE_DEVICE: {
        //                                 usb.tx_pointer = 
        //                                         (unsigned char *)
        //                                         &device_descriptor;
        //                                 usb.tx_bytes_to_send = 
        //                                         sizeof(device_descriptor);
                                        
        //                                 break;
        //                             }

        //                             case DESC_TYPE_CONFIGURATION: {
        //                                 usb.tx_pointer = (unsigned char *)
        //                                         &full_configuration_descriptor;
        //                                 usb.tx_bytes_to_send = 
        //                                         usb.request.wLength;

        //                                 break;
        //                             }

        //                             case DESC_TYPE_HID_REPORT: {
        //                                 usb.tx_pointer = 
        //                                         (unsigned char *)
        //                                         &hid_report_descriptor;
        //                                 usb.tx_bytes_to_send = 
        //                                         sizeof(hid_report_descriptor);

        //                                 break;
        //                             }

        //                             // case DESC_TYPE_STRING: {
        //                             //     switch(usb.request.wIndex) {
        //                             //         case STRING_DESCRIPTOR0: {
        //                             //             usb.tx_pointer = 
        //                             //                 (unsigned char *)
        //                             //                 &string_descriptor0;
        //                             //             usb.tx_bytes_to_send = 
        //                             //                 string_descriptor0.bLength;

        //                             //                 string_req++;
                                                
        //                             //             break;
        //                             //         }

        //                             //         case STRING_DESCRIPTOR_MANUFACTURER: {
        //                             //             usb.tx_pointer = 
        //                             //                 (unsigned char *)
        //                             //                 &string_descriptor_manufacturer;
        //                             //             usb.tx_bytes_to_send = 
        //                             //                 string_descriptor_manufacturer.bLength;
                                                
        //                             //             break;
        //                             //         }

        //                             //         case STRING_DESCRIPTOR_PRODUCT: {
        //                             //             usb.tx_pointer = 
        //                             //                 (unsigned char *)
        //                             //                 &string_descriptor_product;
        //                             //             usb.tx_bytes_to_send = 
        //                             //                 string_descriptor_product.bLength;
                                                
        //                             //             break;
        //                             //         }

        //                             //         case STRING_DESCRIPTOR_SERIAL_NUMBER: {
        //                             //             usb.tx_pointer = 
        //                             //                 (unsigned char *)
        //                             //                 &string_descriptor_serial_number;
        //                             //             usb.tx_bytes_to_send = 
        //                             //                 string_descriptor_serial_number.bLength;
                                                
        //                             //             break;
        //                             //         }
        //                             //     }

        //                             //     break;
        //                             // }
        //                         }
                                
        //                         break;
        //                     }

        //                     case SETUP_DEVICE_REQS_SET_ADDRESS: {
        //                         usb.request.wValue = (unsigned short)
        //                                 ((endpoint0_buffer[3] << 8) |
        //                                 endpoint0_buffer[2]);

        //                         // save received address
        //                         usb.device_address = usb.request.wValue;

        //                         usb.tx_bytes_to_send = 0;

        //                         usb.device_state = USB_DEVICE_STATE_ADDRESSED;

        //                         break;
        //                     }

        //                     case SETUP_DEVICE_REQS_SET_CONFIGURATION: {
        //                         usb.request.wValue = (unsigned short)
        //                                 ((endpoint0_buffer[3] << 8) |
        //                                 endpoint0_buffer[2]);

        //                         usb.tx_bytes_to_send = 0;

        //                         usb.device_state = USB_DEVICE_STATE_CONFIGURED;

        //                         break;
        //                     }

        //                     default: {
        //                         usb.device_error = REQ_NOT_IMPLEMENTED;

        //                         break;
        //                     }
        //                 }

        //                 break;
        //             }

        //             case USB_SETUP_REQUEST_TYPE_TYPE_CLASS: {
        //                 switch (usb.request.bRequest) {
        //                     case HID_REQS_SET_IDLE: {
        //                         usb.tx_bytes_to_send = 0;
        //                     }
                        
        //                     default: {
        //                         usb.device_error = REQ_NOT_IMPLEMENTED;

        //                         break;
        //                     }
        //                 }

        //                 break;
        //             }
        //         }

        //         USBFSD->UEP0_RX_CTRL &= RX_ANSWER_ACK;

        //         if (usb.device_error) {
        //             USBFSD->UEP0_TX_LEN = 0;
        //             USBFSD->UEP0_TX_CTRL |= TX_ANSWER_STALL;
        //             USBFSD->UEP0_TX_CTRL ^= RB_UEP_T_TOG;

        //             usb.device_error = NO_ERROR;
        //         } else {
        //             write_bytes_endpoint0();
        //         }

        //         break;
        //     }
        // }

        USBD_ISTR &= ~USBD_CTR;   // clear interrupt
    }
}
