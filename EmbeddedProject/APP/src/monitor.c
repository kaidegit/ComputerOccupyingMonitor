//
// Created by yekai on 2022/3/8.
//

#include "monitor.h"
#include "CH57x_common.h"
#include "io_config.h"
#include "log.h"

// ����CH571�˸���PWMģ��Ͳ���ʧ�������ʱʹ�����ģ��PWM�������
// ���ڱ���ĿҪ�󲻸ߣ�����ͼһ�ּ��𣬶��Ҿ������ԣ�ͬռ�ձȵ�100khz��100hz��pwm�Ե�ѹ����ʾ��𲻴󣬴�Լ���0.1V
// ��ʹ�ö�ʱ����ʱ��תio������һ��100hz��pwm����ÿ��PWM����������200���Ƚϵ㣬��ʱʱ��Ϊ1s / 100 / 200
// �����ѹ = 3.3 * �Ƚ�ֵ / 200
// ���ڵ�ѹ��Ϊ3V�汾���ٷֱ� = �����ѹ / 3 = 3.3 / 3 * �Ƚ�ֵ / 200
// �Ƚ�ֵ = �ٷֱ� * 200 * 3 / 3.3

// ���þ�ֵ�˲�
#define MEAN_FILTERING

#define COUNT_MAX 200
#define PWM_FREQ 100
uint16_t compare0, compare1;
uint16_t count;

void Monitor_PWM_Init() {
    compare0 = compare1 = 0;
    count = 0;
    // ��ʼ��GPIO
    GPIOA_ModeCfg(MONITOR_CH0_Pin, GPIO_ModeOut_PP_20mA); // PA12
    GPIOA_ModeCfg(MONITOR_CH1_Pin, GPIO_ModeOut_PP_20mA); // PA13

    Monitor_Set_Usage(0, 10);
    Monitor_Set_Usage(1, 10);

    // ��ʼ����ʱ��
    TMR0_TimerInit(FREQ_SYS / COUNT_MAX / PWM_FREQ);         // ���ö�ʱʱ�� 1s/100/200
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // �����ж�
    PFIC_EnableIRQ(TMR0_IRQn);
}

// chΪͨ����0Ϊǰ��PA12��1Ϊ���PA13
// occupyΪ�ٷֱȣ�0-100
void Monitor_Set_Usage(uint8_t ch, uint16_t usage) {
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
void TMR0_IRQHandler(void) { // TMR0 ��ʱ�ж�
    if (TMR0_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END); // ����жϱ�־
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

