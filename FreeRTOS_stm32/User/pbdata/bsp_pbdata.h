#ifndef BSP_PBDATA_H
#define	BSP_PBDATA_H

#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdio.h>


/*����ջ�����ȼ��궨��*/

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




/*CAN���ͱ��Ĵ�������*/
#define NOERROR                 (uint8_t )0X00
#define Parameter_Error         (uint8_t )0X01
#define Invalid_Command         (uint8_t )0X02
#define Address_Recognition     (uint8_t )0X03
#define Electronic_Tag_Empty    (uint8_t )0X04
#define Hardware_Malfunction	(uint8_t )0X05
#define Rectifier_Interrupted   (uint8_t )0X06
#define Rectifier_SelfRegulate  (uint8_t )0X07
#define Address_Conflict        (uint8_t )0X08

/*CAN���ͱ����ź�ID*/
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


/*����485ͨѶ���Ľṹ��*/

/*���ͱ��Ľṹ��*/
typedef struct
{
	uint8_t Head0;//��ͷ0
	uint8_t Head1;//��ͷ1
	uint8_t Recipient_Type;//���շ�����
	uint8_t Recipient_Address;//���շ���ַ
	uint8_t Sender_Type;//���ͷ�����
	uint8_t Sender_Address;//���ͷ���ַ
	uint8_t Message_Type;//��������
	uint8_t Mes_Length1;//���ĳ���
	uint8_t Mes_Length0;
	uint8_t Data[8];//8���ֽ����鱣����������
	uint8_t Check_Digit;//У��λ

}RS485_TX_Packet;

///*�ӹ��ģ�鱨�Ľṹ��*/
//typedef struct
//{
//	uint8_t Head0;//��ͷ0
//	uint8_t Head1;//��ͷ1
//	uint8_t Recipient_Type;//���շ�����
//	uint8_t Recipient_Address;//���շ���ַ
//	uint8_t Sender_Type;//���ͷ�����
//	uint8_t Sender_Address;//���ͷ���ַ
//	uint8_t Message_Type;//��������
//	uint8_t Mes_Length1;//���ĳ���
//	uint8_t Mes_Length0;
//	uint16_t Data_V;//����ѹ
//	uint16_t Data_VW;//����ѹ����
//	uint16_t Data_I;//������
//	uint16_t Data_IW;//����������
//	uint8_t Check_Digit;//У��λ

//}RS485_RX_PV_Packet;

///*��˫��DCģ�鱨�Ľṹ��*/
//typedef struct
//{
//	uint8_t Head0;//��ͷ0
//	uint8_t Head1;//��ͷ1
//	uint8_t Recipient_Type;//���շ�����
//	uint8_t Recipient_Address;//���շ���ַ
//	uint8_t Sender_Type;//���ͷ�����
//	uint8_t Sender_Address;//���ͷ���ַ
//	uint8_t Message_Type;//��������
//	uint8_t Mes_Length1;//���ĳ���
//	uint8_t Mes_Length0;
//	uint8_t BatteryNum; //�������
//	
//	uint16_t Data_AV;//������ѹ
//	uint16_t Data_AP;//��������
//	uint16_t Data_AT;//������¶�
//	uint16_t Data_AR;//���������
//	
//	uint16_t Data_BV;//������ѹ
//	uint16_t Data_BP;//��������
//	uint16_t Data_BT;//������¶�
//	uint16_t Data_BR;//���������
//	uint8_t Check_Digit;//У��λ

//}RS485_RX_DC_Packet;

///*��˫��DCģ�鱨�Ľṹ��*/
//typedef struct
//{
//	uint8_t Head0;//��ͷ0
//	uint8_t Head1;//��ͷ1
//	uint8_t Recipient_Type;//���շ�����
//	uint8_t Recipient_Address;//���շ���ַ
//	uint8_t Sender_Type;//���ͷ�����
//	uint8_t Sender_Address;//���ͷ���ַ
//	uint8_t Message_Type;//��������
//	uint8_t Mes_Length1;//���ĳ���
//	uint8_t Mes_Length0;
//	uint8_t LoadNum; //������
//	
//	uint16_t Data_ASta;//����״̬
//	uint16_t Data_AV;//���ص�ѹ
//	uint16_t Data_AI;//���ص���
//	uint16_t Data_AW;//���ؾ���
//	
//	uint16_t Data_BSta;//����״̬
//	uint16_t Data_BV;//���ص�ѹ
//	uint16_t Data_BI;//���ص���
//	uint16_t Data_BW;//���ؾ���
//	uint8_t Check_Digit;//У��λ

//}RS485_RX_Load_Packet;



