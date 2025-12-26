#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include "usb_standards.h"
#include "usb_hid_standards.h"

const USBDeviceDescriptor device_descriptor = {
    .bLength = sizeof(USBDeviceDescriptor),
    .bDescriptorType = DESC_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = DEVICE_CLASS_PER_INTERFACE,
    .bDeviceSubClass = DEVICE_SUBCLASS_NONE,
    .bDeviceProtocol = DEVICE_PROTOCOL_NONE,
    .bMaxPacketSize = 64,
    .idVendor = 0x6666,
    .idProduct = 0x1111,
    .bcdDevice = 0x0100,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

const unsigned char hid_report_descriptor[] = {
	HID_USAGE_PAGE(HID_PAGE_DESKTOP),
	HID_USAGE(HID_DESKTOP_MOUSE),
	HID_COLLECTION(HID_APPLICATION_COLLECTION),
		HID_USAGE(HID_DESKTOP_POINTER),
		HID_COLLECTION(HID_PHYSICAL_COLLECTION),
			HID_USAGE(HID_DESKTOP_X),
			HID_USAGE(HID_DESKTOP_Y),
			HID_LOGICAL_MINIMUM(-127),
			HID_LOGICAL_MAXIMUM(127),
			HID_REPORT_SIZE(8),
			HID_REPORT_COUNT(2),
			HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

			HID_USAGE_PAGE(HID_PAGE_BUTTON),
			HID_USAGE_MINIMUM(1),
			HID_USAGE_MAXIMUM(3),
			HID_LOGICAL_MINIMUM(0),
			HID_LOGICAL_MAXIMUM(1),
			HID_REPORT_SIZE(1),
			HID_REPORT_COUNT(3),
			HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_REPORT_SIZE(1),
			HID_REPORT_COUNT(5),
			HID_INPUT(HID_IOF_CONSTANT),
		HID_END_COLLECTION,
	HID_END_COLLECTION
};

const USBFullConfigurationDescriptor full_configuration_descriptor = {
    .configuration_descriptor = {
        .bLength = sizeof(USBConfigurationDescriptor),
        .bDescriptorType = DESC_TYPE_CONFIGURATION,
        .wTotalLength = (sizeof(USBConfigurationDescriptor) +
                sizeof(USBInterfaceDescriptor) +
                sizeof(USBHIDDescriptor) +
                sizeof(USBEndpointDescriptor)),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,

        .bmAttributes = {
            .reserved40 = 0,
            .remote_wakeup = CONFIGURATION_ATTRIBUTE_NO_REMOTE_WAKEUP,
            .self_powered = CONFIGURATION_ATTRIBUTE_BUS_POWERED,
            .reserved7 = 1
        },

        .bMaxPower = CONFIGURATION_CURRENT_200mA
    },

    .interface_descriptor = {
        .bLength = sizeof(USBInterfaceDescriptor),
        .bDescriptorType = DESC_TYPE_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = DEVICE_CLASS_HID,
        .bInterfaceSubClass = DEVICE_SUBCLASS_NONE,
        .bInterfaceProtocol = DEVICE_PROTOCOL_NONE,
        .iInterface = 0
    },

    .hid_descriptor = {
        .bLength = sizeof(USBHIDDescriptor),
        .bDescriptorType = DESC_TYPE_HID,
        .bcdHID = 0x0110,
        .bCountryCode = HID_COUNTRYCODE_NOT_SUPPORTED,
        .bNumDescriptors = 1,
        .bDescriptorType0 = DESC_TYPE_HID_REPORT,
        .wDescriptorLength0 = sizeof(hid_report_descriptor)
    },

    .endpoint_descriptor = {
        .bLength = sizeof(USBEndpointDescriptor),
        .bDescriptorType = DESC_TYPE_ENDPOINT,

        .bEndpointAddress = {
            .endpoint_number = 1,
            .reserved64 = 0,
            .direction = ENDPOINT_ADDRESS_IN
        },
    
        .bmAttributes = {
            .transfer_type = ENDPOINT_ATTRIBUTES_INTERRUPT,
            .synchronization_type = 0,
            .usage_type = ENDPOINT_ATTRIBUTES_DATA,
            .reserved76 = 0
        },
    
        .wMaxPacketSize = {
            .packet_size = 64,
            .transaction_opportunities = 0,
            .reserved1513 = 0
        },

        .bInterval = ENDPOINT_INTERVALS_100ms
    }
};

// enum holding indexes of string descriptors
typedef enum USBStringDescriptorIndex {
    STRING_DESCRIPTOR0,
    STRING_DESCRIPTOR_MANUFACTURER,
    STRING_DESCRIPTOR_PRODUCT,
    STRING_DESCRIPTOR_SERIAL_NUMBER
} USBStringDescriptorIndex;

const USBStringDescriptor0 string_descriptor0 = {
    .bLength = sizeof(USBStringDescriptor0),
    .bDescriptorType = DESC_TYPE_STRING,
    .wLANGID0 = STRING_LANGID_ENGLISH_US
};

const USBStringDescriptor string_descriptor_manufacturer = {
    .bLength = 2 + 2 * 8,
    .bDescriptorType = DESC_TYPE_STRING,
    .bString = u"m-fudala"
};

const USBStringDescriptor string_descriptor_product = {
    .bLength = 2 + 2 * 14,
    .bDescriptorType = DESC_TYPE_STRING,
    .bString = u"CH32V203 mouse"
};

const USBStringDescriptor string_descriptor_serial_number = {
    .bLength = 2 + 2 * 3,
    .bDescriptorType = DESC_TYPE_STRING,
    .bString = u"1.0"
};

#endif