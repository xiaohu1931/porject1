#ifndef __AT24C02_H__
#define __AT24C02_H__

#include <reg52.h>
#include <intrins.h>

sbit EEPROM_SDA = P2^0;
sbit EEPROM_SCL = P2^1;

void I2C_Delay(void)
{
    unsigned char i;
    for (i = 0; i < 4; i++) _nop_();
}

void I2C_Start(void)
{
    EEPROM_SDA = 1;
    EEPROM_SCL = 1;
    I2C_Delay();
    EEPROM_SDA = 0;
    I2C_Delay();
    EEPROM_SCL = 0;
}

void I2C_Stop(void)
{
    EEPROM_SDA = 0;
    EEPROM_SCL = 1;
    I2C_Delay();
    EEPROM_SDA = 1;
    I2C_Delay();
}

bit I2C_WriteByte(unsigned char dat)
{
    unsigned char i;
    bit ack;

    for (i = 0; i < 8; i++) {
        EEPROM_SDA = (dat & 0x80) ? 1 : 0;
        I2C_Delay();
        EEPROM_SCL = 1;
        I2C_Delay();
        EEPROM_SCL = 0;
        dat <<= 1;
    }

    EEPROM_SDA = 1;
    I2C_Delay();
    EEPROM_SCL = 1;
    I2C_Delay();
    ack = (EEPROM_SDA == 0);
    EEPROM_SCL = 0;
    return ack;
}

unsigned char I2C_ReadByte(bit send_ack)
{
    unsigned char i;
    unsigned char dat = 0;

    EEPROM_SDA = 1;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        EEPROM_SCL = 1;
        I2C_Delay();
        if (EEPROM_SDA) dat |= 0x01;
        EEPROM_SCL = 0;
        I2C_Delay();
    }

    EEPROM_SDA = send_ack ? 0 : 1;
    EEPROM_SCL = 1;
    I2C_Delay();
    EEPROM_SCL = 0;
    EEPROM_SDA = 1;
    return dat;
}

void AT24C02_WaitReady(void)
{
    unsigned char retry = 255;

    while (retry--) {
        I2C_Start();
        if (I2C_WriteByte(0xA0)) {
            I2C_Stop();
            return;
        }
        I2C_Stop();
    }
}

void AT24C02_WriteByte(unsigned char addr, unsigned char dat)
{
    I2C_Start();
    I2C_WriteByte(0xA0);
    I2C_WriteByte(addr);
    I2C_WriteByte(dat);
    I2C_Stop();
    AT24C02_WaitReady();
}

unsigned char AT24C02_ReadByte(unsigned char addr)
{
    unsigned char dat;

    I2C_Start();
    I2C_WriteByte(0xA0);
    I2C_WriteByte(addr);
    I2C_Start();
    I2C_WriteByte(0xA1);
    dat = I2C_ReadByte(0);
    I2C_Stop();
    return dat;
}

#endif
