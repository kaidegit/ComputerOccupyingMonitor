# 基于沁恒CH571的电脑状态显示表

## 声明

本项目为复刻项目，参考的原项目为：[ShaderFallback/CpuRamGet: Esp32 monitor client (github.com)](https://github.com/ShaderFallback/CpuRamGet)和[基于STM32的电脑状态显示仪 - 立创EDA开源硬件平台 (oshwhub.com)](https://oshwhub.com/freeak_miku/CpuRamGet)

继承许可为GPLv3，继承原作者不可商用

## 存在的设计缺陷

PWM输出未接在CH571硬件PWM上。可使用CH573或使用软件模拟PWM。

当前PCB已修改为TIM脚位，但程序还是根据原管脚编写。两个版本的PCB详见附件。

## 对原项目的改进

* 使用CH571，降低成本
* 重新设计外壳，提高角度，观看数据更加舒适
* 增加灯条（虽然实际效果emmm）

## 项目功能及TODO

- [x] USB CDC虚拟串口
- [x] 串口解析AIDA64字符串
- [x] PWM输出数据至电压表
- [x] LED灯逻辑：串口接收到可解析的字符串后亮起30s
- [ ] 蓝牙串口
- [ ] MRS支持
- [ ] 显示指针控制算法

## 机械部分

机械部分使用SW2020绘制的外壳，做了无支撑打印的优化。

## 硬件部分

电气部分使用CH571作为主控，降低了成本，同时也带有一个蓝牙，可以通过蓝牙连接设备进行串口数据的传输。

## 程序部分

程序主要分为USB接收解析字符串、PWM输出、灯条控制等。

USB部分主要根据沁恒的CDC例程。（这例程居然得去论坛找。。。）主要修改USB

```c
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
                    LED_Update();
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
                    LED_Update();
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                } else {
                    processLen = 0;
                    memset(processString, 0, sizeof(processString));
                }
            }
        }
    }
```

PWM使用一个定时器模拟输出

```c
// 启用均值滤波
#define MEAN_FILTERING

#define COUNT_MAX 200
#define PWM_FREQ 100
uint16_t compare0, compare1;
uint16_t count;

void Monitor_PWM_Init() {
    compare0 = compare1 = 0;
    count = 0;
    // 初始化GPIO
    GPIOA_ModeCfg(MONITOR_CH0_Pin, GPIO_ModeOut_PP_20mA); // PA12
    GPIOA_ModeCfg(MONITOR_CH1_Pin, GPIO_ModeOut_PP_20mA); // PA13

    Monitor_Set_Usage(0, 10);
    Monitor_Set_Usage(1, 10);

    // 初始化定时器
    TMR0_TimerInit(FREQ_SYS / COUNT_MAX / PWM_FREQ);         // 设置定时时间 1s/100/200
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // 开启中断
    PFIC_EnableIRQ(TMR0_IRQn);
}

// ch为通道，0为前表PA12，1为后表PA13
// occupy为百分比，0-100
void Monitor_Set_Usage(uint8_t ch, uint16_t usage) {
    if (usage == 0) {
        usage++;
    }
    if (usage > 100) {
        return;
    }
#ifdef MEAN_FILTERING
    static uint16_t usage_pp_ch0 = 0, usage_p_ch0 = 0;
    static uint16_t usage_pp_ch1 = 0, usage_p_ch1 = 0;
    if (ch == 0) {
        usage = (usage * 7 + usage_pp_ch0 * 1 + usage_p_ch0 * 2) / 10;
        usage_pp_ch0 = usage_p_ch0;
        usage_p_ch0 = usage;
        log_printf("ch0:%d,%d,%d", usage, usage_p_ch0, usage_pp_ch0);
    } else if (ch == 1) {
        usage = (usage * 7 + usage_pp_ch1 * 1 + usage_p_ch1 * 2) / 10;
        usage_pp_ch1 = usage_p_ch1;
        usage_p_ch1 = usage;
        log_printf("ch1:%d,%d,%d", usage, usage_p_ch1, usage_pp_ch1);
    }
#endif
    usage > 100 ? usage = 100 : usage;
    if (ch == 0) {
        compare0 = (uint16_t) (usage * COUNT_MAX * 3 / 3.3 / 100);
    } else if (ch == 1) {
        compare1 = (uint16_t) (usage * COUNT_MAX * 3 / 3.3 / 100);
    }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR0_IRQHandler(void) { // TMR0 定时中断
    if (TMR0_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
        count++;
        if (count == compare0) {
            GPIOA_ResetBits(MONITOR_CH0_Pin);
        }
        if (count == compare1) {
            GPIOA_ResetBits(MONITOR_CH1_Pin);
        }
        if (count == COUNT_MAX) {
            count = 0;
            GPIOA_SetBits(MONITOR_CH0_Pin | MONITOR_CH1_Pin);
        }
    }
}
```

灯条使用一个定时器计数，串口解析成功数据后清空计数

```c
uint32_t ledSleep_count = 0;
#define SLEEP_TIME_S 30

void LED_Init() {
    GPIOB_ModeCfg(LED_Pin, GPIO_ModeOut_PP_20mA);
    LED_On();

    TMR1_TimerInit(FREQ_SYS / 100);         // 设置定时时间 1s/100
    TMR1_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // 开启中断
    PFIC_EnableIRQ(TMR1_IRQn);

    LED_Update();
}

void LED_Update(){
    LED_On();
    ledSleep_count = 0;
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR1_IRQHandler(void) { // TMR1 定时中断
    if (TMR1_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR1_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
        ledSleep_count++;
        if (ledSleep_count >= (SLEEP_TIME_S * 100)) {
            ledSleep_count = 0;
            LED_Off();
        }
    }
}
```



