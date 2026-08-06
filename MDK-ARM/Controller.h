#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "main.h"
#include "data_structs.h"
#include "cpg_control.h"

extern CPG_Handle_t my_cpg;

void Controller_Run(const IMUData_t *imu, const ForceData_t *force,const MotorFb_t *fb, MotorCmd_t *out_cmd);

#endif
