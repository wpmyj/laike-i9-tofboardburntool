/****************************************************************************************************/
/*			@FileName		:	TK_TestController.c												  	*/
/*			@Author			:	zhengbaiqiu															*/
/*			@Version		:	v1.0																*/
/*			@Date			:	2019.11.12  														*/
/****************************************************************************************************/


/* Includes --------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Project ---------------------------------------------------------------------*/
#include "maindef.h"
#include "bsp.h"
#include "timer.h"
#include "apiLib.h"
#include "TK_TestController.h"


/* Variable --------------------------------------------------------------------*/
/* Function --------------------------------------------------------------------*/
void TK_TestController( void *pvParameters );
void api_SendCMD_To_Display(u8 mode,u8 cmd,u8 status);
u8 api_SendCMD_To_MainBoard(u8 seq,u8 mode,u8 cmd,u16 waittime,u8 type,u8 num);
u8 iiiiiiiii;


/*****************************************************************************
 * @name       :void TK_TestController( void *pvParameters )
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :ʱ���������
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
u8 iiiii;
void TK_TestController( void *pvParameters )
{
	//api_MachinePowerOn();
	for(;;)
	{
		IWDG_ReloadCounter();
				
		//api_Led_Status_Set(0,1);
		//api_Led_Status_Set(1,1);
		//api_Control_Motor(0,1);
// 		api_Control_Motor(1,1);
		
// 		api_Control_Motor(13,1);
// 		PCout(7) = 1;
		
		if(mMaininf.mUart1.mReceiveFlag == 1)   //����
		{
			mMaininf.mUart1.mReceiveFlag = 0;
			api_UART2_Display_SendCMDData(2,1,1);
 			vTaskDelay(1000);
			mMaininf.mWork.mWorkChecking = 1;
			PAout(12) = 1;
			PAout(15) = 1;
			PEout(1) = 0;
			vTaskDelay(10000);
			api_SendCMD_To_MainBoard(0,1,1,100,1,5);
			mMaininf.mWork.mWorkStep = mMaininf.mUart1.ReceiveBuf[2];
		}
		
		if((mMaininf.mWork.mWorkChecking == 0) && (mMaininf.mWork.mWorkKeyPressFlag == 1))
		{
			mMaininf.mWork.mWorkKeyPressFlag = 0;
			mMaininf.mWork.mWorkKeyPressFlag1 = 0;
			mMaininf.mWork.mWorkFirstLedFlag = 0;
			mMaininf.mWork.mWorkBurnLed[ENUM_BUZZ].mCountDownFlag = 0;
			api_Led_Status_Set(0,0);
			api_Led_Status_Set(1,2);
			api_Control_Motor(13,1);//����
			vTaskDelay(1000);
			PCout(7) = 1;//����
			vTaskDelay(500);
			PCout(7) = 0;
			mMaininf.mSystem.mBurnStartTime = api_GetCurrentSystemTime();
			vTaskDelay(1600);
			mMaininf.mWork.mWorkChecking = 1;
			
		}
		
		if(mMaininf.mWork.mWorkChecking == 1)
		{
			if((api_GetLedTimeFlag(ENUM_GREEN_LED) == 0) && (api_GetLedTimeFlag(ENUM_RED_LED) == 0))
			{
				api_Led_Status_Set(0,1);
				api_Led_Status_Set(1,0);
				api_Control_Motor(13,0);
				mMaininf.mWork.mWorkChecking = 0;
			}
			
			if((api_GetGuideFlag(ENUM_GREEN_LED) == 1) && (api_GetGuideFlag(ENUM_RED_LED) == 1))
			{
				if(mMaininf.mWork.mWorkFirstLedFlag == 0)
				{
					mMaininf.mWork.mWorkFirstLedFlag = 1;
					mMaininf.mSystem.mLedDoubleFlashStartTime = api_GetCurrentSystemTime();
				}
				
				if(api_GetSystemTimePeriod(mMaininf.mSystem.mLedDoubleFlashStartTime) < 3000)
				{
					if(api_GetGuideFlag(ENUM_BUZZ) == 1)
					{
						mMaininf.mWork.mWorkBurnLed[ENUM_BUZZ].mCountDownFlag = 0;
						
						api_Led_Status_Set(0,0);
						api_Led_Status_Set(1,1);
						api_Control_Motor(13,0);
						mMaininf.mWork.mWorkChecking = 0;
					}
				}
				else
				{
					api_Led_Status_Set(0,1);
					api_Led_Status_Set(1,0);
					api_Control_Motor(13,0);
					mMaininf.mWork.mWorkChecking = 0;
				}
				
			}
			if((api_GetGuideFlag(ENUM_GREEN_LED) == 2) && (api_GetGuideFlag(ENUM_RED_LED) == 2))
			{
				api_Led_Status_Set(0,1);
				api_Led_Status_Set(1,0);
				api_Control_Motor(13,0);
				mMaininf.mWork.mWorkChecking = 0;
			}
			
			
			//��¼��ʱ
			if(api_GetSystemTimePeriod(mMaininf.mSystem.mBurnStartTime) > 40000)
			{
				api_Led_Status_Set(0,1);
				api_Led_Status_Set(1,0);
				api_Control_Motor(13,0);
				mMaininf.mWork.mWorkChecking = 0;
			}
		}
	}
}

/*****************************************************************************
 * @name       :void api_SendCMD_To_Display(u8 mode,u8 cmd,u8 status)
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :send data
 * @parameters :cmd
 * @retvalue   :None
******************************************************************************/
void api_SendCMD_To_Display(u8 mode,u8 cmd,u8 status)
{
	u8 mCont = 0;
	u8 mStatus = 0;
	
	do
	{
		if(++mCont > 3)
		{
			return;
		}
		
		api_UART2_Display_SendCMDData(mode,cmd,status);
		vTaskDelay(1000);
		if((mMaininf.mUart2.mReceiveFlag == 1) && (mMaininf.mUart2.ReceiveBuf[0] == 1) && (mMaininf.mUart2.ReceiveBuf[1] == cmd) &&
		   (mMaininf.mUart2.ReceiveBuf[2] == 1))
		{
			mMaininf.mUart2.mReceiveFlag = 0;
			mStatus = 1;
		}
	}while(mStatus == 0);
	
}

