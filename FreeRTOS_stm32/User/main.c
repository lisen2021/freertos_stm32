/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2022.3.3
  * @brief   FreeRTOS V9.0.0  + STM32 固件库例程
*/

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_key.h"
#include "./can/bsp_can.h"
#include "bsp_485.h"
#include "bsp_pbdata.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */


static TaskHandle_t CAN1_Set_Task_Handle = NULL;/* 设置整流模块输出电压,过压保护点，限流点的任务句柄 */
static TaskHandle_t CAN1_Query_Task_Handle = NULL;/* 查询整流模块实时数据任务句柄 */
static TaskHandle_t CAN1_Receive_Task_Handle = NULL;/* 接收整流模块反馈数据并进行数据存储任务句柄 */

static TaskHandle_t RS485_Query_Task_Handle = NULL;/* 用于查询485节点任务实现 */
static TaskHandle_t RS485_Receive_Task_Handle = NULL;/* 接收各485模块数据并解析存储任务实现 */
/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
 
SemaphoreHandle_t BinarySem_Handle_CAN_Set =NULL;//CAN设置命令返回数据接受成功后的二值信号量
SemaphoreHandle_t BinarySem_Handle_CAN_RT =NULL;//CAN实时数据接受成功后的二值信号量
SemaphoreHandle_t BinarySem_Handle_485 =NULL;//CAN实时数据接受成功后的二值信号量


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
__IO uint32_t can1_message = 0;	//用于标志can发送的命令，在CAN_Transmit(CanTxMsg *TxMessage)中判断
__IO uint32_t RS485_message = 0;	//用于标志can发送的命令，在CAN_Transmit(CanTxMsg *TxMessage)中判断
__IO uint32_t X_flag0 = 0;      //模拟上位机发来的设定输出电压命令产生的标志位
CanTxMsg TxMessage_CAN;			 //发送缓冲区
CanRxMsg RxMessage_CAN;		     //接收缓冲区

CanRxMsg RxSetMessage_CAN[3];		     //接收设置命令后返回的报文的数据缓冲区
CanRxMsg RxRTMessage_CAN[12];		     //接收实时数据的缓冲区

RS485_TX_Packet TxMessage_485;//485发送数据缓冲区


#define  UART_BUFF_SIZE      1024
extern uint8_t  uart_buff[UART_BUFF_SIZE];
extern volatile    uint16_t Uart_Rx_Num ;


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
static void AppTaskCreate(void);/* 用于创建任务 */
static void CAN1_Set_Task(void* parameter);//用于设置整流模块输出电压,过压保护点，限流点任务实现
static void CAN1_Query_Task(void* pvParameters);/* 用于查询整流模块实时数据任务实现 */
static void CAN1_Receive_Task(void* pvParameters);/* 整流模块数据并解析存储任务实现 */

static void RS485_Query_Task(void* pvParameters);/* 用于查询485节点任务实现 */
static void RS485_Receive_Task(void* pvParameters);/* 接收各485模块数据并解析存储任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();
  
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS固件库例程！\n\n");
  
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */  
  
  
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区

	/* 创建 BinarySem_Handle_CAN_Set */
  BinarySem_Handle_CAN_Set= xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_Set)
    printf("BinarySem_Handle_CAN_Set二值信号量创建成功!\n");
	
	/* 创建 BinarySem_Handle_CAN_RT */
  BinarySem_Handle_CAN_RT = xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_RT)
    printf("BinarySem_Handle_CAN_RT二值信号量创建成功!\n");
  
  	/* 创建 BinarySem_Handle_485 */
  BinarySem_Handle_485 = xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_RT)
    printf("BinarySem_Handle_485二值信号量创建成功!\n");

