/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2022.3.3
  * @brief   FreeRTOS V9.0.0  + STM32 �̼�������
*/

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_key.h"
#include "./can/bsp_can.h"
#include "bsp_485.h"
#include "bsp_pbdata.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */


static TaskHandle_t CAN1_Set_Task_Handle = NULL;/* ��������ģ�������ѹ,��ѹ�����㣬������������� */
static TaskHandle_t CAN1_Query_Task_Handle = NULL;/* ��ѯ����ģ��ʵʱ���������� */
static TaskHandle_t CAN1_Receive_Task_Handle = NULL;/* ��������ģ�鷴�����ݲ��������ݴ洢������ */

static TaskHandle_t RS485_Query_Task_Handle = NULL;/* ���ڲ�ѯ485�ڵ�����ʵ�� */
static TaskHandle_t RS485_Receive_Task_Handle = NULL;/* ���ո�485ģ�����ݲ������洢����ʵ�� */
/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
 
SemaphoreHandle_t BinarySem_Handle_CAN_Set =NULL;//CAN������������ݽ��ܳɹ���Ķ�ֵ�ź���
SemaphoreHandle_t BinarySem_Handle_CAN_RT =NULL;//CANʵʱ���ݽ��ܳɹ���Ķ�ֵ�ź���
SemaphoreHandle_t BinarySem_Handle_485 =NULL;//CANʵʱ���ݽ��ܳɹ���Ķ�ֵ�ź���


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

__IO uint32_t flag = 0;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
__IO uint32_t can1_message = 0;	//���ڱ�־can���͵������CAN_Transmit(CanTxMsg *TxMessage)���ж�
__IO uint32_t RS485_message = 0;	//���ڱ�־can���͵������CAN_Transmit(CanTxMsg *TxMessage)���ж�
__IO uint32_t X_flag0 = 0;      //ģ����λ���������趨�����ѹ��������ı�־λ
CanTxMsg TxMessage_CAN;			 //���ͻ�����
CanRxMsg RxMessage_CAN;		     //���ջ�����

CanRxMsg RxSetMessage_CAN[3];		     //������������󷵻صı��ĵ����ݻ�����
CanRxMsg RxRTMessage_CAN[12];		     //����ʵʱ���ݵĻ�����

RS485_TX_Packet TxMessage_485;//485�������ݻ�����


#define  UART_BUFF_SIZE      1024
extern uint8_t  uart_buff[UART_BUFF_SIZE];
extern volatile    uint16_t Uart_Rx_Num ;


/*
*************************************************************************
*                             ��������
*************************************************************************
*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
static void AppTaskCreate(void);/* ���ڴ������� */
static void CAN1_Set_Task(void* parameter);//������������ģ�������ѹ,��ѹ�����㣬����������ʵ��
static void CAN1_Query_Task(void* pvParameters);/* ���ڲ�ѯ����ģ��ʵʱ��������ʵ�� */
static void CAN1_Receive_Task(void* pvParameters);/* ����ģ�����ݲ������洢����ʵ�� */

static void RS485_Query_Task(void* pvParameters);/* ���ڲ�ѯ485�ڵ�����ʵ�� */
static void RS485_Receive_Task(void* pvParameters);/* ���ո�485ģ�����ݲ������洢����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */  
  
  
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���

	/* ���� BinarySem_Handle_CAN_Set */
  BinarySem_Handle_CAN_Set= xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_Set)
    printf("BinarySem_Handle_CAN_Set��ֵ�ź��������ɹ�!\n");
	
	/* ���� BinarySem_Handle_CAN_RT */
  BinarySem_Handle_CAN_RT = xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_RT)
    printf("BinarySem_Handle_CAN_RT��ֵ�ź��������ɹ�!\n");
  
  	/* ���� BinarySem_Handle_485 */
  BinarySem_Handle_485 = xSemaphoreCreateBinary();	 
  
  if(NULL != BinarySem_Handle_CAN_RT)
    printf("BinarySem_Handle_485��ֵ�ź��������ɹ�!\n");

