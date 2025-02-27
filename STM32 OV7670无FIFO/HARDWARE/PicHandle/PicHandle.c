#include "PicHandle.h"
#include "lcd.h"


//��ֵ����ֵ
#define threshold 32767



int MaxRed=0;										//���ں�ɫ��HSV��Ȩ���ֵ
int Red_X,Red_Y;								//��¼��������ĵ������ֵ
float PixelNums;								//��������ĵ������ĵ�����ص���
float Final_Dis;								//��¼����


//float abs(float x)
//{
//    if(x<0) x=0-x;
//    return x;
//}

//
//float sin(float x)
//{

//    const float B = 1.2732395447;
//    const float C = -0.4052847346;
//    const float P = 0.2310792853;              //0.225;
//    float y = B * x + C * x * abs(x);
//    y = P * (y * abs(y) - y) + y;
//    return y;
//}

////
//float cos(float x)
//{
//    const float Q = 1.5707963268;
//    const float PI =3.1415926536;
//    x += Q;

//    if(x > PI)
//    x -= 2 * PI;

//    return( sin(x));
//}

////
//float tan(float y)          //input radian
//{
//    float tan1;
//    tan1=sin(y)/cos(y);
//    return tan1;
//}


/***********************************************
*name:          SqrtByNewton(float x)
*parameter:     float x
*function:      square root
*return:        the result of square root
*reserved:
***********************************************/
float SqrtByNewton(float x)
{

	int temp=0x1fc00000+((*(int *)&x)>>1);
	float val=*(float*)&temp;

	val =(val + x/val) / 2;
	val =(val + x/val) / 2;
	val =(val + x/val) / 2;

	return val;
}



/***********************************************
*name:         	int RGB2HSV(int R,int G,int B)
*parameter:    	int R,int G,int B
*function:     	convert RGB to HSV
*return:       	���ع���HSV���ں�ɫ��һ����Ȩ
*reserved:
***********************************************/
int RGB2HSV(int R,int G,int B)						//R'G'B�������Ϳɸġ�
{
	float maxRGB,minRGB,des;
	float H,S,V;
	int Hi,Si,Vi;
	int RedHSV;

	maxRGB=((R>=G)?R:G);							//the maximum
	maxRGB=((maxRGB>=B)?maxRGB:B);
	minRGB=((R<=G)?R:G);							//the minimum
	minRGB=((minRGB<=B)?minRGB:B);
	des=maxRGB-minRGB;

	V=maxRGB;													//the value of V
	Vi=(int)(V*366/255);							//force to int

	if(maxRGB==0)											//the value of S
		S=0;
	else
		S=1-minRGB/maxRGB;
	Si=(int)(S*360);									//force to int

	if(maxRGB==minRGB)								//the value of H
		H=0;
	else if((maxRGB==R)&&(G>=B))
		H=60*(G-B)/des;
	else if((maxRGB==R)&&(G<B))
		H=60*(G-B)/des+360;
	else if(maxRGB==G)
		H=60*(B-R)/des+120;
	else if(maxRGB==B)
		H=60*(R-G)/des+240;
	else
		;
	if(H<0)
		H=H+360;

	if((H>=0)&&(H<=60))								//���ں�ɫ���м�Ȩ
		H=60-H;
	else if((H>=300)&&(H<=360))
		H=360-H;
	else
		H=0;
	Hi=(int)H*60;

	if((H>60)&&(H<300))								//the color is not red
		RedHSV=0;
	else											//the color is red
		RedHSV=Hi+Si+Vi;
	return(RedHSV);
}


/***********************************************
*name:
*parameter:
*function:
*return:
*reserved:
***********************************************/
void PicHandle(int sta_y,unsigned int *RGB)
{
	int j;
	int maxRed1;
	int RGB_R,RGB_G,RGB_B;							//the separate value
	float xx,yy,pixelnum;								//the transitional value
	
	for(j=0;j<320;j++)
	{
		RGB_R=(RGB[j]&RGB565_R)>>11;
		RGB_G=(RGB[j]&RGB565_G)>>5;
		RGB_B=(RGB[j]&RGB565_B);
	
		maxRed1=RGB2HSV(RGB_R,RGB_G,RGB_B);
	
		if(MaxRed<maxRed1)
		{
			MaxRed=maxRed1;
			xx=(float)j;
			yy=(float)sta_y;
		}
	}
	pixelnum=(xx-159)*(xx-159)+(yy-119)*(yy-119);		//(159,119)
	pixelnum=SqrtByNewton(pixelnum);
	Red_X=(int)xx;																	//����x��yֵ����������ʮ�ֱ��
	Red_Y=(int)yy;
	PixelNums=pixelnum;
	Drawx(Red_X,Red_Y);
	
	return;
}

//�ú������ڼ���������ɫ��ʵ�ʵĲ��
short int Getsub(u16 RGBReal,u16 RGBCompare)
{
  if((RGBReal-RGBCompare)<0)
  return  (RGBCompare-RGBReal); 
  else
  return  (RGBReal-RGBCompare); 
}	




/***********************************************
*name:          Distance(float pixelnum)
*parameter:     float pixelnum//��ߵ����ĵ����ص�
*function:      return the distance/cm
*return:        the distance;unit:cm.
*reserved:
***********************************************/
void Distance(void)
{
    float dis;
    dis=RPC*PixelNums+RO;				
    dis=tan(dis);
    dis=(L2C_H/dis);
		Final_Dis=dis;
    return;
}


//��ֵ���㷨
u16 Binary(u16 pixel)
{
        static u16 Gray;
	    u8 R,G,B;
        /*******��ȡR,G,Bֵ*******/
        R = (pixel&RGB565_R)>>11;
        G = (pixel&RGB565_G)>>5;
        B = (pixel&RGB565_B);
        /*******�Ҷ�ֵ����*******/
        /*******�����ϴ󲿷ֵĹ�ʽ�����8λ��*******/
        /*******������ʽ�����12λ��******/
        Gray = (u16)((R*634+G*613+B*232));
        /*******��ֵ��*******/
        if(Gray<threshold)
                Gray = BLACK;
        else if(Gray>=threshold)
                Gray = WHITE;
        return Gray;
}
//��λ�����лҶȻ�
u16 GRB2GRey(u16 pixel)
{
        static u16 Gray;
	    u8 R,G,B;
	    u8 temp;
        /*******��ȡR,G,Bֵ*******/
        R = (pixel&RGB565_R)>>11;
        G = (pixel&RGB565_G)>>5;
        B = (pixel&RGB565_B);
        temp =(R*28+G*151+B*77)>>8;
        Gray = (u16)(((temp)&0xf8)>>3)|(((temp)&0xfc)<<3)|(((temp)&0xf8)<<8) ;
	    return Gray;
}


//��ֵ�任
//����˵��:
//LPSTR lpDIBBits:ָ��ԴDIBͼ��ָ��
//LONG  lWidth:Դͼ���ȣ���������
//LONG  lHeight:Դͼ��߶ȣ���������
//BYTE  bThre:��ֵ
//����˵��:
//�ú���������ͼ�������ֵ�任�����ڻҶ�ֵС����ֵ������ֱ������
//�Ҷ�ֵΪ0���Ҷ�ֵ������ֵ������ֱ������Ϊ255��
u16 ThresholdTrans(u16 pixel, u16 bThre)
{

   // �ж��Ƿ�С����ֵ
   if (pixel< bThre)
   {
    // ֱ�Ӹ�ֵΪ0
      pixel = 0;
   }
   else
   {
    // ֱ�Ӹ�ֵΪ255
       pixel= 255;
    }
    return pixel;
}
