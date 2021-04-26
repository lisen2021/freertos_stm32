#ifndef BSP_PBDATA_H
#define	BSP_PBDATA_H

#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdio.h>


/*任务栈与优先级宏定义*/

#define CAN1_SET_TASK_PRIO	   		15           
#define CAN1_SET_STK_SIZE 	   		128

#define CAN1_QUREY_TASK_PRIO	   	14          
#define CAN1_QUREY_STK_SIZE 	   	128

#define CAN1_RX_TASK_PRIO	   		16          
#define CAN1_RX_STK_SIZE 	   		128

#define RS485_QUREY_TASK_PRIO	   	13          
#define RS485_QUREY_STK_SIZE 	   	128

#define RS485_RX_TASK_PRIO	   		16          
#define RS485_RX_STK_SIZE 	   		128




/*CAN传送报文错误类型*/
#define NOERROR                 (uint8_t )0X00
#define Parameter_Error         (uint8_t )0X01
#define Invalid_Command         (uint8_t )0X02
#define Address_Recognition     (uint8_t )0X03
#define Electronic_Tag_Empty    (uint8_t )0X04
#define Hardware_Malfunction	(uint8_t )0X05
#define Rectifier_Interrupted   (uint8_t )0X06
#define Rectifier_SelfRegulate  (uint8_t )0X07
#define Address_Conflict        (uint8_t )0X08

/*CAN传送报文信号ID*/
#define DC_V_SET                (uint16_t) 0x100
#define Input_Power             (uint16_t) 0x170
#define Input_Frequency         (uint16_t) 0x171
#define Input_Current  			(uint16_t) 0x172
#define Output_Power  			(uint16_t) 0x173
#define Efficiency  			(uint16_t) 0x174
#define Output_V_MeasureValue  	(uint16_t) 0x175
#define Input_V  				(uint16_t) 0x178
#define Temperature  			(uint16_t) 0x180
#define Output_Current  		(uint16_t) 0x182
#define Alarm_Status  			(uint16_t) 0x183
#define Output_V  				(uint16_t) 0x184


/*定义485通讯报文结构体*/

/*发送报文结构体*/
typedef struct
{
	uint8_t Head0;//报头0
	uint8_t Head1;//报头1
	uint8_t Recipient_Type;//接收方类型
	uint8_t Recipient_Address;//接收方地址
	uint8_t Sender_Type;//发送方类型
	uint8_t Sender_Address;//发送方地址
	uint8_t Message_Type;//报文类型
	uint8_t Mes_Length1;//正文长度
	uint8_t Mes_Length0;
	uint8_t Data[8];//8个字节数组保存正文内容
	uint8_t Check_Digit;//校验位

}RS485_TX_Packet;

///*接光伏模块报文结构体*/
//typedef struct
//{
//	uint8_t Head0;//报头0
//	uint8_t Head1;//报头1
//	uint8_t Recipient_Type;//接收方类型
//	uint8_t Recipient_Address;//接收方地址
//	uint8_t Sender_Type;//发送方类型
//	uint8_t Sender_Address;//发送方地址
//	uint8_t Message_Type;//报文类型
//	uint8_t Mes_Length1;//正文长度
//	uint8_t Mes_Length0;
//	uint16_t Data_V;//充电电压
//	uint16_t Data_VW;//充电电压警告
//	uint16_t Data_I;//充电电流
//	uint16_t Data_IW;//充电电流警告
//	uint8_t Check_Digit;//校验位

//}RS485_RX_PV_Packet;

///*接双向DC模块报文结构体*/
//typedef struct
//{
//	uint8_t Head0;//报头0
//	uint8_t Head1;//报头1
//	uint8_t Recipient_Type;//接收方类型
//	uint8_t Recipient_Address;//接收方地址
//	uint8_t Sender_Type;//发送方类型
//	uint8_t Sender_Address;//发送方地址
//	uint8_t Message_Type;//报文类型
//	uint8_t Mes_Length1;//正文长度
//	uint8_t Mes_Length0;
//	uint8_t BatteryNum; //电池组数
//	
//	uint16_t Data_AV;//电池组电压
//	uint16_t Data_AP;//电池组电量
//	uint16_t Data_AT;//电池组温度
//	uint16_t Data_AR;//电池组内阻
//	
//	uint16_t Data_BV;//电池组电压
//	uint16_t Data_BP;//电池组电量
//	uint16_t Data_BT;//电池组温度
//	uint16_t Data_BR;//电池组内阻
//	uint8_t Check_Digit;//校验位

