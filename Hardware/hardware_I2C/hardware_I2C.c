#include "hardware_I2C.h"

// I2C1配置（默认使用PB6-SCL, PB7-SDA）
I2C_Config I2C1_Config = {
    .GPIOx = GPIOB,
    .SCL_Pin = GPIO_Pin_6,
    .SDA_Pin = GPIO_Pin_7,
    .RCC_APBPeriph = RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
    .I2Cx = I2C1,
    .ClockSpeed = 100000  // 100kHz标准模式
};

// I2C2配置（默认使用PB10-SCL, PB11-SDA）
I2C_Config I2C2_Config = {
    .GPIOx = GPIOB,
    .SCL_Pin = GPIO_Pin_10,
    .SDA_Pin = GPIO_Pin_11,
    .RCC_APBPeriph = RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
    .I2Cx = I2C2,
    .ClockSpeed = 100000  // 100kHz标准模式
};

/**
  * @brief  I2C GPIO初始化
  * @param  config: I2C配置结构体指针
  */
void I2C_GPIO_Init(I2C_Config* config) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO和AFIO时钟
    RCC_APB2PeriphClockCmd(config->RCC_APBPeriph, ENABLE);
    
    // 如果使用I2C1，还需要使能I2C1时钟
    if (config->I2Cx == I2C1) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    } 
    // 如果使用I2C2，使能I2C2时钟
    else if (config->I2Cx == I2C2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    }
    
    // 配置SCL引脚为开漏输出
    GPIO_InitStructure.GPIO_Pin = config->SCL_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;  // 复用开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(config->GPIOx, &GPIO_InitStructure);
    
    // 配置SDA引脚为开漏输出
    GPIO_InitStructure.GPIO_Pin = config->SDA_Pin;
    GPIO_Init(config->GPIOx, &GPIO_InitStructure);
}

/**
  * @brief  I2C外设配置初始化（重命名以避免与标准库冲突）
  * @param  I2Cx: I2C外设（I2C1或I2C2）
  * @param  ClockSpeed: I2C时钟速度（Hz）
  */
void I2C_ConfigInit(I2C_TypeDef* I2Cx, uint32_t ClockSpeed) {
    I2C_InitTypeDef I2C_InitStructure;
    
    // 默认初始化参数
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;               // 主机模式，从机地址任意
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    
    // 应用初始化配置
    I2C_Init(I2Cx, &I2C_InitStructure);
    
    // 使能I2C
    I2C_Cmd(I2Cx, ENABLE);
}

/**
  * @brief  简易I2C初始化函数（使用配置结构体）
  * @param  I2Cx: I2C外设（I2C1或I2C2）
  * @param  ClockSpeed: I2C时钟速度（Hz）
  */
void I2C_QuickInit(I2C_TypeDef* I2Cx, uint32_t ClockSpeed) {
    I2C_InitTypeDef I2C_InitStructure;
    
    // 根据I2C外设使能相应的时钟
    if (I2Cx == I2C1) {
        // 使能GPIOB和AFIO时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
        // 使能I2C1时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        
        // 配置PB6(SCL)和PB7(SDA)为复用开漏输出
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    } 
    else if (I2Cx == I2C2) {
        // 使能GPIOB和AFIO时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
        // 使能I2C2时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        
        // 配置PB10(SCL)和PB11(SDA)为复用开漏输出
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    
    // 配置I2C参数
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    
    // 初始化I2C
    I2C_Init(I2Cx, &I2C_InitStructure);
    
    // 使能I2C
    I2C_Cmd(I2Cx, ENABLE);
}


/**
  * @brief  检查I2C总线是否忙
  * @param  I2Cx: I2C外设
  * @param  timeout: 超时值
  * @return I2C状态
  */
I2C_Status I2C_CheckBusy(I2C_TypeDef* I2Cx, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 等待总线空闲
    tickstart = 0;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
        if (++tickstart > timeout) {
            return I2C_BUSY;
        }
    }
    return I2C_OK;
}

/**
  * @brief  发送起始条件
  * @param  I2Cx: I2C外设
  * @param  timeout: 超时值
  * @return I2C状态
  */
I2C_Status I2C_GenerateStart(I2C_TypeDef* I2Cx, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 发送起始条件
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // 等待起始条件发送完成
    tickstart = 0;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (++tickstart > timeout) {
            return I2C_TIMEOUT_ERR;
        }
    }
    return I2C_OK;
}

/**
  * @brief  发送停止条件
  * @param  I2Cx: I2C外设
  * @param  timeout: 超时值
  * @return I2C状态
  */
I2C_Status I2C_GenerateStop(I2C_TypeDef* I2Cx, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 发送停止条件
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    // 等待总线空闲
    tickstart = 0;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)) {
        if (++tickstart > timeout) {
            return I2C_TIMEOUT_ERR;
        }
    }
    return I2C_OK;
}

