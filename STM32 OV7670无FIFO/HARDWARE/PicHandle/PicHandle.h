#ifndef __PicHandle__H__
#define __PicHandle__H__

#include<math.h>
#include "sys.h"
#define RGB565_R 0xF800             //the separate RGB value
#define RGB565_G 0x07E0
#define RGB565_B 0x001F

//这些是要实测的参数！！
#define L2C_H 7.350                 //the distance from laiser to CCD_cm
#define RO 0.0053867                //Radian Offset
#define RPC 0.000795302             //Radians per pixel picth
																		
//float abs(float x);
//float sin(float x);
//float cos(float x);
//float tan(float y);
u16 Binary(u16 pixel);
float SqrtByNewton(float x);
int RGB2HSV(int R,int G,int B);
void PicHandle(int sta_y,unsigned int *RGB);
void Distance(void);

#endif
