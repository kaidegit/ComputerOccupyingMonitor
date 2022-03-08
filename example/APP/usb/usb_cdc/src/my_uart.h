#ifndef _MY_UART__H
#define _MY_UART__H

#include "ringbuffer.h"
extern ring_buffer_t usb_to_uart_rb;
extern ring_buffer_t uart_to_usb_rb;

void uart0_init(void);



#endif