/**
  * @brief  发送设备地址
  * @param  I2Cx: I2C外设
  * @param  address: 设备地址
  * @param  direction: 方向（0-写，1-读）
  * @param  timeout: 超时值
  * @return I2C状态
  */
I2C_Status I2C_SendAddress(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 发送设备地址
    I2C_Send7bitAddress(I2Cx, address, direction ? I2C_Direction_Receiver : I2C_Direction_Transmitter);
    
    // 等待地址发送完成并收到应答
    if (direction == 0) {  // 写方向
        tickstart = 0;
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            if (++tickstart > timeout) {
                return I2C_TIMEOUT_ERR;
            }
        }
    } else {  // 读方向
        tickstart = 0;
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
            if (++tickstart > timeout) {
                return I2C_TIMEOUT_ERR;
            }
        }
    }
    return I2C_OK;
}

/**
  * @brief  发送一个字节数据
  * @param  I2Cx: I2C外设
  * @param  data: 要发送的数据
  * @param  timeout: 超时值
  * @return I2C状态
  */
I2C_Status I2C_SendByte(I2C_TypeDef* I2Cx, uint8_t data, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 发送数据
    I2C_SendData(I2Cx, data);
    
    // 等待数据发送完成
    tickstart = 0;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (++tickstart > timeout) {
            return I2C_TIMEOUT_ERR;
        }
    }
    return I2C_OK;
}

/**
  * @brief  接收一个字节数据
  * @param  I2Cx: I2C外设
  * @param  timeout: 超时值
  * @return 接收到的数据
  */
uint8_t I2C_ReceiveByte(I2C_TypeDef* I2Cx, uint32_t timeout) {
    uint32_t tickstart = 0;
    
    // 等待接收完成
    tickstart = 0;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
        if (++tickstart > timeout) {
            return 0xFF;  // 超时返回错误值
        }
    }
    
    // 返回接收到的数据
    return I2C_ReceiveData(I2Cx);
}

/**
  * @brief  向设备存储器写入多个字节
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址（7位地址）
  * @param  memAddr: 存储器地址
  * @param  data: 数据缓冲区指针
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_MemWrite(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t memAddr, uint8_t* data, uint16_t len) {
    I2C_Status status;
    uint16_t i;
    
    // 检查总线是否忙
    status = I2C_CheckBusy(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送起始条件
    status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送设备地址（写方向）
    status = I2C_SendAddress(I2Cx, devAddr, 0, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送存储器地址
    status = I2C_SendByte(I2Cx, memAddr, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送数据
    for (i = 0; i < len; i++) {
        status = I2C_SendByte(I2Cx, data[i], I2C_TIMEOUT);
        if (status != I2C_OK) return status;
    }
    
    // 发送停止条件
    status = I2C_GenerateStop(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    return I2C_OK;
}

/**
  * @brief  从设备存储器读取多个字节
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址（7位地址）
  * @param  memAddr: 存储器地址
  * @param  data: 数据缓冲区指针
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_MemRead(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t memAddr, uint8_t* data, uint16_t len) {
    I2C_Status status;
    uint16_t i;
    uint32_t timeout;
    
    // 检查总线是否忙
    status = I2C_CheckBusy(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送起始条件（写模式）
    status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送设备地址（写方向）- 写入存储器地址
    status = I2C_SendAddress(I2Cx, devAddr, 0, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送存储器地址
    status = I2C_SendByte(I2Cx, memAddr, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 重新发送起始条件（重复起始条件）- 切换为读模式
    status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送设备地址（读方向）
    status = I2C_SendAddress(I2Cx, devAddr, 1, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 读取数据
    if (len == 1) {
        // 单字节读取的特殊处理：在接收前就禁用ACK并发送STOP
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        I2C_GenerateSTOP(I2Cx, ENABLE);
        
        // 等待数据接收完成
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (timeout-- == 0) {
                I2C_AcknowledgeConfig(I2Cx, ENABLE);
                return I2C_TIMEOUT_ERR;
            }
        }
        
        // 读取数据
        data[0] = I2C_ReceiveData(I2Cx);
    } else {
        // 多字节读取（>=2）
        for (i = 0; i < len; i++) {
            if (i == len - 1) {
                // 最后一个字节：禁用ACK并发送STOP
                I2C_AcknowledgeConfig(I2Cx, DISABLE);
                I2C_GenerateSTOP(I2Cx, ENABLE);
            } else if (i == 0) {
                // 第一个字节：确保ACK使能（对于多个字节）
                I2C_AcknowledgeConfig(I2Cx, ENABLE);
            }
            
            // 等待数据接收完成
            timeout = I2C_TIMEOUT;
            while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
                if (timeout-- == 0) {
                    I2C_AcknowledgeConfig(I2Cx, ENABLE);
                    return I2C_TIMEOUT_ERR;
                }
            }
            
            // 读取数据
            data[i] = I2C_ReceiveData(I2Cx);
        }
    }
    
    // 重新使能ACK，为后续通信做准备
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    
    // 等待总线空闲
    timeout = I2C_TIMEOUT;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
        if (timeout-- == 0) {
            return I2C_TIMEOUT_ERR;
        }
    }
    
    return I2C_OK;
}

//////=============================================================================================

/**
  * @brief  写入多个字节到设备寄存器
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  regAddr: 寄存器地址
  * @param  data: 数据缓冲区
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len) {
    return I2C_MemWrite(I2Cx, devAddr, regAddr, data, len);
}

/**
  * @brief  从设备寄存器读取多个字节
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  regAddr: 寄存器地址
  * @param  data: 数据缓冲区
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_ReadMulti(   I2C_TypeDef* I2Cx,
                            uint8_t devAddr,
                            uint8_t regAddr,
                            uint8_t* data,
                            uint16_t len) {
    return I2C_MemRead(I2Cx, devAddr, regAddr, data, len);
}

/**
  * @brief  写入单个字节到设备寄存器
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  regAddr: 寄存器地址
  * @param  data: 要写入的数据
  * @return I2C状态
  */
