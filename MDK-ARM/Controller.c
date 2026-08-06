#include "Controller.h"
#include <math.h>
#include <stdint.h>

#ifndef CONTROLLER_PI
#define CONTROLLER_PI 3.14159265358979323846f
#endif

// 轮子半径，用于将 CPG 输出的线速度(m/s)换算成轮速(rpm)。
// 如果实际轮径不是 150mm，需要按实测半径修改该值。
#define WHEEL_RADIUS_M        0.075f
#define MOTOR_RPM_LIMIT       400.0f
#define CPG_CONTROL_DT_S      0.001f

CPG_Handle_t my_cpg;

static float clampf(float x, float lo, float hi)
{
    return (x < lo) ? lo : ((x > hi) ? hi : x);
}

static float mps_to_rpm(float v_mps)
{
    return (v_mps * 60.0f) / (2.0f * CONTROLLER_PI * WHEEL_RADIUS_M);
}

void Controller_Run(const IMUData_t *imu,
                    const ForceData_t *force,
                    const MotorFb_t *fb,
                    MotorCmd_t *out_cmd)
{
    float F_x = 0.0f;
    float v_cmd;
    float target_rpm_f;

    (void)imu;
    (void)fb;

    if (out_cmd == NULL) {
        return;
    }

    // 当前控制输入使用右把手力传感器解析后的 X 向力。
    // 上层 ControlTask 已完成新鲜度检查；过期时 force 会被清零。
    if (force != NULL && force->timestamp != 0U) {
        F_x = force->fx;
    }

    // CPG 变导纳控制：输入外力(N)，输出车体目标线速度(m/s)。
    v_cmd = CPG_Step(&my_cpg, F_x, CPG_CONTROL_DT_S);

    // 保留原有电机速度模式下发逻辑：MotorCmd_t.target_vel 表示 rpm。
    target_rpm_f = mps_to_rpm(v_cmd);
    target_rpm_f = clampf(target_rpm_f, -MOTOR_RPM_LIMIT, MOTOR_RPM_LIMIT);

    out_cmd->ctrl_mode = 1;
    out_cmd->target_vel = (int32_t)lrintf(target_rpm_f);
    out_cmd->target_torque = 0;
}
