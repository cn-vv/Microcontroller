#include "sys.h"
#include "ov7670.h"
#include "ov7670cfg.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "lcd.h"




//VSYNC     PG15		��֡ͬ���źţ�out��
//HREF			PD6 		����ͬ���źţ�out��
//XCLK			PA8			��ʱ���źţ�in��
//PCLK			PB4			������ʱ�ӣ�out��
//PWDN      	PB3			������ѡ��ģʽ  0���� 1POWER DOWN������ʹ������,��������JTDO���ӣ����ȹر�
//RESET			PG14		����λ�ˣ�0RESET 1һ��ģʽ ������ʹ������
//SIOC			PD3
//SIOD			PG13
	


//��ʼ��OV7670
//����0:�ɹ�
//��������ֵ:�������
u8 OV7670_Init(void)
{
	u8 temp;
	u16 i=0;
	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ����ض˿�ʱ��
	RCC->APB2ENR|=1<<0;  //����AFʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15; 	//PG15 ���� ����					VSYNC
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;			//PB4  �˿�����			 PCLK
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //������������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;  	//PD6		HREF		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //PB3 �˿�����			PWDN  ����ʹ������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  		//PG14  RESET		����ʹ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_14);	


	GPIO_InitStructure.GPIO_Pin  = 0xff; //PC0~7 ���� ����			D0~8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//GPIO_Mode_IPU;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//SWD   �ر�JTAG
	
	CLK_init_ON();


 	SCCB_Init();        		//��ʼ��SCCB ��IO��
 	if(SCCB_WR_Reg(0x12,0x80))return 1;	//��λSCCB
	delay_ms(50); 
	temp=SCCB_RD_Reg(0x12);
	//��ȡ��Ʒ�ͺ�
 	temp=SCCB_RD_Reg(0x0b);
	if(temp!=0x73)return 2;  
 	temp=SCCB_RD_Reg(0x0a);	
	if(temp!=0x76)return 2;
	//��ʼ������	  ��OV7670�Ĵ������в���
	for(i=0;i<sizeof(ov7670_init_reg_tbl)/sizeof(ov7670_init_reg_tbl[0])/2;i++)
	{
	   	SCCB_WR_Reg(ov7670_init_reg_tbl[i][0],ov7670_init_reg_tbl[i][1]);
		delay_ms(2);
 	}
   	return 0x00; 	//ok
} 

void CLK_init_ON(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//HSE�Ǹ����ⲿʱ�ӣ�Ƶ�ʷ�ΧΪ4MHz~16MHz   ʵ�����Ϊ8MHz
    RCC_MCOConfig(RCC_MCO_HSE  );//hsi	Selects the clock source to output on MCO pin.   RCC_MCO_HSE: HSE oscillator clock selected
}
/*void CLK_init_OFF(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}*/


	
//����ͼ���������
//��QVGA���á�
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width*2;	//V*2
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_RD_Reg(0X03);				//��ȡVref֮ǰ��ֵ
	temp&=0XF0;
	temp|=((endx&0X03)<<2)|(sx&0X03);
	SCCB_WR_Reg(0X03,temp);				//����Vref��start��end�����2λ
	SCCB_WR_Reg(0X19,sx>>2);			//����Vref��start��8λ
	SCCB_WR_Reg(0X1A,endx>>2);			//����Vref��end�ĸ�8λ

	temp=SCCB_RD_Reg(0X32);				//��ȡHref֮ǰ��ֵ
	temp&=0XC0;
	temp|=((endy&0X07)<<3)|(sy&0X07);
	SCCB_WR_Reg(0X17,sy>>3);			//����Href��start��8λ
	SCCB_WR_Reg(0X18,endy>>3);			//����Href��end�ĸ�8λ
}


