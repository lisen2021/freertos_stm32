/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./can/bsp_can.h"
#include "bsp_485.h"


#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h" 
#include "semphr.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick�жϷ�����
void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}


extern CanRxMsg RxMessage_CAN;				     //���ջ�����
extern CanRxMsg RxSetMessage_CAN[3];		     //������������󷵻صı��ĵ����ݻ�����
extern CanRxMsg RxRTMessage_CAN[12];		     //����ʵʱ���ݵĻ�����
extern SemaphoreHandle_t BinarySem_Handle_CAN_Set;//CAN������������ݽ��ܳɹ���Ķ�ֵ�ź���
extern SemaphoreHandle_t BinarySem_Handle_CAN_RT;//CANʵʱ���ݽ��ܳɹ���Ķ�ֵ�ź���

void CAN_RX_IRQHandler(void)
{
	uint32_t ulReturn;
    static 	uint8_t   i = 0;//ʹ��static���ξֲ�������ֻ����һ�γ�ʼ���������Ϳ��Գ������������ᷴ����0
	static 	uint8_t   j = 0;
	BaseType_t pxHigherPriorityTaskWoken;
	
   /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
   ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
	/*�������ж�������*/
	CAN_Receive(CANx, CAN_FIFO0, &RxMessage_CAN);

	/* �жϽ������ݵ�ID*/ 
	if(((RxMessage_CAN.ExtId==ID_SetVout_Back )||(RxMessage_CAN.ExtId==ID_SetOverVoltage_Back)) && (RxMessage_CAN.IDE==CAN_ID_EXT) && (RxMessage_CAN.DLC==8) )
	{
		RxSetMessage_CAN[i] = RxMessage_CAN;
		i++;
		if(i == 3)
		{
			//������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
			xSemaphoreGiveFromISR(BinarySem_Handle_CAN_Set,&pxHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
			i = 0;
		}	

	}
		/* �жϽ������ݵ�ID*/ 
	else if((RxMessage_CAN.ExtId==ID_Query_Back) && (RxMessage_CAN.IDE==CAN_ID_EXT) && (RxMessage_CAN.DLC==8) )
	{
		RxRTMessage_CAN[j] = RxMessage_CAN;
		j++;
		if(j == 12)
		{
			//������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
			xSemaphoreGiveFromISR(BinarySem_Handle_CAN_RT,&pxHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
			j = 0;
		}	

	}
	else
	{
		printf("δ���ܵ���Ч�Ļظ�����");
	}
	//�����Ҫ�Ļ�����һ�������л���ϵͳ���ж��Ƿ���Ҫ�����л�
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}



/************************ 485ͨѶ���ݽ����жϷ�����**************************/
//�жϻ��洮������
extern RS485_TX_Packet TxMessage_485;				 //���ջ�����
extern SemaphoreHandle_t BinarySem_Handle_485;

#define     UART_BUFF_SIZE      1024
volatile    uint16_t Uart_Rx_Num = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];

void _485_IRQHandler(void)
{
  uint32_t ulReturn;
  BaseType_t pxHigherPriorityTaskWoken;
	
   /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
  clean_rebuff();//��ջ�����
	
  if(USART_GetITStatus(_485_USART, USART_IT_RXNE) != RESET) //�жϲ���
	{  
		USART_ClearITPendingBit(_485_USART,USART_IT_RXNE); //����жϱ�־
		uart_buff[Uart_Rx_Num] = USART_ReceiveData(_485_USART);//����һ���ֽ�����
		Uart_Rx_Num++; //���ȼ�1
	}
	
	uart_buff[Uart_Rx_Num-1]=buffer_raw_cksum((void *)uart_buff, sizeof(uart_buff));//�����������У���λ
	
	if( (uart_buff[3] == 0x13)&&(uart_buff[Uart_Rx_Num-1] == TxMessage_485.Check_Digit ) ) //�ж������յ������Ƿ�Ϊ�趨ֵ��ȷ��������ȷ��
	{
		//������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
		xSemaphoreGiveFromISR(BinarySem_Handle_485,&pxHigherPriorityTaskWoken);//�ͷŶ�ֵ�ź���
		//�����Ҫ�Ļ�����һ�������л���ϵͳ���ж��Ƿ���Ҫ�����л�
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}
	else
	{
			 printf("У��Ͳ�һ�£��������ݳ���");
	}
	
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) //���
		{
			USART_ClearFlag(USART2,USART_FLAG_ORE);  //��SR
			USART_ReceiveData(_485_USART); //��DR  
		}    
		
		USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);    

   /* �˳��ٽ�� */
   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

//��ջ�����
void clean_rebuff(void) 
{

    uint16_t i=UART_BUFF_SIZE+1;
    Uart_Rx_Num = 0;
	while(i)
    uart_buff[--i]=0;

}