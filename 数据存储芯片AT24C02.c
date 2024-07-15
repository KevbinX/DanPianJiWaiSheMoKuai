/*******************************************************************************
                      AT24C02.h
*******************************************************************************/
#ifndef AT24C02_H_
#define AT24C02_H_

// 定义AT24C02读取单个字节的函数
unsigned char AT24C02_Read_Byte(unsigned char address);
// 该函数将从指定地址读取一个字节数据，并返回读取结果

// 定义AT24C02写入单个字节的函数
void AT24C02_Write_Byte(unsigned char address, unsigned char dat);
// 该函数将往指定地址写入一个字节数据，以完成存储操作

#endif
/*******************************************************************************
                      AT24C02.c
*******************************************************************************/
// 包含STC89C5xRC单片机芯片头文件
#include <STC89C5xRC.H>

// 包含AT24C02 EEPROM的头文件
#include "AT24C02.h"

// 包含公共函数和变量的头文件
#include "public.h"

// 定义AT24C02 EEPROM的地址为0xA0
#define AT24C02_ADDRESS 0xA0

// 将SDA引脚定义为P2口的第0位
sbit SDA = P2^0;

// 将SCL引脚定义为P2口的第1位
sbit SCL = P2^1;


// IIC总线启动函数
void IIC_Start(void)
{
    // 置SDA高电平，准备生成启动信号
    SDA = 1;
    
    // 置SCL高电平，防止总线冲突
    SCL = 1;
    
    // 将SDA引脚拉低，生成启动信号
    SDA = 0;
    
    // 将SCL引脚拉低，准备发送读写数据
    SCL = 0; // 准备发送读写数据
}


// IIC总线结束函数
void IIC_Stop(void)
{
    // 将SDA引脚拉低，准备生成停止信号
    SDA = 0;
    
    // 将SCL引脚拉高，生成停止信号的时钟边沿
    SCL = 1;
    
    // 将SDA引脚拉高，生成停止信号
    SDA = 1;
}

// 等待应答信号函数
unsigned char IIC_Wait_ACK(void)
{
    // 主机松开SDA，释放总线，等待从机回复应答
    SDA = 1; 
	
    // 置SCL高电平，发送时钟信号
    SCL = 1;
    
    // 等待20个10us延迟，等待从机回复应答
    Delay10us(20);
    
    // 置SCL低电平，准备读取SDA电平状态
    SCL = 0;
    
    // 应该返回SDA的电平状态，0表示应答，1表示未应答
    // 但这里直接返回0，可能有bug
    return 0;
}


// 发送应答信号函数
void IIC_ACK(unsigned char ack)
{
    // 置SCL低电平，准备发送应答信号
    SCL = 0;
    
    // 将应答信号ack发送到SDA引脚
    SDA = ack;
    
    // 置SCL高电平，发送时钟信号
    SCL = 1;	
	
    // 置SCL低电平，结束应答信号的发送
    SCL = 0;
}

// IIC总线发送一个字节函数
void IIC_Write_Byte(unsigned char dat)
{
    // 定义循环变量i
    unsigned char i;
    
    // 置SCL低电平，准备发送数据
    SCL = 0;
    
    // 循环发送8位数据
    for(i=0;i<8;i++){
        // 将数据dat的第i位发送到SDA引脚
        SDA = dat&(0x80>>i);// 位ewise AND操作，取数据的第i位， 一位一位从高往低发送数据到SDA
        
        // 置SCL高电平，发送时钟信号
        SCL = 1;
        
        // 置SCL低电平，准备发送下一位数据
        SCL = 0;
    }
}

// IIC总线读取一个字节函数
unsigned char IIC_Read_Byte(void)
{
    // 定义循环变量i和接收数据变量receive
    unsigned char i, receive = 0x00;
    
    // 主机释放SDA总线，准备接收数据
    SDA = 1; 					// 主动拉高数据线，等待从机发送数据
    
    // 循环读取8位数据
    for(i=0;i<8;i++)
    {	
        // 置SCL高电平，发送时钟信号
        SCL = 1;
        
        // 读取SDA引脚的电平状态
        if(SDA)
        {
          receive |= (0x80>>i);// 如果SDA高电平，则设置receive对应位为1
        } 
        // 置SCL低电平，准备读取下一位数据
        SCL = 0;
		
    }
    
    // 返回接收到的数据
    return receive;
}

// 从AT24C02当前地址写入一个字节函数
void AT24C02_Write_Byte(unsigned char address,unsigned char dat)
{
    // 生成IIC总线开始信号
    IIC_Start();							// 开始信号
    
    // 发送AT24C02的设备地址和写命令
    IIC_Write_Byte(AT24C02_ADDRESS);		// 发送AT24C2的所在地址（89C51默认A1A2A3接地）和写命令
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 发送写入数据所在的地址
    IIC_Write_Byte(address);				// 发送写入数据所在的地址
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 发送要写入的数据
    IIC_Write_Byte(dat);					// 发送写命令
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 生成IIC总线结束信号
    IIC_Stop();								// 结束信号
    
    // 等待写周期完成（5ms）
    Delayms(5);								// 写周期
}


// 从AT24C02读取一个字节函数
unsigned char AT24C02_Read_Byte(unsigned char address)
{
    unsigned char Data;
    
    // 生成IIC总线开始信号
    IIC_Start();							// 开始信号
    
    // 发送AT24C02的设备地址和写命令
    IIC_Write_Byte(AT24C02_ADDRESS);		// 发送AT24C2的所在地址（89C51默认A1A2A3接地）和写命令
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 发送读取数据所在的地址
    IIC_Write_Byte(address);				// 发送读取数据所在的地址
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 生成IIC总线开始信号（重新启动）
    IIC_Start();							// 开始信号
    
    // 发送读命令
    IIC_Write_Byte(0xA1);	// 发送读命令
    
    // 等待从机回复应答信号
    IIC_Wait_ACK();							// 等待应答
    
    // 读取数据
    Data = IIC_Read_Byte();					// 读数据
    
    // 发送结束应答信号
    IIC_ACK(1);								// 结束应答
    
    // 生成IIC总线结束信号
    IIC_Stop();								// 结束信号
    
    // 返回读取的数据
    return Data;
}




