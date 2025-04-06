#ifndef USB_STANDARDS_H_
#define USB_STANDARDS_H_

// structure of USB setup packet
typedef struct USBSetupRequest {
    struct {
        unsigned char recipent : 5;
        unsigned char type : 2;
        unsigned char transfer_direction : 1;       
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

// standard descriptor types
#define DESC_TYPE_DEVICE                            0x1
#define DESC_TYPE_CONFIGURATION                     0x2
#define DESC_TYPE_STRING                            0x3
#define DESC_TYPE_INTERFACE                         0x4
#define DESC_TYPE_ENDPOINT                          0x5
#define DESC_TYPE_QUALIFIER                         0x6
#define DESC_TYPE_OTHER                             0x7
#define DESC_TYPE_INTERFACEPOWER                    0x8
#define DESC_TYPE_OTG                               0x9
#define DESC_TYPE_DEBUG                             0xA
#define DESC_TYPE_INTERFACESOC                      0xB
#define DESC_TYPE_CS_INTERFACE                      0x24
#define DESC_TYPE_CS_ENDPOINT                       0x25

// structure of USB device descriptor
typedef struct USBDeviceDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
} __attribute__((__packed__)) USBDeviceDescriptor;

// USB device classes
#define DEVICE_CLASS_PER_INTERFACE                  0x00
#define DEVICE_CLASS_AUDIO                          0x01
#define DEVICE_CLASS_HID                            0x03
#define DEVICE_CLASS_PHYSICAL                       0x05
#define DEVICE_CLASS_STILL_IMAGE                    0x06
#define DEVICE_CLASS_PRINTER                        0x07
#define DEVICE_CLASS_MASS_STORAGE                   0x08
#define DEVICE_CLASS_HUB                            0x09
#define DEVICE_CLASS_CSCID                          0x0B
#define DEVICE_CLASS_CONTENT_SEC                    0x0D
#define DEVICE_CLASS_VIDEO                          0x0E
#define DEVICE_CLASS_HEALTHCARE                     0x0F
#define DEVICE_CLASS_AV                             0x10
#define DEVICE_CLASS_BILLBOARD                      0x11
#define DEVICE_CLASS_CBRIDGE                        0x12
#define DEVICE_CLASS_DIAGNOSTIC                     0xDC
#define DEVICE_CLASS_WIRELESS                       0xE0
#define DEVICE_CLASS_MISC                           0xEF
#define DEVICE_CLASS_IAD                            0xEF
#define DEVICE_CLASS_APP_SPEC                       0xFE
#define DEVICE_CLASS_VENDOR                         0xFF

// USB device subclasses
#define DEVICE_SUBCLASS_NONE                        0x00
#define DEVICE_SUBCLASS_IAD                         0x02
#define DEVICE_SUBCLASS_VENDOR                      0xFF

// USB device protocols
#define DEVICE_PROTOCOL_NONE                        0x00
#define DEVICE_PROTOCOL_IAD                         0x01
#define DEVICE_PROTOCOL_VENDOR                      0xFF

// structure of USB configuration descriptor
typedef struct USBConfigurationDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short wTotalLength;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;

    struct {
        unsigned char reserved40: 5;
        unsigned char remote_wakeup : 1;
        unsigned char self_powered : 1;
        unsigned char reserved7 : 1;
    } bmAttributes;

    unsigned char bMaxPower;    
} __attribute__((__packed__)) USBConfigurationDescriptor;

// USB configuration attributes
#define CONFIGURATION_ATTRIBUTE_NO_REMOTE_WAKEUP    0x00
#define CONFIGURATION_ATTRIBUTE_REMOTE_WAKEUP       0x01
#define CONFIGURATION_ATTRIBUTE_BUS_POWERED         0x00
#define CONFIGURATION_ATTRIBUTE_SELF_POWERED        0x01

// USB configuration power consumptions
#define CONFIGURATION_CURRENT_200mA                 100

// structure of USB interface descriptor
typedef struct USBInterfaceDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
} __attribute__((__packed__)) USBInterfaceDescriptor;

// structure of USB endpoint descriptor
typedef struct USBEndpointDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;

    struct {
        unsigned char endpoint_number : 4;
        unsigned char reserved64 : 3;
        unsigned char direction : 1;
    } bEndpointAddress;

    struct {
        unsigned char transfer_type: 2;
        unsigned char synchronization_type : 2;
        unsigned char usage_type : 2;
        unsigned char reserved76 : 2;
    } bmAttributes;

    struct {
        unsigned short packet_size : 11;
        unsigned short transaction_opportunities : 2;
        unsigned short reserved1513 : 3;
    } wMaxPacketSize;

    unsigned char bInterval;
} __attribute__((__packed__)) USBEndpointDescriptor;

// USB endpoint addressess
#define ENDPOINT_ADDRESS_OUT                        0x00
#define ENDPOINT_ADDRESS_IN                         0x01

// USB endpoint attributes
#define ENDPOINT_ATTRIBUTES_CONTROL                 0x00
#define ENDPOINT_ATTRIBUTES_ISOCHRONOUS             0x01
#define ENDPOINT_ATTRIBUTES_BULK                    0x02
#define ENDPOINT_ATTRIBUTES_INTERRUPT               0x03
#define ENDPOINT_ATTRIBUTES_NO_SYNCHRONIZATION      0x00
#define ENDPOINT_ATTRIBUTES_ASYNCHRONOUS            0x01
#define ENDPOINT_ATTRIBUTES_ADAPTIVE                0x02
#define ENDPOINT_ATTRIBUTES_SYNCHRONOUS             0x03
#define ENDPOINT_ATTRIBUTES_DATA                    0x00
#define ENDPOINT_ATTRIBUTES_FEEDBACK                0x01
#define ENDPOINT_ATTRIBUTES_FEEDBACK_DATA           0x02

// USB endpoint intervals
#define ENDPOINT_INTERVALS_5ms                      5
#define ENDPOINT_INTERVALS_1000ms                   1000

// structure of USB full configuration descriptor
typedef struct USBFullConfigurationDescriptor {
    USBConfigurationDescriptor configuration_descriptor;
    USBInterfaceDescriptor interface_descriptor;
    USBHIDDescriptor hid_descriptor;
    USBEndpointDescriptor endpoint_descriptor;
} __attribute__((__packed__)) USBFullConfigurationDescriptor;

#endif