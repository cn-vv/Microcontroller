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
KEY0:����
KEY1:�Ҷ�ͼ����ֵ��
KEY_UP:��BMPλͼ�ϴ�������
********************************************/
 

 
u16 Row[320];            //���ڴ洢һ��������Ϣ
//u16 Color[240][320];   //���ڴ洢һ��ͼƬ240*320���ص���Ϣ
u16 s_xfact;      //���ڼ�¼������λ��
u16 s_yfact;

//�ļ������������⸲�ǣ�
//��ϳ�:����"0:PHOTO/PIC13141.bmp"���ļ���
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
}


 int main(void)
 {	 
	u8 res;							 
	u8 *pname;				//��·�����ļ��� 
	u8 key;					//��ֵ		   
	u8 i;						 
	u8 sd_ok=1;				//0,sd��������;1,SD������.
	u8 stopflag=0;
	u8 effect=0;	    
	u8 flag=0;
	u16 pixcnt=0;				//����ͳ��
	u16 linecnt=0;				//����ͳ�� 
	delay_init();	    	 //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD     
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH. 
	POINT_COLOR=RED;      
	res=f_mkdir("0:/PHOTO");		//����PHOTO�ļ���
	LCD_Scan_Dir(U2D_L2R);									   						    
 	pname=mymalloc(SRAMIN,30);	//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
 	while(pname==NULL)			//�ڴ�������
 	{	    			  
	}   											  
	while(OV7670_Init())//��ʼ��OV7670
	{
	}

	delay_ms(1500);	 		 
	OV7670_Window_Set(10,174,240,320);	//���ô���  			    		    
	LCD_Clear(BLACK);
 	
	
	
	
	while(1)
	{	
		key=KEY_Scan(0);//��֧������
		switch(key)		
		{		
					case KEY0_PRES:		
				   {
					if(sd_ok)
					{
						LED1=0;	//����DS1,��ʾ��������
						camera_new_pathname(pname);//�õ��ļ���		    
						if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//��������
						{	 
						}
					}
					LED1=1;//�ر�DS1
					delay_ms(1800);//�ȴ�1.8����
					  }break;					
					case KEY1_PRES:
					{
					  flag++;
					  if(flag==3)
					  {flag=0;}				  
					}break;
		  }

			
			
					LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		while(OV7670_VSYNC==0);//
		while(OV7670_VSYNC==1);//		ֻ����VSYNCΪ��ʱ���Ŵ�������	
 	
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
				else if(flag==1)LCD->LCD_RAM=Binary(Row[pixcnt]);//RGB565��ֵ���㷨
				else LCD->LCD_RAM=GRB2GRey(Row[pixcnt]);//�ҶȻ��㷨	
			}
			
			
	   }	   										    
    }
}












