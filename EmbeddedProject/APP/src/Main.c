/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description 		   : PWM4-11������ʾ
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include <stdio.h>
#include "CH57x_common.h"
#include "monitor.h"
#include "usb_cdc.h"
#include "ringbuffer.h"

#define LOG_EN

void usb_uart_comm_handle(void);

void log_printf(const char *fmt, ...);

extern ring_buffer_t usb_to_uart_rb;
extern ring_buffer_t uart_to_usb_rb;

/*********************************************************************
 * @fn      main
 *
 * @brief   ������
 *
 * @return  none
 */
int main() {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    ring_buffer_init(&usb_to_uart_rb);
    ring_buffer_init(&uart_to_usb_rb);
    usb_cdc_init();

    Monitor_PWM_Init();

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        DelayMs(10);
        usb_uart_comm_handle();
    }
#pragma clang diagnostic pop
}

static uint8_t pow_u8(uint8_t base, uint8_t power) {
    if (power == 0) {
        return 1;
    }
    uint8_t ans = 1;
    for (uint8_t i = 0; i < power; i++) {
        ans = ans * base;
    }
    return ans;
}

/// USB<-->�������ݴ���
void usb_uart_comm_handle(void) {
    uint8_t len = 0;
    static char recBuf[128] = {0};
    static char processString[64] = {0};
    static uint8_t processLen = 0;
    // USB�Ϸ�
    len = ring_buffer_num_items(&uart_to_usb_rb);
    if (len != 0) {
        len = len >= 64 ? 64 : len;
        ring_buffer_dequeue_arr(&uart_to_usb_rb, (char *) pEP2_IN_DataBuf, len);
        DevEP2_IN_Deal(len);
    }
    // USB�·�
    len = ring_buffer_num_items(&usb_to_uart_rb);
    if (len != 0) {
        ring_buffer_dequeue_arr(&usb_to_uart_rb, recBuf, len);
        // ��%��β����
        for (uint8_t i = 0; i < len; i++) {
            if (recBuf[i] != '%') {
                if (processLen < 64) {
                    processLen++;
                    processString[processLen] = recBuf[i];
                } else {
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                }
            } else {
                // processStringӦΪ"        CPU/RAM XXX"
                // ���ȡ����processStringӦΪ"PU XXX" "XXX"��
                // ��ȡ����˼·�������ʼ��ȡ
                if (strstr(processString, "CPU") != NULL) {
                    uint8_t num = 0;
                    uint8_t numLen = 0;
                    while (('0' < processString[processLen - numLen]) &&
                           (processString[processLen - numLen] < '9')) {
                        num += (processString[processLen - numLen] - '0') * pow_u8(10, numLen);
                        numLen++;
                        if (numLen == 3) {
                            break;
                        }
                    }
                    log_printf("CPU:%d\r\n", (int) num);
                    Monitor_Set_Usage(0, num);
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                } else if (strstr(processString, "RAM") != NULL) {
                    uint8_t num = 0;
                    uint8_t numLen = 0;
                    while (('0' < processString[processLen - numLen]) &&
                           (processString[processLen - numLen] < '9')) {
                        num += (processString[processLen - numLen] - '0') * pow_u8(10, numLen);
                        numLen++;
                        if (numLen == 3) {
                            break;
                        }
                    }
                    log_printf("RAM:%d\r\n", (int) num);
                    Monitor_Set_Usage(1, num);
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                } else {
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                }
            }
        }
    }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void USB_IRQHandler(void) {
    USB_DevTransProcess();
}

void log_printf(const char *fmt, ...) {
#ifdef LOG_EN
    uint16_t i;
    char buf[256];

    va_list arg = (va_list) ((char *) (&fmt) + 4);
    i = vsprintf(buf, fmt, arg);
    for (uint16_t j = 0; j < i; j++) {
        ring_buffer_queue(&uart_to_usb_rb, buf[i]);
    }
#else
    (const char) *fmt;
#endif
}