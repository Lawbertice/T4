#ifndef __POTENTIOMETER_H
#define __POTENTIOMETER_H

#include "stm32f1xx_hal.h"

#define POTENTIOMETER_MAX_VOLTAGE       3.3f
#define POTENTIOMETER_ADC_FULL_SCALE    4095.0f

extern volatile uint16_t Potentiometer_Raw;
extern volatile float Potentiometer_Voltage;
extern volatile float Potentiometer_TargetSpeed;

void Potentiometer_Init(void);
void Potentiometer_Update(void);

#endif
