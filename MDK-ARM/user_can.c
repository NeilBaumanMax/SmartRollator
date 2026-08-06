
#include "user_can.h"

uint8_t TPDO_Config_Status = 0;

//CAN过滤器配置
void CAN_ConfigFilter(void)
{
	  FDCAN_FilterTypeDef can_filter_st;

    can_filter_st.IdType       = FDCAN_STANDARD_ID;       // 标准 ID
    can_filter_st.FilterIndex  = 0;
    can_filter_st.FilterType   = FDCAN_FILTER_MASK;       // 掩码滤波
    can_filter_st.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 接收到 FIFO0
    can_filter_st.FilterID1    = 0x701U;
    can_filter_st.FilterID2    = 0x7FFU; // 标准 ID 掩码
	

    HAL_FDCAN_ConfigFilter(&hfdcan1, &can_filter_st);
	  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
		HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 0);
	
}

//CAN初始化配置
void CAN_Init(void)
{
	CAN_ConfigFilter();//CAN过滤器配置

	
	HAL_FDCAN_Start(&hfdcan1);//开启CAN
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0);
	
}

//CAN标准帧发送函数
uint8_t CAN_Send(uint32_t ID, uint32_t Data_Size, uint8_t* Data)
{
	FDCAN_TxHeaderTypeDef TxHeader;
  uint32_t TxMailbox;
 // 说明：FDCAN 最大支持 64 字节，这里按经典 CAN 使用 8 字节以内
	
	 // 填充发送帧头
    TxHeader.Identifier            = ID;               // 标准 ID
    TxHeader.IdType                = FDCAN_STANDARD_ID; // 标准 ID
    TxHeader.TxFrameType           = FDCAN_DATA_FRAME;  // 数据帧
    switch(Data_Size) {
        case 0:  TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1:  TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2:  TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3:  TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4:  TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5:  TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6:  TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7:  TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:  TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
    }
    
    TxHeader.ErrorStateIndicator   = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch         = FDCAN_BRS_OFF;     // 关闭速率切换，按经典 CAN 发送
    TxHeader.FDFormat              = FDCAN_CLASSIC_CAN; // 经典 CAN 格式，非 CAN FD
    TxHeader.TxEventFifoControl    = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker         = 0;                // 消息标记
     // 将消息加入 FDCAN 发送 FIFO/队列
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, Data) != HAL_OK) {
        return 1; // 发送失败
    }
    return 0; // 发送成功
}

//CAN接收回调函数
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t aData[64] = {0}; // FDCAN 接收缓冲，最大 64 字节(CAN FD)

    // 检查是否有新的 FIFO0 消息
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        // 从 FIFO0 读取消息
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, aData) != HAL_OK)
        {
            Error_Handler();
        }
        else
        {
            // 检测 NMT 心跳：标准 ID 的高位为 0x700
            // 标准帧使用 11 位 ID
            if ((RxHeader.Identifier & 0x701) == 0x701)
            {
                TPDO_Config_Status = 1; // 收到 NMT 心跳，开始配置 PDO
            }

            // TODO: 后续可在这里解析实际反馈帧
        }
        // 说明：HAL_FDCAN_ActivateNotification 会保持通知使能
        // 如需特殊行为，可在这里重新配置中断
    }
}


//初始化控制
void ZLAC8015D_Test(void)
{
	switch(TPDO_Config_Status)
	{
		case 1:
		{
			RPDO0_Config(0x01);//控制字
			RPDO1_Config(0x01);//target velocity
//			RPDO2_Config(0x01);//left motor target position
//			RPDO3_Config(0x01);//right motor target position
			TPDO0_Config(0x01);//actual velocity
			TPDO1_Config(0x01);//actual current
			TPDO2_Config(0x01);//actual position
			TPDO3_Config(0x01);//fault code
			Profile_Velocity_Init(0x01,0x64);
			NMT_Control(0x01, 0x01);
			Driver_Enable(0x01);
			TPDO_Config_Status = 2;
			break;
		}
		case 2:
		{
			Profile_Velocity_Test(0x01);
			break;
		}
	}
}