/***************************************CAN1通讯相关任务************************************************************/
  
 /* 创建CAN1_Set_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_Set_Task, /* 任务入口函数 */
                        (const char*    )"CAN1_Set_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )CAN1_SET_TASK_PRIO,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&CAN1_Set_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建CAN1_Set_Task任务成功!\r\n");
  
  /* 创建CAN1_Query任务 */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_QUREY_TASK_PRIO, /* 任务入口函数 */
                        (const char*    )"CAN1_Query_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )14,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&CAN1_Query_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建CAN1_QueryV_Task任务成功!\r\n");
	
  
  /* 创建CAN1_Receive任务 */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_Receive_Task,  /* 任务入口函数 */
                        (const char*    )"CAN1_Receive_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )CAN1_RX_TASK_PRIO, /* 任务的优先级 */
                        (TaskHandle_t*  )&CAN1_Receive_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建CAN1_Receive_Task任务成功!\r\n");
  
  
  
/***************************************485通讯相关任务************************************************************/
  
  /* 创建RS485_Receive任务 */
  xReturn = xTaskCreate((TaskFunction_t )RS485_Receive_Task,  /* 任务入口函数 */
                        (const char*    )"RS485_Receive_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )RS485_RX_TASK_PRIO, /* 任务的优先级 */
                        (TaskHandle_t*  )&RS485_Receive_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建RS485_Receive_Task任务成功!\r\n");
  
   /* 创建RS485_Query_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )RS485_Query_Task,  /* 任务入口函数 */
                        (const char*    )"RS485_Query_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )RS485_QUREY_TASK_PRIO, /* 任务的优先级 */
                        (TaskHandle_t*  )&RS485_Query_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建RS485_Query_Task任务成功!\r\n");
  
 
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


/********************************任务函数主体部分*************************************************/



/**********************************************************************
  * @ 函数名  ： CAN1_Receive_Task
  * @ 功能说明： can数据接收任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void CAN1_Receive_Task(void* parameter)
{		
	BaseType_t xReturn_Set = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	BaseType_t xReturn_RT = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	printf("\r\n 开始接收任务\r\n");
    while (1)
   {
    //获取设置命令的二值信号量 xSemaphore,没获取到则一直等待
	xReturn_Set = xSemaphoreTake(BinarySem_Handle_CAN_Set, portMAX_DELAY); 
    if(pdPASS == xReturn_Set)
    {	
			printf("\r\nCAN接收了3条数据：\r\n");	
			if( pdPASS == CAN1_Set_handle(RxSetMessage_CAN))//获取结构体数组的指针作为传递参数
			{
				printf("整流模块初始设置完毕");
				vTaskDelete(CAN1_Set_Task_Handle);//删除初始化设置任务，避免重复初始化
			} 
    }
	
	 //获取查询实时数据的二值信号量 xSemaphore,没获取到则一直等待
	xReturn_RT = xSemaphoreTake(BinarySem_Handle_CAN_RT, portMAX_DELAY); 
    if(pdPASS == xReturn_RT)
    {	
			printf("\r\nCAN接收了12条数据：\r\n");	
			if( pdPASS == CAN1_Query_handle(RxRTMessage_CAN))//获取结构体数组的指针作为传递参数
			{
				printf("实时数据接收完毕");
				CAN1_Data_Tostatus(RxRTMessage_CAN);//将接收到的数据存储到状态信息包中 
				
			} 
    }
		
		
	vTaskDelay(10);/* 延时10个tick.因为接受任务的优先级大于发送 */
  }

}

