#include <stdio.h>
#include <math.h>

typedef struct
{
    float Kp,Ki,Kd;
    float integral;
    float derivative;
    float prev_error;
}PID;

void PID_Init(PID *mPID);

float ComputePID(PID *mPID,
                 float curr_value,
                 float target_value,
                 float Ts);