//保存参数配置至EEPROM
uint8_t Save_EEPROM(uint8_t ID)
{
	uint8_t Data[8] = {0x2B, 0x10, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00};
	return CAN_Send(0x600 + ID, 0x08, Data);
}

//恢复出厂配置
uint8_t Reset_Driver(uint8_t ID)
{
	uint8_t Data[8] = {0x2B, 0x09, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00};
	return CAN_Send(0x600 + ID, 0x08, Data);
}

//RPDO事件触发
//RPDO0映射0x6040（控制字）
//RPDO0-COB-ID:0x200 + ID
uint8_t RPDO0_Config(uint8_t ID)
{
	uint8_t Data[8] = {0x2F, 0x00, 0x14, 0x02, 0xFE, 0x00, 0x00, 0x00};//RPDO0事件触发
	CAN_Send(0x600 + ID, 0x08, Data);
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x00;
	Data[2] = 0x16;
	Data[3] = 0x01;
	Data[4] = 0x10;
	Data[5] = 0x00;
	Data[6] = 0x40;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);// RPDO0 映射 0x6040:00 控制字，长度 0x10 bit
	HAL_Delay(5);

	Data[0] = 0x2F;
	Data[1] = 0x00;
	Data[2] = 0x16;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO0开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//RPDO1事件触发
//RPDO1映射0x60FF 03（目标速度）
//RPDO1-COB-ID:0x300 + ID
uint8_t RPDO1_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x01;
	Data[2] = 0x14;
	Data[3] = 0x02;
	Data[4] = 0xFE;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO1事件触发
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x01;
	Data[2] = 0x16;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x03;
	Data[6] = 0xFF;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO1映射0x60FF 03
	HAL_Delay(5);
	
	Data[0] = 0x2F;
	Data[1] = 0x01;
	Data[2] = 0x16;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO1开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//RPDO2事件触发
//RPDO2映射0x607A 01（左电机目标位置）
//RPDO2-COB-ID:0x400 + ID
uint8_t RPDO2_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x02;
	Data[2] = 0x14;
	Data[3] = 0x02;
	Data[4] = 0xFE;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO2事件触发
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x02;
	Data[2] = 0x16;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x01;
	Data[6] = 0x7A;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO2映射0x607A 01
	HAL_Delay(5);
	
	Data[0] = 0x2F;
	Data[1] = 0x02;
	Data[2] = 0x16;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO2开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//RPDO3事件触发
//RPDO3映射0x607A 02（右电机目标位置）
//RPDO3-COB-ID:0x500 + ID
uint8_t RPDO3_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x03;
	Data[2] = 0x14;
	Data[3] = 0x02;
	Data[4] = 0xFE;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO3事件触发
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x03;
	Data[2] = 0x16;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x02;
	Data[6] = 0x7A;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO3映射0x607A 02
	HAL_Delay(5);
	
	Data[0] = 0x2F;
	Data[1] = 0x03;
	Data[2] = 0x16;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//RPDO3开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//TPDO定时器触发
//TPDO0定时器100ms
//TPDO0映射0x606C 03（反馈速度）
//TPDO0-COB-ID:0x180 + ID
uint8_t TPDO0_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x00;
	Data[2] = 0x18;
	Data[3] = 0x02;
	Data[4] = 0xFF;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO0事件触发
	HAL_Delay(5);
	
	Data[0] = 0x2B;
	Data[1] = 0x00;
	Data[2] = 0x18;
	Data[3] = 0x05;
	Data[4] = 0x28;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO0定时器200*0.5ms
	HAL_Delay(5);


	Data[0] = 0x23;
	Data[1] = 0x00;
	Data[2] = 0x1A;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x03;
	Data[6] = 0x6C;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO0映射0x606C 03
	HAL_Delay(5);

	Data[0] = 0x2F;
	Data[1] = 0x00;
	Data[2] = 0x1A;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO0开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//TPDO1定时器触发
