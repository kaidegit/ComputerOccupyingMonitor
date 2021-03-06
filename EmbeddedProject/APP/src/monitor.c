//
// Created by yekai on 2022/3/8.
//

#include "monitor.h"
#include "CH57x_common.h"
#include "io_config.h"
#include "log.h"

// 由于CH571阉割了PWM模块和布线失误，这边暂时使用软件模拟PWM进行输出
// 由于本项目要求不高，属于图一乐级别，而且经过测试，同占空比的100khz与100hz的pwm对电压表显示差别不大，大约相差0.1V
// 故使用定时器定时翻转io来产生一个100hz的pwm波，每个PWM周期内设置200个比较点，定时时间为1s / 100 / 200
// 输出电压 = 3.3 * 比较值 / 200
// 由于电压表为3V版本，百分比 = 输出电压 / 3 = 3.3 / 3 * 比较值 / 200
// 比较值 = 百分比 * 200 * 3 / 3.3

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

