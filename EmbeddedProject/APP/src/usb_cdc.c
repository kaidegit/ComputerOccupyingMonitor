
#define DBG_TAG "USB/CDC"
#define DBG_LVL DBG_INFO

#include "usb_cdc.h"
#include "CH57x_common.h"
#include "usb_descript.h"
#include "ringbuffer.h"

// 语言描述符
const UINT8 MyLangDescr[] = {0x04, 0x03, 0x09, 0x04};
// 厂家信息 ghtech
const UINT8 MyManuInfo[] = {0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0};
// 产品信息 www.ghtech.cn
const UINT8 MyProdInfo[] = {0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '7', 0, 'x', 0};
// 序列号描述符
const UINT8 MySerDes[] = {
        0x14, 0x03,
        0x32, 0x00, 0x30, 0x00, 0x31, 0x00, 0x37, 0x00, 0x2D, 0x00,
        0x32, 0x00, 0x2D, 0x00,
        0x32, 0x00, 0x35, 0x00
};
const uint8_t CfgDesc[] = {
        0x09, 0x02, 0x43, 0x00, 0x02, 0x01, 0x00, 0xa0, 0x32, //配置描述符（两个接口）
        //以下为接口0（CDC接口）描述符
        0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00, //CDC接口描述符(一个端点)
        //以下为功能描述符
        0x05, 0x24, 0x00, 0x10, 0x01,                         //功能描述符(头)
        0x05, 0x24, 0x01, 0x00, 0x00,                         //管理描述符(没有数据类接口) 03 01
        0x04, 0x24, 0x02,
        0x02,                               //支持Set_Line_Coding、Set_Control_Line_State、Get_Line_Coding、Serial_State
        0x05, 0x24, 0x06, 0x00, 0x01,                         //编号为0的CDC接口;编号1的数据类接口
        0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0xFF,             //中断上传端点描述符
        //以下为接口1（数据接口）描述符
        0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00, //数据接口描述符
        0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,             //端点描述符
        0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,             //端点描述符
};
/**********************************************************/
extern ring_buffer_t usb_to_uart_rb;
extern ring_buffer_t uart_to_usb_rb;
UINT8 DevConfig;
UINT8 SetupReqCode;
UINT16 SetupReqLen;
const UINT8 *pDescr;
Line_Coding_t Line_Coding;
//cdc参数
const UINT8 myLineCoding[7] = {0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x08}; //初始化波特率为57600，1停止位，无校验，8数据位。

/******** 用户自定义分配端点RAM ****************************************/
__attribute__((aligned(4))) UINT8 EP0_Databuf[64 + 64 + 64]; //ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4))) UINT8 EP1_Databuf[64 + 64];      //ep1_out(64)+ep1_in(64)
__attribute__((aligned(4))) UINT8 EP2_Databuf[64 + 64];      //ep2_out(64)+ep2_in(64)
__attribute__((aligned(4))) UINT8 EP3_Databuf[64 + 64];      //ep3_out(64)+ep3_in(64)

