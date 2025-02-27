#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"	 
#include "ov7670.h" 
#include "timer.h" 
#include "PicHandle.h"
 
/*****************************************
KEY0:拍照
KEY1:灰度图，二值化
KEY_UP:将BMP位图上传到电脑
********************************************/
 

 
u16 Row[320];            //用于存储一行像素信息
//u16 Color[240][320];   //用于存储一张图片240*320像素的信息
u16 s_xfact;      //用于记录特征点位置
u16 s_yfact;

//文件名自增（避免覆盖）
//组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
}


 int main(void)
 {	 
	u8 res;							 
	u8 *pname;				//带路径的文件名 
	u8 key;					//键值		   
	u8 i;						 
	u8 sd_ok=1;				//0,sd卡不正常;1,SD卡正常.
	u8 stopflag=0;
	u8 effect=0;	    
	u8 flag=0;
	u16 pixcnt=0;				//像素统计
	u16 linecnt=0;				//行数统计 
	delay_init();	    	 //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD     
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH. 
	POINT_COLOR=RED;      
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	LCD_Scan_Dir(U2D_L2R);									   						    
 	pname=mymalloc(SRAMIN,30);	//为带路径的文件名分配30个字节的内存		    
 	while(pname==NULL)			//内存分配出错
 	{	    			  
	}   											  
	while(OV7670_Init())//初始化OV7670
	{
	}

	delay_ms(1500);	 		 
	OV7670_Window_Set(10,174,240,320);	//设置窗口  			    		    
	LCD_Clear(BLACK);
 	
	
	
	
	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		switch(key)		
		{		
					case KEY0_PRES:		
				   {
					if(sd_ok)
					{
						LED1=0;	//点亮DS1,提示正在拍照
						camera_new_pathname(pname);//得到文件名		    
						if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//拍照有误
						{	 
						}
					}
					LED1=1;//关闭DS1
					delay_ms(1800);//等待1.8秒钟
					  }break;					
					case KEY1_PRES:
					{
					  flag++;
					  if(flag==3)
					  {flag=0;}				  
					}break;
		  }

			
			
					LCD_SetCursor(0x00,0x0000);	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		while(OV7670_VSYNC==0);//
		while(OV7670_VSYNC==1);//		只有在VSYNC为低时，才传输数据	
 	
		for(linecnt=0;linecnt<240;linecnt++)		
		{
			while(OV7670_HREF==0);
			for(pixcnt=0;pixcnt<320;pixcnt++)
			{
				while(OV7670_PCLK==0);
				Row[pixcnt]=GPIOC->IDR&0XFF;
				while(OV7670_PCLK==1); 
				Row[pixcnt]<<=8;
				while(OV7670_PCLK==0);  
				Row[pixcnt]|=GPIOC->IDR&0XFF; 
				while(OV7670_PCLK==1);
                				
			} 	
			for(pixcnt=0;pixcnt<320;pixcnt++)
			{
				if(flag==0)LCD->LCD_RAM=Row[pixcnt];
				else if(flag==1)LCD->LCD_RAM=Binary(Row[pixcnt]);//RGB565二值化算法
				else LCD->LCD_RAM=GRB2GRey(Row[pixcnt]);//灰度化算法	
			}
			
			
	   }	   										    
    }
}












