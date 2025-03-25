#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include "usb_standards.h"

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
    .iManufacturer = 0,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1
};

const USBConfigurationDescriptor configuration_descriptor = {
    .bLength = sizeof(USBConfigurationDescriptor),
    .bDescriptorType = DESC_TYPE_CONFIGURATION,
    .wTotalLength = (sizeof(USBConfigurationDescriptor) +
            sizeof(USBInterfaceDescriptor) + sizeof(USBEndpointDescriptor)),
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
        .wDescriptorLength0 = 0
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

        .bInterval = ENDPOINT_INTERVALS_5ms
    }
};

#endif