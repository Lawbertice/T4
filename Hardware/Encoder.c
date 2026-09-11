#include "Encoder.h"

#define ENCODER_PPR                 13.0f
#define ENCODER_QUADRATURE_FACTOR    4.0f
#define MOTOR_REDUCTION_RATIO       20.409f
#define TWO_PI                       6.283185307f

extern TIM_HandleTypeDef htim3;

static uint16_t last_count;
static uint32_t last_tick;

void Encoder_Init(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    last_count = 0;
    last_tick = HAL_GetTick();
}

int32_t Get_angleSpeed(void)
{
    uint16_t current_count = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
    uint32_t current_tick = HAL_GetTick();
    uint32_t elapsed_ms = current_tick - last_tick;
    int16_t delta_count;
    float angle_speed;

    if (elapsed_ms == 0U)
    {
        return 0;
    }

    /* Signed 16-bit subtraction handles one TIM3 counter wrap correctly. */
    delta_count = (int16_t)(current_count - last_count);
    last_count = current_count;
    last_tick = current_tick;

    angle_speed = (float)delta_count * TWO_PI * 100000.0f;
    angle_speed /= ENCODER_PPR * ENCODER_QUADRATURE_FACTOR;
    angle_speed /= MOTOR_REDUCTION_RATIO * (float)elapsed_ms;

    return (int32_t)(angle_speed + ((angle_speed >= 0.0f) ? 0.5f : -0.5f));
}

