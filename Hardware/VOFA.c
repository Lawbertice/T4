#include "VOFA.h"
#include "Motor.h"
#include "Potentiometer.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

#define VOFA_SEND_PERIOD_MS  50U

extern UART_HandleTypeDef huart1;

static uint32_t last_send_tick;

void VOFA_Init(void)
{
    last_send_tick = HAL_GetTick();
}

void VOFA_Process(void)
{
    uint32_t now = HAL_GetTick();
    int32_t target_centi_rad_s;
    int32_t actual_centi_rad_s;
    int32_t voltage_centi;
    int length;
    char frame[40];

    if ((uint32_t)(now - last_send_tick) < VOFA_SEND_PERIOD_MS)
    {
        return;
    }
    last_send_tick = now;

    target_centi_rad_s = (int32_t)(Motor_TargetSpeed * 100.0f + 0.5f);
    actual_centi_rad_s = (int32_t)(Motor_ActualSpeed * 100.0f + 0.5f);
    voltage_centi = (int32_t)(Potentiometer_Voltage * 100.0f + 0.5f);

    /* FireWater frame: target speed, actual speed, potentiometer voltage. */
    length = snprintf(frame, sizeof(frame),
                      "%ld.%02ld,%ld.%02ld,%ld.%02ld\r\n",
                      (long)(target_centi_rad_s / 100),
                      (long)(target_centi_rad_s % 100),
                      (long)(actual_centi_rad_s / 100),
                      (long)(actual_centi_rad_s % 100),
                      (long)(voltage_centi / 100),
                      (long)(voltage_centi % 100));

    if (length > 0)
    {
        uint16_t send_length = (length < (int)sizeof(frame)) ?
                               (uint16_t)length : (uint16_t)(sizeof(frame) - 1U);
        HAL_UART_Transmit(&huart1, (uint8_t *)frame, send_length, 10U);
    }
}
