#include "pid.hpp"

void PID_Init(PID *mPID, float Kp, float Ki, float Kd)
{
    mPID->Kp = Kp;
    mPID->Ki = Ki;
    mPID->Kd = Kd;
    mPID->derivative = 0.0f;
    mPID->integral = 0.0f;
    mPID->prev_error = 0.0f;
}

float ComputePID(PID *mPID,
                 float curr_value,
                 float target_value,
                 float Ts)
{
    if(Ts > 0.0f)
    {
        //Proportion
        float error = target_value - curr_value;

        //Integral
        mPID->integral += error * Ts;

        //Derivative
        mPID->derivative = (error - mPID->prev_error) / Ts;
        mPID->prev_error = error;

        return (mPID->Kp * error
                + mPID->Ki * mPID->integral
                + mPID->Kd * mPID->derivative);
    }
    else
        return 0.0f;
}