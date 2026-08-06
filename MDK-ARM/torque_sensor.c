#include "torque_sensor.h"
#include <string.h>


static float parse_big_endian_float(const uint8_t *data)
{
    uint32_t raw = 0;
    float value = 0.0f;

    raw |= (uint32_t)data[0] << 24;
    raw |= (uint32_t)data[1] << 16;
    raw |= (uint32_t)data[2] << 8;
    raw |= (uint32_t)data[3];
    memcpy(&value, &raw, sizeof(value));

    return value;
}

int processForceData(const uint8_t *hexData, size_t dataLength, ForceData_t *force1, ForceData_t *force2)
{
    // 检查数据长度
    // 帧头 3 字节 + 数据 24 字节(6 个 float * 4 字节) + 校验 2 字节 = 29 字节
    if (hexData == NULL || force1 == NULL || force2 == NULL || dataLength < 29U) {
        return -1; // 数据长度不足
    }
    
    // 跳过前 3 字节帧头，从第一个 float 数据开始解析
    const uint8_t *dataPtr = hexData + 3;
    
    // 解析第一组力传感器数据(x, y, z)
    force1->fx = parse_big_endian_float(dataPtr);
    dataPtr += 4;
    force1->fy = parse_big_endian_float(dataPtr);
    dataPtr += 4;
    force1->fz = parse_big_endian_float(dataPtr);
    dataPtr += 4;
    force1->torque_x = 0.0f;
    force1->torque_y = 0.0f;
    
    // 解析第二组力传感器数据(x, y, z)
    force2->fx = parse_big_endian_float(dataPtr);
    dataPtr += 4;
    force2->fy = parse_big_endian_float(dataPtr);
    dataPtr += 4;
    force2->fz = parse_big_endian_float(dataPtr);
    force2->torque_x = 0.0f;
    force2->torque_y = 0.0f;
    
    return 0; // 解析成功
}

void RS485_SetMode(uint8_t mode)
{
  if (mode) // 发送模式
  {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET); // DE 置高
  }
  else // 接收模式
  {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET); // DE 置低
  }
  
  // 等待 RS485 收发方向稳定
  HAL_Delay(1);
}
