#include "my_uart.h"

#include "CH57x_common.h"

ring_buffer_t usb_to_uart_rb;
ring_buffer_t uart_to_usb_rb;


void uart0_init(void)
{
  GPIOB_SetBits(GPIO_Pin_7);
  GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
  GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);
  UART0_DefInit(); // 115200 8 n 1
  UART0_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
  UART0_ByteTrigCfg(UART_1BYTE_TRIG);

  ring_buffer_init(&usb_to_uart_rb);
  ring_buffer_init(&uart_to_usb_rb);
}


__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode"))) void
UART0_IRQHandler(void)
{
  uint8_t i;
  switch (UART0_GetITFlag())
  {
  case UART_II_LINE_STAT: // 线路状态错误
  {
    UART0_GetLinSTA();
    break;
  }

  case UART_II_RECV_RDY: // 数据达到设置触发点
  {
    // 把收到的数据放到缓冲区里
    ring_buffer_queue(&uart_to_usb_rb,UART0_RecvByte());
  }
  break;

  case UART_II_RECV_TOUT: // 接收超时，暂时一帧数据接收完成

    break;

  case UART_II_THR_EMPTY: // 发送缓存区空，可继续发送
    break;

  case UART_II_MODEM_CHG: // 只支持串口0
    break;

  default:
    break;
  }
}











