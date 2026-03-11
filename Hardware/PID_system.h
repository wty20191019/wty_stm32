#ifndef __PID_SYSTEM_H
#define __PID_SYSTEM_H



typedef struct {
    float Target;
    float Actual;
    float Out;
    
    float Kp;
    float Ki;
    float Kd;
    
    float Error0;
    float Error1;
    float ErrorInt;
    
    float OutMax;
    float OutMin;
} PID_t;

void PID_Update(PID_t *p);


#endif /* __PID_SYSTEM_H */







