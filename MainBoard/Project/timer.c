/****************************************************************************************************/
/*			@FileName		:	delay.c															  	*/
/*			@Author			:	zhengbaiqiu															*/
/*			@Version		:	v1.0																*/
/*			@Date			:	2019.11.12  														*/
/****************************************************************************************************/


/* Includes --------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Project ---------------------------------------------------------------------*/
#include "timer.h" 

/* Variable --------------------------------------------------------------------*/
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

/* Function --------------------------------------------------------------------*/
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);




/*****************************************************************************
 * @name       :void delay_init(u8 SYSCLK)
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :Configure system tick delay
 * @parameters :system clock
 * @retvalue   :None
******************************************************************************/
void delay_init(u8 SYSCLK)  //SYSCLK:ϵͳʱ��
{
	//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}	

/*****************************************************************************
 * @name       :void delay_ms(u16 nms)
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :delay ms
 * @parameters :nms
 * @retvalue   :None
******************************************************************************/
void delay_ms(u16 nms)
{	
	//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:nms<=0xffffff*8*1000/SYSCLK
	//SYSCLK��λΪHz,nms��λΪms ��72M������,nms<=1864 
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}  

/*****************************************************************************
 * @name       :void delay_us(u32 nus)
 * @date       :2020-09-19 
 * @author     :zhengbaiqiu 
 * @function   :delay us
 * @parameters :nus
 * @retvalue   :None
******************************************************************************/	    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}




