void USB_DevTransProcess(void) {
    UINT8 len, chtype;
    UINT8 intflag, errflag = 0;

    intflag = R8_USB_INT_FG;
    if (intflag & RB_UIF_TRANSFER) {
        //LOG_D("transfer interrupt.");
        if ((R8_USB_INT_ST & MASK_UIS_TOKEN) != MASK_UIS_TOKEN) { // 非空闲
            // 分析操作令牌和端点号
            switch (R8_USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP)) {
                case UIS_TOKEN_IN: {
                    /**
                     * 控制端点数据缓冲是64字节 如果第一包没有发完64字节 则在endpoint0 in 里接着发
                     * 如何判断一包数据是否发送完成呢(数据长度>= endpoint 0 buffer length)？数据包发送完成后会发送一个长度为0的空包给主机
                    */
                    switch (SetupReqCode) {
                        case USB_GET_DESCRIPTOR: {
                            len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen; // 本次传输长度
                            memcpy(pEP0_DataBuf, pDescr, len);                          /* 加载上传数据 */
                            SetupReqLen -= len;
                            pDescr += len;
                            R8_UEP0_T_LEN = len;
                            R8_UEP0_CTRL ^= RB_UEP_T_TOG; // 翻转
                        }
                            break;
                        case USB_SET_ADDRESS: {
                            R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | SetupReqLen;
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            //LOG_D("USB_SET_ADDRESS -> %d ", R8_USB_DEV_AD);
                        }
                            break;
                        default: {
                            R8_UEP0_T_LEN = 0; // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                        }
                            break;
                    }
                }
                    break;
                    // endpoint0 out
                case UIS_TOKEN_OUT: {
                    //LOG_D("endpoint0 out -> len = %d", R8_USB_RX_LEN);
                    len = R8_USB_RX_LEN;
                    /* 类请求 且是 设置linecodeing*/
                    if (SetupReqCode == SET_LINE_CODING) {
                        if (RB_U_TOG_OK) {
                            if (pEP0_DataBuf[0] == 0xa1 && pEP0_DataBuf[1] == 0x21) {
                                Line_Coding.bCharFormat = pEP0_DataBuf[4];
                                Line_Coding.bParityType = pEP0_DataBuf[5];
                                Line_Coding.bDataBits = pEP0_DataBuf[6];
                            } else {
                                Line_Coding.dwDTERate = *(uint32_t *) pEP0_DataBuf;
                            }
//                            UART0_BaudRateCfg(Line_Coding.dwDTERate);
                            //LOG_D("set line coding dataRate : %ld , charFormat : %d parityType : %d dataBits : %d", Line_Coding.dwDTERate, Line_Coding.bCharFormat, Line_Coding.bParityType, Line_Coding.bDataBits);
                            R8_UEP0_T_LEN = 0;
                            R8_UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_ACK; // 准备上传0包
                        }
                    } else {
                        R8_UEP0_T_LEN = 0;
                        R8_UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK; //状态阶段，对IN响应NAK
                    }
                    R8_UEP0_CTRL ^= RB_UEP_R_TOG;                                     //同步标志位翻转
                }
                    break;
                case UIS_TOKEN_OUT | 1: {
                    //LOG_I("endpoint1 out -> len = %d", R8_USB_RX_LEN);
                }
                    break;
                case UIS_TOKEN_IN | 1: {
                    //LOG_I("endpoint1 in -> ");
                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                }
                    break;
                case UIS_TOKEN_OUT | 2: {
                    //LOG_D("endpoint2 out -> len = %d",R8_USB_RX_LEN);
                    if (R8_USB_INT_ST & RB_UIS_TOG_OK) {
                        // 不同步的数据包将丢弃
                        len = R8_USB_RX_LEN;
                        ring_buffer_queue_arr(&usb_to_uart_rb, pEP2_OUT_DataBuf, len);
                        // DevEP2_OUT_Deal(len);
                    }
                }
                    break;
                case UIS_TOKEN_IN | 2: {
                    // LOG_D("endpoint2 in -> ");
                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                }
                    break;
                case UIS_TOKEN_OUT | 3: {
                    //LOG_D("endpoint3 out -> ");
                    if (R8_USB_INT_ST & RB_UIS_TOG_OK) { // 不同步的数据包将丢弃
                        len = R8_USB_RX_LEN;
                        DevEP3_OUT_Deal(len);
                    }
                }
                    break;
                case UIS_TOKEN_IN | 3: {
                    //LOG_D("endpoint3 in -> ");
                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                }
                    break;
                case UIS_TOKEN_OUT | 4: {
                    //LOG_D("endpoint4 out -> ");
                    if (R8_USB_INT_ST & RB_UIS_TOG_OK) {
                        R8_UEP4_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP4_OUT_Deal(len);
                    }
                }
                    break;
                case UIS_TOKEN_IN | 4: {
                    //LOG_D("endpoint4 in -> ");
                    R8_UEP4_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                }
                    break;
                default:
                    break;
            }
            R8_USB_INT_FG = RB_UIF_TRANSFER; //写0清空中断
        }
        // Setup包处理
        if (R8_USB_INT_ST & RB_UIS_SETUP_ACT) {
            // LOG_I("setup package -> ");
            R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            SetupReqLen = pSetupReqPak->wLength;
            SetupReqCode = pSetupReqPak->bRequest;
            chtype = pSetupReqPak->bRequestType;
            len = 0;
            errflag = 0;
            // 判断请求种类 0标准请求 1 类请求 2 厂商请求 3 保留
            if ((pSetupReqPak->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
                switch (SetupReqCode) {
                    case SET_LINE_CODING: {
                        //LOG_D("SET_LINE_CODING");
                    }
                        break;
                    case GET_LINE_CODING: {
                        //LOG_D("GET_LINE_CODING");
                        pDescr = (uint8_t *) &Line_Coding;
                        len = sizeof(Line_Coding_t);
                    }
                        break;
                    case SET_CONTROL_LINE_STATE: {
                        //LOG_D("SET_CONTROL_LINE_STATE");
                        len = 0;
                    }
                        break;
                    default:
                        errflag = 0xff;
                        break;
                }
            }
                /* 标准请求 */
            else if ((pSetupReqPak->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
                // 请求类型
                switch (SetupReqCode) {
                    // 获取描述符
                    case USB_GET_DESCRIPTOR: {
                        // 描述符类型
                        switch (((pSetupReqPak->wValue) >> 8)) {
                            // 设备描述符
                            case USB_DESCR_TYP_DEVICE: {
                                pDescr = (uint8_t *) &Device_Descriptor;
                                len = Device_Descriptor.bLength;
                            }
                                break;
                                // 配置描述符
                            case USB_DESCR_TYP_CONFIG: {
                                pDescr = (uint8_t *) &Descriptor_Set;
                                len = Descriptor_Set.Configuration_Desc.wTotallLength;
                            }
                                break;
                                // 字符串描述符
                            case USB_DESCR_TYP_STRING: {
                                switch ((pSetupReqPak->wValue) & 0xff) {
                                    // 厂商字符串
                                    case 1:
                                        pDescr = MyManuInfo;
                                        len = MyManuInfo[0];
                                        break;
                                        // 产品描述字符串
                                    case 2:
                                        pDescr = MyProdInfo;
                                        len = MyProdInfo[0];
                                        break;
                                        // 设备序列号字符串
                                    case 3:
                                        pDescr = MySerDes;
                                        len = MySerDes[0];
                                        break;
                                        // 语言ID
                                    case 0:
                                        pDescr = (uint8_t *) &ID_Descriptor;
                                        len = ID_Descriptor.bLength;
                                        break;
                                    default:
                                        errflag = 0xFF; // 不支持的字符串描述符
                                        break;
                                }
                            }
                                break;
                                // 其他描述符暂不处理
                            default:
                                //LOG_E("others descriptor. %d", ((pSetupReqPak->wValue) >> 8));
                                errflag = 0xff;
                                break;
                        }
                        if (SetupReqLen > len)
                            SetupReqLen = len; //实际需上传总长度
                        len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                        memcpy(pEP0_DataBuf, pDescr, len);
                        pDescr += len;
                    }
                        break;
                        // 设置地址
                    case USB_SET_ADDRESS:
                        SetupReqLen = (pSetupReqPak->wValue) & 0xff;
                        break;
                        // 获取配置
                    case USB_GET_CONFIGURATION:
                        pEP0_DataBuf[0] = DevConfig;
                        if (SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;
                        // 设置配置 启用端点
                    case USB_SET_CONFIGURATION:
                        DevConfig = (pSetupReqPak->wValue) & 0xff; // 设置配置值
                        break;
                    case USB_CLEAR_FEATURE: {
                        if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // 端点
                        {
                            switch ((pSetupReqPak->wIndex) & 0xff) {
                                case 0x82:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x02:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                case 0x81:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x01:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                default:
                                    errflag = 0xFF; // 不支持的端点
                                    break;
                            }
                        } else
                            errflag = 0xFF;
                    }
                        break;
                    case USB_GET_INTERFACE:
                        pEP0_DataBuf[0] = 0x00;
                        if (SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;
                    case USB_GET_STATUS:
                        pEP0_DataBuf[0] = 0x00;
                        pEP0_DataBuf[1] = 0x00;
                        if (SetupReqLen > 2)
                            SetupReqLen = 2;
                        break;
                    default:
                        errflag = 0xff;
                        break;
                }
            }
            if (errflag == 0xff) {// 错误或不支持
                //                  SetupReqCode = 0xFF;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            } else {
                if (chtype & 0x80) // 上传
                {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                } else {
                    len = 0; // 下传
                }
                R8_UEP0_T_LEN = len;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 默认数据包是DATA1
                //LOG_D("setup pkg send len = %d", R8_UEP0_T_LEN);
            }
            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
    } else if (intflag & RB_UIF_BUS_RST) {
        //LOG_D("\r\nreset interrupt.");
        R8_USB_DEV_AD = 0;
        R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
        R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
        R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
        R8_USB_INT_FG = RB_UIF_BUS_RST;
    } else if (intflag & RB_UIF_SUSPEND) {
        //LOG_D("\r\nsuspend interrupt.");
        if (R8_USB_MIS_ST & RB_UMS_SUSPEND) { ;
        } // 挂起
        else { ;
        } // 唤醒
        R8_USB_INT_FG = RB_UIF_SUSPEND;
    } else {
        // LOG_E("unknow interrupt.");
        R8_USB_INT_FG = intflag;
    }
}

void usb_cdc_init(void) {
    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;
    USB_DeviceInit();
    descript_init();
    Line_Coding.dwDTERate = 115200; //Data terminal rate, in bits per second.
    Line_Coding.bCharFormat = 0;    //Stop bits：0 - 1 Stop bit/1 - 1.5 Stop bits/2 - 2 Stop bits
    Line_Coding.bParityType = 0;    //Parity 0 - None/1 - Odd/2 - Even/3 - Mark/4 - Space
    Line_Coding.bDataBits = 8;      //Data bits (5, 6, 7, 8 or 16)
    PFIC_EnableIRQ(USB_IRQn);
}

void DevEP1_OUT_Deal(UINT8 l) { /* 用户可自定义 */
    UINT8 i;

    for (i = 0; i < l; i++) {
        pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
    }
    DevEP1_IN_Deal(l);
}

void DevEP2_OUT_Deal(UINT8 l) { /* 用户可自定义 */
    UINT8 i;

    for (i = 0; i < l; i++) {
        pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
    }
    DevEP2_IN_Deal(l);
}

void DevEP3_OUT_Deal(UINT8 l) { /* 用户可自定义 */
    UINT8 i;

    for (i = 0; i < l; i++) {
        pEP3_IN_DataBuf[i] = ~pEP3_OUT_DataBuf[i];
    }
    DevEP3_IN_Deal(l);
}

void DevEP4_OUT_Deal(UINT8 l) { /* 用户可自定义 */
    UINT8 i;

    for (i = 0; i < l; i++) {
        pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
    }
    DevEP4_IN_Deal(l);
}
