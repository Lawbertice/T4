#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f1xx_hal.h"

#define MOTOR_TARGET_SPEED_MAX_RAD_S  75.0f

extern volatile float Motor_TargetSpeed;
extern volatile float Motor_ActualSpeed;

void Motor_Init(void);
void Motor_Control(void);
void Motor_SetTargetSpeed(float target_speed_rad_s);
void Motor_SetDuty(uint16_t duty_permille);
void Motor_Stop(void);
uint16_t Motor_GetDuty(void);

#endif
