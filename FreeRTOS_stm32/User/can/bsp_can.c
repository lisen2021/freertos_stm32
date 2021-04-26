/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   can驱动（回环模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"
#include "portmacro.h"
#include "projdefs.h"


extern  __IO uint32_t can1_message;
/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
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
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	 	/*中断设置*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN RX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;		   //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	/************************CAN通信参数设置**********************************/
	/* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN寄存器初始化*/
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;  //回环工作模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=4 bs2=2 位时间宽度为(1+4+2) 波特率即为时钟周期tq*(1+4+2)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 时间段1 占用了4个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 时间段2 占用了2个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 42 MHz) */
	CAN_InitStructure.CAN_Prescaler =6;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+4+2)/6=1 Mbps
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN筛选器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=14;					//筛选器组14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在掩码模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */

	CAN_FilterInitStructure.CAN_FilterIdHigh= ((((u32)ID_SetVout<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 
	CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)ID_SetVout<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;			//筛选器高16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=  0x3EFF;			//筛选器低16其中三位不必匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能筛选器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


/**
  * @brief  初始化 Rx Message数据结构体
  * @param  RxMessage: 指向要初始化的数据结构体
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*把接收结构体清零*/
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
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置,设置一个数据内容为0-7的数据包
 * 输入  ：发送报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage)
{	  
					 
  
  TxMessage->IDE=CAN_ID_EXT;			//扩展模式
  TxMessage->RTR=CAN_RTR_DATA;			//发送的是数据
  TxMessage->DLC=8;					    //数据长度为8字节
	
if (can1_message==0)
{
	  TxMessage->ExtId=ID_SetVout;//使用的扩展ID
	
	  /*设置要发送的数据0-7,此时表示设置输出电压为11V*/
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
	  TxMessage->ExtId=ID_SetOverVoltage;//使用的扩展ID
	
		/*设置要发送的数据0-7,此时表示设置过压保护点为12.5V*/
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
    TxMessage->ExtId=ID_SetCurrentLimit;//使用的扩展ID
	
		/*设置要发送的数据0-7,此时表示设置默认限流点为1.22A*/
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
    TxMessage->ExtId=ID_Query;//使用的扩展ID
	
		/*设置要发送的数据0-7,此时表示查询整流模块全部实时数据*/
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
 * 函数名：CAN1_Set_handle
 * 描述  ：CAN设置命令发出后获取的回复数据分析,此处先做简单判断，只看数据的前一个元素即可
 * 输入  ：发送报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	 
BaseType_t CAN1_Set_handle(CanRxMsg *RxSetMessage_CAN)
{	 
    uint16_t i = 0;	
	BaseType_t xReturn_Set_task = pdPASS;
	if(RxSetMessage_CAN[0].Data[0] == 0x01)
	{
	  printf("输出电压设置正确");
	}
	else { i++;}
	
	if(RxSetMessage_CAN[1].Data[0] == 0x01)
	{
	  printf("过压保护点设置正确");
	}
	else { i++;}
	
	if(RxSetMessage_CAN[2].Data[0] == 0x01)
	{
	  printf("限流点设置正确");
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
 * 函数名：CAN1_Query_handle
 * 描述  ：CAN查询命令发出后出后获取的回复数据分析,此处将几个重要的指令分析一下
 * 输入  ：接收报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	
BaseType_t CAN1_Query_handle(CanRxMsg *RxRTMessage_CAN)
{	
	BaseType_t xReturn_RT_task = pdPASS;
    
	return xReturn_RT_task;
}
/**************************END OF FILE************************************/











