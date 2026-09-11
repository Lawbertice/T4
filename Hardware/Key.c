#include "main.h"
#include "Key.h"

uint8_t Key_GetNum(void)
{
  static GPIO_PinState sampled_state = GPIO_PIN_SET;
  static GPIO_PinState stable_state = GPIO_PIN_SET;
  static uint32_t last_change_tick = 0U;
  GPIO_PinState current_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
  uint32_t now = HAL_GetTick();

  if (current_state != sampled_state)
  {
    sampled_state = current_state;
    last_change_tick = now;
  }

  if ((sampled_state != stable_state) &&
      ((uint32_t)(now - last_change_tick) >= 20U))
  {
    stable_state = sampled_state;
    if (stable_state == GPIO_PIN_RESET)
    {
      return 1U;
    }
  }

  return 0U;
}
