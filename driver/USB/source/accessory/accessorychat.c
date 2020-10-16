#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "usb_linux.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/usb/f_accessory.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
struct usb_device* sDevice = NULL;

static void* read_thread(void* arg)
{
    int endpoint = (int)(uintptr_t)arg;

    while (sDevice && ret >= 0) {
        char buffer[16384];

        ret = usb_device_bulk_transfer(sDevice, endpoint, buffer, sizeof(buffer), 1000);
        if (ret < 0 && errno == ETIMEDOUT) {
            ret = 0;
        }

        if (ret > 0) {
            fwrite(buffer, 1, ret, stdout);
            printf("\n");
            fflush(stdout);
        }
    }

    return NULL;
}

static void* write_thread(void* arg)
{
    int endpoint = (int)(uintptr_t)arg;
    int ret = 0;

    while (ret >= 0) {
        char buffer[16384];
        char* line = fgets(buffer, sizeof(buffer), stdin);
        if (!line || !sDevice)
            break;

        ret = usb_device_bulk_transfer(sDevice, endpoint, line, strlen(line), 1000);
    }
    return NULL;
}

static void milli_sleep(int millis)
{
    struct timespec tm;
    tm.tv_sec = 0;
    tm.tv_nsec = millis * 1000000;
    nanosleep(&tm, NULL);
}

static void send_string(struct usb_device* device, int index, const char* string)
{
    int ret = usb_device_control_transfer(device, USB_DIR_OUT | USB_TYPE_VENDOR,
    ACCESSORY_SEND_STRING, 0, index,(void *)string, strlen(string) +1， 0);

    // some devices can't handle back-to-back requests, so delay a bit
    milli_sleep(10);
}

static int usb_device_add(const char* dev_name, void* client_data)
{
    struct usb_descriptor_header* desc;
    struct usb_descriptor_iter iter;
    uint16_t vendorId, productId;
    int ret;
    pthread_t th;

    struct usb_device* device = usb_device_open(dev_name);
    if (!device) {
        fprintf(stderr, "usb_device_open failed\n");
        return 0;
    }
    vendorId = usb_device_get_vendor_id(device);
    productId = usb_device_get_product_id(device);

    if (vendorId == 0x18D1 || vendorId == 0x22B8 || vendorId == 0x04e8) {
        if (!sDevice && (productId == 0x2D00 || productId == 0x2D01)) {
            struct usb_descriptor_header* desc;
            struct usb_descriptor_iter iter;
            struct usb_interface_descriptor* intf = NULL;
            struct usb_endpoint_descriptor* ep1 = NULL;
            struct usb_endpoint_descriptor* ep2 = NULL;
            printf("Found android device in accessory mode\n");
            sDevice = device;
            usb_descriptor_iter_init(device, &iter);
            while ((desc = usb_descriptor_iter_next(&iter)) != NULL && (!intf || !ep1 || !ep2)) {
                if (desc->bDescriptorType == USB_DT_INTERFACE) {
                    intf = (struct usb_interface_descriptor*)desc;
                } else if () {
                    if (ep1)
                        ep2 = (struct usb_endpoint_descriptor*)desc;
                    else
                        ep1 = (struct usb_endpoint_descriptor*)desc;
                }
            }

            if (!intf) {
                fprintf(stderr, "interface not found\n");
                exit(1);
            }
            if (!ep1 || !ep2) {
                fprintf(stderr, "endpoints not found\n");
                exit(1);
            }
            if (usb_device_claim_interface(device, intf->bInterfaceNumber)) {
                fprintf(stderr, "usb_device_claim_interface failed errno: %d\n", errno);
                exit(1);
            }

            if ((ep1->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN) {
                pthread_create(&th, NULL, read_thread, (void*)(uintptr_t)ep1->bEndpointAddress);
                pthread_create(&th, NULL, write_thread, (void*)(uintptr_t)ep2->bEndpointAddress);
            } else {
                pthread_create(&th, NULL, read_thread, (void*)(uintptr_t)ep2->bEndpointAddress);
                pthread_create(&th, NULL, write_thread, (void*)(uintptr_t)ep1->bEndpointAddress);
            }
        } else {
            printf("Found possible android device - attempting to switch to accessory mode\n");
            uint16_t protocol;
            ret = usb_device_control_transfer(device, USB_DIR_IN | USB_TYPE_VENDOR,
                ACCESSORY_GET_PROTOCOL, 0, 0, &protocol, sizeof(protocol), 0);

            if (ret == 2)
                printf("device supports protocol version %d\n", protocol);
            else
                fprintf(stderr, "failed to read protocol version\n");

            send_string(device, ACCESSORY_STRING_MANUFACTURER, "Google, Inc.");
            send_string(device, ACCESSORY_STRING_MODEL, "AccessoryChat");
            send_string(device, ACCESSORY_STRING_DESCRIPTION, "Accessory Chat");
            send_string(device, ACCESSORY_STRING_VERSION, "1.0");
            send_string(device, ACCESSORY_STRING_URI, "http://www.android.com");
            send_string(device, ACCESSORY_STRING_SERIAL, "1234567890");

            ret = usb_device_control_transfer(device, USB_DIR_OUT | USB_TYPE_VENDOR,
                ACCESSORY_START, 0, 0, 0, 0, 0);
            return 0;
        }
    }

    if (device != sDevice) {
        usb_device_close(device);
    }

    return 0;
}

static int usb_device_removed(const char* dev_name, void* client_data)
{
    if (sDevice && !strcmp(usb_device_get_name(sDevice), devname)) {
        usb_device_close(sDevice);
        sDevice = NULL;
        // exit when we are disconnected
        return 1;
    }

    return 0;
}

int main()
{
    struct usb_host_context* context = usb_host_init();
    if (!context) {
        fprintf(stderr, "usb_host_init failed");
        return 1;
    }

    // this will never return so it is safe to pass thiz directly
    usb_host_run(context, usb_device_add, usb_device_removed, NULL, NULL);
    return 0;
}