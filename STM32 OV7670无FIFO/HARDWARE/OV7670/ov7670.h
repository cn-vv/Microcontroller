#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"



//VSYNC         PG15	��֡ͬ���źţ�out��
//HREF			PD6 	����ͬ���źţ�out��
//XCLK			PA8		��ʱ���źţ�in��
//PCLK			PB4		������ʱ�ӣ�out��
//PWDN      PB3		������ѡ��ģʽ  0���� 1POWER DOWN������ʹ������
//RESET			PG14	����λ�ˣ�0RESET 1һ��ģʽ ������ʹ������

#define OV7670_VSYNC 	PGin(15)
#define OV7670_HREF		PDin(6)
#define	OV7670_XCLK		PAin(8)
#define	OV7670_PCLK		PBin(4)
#define	OV7670_PWDN		PBin(3)
#define	OV7670_RESET	PGin(14)

#define OV7670_DATA   GPIO_ReadInputData(GPIOC,0x00FF) 					//��������˿�
//GPIOC->IDR&0x00FF 
/////////////////////////////////////////
#define CHANGE_REG_NUM 							171			//��Ҫ���õļĴ�������		  
extern const u8 ov7670_init_reg_tbl[CHANGE_REG_NUM][2];		//�Ĵ����������ñ�
	    				 
u8   OV7670_Init(void);		  	   		 

void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
void CLK_init_ON(void);
void CLK_init_OFF(void);
void timer_init(void);	



#endif
















