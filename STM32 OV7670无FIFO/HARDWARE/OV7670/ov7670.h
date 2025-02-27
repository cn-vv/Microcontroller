#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"



//VSYNC         PG15	（帧同步信号：out）
//HREF			PD6 	（行同步信号：out）
//XCLK			PA8		（时钟信号：in）
//PCLK			PB4		（像素时钟：out）
//PWDN      PB3		（功耗选择模式  0工作 1POWER DOWN）正常使用拉低
//RESET			PG14	（复位端：0RESET 1一般模式 ）正常使用拉高

#define OV7670_VSYNC 	PGin(15)
#define OV7670_HREF		PDin(6)
#define	OV7670_XCLK		PAin(8)
#define	OV7670_PCLK		PBin(4)
#define	OV7670_PWDN		PBin(3)
#define	OV7670_RESET	PGin(14)

#define OV7670_DATA   GPIO_ReadInputData(GPIOC,0x00FF) 					//数据输入端口
//GPIOC->IDR&0x00FF 
/////////////////////////////////////////
#define CHANGE_REG_NUM 							171			//需要配置的寄存器总数		  
extern const u8 ov7670_init_reg_tbl[CHANGE_REG_NUM][2];		//寄存器及其配置表
	    				 
u8   OV7670_Init(void);		  	   		 

void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
void CLK_init_ON(void);
void CLK_init_OFF(void);
void timer_init(void);	



#endif
