/***************************************CAN1ͨѶ�������************************************************************/
  
 /* ����CAN1_Set_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_Set_Task, /* ������ں��� */
                        (const char*    )"CAN1_Set_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )CAN1_SET_TASK_PRIO,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&CAN1_Set_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����CAN1_Set_Task����ɹ�!\r\n");
  
  /* ����CAN1_Query���� */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_QUREY_TASK_PRIO, /* ������ں��� */
                        (const char*    )"CAN1_Query_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )14,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&CAN1_Query_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����CAN1_QueryV_Task����ɹ�!\r\n");
	
  
  /* ����CAN1_Receive���� */
  xReturn = xTaskCreate((TaskFunction_t )CAN1_Receive_Task,  /* ������ں��� */
                        (const char*    )"CAN1_Receive_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )CAN1_RX_TASK_PRIO, /* ��������ȼ� */
                        (TaskHandle_t*  )&CAN1_Receive_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����CAN1_Receive_Task����ɹ�!\r\n");
  
  
  
/***************************************485ͨѶ�������************************************************************/
  
  /* ����RS485_Receive���� */
  xReturn = xTaskCreate((TaskFunction_t )RS485_Receive_Task,  /* ������ں��� */
                        (const char*    )"RS485_Receive_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )RS485_RX_TASK_PRIO, /* ��������ȼ� */
                        (TaskHandle_t*  )&RS485_Receive_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����RS485_Receive_Task����ɹ�!\r\n");
  
   /* ����RS485_Query_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )RS485_Query_Task,  /* ������ں��� */
                        (const char*    )"RS485_Query_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )RS485_QUREY_TASK_PRIO, /* ��������ȼ� */
                        (TaskHandle_t*  )&RS485_Query_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����RS485_Query_Task����ɹ�!\r\n");
  
 
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}


/********************************���������岿��*************************************************/



/**********************************************************************
  * @ ������  �� CAN1_Receive_Task
  * @ ����˵���� can���ݽ�������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void CAN1_Receive_Task(void* parameter)
{		
	BaseType_t xReturn_Set = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	BaseType_t xReturn_RT = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	printf("\r\n ��ʼ��������\r\n");
    while (1)
   {
    //��ȡ��������Ķ�ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
	xReturn_Set = xSemaphoreTake(BinarySem_Handle_CAN_Set, portMAX_DELAY); 
    if(pdPASS == xReturn_Set)
    {	
			printf("\r\nCAN������3�����ݣ�\r\n");	
			if( pdPASS == CAN1_Set_handle(RxSetMessage_CAN))//��ȡ�ṹ�������ָ����Ϊ���ݲ���
			{
				printf("����ģ���ʼ�������");
				vTaskDelete(CAN1_Set_Task_Handle);//ɾ����ʼ���������񣬱����ظ���ʼ��
			} 
    }
	
	 //��ȡ��ѯʵʱ���ݵĶ�ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
	xReturn_RT = xSemaphoreTake(BinarySem_Handle_CAN_RT, portMAX_DELAY); 
    if(pdPASS == xReturn_RT)
    {	
			printf("\r\nCAN������12�����ݣ�\r\n");	
			if( pdPASS == CAN1_Query_handle(RxRTMessage_CAN))//��ȡ�ṹ�������ָ����Ϊ���ݲ���
			{
				printf("ʵʱ���ݽ������");
				CAN1_Data_Tostatus(RxRTMessage_CAN);//�����յ������ݴ洢��״̬��Ϣ���� 
				
			} 
    }
		
		
	vTaskDelay(10);/* ��ʱ10��tick.��Ϊ������������ȼ����ڷ��� */
  }

}

