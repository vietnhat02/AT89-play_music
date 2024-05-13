#ifndef __I2C_H_
#define __I2C_H_

#include<reg52.h>
sbit SCL=P2^1;
sbit SDA=P2^0;

void I2cStart();
void I2cStop();
unsigned char I2cSendByte(unsigned char dat);
unsigned char I2cReadByte();
void I2cReadRespon();
#endif
