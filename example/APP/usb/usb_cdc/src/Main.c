/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 模拟CDC设备，实现USB和串口0直接的数据互传
 *******************************************************************************/
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include "CH57x_common.h"
#include "usb_cdc.h"
#include "my_uart.h"

unsigned int systick = 0;

unsigned int get_systick(void)
{
  return systick;
}

/**
 * USB<-->串口数据处理
*/
void usb_uart_comm_handle(void)
{
  uint8_t len = 0;
  static uint8_t tempbuf[128] = {0};
  len = ring_buffer_num_items(&usb_to_uart_rb);
  if (len != 0)
  {
    ring_buffer_dequeue_arr(&usb_to_uart_rb, tempbuf, len);
    UART0_SendString(tempbuf, len);
  }

  len = ring_buffer_num_items(&uart_to_usb_rb);
  if (len != 0)
  {
    len = len >= 64 ? 64 : len;
    ring_buffer_dequeue_arr(&uart_to_usb_rb, pEP2_IN_DataBuf, len);
    DevEP2_IN_Deal(len);
  }
}

int main()
{
  uint8_t uart1Recv = 0;
  char data;
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  uart0_init();。
  usb_cdc_init();
  PFIC_EnableIRQ(USB_IRQn);
  PFIC_EnableIRQ(UART0_IRQn);
  for (;;)
  {
    DelayMs(10);
    usb_uart_comm_handle();
  }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode"))) void
USB_IRQHandler(void) /* USB中断服务程序,使用寄存器组1 */
{
  USB_DevTransProcess();
}

