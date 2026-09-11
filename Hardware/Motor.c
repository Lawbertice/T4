#include "Motor.h"
#include "Encoder.h"

/* Tune these two parameters while observing VOFA+. */
#define MOTOR_SPEED_KP             2.0f
#define MOTOR_SPEED_KI             0.28f
#define MOTOR_SPEED_FILTER_ALPHA   0.35f
#define MOTOR_CONTROL_PERIOD_MS    10U
#define MOTOR_DUTY_MAX             1000U

#define MOTOR_AIN1_PORT            GPIOA
#define MOTOR_AIN1_PIN             GPIO_PIN_4
#define MOTOR_AIN2_PORT            GPIOA
#define MOTOR_AIN2_PIN             GPIO_PIN_5

extern TIM_HandleTypeDef htim2;

volatile float Motor_TargetSpeed = 0.0f;
volatile float Motor_ActualSpeed = 0.0f;

static float last_error = 0.0f;
static float pwm_command = 0.0f;
static uint16_t motor_duty = 0U;
static uint32_t last_control_tick = 0U;

void Motor_SetDuty(uint16_t duty_permille)
{
    uint32_t timer_period;
    uint32_t compare;

    if (duty_permille > MOTOR_DUTY_MAX)
    {
        duty_permille = MOTOR_DUTY_MAX;
    }

    timer_period = __HAL_TIM_GET_AUTORELOAD(&htim2) + 1U;
    compare = timer_period * duty_permille / MOTOR_DUTY_MAX;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, compare);
    motor_duty = duty_permille;
}

void Motor_Init(void)
{
    /* AIN2=3.3 V and AIN1=0 V is the requested forward direction. */
    HAL_GPIO_WritePin(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);

    Motor_SetDuty(0U);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    Encoder_Init();

    Motor_TargetSpeed = 0.0f;
    Motor_ActualSpeed = 0.0f;
    last_error = 0.0f;
    pwm_command = 0.0f;
    last_control_tick = HAL_GetTick();
}

void Motor_SetTargetSpeed(float target_speed_rad_s)
{
    if (target_speed_rad_s < 0.0f)
    {
        target_speed_rad_s = 0.0f;
    }
    else if (target_speed_rad_s > MOTOR_TARGET_SPEED_MAX_RAD_S)
    {
        target_speed_rad_s = MOTOR_TARGET_SPEED_MAX_RAD_S;
    }

    Motor_TargetSpeed = target_speed_rad_s;
}

void Motor_Stop(void)
{
    Motor_TargetSpeed = 0.0f;
    Motor_ActualSpeed = 0.0f;
    last_error = 0.0f;
    pwm_command = 0.0f;
    Motor_SetDuty(0U);
}

uint16_t Motor_GetDuty(void)
{
    return motor_duty;
}

void Motor_Control(void)
{
    uint32_t now = HAL_GetTick();
    int32_t measured_speed_centi_rad_s;
    float measured_speed;
    float error;

    if ((uint32_t)(now - last_control_tick) < MOTOR_CONTROL_PERIOD_MS)
    {
        return;
    }
    last_control_tick = now;

    measured_speed_centi_rad_s = Get_angleSpeed();
    if (measured_speed_centi_rad_s < 0)
    {
        measured_speed_centi_rad_s = -measured_speed_centi_rad_s;
    }
    measured_speed = (float)measured_speed_centi_rad_s / 100.0f;
    Motor_ActualSpeed += MOTOR_SPEED_FILTER_ALPHA *
                         (measured_speed - Motor_ActualSpeed);

    if (Motor_TargetSpeed <= 0.0f)
    {
        last_error = 0.0f;
        pwm_command = 0.0f;
        Motor_SetDuty(0U);
        return;
    }

    error = Motor_TargetSpeed - Motor_ActualSpeed;

    /* Incremental PI: Kp acts on delta error and Ki on current error. */
    pwm_command += MOTOR_SPEED_KP * (error - last_error) +
                   MOTOR_SPEED_KI * error;
    last_error = error;

    if (pwm_command > (float)MOTOR_DUTY_MAX)
    {
        pwm_command = (float)MOTOR_DUTY_MAX;
    }
    else if (pwm_command < 0.0f)
    {
        pwm_command = 0.0f;
    }

    Motor_SetDuty((uint16_t)(pwm_command + 0.5f));
}
