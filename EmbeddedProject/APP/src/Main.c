#include <stdio.h>
#include <stdarg.h>
#include "CH57x_common.h"
#include "monitor.h"
#include "usb_cdc.h"
#include "ringbuffer.h"

#define LOG_EN

void usb_uart_comm_handle(void);

void log_printf(const char *fmt, ...);

extern ring_buffer_t usb_to_uart_rb;
extern ring_buffer_t uart_to_usb_rb;

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

/// USB<-->串口数据处理
void usb_uart_comm_handle(void) {
    uint8_t len = 0;
    static char recBuf[128] = {0};
    static char processString[64] = {0};
    static uint8_t processLen = 0;
    // USB上发
    len = ring_buffer_num_items(&uart_to_usb_rb);
    if (len != 0) {
        len = len >= 64 ? 64 : len;
        ring_buffer_dequeue_arr(&uart_to_usb_rb, (char *) pEP2_IN_DataBuf, len);
        DevEP2_IN_Deal(len);
    }
    // USB下发
    len = ring_buffer_num_items(&usb_to_uart_rb);
    if (len != 0) {
        ring_buffer_dequeue_arr(&usb_to_uart_rb, recBuf, len);
        // 以%结尾解析
        for (uint8_t i = 0; i < len; i++) {
            if (recBuf[i] != '%') {
                if (processLen < 64) {
                    processString[processLen] = recBuf[i];
                    processLen++;
                } else {
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                }
            } else {
                // processString应为"        CPU/RAM XXX"
                // 如读取错误，processString应为"PU XXX" "XXX"等
                // 提取数字思路：从最后开始提取
                processLen--;
                if (strstr(processString, "CPU") != NULL) {
                    uint8_t num = 0;
                    uint8_t numLen = 0;
                    while (('0' <= processString[processLen - numLen]) &&
                           (processString[processLen - numLen] <= '9')) {
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
                    while (('0' <= processString[processLen - numLen]) &&
                           (processString[processLen - numLen] <= '9')) {
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
    va_list args;//定义一个指针变量

    va_start (args, fmt);
    i = vsnprintf(buf, sizeof(buf), fmt, args);
    ring_buffer_queue_arr(&uart_to_usb_rb, buf, i);
    va_end (args);
#else
    (const char) *fmt;
#endif
}