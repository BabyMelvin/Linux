#include <stdio.h>
#include <string.h>
#include <iostream>
#include "conio.h"
#include "lusb0_usb.h"

#pragma comment(lib, "libusb.lib")

#define m_dev_VID 0x1483 /*Vendor id of m_dev*/
#define m_dev_PID 0x5751 /*Product id of m_dev*/
#define EP1_OUT_SIZE 63  //可根据设备修改大小
#define EP1_IN_SIZE  63

int main (int argc, char *argv[])
{
    struct usb_device *m_dev = NULL;
    struct usb_dev_handle *m_dev_handle;
    char str[64];

    memset(str, 0, sizeof(str));

    usb_init();
    usb_find_busses();
    usb_find_devices();

    struct usb_bus *bus;

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        struct usb_devices *dev;
        for (dev = bus->device; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == m_dev_VID && 
                    dev->descriptor.idProduct == m_dev_PID) {
                m_dev = dev;
            }
        }
    }

    if (!m_dev) {
        printf("m_dev not found\n");
    }

    m_dev_handle = usb_open(m_dev);
    if (!m_dev_handle) {
        printf("could not open m_dev\n");
        return 1;
    }

    printf("设备打开成功\n");
    if (usb_set_configuration(m_dev_handle, 1) < 0) {
        printf("could not set configuraion\n");
        usb_close(m_dev_handle);
    }

    /* claim_interface 0 注册于操作系统通讯的接口0*/
    if (usb_claim_interface(m_dev_handle, 0) < 0) {
        printf("Could not calim interface\n");
        usb_close(m_dev_handle);
        return 1;
    }

    // 获取端点的地址
    int ep = m_dev->config->interface->altsetting->endpoint->bEndpointAddress;
    int EP_IN = 0;
    int EP_OUT = 0;
    if (ep > 0x0f) {
        EP_IN = ep;
        EP_OUT = ep - 0x80;
    } else {
        EP_OUT = ep;
        EP_IN = ep + 0x80;
    }

    printf("EP_IN:0x%02x, EP_OUT:0x%02\n", EP_IN, EP_OUT);
    char WriteTestData[2048] = {0};
    char ReadTestData[2048] = {0};

    for (int i; i < 2048; i++) {
        WriteTestData[i] = i;
    }

    // 端点1写入数据
    int ret;
    ret = usb_writen(m_dev_handle, EP_OUT, WriteTestData, EP1_OUT_SIZE,500);
    if (ret != EP1_OUT_SIZE) {
        printf("端点1写入诗句失败:%d\n", ret);
        return 1;
    } else {
        printf("端点1写入数据成功!\n");
    }

    // 端点1读取数据
    ret = usb_bulk_read(m_dev_handle, EP_IN, ReadTestData, EP1_IN_SIZE, 500);
    if (ret != EP1_IN_SIZE) {
        printf("端点1 读取数据失败 %d\n", ret);
        return 1;
    } else {
        printf("端点1读取读取数据成功");
        return 0
    }

    usb_release_interface(m_dev_handle, 0);
    usb_close(m_dev_handle);
    printf("设备关闭");

    return 0;
}
