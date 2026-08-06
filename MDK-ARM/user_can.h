
#ifndef __USER_CAN_H__
#define __USER_CAN_H__

#include "fdcan.h"
#include "main.h"

//#include "ZLAC8015D.h"


extern uint8_t TPDO_Config_Status;

void CAN_ConfigFilter(void);
void CAN_Init(void);
uint8_t RPDO0_Config(uint8_t ID);//RPDO0配置
uint8_t RPDO1_Config(uint8_t ID);//RPDO1配置
uint8_t RPDO2_Config(uint8_t ID);//RPDO2配置
uint8_t RPDO3_Config(uint8_t ID);//RPDO3配置
uint8_t TPDO0_Config(uint8_t ID);//TPDO0配置
uint8_t TPDO1_Config(uint8_t ID);//TPDO1配置
uint8_t TPDO2_Config(uint8_t ID);//TPDO2配置
uint8_t TPDO3_Config(uint8_t ID);//TPDO3配置
uint8_t NMT_Control(uint8_t ID, uint8_t Data0);//NMT控制
uint8_t Driver_Enable(uint8_t ID);//使能切换
uint8_t Profile_Velocity_Init(uint8_t ID,uint8_t AccSet);//速度模式初始配置
void Profile_Velocity_Test(uint8_t ID);//速度模式测试
void ZLAC8015D_Test(void);//ZLAC8015D测试程序
uint8_t CAN_Send(uint32_t ID, uint32_t Data_Size, uint8_t* Data);

static inline uint8_t ZLAC_SetVelocity_rpm(uint8_t id, int16_t left_rpm, int16_t right_rpm)
{
    // 与 Profile_Velocity_Test 一致：0x300 + ID 发送 4 字节，L(low,high), R(low,high)
    uint8_t d[4];
    d[0] = (uint8_t)(left_rpm & 0xFF);
    d[1] = (uint8_t)((left_rpm >> 8) & 0xFF);
    d[2] = (uint8_t)(right_rpm & 0xFF);
    d[3] = (uint8_t)((right_rpm >> 8) & 0xFF);
    return CAN_Send(0x300U + id, 4U, d);
}


#endif
