#ifndef __TEMP_H_
#define __TEMP_H_

#include<reg52.h>
//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--定义使用的IO口--//
sbit DQ=P3^4;

//--声明全局函数--//
void Delay_DS18B20(int num);
void Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
unsigned int ReadTemperature(void);
unsigned int check_wendu(void);

#endif
