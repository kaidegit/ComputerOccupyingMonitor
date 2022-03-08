#ifndef _USB_DESCRIPT__H
#define _USB_DESCRIPT__H

#include <stdint.h>

/**
 * USB��5�ӳ�����������
*/
#define DEVICE_DESCRIPTOR 0x01
#define CONFIGURATION_DESCRIPTOR 0x02
#define STRING_DESCRIPTOR 0x03
#define INTERFACE_DESCRIPTOR 0x04
#define ENDPOINT_DESCRIPTOR 0x05
#define CS_INTERFACE 0x24

#pragma pack(1)
// �豸������
typedef struct
{
    uint8_t bLength;            //���������ṹ���С
    uint8_t bDescriptorType;    //����������
    uint16_t bcdUSB;            //USB�汾��
    uint8_t bDeviceClass;       //�豸����루��USB�ٷ����䣩
    uint8_t bDeviceSubClass;    //������루��USB�ٷ����䣩
    uint8_t bDeviceProtocol;    //�豸Э����루��USB�ٷ����䣩
    uint8_t bMaxPacketSize0;    //�˵�0��������С����Ч��СΪ8,16,32,64��
    uint16_t idVendor;          //�������̱�ţ���USB�ٷ����䣩
    uint16_t idProduct;         //��Ʒ��ţ����쳧�̷��䣩
    uint16_t bcdDevice;         //�豸�������
    uint8_t iManufacturer;      //�豸�����ַ�������
    uint8_t iProduct;           //��Ʒ�����ַ�������
    uint8_t iSerialNumber;      //�豸���к��ַ�������
    uint8_t bNumConfigurations; //��ǰ�ٶ�����֧�ֵ���������
} Device_Descriptor_t;

// ����������
typedef struct
{
    uint8_t bLength;             //����������
    uint8_t bDescriptorType;     //����������
    uint16_t wTotallLength;      //���������������ܳ���
    uint8_t bNumInterfaces;      //����֧�ֵĽӿ���
    uint8_t bConfigurationValue; //�����õ�ֵ
    uint8_t iConfiguration;      //���������õ��ַ�������ֵ
    uint8_t bmAttributes;        //����ģʽѡ��Bit4-0������D7:				���߹��磬D6:�Թ��磬D5:Զ�̻���
    uint8_t bMaxPower;           //�豸��Ҫ�������� 1LSB = 2mA
} Configuration_Descriptor_t;

// �ӿ�������
typedef struct
{
    uint8_t bLength;            //��������С��
    uint8_t bDescriptorType;    //�ӿ����������ͣ�
    uint8_t bInterfaceNumber;   //�ýӿڵı�ţ�
    uint8_t bAlternateSetting;  //����Ϊ��һ���ֶ�ѡ��ɹ��滻��λ�ã������õĽӿ���������ţ�
    uint8_t bNumEndpoint;       //ʹ�õĶ˵���Ŀ���˵㣰���⣮
    uint8_t bInterfaceClass;    //���ʹ��룮
    uint8_t bInterfaceSubClass; //�����ʹ��룮
    uint8_t bInterfaceProtocol; //Э����룮
    uint8_t iInterface;         //�ַ���������������
} Interface_Descriptor_t;

// �˵�������
typedef struct
{
    uint8_t bLength;         //��������С��
    uint8_t bDescriptorType; //�ӿ����������ͣ�
    uint8_t bEndpointType;   //USB�豸�Ķ˵��ַ��Bit7�����򣬶��ڿ��ƶ˵�							���Ժ��ԣ�1/0:IN/OUT��Bit6-4��������BIt3-0���˵�ţ�
    uint8_t bmAttributes;    //�˵����ԣ�Bit7-2��������BIt1-0��00���ƣ�01ͬ����02������03					�жϣ�
    uint16_t wMaxPacketSize; //���˵���ջ��͵������Ϣ����С��
    uint8_t bInterval;       //��ѯ���ݴ��Ͷ˵��ʱ�����������������ͺͿ��ƴ��͵Ķ˵����			������ͬ�����͵Ķ˵㣬����Ϊ���������жϴ��͵Ķ˵㣬��ΧΪ������������
} Endpoint_Descriptor_t;

// ����ID������
typedef struct
{
    uint8_t bLength;         //��������С��
    uint8_t bDescriptorType; //�ӿ����������ͣ��̶�Ϊ0x03��
    uint16_t ID;             //���Ա��룮
} ID_Descriptor_t;

typedef struct
{
    uint8_t bFunctionLength;    //����������
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x00
    uint16_t bcdCDC;            //0x0110
} Header_Func_Desc_t;

typedef struct
{
    uint8_t bFunctionLength;    //����������
    uint8_t bDescriptorType;    // CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x01
    uint8_t bmCapabilities;     //D0=0��ʾ��������ù���D0=0ʱD1��Ч��D2-D7����
    uint8_t bDataInterface;
} Call_Management_Func_Desc_t;

typedef struct
{
    uint8_t bFunctionLength;    //����������
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x02
    uint8_t bmCapabilities;     //
} Abstract_Control_Management_Func_Desc_t;

#define SLAVEINTERFACE 1
typedef struct
{
    uint8_t bFunctionLength;    //����������
    uint8_t bDescriptorType;    //CS_INTERFACE
    uint8_t bDescriptorSubtype; //0x06
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface[SLAVEINTERFACE];
} Union_Functional_Descriptor_t;

// ����������
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
