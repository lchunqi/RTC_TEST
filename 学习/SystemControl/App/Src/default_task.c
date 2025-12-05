#include "default_task.h"
#include "main.h"
#include "cmsis_os.h"
#include "command.h"
#include "sd2506.h"
#include "voltage.h"

RealTime g_systemTime = {0};

void LedHeart(void)
{
    static uint32_t ledTick = 0;

    if (HAL_GetTick() >= ledTick + 1000) {
        ledTick = HAL_GetTick();
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
    }
}

void sd2506GetTime(void)
{
    static uint32_t rtcTick = 0;

    if (HAL_GetTick() >= rtcTick + 333) {
        rtcTick = HAL_GetTick();
        rtc_read_date(&g_systemTime);
    }

}

float g_12Voltage = 0;
float g_6Voltage = 0;
void check_system_voltage(void)
{
    int i;
    float adcValue;
    static uint32_t adcTick = 0;
    
    if (HAL_GetTick() >= adcTick + 1000) {
        adcValue = 0;
        for (i = 0; i < 5; i++) {
          adcValue += ststem_voltage(CHANNEL_12V);
        }
        g_12Voltage = (adcValue / 5) * 3.3 * 6.0 / 4096;

        adcValue = 0;
        for (i = 0; i < 5; i++) {
          adcValue += ststem_voltage(CHANNEL_6V5);
        }
        g_6Voltage = (adcValue / 5) * 3.3 * 6.0 / 4096;
    }
}

void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  usb_usart_init();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    LedHeart();
    sd2506GetTime();
    CheckCommandList();
    check_system_voltage();
  }
  /* USER CODE END 5 */
}

