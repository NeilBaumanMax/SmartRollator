#include "cpg_control.h"
#include <math.h>
#include <stddef.h>

#ifndef CPG_PI
#define CPG_PI 3.14159265358979323846f
#endif

#define CPG_DEFAULT_ALPHA      10.0f
#define CPG_DEFAULT_MU         1.0f
#define CPG_DEFAULT_OMEGA      (2.0f * CPG_PI * 0.9f)
#define CPG_DEFAULT_EPSILON    0.01f
#define CPG_DEFAULT_X          0.1f
#define CPG_DEFAULT_Y          0.0f
#define CPG_DEFAULT_M          15.0f
#define CPG_DEFAULT_D_BASE     40.0f
#define CPG_DEFAULT_LAMBDA     20.0f
#define CPG_DEFAULT_V_C        0.0f

#define CPG_D_MIN              15.0f
#define CPG_D_MAX              80.0f
#define CPG_V_MIN              (-0.5f)
#define CPG_V_MAX              1.2f

// 防止外部异常输入或积分发散造成状态爆炸。
#define CPG_FORCE_LIMIT        500.0f
#define CPG_STATE_LIMIT        5.0f
#define CPG_DERIV_LIMIT        200.0f
#define CPG_DT_MIN             0.0001f
#define CPG_DT_MAX             0.1f
#define CPG_PARAM_MIN          0.0001f

static float CPG_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static float CPG_ClampFinite(float value, float min_value, float max_value, float fallback)
{
    if (isnan(value) || isinf(value)) {
        value = fallback;
    }

    return CPG_Clamp(value, min_value, max_value);
}

static float CPG_EnsurePositive(float value, float default_value)
{
    if (isnan(value) || isinf(value) || value < CPG_PARAM_MIN) {
        return default_value;
    }

    return value;
}

void CPG_Init(CPG_Handle_t *hcpg)
{
    if (hcpg == NULL) {
        return;
    }

    hcpg->alpha   = CPG_DEFAULT_ALPHA;
    hcpg->mu      = CPG_DEFAULT_MU;
    hcpg->omega   = CPG_DEFAULT_OMEGA;
    hcpg->epsilon = CPG_DEFAULT_EPSILON;

    hcpg->x = CPG_DEFAULT_X;
    hcpg->y = CPG_DEFAULT_Y;

    hcpg->M      = CPG_DEFAULT_M;
    hcpg->D_base = CPG_DEFAULT_D_BASE;
    hcpg->lambda = CPG_DEFAULT_LAMBDA;
    hcpg->v_c    = CPG_DEFAULT_V_C;
}

float CPG_Step(CPG_Handle_t *hcpg, float F_ext, float dt)
{
    float r2;
    float dx;
    float dy;
    float D_current;
    float a_c;

    if (hcpg == NULL) {
        return 0.0f;
    }

    // 对输入、步长和内部状态做防发散保护。
    F_ext = CPG_ClampFinite(F_ext, -CPG_FORCE_LIMIT, CPG_FORCE_LIMIT, 0.0f);
    dt = CPG_ClampFinite(dt, CPG_DT_MIN, CPG_DT_MAX, CPG_DT_MIN);

    hcpg->alpha   = CPG_EnsurePositive(hcpg->alpha, CPG_DEFAULT_ALPHA);
    hcpg->mu      = CPG_EnsurePositive(hcpg->mu, CPG_DEFAULT_MU);
    hcpg->omega   = CPG_ClampFinite(hcpg->omega, -100.0f, 100.0f, CPG_DEFAULT_OMEGA);
    hcpg->epsilon = CPG_ClampFinite(hcpg->epsilon, -10.0f, 10.0f, CPG_DEFAULT_EPSILON);
    hcpg->M       = CPG_EnsurePositive(hcpg->M, CPG_DEFAULT_M);
    hcpg->D_base  = CPG_EnsurePositive(hcpg->D_base, CPG_DEFAULT_D_BASE);
    hcpg->lambda  = CPG_ClampFinite(hcpg->lambda, -200.0f, 200.0f, CPG_DEFAULT_LAMBDA);
    hcpg->x       = CPG_ClampFinite(hcpg->x, -CPG_STATE_LIMIT, CPG_STATE_LIMIT, CPG_DEFAULT_X);
    hcpg->y       = CPG_ClampFinite(hcpg->y, -CPG_STATE_LIMIT, CPG_STATE_LIMIT, CPG_DEFAULT_Y);
    hcpg->v_c     = CPG_ClampFinite(hcpg->v_c, CPG_V_MIN, CPG_V_MAX, CPG_DEFAULT_V_C);

    // 1. 使用一阶欧拉法更新 Hopf 振荡器。
    r2 = hcpg->x * hcpg->x + hcpg->y * hcpg->y;
    dx = hcpg->alpha * (hcpg->mu - r2) * hcpg->x
       - hcpg->omega * hcpg->y
       + hcpg->epsilon * F_ext;
    dy = hcpg->alpha * (hcpg->mu - r2) * hcpg->y
       + hcpg->omega * hcpg->x;

    dx = CPG_ClampFinite(dx, -CPG_DERIV_LIMIT, CPG_DERIV_LIMIT, 0.0f);
    dy = CPG_ClampFinite(dy, -CPG_DERIV_LIMIT, CPG_DERIV_LIMIT, 0.0f);

    hcpg->x += dx * dt;
    hcpg->y += dy * dt;
    hcpg->x = CPG_ClampFinite(hcpg->x, -CPG_STATE_LIMIT, CPG_STATE_LIMIT, CPG_DEFAULT_X);
    hcpg->y = CPG_ClampFinite(hcpg->y, -CPG_STATE_LIMIT, CPG_STATE_LIMIT, CPG_DEFAULT_Y);

    // 2. 根据 CPG 状态计算动态阻尼，并限制在安全范围内。
    D_current = hcpg->D_base - hcpg->lambda * hcpg->x;
    D_current = CPG_ClampFinite(D_current, CPG_D_MIN, CPG_D_MAX, hcpg->D_base);

    // 3. 导纳模型积分求速度：M * a_c + D_current * v_c = F_ext。
    a_c = (F_ext - D_current * hcpg->v_c) / hcpg->M;
    a_c = CPG_ClampFinite(a_c, -CPG_DERIV_LIMIT, CPG_DERIV_LIMIT, 0.0f);

    hcpg->v_c += a_c * dt;

    // 4. 输出速度限幅，单位为 m/s。
    hcpg->v_c = CPG_ClampFinite(hcpg->v_c, CPG_V_MIN, CPG_V_MAX, CPG_DEFAULT_V_C);

    return hcpg->v_c;
}

void CPG_Update_Frequency(CPG_Handle_t *hcpg, float freq_hz)
{
    if (hcpg == NULL) {
        return;
    }

    // 仅接受人体正常步频范围内的数据，异常值直接丢弃。
    if (isnan(freq_hz) || isinf(freq_hz)) {
        return;
    }

    if (freq_hz >= 0.3f && freq_hz <= 2.0f) {
        hcpg->omega = 2.0f * 3.1415926f * freq_hz;
    }
}
