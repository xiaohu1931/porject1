#include"temp.h"

/*****延时子程序：该延时主要用于ds18b20延时*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
/*****初始化DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ复位
  Delay_DS18B20(8);    //稍做延时
  DQ = 0;         //单片机将DQ拉低
  Delay_DS18B20(80);   //精确延时，大于480us
  DQ = 1;         //拉高总线
  Delay_DS18B20(14);
  x = DQ;           //稍做延时后，如果x=0则初始化成功，x=1则初始化失败
  Delay_DS18B20(20);
}
/*****读一个字节*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****写一个字节*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
/*****读取温度*****/
unsigned int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0x44);  //启动温度转换
  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0xBE);  //读取温度寄存器
  a=ReadOneChar();     //读低8位
  b=ReadOneChar();    //读高8位
  t=b;
  t<<=8;
  t=t|a;
  tt=t*0.0625;
  t= tt*100+0.5;     //放大10倍输出并四舍五入
  return(t);
}
//=====================================================================================
//=====================================================================================
//=====================================================================================


/*****校准温度*****/
unsigned int check_wendu(void)
{
	unsigned int c;
	c=ReadTemperature()-5;  			//获取温度值并减去DS18B20的温漂误差
	if(c<1) c=0;
	if(c>=999) c=999;
	return c;
}