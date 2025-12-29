#ifndef USB_HID_STANDARDS_H_
#define USB_HID_STANDARDS_H_

#include "usb_hid.h"
#include "usb_hid_usage_desktop.h"
#include "usb_hid_usage_button.h"

#define DESC_TYPE_HID                               0x21
#define DESC_TYPE_HID_REPORT                        0x22

// structure of HID descriptor
typedef struct USBHIDDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdHID;
    unsigned char bCountryCode;
    unsigned char bNumDescriptors;
    unsigned char bDescriptorType0;
    unsigned short wDescriptorLength0;
} __attribute__((__packed__)) USBHIDDescriptor;

// HID country codes
#define HID_COUNTRYCODE_NOT_SUPPORTED               0x00

// HID descriptor types
#define HID_DESC_HID_REPORT                         0x00

// HID requests
#define HID_REQS_GET_REPORT	                        0x01
#define HID_REQS_GET_IDLE	                        0x02
#define HID_REQS_GET_PROTOCOL	                    0x03
#define HID_REQS_SET_REPORT	                        0x09
#define HID_REQS_SET_IDLE	                        0x0A
#define HID_REQS_SET_PROTOCOL	                    0x0B

// size of HID report
#define HID_REPORT_BYTES                            0x03

// structure of HID report
typedef struct USBHIDReport {
    char x;
    char y;
    unsigned char buttons;
} __attribute__((__packed__)) USBHIDReport;

#endif