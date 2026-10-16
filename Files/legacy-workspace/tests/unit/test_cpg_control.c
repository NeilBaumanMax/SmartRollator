/**
 * test_cpg_control.c — CPG 控制算法单元测试模板
 *
 * 编译 (Host PC):
 *   gcc -std=c11 -Wall -DTEST -I../../firmware/App test_cpg_control.c ../../firmware/App/cpg_control.c -lm -o cpg_test
 *   ./cpg_test
 *
 * 依赖: 只需要 math.h，CPG 模块无硬件依赖
 */

#ifdef TEST

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../../firmware/App/cpg_control.h"

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { \
    tests_run++; \
    printf("  TEST: %s ... ", name); \
} while(0)

#define PASS() do { \
    tests_passed++; \
    printf("PASS\n"); \
} while(0)

#define FAIL(msg) do { \
    tests_failed++; \
    printf("FAIL: %s\n", msg); \
} while(0)

#define ASSERT_FLOAT_EQ(a, b, eps) do { \
    if (fabsf((a) - (b)) > (eps)) { \
        char buf[128]; \
        snprintf(buf, sizeof(buf), "expected %.6f, got %.6f", (double)(b), (double)(a)); \
        FAIL(buf); \
        return; \
    } \
} while(0)

/* ---- Tests ---- */

static void test_cpg_init_defaults(void)
{
    TEST("CPG_Init sets default parameters");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    ASSERT_FLOAT_EQ(cpg.alpha,   10.0f,        0.001f);
    ASSERT_FLOAT_EQ(cpg.mu,      1.0f,         0.001f);
    ASSERT_FLOAT_EQ(cpg.epsilon, 0.01f,        0.0001f);
    ASSERT_FLOAT_EQ(cpg.M,       15.0f,        0.001f);
    ASSERT_FLOAT_EQ(cpg.D_base,  40.0f,        0.001f);
    ASSERT_FLOAT_EQ(cpg.lambda,  20.0f,        0.001f);
    ASSERT_FLOAT_EQ(cpg.x,       0.1f,         0.001f);
    ASSERT_FLOAT_EQ(cpg.y,       0.0f,         0.001f);
    ASSERT_FLOAT_EQ(cpg.v_c,     0.0f,         0.001f);
    PASS();
}

static void test_cpg_init_null_safe(void)
{
    TEST("CPG_Init handles NULL pointer");
    CPG_Init(NULL);  // 不应崩溃
    PASS();
}

static void test_cpg_step_null_safe(void)
{
    TEST("CPG_Step handles NULL handle");
    float v = CPG_Step(NULL, 10.0f, 0.001f);
    ASSERT_FLOAT_EQ(v, 0.0f, 0.001f);
    PASS();
}

static void test_cpg_step_nan_protection(void)
{
    TEST("CPG_Step handles NaN input");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);
    float v = CPG_Step(&cpg, NAN, 0.001f);
    // 不应返回 NaN
    assert(!isnan(v));
    // 检查内部状态未被 NaN 污染
    assert(!isnan(cpg.x));
    assert(!isnan(cpg.y));
    assert(!isnan(cpg.v_c));
    PASS();
}

static void test_cpg_step_inf_protection(void)
{
    TEST("CPG_Step handles INF input");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);
    float v = CPG_Step(&cpg, INFINITY, 0.001f);
    assert(!isnan(v) && !isinf(v));
    assert(!isnan(cpg.x) && !isinf(cpg.x));
    PASS();
}

static void test_cpg_step_zero_force(void)
{
    TEST("CPG_Step with zero force converges");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    // 运行若干步，状态应收敛到极限环
    for (int i = 0; i < 5000; i++) {
        float v = CPG_Step(&cpg, 0.0f, 0.001f);
        assert(!isnan(v) && !isinf(v));
    }

    // x 应接近极限环 (μ=1 → 半径≈1)
    float r = sqrtf(cpg.x * cpg.x + cpg.y * cpg.y);
    ASSERT_FLOAT_EQ(r, 1.0f, 0.1f);
    PASS();
}

