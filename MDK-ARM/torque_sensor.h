#ifndef __TORQUE_SENSOR_H__
#define __TORQUE_SENSOR_H__

#include "main.h"
#include "data_structs.h"
#include <stddef.h>
#include <stdint.h>

int processForceData(const uint8_t *hexData, size_t dataLength, ForceData_t *force1, ForceData_t *force2);
void RS485_SetMode(uint8_t mode);

#endif
