#include "mpu6050.h"
#include "I2C2.h" 
#include "DWT_Delay.h"                  //Base    DWT
#include "stm32f10x.h"                  // Device header

#define delay_ms(x)     DWT_Delay_ms(x)
#define I2C_TIMEOUT     10000



/**
  * @brief  向MPU6050写入多个寄存器数据
  * @param  addr: 器件I2C地址 (7位地址，函数内部会左移)
  * @param  reg:  要写入的寄存器地址
  * @param  len:  要写入的数据长度
  * @param  buf:  数据缓冲区指针
  * @retval 0: 成功, 其他: 失败 (错误码)
  */
uint8_t mpu6050_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf)
{
    addr=addr<<1;
    
    uint32_t timeout;
    uint8_t i;
    int8_t ret = 0;

    // 1. 发送起始条件
    I2C_GenerateSTART(I2C2, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((timeout--) == 0) 
        {
            ret = -1;
            goto error;
        }
    }

    // 2. 发送器件地址（写）
    I2C_Send7bitAddress(I2C2, addr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((timeout--) == 0) 
        {
            ret = -2;
            goto error;
        }
    }

    // 3. 发送寄存器地址
    I2C_SendData(I2C2, reg);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    {
        if((timeout--) == 0) 
        {
            ret = -3;
            goto error;
        }
    }

    // 4. 循环发送数据
    for(i = 0; i < len; i++)
    {
        I2C_SendData(I2C2, buf[i]);
        timeout = I2C_TIMEOUT;

        if(i == (len - 1))
        {
            // 最后一个字节，等待传输完成
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            {
                if((timeout--) == 0) 
                {
                    ret = -4;
                    goto error;
                }
            }
        }
        else
        {
            // 非最后一个字节，等待进入"正在传输"状态
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
            {
                if((timeout--) == 0) 
                {
                    ret = -5;
                    goto error;
                }
            }
        }
    }

    // 5. 发送停止条件
    I2C_GenerateSTOP(I2C2, ENABLE);

    // 可选：等待总线空闲
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
    {
        if((timeout--) == 0) 
        {
            ret = -6;
            goto error;
        }
    }

    return 0; // 成功

error:
    // 发生错误时发送停止条件并返回错误码
    I2C_GenerateSTOP(I2C2, ENABLE);
    return ret;
}




/**
  * @brief  从MPU6050读取多个寄存器数据
  * @param  addr: 器件I2C地址 (7位地址，函数内部会左移)
  * @param  reg:  要读取的寄存器地址
  * @param  len:  要读取的数据长度
  * @param  buf:  数据缓冲区指针
  * @retval 0: 成功, 其他: 失败 (错误码)
  */
uint8_t mpu6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    addr=addr<<1;
    
    uint32_t timeout;
    uint8_t i;
    int8_t ret = 0;

    // 1. 发送起始条件（写模式）
    I2C_GenerateSTART(I2C2, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((timeout--) == 0)
        {
            ret = -1;
            goto error;
        }
    }
    
    

    // 2. 发送器件地址（写模式）
    I2C_Send7bitAddress(I2C2, addr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((timeout--) == 0)
        {
            ret = -2;
            goto error;
        }
    }

    // 3. 发送寄存器地址
    I2C_SendData(I2C2, reg);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((timeout--) == 0)
        {
            ret = -3;
            goto error;
        }
    }
    
    // 4. 重新发送起始条件（读模式）
    I2C_GenerateSTART(I2C2, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((timeout--) == 0)
        {
            ret = -4;
            goto error;
        }
    }
    
    // 5. 发送器件地址（读模式）
    I2C_Send7bitAddress(I2C2, addr, I2C_Direction_Receiver);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((timeout--) == 0)
        {
            ret = -5;
            goto error;
        }
    }
    
    // 6. 读取数据
    for(i = 0; i < len; i++)
    {
        if(i == (len - 1))
        {
            // 最后一个字节，禁用ACK并生成停止条件
            I2C_AcknowledgeConfig(I2C2, DISABLE);
            I2C_GenerateSTOP(I2C2, ENABLE);
        }
        
        // 等待接收数据
        timeout = I2C_TIMEOUT;
        while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if((timeout--) == 0)
            {
                ret = -6;
                I2C_AcknowledgeConfig(I2C2, ENABLE); // 恢复ACK使能
                goto error;
            }
        }
        
        // 读取数据
        buf[i] = I2C_ReceiveData(I2C2);
    }
    
    // 7. 重新使能ACK，为后续通信做准备
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    
    // 8. 等待总线空闲
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
    {
        if((timeout--) == 0)
        {
            ret = -7;
            goto error;
        }
    }
    
    return 0; // 读操作成功

