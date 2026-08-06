#ifndef __DATA_STRUCTS_H
#define __DATA_STRUCTS_H

#include <stdint.h>

#define LOG_MSG_MAX_LEN 128

typedef struct
{
    float fx;
    float fy;
    float fz;
    float torque_x;
    float torque_y;
    uint32_t timestamp;
} ForceData_t;

typedef struct
{
    float accel[3];
    float gyro[3];
    float quat[4];
    float roll;
    float pitch;
    float yaw;
    uint32_t timestamp;
} IMUData_t;

typedef struct
{
    int32_t pos;
    int32_t vel;
    int16_t torque;
    uint8_t status;
    uint32_t timestamp;
} MotorFb_t;

typedef struct
{
    int32_t target_pos;
    int32_t target_vel;
		int32_t target_acc;
    int16_t target_torque;
    uint8_t ctrl_mode;
    uint32_t timestamp;
} MotorCmd_t;

typedef struct
{
    char text[LOG_MSG_MAX_LEN];
    uint32_t timestamp;
    uint8_t level;
} LogMsg_t;

typedef struct
{
    uint16_t seq;
    uint8_t status;
    uint16_t marker_id;
    int16_t x_mm;
    int16_t y_mm;
    int16_t z_mm;
    int16_t roll_x10;
    int16_t pitch_x10;
    int16_t yaw_x10;
    int16_t dist_mm;
    uint32_t timestamp;
} VisionPose_t;

typedef struct {
  ForceData_t  force;
  MotorCmd_t   cmd;
  VisionPose_t vision;
  uint32_t     ts_force;
  uint32_t     ts_cmd;
  uint32_t     ts_vision;
} monitor_snapshot_t;
#endif /* __DATA_STRUCTS_H */
