/**
 * test_controller.c — Controller 模块单元测试模板
 *
 * 编译 (Host PC):
 *   gcc -std=c11 -Wall -DTEST \
 *       -I../../firmware/App -I../../firmware/Core/Inc \
 *       test_controller.c ../../firmware/App/Controller.c \
 *       ../../firmware/App/cpg_control.c -lm -o controller_test
 *   ./controller_test
 *
 * 注意: 需要 mock main.h 中引用的 STM32 HAL 类型。
 */

#ifdef TEST

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

/* ---- Mock STM32 / FreeRTOS 类型 ---- */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef signed int   int32_t;
typedef signed short int16_t;

#define HAL_OK      0x00
#define HAL_ERROR   0x01
#define osOK        0
#define osWaitForever 0xFFFFFFFF

/* ---- 包含被测模块 ---- */
#include "../../firmware/App/data_structs.h"
#include "../../firmware/App/cpg_control.h"
#include "../../firmware/App/Controller.h"

/* ---- 测试框架 ---- */

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

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        char buf[128]; \
        snprintf(buf, sizeof(buf), "expected %d, got %d", (int)(b), (int)(a)); \
        FAIL(buf); \
        return; \
    } \
} while(0)

static CPG_Handle_t my_cpg;

/* ---- Tests ---- */

static void test_controller_zero_force(void)
{
    TEST("Controller_Run with zero force outputs zero velocity");
    CPG_Init(&my_cpg);

    ForceData_t force = {0};
    force.timestamp = 1;

    MotorCmd_t cmd = {0};
    Controller_Run(NULL, &force, NULL, &cmd);

    ASSERT_EQ(cmd.ctrl_mode, 1);
    ASSERT_EQ(cmd.target_vel, 0);  // 零输入 → 零输出（稳态）
    PASS();
}

static void test_controller_null_out_cmd(void)
{
    TEST("Controller_Run handles NULL out_cmd safely");
    CPG_Init(&my_cpg);
    ForceData_t force = {.fx = 10.0f, .timestamp = 1};
    Controller_Run(NULL, &force, NULL, NULL);  // 不应崩溃
    PASS();
}

static void test_controller_stale_force_ignored(void)
{
    TEST("Controller_Run ignores force with zero timestamp");
    CPG_Init(&my_cpg);

    ForceData_t force = {.fx = 100.0f, .timestamp = 0};  // 过期
    MotorCmd_t cmd = {0};
    Controller_Run(NULL, &force, NULL, &cmd);

    ASSERT_EQ(cmd.target_vel, 0);
    PASS();
}

static void test_controller_positive_force_positive_velocity(void)
{
    TEST("Controller_Run with positive fx produces positive velocity");
    CPG_Init(&my_cpg);

    ForceData_t force = {.fx = 30.0f, .timestamp = 1};
    MotorCmd_t cmd = {0};

    // 运行多步让系统响应
    for (int i = 0; i < 500; i++) {
        force.timestamp = i + 1;
        Controller_Run(NULL, &force, NULL, &cmd);
    }

    // 正向力应产生正转速
    assert(cmd.target_vel > 0);
    PASS();
}

static void test_controller_rpm_within_limits(void)
{
    TEST("Controller_Run output stays within MOTOR_RPM_LIMIT");
    CPG_Init(&my_cpg);

    ForceData_t force = {.fx = 490.0f, .timestamp = 1};  // 接近上限的大力
    MotorCmd_t cmd = {0};

    for (int i = 0; i < 2000; i++) {
        force.timestamp = i + 1;
        Controller_Run(NULL, &force, NULL, &cmd);
    }

    assert(cmd.target_vel <= 400);
    assert(cmd.target_vel >= -400);
    PASS();
}

static void test_controller_null_force_handled(void)
{
    TEST("Controller_Run handles NULL force pointer");
    CPG_Init(&my_cpg);
    MotorCmd_t cmd = {0};
    Controller_Run(NULL, NULL, NULL, &cmd);

    ASSERT_EQ(cmd.ctrl_mode, 1);
    ASSERT_EQ(cmd.target_vel, 0);
    PASS();
}

/* ---- Runner ---- */

int main(void)
{
    printf("=== SmartRollator Controller Unit Tests ===\n\n");

    test_controller_zero_force();
    test_controller_null_out_cmd();
    test_controller_stale_force_ignored();
    test_controller_positive_force_positive_velocity();
    test_controller_rpm_within_limits();
    test_controller_null_force_handled();

    printf("\n=== Results: %d run, %d passed, %d failed ===\n",
           tests_run, tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}

#else
int main(void) { return 0; }
#endif
