#include "PID_system.h"


static PID_Controller pid_controllers[MAX_PID_CONTROLLERS];
static uint8_t pid_count = 0;

void PID_System_Init(void)
{
    // 使用memset快速清零
    memset(pid_controllers, 0, sizeof(pid_controllers));
    pid_count = 0;
    
    
}




PID_Controller* PID_Create_Instance(
     float Kp, float Ki, float Kd,
     float K_enabled_integral, 
     float integral_limit,
     float output_min, float output_max)
 {
     if(pid_count >= MAX_PID_CONTROLLERS)
         return NULL;
     PID_Controller *pid = &pid_controllers[pid_count];
     pid_count++;
     PID_Parameter_Set(pid, Kp, Ki, Kd,
                       K_enabled_integral,
                       integral_limit,
                       output_min, output_max);
     PID_Reset(pid);
     PID_Enable(pid, 1);
     return pid;
 }





PID_Controller* PID_Get_Instance(uint8_t index)
{
    if(index >= pid_count)
        return NULL;
    return &pid_controllers[index];
}




void PID_Parameter_Set(
     PID_Controller *pid,
     float Kp, float Ki, float Kd,
     float K_enabled_integral,
     float integral_limit,
     float output_min, float output_max)
 {
     if(pid == NULL) return;
     pid->Kp = Kp;
     pid->Ki = Ki;
     pid->Kd = Kd;
     pid->K_enabled_integral = K_enabled_integral;
     pid->integral_limit = integral_limit;
     pid->output_min = output_min;
     pid->output_max = output_max;
 }


void PID_SetTarget(PID_Controller *pid, float target)
{
    if(pid == NULL) return;
    pid->target = target;
}



void PID_Update(PID_Controller *pid, float input)
 {
     if(pid == NULL || !pid->enabled) return;
     
     float dt = PID_UPDATE_PERIOD / 1000.0f;
     pid->input = input;
     pid->error = pid->target - input;
     
     float enable_th = pid->K_enabled_integral * pid->integral_limit;
     if(pid->error < enable_th && pid->error > -enable_th)
     {
         
         pid->integral += pid->error * dt;
         if(pid->integral >  pid->integral_limit)
             pid->integral =  pid->integral_limit;
         if(pid->integral < -pid->integral_limit)
             pid->integral = -pid->integral_limit;
     }
     
     pid->derivative = (pid->error - pid->prev_error) / dt;
     pid->output = pid->Kp * pid->error +
                   pid->Ki * pid->integral +
                   pid->Kd * pid->derivative;
     if(pid->output > pid->output_max)
         pid->output = pid->output_max;
     if(pid->output < pid->output_min)
         pid->output = pid->output_min;
     pid->prev_error = pid->error;
 }



void PID_Enable(PID_Controller *pid, uint8_t enable)
{
    if(pid == NULL) return;
    
    pid->enabled = enable;
    if(!enable)
        PID_Reset(pid);
}




void PID_Reset(PID_Controller *pid)
{
    if(pid == NULL) return;
    
    pid->input = 0.0f;
    pid->error = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
}




