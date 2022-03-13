//
// Created by yekai on 2022/3/13.
//

#include "led.h"
#include "io_config.h"
#include "CH57x_common.h"

uint32_t ledSleep_count = 0;
#define SLEEP_TIME_S 30

void LED_Init() {
    GPIOB_ModeCfg(LED_Pin, GPIO_ModeOut_PP_20mA);
    LED_On();

    TMR1_TimerInit(FREQ_SYS / 100);         // ���ö�ʱʱ�� 1s/100
    TMR1_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // �����ж�
    PFIC_EnableIRQ(TMR1_IRQn);

    LED_Update();
}

inline void LED_On() {
    GPIOB_SetBits(LED_Pin);
}

inline void LED_Off() {
    GPIOB_ResetBits(LED_Pin);
}

void LED_Update(){
    LED_On();
    ledSleep_count = 0;
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR1_IRQHandler(void) { // TMR1 ��ʱ�ж�
    if (TMR1_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR1_ClearITFlag(TMR0_3_IT_CYC_END); // ����жϱ�־
        ledSleep_count++;
        if (ledSleep_count >= (SLEEP_TIME_S * 100)) {
            ledSleep_count = 0;
            LED_Off();
        }
    }
}