/*****************************************************************************
 * @name       :void api_SendCMD_To_MainBoard(u8 seq,u8 mode,u8 cmd,u16 waittime)
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :�����������
 * @parameters :testtype : 0:��ȡ����   1:��ⷢ��    2:������   waittime:���μ��ʱ��  
				type:����0��״̬  1������     num:�ط�����
 * @retvalue   :0:���������ݷ���     1�����������ݷ���
******************************************************************************/
u8 api_SendCMD_To_MainBoard(u8 seq,u8 mode,u8 cmd,u16 waittime,u8 type,u8 num)
{
	u8 mCont = 0;
	
	mMaininf.mUart1.ReceiveBuf[4] = 0;
	
	if(type == 0)
	{
		do{
			if(++mCont > num) return 0;//δ���յ�����
			api_UART1_MainBoard_SendCMDData(seq,mode,cmd);
			vTaskDelay(waittime);
		}while((mMaininf.mUart1.mReceiveFlag == 0) || ((mMaininf.mUart1.mReceiveFlag == 1) && (mMaininf.mUart1.ReceiveBuf[4] == 0)));
		
	}
	else
	{
		do{
			if(++mCont > num) return 0;//δ���յ�����
			api_UART1_MainBoard_SendCMDData(seq,mode,cmd);
			vTaskDelay(waittime);
		}while(mMaininf.mUart1.mReceiveFlag == 0);
		
	}
	
	mMaininf.mUart1.mReceiveFlag = 0;
	
	return 1;//���յ�����
}

