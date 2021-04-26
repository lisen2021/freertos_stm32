#include "bsp_pbdata.h"
#include "stdio.h"
#include "string.h"
#include "./can/bsp_can.h"
#include "time.h"


extern CanRxMsg RxMessage_CAN;				 //接收缓冲区

#define  UART_BUFF_SIZE      1024
extern uint8_t     uart_buff[UART_BUFF_SIZE];
extern volatile    uint16_t Uart_Rx_Num ;

StatusPacket sta1;

//RS485_RX_PV_Packet PV_RXMessage_485;//485接收光伏数据缓冲区
//RS485_RX_DC_Packet DC_RXMessage_485;//485接收双向DC-DC数据缓冲区
//RS485_RX_Load_Packet Load_RXMessage_485;//485接收负载数据缓冲区


uint16_t Data_Integrate(uint8_t a,uint8_t b);
char * time_to_timestamp(void);

/*状态数据包初始化*/
void StatusPacket_Init(StatusPacket *SP)
{
	int i = 0;
	strcpy (SP->TypeMark, "pi" );
    SP->PacketMessage = (uint32_t)0x02;//代表第0个数据包，数据包长度为128（用2代表）
	strcpy (SP->StationID, "HB.L0000" );//台网代码.台站ID，目前定为湖北.0
	SP->Time = (uint32_t)time_to_timestamp(); 
	
	SP->SVA = 0;
	SP->SVAW = 0;
	SP->SIA = 0;
	SP->SIAW = 0;
	SP->SVB = 0;
	SP->SVBW = 0;
	SP->SIB = 0;
	SP->SIBW = 0;
	
	SP->S1_State = 1;//表示该路负载为开启状态
	SP->S1_OV = 0;
	SP->S1_Current = 0;
	SP->S1_CurrentW = 0;
	
	SP->S2_State = 1;//表示该路负载为开启状态
	SP->S2_OV = 0;
	SP->S2_Current = 0;
	SP->S2_CurrentW = 0;
	
	SP->S3_State = 0;//表示该路负载为关闭状态
	SP->S3_OV = 0;
	SP->S3_Current = 0;
	SP->S3_CurrentW = 0;
	
	SP->S4_State = 0;//表示该路负载为关闭状态
	SP->S4_OV = 0;
	SP->S4_Current = 0;
	SP->S4_CurrentW = 0;
	
	SP->S5_State = 0;//表示该路负载为关闭状态
	SP->S5_OV = 0;
	SP->S5_Current = 0;
	SP->S5_CurrentW = 0;
	
	SP->S6_State = 0;//表示该路负载为关闭状态
	SP->S6_OV = 0;
	SP->S6_Current = 0;
	SP->S6_CurrentW = 0;
	
    SP->TemperA = 250;
	SP->TemperB = 250;
	SP->TemperW = 0;
	
	SP->HumidityA = 65;
	SP->HumidityB = 65;
	SP->HumidityW = 0;
	
	SP->AP = 13332;
	
	SP->SwitchA_Value = 0;
	SP->SwitchB_Value = 0;
	SP->SwitchC_Value = 0;
	SP->SwitchD_Value = 0;
	SP->SwitchE_Value = 0;
	
	for(i=0;i<22;i++)
	{
	   SP->RC[i] = 0;
	}
}


