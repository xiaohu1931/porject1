#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define uint  unsigned int
#define uchar unsigned char

/* LCD1602引脚 */
sbit LCDRS = P2^3;
sbit LCDEN = P2^4;

uchar code Init1[] = "T:00C H:00%     ";
uchar code Init2[] = "SAMPLE RATE:01Hz";

/* LCD延时 */
void LCDdelay(uint z)
{
    uint x;
    uint y;

    for (x = z; x > 0; x--)
    {
        for (y = 10; y > 0; y--);
    }
}

/* 写LCD命令 */
void write_com(uchar com)
{
    LCDRS = 0;
    P0 = com;
    LCDdelay(5);
    LCDEN = 1;
    LCDdelay(5);
    LCDEN = 0;
}

/* 写LCD数据 */
void write_data(uchar date)
{
    LCDRS = 1;
    P0 = date;
    LCDdelay(5);
    LCDEN = 1;
    LCDdelay(5);
    LCDEN = 0;
}

/* LCD1602初始化 */
void Init1602(void)
{
    uchar i;

    write_com(0x38);
    write_com(0x0c);
    write_com(0x06);
    write_com(0x01);
    write_com(0x80);

    for (i = 0; i < 16; i++)
    {
        write_data(Init1[i]);
    }

    write_com(0xc0);
    for (i = 0; i < 16; i++)
    {
        write_data(Init2[i]);
    }
}

/* 显示温度、湿度和采样率 */
void Display_1602(unsigned int temperature,
                  unsigned int humidity,
                  unsigned int frequency,
                  bit hide_frequency)
{
    write_com(0x82);
    write_data('0' + temperature / 10);
    write_data('0' + temperature % 10);

    write_com(0x88);
    write_data('0' + humidity / 10);
    write_data('0' + humidity % 10);

    write_com(0xcc);
    if (hide_frequency)
    {
        write_data(' ');
        write_data(' ');
    }
    else
    {
        write_data('0' + frequency / 10);
        write_data('0' + frequency % 10);
    }
}

#endif