error:
    // 发生错误时发送停止条件
    I2C_GenerateSTOP(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE); // 确保ACK重新使能
    return ret;
}






void mpu6050_write_reg(uint8_t reg, uint8_t dat)
{
   mpu6050_write(MPU_ADDR,reg,1,&dat);
}

uint8_t   mpu6050_read_reg (uint8_t reg)
{
      uint8_t dat;
   mpu6050_read(MPU_ADDR,reg,1,&dat);
      return dat;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
     mpu6050_write_reg(GYRO_CONFIG,fsr<<3);//设置陀螺仪满量程范围  
     return 0;
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
     mpu6050_write_reg(ACCEL_CONFIG,fsr<<3);//设置加速度传感器满量程范围  
     return 0;
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_LPF(u16 lpf)
{
     uint8_t data=0;
     if(lpf>=188)data=1;
     else if(lpf>=98)data=2;
     else if(lpf>=42)data=3;
     else if(lpf>=20)data=4;
     else if(lpf>=10)data=5;
     else data=6; 
     mpu6050_write_reg(MPU_CFG_REG,data);//设置数字低通滤波器  
     return 0;
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Rate(u16 rate)
{
     uint8_t data;
     if(rate>1000)rate=1000;
     if(rate<4)rate=4;
     data=1000/rate-1;
     mpu6050_write_reg(MPU_SAMPLE_RATE_REG,data);     //设置数字低通滤波器
      return MPU_Set_LPF(rate/2);     //自动设置LPF为采样率的一半
}

void MPU6050_Init(void)
{ 
    uint8_t res; 
    I2C2_Init();                                    //初始化IIC总线
    mpu6050_write_reg(PWR_MGMT_1,0X80);             //复位MPU6050
    delay_ms(100);
    mpu6050_write_reg(PWR_MGMT_1,0X00);             //唤醒MPU6050 
    MPU_Set_Gyro_Fsr(3);                            //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                           //加速度传感器,±2g
    MPU_Set_Rate(200);                              //设置采样率50Hz
    mpu6050_write_reg(MPU_INT_EN_REG,0X00);         //关闭所有中断
    mpu6050_write_reg(MPU_USER_CTRL_REG,0X00);      //I2C主模式关闭
    mpu6050_write_reg(MPU_FIFO_EN_REG,0X00);        //关闭FIFO
    mpu6050_write_reg(MPU_INTBP_CFG_REG,0X80);      //INT引脚低电平有效
    res=mpu6050_read_reg(MPU_DEVICE_ID_REG); 
    
    if(res==MPU_ADDR)//器件ID正确
    {
        mpu6050_write_reg(PWR_MGMT_1,0X01);         //设置CLKSEL,PLL X轴为参考
        mpu6050_write_reg(PWR_MGMT_2,0X00);         //加速度与陀螺仪都工作
        MPU_Set_Rate(100);                          //设置采样率为50Hz
    }
}
 
//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
     float temp;
     mpu6050_read(MPU_ADDR,TEMP_OUT_H,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res;  
     res=mpu6050_read(MPU_ADDR,GYRO_XOUT_H,6,buf);
     if(res==0)
     {
          *gx=((u16)buf[0]<<8)|buf[1];  
          *gy=((u16)buf[2]<<8)|buf[3];  
          *gz=((u16)buf[4]<<8)|buf[5];
     }      
    return res;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
     res=mpu6050_read(MPU_ADDR,ACCEL_XOUT_H,6,buf);
     if(res==0)
     {
          *ax=((u16)buf[0]<<8)|buf[1];  
          *ay=((u16)buf[2]<<8)|buf[3];  
          *az=((u16)buf[4]<<8)|buf[5];
     }      
    return res;
}
