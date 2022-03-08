#include "usb_descript.h"

Device_Descriptor_t Device_Descriptor;
Descriptor_Set_t Descriptor_Set;
ID_Descriptor_t ID_Descriptor;

void descript_init(void)
{
    // �豸������
    Device_Descriptor.bLength = sizeof(Device_Descriptor_t); //���������ṹ���С
    Device_Descriptor.bDescriptorType = DEVICE_DESCRIPTOR;   //����������
    Device_Descriptor.bcdUSB = 0x0110;                       //USB�汾��
    Device_Descriptor.bDeviceClass = 2;                      //�豸����루��USB�ٷ����䣩
    Device_Descriptor.bDeviceSubClass = 0;                   //������루��USB�ٷ����䣩
    Device_Descriptor.bDeviceProtocol = 0;                   //�豸Э����루��USB�ٷ����䣩
    Device_Descriptor.bMaxPacketSize0 = 64;                  //�˵�0��������С����Ч��СΪ8,16,32,64��
    Device_Descriptor.idVendor = 0x1a86;                     //�������̱�ţ���USB�ٷ����䣩
    Device_Descriptor.idProduct = 0x5722;                    //��Ʒ��ţ����쳧�̷��䣩
    Device_Descriptor.bcdDevice = 0x01;                      //�豸�������
    Device_Descriptor.iManufacturer = 1;                     //�豸�����ַ�������
    Device_Descriptor.iProduct = 2;                          //��Ʒ�����ַ�������
    Device_Descriptor.iSerialNumber = 3;                     //�豸���к��ַ�������
    Device_Descriptor.bNumConfigurations = 1;                //��ǰ�ٶ�����֧�ֵ���������

    // ����������
    Descriptor_Set.Configuration_Desc. bLength = sizeof(Configuration_Descriptor_t);            //����������
    Descriptor_Set.Configuration_Desc. bDescriptorType = CONFIGURATION_DESCRIPTOR;    //����������
    Descriptor_Set.Configuration_Desc. wTotallLength = sizeof(Descriptor_Set_t);     //���������������ܳ���
    Descriptor_Set.Configuration_Desc. bNumInterfaces = 2;     //����֧�ֵĽӿ���
    Descriptor_Set.Configuration_Desc. bConfigurationValue = 1;//�����õ�ֵ
    Descriptor_Set.Configuration_Desc. iConfiguration = 0;     //���������õ��ַ�������ֵ
    Descriptor_Set.Configuration_Desc. bmAttributes = 0x80;       //����ģʽѡ��Bit4-0������D7:���߹��磬D6:�Թ��磬D5:Զ�̻���
    Descriptor_Set.Configuration_Desc. bMaxPower = 200;          //�豸��Ҫ�������� ��λ2mA

    //ͨ�Žӿ�������
    Descriptor_Set.Communication_Interface_Desc. bLength = sizeof(Interface_Descriptor_t);            //��������С��
    Descriptor_Set.Communication_Interface_Desc. bDescriptorType = INTERFACE_DESCRIPTOR;    //�ӿ����������ͣ�
    Descriptor_Set.Communication_Interface_Desc. bInterfaceNumber = 0;   //�ýӿڵı�ţ�
    Descriptor_Set.Communication_Interface_Desc. bAlternateSetting = 0;  //����Ϊ��һ���ֶ�ѡ��ɹ��滻��λ�ã������õĽӿ���������ţ�
    Descriptor_Set.Communication_Interface_Desc. bNumEndpoint = 1;       //ʹ�õĶ˵���Ŀ��
    Descriptor_Set.Communication_Interface_Desc. bInterfaceClass = 0x02;    //���ʹ��룮
    Descriptor_Set.Communication_Interface_Desc. bInterfaceSubClass = 0x02; //�����ʹ��룮
    Descriptor_Set.Communication_Interface_Desc. bInterfaceProtocol = 1; //Э����룮
    Descriptor_Set.Communication_Interface_Desc. iInterface = 0;         //�ַ���������������

    Descriptor_Set.Header_Func_Desc. bFunctionLength = sizeof(Header_Func_Desc_t);//����������
    Descriptor_Set.Header_Func_Desc. bDescriptorType = CS_INTERFACE;//CS_INTERFACE
    Descriptor_Set.Header_Func_Desc. bDescriptorSubtype = 0x00;//0x00
    Descriptor_Set.Header_Func_Desc. bcdCDC = 0x0110;//0x0110
    
    Descriptor_Set.Call_Management_Func_Desc. bFunctionLength = sizeof(Call_Management_Func_Desc_t);//����������
    Descriptor_Set.Call_Management_Func_Desc. bDescriptorType = CS_INTERFACE;// CS_INTERFACE
    Descriptor_Set.Call_Management_Func_Desc. bDescriptorSubtype = 0x01;//0x01
    Descriptor_Set.Call_Management_Func_Desc. bmCapabilities = 0x00;//D0=0��ʾ��������ù���D0=0ʱD1��Ч��D2-D7����
    Descriptor_Set.Call_Management_Func_Desc. bDataInterface = 0;
    
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bFunctionLength = sizeof(Abstract_Control_Management_Func_Desc_t); //����������
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bDescriptorType = CS_INTERFACE;//CS_INTERFACE
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bDescriptorSubtype = 0x02; //0x02
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bmCapabilities = 0x2;//
    
    Descriptor_Set.Union_Functional_Desc. bFunctionLength = sizeof(Union_Functional_Descriptor_t); //����������
    Descriptor_Set.Union_Functional_Desc. bDescriptorType = CS_INTERFACE; //CS_INTERFACE
    Descriptor_Set.Union_Functional_Desc. bDescriptorSubtype = 0x06;//0x06
    Descriptor_Set.Union_Functional_Desc. bMasterInterface = 0;
    Descriptor_Set.Union_Functional_Desc. bSlaveInterface[0] = 1;

    //Communication�˵�������
    Descriptor_Set.Communication_Endpoint_Desc. bLength = sizeof(Endpoint_Descriptor_t);            //��������С��    
    Descriptor_Set.Communication_Endpoint_Desc. bDescriptorType = ENDPOINT_DESCRIPTOR;    //���������ͣ�    
    Descriptor_Set.Communication_Endpoint_Desc. bEndpointType = 0x81;      //USB�豸�Ķ˵��ַ��Bit7�����򣬶��ڿ��ƶ˵���Ժ��ԣ�1/0:IN/OUT��Bit6-4��������BIt3-0���˵�ţ�
    Descriptor_Set.Communication_Endpoint_Desc. bmAttributes = 3;       //�˵����ԣ�Bit7-2��������BIt1-0��00���ƣ�01ͬ����02������03�жϣ�
    Descriptor_Set.Communication_Endpoint_Desc. wMaxPacketSize = 16;    //���˵���ջ��͵������Ϣ����С��
    Descriptor_Set.Communication_Endpoint_Desc. bInterval = 10;          //��ѯ���ݴ��Ͷ˵��ʱ�����������������ͺͿ��ƴ��͵Ķ˵���ԣ�����ͬ�����͵Ķ˵㣬����Ϊ���������жϴ��͵Ķ˵㣬��ΧΪ������������

    //DATA�ӿ�������
    Descriptor_Set.Data_Interface_Desc. bLength = sizeof(Interface_Descriptor_t);            //��������С��
    Descriptor_Set.Data_Interface_Desc. bDescriptorType = INTERFACE_DESCRIPTOR;    //�ӿ����������ͣ�
    Descriptor_Set.Data_Interface_Desc. bInterfaceNumber = 1;   //�ýӿڵı�ţ�
    Descriptor_Set.Data_Interface_Desc. bAlternateSetting = 0;  //����Ϊ��һ���ֶ�ѡ��ɹ��滻��λ�ã������õĽӿ���������ţ�
    Descriptor_Set.Data_Interface_Desc. bNumEndpoint = 2;       //ʹ�õĶ˵���Ŀ��
    Descriptor_Set.Data_Interface_Desc. bInterfaceClass = 0x0A;    //���ʹ��룮
    Descriptor_Set.Data_Interface_Desc. bInterfaceSubClass = 0x00; //�����ʹ��룮
    Descriptor_Set.Data_Interface_Desc. bInterfaceProtocol = 0; //Э����룮
    Descriptor_Set.Data_Interface_Desc. iInterface = 0;         //�ַ���������������

    Descriptor_Set.Data_Endpoint_Desc1. bLength = sizeof(Endpoint_Descriptor_t);            //��������С��    
    Descriptor_Set.Data_Endpoint_Desc1. bDescriptorType = ENDPOINT_DESCRIPTOR;    //���������ͣ�    
    Descriptor_Set.Data_Endpoint_Desc1. bEndpointType = 0x82;      //USB�豸�Ķ˵��ַ��Bit7�����򣬶��ڿ��ƶ˵���Ժ��ԣ�1/0:IN/OUT��Bit6-4��������BIt3-0���˵�ţ�
    Descriptor_Set.Data_Endpoint_Desc1. bmAttributes = 0x02;       //�˵����ԣ�Bit7-2��������BIt1-0��00���ƣ�01ͬ����02������03�жϣ�
    Descriptor_Set.Data_Endpoint_Desc1. wMaxPacketSize = 64;    //���˵���ջ��͵������Ϣ����С��
    Descriptor_Set.Data_Endpoint_Desc1. bInterval = 0;          //��ѯ���ݴ��Ͷ˵��ʱ�����������������ͺͿ��ƴ��͵Ķ˵���ԣ�����ͬ�����͵Ķ˵㣬����Ϊ���������жϴ��͵Ķ˵㣬��ΧΪ������������

    Descriptor_Set.Data_Endpoint_Desc2. bLength = sizeof(Endpoint_Descriptor_t);            //��������С��    
    Descriptor_Set.Data_Endpoint_Desc2. bDescriptorType = ENDPOINT_DESCRIPTOR;    //���������ͣ�    
    Descriptor_Set.Data_Endpoint_Desc2. bEndpointType = 0x02;      //USB�豸�Ķ˵��ַ��Bit7�����򣬶��ڿ��ƶ˵���Ժ��ԣ�1/0:IN/OUT��Bit6-4��������BIt3-0���˵�ţ�
    Descriptor_Set.Data_Endpoint_Desc2. bmAttributes = 0x02;       //�˵����ԣ�Bit7-2��������BIt1-0��00���ƣ�01ͬ����02������03�жϣ�
    Descriptor_Set.Data_Endpoint_Desc2. wMaxPacketSize = 64;    //���˵���ջ��͵������Ϣ����С��
    Descriptor_Set.Data_Endpoint_Desc2. bInterval = 0;          //��ѯ���ݴ��Ͷ˵��ʱ�����������������ͺͿ��ƴ��͵Ķ˵���ԣ�����ͬ�����͵Ķ˵㣬����Ϊ���������жϴ��͵Ķ˵㣬��ΧΪ������������

    ID_Descriptor.bLength = sizeof(ID_Descriptor_t);
    ID_Descriptor.bDescriptorType = STRING_DESCRIPTOR;
    ID_Descriptor.ID = 0x0409;
    

}
