#include "usb_descript.h"

Device_Descriptor_t Device_Descriptor;
Descriptor_Set_t Descriptor_Set;
ID_Descriptor_t ID_Descriptor;

void descript_init(void)
{
    // 设备描述符
    Device_Descriptor.bLength = sizeof(Device_Descriptor_t); //该描述符结构体大小
    Device_Descriptor.bDescriptorType = DEVICE_DESCRIPTOR;   //描述符类型
    Device_Descriptor.bcdUSB = 0x0110;                       //USB版本号
    Device_Descriptor.bDeviceClass = 2;                      //设备类代码（由USB官方分配）
    Device_Descriptor.bDeviceSubClass = 0;                   //子类代码（由USB官方分配）
    Device_Descriptor.bDeviceProtocol = 0;                   //设备协议代码（由USB官方分配）
    Device_Descriptor.bMaxPacketSize0 = 64;                  //端点0的最大包大小（有效大小为8,16,32,64）
    Device_Descriptor.idVendor = 0x1a86;                     //生产厂商编号（由USB官方分配）
    Device_Descriptor.idProduct = 0x5722;                    //产品编号（制造厂商分配）
    Device_Descriptor.bcdDevice = 0x01;                      //设备出厂编号
    Device_Descriptor.iManufacturer = 1;                     //设备厂商字符串索引
    Device_Descriptor.iProduct = 2;                          //产品描述字符串索引
    Device_Descriptor.iSerialNumber = 3;                     //设备序列号字符串索引
    Device_Descriptor.bNumConfigurations = 1;                //当前速度下能支持的配置数量

    // 配置描述符
    Descriptor_Set.Configuration_Desc. bLength = sizeof(Configuration_Descriptor_t);            //描述符长度
    Descriptor_Set.Configuration_Desc. bDescriptorType = CONFIGURATION_DESCRIPTOR;    //描述符类型
    Descriptor_Set.Configuration_Desc. wTotallLength = sizeof(Descriptor_Set_t);     //配置描述符集合总长度
    Descriptor_Set.Configuration_Desc. bNumInterfaces = 2;     //配置支持的接口数
    Descriptor_Set.Configuration_Desc. bConfigurationValue = 1;//该配置的值
    Descriptor_Set.Configuration_Desc. iConfiguration = 0;     //描述该配置的字符串索引值
    Descriptor_Set.Configuration_Desc. bmAttributes = 0x80;       //供电模式选择．Bit4-0保留，D7:总线供电，D6:自供电，D5:远程唤醒
    Descriptor_Set.Configuration_Desc. bMaxPower = 200;          //设备需要的最大电流 单位2mA

    //通信接口描述符
    Descriptor_Set.Communication_Interface_Desc. bLength = sizeof(Interface_Descriptor_t);            //描述符大小．
    Descriptor_Set.Communication_Interface_Desc. bDescriptorType = INTERFACE_DESCRIPTOR;    //接口描述符类型．
    Descriptor_Set.Communication_Interface_Desc. bInterfaceNumber = 0;   //该接口的编号．
    Descriptor_Set.Communication_Interface_Desc. bAlternateSetting = 0;  //用于为上一个字段选择可供替换的位置．即备用的接口描述符标号．
    Descriptor_Set.Communication_Interface_Desc. bNumEndpoint = 1;       //使用的端点数目。
    Descriptor_Set.Communication_Interface_Desc. bInterfaceClass = 0x02;    //类型代码．
    Descriptor_Set.Communication_Interface_Desc. bInterfaceSubClass = 0x02; //子类型代码．
    Descriptor_Set.Communication_Interface_Desc. bInterfaceProtocol = 1; //协议代码．
    Descriptor_Set.Communication_Interface_Desc. iInterface = 0;         //字符串描述符的索引

    Descriptor_Set.Header_Func_Desc. bFunctionLength = sizeof(Header_Func_Desc_t);//描述符长度
    Descriptor_Set.Header_Func_Desc. bDescriptorType = CS_INTERFACE;//CS_INTERFACE
    Descriptor_Set.Header_Func_Desc. bDescriptorSubtype = 0x00;//0x00
    Descriptor_Set.Header_Func_Desc. bcdCDC = 0x0110;//0x0110
    
    Descriptor_Set.Call_Management_Func_Desc. bFunctionLength = sizeof(Call_Management_Func_Desc_t);//描述符长度
    Descriptor_Set.Call_Management_Func_Desc. bDescriptorType = CS_INTERFACE;// CS_INTERFACE
    Descriptor_Set.Call_Management_Func_Desc. bDescriptorSubtype = 0x01;//0x01
    Descriptor_Set.Call_Management_Func_Desc. bmCapabilities = 0x00;//D0=0表示不处理调用管理，D0=0时D1无效，D2-D7保留
    Descriptor_Set.Call_Management_Func_Desc. bDataInterface = 0;
    
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bFunctionLength = sizeof(Abstract_Control_Management_Func_Desc_t); //描述符长度
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bDescriptorType = CS_INTERFACE;//CS_INTERFACE
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bDescriptorSubtype = 0x02; //0x02
    Descriptor_Set.Abstract_Control_Management_Func_Desc. bmCapabilities = 0x2;//
    
    Descriptor_Set.Union_Functional_Desc. bFunctionLength = sizeof(Union_Functional_Descriptor_t); //描述符长度
    Descriptor_Set.Union_Functional_Desc. bDescriptorType = CS_INTERFACE; //CS_INTERFACE
    Descriptor_Set.Union_Functional_Desc. bDescriptorSubtype = 0x06;//0x06
    Descriptor_Set.Union_Functional_Desc. bMasterInterface = 0;
    Descriptor_Set.Union_Functional_Desc. bSlaveInterface[0] = 1;

    //Communication端点描述符
    Descriptor_Set.Communication_Endpoint_Desc. bLength = sizeof(Endpoint_Descriptor_t);            //描述符大小．    
    Descriptor_Set.Communication_Endpoint_Desc. bDescriptorType = ENDPOINT_DESCRIPTOR;    //描述符类型．    
    Descriptor_Set.Communication_Endpoint_Desc. bEndpointType = 0x81;      //USB设备的端点地址．Bit7，方向，对于控制端点可以忽略，1/0:IN/OUT．Bit6-4，保留．BIt3-0：端点号．
    Descriptor_Set.Communication_Endpoint_Desc. bmAttributes = 3;       //端点属性．Bit7-2，保留．BIt1-0：00控制，01同步，02批量，03中断．
    Descriptor_Set.Communication_Endpoint_Desc. wMaxPacketSize = 16;    //本端点接收或发送的最大信息包大小．
    Descriptor_Set.Communication_Endpoint_Desc. bInterval = 10;          //轮询数据传送端点的时间间隔．对于批量传送和控制传送的端点忽略．对于同步传送的端点，必须为１，对于中断传送的端点，范围为１－２５５．

    //DATA接口描述符
    Descriptor_Set.Data_Interface_Desc. bLength = sizeof(Interface_Descriptor_t);            //描述符大小．
    Descriptor_Set.Data_Interface_Desc. bDescriptorType = INTERFACE_DESCRIPTOR;    //接口描述符类型．
    Descriptor_Set.Data_Interface_Desc. bInterfaceNumber = 1;   //该接口的编号．
    Descriptor_Set.Data_Interface_Desc. bAlternateSetting = 0;  //用于为上一个字段选择可供替换的位置．即备用的接口描述符标号．
    Descriptor_Set.Data_Interface_Desc. bNumEndpoint = 2;       //使用的端点数目。
    Descriptor_Set.Data_Interface_Desc. bInterfaceClass = 0x0A;    //类型代码．
    Descriptor_Set.Data_Interface_Desc. bInterfaceSubClass = 0x00; //子类型代码．
    Descriptor_Set.Data_Interface_Desc. bInterfaceProtocol = 0; //协议代码．
    Descriptor_Set.Data_Interface_Desc. iInterface = 0;         //字符串描述符的索引

    Descriptor_Set.Data_Endpoint_Desc1. bLength = sizeof(Endpoint_Descriptor_t);            //描述符大小．    
    Descriptor_Set.Data_Endpoint_Desc1. bDescriptorType = ENDPOINT_DESCRIPTOR;    //描述符类型．    
    Descriptor_Set.Data_Endpoint_Desc1. bEndpointType = 0x82;      //USB设备的端点地址．Bit7，方向，对于控制端点可以忽略，1/0:IN/OUT．Bit6-4，保留．BIt3-0：端点号．
    Descriptor_Set.Data_Endpoint_Desc1. bmAttributes = 0x02;       //端点属性．Bit7-2，保留．BIt1-0：00控制，01同步，02批量，03中断．
    Descriptor_Set.Data_Endpoint_Desc1. wMaxPacketSize = 64;    //本端点接收或发送的最大信息包大小．
    Descriptor_Set.Data_Endpoint_Desc1. bInterval = 0;          //轮询数据传送端点的时间间隔．对于批量传送和控制传送的端点忽略．对于同步传送的端点，必须为１，对于中断传送的端点，范围为１－２５５．

    Descriptor_Set.Data_Endpoint_Desc2. bLength = sizeof(Endpoint_Descriptor_t);            //描述符大小．    
    Descriptor_Set.Data_Endpoint_Desc2. bDescriptorType = ENDPOINT_DESCRIPTOR;    //描述符类型．    
    Descriptor_Set.Data_Endpoint_Desc2. bEndpointType = 0x02;      //USB设备的端点地址．Bit7，方向，对于控制端点可以忽略，1/0:IN/OUT．Bit6-4，保留．BIt3-0：端点号．
    Descriptor_Set.Data_Endpoint_Desc2. bmAttributes = 0x02;       //端点属性．Bit7-2，保留．BIt1-0：00控制，01同步，02批量，03中断．
    Descriptor_Set.Data_Endpoint_Desc2. wMaxPacketSize = 64;    //本端点接收或发送的最大信息包大小．
    Descriptor_Set.Data_Endpoint_Desc2. bInterval = 0;          //轮询数据传送端点的时间间隔．对于批量传送和控制传送的端点忽略．对于同步传送的端点，必须为１，对于中断传送的端点，范围为１－２５５．

    ID_Descriptor.bLength = sizeof(ID_Descriptor_t);
    ID_Descriptor.bDescriptorType = STRING_DESCRIPTOR;
    ID_Descriptor.ID = 0x0409;
    

}