I2C_Status I2C_WriteByte(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return I2C_MemWrite(I2Cx, devAddr, regAddr, &data, 1);
}

/**
  * @brief  从设备寄存器读取单个字节
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  regAddr: 寄存器地址
  * @return 读取到的数据
  */
uint8_t I2C_ReadByte(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr) {
    uint8_t data = 0;
    I2C_MemRead(I2Cx, devAddr, regAddr, &data, 1);
    return data;
}


//=============================================================================================////


/**
  * @brief  写入缓冲区数据（无寄存器地址）
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  data: 数据缓冲区
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_WriteBuffer(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t* data, uint16_t len) {
    I2C_Status status;
    uint16_t i;
    
    // 检查总线是否忙
    status = I2C_CheckBusy(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送起始条件
    status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送设备地址（写方向）
    status = I2C_SendAddress(I2Cx, devAddr, 0, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送数据
    for (i = 0; i < len; i++) {
        status = I2C_SendByte(I2Cx, data[i], I2C_TIMEOUT);
        if (status != I2C_OK) return status;
    }
    
    // 发送停止条件
    status = I2C_GenerateStop(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    return I2C_OK;
}

/**
  * @brief  读取缓冲区数据（无寄存器地址）
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  data: 数据缓冲区
  * @param  len: 数据长度
  * @return I2C状态
  */
I2C_Status I2C_ReadBuffer(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t* data, uint16_t len) {
    I2C_Status status;
    uint16_t i;
    
    // 检查总线是否忙
    status = I2C_CheckBusy(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送起始条件
    status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 发送设备地址（读方向）
    status = I2C_SendAddress(I2Cx, devAddr, 1, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    // 读取数据
    if (len > 1) {
        // 使能应答
        I2C_AcknowledgeConfig(I2Cx, ENABLE);
        
        // 读取前len-1个字节
        for (i = 0; i < len - 1; i++) {
            data[i] = I2C_ReceiveByte(I2Cx, I2C_TIMEOUT);
        }
        
        // 读取最后一个字节前禁用应答
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        
        // 读取最后一个字节
        data[len - 1] = I2C_ReceiveByte(I2Cx, I2C_TIMEOUT);
        
        // 重新使能应答
        I2C_AcknowledgeConfig(I2Cx, ENABLE);
    } else {
        // 单个字节读取
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        data[0] = I2C_ReceiveByte(I2Cx, I2C_TIMEOUT);
        I2C_AcknowledgeConfig(I2Cx, ENABLE);
    }
    
    // 发送停止条件
    status = I2C_GenerateStop(I2Cx, I2C_TIMEOUT);
    if (status != I2C_OK) return status;
    
    return I2C_OK;
}

//=================================================================================================


/**
  * @brief  检查设备是否就绪
  * @param  I2Cx: I2C外设
  * @param  devAddr: 设备地址
  * @param  trials: 尝试次数
  * @return I2C状态
  */
I2C_Status I2C_IsDeviceReady(I2C_TypeDef* I2Cx, uint8_t devAddr, uint32_t trials) {
    uint32_t i;
    I2C_Status status;
    
    for (i = 0; i < trials; i++) {
        // 检查总线是否忙
        status = I2C_CheckBusy(I2Cx, I2C_TIMEOUT);
        if (status != I2C_OK) continue;
        
        // 发送起始条件
        status = I2C_GenerateStart(I2Cx, I2C_TIMEOUT);
        if (status != I2C_OK) continue;
        
        // 发送设备地址（写方向）
        status = I2C_SendAddress(I2Cx, devAddr, 0, I2C_TIMEOUT);
        if (status == I2C_OK) {
            // 发送停止条件
            I2C_GenerateStop(I2Cx, I2C_TIMEOUT);
            return I2C_OK;  // 设备响应了
        }
        
        // 发送停止条件
        I2C_GenerateStop(I2Cx, I2C_TIMEOUT);
    }
    
    return I2C_ERROR;  // 设备未响应
}


//=================================================================================================