/*485数据存储到状态包*/
void RS485_Data_Tostatus()
{
	
    switch (uart_buff[4]){
        case 0x10 :if(uart_buff[5]== 0x01)
				          {					
										sta1.SVA =  Data_Integrate(uart_buff[10],uart_buff[9]) ;
										sta1.SVAW = Data_Integrate(uart_buff[12],uart_buff[11]) ;
										sta1.SIA =  Data_Integrate(uart_buff[14],uart_buff[13]) ;
										sta1.SIAW = Data_Integrate(uart_buff[16],uart_buff[15]) ;
				          };  break;
						  
        case 0x11 :{
//										DC_RXMessage_485.BatteryNum = uart_buff[9] ;
//										DC_RXMessage_485.Data_AV = Data_Integrate(uart_buff[11],uart_buff[10]) ;
//										DC_RXMessage_485.Data_AP = Data_Integrate(uart_buff[13],uart_buff[12]) ;
//										DC_RXMessage_485.Data_AT = Data_Integrate(uart_buff[15],uart_buff[14]) ;
//										DC_RXMessage_485.Data_AR = Data_Integrate(uart_buff[17],uart_buff[16]) ;
//										DC_RXMessage_485.Data_BV = Data_Integrate(uart_buff[19],uart_buff[18]) ;
//										DC_RXMessage_485.Data_BP = Data_Integrate(uart_buff[21],uart_buff[20]) ;
//										DC_RXMessage_485.Data_BT = Data_Integrate(uart_buff[23],uart_buff[22]) ;
//										DC_RXMessage_485.Data_BR = Data_Integrate(uart_buff[25],uart_buff[24]) ;
									  
										
									
				   };   break;
		
        case 0x12 :{
									  
						    sta1.S1_State =       Data_Integrate(uart_buff[11],uart_buff[10]) ;
						    sta1.S1_OV =          Data_Integrate(uart_buff[13],uart_buff[12]) ;
						    sta1.S1_Current =     Data_Integrate(uart_buff[15],uart_buff[14]) ;
						    sta1.S1_CurrentW=     Data_Integrate(uart_buff[17],uart_buff[16]) ;
						    sta1.S2_State =       Data_Integrate(uart_buff[19],uart_buff[18]) ;
						    sta1.S2_OV =          Data_Integrate(uart_buff[21],uart_buff[20]) ;
						    sta1.S2_Current=      Data_Integrate(uart_buff[23],uart_buff[22]) ;
						    sta1.S2_CurrentW =    Data_Integrate(uart_buff[25],uart_buff[24]) ;
							sta1.S3_State =       Data_Integrate(uart_buff[27],uart_buff[26]) ;
						    sta1.S3_OV =          Data_Integrate(uart_buff[29],uart_buff[28]) ;
						    sta1.S3_Current=      Data_Integrate(uart_buff[31],uart_buff[30]) ;
						    sta1.S3_CurrentW =    Data_Integrate(uart_buff[33],uart_buff[32]) ;
							sta1.S4_State =       Data_Integrate(uart_buff[35],uart_buff[34]) ;
						    sta1.S4_OV =          Data_Integrate(uart_buff[37],uart_buff[36]) ;
						    sta1.S4_Current=      Data_Integrate(uart_buff[39],uart_buff[38]) ;
						    sta1.S4_CurrentW =    Data_Integrate(uart_buff[41],uart_buff[40]) ;
			                sta1.S5_State =       Data_Integrate(uart_buff[43],uart_buff[42]) ;
						    sta1.S5_OV =          Data_Integrate(uart_buff[45],uart_buff[44]) ;
						    sta1.S5_Current=      Data_Integrate(uart_buff[47],uart_buff[46]) ;
						    sta1.S5_CurrentW =    Data_Integrate(uart_buff[49],uart_buff[48]) ;
							sta1.S6_State =       Data_Integrate(uart_buff[51],uart_buff[50]) ;
						    sta1.S6_OV =          Data_Integrate(uart_buff[53],uart_buff[52]) ;
						    sta1.S6_Current=      Data_Integrate(uart_buff[55],uart_buff[54]) ;
						    sta1.S6_CurrentW =    Data_Integrate(uart_buff[57],uart_buff[56]) ;
									
				    };   break;

        default:printf("error\n");
    }
}

/*CAN数据保存到状态数据包函数*/
void CAN1_Data_Tostatus(CanRxMsg *RxRTMessage_CAN)
{
   uint16_t DATA_Head;//数据前2个字节
   uint8_t  DATA_ERRORTYPE; //错误类型
   uint16_t DATA_ID;//信号id
   uint16_t DATA;//后两个字节的数据内容
   uint8_t i = 0;
   uint8_t j = 6;
   
   for(i=0;i<2;i++)
	{
		  DATA_Head = Data_Integrate(RxRTMessage_CAN[j].Data[1],RxRTMessage_CAN[j].Data[0]);//整合前两字节到一个16位缓冲区
		  DATA_ERRORTYPE= (uint8_t)((DATA_Head&0xf000)>>12);//取高4位数据
		  DATA_ID = (DATA_Head&0x0fff);//取低12位数据
		  
		 if(DATA_ERRORTYPE  == NOERROR)//判断传输数据无错误
		 {
			//根据信号ID将数据存储到状态信息包的不同位置
			switch(DATA_ID){
				case Output_V : {  	DATA  = Data_Integrate(RxRTMessage_CAN[j].Data[7],RxRTMessage_CAN[j].Data[6]);
									sta1.SVB = DATA ;
								} ; break;

				case Output_Current : {  DATA  = Data_Integrate(RxRTMessage_CAN[j].Data[7],RxRTMessage_CAN[j].Data[6]);
										 sta1.SIB = DATA ;
									  } ; break;
			}
		
		 }
		 else{
		  printf("数据传输出错！");
		 }
		 
		 j=10;
    }

}

/*将两个8位字节数据存储到16位数据中*/
uint16_t Data_Integrate(uint8_t a,uint8_t b)
{
     
    uint16_t  Data_16=0;
    uint8_t *p;
    p = ( uint8_t*)(&Data_16);
 
	*p = a;
	*(p+1) = b;
 
   return Data_16;
}

/*时间戳函数，获取当前时间并转换为距离1970年01月01日00时00分00秒的秒数*/
char * time_to_timestamp(void)
{
	unsigned int timestamp;
	struct tm stm;
	char *timestamp_buf;
	char *buf;
	timestamp_buf = (char *)malloc(10);
	buf = (char *)malloc(100);
	
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef	RTC_TimeStruct;
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	
	stm.tm_year = RTC_DateStruct.RTC_Year + 100;	//RTC_Year rang 0-99,but tm_year since 1900
	stm.tm_mon	= RTC_DateStruct.RTC_Month-1;		//RTC_Month rang 1-12,but tm_mon rang 0-11
	stm.tm_mday	= RTC_DateStruct.RTC_Date;			//RTC_Date rang 1-31 and tm_mday rang 1-31
	stm.tm_hour	= RTC_TimeStruct.RTC_Hours-8;			//RTC_Hours rang 0-23 and tm_hour rang 0-23
	stm.tm_min	= RTC_TimeStruct.RTC_Minutes;   //RTC_Minutes rang 0-59 and tm_min rang 0-59
	stm.tm_sec	= RTC_TimeStruct.RTC_Seconds;		

	sprintf(timestamp_buf,"%u",mktime(&stm));
	return timestamp_buf;
}