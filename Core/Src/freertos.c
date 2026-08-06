/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_can.h"      // 提供 CAN_Send / RPDO* / NMT_Control / Driver_Enable 等接口
#include "data_structs.h"  // ForceData_t / IMUData_t / MotorFb_t / MotorCmd_t / LogMsg_t
#include "Controller.h"
#include "cpg_control.h"
#include "torque_sensor.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>   // 负责 va_list / va_start / va_end / vsnprintf


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
/* USER CODE BEGIN PTD */
extern UART_HandleTypeDef huart1;   // 由 CubeMX 生成的 USART1 句柄

// 心跳标志位
#define HB_FORCE   (1u << 0)
#define HB_CAN     (1u << 1)
#define HB_IMU     (1u << 2)
#define HB_CTRL    (1u << 3)
#define HB_LOG     (1u << 4)

static monitor_snapshot_t g_mon = {0};

// ---- 串口1接收缓冲（简单行缓冲，回车触发解析）----
static uint8_t  log_rx_ch;                 // 单字节中断接收
static char     log_line_buf[64];          // 简单命令行缓冲
static uint32_t vision_frame_len = 0;
static volatile bool vision_frame_ready = false;
static volatile bool vision_frame_active = false;
static char vision_line_storage[160];

#define vision_rx_ch log_rx_ch
#define log_line_buf vision_line_storage
#define vision_frame_buf vision_line_storage
#define log_line_len vision_frame_len
#define log_line_ready vision_frame_ready

// 周期打印开关与周期
static volatile bool log_stream_on = true;
static const uint32_t LOG_PERIOD_MS = 100;
// 在 Force 任务解析后调用：更新最新力
static inline void Monitor_UpdateForce(const ForceData_t *f);

// 在控制器算完命令后调用：更新最新控制命令
static inline void Monitor_UpdateCmd(const MotorCmd_t *cmd);
static inline void Monitor_UpdateVision(const VisionPose_t *vision);
// 读取快照（Log_task 使用）
static inline void Monitor_ReadSnapshot(monitor_snapshot_t *out);
// 从队列中读取最新一条消息，丢弃旧消息
static inline osStatus_t mq_get_latest(osMessageQueueId_t q, void *item, uint32_t item_size, uint32_t timeout_ms)
{
  osStatus_t st = osMessageQueueGet(q, item, NULL, timeout_ms);
  if (st != osOK) return st;

  uint32_t remain = osMessageQueueGetCount(q);
  while (remain--)
  {
    uint8_t tmp[256];
    if (item_size > sizeof(tmp)) break;
    if (osMessageQueueGet(q, tmp, NULL, 0) == osOK) {
      memcpy(item, tmp, item_size);
    } else {
      break;
    }
  }
  return osOK;
}