//}RS485_RX_DC_Packet;

///*接双向DC模块报文结构体*/
//typedef struct
//{
//	uint8_t Head0;//报头0
//	uint8_t Head1;//报头1
//	uint8_t Recipient_Type;//接收方类型
//	uint8_t Recipient_Address;//接收方地址
//	uint8_t Sender_Type;//发送方类型
//	uint8_t Sender_Address;//发送方地址
//	uint8_t Message_Type;//报文类型
//	uint8_t Mes_Length1;//正文长度
//	uint8_t Mes_Length0;
//	uint8_t LoadNum; //负载数
//	
//	uint16_t Data_ASta;//开关状态
//	uint16_t Data_AV;//负载电压
//	uint16_t Data_AI;//负载电流
//	uint16_t Data_AW;//负载警告
//	
//	uint16_t Data_BSta;//开关状态
//	uint16_t Data_BV;//负载电压
//	uint16_t Data_BI;//负载电流
//	uint16_t Data_BW;//负载警告
//	uint8_t Check_Digit;//校验位

//}RS485_RX_Load_Packet;



/*定义状态信息包结构体*/
typedef struct
{
  char TypeMark[2] ;  /*类型标志 pi代表电源状态信息包 */

  int  PacketMessage;  /*包长索引+包序号 */

  char StationID[8];     /*台站ID */

  uint32_t Time  ;     /*绝对时间，相对1970.1.1 00：00；00 经过的秒数 */

  short SVA;     /*光伏A的充电电压 */

  short SVAW; /*光伏A的充电电压告警 */
	
  short SIA;     /*光伏A的充电电流 */

  short SIAW; /*光伏A的充电电流告警 */
	
  short SVB;     /*市电或光伏B的充电电压 */

  short SVBW; /*光伏B的充电电压告警 */
	
  short SIB;     /*市电或光伏B的充电电流 */

  short SIBW; /*光伏B的充电电流告警 */
  
  short S1_State;  /*第一路开关状态 */
  short S1_OV;     /*第一路输出电压 */  
  short S1_Current;/*第一路负载电流 */ 
  short S1_CurrentW; /*第一路负载电流告警 */ 
  
  short S2_State;  /*第二路开关状态 */
  short S2_OV;     /*第二路输出电压 */  
  short S2_Current;/*第二路负载电流 */ 
  short S2_CurrentW; /*第二路负载电流告警 */
  
  short S3_State;  /*第三路开关状态 */
  short S3_OV;     /*第三路输出电压 */  
  short S3_Current;/*第三路负载电流 */ 
  short S3_CurrentW; /*第三路负载电流告警 */
  
  short S4_State;  /*第四路开关状态 */
  short S4_OV;     /*第四路输出电压 */  
  short S4_Current;/*第四路负载电流 */ 
  short S4_CurrentW; /*第四路负载电流告警 */
  
  short S5_State;  /*第五路开关状态 */
  short S5_OV;     /*第五路输出电压 */  
  short S5_Current;/*第五路负载电流 */ 
  short S5_CurrentW; /*第五路负载电流告警 */
  
  short S6_State;  /*第六路开关状态 */
  short S6_OV;     /*第六路输出电压 */  
  short S6_Current;/*第六路负载电流 */ 
  short S6_CurrentW; /*第六路负载电流告警 */

  short TemperA; /*环境温度A*/
  short TemperB; /*环境温度B*/
  short TemperW; /*环境温度告警*/
  
  short HumidityA; /*环境湿度A*/
  short HumidityB; /*环境湿度B*/
  short HumidityW; /*环境湿度告警*/
  
  short AP;/*大气压力*/
  
  short SwitchA_Value; /*开关量A*/
  short SwitchB_Value; /*开关量B*/
  short SwitchC_Value; /*开关量C*/
  short SwitchD_Value; /*开关量D*/
  short SwitchE_Value; /*开关量E*/

  uint8_t RC[22]; /*保留字节，使得数据包长度为128字节*/
  
} StatusPacket;

