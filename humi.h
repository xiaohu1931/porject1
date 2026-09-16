#ifndef __HUMI_H__
#define __HUMI_H__

#include <reg52.h>
#include <intrins.h>

/* SHT11命令定义 */
#define TEMP            0
#define HUMI            1
#define MEASURE_TEMP    0x03
#define MEASURE_HUMI    0x05
#define RESET           0x1e
#define ACK             0
#define noACK           1

/* SHT11引脚定义 */
sbit DATA = P3^5;
sbit SCK  = P3^4;

/* 短延时 */
void humi_delay(void)
{
    _nop_();
    _nop_();
    _nop_();
}

/* 向SHT11写入一个字节 */
char s_write_byte(unsigned char value)
{
    unsigned char i;
    char error = 0;

    for (i = 0x80; i > 0; i /= 2)
    {
        if (i & value)
        {
            DATA = 1;
        }
        else
        {
            DATA = 0;
        }

        SCK = 1;
        humi_delay();
        SCK = 0;
    }

    DATA = 1;
    SCK = 1;
    humi_delay();
    error = DATA;
    SCK = 0;

    return error;
}

/* 从SHT11读取一个字节 */
char s_read_byte(unsigned char ack)
{
    unsigned char i;
    unsigned char value = 0;

    DATA = 1;
    for (i = 0x80; i > 0; i /= 2)
    {
        SCK = 1;
        if (DATA)
        {
            value |= i;
        }
        SCK = 0;
    }

    DATA = !ack;
    SCK = 1;
    humi_delay();
    SCK = 0;
    DATA = 1;

    return value;
}

/* 启动SHT11通信 */
void s_transstart(void)
{
    DATA = 1;
    SCK = 1;
    humi_delay();

    DATA = 0;
    humi_delay();
    SCK = 0;
    humi_delay();

    SCK = 1;
    humi_delay();
    DATA = 1;
    humi_delay();
    SCK = 0;
}

/* 读取一次温度或湿度原始数据 */
char s_measure(unsigned char *value,
               unsigned char *checksum,
               unsigned char mode)
{
    unsigned int timeout = 0;
    char error = 0;

    s_transstart();

    if (mode == TEMP)
    {
        error = s_write_byte(MEASURE_TEMP);
    }
    else
    {
        error = s_write_byte(MEASURE_HUMI);
    }

    while (DATA && (timeout < 65535))
    {
        timeout++;
        humi_delay();
    }

    if (DATA || error)
    {
        return 1;
    }

    value[0] = s_read_byte(ACK);
    value[1] = s_read_byte(ACK);
    *checksum = s_read_byte(noACK);

    return 0;
}

/* 将原始数据换算成温度和相对湿度 */
void calc_sth11(float *humidity, float *temperature)
{
    float raw_humidity = *humidity;
    float raw_temperature = *temperature;
    float humidity_linear;
    float humidity_true;
    float temperature_c;

    temperature_c = raw_temperature * 0.01 - 40;
    humidity_linear = -0.0000028 * raw_humidity * raw_humidity
                      + 0.0405 * raw_humidity - 4.0;
    humidity_true = (temperature_c - 25)
                    * (0.01 + 0.00008 * raw_humidity)
                    + humidity_linear;

    if (humidity_true > 100)
    {
        humidity_true = 99;
    }
    if (humidity_true < 0.1)
    {
        humidity_true = 0.1;
    }

    *temperature = temperature_c;
    *humidity = humidity_true;
}

/* 读取并换算温湿度 */
bit SHT11_Read(unsigned char *temperature, unsigned char *humidity)
{
    unsigned char temp_data[2];
    unsigned char humi_data[2];
    unsigned char checksum;
    unsigned int temp_value;
    unsigned int humi_value;
    float temp_float;
    float humi_float;

    if (s_measure(temp_data, &checksum, TEMP))
    {
        return 0;
    }

    if (s_measure(humi_data, &checksum, HUMI))
    {
        return 0;
    }

    temp_value = ((unsigned int)temp_data[0] << 8) | temp_data[1];
    humi_value = ((unsigned int)humi_data[0] << 8) | humi_data[1];

    temp_float = temp_value;
    humi_float = humi_value;
    calc_sth11(&humi_float, &temp_float);

    if (temp_float < 0)
    {
        *temperature = 0;
    }
    else
    {
        *temperature = (unsigned char)(temp_float + 0.5);
    }

    *humidity = (unsigned char)(humi_float + 0.5);

    return 1;
}

#endif
