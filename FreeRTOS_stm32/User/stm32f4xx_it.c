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


#include "FreeRTOS.h"					//FreeRTOS使用		  
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
//systick中断服务函数
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


extern CanRxMsg RxMessage_CAN;				     //接收缓冲区
extern CanRxMsg RxSetMessage_CAN[3];		     //接收设置命令后返回的报文的数据缓冲区
extern CanRxMsg RxRTMessage_CAN[12];		     //接收实时数据的缓冲区
extern SemaphoreHandle_t BinarySem_Handle_CAN_Set;//CAN设置命令返回数据接受成功后的二值信号量
extern SemaphoreHandle_t BinarySem_Handle_CAN_RT;//CAN实时数据接受成功后的二值信号量

void CAN_RX_IRQHandler(void)
{
	uint32_t ulReturn;
    static 	uint8_t   i = 0;//使用static修饰局部变量后只进行一次初始化，这样就可以持续操作，不会反复归0
	static 	uint8_t   j = 0;
	BaseType_t pxHigherPriorityTaskWoken;
	
   /* 进入临界段，临界段可以嵌套 */
   ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
	/*从邮箱中读出报文*/
	CAN_Receive(CANx, CAN_FIFO0, &RxMessage_CAN);

	/* 判断接收数据的ID*/ 
	if(((RxMessage_CAN.ExtId==ID_SetVout_Back )||(RxMessage_CAN.ExtId==ID_SetOverVoltage_Back)) && (RxMessage_CAN.IDE==CAN_ID_EXT) && (RxMessage_CAN.DLC==8) )
	{
		RxSetMessage_CAN[i] = RxMessage_CAN;
		i++;
		if(i == 3)
		{
			//给出二值信号量 ，发送接收到新数据标志，供前台程序查询
			xSemaphoreGiveFromISR(BinarySem_Handle_CAN_Set,&pxHigherPriorityTaskWoken);	//释放二值信号量
			i = 0;
		}	

	}
		/* 判断接收数据的ID*/ 
	else if((RxMessage_CAN.ExtId==ID_Query_Back) && (RxMessage_CAN.IDE==CAN_ID_EXT) && (RxMessage_CAN.DLC==8) )
	{
		RxRTMessage_CAN[j] = RxMessage_CAN;
		j++;
		if(j == 12)
		{
			//给出二值信号量 ，发送接收到新数据标志，供前台程序查询
			xSemaphoreGiveFromISR(BinarySem_Handle_CAN_RT,&pxHigherPriorityTaskWoken);	//释放二值信号量
			j = 0;
		}	

	}
	else
	{
		printf("未接受到有效的回复数据");
	}
	//如果需要的话进行一次任务切换，系统会判断是否需要进行切换
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}



/************************ 485通讯数据接受中断服务函数**************************/
//中断缓存串口数据
extern RS485_TX_Packet TxMessage_485;				 //接收缓冲区
extern SemaphoreHandle_t BinarySem_Handle_485;

#define     UART_BUFF_SIZE      1024
volatile    uint16_t Uart_Rx_Num = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];

void _485_IRQHandler(void)
{
  uint32_t ulReturn;
  BaseType_t pxHigherPriorityTaskWoken;
	
   /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
  clean_rebuff();//清空缓存区
	
  if(USART_GetITStatus(_485_USART, USART_IT_RXNE) != RESET) //中断产生
	{  
		USART_ClearITPendingBit(_485_USART,USART_IT_RXNE); //清除中断标志
		uart_buff[Uart_Rx_Num] = USART_ReceiveData(_485_USART);//接受一个字节数据
		Uart_Rx_Num++; //长度加1
	}
	
	uart_buff[Uart_Rx_Num-1]=buffer_raw_cksum((void *)uart_buff, sizeof(uart_buff));//计算接受数据校验和位
	
	if( (uart_buff[3] == 0x13)&&(uart_buff[Uart_Rx_Num-1] == TxMessage_485.Check_Digit ) ) //判断最后接收的数据是否为设定值，确定数据正确性
	{
		//给出二值信号量 ，发送接收到新数据标志，供前台程序查询
		xSemaphoreGiveFromISR(BinarySem_Handle_485,&pxHigherPriorityTaskWoken);//释放二值信号量
		//如果需要的话进行一次任务切换，系统会判断是否需要进行切换
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}
	else
	{
			 printf("校验和不一致，传输数据出错");
	}
	
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) //溢出
		{
			USART_ClearFlag(USART2,USART_FLAG_ORE);  //读SR
			USART_ReceiveData(_485_USART); //读DR  
		}    
		
		USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);    

   /* 退出临界段 */
   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

//清空缓冲区
void clean_rebuff(void) 
{

    uint16_t i=UART_BUFF_SIZE+1;
    Uart_Rx_Num = 0;
	while(i)
    uart_buff[--i]=0;

}