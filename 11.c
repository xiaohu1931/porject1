#include <reg52.h>
#include <display.h>
#include "at24c02.h"
#include "humi.h"

#define uchar unsigned char

/* 按键引脚定义 */
sbit Key1 = P2^6;                 // 设置键
sbit Key2 = P2^7;                 // 加键
sbit Key3 = P3^7;                 // 减键

/* 温湿度及采样率变量 */
uchar temperature = 0;
uchar humidity = 0;
uchar sample_frequency = 1;
uchar sample_accumulator = 0;
uchar blink_ticks = 0;

bit sample_ready = 1;
bit frequency_visible = 1;
uchar set = 0;

/* 保存采样率到24C02 */
void write_eeprom(void)
{
    AT24C02_WriteByte(0, sample_frequency);
}

/* 从24C02读取采样率 */
void init_eeprom(void)
{
    sample_frequency = AT24C02_ReadByte(0);

    if ((sample_frequency < 1) || (sample_frequency > 10))
    {
        sample_frequency = 1;
        write_eeprom();
    }
}

/* 定时器0初始化，定时50ms */
void init(void)
{
    TMOD = 0x01;
    TL0 = 0xb0;
    TH0 = 0x3c;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

/* 按键扫描 */
void Key(void)
{
    /* 进入或退出采样率设置 */
    if (Key1 == 0)
    {
        while (Key1 == 0);

        set++;
        frequency_visible = 1;
        blink_ticks = 0;

        if (set >= 2)
        {
            set = 0;
        }

        write_com(0x0c);
    }

    /* 增加采样率 */
    if ((Key2 == 0) && (set != 0))
    {
        while (Key2 == 0);

        if (sample_frequency >= 10)
        {
            sample_frequency = 1;
        }
        else
        {
            sample_frequency++;
        }

        frequency_visible = 1;
        blink_ticks = 0;
        write_eeprom();
    }

    /* 减少采样率 */
    if ((Key3 == 0) && (set != 0))
    {
        while (Key3 == 0);

        if (sample_frequency <= 1)
        {
            sample_frequency = 10;
        }
        else
        {
            sample_frequency--;
        }

        frequency_visible = 1;
        blink_ticks = 0;
        write_eeprom();
    }
}

/* 主函数 */
void main(void)
{
    Init1602();
    init_eeprom();
    init();

    while (1)
    {
        if ((set == 0) && sample_ready)
        {
            ET0 = 0;
            SHT11_Read(&temperature, &humidity);
            ET0 = 1;
            sample_ready = 0;
        }

        Display_1602(temperature, humidity, sample_frequency,
                     (bit)((set != 0) && (!frequency_visible)));
        Key();
    }
}

/* 定时器0中断函数 */
void time0_int(void) interrupt 1
{
    static uchar count = 0;

    TL0 = 0xb0;
    TH0 = 0x3c;

    count++;
    if (count >= 1)
    {
        count = 0;

        /* 设置状态下控制采样率数字闪烁 */
        if (set != 0)
        {
            blink_ticks++;
            if (blink_ticks >= 10)
            {
                blink_ticks = 0;
                frequency_visible = !frequency_visible;
            }
            return;
        }

        frequency_visible = 1;
        blink_ticks = 0;

        /* 根据设置的频率产生采样标志 */
        sample_accumulator += sample_frequency;
        if (sample_accumulator >= 20)
        {
            sample_accumulator -= 20;
            sample_ready = 1;
        }
    }
}
