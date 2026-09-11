#include "Potentiometer.h"
#include "Motor.h"

#define POTENTIOMETER_FILTER_ALPHA  0.15f
#define POTENTIOMETER_ZERO_RAW      20U

extern ADC_HandleTypeDef hadc1;

volatile uint16_t Potentiometer_Raw = 0U;
volatile float Potentiometer_Voltage = 0.0f;
volatile float Potentiometer_TargetSpeed = 0.0f;

static float filtered_raw = 0.0f;
static uint8_t first_sample = 1U;

void Potentiometer_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    first_sample = 1U;
}

void Potentiometer_Update(void)
{
    uint16_t raw;

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 2U) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return;
    }

    raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    if (first_sample != 0U)
    {
        filtered_raw = (float)raw;
        first_sample = 0U;
    }
    else
    {
        filtered_raw += POTENTIOMETER_FILTER_ALPHA *
                        ((float)raw - filtered_raw);
    }

    Potentiometer_Raw = (uint16_t)(filtered_raw + 0.5f);
    Potentiometer_Voltage = filtered_raw * POTENTIOMETER_MAX_VOLTAGE /
                            POTENTIOMETER_ADC_FULL_SCALE;

    if (Potentiometer_Raw <= POTENTIOMETER_ZERO_RAW)
    {
        Potentiometer_TargetSpeed = 0.0f;
    }
    else
    {
        Potentiometer_TargetSpeed = filtered_raw *
                                    MOTOR_TARGET_SPEED_MAX_RAD_S /
                                    POTENTIOMETER_ADC_FULL_SCALE;
    }
}
