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

union USBDeviceDescriptorUnion device_descriptor_union = {
    .device_descriptor_u = device_descriptor
};

#endif