/**********************************************************************
  * @ ������  �� CAN1_Set_Task
  * @ ����˵���� ������������ģ��������ѹ����ѹ�����㣬������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void CAN1_Set_Task(void* parameter)
{	
	uint16_t  i =0;
  while (1)
  {
		/*��һ�ΰ�������һ������*/
		if(	X_flag0==0)
		{
			for(i=0;i<3;i++)
			{
				can1_message = i;//��������ģ�������ѹ�����־λ
				LED_BLUE;
				CAN_SetMsg(&TxMessage_CAN);//���ñ�������
				CAN_Transmit(CANx, &TxMessage_CAN);//��������
				while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
				//vTaskDelay(10);//�Ƿ�ʹ����ʱ�����ȴ�һ��ʱ���ٷ�����һ���������֤
			
			}

			
//      can1_message = 1;//��������ģ�������ѹ�����־λ
//			LED_BLUE;
//			CAN_SetMsg(&TxMessage_CAN);//���ñ�������
//			CAN_Transmit(CANx, &TxMessage_CAN);//��������
//			while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬				
//			
//      can1_message = 2;//��������ģ�������ѹ�����־λ
//			LED_BLUE;
//			CAN_SetMsg(&TxMessage_CAN);//���ñ�������
//			CAN_Transmit(CANx, &TxMessage_CAN);//��������
//			while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬				
//			
//			LED_GREEN;
			
			printf("\r\n��ʹ��CAN�����������\r\n"); 			
			//CAN_DEBUG_ARRAY(TxMessage_CAN.Data,8); 
		}
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}
/**********************************************************************
  * @ ������  �� CAN1_Query_Task
  * @ ����˵���� ���������ѯģ��������ѹ/����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void CAN1_Query_Task(void* parameter)
{	
  while (1)
  {
	  
       can1_message = 3;
	   LED_BLUE;
	   CAN_SetMsg(&TxMessage_CAN);//���ñ�������
	   CAN_Transmit(CANx, &TxMessage_CAN);//��������
	   while(CAN_TransmitStatus(CANx,CAN_TXMAILBOX_0)==CAN_TxStatus_Failed){};
		 //vTaskDelay(10);//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
			
	   LED_GREEN;
			
	   printf("\r\n��ʹ��CAN���Ͳ�ѯʵʱ�������\r\n"); 			
	   //CAN_DEBUG_ARRAY(TxMessage_CAN.Data,8); 
	    
       vTaskDelay(30);/* ��ʱ30��tick */
  }
}



/*****************************************RS485ͨѶ����ʵ��******************************************************************************/
/**********************************************************************
  * @ ������  �� RS485_Receive_Task
  * @ ����˵���� ����485�ϸ��ڵ��Ӧ������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void RS485_Receive_Task(void* pvParameters)/* ���ո�485ģ�����ݲ������洢����ʵ�� */
{
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	printf("\r\n ��ӭʹ��Ұ��  STM32 F407 �����塣\r\n");
    printf("\r\n 485ͨѶ�ػ�ʵ������\r\n");
	

  while (1)
  {
    //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
	xReturn = xSemaphoreTake(BinarySem_Handle_485,/* ��ֵ�ź������ */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
    if(pdPASS == xReturn)
    {	
		printf("\r\nCAN���յ����ݣ�\r\n");
		_485_DEBUG_ARRAY(uart_buff, 30);//��ʾ������������
		RS485_Data_Tostatus();	//�������ݵ�״̬��

    }
  }

}
/**********************************************************************
  * @ ������  �� RS485_Query_Task
  * @ ����˵���� ���������ѯ��ѯ485�ڵ�
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void RS485_Query_Task(void* pvParameters)/* ���ڲ�ѯ485�ڵ�ʵ�� */
{
  while (1)
  {
	  
	  uint16_t i=0;
	  for(i=0;i<6;i++)
	  {
		RS485_message = i;
		RS485_SetMsg(&TxMessage_485);//���ñ�������	
		_485_SendStr_length((uint8_t *) &TxMessage_485,18 );//��������
      
	  }
	  vTaskDelay(20);/* ��ʱ20��tick */  
  }

}


/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	Debug_USART_Config();
  
    /* ������ʼ��	*/
    Key_GPIO_Config();
	
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	
	/*��ʼ��485,���жϽ���485���ݰ�*/
	_485_Config();
}

/********************************END OF FILE****************************/
