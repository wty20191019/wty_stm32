#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[6];                   // DMA直接写入的原始ADC值
uint16_t AD_Filtered_Value[6];          // 滤波后的ADC值
volatile uint8_t filter_ready = 0;      // 滤波完成标志

// 滤波模式枚举
typedef enum {
    FILTER_NONE = 0,     // 不滤波
    FILTER_MOVING_AVG,   // 移动平均滤波
    FILTER_MEDIAN,       // 中值滤波
    FILTER_KALMAN        // 卡尔曼滤波
} FilterMode;

FilterMode current_filter_mode = FILTER_NONE;

// 移动平均滤波器
#define MOVING_AVG_SIZE 10
typedef struct {
    uint16_t buffer[MOVING_AVG_SIZE][6];  // 6通道×4个采样
    uint32_t sum[6];                      // 每个通道的累加和
    uint8_t index;                        // 当前索引
    uint8_t count;                        // 当前采样数
} MovingAvgFilter;

MovingAvgFilter moving_avg_filter;

// 中值滤波器
#define MEDIAN_SIZE 10
typedef struct {
    uint16_t buffer[MEDIAN_SIZE][6];      // 6通道×3个采样
    uint8_t index;                        // 当前索引
} MedianFilter;

MedianFilter median_filter;

// 卡尔曼滤波器
typedef struct {
    float Q;    // 过程噪声协方差
    float R;    // 测量噪声协方差
    float x;    // 状态估计
    float P;    // 估计误差协方差
} KalmanFilter;

KalmanFilter kalman_filters[6];

// 初始化移动平均滤波器
void MovingAvg_Init(void) {
    for(int i = 0; i < 6; i++) {
        moving_avg_filter.sum[i] = 0;
        for(int j = 0; j < MOVING_AVG_SIZE; j++) {
            moving_avg_filter.buffer[j][i] = 0;
        }
    }
    moving_avg_filter.index = 0;
    moving_avg_filter.count = 0;
}

// 移动平均滤波处理
void MovingAvg_Process(void) {
    // 将当前采样存入缓冲区
    for(int i = 0; i < 6; i++) {
        // 减去最旧的值
        moving_avg_filter.sum[i] -= moving_avg_filter.buffer[moving_avg_filter.index][i];
        // 加上最新的值
        moving_avg_filter.buffer[moving_avg_filter.index][i] = AD_Value[i];
        moving_avg_filter.sum[i] += AD_Value[i];
    }
    
    // 更新索引
    moving_avg_filter.index = (moving_avg_filter.index + 1) % MOVING_AVG_SIZE;
    
    // 更新计数
    if(moving_avg_filter.count < MOVING_AVG_SIZE) {
        moving_avg_filter.count++;
    }
    
    // 计算平均值
    for(int i = 0; i < 6; i++) {
        AD_Filtered_Value[i] = moving_avg_filter.sum[i] / moving_avg_filter.count;
    }
    
    filter_ready = 1;
}

// 中值滤波排序函数
uint16_t median_of_three(uint16_t a, uint16_t b, uint16_t c) {
    if(a > b) {
        uint16_t temp = a;
        a = b;
        b = temp;
    }
    if(b > c) {
        uint16_t temp = b;
        b = c;
        c = temp;
    }
    if(a > b) {
        uint16_t temp = a;
        a = b;
        b = temp;
    }
    return b;  // 返回中间值
}

// 初始化中值滤波器
void Median_Init(void) {
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < MEDIAN_SIZE; j++) {
            median_filter.buffer[j][i] = 0;
        }
    }
    median_filter.index = 0;
}

// 中值滤波处理
void Median_Process(void) {
    static uint8_t init_count = 0;
    
    // 将当前采样存入缓冲区
    for(int i = 0; i < 6; i++) {
        median_filter.buffer[median_filter.index][i] = AD_Value[i];
    }
    
    // 更新索引
    median_filter.index = (median_filter.index + 1) % MEDIAN_SIZE;
    
    // 等待缓冲区填满
    if(init_count < MEDIAN_SIZE) {
        init_count++;
        filter_ready = 0;
        return;
    }
    
    // 计算中值
    for(int i = 0; i < 6; i++) {
        // 获取缓冲区中的三个值
        uint16_t a = median_filter.buffer[0][i];
        uint16_t b = median_filter.buffer[1][i];
        uint16_t c = median_filter.buffer[2][i];
        
        // 计算中值
        AD_Filtered_Value[i] = median_of_three(a, b, c);
    }
    
    filter_ready = 1;
}

// 初始化卡尔曼滤波器
void Kalman_Init(void) {
    for(int i = 0; i < 6; i++) {
        kalman_filters[i].Q = 0.1f;      // 过程噪声，可根据实际情况调整
        kalman_filters[i].R = 0.5f;      // 测量噪声，可根据实际情况调整
        kalman_filters[i].x = 2048.0f;   // 初始状态估计（ADC中间值）
        kalman_filters[i].P = 1.0f;      // 初始估计误差协方差
    }
}

// 卡尔曼滤波处理
void Kalman_Process(void) {
    for(int i = 0; i < 6; i++) {
        float z = (float)AD_Value[i];  // 测量值
        
        // 预测
        kalman_filters[i].P = kalman_filters[i].P + kalman_filters[i].Q;
        
        // 计算卡尔曼增益
        float K = kalman_filters[i].P / (kalman_filters[i].P + kalman_filters[i].R);
        
        // 更新
        kalman_filters[i].x = kalman_filters[i].x + K * (z - kalman_filters[i].x);
        kalman_filters[i].P = (1.0f - K) * kalman_filters[i].P;
        
        // 转换为整型
        AD_Filtered_Value[i] = (uint16_t)kalman_filters[i].x;
    }
    
    filter_ready = 1;
}

// 滤波处理主函数
void ADC_Filter_Handler(void)
{
    switch(current_filter_mode)
    {
        case FILTER_MOVING_AVG:
            MovingAvg_Process();
            break;
            
        case FILTER_MEDIAN:
            Median_Process();
            break;
            
        case FILTER_KALMAN:
            Kalman_Process();
            break;
            
        case FILTER_NONE:
        default:
            // 不滤波，直接复制
            for(int i = 0; i < 6; i++) {
                AD_Filtered_Value[i] = AD_Value[i];
            }
            filter_ready = 1;
            break;
    }
}

// 获取滤波完成状态
uint8_t ADC_Filter_IsReady(void) {
    return filter_ready;
}

// 清除滤波完成标志
void ADC_Filter_ClearReady(void) {
    filter_ready = 0;
}

// 主ADC初始化函数
void AD_DMA_Init(uint8_t filter_mode) {
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    /*设置ADC时钟*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*规则组通道配置*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 6, ADC_SampleTime_55Cycles5);
    
    /*ADC初始化*/
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 6;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /*DMA初始化*/
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;  // DMA直接写入原始数组
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 6;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /*配置滤波模式*/
    current_filter_mode = filter_mode;
    filter_ready = 0;
    
    // 根据滤波模式初始化相应的滤波器
    switch(filter_mode) {
        case FILTER_MOVING_AVG:
            MovingAvg_Init();
            break;
            
        case FILTER_MEDIAN:
            Median_Init();
            break;
            
        case FILTER_KALMAN:
            Kalman_Init();
            break;
            
        case FILTER_NONE:
        default:
            // 不滤波，无需特殊初始化
            break;
    }
    
    /*DMA和ADC使能*/
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    
    /*ADC校准*/
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1) == SET);
    
    /*ADC触发*/
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