//TPDO1定时器100ms
//TPDO1映射0x6077 03(反馈电流)
//TPDO1-COB-ID:0x280 + ID
uint8_t TPDO1_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x01;
	Data[2] = 0x18;
	Data[3] = 0x02;
	Data[4] = 0xFF;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO1定时器触发
	HAL_Delay(5);
	
	Data[0] = 0x2B;
	Data[1] = 0x01;
	Data[2] = 0x18;
	Data[3] = 0x05;
	Data[4] = 0x28;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);// TPDO1 定时器 200*0.5ms
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x01;
	Data[2] = 0x1A;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x03;
	Data[6] = 0x77;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO1映射0x6077 03
	HAL_Delay(5);
	
	Data[0] = 0x2F;
	Data[1] = 0x01;
	Data[2] = 0x1A;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO1开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//TPDO2定时器触发
//TPDO2定时器时间100ms
//TPDO2映射0x6064 01（左电机反馈位置）
//TPDO2映射0x6064 02（右电机反馈位置）
//TPDO2-COB-ID:0x380 + ID
uint8_t TPDO2_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x02;
	Data[2] = 0x18;
	Data[3] = 0x02;
	Data[4] = 0xFF;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO2事件触发
	HAL_Delay(5);
	
	Data[0] = 0x2B;
	Data[1] = 0x02;
	Data[2] = 0x18;
	Data[3] = 0x05;
	Data[4] = 0x28;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO2定时器200*0.5ms
	HAL_Delay(5);


	Data[0] = 0x23;
	Data[1] = 0x02;
	Data[2] = 0x1A;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x01;
	Data[6] = 0x64;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO2映射0x6064 01
	HAL_Delay(5);
	
	Data[0] = 0x23;
	Data[1] = 0x02;
	Data[2] = 0x1A;
	Data[3] = 0x02;
	Data[4] = 0x20;
	Data[5] = 0x02;
	Data[6] = 0x64;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO2映射0x6064 02
	HAL_Delay(5);

	
	Data[0] = 0x2F;
	Data[1] = 0x02;
	Data[2] = 0x1A;
	Data[3] = 0x00;
	Data[4] = 0x02;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO2开启2个映射
	HAL_Delay(5);

	return 0x00;
}

//TPDO3事件触发
//TPDO3映射0x603F 00（故障码）
// TPDO3 禁止时间 500ms
//TPDO3-COB-ID:0x480 + ID
uint8_t TPDO3_Config(uint8_t ID)
{
	uint8_t Data[8];
	
	Data[0] = 0x2F;
	Data[1] = 0x03;
	Data[2] = 0x18;
	Data[3] = 0x02;
	Data[4] = 0xFE;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO3事件触发
	HAL_Delay(5);

	Data[0] = 0x2B;
	Data[1] = 0x03;
	Data[2] = 0x18;
	Data[3] = 0x03;
	Data[4] = 0xE8;
	Data[5] = 0x03;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO3禁止时间1000*0.5ms
	HAL_Delay(5);

	
	Data[0] = 0x23;
	Data[1] = 0x03;
	Data[2] = 0x1A;
	Data[3] = 0x01;
	Data[4] = 0x20;
	Data[5] = 0x00;
	Data[6] = 0x3F;
	Data[7] = 0x60;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO3映射0x603F 00
	HAL_Delay(5);
	
	Data[0] = 0x2F;
	Data[1] = 0x03;
	Data[2] = 0x1A;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//TPDO3开启1个映射
	HAL_Delay(5);

	return 0x00;
}

