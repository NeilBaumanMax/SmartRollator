#ifndef __CPG_CONTROL_H__
#define __CPG_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float alpha;    // Hopf 振荡器收敛速度参数
    float mu;       // Hopf 振荡器目标幅值平方
    float omega;    // Hopf 振荡器角频率(rad/s)
    float epsilon;  // 外力输入耦合系数

    float x;        // Hopf 振荡器状态 x
    float y;        // Hopf 振荡器状态 y

    float M;        // 导纳模型虚拟质量
    float D_base;   // 基础阻尼
    float lambda;   // CPG 状态到阻尼调制的增益
    float v_c;      // 导纳输出速度(m/s)
} CPG_Handle_t;

void CPG_Init(CPG_Handle_t *hcpg);
float CPG_Step(CPG_Handle_t *hcpg, float F_ext, float dt);
void CPG_Update_Frequency(CPG_Handle_t *hcpg, float freq_hz);

#ifdef __cplusplus
}
#endif

#endif /* __CPG_CONTROL_H__ */