/**********************************************************************
  * @ 函数名  ： CAN1_Set_Task
  * @ 功能说明： 发送命令设置模块的输出电压，过压保护点，限流点
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void CAN1_Set_Task(void* parameter)
{	
	uint16_t  i =0;
  while (1)
  {
		/*按一次按键发送一次数据*/
		if(	X_flag0==0)
		{
			for(i=0;i<3;i++)
			{
				can1_message = i;//设置整流模块输出电压命令标志位
				LED_BLUE;
				CAN_SetMsg(&TxMessage_CAN);//设置报文数据
				CAN_Transmit(CANx, &TxMessage_CAN);//发送命令
				while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//等待发送完毕，可使用CAN_TransmitStatus查看状态
				//vTaskDelay(10);//是否使用延时函数等待一段时间再发送下一条命令待验证
			
			}

			
//      can1_message = 1;//设置整流模块输出电压命令标志位
//			LED_BLUE;
//			CAN_SetMsg(&TxMessage_CAN);//设置报文数据
//			CAN_Transmit(CANx, &TxMessage_CAN);//发送命令
//			while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//等待发送完毕，可使用CAN_TransmitStatus查看状态				
//			
//      can1_message = 2;//设置整流模块输出电压命令标志位
//			LED_BLUE;
//			CAN_SetMsg(&TxMessage_CAN);//设置报文数据
//			CAN_Transmit(CANx, &TxMessage_CAN);//发送命令
//			while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//等待发送完毕，可使用CAN_TransmitStatus查看状态				
//			
//			LED_GREEN;
			
			printf("\r\n已使用CAN发送设置命令！\r\n"); 			
			//CAN_DEBUG_ARRAY(TxMessage_CAN.Data,8); 
		}
    vTaskDelay(20);/* 延时20个tick */
  }
}
/**********************************************************************
  * @ 函数名  ： CAN1_Query_Task
  * @ 功能说明： 发送命令查询模块的输出电压/电流
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void CAN1_Query_Task(void* parameter)
{	
  while (1)
  {
	  
       can1_message = 3;
	   LED_BLUE;
	   CAN_SetMsg(&TxMessage_CAN);//设置报文数据
	   CAN_Transmit(CANx, &TxMessage_CAN);//发送命令
	   while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};
		 //vTaskDelay(10);//等待发送完毕，可使用CAN_TransmitStatus查看状态
			
	   LED_GREEN;
			
	   printf("\r\n已使用CAN发送查询实时数据命令！\r\n"); 			
	   //CAN_DEBUG_ARRAY(TxMessage_CAN.Data,8); 
	    
       vTaskDelay(30);/* 延时30个tick */
  }
}



/*****************************************RS485通讯任务实现******************************************************************************/
/**********************************************************************
  * @ 函数名  ： RS485_Receive_Task
  * @ 功能说明： 接收485上各节点的应答数据
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void RS485_Receive_Task(void* pvParameters)/* 接收各485模块数据并解析存储任务实现 */
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	printf("\r\n 欢迎使用野火  STM32 F407 开发板。\r\n");
    printf("\r\n 485通讯回环实验例程\r\n");
	

  while (1)
  {
    //获取二值信号量 xSemaphore,没获取到则一直等待
	xReturn = xSemaphoreTake(BinarySem_Handle_485,/* 二值信号量句柄 */
                              portMAX_DELAY); /* 等待时间 */
    if(pdPASS == xReturn)
    {	
		printf("\r\nCAN接收到数据：\r\n");
		_485_DEBUG_ARRAY(uart_buff, 30);//显示接受数据内容
		RS485_Data_Tostatus();	//保存数据到状态包

    }
  }

}
/**********************************************************************
  * @ 函数名  ： RS485_Query_Task
  * @ 功能说明： 发送命令查询查询485节点
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void RS485_Query_Task(void* pvParameters)/* 用于查询485节点实现 */
{
  while (1)
  {
	  
	  uint16_t i=0;
	  for(i=0;i<6;i++)
	  {
		RS485_message = i;
		RS485_SetMsg(&TxMessage_485);//设置报文数据	
		_485_SendStr_length((uint8_t *) &TxMessage_485,18 );//发送命令
      
	  }
	  vTaskDelay(20);/* 延时20个tick */  
  }

}


/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 串口初始化	*/
	Debug_USART_Config();
  
    /* 按键初始化	*/
    Key_GPIO_Config();
	
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
	
	/*初始化485,在中断接收485数据包*/
	_485_Config();
}

/********************************END OF FILE****************************/