//NMT发送
//写入0x01 0x01：开启1号驱动PDO传输
uint8_t NMT_Control(uint8_t Data0, uint8_t ID)
{
	uint8_t Data[2] = {Data0, ID};
	
	return CAN_Send(0x000, 0x02, Data);
}

//使能电机
uint8_t Driver_Enable(uint8_t ID)
{
	uint8_t Data_2Byte[2];
	
	Data_2Byte[0] = 0x06;
	Data_2Byte[1] = 0x00;
	CAN_Send(0x200 + ID, 0x02, Data_2Byte);
	
	Data_2Byte[0] = 0x07;
	Data_2Byte[1] = 0x00;
	CAN_Send(0x200 + ID, 0x02, Data_2Byte);
	HAL_Delay(5);
	
	Data_2Byte[0] = 0x0F;
	Data_2Byte[1] = 0x00;
	return CAN_Send(0x200 + ID, 0x02, Data_2Byte);
}
//速度模式初始化
uint8_t Profile_Velocity_Init(uint8_t ID,uint8_t AccSet)
{
	uint8_t Data[8] = {0x2F, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00};//设置速度模式
	CAN_Send(0x600 + ID, 0x08, Data);
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x83;
	Data[2] = 0x60;
	Data[3] = 0x01;
	Data[4] = AccSet;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);// 设置左电机加速时间，单位由驱动器对象字典定义
	HAL_Delay(5);
	
	Data[0] = 0x23;
	Data[1] = 0x83;
	Data[2] = 0x60;
	Data[3] = 0x02;
	Data[4] = AccSet;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//设置右电机加速时间100ms
	HAL_Delay(5);
	
	Data[0] = 0x23;
	Data[1] = 0x84;
	Data[2] = 0x60;
	Data[3] = 0x01;
	Data[4] = AccSet;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//设置左电机减速时间100ms
	HAL_Delay(5);

	Data[0] = 0x23;
	Data[1] = 0x84;
	Data[2] = 0x60;
	Data[3] = 0x02;
	Data[4] = AccSet;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//设置右电机减速时间100ms
	HAL_Delay(5);

	Data[0] = 0x2B;
	Data[1] = 0x0F;
	Data[2] = 0x20;
	Data[3] = 0x00;
	Data[4] = 0x01;
	Data[5] = 0x00;
	Data[6] = 0x00;
	Data[7] = 0x00;
	CAN_Send(0x600 + ID, 0x08, Data);//设置同步控制方式
	HAL_Delay(5);
	
	return 0x00;
}


//速度模式测试
void Profile_Velocity_Test(uint8_t ID)
{
	uint8_t Data_4Byte[4];
	
//	Data_4Byte[0] = 0x64;
//	Data_4Byte[1] = 0x00;
//	Data_4Byte[2] = 0x64;
//	Data_4Byte[3] = 0x00;
//	CAN_Send(0x300 + ID, 0x04, Data_4Byte);//同步100rpm
//	HAL_Delay(5000);
//	
//	Data_4Byte[0] = 0x9C;
//	Data_4Byte[1] = 0xFF;
//	Data_4Byte[2] = 0x9C;
//	Data_4Byte[3] = 0xFF;
//	CAN_Send(0x300 + ID, 0x04, Data_4Byte);//同步-100rpm
//	HAL_Delay(5000);
	
		Data_4Byte[0] = 0x64;
	Data_4Byte[1] = 0x00;
		Data_4Byte[2] = 0x9C;
	Data_4Byte[3] = 0xFF;
	CAN_Send(0x300 + ID, 0x04, Data_4Byte);//同步100rpm
	HAL_Delay(5000);
	
	Data_4Byte[0] = 0x9C;
	Data_4Byte[1] = 0xFF;
	Data_4Byte[2] = 0x64;
	Data_4Byte[3] = 0x00;
	CAN_Send(0x300 + ID, 0x04, Data_4Byte);//同步-100rpm
	HAL_Delay(5000);
}


