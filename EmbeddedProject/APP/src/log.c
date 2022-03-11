//
// Created by yekai on 2022/3/12.
//

#include "log.h"
#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "ringbuffer.h"

extern ring_buffer_t usb_to_uart_rb;
extern ring_buffer_t uart_to_usb_rb;

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