/*定义监测数据包结构体*/
typedef struct
{
  struct
  {
	char TypeMark[2] ; /*数据包标识符，为wc,ws*/
	int  PacketMessage;  /*包长索引+包序号 */
	char QualityIndicator;/*质量指示符，一般为D*/
	char RC;              /*保留字符，一般为空*/
	uint8_t StationID[5];       /*台站ID*/
	uint8_t LocationID[2];   /*台站的传感器编号98表示电源*/
	uint8_t ChannelID[3];    /*通道标识符*/
	uint8_t NetworkCode[2];   /*台网编码*/
	uint8_t Time[10];      /*记录起始时间，绝对UTC时间*/       
	short Sample_Number;   /*数据包采样点个数*/
	short Sampling_rate_factor;   /*数据采样率因子*/
	short Sampling_rate_multiplier;   /*数据采样率因子为乘数还是除数*/
	uint8_t Active_flag;   /*活动标志*/
	uint8_t Timer_flag;   /*输入输出和时钟标志*/
	uint8_t DataQualityIndicator;   /*数据质量标志*/
	uint8_t SubBlock_Number;   /*子块的数目*/
	int Time_correction_value; /*时间校正值*/
	short Data_start_offset;   /*数据开始偏移量*/
	short SubBlock1_offset;    /*第一个数据子块偏移*/

  }FixedHead;
  
  struct
  {
     
	short Typde;   /*子块类型*/
	short NextSubBlock_ByteNumber;   /*下一个子块的字节号*/
	uint8_t CodeFormat;   /*编码格式*/
	uint8_t Word_Order;   /*字节码顺序，高端还是低端*/
	uint8_t Packet_Length;   /*数据包的长度。固定为8*/
	char RC;              /*保留字符，一般为空*/

  }SubBlock;

    char ChannelSequenceID;   /*通道顺序标识*/
	char RC;              /*保留字符，一般为空*/
	int  dimension; /*量纲*/
    uint8_t Data[192];/*数据区*/
  
} MonitoringDataPackage;



/*******************************************校验和函数部分***************************************************/


static inline uint32_t __buffer_raw_cksum(const void *buf, uint32_t len, uint32_t sum)
{
	/* workaround gcc strict-aliasing warning */
	uintptr_t ptr = (uintptr_t)buf;
	const uint16_t *u16 = (const uint16_t *)ptr;


	while (len >= (sizeof(*u16) * 4)) {
		sum += u16[0];
		sum += u16[1];
		sum += u16[2];
		sum += u16[3];
		len -= sizeof(*u16) * 4;
		u16 += 4;
	}
	while (len >= sizeof(*u16)) {
		sum += *u16;
		len -= sizeof(*u16);
		u16 += 1;
	}


	/* if length is in odd bytes */
	if (len == 1)
	sum += *((uint8_t *)u16);


	return sum;
}


static inline uint16_t __buffer_raw_cksum_reduce(uint32_t sum)
{
	sum = ((sum & 0xffff0000) >> 16) + (sum & 0xffff);
	sum = ((sum & 0xffff0000) >> 16) + (sum & 0xffff);
	return (uint16_t)sum;
}

/*校验位和函数*/
static inline uint16_t buffer_raw_cksum(const void *buf, size_t len)
{
	uint32_t sum;
	uint16_t cksum;


	sum = __buffer_raw_cksum(buf, len, 0);
	cksum = __buffer_raw_cksum_reduce(sum);


	cksum = (~cksum) & 0xffff;
	if (cksum == 0)
	cksum = 0xffff;

	return cksum;
}



void StatusPacket_Init(StatusPacket *SP);
uint16_t Data_Integrate(uint8_t a,uint8_t b);
void RS485_Data_Tostatus();
void CAN1_Data_Tostatus();
char * time_to_timestamp();
#endif /* BSP_PBDATA_H */
