#ifndef _USB_DESCRIPT__H
#define _USB_DESCRIPT__H

#include <stdint.h>

/**
 * USB有5钟常见的描述符
*/
#define DEVICE_DESCRIPTOR 0x01
#define CONFIGURATION_DESCRIPTOR 0x02
#define STRING_DESCRIPTOR 0x03
#define INTERFACE_DESCRIPTOR 0x04
#define ENDPOINT_DESCRIPTOR 0x05
#define CS_INTERFACE 0x24

#pragma pack(1)
// 设备描述符
typedef struct
{
    uint8_t bLength;            //该描述符结构体大小
    uint8_t bDescriptorType;    //描述符类型
    uint16_t bcdUSB;            //USB版本号
    uint8_t bDeviceClass;       //设备类代码（由USB官方分配）
    uint8_t bDeviceSubClass;    //子类代码（由USB官方分配）
    uint8_t bDeviceProtocol;    //设备协议代码（由USB官方分配）
    uint8_t bMaxPacketSize0;    //端点0的最大包大小（有效大小为8,16,32,64）
    uint16_t idVendor;          //生产厂商编号（由USB官方分配）
    uint16_t idProduct;         //产品编号（制造厂商分配）
    uint16_t bcdDevice;         //设备出厂编号
    uint8_t iManufacturer;      //设备厂商字符串索引
    uint8_t iProduct;           //产品描述字符串索引
    uint8_t iSerialNumber;      //设备序列号字符串索引
    uint8_t bNumConfigurations; //当前速度下能支持的配置数量
} Device_Descriptor_t;

// 配置描述符
typedef struct
{
    uint8_t bLength;             //描述符长度
    uint8_t bDescriptorType;     //描述符类型
    uint16_t wTotallLength;      //配置描述符集合总长度
    uint8_t bNumInterfaces;      //配置支持的接口数
    uint8_t bConfigurationValue; //该配置的值
    uint8_t iConfiguration;      //描述该配置的字符串索引值
    uint8_t bmAttributes;        //供电模式选择．Bit4-0保留，D7:				总线供电，D6:自供电，D5:远程唤醒
    uint8_t bMaxPower;           //设备需要的最大电流 1LSB = 2mA
} Configuration_Descriptor_t;

// 接口描述符
typedef struct
{
    uint8_t bLength;            //描述符大小．
    uint8_t bDescriptorType;    //接口描述符类型．
    uint8_t bInterfaceNumber;   //该接口的编号．
    uint8_t bAlternateSetting;  //用于为上一个字段选择可供替换的位置．即备用的接口描述符标号．
    uint8_t bNumEndpoint;       //使用的端点数目．端点０除外．
    uint8_t bInterfaceClass;    //类型代码．
    uint8_t bInterfaceSubClass; //子类型代码．
    uint8_t bInterfaceProtocol; //协议代码．
    uint8_t iInterface;         //字符串描述符的索引
} Interface_Descriptor_t;

// 端点描述符
typedef struct
{
    uint8_t bLength;         //描述符大小．
    uint8_t bDescriptorType; //接口描述符类型．
    uint8_t bEndpointType;   //USB设备的端点地址．Bit7，方向，对于控制端点							可以忽略，1/0:IN/OUT．Bit6-4，保留．BIt3-0：端点号．
    uint8_t bmAttributes;    //端点属性．Bit7-2，保留．BIt1-0：00控制，01同步，02批量，03					中断．
    uint16_t wMaxPacketSize; //本端点接收或发送的最大信息包大小．
    uint8_t bInterval;       //轮询数据传送端点的时间间隔．对于批量传送和控制传送的端点忽略			．对于同步传送的端点，必须为１，对于中断传送的端点，范围为１－２５５．
} Endpoint_Descriptor_t;

// 语言ID描述符
typedef struct
{
    uint8_t bLength;         //描述符大小．
    uint8_t bDescriptorType; //接口描述符类型．固定为0x03．
    uint16_t ID;             //语言编码．
} ID_Descriptor_t;

typedef struct
{
    uint8_t bFunctionLength;    //描述符长度
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x00
    uint16_t bcdCDC;            //0x0110
} Header_Func_Desc_t;

typedef struct
{
    uint8_t bFunctionLength;    //描述符长度
    uint8_t bDescriptorType;    // CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x01
    uint8_t bmCapabilities;     //D0=0表示不处理调用管理，D0=0时D1无效，D2-D7保留
    uint8_t bDataInterface;
} Call_Management_Func_Desc_t;

typedef struct
{
    uint8_t bFunctionLength;    //描述符长度
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x02
    uint8_t bmCapabilities;     //
} Abstract_Control_Management_Func_Desc_t;

#define SLAVEINTERFACE 1
typedef struct
{
    uint8_t bFunctionLength;    //描述符长度
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x06
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface[SLAVEINTERFACE];
} Union_Functional_Descriptor_t;

// 描述符集合
typedef struct
{
    Configuration_Descriptor_t Configuration_Desc;
    Interface_Descriptor_t Communication_Interface_Desc;
    Header_Func_Desc_t Header_Func_Desc;
    Call_Management_Func_Desc_t Call_Management_Func_Desc;
    Abstract_Control_Management_Func_Desc_t Abstract_Control_Management_Func_Desc;
    Union_Functional_Descriptor_t Union_Functional_Desc;
    Endpoint_Descriptor_t Communication_Endpoint_Desc;
    Interface_Descriptor_t Data_Interface_Desc;
    Endpoint_Descriptor_t Data_Endpoint_Desc1;
    Endpoint_Descriptor_t Data_Endpoint_Desc2;
} Descriptor_Set_t;



#pragma pack()

extern Device_Descriptor_t Device_Descriptor;
extern Descriptor_Set_t Descriptor_Set;
extern ID_Descriptor_t ID_Descriptor;


void descript_init(void);

#endif
