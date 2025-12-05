
#include "voltage.h"
extern ADC_HandleTypeDef hadc1;


uint32_t ststem_voltage(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t value;

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_Delay(1);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);//time out 2ms
    value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return value;
}
