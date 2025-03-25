#ifndef USB_HID_STANDARDS_H_
#define USB_HID_STANDARDS_H_

#define DESC_TYPE_HID                               0x21
#define DESC_TYPE_HID_REPORT                        0x22

// structure of USB HID descriptor
typedef struct USBHIDDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdHID;
    unsigned char bCountryCode;
    unsigned char bNumDescriptors;
    unsigned char bDescriptorType0;
    unsigned short wDescriptorLength0;
} __attribute__((__packed__)) USBHIDDescriptor;

// USB HID country codes
#define HID_COUNTRYCODE_NOT_SUPPORTED               0x00

// USB HID descriptor types
#define HID_DESC_HID_REPORT                         0x00

#endif