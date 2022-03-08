#ifndef _USB_CDC__H
#define _USB_CDC__H
#include "stdint.h"


#define DevEP0SIZE 0x40

#define  SET_LINE_CODING                0X20            // Configures DTE rate, stop-bits, parity, and number-of-character
#define  GET_LINE_CODING                0X21            // This request allows the host to find out the currently configured line coding.
#define  SET_CONTROL_LINE_STATE         0X22            // This request generates RS-232/V.24 style control signals.


#pragma pack(1)
typedef struct
{
    uint32_t dwDTERate;  //Data terminal rate, in bits per second.
    uint8_t bCharFormat; //Stop bits锛�0 - 1 Stop bit/1 - 1.5 Stop bits/2 - 2 Stop bits
    uint8_t bParityType; //Parity 0 - None/1 - Odd/2 - Even/3 - Mark/4 - Space
    uint8_t bDataBits;   //Data bits (5, 6, 7, 8 or 16)
} Line_Coding_t;
#pragma pack()



extern Line_Coding_t Line_Coding;

void usb_cdc_init(void);
void USB_DevTransProcess(void);







#endif









