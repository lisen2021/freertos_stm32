/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   485����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./485/bsp_485.h"
#include "bsp_pbdata.h"
#include <stdarg.h>


static void Delay(__IO u32 nCount); 

extern __IO uint32_t RS485_message ;

/// ����USART�����ж�
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = _485_INT_IRQ; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * ��������_485_Config
 * ����  ��USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void _485_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART clock */
	RCC_AHB1PeriphClockCmd(_485_USART_RX_GPIO_CLK|_485_USART_TX_GPIO_CLK|_485_RE_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(_485_USART_CLK, ENABLE); 

	
	  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(_485_USART_RX_GPIO_PORT,_485_USART_RX_SOURCE, _485_USART_RX_AF);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(_485_USART_TX_GPIO_PORT,_485_USART_TX_SOURCE,_485_USART_TX_AF);

	
	/* USART GPIO config */
   /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = _485_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(_485_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Pin = _485_USART_RX_PIN;
  GPIO_Init(_485_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  
  /* 485�շ����ƹܽ� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = _485_RE_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(_485_RE_GPIO_PORT, &GPIO_InitStructure);
	  
	/* USART mode config */
	USART_InitStructure.USART_BaudRate = _485_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(_485_USART, &USART_InitStructure); 
    USART_Cmd(_485_USART, ENABLE);
	
	NVIC_Configuration();
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(_485_USART, USART_IT_RXNE, ENABLE);
	
	GPIO_ResetBits(_485_RE_GPIO_PORT,_485_RE_PIN); //Ĭ�Ͻ������ģʽ
}



/***************** ����һ���ַ�  **********************/
//ʹ�õ��ֽ����ݷ���ǰҪʹ�ܷ������ţ����ͺ�Ҫʹ�ܽ������š�
void _485_SendByte(  uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(_485_USART,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(_485_USART, USART_FLAG_TXE) == RESET);	
	
}
/*****************  ����ָ�����ȵ��ַ��� **********************/
void _485_SendStr_length( uint8_t * str,uint32_t strlen )
{
	unsigned int k=0;

	_485_TX_EN()	;//	ʹ�ܷ�������	
    do 
    {
        _485_SendByte( *((uint8_t *)&str +k));
        k++;
    } while(k < strlen);
		
	/*�Ӷ�����ʱ����֤485�����������*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	ʹ�ܽ�������
}


/*****************  �����ַ��� **********************/
void _485_SendString(  uint8_t *str)
{
	unsigned int k=0;
	
	_485_TX_EN()	;//	ʹ�ܷ�������
	
    do 
    {
        _485_SendByte(  *(str + k) );
        k++;
    } while(*(str + k)!='\0');
	
	/*�Ӷ�����ʱ����֤485�����������*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	ʹ�ܽ�������
}



////�жϻ��洮������
//#define UART_BUFF_SIZE      1024
//volatile    uint16_t uart_p = 0;
//uint8_t     uart_buff[UART_BUFF_SIZE];

//void bsp_485_IRQHandler(void)
//{
//    if(uart_p<UART_BUFF_SIZE)
//    {
//        if(USART_GetITStatus(_485_USART, USART_IT_RXNE) != RESET)
//        {
//            uart_buff[uart_p] = USART_ReceiveData(_485_USART);
//            uart_p++;
//						
//						USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);
//        }
//    }
//		else
//		{
//			USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);
////			clean_rebuff();       
//		}
//}



////��ȡ���յ������ݺͳ���
//char *get_rebuff(uint16_t *len) 
//{
//    *len = uart_p;
//    return (char *)&uart_buff;
//}

////��ջ�����
//void clean_rebuff(void) 
//{

//    uint16_t i=UART_BUFF_SIZE+1;
//    uart_p = 0;
//	while(i)
//		uart_buff[--i]=0;

//}


static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


/*
 * ��������RS485_SetMsg
 * ����  ��485ͨ�ű�����������,����һ����������Ϊ0-7�����ݰ�
 * ����  �����ͱ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void RS485_SetMsg(RS485_TX_Packet *TxMessage)
{	  
					 
  
  TxMessage->Head0 = 0x55;			
  TxMessage->Head1 = 0x7e;		
  TxMessage->Sender_Type = 0x13;
  TxMessage->Sender_Address = 0x01;
  TxMessage->Message_Type = 0x02;
  TxMessage->Mes_Length1= 0x00;
  TxMessage->Mes_Length0 = 0x08;
  TxMessage->Check_Digit = 0x00;
  
  	
/*�����1������*/	
if (RS485_message==0)
{
	    TxMessage->Recipient_Type = 0x10;
	    TxMessage->Recipient_Address = 0x01;


		TxMessage->Data[0]=0x02;
		TxMessage->Data[1]=0x01;
		TxMessage->Data[2]=0x01;
		TxMessage->Data[3]=0x01;
		TxMessage->Data[4]=0x01;
		TxMessage->Data[5]=0x01;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
	    
}
/*�����2������*/
else if(RS485_message==1)
{
	    TxMessage->Recipient_Type = 0x10;
	    TxMessage->Recipient_Address = 0x02;


		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x02;
		TxMessage->Data[2]=0x01;
		TxMessage->Data[3]=0x01;
		TxMessage->Data[4]=0x01;
		TxMessage->Data[5]=0x01;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
}
/*���DCDC1������*/
else if(RS485_message==2)
{
	    TxMessage->Recipient_Type = 0x11;
	    TxMessage->Recipient_Address = 0x01;


		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x01;
		TxMessage->Data[2]=0x02;
		TxMessage->Data[3]=0x01;
		TxMessage->Data[4]=0x01;
		TxMessage->Data[5]=0x01;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
	
}

/*���DCDC2������*/
else if(RS485_message==3)
{
	    TxMessage->Recipient_Type = 0x11;
	    TxMessage->Recipient_Address = 0x02;


		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x01;
		TxMessage->Data[2]=0x01;
		TxMessage->Data[3]=0x02;
		TxMessage->Data[4]=0x01;
		TxMessage->Data[5]=0x01;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
	
}

/*��鸺��1������*/
else if(RS485_message==4)
{
	    TxMessage->Recipient_Type = 0x12;
	    TxMessage->Recipient_Address = 0x01;


		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x01;
		TxMessage->Data[2]=0x01;
		TxMessage->Data[3]=0x01;
		TxMessage->Data[4]=0x02;
		TxMessage->Data[5]=0x01;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
	
}

/*��鸺��2������*/
else if(RS485_message==5)
{
	    TxMessage->Recipient_Type = 0x12;
	    TxMessage->Recipient_Address = 0x02;


		TxMessage->Data[0]=0x01;
		TxMessage->Data[1]=0x01;
		TxMessage->Data[2]=0x01;
		TxMessage->Data[3]=0x01;
		TxMessage->Data[4]=0x01;
		TxMessage->Data[5]=0x02;
		TxMessage->Data[6]=0x00;
		TxMessage->Data[7]=0x00;
	
}
    TxMessage->Check_Digit = buffer_raw_cksum((void *)&TxMessage, sizeof(TxMessage));//У���
}

