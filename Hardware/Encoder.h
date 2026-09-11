#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f1xx_hal.h"

void Encoder_Init(void);
int32_t Get_angleSpeed(void);

#endif
