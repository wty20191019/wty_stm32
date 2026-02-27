#include "Serial.h"



uint8_t USART1_RxBuffer[USART1_RX_BUF_SIZE];  // 接收缓冲区
uint16_t USART1_RxLen = 0;                    // 接收数据长度
uint8_t USART1_RxFlag = 0;                    // 接收完成标志（0：未完成，1：完成）

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);      //开启USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       //开启GPIOA的时钟
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    //将PA9引脚初始化为复用推挽输出
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    //将PA10引脚初始化为上拉输入
    
    /*USART初始化*/
    USART_InitTypeDef USART_InitStructure;                                              //定义结构体变量
    USART_InitStructure.USART_BaudRate = USART1_BaudRate;                               //波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制，不需要
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                     //模式，发送模式和接收模式均选择
    USART_InitStructure.USART_Parity = USART_Parity_No;                                 //奇偶校验，不需要
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                              //停止位，选择1位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         //字长，选择8位
    USART_Init(USART1, &USART_InitStructure);                                           //将结构体变量交给USART_Init，配置USART1
    
    /*中断输出配置*/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);              //开启串口接收数据的中断
    
    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;                            //定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //选择配置NVIC的USART1线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;       //指定NVIC线路的抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //指定NVIC线路的响应优先级
    NVIC_Init(&NVIC_InitStructure);                                 //将结构体变量交给NVIC_Init，配置NVIC外设
    
    /*USART使能*/
    USART_Cmd(USART1, ENABLE);                                //使能USART1，串口开始运行
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;    //设置结果初值为1
    while (Y --)            //执行Y次
    {
        Result *= X;        //将X累乘到结果
    }
    return Result;
}

/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  */
void USART1_IRQHandler(void)
{
    uint8_t ucReceivedData;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        ucReceivedData = USART_ReceiveData(USART1); // 读取数据
        
        //将接收的数据存入缓冲区，避免丢失
        if (USART1_RxLen < USART1_RX_BUF_SIZE)      // 防止缓冲区溢出
        {
            USART1_RxBuffer[USART1_RxLen++] = ucReceivedData;
            // 示例：检测到换行符视为一帧数据接收完成（可根据需求修改结束符）
            if (ucReceivedData == '\n' || ucReceivedData == '\r')
            {
                USART1_RxFlag = 1;
            }
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/**
  * 函    数：串口发送单字节
  * 参    数：Byte 要发送的字节
  * 返 回 值：0-发送成功，1-发送超时
  */
uint8_t Serial_SendByte(uint8_t Byte)
{
    //设置合理的超时值（根据波特率调整，115200波特率下1字节约87us，这里设1ms超时足够）
    uint32_t timeout = 10000;  // 超时计数（循环次数，约1ms）
    
    USART_SendData(USART1, Byte);
    
    // 等待发送完成标志置位，同时处理超时
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
        if(--timeout == 0)
        {
            return 1;  // 超时返回错误
        }
    }
    return 0;  // 发送成功
}

/**
  * 函    数：串口发送数组
  * 参    数：Array 要发送的数组首地址
  *         Length 数组长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)               
    {
        Serial_SendByte(Array[i]);        
    }
}

/**
  * 函    数：串口发送字符串
  * 参    数：String 要发送的字符串首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String)
{
    while (*String != '\0')
    {
        Serial_SendByte(*String);
        String++;
    }
}

/**
  * 函    数：串口发送数字（指定长度）
  * 参    数：Number 要发送的数字
  *         Length 数字的位数
  * 返 回 值：无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)        //根据数字长度遍历数字的每一位
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');    //依次发送每位数字
    }
}

/**
  * 函    数：串口格式化打印（安全版，防止缓冲区溢出）
  * 参    数：format 格式化字符串
  *         ... 可变参数
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
    char String[256];                  
    va_list arg;                        
    va_start(arg, format);              
    // 改用vsnprintf，限制最大长度，避免缓冲区溢出
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0'; // 强制结尾符，防止溢出
    va_end(arg);                        
    Serial_SendString(String);    
}

/**
  * 函    数：串口发送结构体（按字节发送）
  * 参    数：pStruct 结构体首地址
  *         Size 结构体字节大小
  * 返 回 值：无
  */
void Serial_SendStruct(void *pStruct, uint16_t Size)
{
    // 将结构体指针转换为字节（uint8_t）指针
    uint8_t *pByte = (uint8_t *)pStruct;
    
    // 循环发送每一个字节
    for(uint16_t i = 0; i < Size; i++)
    {
        Serial_SendByte(pByte[i]);
    }
}

/**
  * 函    数：获取串口接收缓冲区数据
  * 参    数：pBuf 接收数据的缓冲区
  *           pLen 接收数据的长度
  * 返 回 值：0-无数据，1-有数据
  */
uint8_t Serial_GetRxData(uint8_t *pBuf, uint16_t *pLen)
{
    if (USART1_RxFlag == 1)
    {
        memcpy(pBuf, USART1_RxBuffer, USART1_RxLen);
        *pLen = USART1_RxLen;
        
        // 清空接收缓冲区和标志
        USART1_RxLen = 0;
        USART1_RxFlag = 0;
        memset(USART1_RxBuffer, 0, USART1_RX_BUF_SIZE);
        
        return 1;
    }
    return 0;
}