static void test_cpg_step_positive_force(void)
{
    TEST("CPG_Step with positive force produces positive velocity");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    // 让系统先稳定
    for (int i = 0; i < 1000; i++) {
        CPG_Step(&cpg, 0.0f, 0.001f);
    }

    // 施加正向力，期望产生正速度
    float v = CPG_Step(&cpg, 50.0f, 0.001f);
    // 注：由于导纳模型的惯性，单步可能看不清，多步平均
    float v_sum = 0.0f;
    for (int i = 0; i < 100; i++) {
        v_sum += CPG_Step(&cpg, 50.0f, 0.001f);
    }
    float v_avg = v_sum / 100.0f;
    assert(v_avg > 0.0f);  // 正向力应产生正向速度
    PASS();
}

static void test_cpg_step_negative_force(void)
{
    TEST("CPG_Step with negative force produces negative velocity");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    for (int i = 0; i < 1000; i++) {
        CPG_Step(&cpg, 0.0f, 0.001f);
    }

    float v_sum = 0.0f;
    for (int i = 0; i < 100; i++) {
        v_sum += CPG_Step(&cpg, -50.0f, 0.001f);
    }
    float v_avg = v_sum / 100.0f;
    assert(v_avg < 0.0f);  // 负向力应产生负向速度
    PASS();
}

static void test_cpg_step_velocity_clamped(void)
{
    TEST("CPG_Step velocity stays within limits");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    // 持续施加极大外力
    for (int i = 0; i < 5000; i++) {
        float v = CPG_Step(&cpg, 500.0f, 0.001f);
        assert(v >= -0.5f - 0.01f);  // 允许微小浮点误差
        assert(v <= 1.2f + 0.01f);
    }
    PASS();
}

static void test_cpg_update_frequency(void)
{
    TEST("CPG_Update_Frequency updates omega");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    CPG_Update_Frequency(&cpg, 0.5f);
    float expected = 2.0f * 3.1415926f * 0.5f;
    ASSERT_FLOAT_EQ(cpg.omega, expected, 0.01f);

    CPG_Update_Frequency(&cpg, 1.5f);
    expected = 2.0f * 3.1415926f * 1.5f;
    ASSERT_FLOAT_EQ(cpg.omega, expected, 0.01f);
    PASS();
}

static void test_cpg_update_frequency_out_of_range(void)
{
    TEST("CPG_Update_Frequency rejects out-of-range values");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);
    float original = cpg.omega;

    CPG_Update_Frequency(&cpg, 0.1f);   // 低于下限 0.3
    ASSERT_FLOAT_EQ(cpg.omega, original, 0.001f);

    CPG_Update_Frequency(&cpg, 3.0f);   // 高于上限 2.0
    ASSERT_FLOAT_EQ(cpg.omega, original, 0.001f);

    CPG_Update_Frequency(&cpg, NAN);    // NaN
    ASSERT_FLOAT_EQ(cpg.omega, original, 0.001f);

    PASS();
}

static void test_cpg_step_dt_clamped(void)
{
    TEST("CPG_Step clamps dt to valid range");
    CPG_Handle_t cpg;
    CPG_Init(&cpg);

    // 极小步长不应导致除零或异常
    float v1 = CPG_Step(&cpg, 10.0f, 0.0f);
    assert(!isnan(v1) && !isinf(v1));

    // 极大步长不应导致发散
    float v2 = CPG_Step(&cpg, 10.0f, 10.0f);
    assert(!isnan(v2) && !isinf(v2));
    PASS();
}

/* ---- Runner ---- */

int main(void)
{
    printf("=== SmartRollator CPG Control Unit Tests ===\n\n");

    test_cpg_init_defaults();
    test_cpg_init_null_safe();
    test_cpg_step_null_safe();
    test_cpg_step_nan_protection();
    test_cpg_step_inf_protection();
    test_cpg_step_zero_force();
    test_cpg_step_positive_force();
    test_cpg_step_negative_force();
    test_cpg_step_velocity_clamped();
    test_cpg_update_frequency();
    test_cpg_update_frequency_out_of_range();
    test_cpg_step_dt_clamped();

    printf("\n=== Results: %d run, %d passed, %d failed ===\n",
           tests_run, tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}

#else
/* 当不作为测试编译时，提供空的 main 以避免链接错误 */
int main(void) {
    return 0;
}
#endif /* TEST */