////////////////////////////////////////////////////////////////////////////
//OV7670��������
//��ƽ������
//0:�Զ�
//1:̫��sunny
//2,����cloudy
//3,�칫��office
//4,����home
void OV7670_Light_Mode(u8 mode)
{
	u8 reg13val=0XE7;//Ĭ�Ͼ�������Ϊ�Զ���ƽ��
	u8 reg01val=0;
	u8 reg02val=0;
	switch(mode)
	{
		case 1://sunny
			reg13val=0XE5;
			reg01val=0X5A;
			reg02val=0X5C;
			break;	
		case 2://cloudy
			reg13val=0XE5;
			reg01val=0X58;
			reg02val=0X60;
			break;	
		case 3://office
			reg13val=0XE5;
			reg01val=0X84;
			reg02val=0X4c;
			break;	
		case 4://home
			reg13val=0XE5;
			reg01val=0X96;
			reg02val=0X40;
			break;	
	}
	SCCB_WR_Reg(0X13,reg13val);//COM8���� 
	SCCB_WR_Reg(0X01,reg01val);//AWB��ɫͨ������ 
	SCCB_WR_Reg(0X02,reg02val);//AWB��ɫͨ������ 
}				  
//ɫ������
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Color_Saturation(u8 sat)
{
	u8 reg4f5054val=0X80;//Ĭ�Ͼ���sat=2,��������ɫ�ȵ�����
 	u8 reg52val=0X22;
	u8 reg53val=0X5E;
 	switch(sat)
	{
		case 0://-2
			reg4f5054val=0X40;  	 
			reg52val=0X11;
			reg53val=0X2F;	 	 
			break;	
		case 1://-1
			reg4f5054val=0X66;	    
			reg52val=0X1B;
			reg53val=0X4B;	  
			break;	
		case 3://1
			reg4f5054val=0X99;	   
			reg52val=0X28;
			reg53val=0X71;	   
			break;	
		case 4://2
			reg4f5054val=0XC0;	   
			reg52val=0X33;
			reg53val=0X8D;	   
			break;	
	}
	SCCB_WR_Reg(0X4F,reg4f5054val);	//ɫ�ʾ���ϵ��1
	SCCB_WR_Reg(0X50,reg4f5054val);	//ɫ�ʾ���ϵ��2 
	SCCB_WR_Reg(0X51,0X00);			//ɫ�ʾ���ϵ��3  
	SCCB_WR_Reg(0X52,reg52val);		//ɫ�ʾ���ϵ��4 
	SCCB_WR_Reg(0X53,reg53val);		//ɫ�ʾ���ϵ��5 
	SCCB_WR_Reg(0X54,reg4f5054val);	//ɫ�ʾ���ϵ��6  
	SCCB_WR_Reg(0X58,0X9E);			//MTXS 
}
//��������
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Brightness(u8 bright)
{
	u8 reg55val=0X00;//Ĭ�Ͼ���bright=2
  	switch(bright)
	{
		case 0://-2
			reg55val=0XB0;	 	 
			break;	
		case 1://-1
			reg55val=0X98;	 	 
			break;	
		case 3://1
			reg55val=0X18;	 	 
			break;	
		case 4://2
			reg55val=0X30;	 	 
			break;	
	}
	SCCB_WR_Reg(0X55,reg55val);	//���ȵ��� 
}
//�Աȶ�����
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Contrast(u8 contrast)
{
	u8 reg56val=0X40;//Ĭ�Ͼ���contrast=2
  	switch(contrast)
	{
		case 0://-2
			reg56val=0X30;	 	 
			break;	
		case 1://-1
			reg56val=0X38;	 	 
			break;	
		case 3://1
			reg56val=0X50;	 	 
			break;	
		case 4://2
			reg56val=0X60;	 	 
			break;	
	}
	SCCB_WR_Reg(0X56,reg56val);	//�Աȶȵ��� 
}
//��Ч����
//0:��ͨģʽ    
//1,��Ƭ
//2,�ڰ�   
//3,ƫ��ɫ
//4,ƫ��ɫ
//5,ƫ��ɫ
//6,����	    
void OV7670_Special_Effects(u8 eft)
{
	u8 reg3aval=0X04;//Ĭ��Ϊ��ͨģʽ
	u8 reg67val=0XC0;
	u8 reg68val=0X80;
	switch(eft)
	{
		case 1://��Ƭ
			reg3aval=0X24;
			reg67val=0X80;
			reg68val=0X80;
			break;	
		case 2://�ڰ�
			reg3aval=0X14;
			reg67val=0X80;
			reg68val=0X80;
			break;	
		case 3://ƫ��ɫ
			reg3aval=0X14;
			reg67val=0Xc0;
			reg68val=0X80;
			break;	
		case 4://ƫ��ɫ
			reg3aval=0X14;
			reg67val=0X40;
			reg68val=0X40;
			break;	
		case 5://ƫ��ɫ
			reg3aval=0X14;
			reg67val=0X80;
			reg68val=0XC0;
			break;	
		case 6://����
			reg3aval=0X14;
			reg67val=0XA0;
			reg68val=0X40;
			break;	 
	}
	SCCB_WR_Reg(0X3A,reg3aval);//TSLB���� 
	SCCB_WR_Reg(0X68,reg67val);//MANU,�ֶ�Uֵ 
	SCCB_WR_Reg(0X67,reg68val);//MANV,�ֶ�Vֵ 
}	


/*
//����LCD��ʾ
void camera_refresh(void)
{
	u32 i,j;
 	u16 color;	 
		
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������ 
		LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
	
		while(OV7670_VSYNC==0);//0-1
    while(OV7670_VSYNC==1);//1-0		ֻ����VSYNCΪ��ʱ���Ŵ�������	

		//240*320=76800    ��ÿ��������RGB565��ʾ����ÿ������ռ�������ֽڣ�16λ���ݣ�  I2Cһ�δ���8λ���� ��һ������Ҫ��2��
		//����ȡ�������ݰ�RGB565�Ĵ������浽һ��16λ��color��������
	for(i=0;i<240;i++)
	{
		while(OV7670_HREF==0);//0-1	ֻ����HREFΪ��ʱ���Ŵ�������
		for(j=0;j<320;j++)		
		{
			
			//��ȡ��8λ
			while(OV7670_PCLK==0);			//0-1			������PCLK�����ر����ȶ����ڴ�ʱ��ȡ����	
			color=GPIOC->IDR&0XFF;	//������
			while(OV7670_PCLK==1);		//1-0
			//���Ƹ߰�λ
			color<<=8;  
			//��ȡ��8λ
			while(OV7670_PCLK==0);			//0-1
			color|=GPIOC->IDR&0XFF;	//������
			while(OV7670_PCLK==1);		//1-0
			//��ʾLCD
			LCD->LCD_RAM=color;    
		}   
		while(OV7670_HREF==1); //1-0 
	}		
	LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
}	
*/