/*����״̬��Ϣ���ṹ��*/
typedef struct
{
  char TypeMark[2] ;  /*���ͱ�־ pi�����Դ״̬��Ϣ�� */

  int  PacketMessage;  /*��������+����� */

  char StationID[8];     /*̨վID */

  uint32_t Time  ;     /*����ʱ�䣬���1970.1.1 00��00��00 ���������� */

  short SVA;     /*���A�ĳ���ѹ */

  short SVAW; /*���A�ĳ���ѹ�澯 */
	
  short SIA;     /*���A�ĳ����� */

  short SIAW; /*���A�ĳ������澯 */
	
  short SVB;     /*�е����B�ĳ���ѹ */

  short SVBW; /*���B�ĳ���ѹ�澯 */
	
  short SIB;     /*�е����B�ĳ����� */

  short SIBW; /*���B�ĳ������澯 */
  
  short S1_State;  /*��һ·����״̬ */
  short S1_OV;     /*��һ·�����ѹ */  
  short S1_Current;/*��һ·���ص��� */ 
  short S1_CurrentW; /*��һ·���ص����澯 */ 
  
  short S2_State;  /*�ڶ�·����״̬ */
  short S2_OV;     /*�ڶ�·�����ѹ */  
  short S2_Current;/*�ڶ�·���ص��� */ 
  short S2_CurrentW; /*�ڶ�·���ص����澯 */
  
  short S3_State;  /*����·����״̬ */
  short S3_OV;     /*����·�����ѹ */  
  short S3_Current;/*����·���ص��� */ 
  short S3_CurrentW; /*����·���ص����澯 */
  
  short S4_State;  /*����·����״̬ */
  short S4_OV;     /*����·�����ѹ */  
  short S4_Current;/*����·���ص��� */ 
  short S4_CurrentW; /*����·���ص����澯 */
  
  short S5_State;  /*����·����״̬ */
  short S5_OV;     /*����·�����ѹ */  
  short S5_Current;/*����·���ص��� */ 
  short S5_CurrentW; /*����·���ص����澯 */
  
  short S6_State;  /*����·����״̬ */
  short S6_OV;     /*����·�����ѹ */  
  short S6_Current;/*����·���ص��� */ 
  short S6_CurrentW; /*����·���ص����澯 */

  short TemperA; /*�����¶�A*/
  short TemperB; /*�����¶�B*/
  short TemperW; /*�����¶ȸ澯*/
  
  short HumidityA; /*����ʪ��A*/
  short HumidityB; /*����ʪ��B*/
  short HumidityW; /*����ʪ�ȸ澯*/
  
  short AP;/*����ѹ��*/
  
  short SwitchA_Value; /*������A*/
  short SwitchB_Value; /*������B*/
  short SwitchC_Value; /*������C*/
  short SwitchD_Value; /*������D*/
  short SwitchE_Value; /*������E*/

  uint8_t RC[22]; /*�����ֽڣ�ʹ�����ݰ�����Ϊ128�ֽ�*/
  
} StatusPacket;

/*���������ݰ��ṹ��*/
typedef struct
{
  struct
  {
	char TypeMark[2] ; /*���ݰ���ʶ����Ϊwc,ws*/
	int  PacketMessage;  /*��������+����� */
	char QualityIndicator;/*����ָʾ����һ��ΪD*/
	char RC;              /*�����ַ���һ��Ϊ��*/
	uint8_t StationID[5];       /*̨վID*/
	uint8_t LocationID[2];   /*̨վ�Ĵ��������98��ʾ��Դ*/
	uint8_t ChannelID[3];    /*ͨ����ʶ��*/
	uint8_t NetworkCode[2];   /*̨������*/
	uint8_t Time[10];      /*��¼��ʼʱ�䣬����UTCʱ��*/       
	short Sample_Number;   /*���ݰ����������*/
	short Sampling_rate_factor;   /*���ݲ���������*/
	short Sampling_rate_multiplier;   /*���ݲ���������Ϊ�������ǳ���*/
	uint8_t Active_flag;   /*���־*/
	uint8_t Timer_flag;   /*���������ʱ�ӱ�־*/
	uint8_t DataQualityIndicator;   /*����������־*/
	uint8_t SubBlock_Number;   /*�ӿ����Ŀ*/
	int Time_correction_value; /*ʱ��У��ֵ*/
	short Data_start_offset;   /*���ݿ�ʼƫ����*/
	short SubBlock1_offset;    /*��һ�������ӿ�ƫ��*/

  }FixedHead;
  
  struct
  {
     
	short Typde;   /*�ӿ�����*/
	short NextSubBlock_ByteNumber;   /*��һ���ӿ���ֽں�*/
	uint8_t CodeFormat;   /*�����ʽ*/
	uint8_t Word_Order;   /*�ֽ���˳�򣬸߶˻��ǵͶ�*/
	uint8_t Packet_Length;   /*���ݰ��ĳ��ȡ��̶�Ϊ8*/
	char RC;              /*�����ַ���һ��Ϊ��*/

  }SubBlock;

    char ChannelSequenceID;   /*ͨ��˳���ʶ*/
	char RC;              /*�����ַ���һ��Ϊ��*/
	int  dimension; /*����*/
    uint8_t Data[192];/*������*/
  
} MonitoringDataPackage;



/*******************************************У��ͺ�������***************************************************/


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

/*У��λ�ͺ���*/
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
