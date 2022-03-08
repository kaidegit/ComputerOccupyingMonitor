#include "shell_cmd.h"
#include "CH57x_common.h"

void shell_write_cb(char data)
{
  UART1_SendString(&data, 1);
}

void shell_cmd_reboot(void)
{
  SYS_ResetExecute();
}












