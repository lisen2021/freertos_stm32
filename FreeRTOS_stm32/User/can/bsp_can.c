/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   can�������ػ�ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"
#include "portmacro.h"
#include "projdefs.h"


extern  __IO uint32_t can1_message;
/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
   	

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	  /* Connect CAN pins to AF9 */
  GPIO_PinAFConfig(CAN_TX_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_RX_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

	  /* Configure CAN TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure CAN RX  pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);


}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	 	/*�ж�����*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN RX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;		   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	/************************CANͨ�Ų�������**********************************/
	/* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN�Ĵ�����ʼ��*/
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN��Ԫ��ʼ��*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;  //�ػ�����ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=4 bs2=2 λʱ����Ϊ(1+4+2) �����ʼ�Ϊʱ������tq*(1+4+2)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 ʱ���1 ռ����4��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 ʱ���2 ռ����2��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 42 MHz) */
	CAN_InitStructure.CAN_Prescaler =6;		   ////BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 42/(1+4+2)/6=1 Mbps
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CANɸѡ����ʼ��*/
	CAN_FilterInitStructure.CAN_FilterNumber=14;					//ɸѡ����14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//����������ģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//ɸѡ��λ��Ϊ����32λ��
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */

	CAN_FilterInitStructure.CAN_FilterIdHigh= ((((u32)ID_SetVout<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)ID_SetVout<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;			//ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=  0x3EFF;			//ɸѡ����16������λ����ƥ��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//ɸѡ����������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//ʹ��ɸѡ��
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


/**
  * @brief  ��ʼ�� Rx Message���ݽṹ��
  * @param  RxMessage: ָ��Ҫ��ʼ�������ݽṹ��
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*�ѽ��սṹ������*/
  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    RxMessage->Data[ubCounter] = 0x00;
  }
}


/*
 * ��������CAN_SetMsg
 * ����  ��CANͨ�ű�����������,����һ����������Ϊ0-7�����ݰ�
 * ����  �����ͱ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage)
{	  
					 
  
  TxMessage->IDE=CAN_ID_EXT;			//��չģʽ
  TxMessage->RTR=CAN_RTR_DATA;			//���͵�������
  TxMessage->DLC=8;					    //���ݳ���Ϊ8�ֽ�
	
if (can1_message==0)
{
	  TxMessage->ExtId=ID_SetVout;//ʹ�õ���չID
	
	  /*����Ҫ���͵�����0-7,��ʱ��ʾ���������ѹΪ11V*/
		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x00;
		TxMessage->Data[2]=0x00;
		TxMessage->Data[3]=0x00;
		TxMessage->Data[4]=0x00;
		TxMessage->Data[5]=0x00;
		TxMessage->Data[6]=0x2C;
		TxMessage->Data[7]=0x00;
	
}
else if(can1_message==1)
{
	  TxMessage->ExtId=ID_SetOverVoltage;//ʹ�õ���չID
	
		/*����Ҫ���͵�����0-7,��ʱ��ʾ���ù�ѹ������Ϊ12.5V*/
		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x02;
		TxMessage->Data[2]=0x00;
		TxMessage->Data[3]=0x00;
		TxMessage->Data[4]=0x00;
		TxMessage->Data[5]=0x00;
		TxMessage->Data[6]=0x32;
		TxMessage->Data[7]=0x00;
}
else if(can1_message==2)
{
    TxMessage->ExtId=ID_SetCurrentLimit;//ʹ�õ���չID
	
		/*����Ҫ���͵�����0-7,��ʱ��ʾ����Ĭ��������Ϊ1.22A*/
		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x03;
		TxMessage->Data[2]=0x00;
		TxMessage->Data[3]=0x00;
		TxMessage->Data[4]=0x00;
		TxMessage->Data[5]=0x00;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;

}		

else if(can1_message==3)
{
    TxMessage->ExtId=ID_Query;//ʹ�õ���չID
	
		/*����Ҫ���͵�����0-7,��ʱ��ʾ��ѯ����ģ��ȫ��ʵʱ����*/
		TxMessage->Data[0]=0x00;
		TxMessage->Data[1]=0x00;
		TxMessage->Data[2]=0x00;
		TxMessage->Data[3]=0x00;
		TxMessage->Data[4]=0x00;
		TxMessage->Data[5]=0x00;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;

}	
}

/*
 * ��������CAN1_Set_handle
 * ����  ��CAN������������ȡ�Ļظ����ݷ���,�˴��������жϣ�ֻ�����ݵ�ǰһ��Ԫ�ؼ���
 * ����  �����ͱ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	 
BaseType_t CAN1_Set_handle(CanRxMsg *RxSetMessage_CAN)
{	 
    uint16_t i = 0;	
	BaseType_t xReturn_Set_task = pdPASS;
	if(RxSetMessage_CAN[0].Data[0] == 0x01)
	{
	  printf("�����ѹ������ȷ");
	}
	else { i++;}
	
	if(RxSetMessage_CAN[1].Data[0] == 0x01)
	{
	  printf("��ѹ������������ȷ");
	}
	else { i++;}
	
	if(RxSetMessage_CAN[2].Data[0] == 0x01)
	{
	  printf("������������ȷ");
	}
	else { i++;}
	
	if(i>0)
	{
		printf("set_error");
		xReturn_Set_task = pdFAIL;
		i= 0;
	}
	else
    {
	  return xReturn_Set_task;
	}
	
	return xReturn_Set_task;
}

/*
 * ��������CAN1_Query_handle
 * ����  ��CAN��ѯ�����������ȡ�Ļظ����ݷ���,�˴���������Ҫ��ָ�����һ��
 * ����  �����ձ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	
BaseType_t CAN1_Query_handle(CanRxMsg *RxRTMessage_CAN)
{	
	BaseType_t xReturn_RT_task = pdPASS;
    
	return xReturn_RT_task;
}
/**************************END OF FILE************************************/











