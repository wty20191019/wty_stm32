#ifndef __PID_SYSTEM_H
#define __PID_SYSTEM_H


#include <stdint.h>

#define MAX_PID_CONTROLLERS 8

typedef struct {
    float Kp;               // 比例系数
    float Ki;               // 积分系数
    float Kd;               // 微分系数
    
    float Target;           // 目标值
    float Actual;           // 实际值
    float Error0;           // 本次误差
    float Error1;           // 上次误差
    float ErrorInt;         // 误差积分
    
    float Out;              // 输出值
    float OutMax;           // 输出最大值
    float OutMin;           // 输出最小值
} PID_Controller;

// 系统初始化
void PID_System_Init(void);

// 创建PID实例
PID_Controller* PID_Create_Instance(void);

// PID更新计算
void PID_Update(PID_Controller* pid);



#endif /* __PID_SYSTEM_H */






//// 示例2：多个PID实例
//void example_multiple_pid(void)
//{
//    PID_System_Init();
//    
//    // 创建多个PID实例
//    PID_Controller* pid1 = PID_Create_Instance();
//    PID_Controller* pid2 = PID_Create_Instance();
//    PID_Controller* pid3 = PID_Create_Instance();
//    
//    if (pid1 == NULL || pid2 == NULL || pid3 == NULL) {
//        return;
//    }
//    
//    // 分别设置参数
//    pid1->Kp = 1.0f; pid1->Ki = 0.1f; pid1->Kd = 0.05f;
//    pid1->OutMax = 100.0f; pid1->OutMin = -100.0f;
//    pid1->Target = 100.0f;
//    
//    pid2->Kp = 0.8f; pid2->Ki = 0.2f; pid2->Kd = 0.1f;
//    pid2->OutMax = 200.0f; pid2->OutMin = -200.0f;
//    pid2->Target = 200.0f;
//    
//    pid3->Kp = 0.5f; pid3->Ki = 0.05f; pid3->Kd = 0.0f;
//    pid3->OutMax = 50.0f; pid3->OutMin = 0.0f;
//    pid3->Target = 30.0f;
//    
//    // 控制循环
//    while (1) {
//        // 更新所有PID控制器
//        pid1->Actual = read_sensor1();
//        pid2->Actual = read_sensor2();
//        pid3->Actual = read_sensor3();
//        
//        PID_Update(pid1);
//        PID_Update(pid2);
//        PID_Update(pid3);
//        
//        set_actuator1(pid1->Out);
//        set_actuator2(pid2->Out);
//        set_actuator3(pid3->Out);
//        
//        delay_ms(10);
//    }
//}