static inline bool fresh(uint32_t now, uint32_t ts, uint32_t thresh_ms)
{
  return (ts != 0U) && ((now - ts) <= thresh_ms);
}


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for ForceSensorTask */
osThreadId_t ForceSensorTaskHandle;
uint32_t ForceSensorTaskBuffer[ 512 ];
osStaticThreadDef_t ForceSensorTaskControlBlock;
const osThreadAttr_t ForceSensorTask_attributes = {
  .name = "ForceSensorTask",
  .cb_mem = &ForceSensorTaskControlBlock,
  .cb_size = sizeof(ForceSensorTaskControlBlock),
  .stack_mem = &ForceSensorTaskBuffer[0],
  .stack_size = sizeof(ForceSensorTaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for CAN_Worker_Task */
osThreadId_t CAN_Worker_TaskHandle;
uint32_t CAN_Worker_TaskBuffer[ 512 ];
osStaticThreadDef_t CAN_Worker_TaskControlBlock;
const osThreadAttr_t CAN_Worker_Task_attributes = {
  .name = "CAN_Worker_Task",
  .cb_mem = &CAN_Worker_TaskControlBlock,
  .cb_size = sizeof(CAN_Worker_TaskControlBlock),
  .stack_mem = &CAN_Worker_TaskBuffer[0],
  .stack_size = sizeof(CAN_Worker_TaskBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for IMUtask */
osThreadId_t IMUtaskHandle;
uint32_t IMUtaskBuffer[ 512 ];
osStaticThreadDef_t IMUtaskControlBlock;
const osThreadAttr_t IMUtask_attributes = {
  .name = "IMUtask",
  .cb_mem = &IMUtaskControlBlock,
  .cb_size = sizeof(IMUtaskControlBlock),
  .stack_mem = &IMUtaskBuffer[0],
  .stack_size = sizeof(IMUtaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for ControlTask */
osThreadId_t ControlTaskHandle;
uint32_t ControlTaskBuffer[ 1024 ];
osStaticThreadDef_t ControlTaskControlBlock;
const osThreadAttr_t ControlTask_attributes = {
  .name = "ControlTask",
  .cb_mem = &ControlTaskControlBlock,
  .cb_size = sizeof(ControlTaskControlBlock),
  .stack_mem = &ControlTaskBuffer[0],
  .stack_size = sizeof(ControlTaskBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for WatchdogTask */
osThreadId_t WatchdogTaskHandle;
uint32_t WatchdogTaskBuffer[ 256 ];
osStaticThreadDef_t WatchdogTaskControlBlock;
const osThreadAttr_t WatchdogTask_attributes = {
  .name = "WatchdogTask",
  .cb_mem = &WatchdogTaskControlBlock,
  .cb_size = sizeof(WatchdogTaskControlBlock),
  .stack_mem = &WatchdogTaskBuffer[0],
  .stack_size = sizeof(WatchdogTaskBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for Log_task */
osThreadId_t Log_taskHandle;
const osThreadAttr_t Log_task_attributes = {
  .name = "Log_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for MotorTxTask */
osThreadId_t MotorTxTaskHandle;
uint32_t MotorTxTaskBuffer[ 512 ];
osStaticThreadDef_t MotorTxTaskControlBlock;
const osThreadAttr_t MotorTxTask_attributes = {
  .name = "MotorTxTask",
  .cb_mem = &MotorTxTaskControlBlock,
  .cb_size = sizeof(MotorTxTaskControlBlock),
  .stack_mem = &MotorTxTaskBuffer[0],
  .stack_size = sizeof(MotorTxTaskBuffer),
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for ForceQueueHandle */
osMessageQueueId_t ForceQueueHandleHandle;
uint8_t ForceQueueBuffer[ 8 * sizeof( ForceData_t ) ];
osStaticMessageQDef_t ForceQueueHandleControlBlock;
const osMessageQueueAttr_t ForceQueueHandle_attributes = {
  .name = "ForceQueueHandle",
  .cb_mem = &ForceQueueHandleControlBlock,
  .cb_size = sizeof(ForceQueueHandleControlBlock),
  .mq_mem = &ForceQueueBuffer,
  .mq_size = sizeof(ForceQueueBuffer)
};
/* Definitions for IMUQueueHandle */
osMessageQueueId_t IMUQueueHandleHandle;
uint8_t IMUQueueBuffer[ 8 * sizeof( IMUData_t ) ];
osStaticMessageQDef_t IMUQueueHandleControlBlock;
const osMessageQueueAttr_t IMUQueueHandle_attributes = {
  .name = "IMUQueueHandle",
  .cb_mem = &IMUQueueHandleControlBlock,
  .cb_size = sizeof(IMUQueueHandleControlBlock),
  .mq_mem = &IMUQueueBuffer,
  .mq_size = sizeof(IMUQueueBuffer)
};
/* Definitions for IMUProcessedQueueHandle */
osMessageQueueId_t IMUProcessedQueueHandleHandle;
uint8_t IMUProcessedQueueBuffer[ 8 * sizeof( IMUData_t ) ];
osStaticMessageQDef_t IMUProcessedQueueHandleControlBlock;
const osMessageQueueAttr_t IMUProcessedQueueHandle_attributes = {
  .name = "IMUProcessedQueueHandle",
  .cb_mem = &IMUProcessedQueueHandleControlBlock,
  .cb_size = sizeof(IMUProcessedQueueHandleControlBlock),
  .mq_mem = &IMUProcessedQueueBuffer,
  .mq_size = sizeof(IMUProcessedQueueBuffer)
};
/* Definitions for MotorFbQueueHandle */
osMessageQueueId_t MotorFbQueueHandleHandle;
uint8_t MotorFbQueueBuffer[ 8 * sizeof( MotorFb_t ) ];
osStaticMessageQDef_t MotorFbQueueHandleControlBlock;
const osMessageQueueAttr_t MotorFbQueueHandle_attributes = {
  .name = "MotorFbQueueHandle",
  .cb_mem = &MotorFbQueueHandleControlBlock,
  .cb_size = sizeof(MotorFbQueueHandleControlBlock),
  .mq_mem = &MotorFbQueueBuffer,
  .mq_size = sizeof(MotorFbQueueBuffer)
};
/* Definitions for MotorCmdQueueHandle */
osMessageQueueId_t MotorCmdQueueHandleHandle;
uint8_t MotorCmdQueueBuffer[ 8 * sizeof( MotorCmd_t ) ];
osStaticMessageQDef_t MotorCmdQueueHandleControlBlock;
const osMessageQueueAttr_t MotorCmdQueueHandle_attributes = {
  .name = "MotorCmdQueueHandle",
  .cb_mem = &MotorCmdQueueHandleControlBlock,
  .cb_size = sizeof(MotorCmdQueueHandleControlBlock),
  .mq_mem = &MotorCmdQueueBuffer,
  .mq_size = sizeof(MotorCmdQueueBuffer)
};
/* Definitions for LogQueueHandle */
osMessageQueueId_t LogQueueHandleHandle;
const osMessageQueueAttr_t LogQueueHandle_attributes = {
  .name = "LogQueueHandle"
};
/* Definitions for CtrlLoopTimerHandle */
osTimerId_t CtrlLoopTimerHandleHandle;
const osTimerAttr_t CtrlLoopTimerHandle_attributes = {
  .name = "CtrlLoopTimerHandle"
};
/* Definitions for LogFlushTimerHandle */
osTimerId_t LogFlushTimerHandleHandle;
const osTimerAttr_t LogFlushTimerHandle_attributes = {
  .name = "LogFlushTimerHandle"
};
/* Definitions for MonitorMutex */
osMutexId_t MonitorMutexHandle;
const osMutexAttr_t MonitorMutex_attributes = {
  .name = "MonitorMutex"
};
/* Definitions for ForceRxSemHandle */
osSemaphoreId_t ForceRxSemHandleHandle;
const osSemaphoreAttr_t ForceRxSemHandle_attributes = {
  .name = "ForceRxSemHandle"
};
/* Definitions for CANRxSemHandle */
osSemaphoreId_t CANRxSemHandleHandle;
const osSemaphoreAttr_t CANRxSemHandle_attributes = {
  .name = "CANRxSemHandle"
};
/* Definitions for HeartbeatFlagsHandle */
osEventFlagsId_t HeartbeatFlagsHandleHandle;
const osEventFlagsAttr_t HeartbeatFlagsHandle_attributes = {
  .name = "HeartbeatFlagsHandle"
};
/* Definitions for SystemStatusFlagsHandle */
osEventFlagsId_t SystemStatusFlagsHandleHandle;
const osEventFlagsAttr_t SystemStatusFlagsHandle_attributes = {
  .name = "SystemStatusFlagsHandle"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static inline void uart1_write(const char *s);
// 打印1行（带换行）
static void uart1_printf_ln(const char *fmt, ...);
static bool parse_aruco_frame(const char *frame, VisionPose_t *out);
static bool hex_to_u8(char hi, char lo, uint8_t *out);

extern UART_HandleTypeDef huart2;
/* USER CODE END FunctionPrototypes */

void StartForceSensorTask(void *argument);
void StartCAN_Worker_Task(void *argument);
void StartIMUtask(void *argument);
void StartControlTask(void *argument);
void StartWatchdogTask(void *argument);
void StartLog_task(void *argument);
void StartMotorTxTask(void *argument);
void CtrlLoopTimerCallback(void *argument);
void LogFlushTimerCallback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of MonitorMutex */
  MonitorMutexHandle = osMutexNew(&MonitorMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ForceRxSemHandle */
  ForceRxSemHandleHandle = osSemaphoreNew(1, 1, &ForceRxSemHandle_attributes);

  /* creation of CANRxSemHandle */
  CANRxSemHandleHandle = osSemaphoreNew(1, 1, &CANRxSemHandle_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of CtrlLoopTimerHandle */
  CtrlLoopTimerHandleHandle = osTimerNew(CtrlLoopTimerCallback, osTimerPeriodic, NULL, &CtrlLoopTimerHandle_attributes);

  /* creation of LogFlushTimerHandle */
  LogFlushTimerHandleHandle = osTimerNew(LogFlushTimerCallback, osTimerPeriodic, NULL, &LogFlushTimerHandle_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of ForceQueueHandle */
  ForceQueueHandleHandle = osMessageQueueNew (8, sizeof(ForceData_t), &ForceQueueHandle_attributes);

  /* creation of IMUQueueHandle */
  IMUQueueHandleHandle = osMessageQueueNew (8, sizeof(IMUData_t), &IMUQueueHandle_attributes);

  /* creation of IMUProcessedQueueHandle */
  IMUProcessedQueueHandleHandle = osMessageQueueNew (8, sizeof(IMUData_t), &IMUProcessedQueueHandle_attributes);

  /* creation of MotorFbQueueHandle */
  MotorFbQueueHandleHandle = osMessageQueueNew (8, sizeof(MotorFb_t), &MotorFbQueueHandle_attributes);

  /* creation of MotorCmdQueueHandle */
  MotorCmdQueueHandleHandle = osMessageQueueNew (8, sizeof(MotorCmd_t), &MotorCmdQueueHandle_attributes);

  /* creation of LogQueueHandle */
  LogQueueHandleHandle = osMessageQueueNew (8, sizeof(LogMsg_t), &LogQueueHandle_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ForceSensorTask */
  ForceSensorTaskHandle = osThreadNew(StartForceSensorTask, NULL, &ForceSensorTask_attributes);

  /* creation of CAN_Worker_Task */
  CAN_Worker_TaskHandle = osThreadNew(StartCAN_Worker_Task, NULL, &CAN_Worker_Task_attributes);

  /* creation of IMUtask */
  IMUtaskHandle = osThreadNew(StartIMUtask, NULL, &IMUtask_attributes);

  /* creation of ControlTask */
  ControlTaskHandle = osThreadNew(StartControlTask, NULL, &ControlTask_attributes);

  /* creation of WatchdogTask */
  WatchdogTaskHandle = osThreadNew(StartWatchdogTask, NULL, &WatchdogTask_attributes);

  /* creation of Log_task */
  Log_taskHandle = osThreadNew(StartLog_task, NULL, &Log_task_attributes);

  /* creation of MotorTxTask */
  MotorTxTaskHandle = osThreadNew(StartMotorTxTask, NULL, &MotorTxTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of HeartbeatFlagsHandle */
  HeartbeatFlagsHandleHandle = osEventFlagsNew(&HeartbeatFlagsHandle_attributes);

  /* creation of SystemStatusFlagsHandle */
  SystemStatusFlagsHandleHandle = osEventFlagsNew(&SystemStatusFlagsHandle_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
	HAL_UART_Receive_IT(&huart1, &vision_rx_ch, 1);

  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartForceSensorTask */
/**
  * @brief  Function implementing the ForceSensorTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartForceSensorTask */
void StartForceSensorTask(void *argument)
{
  /* USER CODE BEGIN StartForceSensorTask */
  /* Infinite loop */
		uint8_t tx_data[8] = { 0x01, 0x03, 0x04, 0x00, 0x00, 0x0c, 0x44, 0xff };
    uint8_t rx_data[29];
    ForceData_t f1, f2;

    for (;;)
    {
        // 1) 设为发送模式（DE=1/RE=0）
        RS485_SetMode(1);

        // 2) 发送查询帧（阻塞，超时100ms）
        if (HAL_UART_Transmit(&huart2, tx_data, sizeof(tx_data), 100) == HAL_OK)
        {
            // **一定要等待发送完成**，避免马上切接收丢最后几字节
            // 如果没有专用API，可等待 TC 置位：
            while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET) {
                taskYIELD();
            }
        }

        // 3) 切接收模式（DE=0/RE=1）
        RS485_SetMode(0);

        // 4) 接收固定29字节（阻塞，超时100ms）
        if (HAL_UART_Receive(&huart2, rx_data, sizeof(rx_data), 100) == HAL_OK)
        {
            // 5) 解析
            if (processForceData(rx_data, sizeof(rx_data), &f1, &f2) == 0)
            {
                // 6) 投递给控制任务（按你定义的队列）
							  uint32_t ts = HAL_GetTick();
								f1.timestamp = ts;
								f2.timestamp = ts;
                osStatus_t q1=osMessageQueuePut(ForceQueueHandleHandle, &f1, 0, 0);
//                osStatus_t q2=osMessageQueuePut(ForceQueueHandleHandle, &f2, 0, 0);
							//uart1_printf_ln("[CTRL ] qh=%p", ForceQueueHandleHandle);
							if (q1 != osOK ) {
							uart1_printf_ln("[FORCE] queue put fail q1=%d  cnt=%lu",
							q1, (unsigned long)osMessageQueueGetCount(ForceQueueHandleHandle));
}
								Monitor_UpdateForce(&f1);   // 或 &f2，看你使用哪个把手/通道

                // 心跳（可选）
             //   osEventFlagsSet(HeartbeatFlagsHandle, HEARTBEAT_FORCE);
            }
        }
        else {
            // 超时/错误：可记录日志或置 stale 标志
        }

        // 7) 周期（不要用 HAL_Delay）
        osDelay(10); // 100 Hz 采样；视传感器能力调整
    }
  /* USER CODE END StartForceSensorTask */
}

/* USER CODE BEGIN Header_StartCAN_Worker_Task */
/**
* @brief Function implementing the CAN_Worker_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN_Worker_Task */
void StartCAN_Worker_Task(void *argument)
{
  /* USER CODE BEGIN StartCAN_Worker_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCAN_Worker_Task */
}

/* USER CODE BEGIN Header_StartIMUtask */
/**
* @brief Function implementing the IMUtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIMUtask */
void StartIMUtask(void *argument)
{
  /* USER CODE BEGIN StartIMUtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIMUtask */
}

/* USER CODE BEGIN Header_StartControlTask */
/**
* @brief Function implementing the ControlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartControlTask */
void StartControlTask(void *argument)
{
  /* USER CODE BEGIN StartControlTask */
  const uint32_t CTRL_PERIOD_MS   = 1;    // 1kHz, dt = 0.001s
  const uint32_t FRESH_THRESH_MS  = 100;  // 数据新鲜度阈值
  const uint8_t  DRIVE_ID         = 0x01; // 驱动器 ID
	const uint8_t accset=0x05;//加速度到达时间 20s
  // 最近一次有效数据，传给 Controller_Run 使用
  static IMUData_t   last_imu   = {0};  // 当前 IMU 未实际接入，保留默认值
  static ForceData_t last_force = {0};
  static MotorFb_t   last_fb    = {0};

  MotorCmd_t out_cmd;
  static bool inited = false;

  // 驱动器初始化，当前在任务启动阶段执行一次
  if (!inited) {
    CAN_Init();
    RPDO1_Config(DRIVE_ID);
    Profile_Velocity_Init(DRIVE_ID,accset);
    NMT_Control(0x01, DRIVE_ID);
    Driver_Enable(DRIVE_ID);
    CPG_Init(&my_cpg);
    inited = true;
  }

  TickType_t last = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil(&last, pdMS_TO_TICKS(CTRL_PERIOD_MS));
    uint32_t now = HAL_GetTick();

    // 1) 从 Force 队列中读取最新力数据
    osStatus_t st_force=mq_get_latest(ForceQueueHandleHandle, &last_force, sizeof(last_force), 0);
		   
//		uart1_printf_ln("[FORCE] qh=%p", ForceQueueHandleHandle);
		if (st_force != osOK) {
    // 队列为空/失败：可设一个标志或偶尔打印一次
     //uart1_printf_ln("[DBG] no force msg (%d)", st_force);
			}
    // 2) 读取电机反馈，当前队列可能为空
    (void)mq_get_latest(MotorFbQueueHandleHandle, &last_fb, sizeof(last_fb), 0);
    // 3) IMU 尚未接入，Controller_Run 会收到 timestamp=0 的 last_imu

    // 4) 数据新鲜度判断，避免使用过期数据
    bool force_ok = fresh(now, last_force.timestamp, FRESH_THRESH_MS);
    bool fb_ok    = fresh(now, last_fb.timestamp,    FRESH_THRESH_MS);
    if (!force_ok) {
      // 力数据过期：清零处理
      memset(&last_force, 0, sizeof(last_force));
      last_force.timestamp = 0;
    }
    if (!fb_ok) {
      memset(&last_fb, 0, sizeof(last_fb));
      last_fb.timestamp = 0;
    }

    // 5) 控制器计算：当前主要使用力数据，预留电机反馈/IMU
    memset(&out_cmd, 0, sizeof(out_cmd));
    out_cmd.ctrl_mode = 1; // 速度模式，由 Controller_Run 继续填充命令
    out_cmd.timestamp = now;
		
    Controller_Run(&last_imu, &last_force, &last_fb, &out_cmd);
		Monitor_UpdateCmd(&out_cmd);
    // 6) 将 out_cmd 投递到 MotorCmd 队列，交给 MotorTxTask 发送
    (void)osMessageQueuePut(MotorCmdQueueHandleHandle, &out_cmd, 0, 0);

    // 7) 更新控制任务心跳
    osEventFlagsSet(HeartbeatFlagsHandleHandle, HB_CTRL);
		//osDelay(10);
  }
  /* USER CODE END StartControlTask */
}

/* USER CODE BEGIN Header_StartWatchdogTask */
/**
* @brief Function implementing the WatchdogTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWatchdogTask */
void StartWatchdogTask(void *argument)
{
  /* USER CODE BEGIN StartWatchdogTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartWatchdogTask */
}

/* USER CODE BEGIN Header_StartLog_task */
/**
* @brief Function implementing the Log_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLog_task */
void StartLog_task(void *argument)
{
  /* USER CODE BEGIN StartLog_task */
  /* Infinite loop */
  /* USER CODE BEGIN StartLog_task */
  uint32_t next_tick = HAL_GetTick();

  // 欢迎/帮助
  uart1_write("\r\n[VISION] USART1 receiver ready, waiting for $ARUCO,...*CS#\r\n");

  for(;;)
  {
    // 1) 命令行解析（非阻塞）
    if (log_line_ready) {
      char cmd[160];
      size_t len;
      // 取走命令
      __disable_irq();
      strncpy(cmd, log_line_buf, sizeof(cmd));
      len = log_line_len;
      log_line_len = 0;
      log_line_ready = false;
      __enable_irq();

      if (len > 0U && cmd[0] == '$') {
        VisionPose_t vision;

        if (parse_aruco_frame(cmd, &vision)) {
          Monitor_UpdateVision(&vision);
          uart1_printf_ln("[VISION] seq=%u st=%u id=%u x=%d y=%d z=%d dist=%d",
                          (unsigned)vision.seq,
                          (unsigned)vision.status,
                          (unsigned)vision.marker_id,
                          (int)vision.x_mm,
                          (int)vision.y_mm,
                          (int)vision.z_mm,
                          (int)vision.dist_mm);
        } else {
          uart1_printf_ln("[VISION] bad frame: %s", cmd);
        }
        continue;
      }

      // 小写化（只有几条简命令可不必）
      // 解析命令
      if (len == 1 && (cmd[0]=='h' || cmd[0]=='H')) {
        uart1_write(
          "Commands:\r\n"
          "  h : help\r\n"
          "  p : print once\r\n"
          "  t : toggle periodic stream\r\n"
        );
      } else if (len == 1 && (cmd[0]=='t' || cmd[0]=='T')) {
        log_stream_on = !log_stream_on;
        uart1_printf_ln("[LOG] stream %s", log_stream_on ? "ON":"OFF");
      } else if (len == 1 && (cmd[0]=='p' || cmd[0]=='P')) {
        monitor_snapshot_t snap;
        Monitor_ReadSnapshot(&snap);
        uart1_printf_ln("F: fx=%d fy=%d fz=%d ts=%lu ms",
                        (int)snap.force.fx, (int)snap.force.fy, (int)snap.force.fz,
                        (unsigned long)snap.ts_force);
        uart1_printf_ln("CMD: mode=%u vel=%ld tq=%d ts=%lu ms",
                        (unsigned)snap.cmd.ctrl_mode,
                        (long)snap.cmd.target_vel,
                        (int)snap.cmd.target_torque,
                        (unsigned long)snap.ts_cmd);
      } else {
        uart1_printf_ln("[LOG] unknown cmd: %s", cmd);
      }
    }

    // 2) 周期打印
    if (log_stream_on) {
      uint32_t now = HAL_GetTick();
      if ((int32_t)(now - next_tick) >= 0) {
        next_tick += LOG_PERIOD_MS;

        monitor_snapshot_t snap;
        Monitor_ReadSnapshot(&snap);

        // 一行打印，便于上位机记录
        // 你也可以改为 CSV 格式：ts,fx,fy,fz,mode,vel,tq
        uart1_printf_ln("ts=%lu | F[fx=%f,fy=%f,fz=%f] | CMD[mode=%u,vel=%ld,tq=%d]",
                        (unsigned long)now,
                        (double)snap.force.fx, (double)snap.force.fy, (double)snap.force.fz,
                        (unsigned)snap.cmd.ctrl_mode, (long)snap.cmd.target_vel, (int)snap.cmd.target_torque);
      }
    }

    // 3) 降低占用
    osDelay(5);
  }
  /* USER CODE END StartLog_task */
  /* USER CODE END StartLog_task */
}

/* USER CODE BEGIN Header_StartMotorTxTask */
/**
* @brief Function implementing the MotorTxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotorTxTask */
void StartMotorTxTask(void *argument)
{
  /* USER CODE BEGIN StartMotorTxTask */
  const uint8_t DRIVE_ID = 0x01;
  MotorCmd_t cmd;

  for(;;)
  {
    if (osMessageQueueGet(MotorCmdQueueHandleHandle, &cmd, NULL, osWaitForever) == osOK)
    {
      if (cmd.ctrl_mode == 1) { // 速度模式
        int16_t wheel_rpm = (int16_t)cmd.target_vel;
        int16_t left_rpm  = wheel_rpm;
        int16_t right_rpm = (int16_t)(-wheel_rpm); // 右轮安装方向相反，数值幅值与左轮严格一致
        (void)ZLAC_SetVelocity_rpm(DRIVE_ID, left_rpm, right_rpm);
      }
      // 其他控制模式可在此扩展
    }
  }
  /* USER CODE END StartMotorTxTask */
}

/* CtrlLoopTimerCallback function */
void CtrlLoopTimerCallback(void *argument)
{
  /* USER CODE BEGIN CtrlLoopTimerCallback */

  /* USER CODE END CtrlLoopTimerCallback */
}

/* LogFlushTimerCallback function */
void LogFlushTimerCallback(void *argument)
{
  /* USER CODE BEGIN LogFlushTimerCallback */

  /* USER CODE END LogFlushTimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
// 在 Force 任务解析后调用：更新最新力
static inline void Monitor_UpdateForce(const ForceData_t *f)
{
  if (!f) return;
  if (MonitorMutexHandle) osMutexAcquire(MonitorMutexHandle, osWaitForever);
  g_mon.force = *f;
  g_mon.ts_force = HAL_GetTick();
  if (MonitorMutexHandle) osMutexRelease(MonitorMutexHandle);
}

// 在控制器算完命令后调用：更新最新控制命令
static inline void Monitor_UpdateCmd(const MotorCmd_t *cmd)
{
  if (!cmd) return;
  if (MonitorMutexHandle) osMutexAcquire(MonitorMutexHandle, osWaitForever);
  g_mon.cmd = *cmd;
  g_mon.ts_cmd = HAL_GetTick();
  if (MonitorMutexHandle) osMutexRelease(MonitorMutexHandle);
}

// 读取快照（Log_task 使用）
static inline void Monitor_UpdateVision(const VisionPose_t *vision)
{
  if (!vision) return;
  if (MonitorMutexHandle) osMutexAcquire(MonitorMutexHandle, osWaitForever);
  g_mon.vision = *vision;
  g_mon.ts_vision = HAL_GetTick();
  if (MonitorMutexHandle) osMutexRelease(MonitorMutexHandle);
}

static inline void Monitor_ReadSnapshot(monitor_snapshot_t *out)
{
  if (!out) return;
  if (MonitorMutexHandle) osMutexAcquire(MonitorMutexHandle, osWaitForever);
  *out = g_mon;
  if (MonitorMutexHandle) osMutexRelease(MonitorMutexHandle);
}

// 中断回调：1字节到达
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    uint8_t c = log_rx_ch;

    if (!log_line_ready) {
      if (c == '\r' || c == '\n') {
        if (log_line_len > 0) { log_line_buf[log_line_len] = '\0'; log_line_ready = true; }
      } else {
        if (log_line_len < (sizeof(log_line_buf)-1)) {
          log_line_buf[log_line_len++] = (char)c;
        } else {
          // 溢出则清空
          log_line_len = 0;
        }
      }
    }

    // 继续下一字节接收
    HAL_UART_Receive_IT(&huart1, &log_rx_ch, 1);
  }
}

// 简单串口发送（阻塞）
static inline void uart1_write(const char *s)
{
  if (!s) return;
  HAL_UART_Transmit(&huart1, (uint8_t*)s, (uint16_t)strlen(s), 50);
}

// 打印1行（带换行）
static void uart1_printf_ln(const char *fmt, ...)
{
  char buf[128];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  if (n < 0) return;
  if (n > (int)sizeof(buf)-3) n = (int)sizeof(buf)-3;
  buf[n++] = '\r'; buf[n++] = '\n'; buf[n] = '\0';
  uart1_write(buf);
}

static bool hex_to_u8(char hi, char lo, uint8_t *out)
{
  uint8_t high;
  uint8_t low;

  if (hi >= '0' && hi <= '9') high = (uint8_t)(hi - '0');
  else if (hi >= 'A' && hi <= 'F') high = (uint8_t)(hi - 'A' + 10U);
  else if (hi >= 'a' && hi <= 'f') high = (uint8_t)(hi - 'a' + 10U);
  else return false;

  if (lo >= '0' && lo <= '9') low = (uint8_t)(lo - '0');
  else if (lo >= 'A' && lo <= 'F') low = (uint8_t)(lo - 'A' + 10U);
  else if (lo >= 'a' && lo <= 'f') low = (uint8_t)(lo - 'a' + 10U);
  else return false;

  *out = (uint8_t)((high << 4) | low);
  return true;
}

static bool parse_aruco_frame(const char *frame, VisionPose_t *out)
{
  const char *asterisk;
  const char *p;
  char body[128];
  char *cursor;
  long values[10];
  uint8_t calc_cs = 0;
  uint8_t recv_cs;
  size_t body_len;
  int i;

  if ((frame == NULL) || (out == NULL) || (frame[0] != '$')) return false;

  asterisk = strchr(frame, '*');
  if (asterisk == NULL) return false;
  if ((asterisk[1] == '\0') || (asterisk[2] == '\0') || (asterisk[3] != '#')) return false;

  for (p = frame + 1; p < asterisk; ++p) {
    calc_cs ^= (uint8_t)(*p);
  }

  if (!hex_to_u8(asterisk[1], asterisk[2], &recv_cs)) return false;
  if (calc_cs != recv_cs) return false;

  body_len = (size_t)(asterisk - (frame + 1));
  if (body_len >= sizeof(body)) return false;

  memcpy(body, frame + 1, body_len);
  body[body_len] = '\0';

  if (strncmp(body, "ARUCO,", 6) != 0) return false;

  cursor = body + 6;
  for (i = 0; i < 10; ++i) {
    char *endp;

    values[i] = strtol(cursor, &endp, 10);
    if (endp == cursor) return false;
    if (i < 9) {
      if (*endp != ',') return false;
      cursor = endp + 1;
    } else if (*endp != '\0') {
      return false;
    }
  }

  out->seq = (uint16_t)values[0];
  out->status = (uint8_t)values[1];
  out->marker_id = (uint16_t)values[2];
  out->x_mm = (int16_t)values[3];
  out->y_mm = (int16_t)values[4];
  out->z_mm = (int16_t)values[5];
  out->roll_x10 = (int16_t)values[6];
  out->pitch_x10 = (int16_t)values[7];
  out->yaw_x10 = (int16_t)values[8];
  out->dist_mm = (int16_t)values[9];
  out->timestamp = HAL_GetTick();

  return true;
}
/* USER CODE END Application */

