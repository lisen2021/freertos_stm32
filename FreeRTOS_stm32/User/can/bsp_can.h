#ifndef __CAN_H
#define	__CAN_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "portmacro.h"
#include "projdefs.h"


#define CANx                       	CAN2
#define CAN_CLK                    RCC_APB1Periph_CAN1 |RCC_APB1Periph_CAN2
#define CAN_RX_IRQ									CAN2_RX0_IRQn
#define CAN_RX_IRQHandler					CAN2_RX0_IRQHandler

#define CAN_RX_PIN                 GPIO_Pin_12
#define CAN_TX_PIN                 GPIO_Pin_13
#define CAN_TX_GPIO_PORT          GPIOB
#define CAN_RX_GPIO_PORT          GPIOB
#define CAN_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define CAN_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define CAN_AF_PORT                GPIO_AF_CAN2
#define CAN_RX_SOURCE              GPIO_PinSource12
#define CAN_TX_SOURCE              GPIO_PinSource13 

#define ID_SetVout                 ((uint32_t)0x108180FE)//该ID代表主机向整流发送设置输出电压命令
#define ID_SetOverVoltage          ((uint32_t)0x108181FE)//该ID代表主机向整流发送设置过压保护点命令
#define ID_SetCurrentLimit         ((uint32_t)0x108181FE)//该ID代表主机向整流发送设置限流点命令
#define ID_SetVout_Back            ((uint32_t)0x1081807E)//该ID代表主机向整流发送设置输出电压命令
#define ID_SetOverVoltage_Back     ((uint32_t)0x1081817E)//该ID代表主机向整流发送设置过压保护点命令
#define ID_Query                   ((uint32_t)0x108140FF) //该ID代表主机向整流发送查询命令
#define ID_Query_Back              ((uint32_t)0x1081407F) //该ID代表主机向整流发送查询命令



/* Mailboxes definition */
#define CAN_TXMAILBOX_0   ((uint8_t)0x00)
#define CAN_TXMAILBOX_1   ((uint8_t)0x01)
#define CAN_TXMAILBOX_2   ((uint8_t)0x02) 
/*debug*/

#define CAN_DEBUG_ON          1
#define CAN_DEBUG_ARRAY_ON   1
#define CAN_DEBUG_FUNC_ON    1
   
   
// Log define
#define CAN_INFO(fmt,arg...)           printf("<<-CAN-INFO->> "fmt"\n",##arg)
#define CAN_ERROR(fmt,arg...)          printf("<<-CAN-ERROR->> "fmt"\n",##arg)
#define CAN_DEBUG(fmt,arg...)          do{\
                                         if(CAN_DEBUG_ON)\
                                         printf("<<-CAN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)

#define CAN_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(CAN_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-CAN-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define CAN_DEBUG_FUNC()               do{\
                                         if(CAN_DEBUG_FUNC_ON)\
                                         printf("<<-CAN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)






static void CAN_GPIO_Config(void);
static void CAN_NVIC_Config(void);
static void CAN_Mode_Config(void);
static void CAN_Filter_Config(void);
void CAN_Config(void);
void CAN_SetMsg(CanTxMsg *TxMessage);
void Init_RxMes(CanRxMsg *RxMessage);
BaseType_t CAN1_Set_handle(CanRxMsg *RxSetMessage_CAN);	
BaseType_t CAN1_Query_handle(CanRxMsg *RxRTMessage_CAN);																			 

#endif







