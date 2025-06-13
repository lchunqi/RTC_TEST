#include "main.h"
#include "current.h"

#define VOLTAGE_REF			2500//mV
#define MEASRUE_VGA			128
#define SAMPLING_RESISTOR	1000//


static void adc_clk_h(void)
{
  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
}

static void adc_clk_l(void)
{
  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
}

static void adc_dout_h(void)
{
  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
}

static void adc_dout_l(void)
{
  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
}


/**
 * @brief Read sda pin state
 * 
 */
static unsigned char adc_read_din(void)
{
	/*user code*/
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_SET) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief When read data, need to set sda as in
 * 
 */
static inline void adc_sda_in(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
 * @brief When send data, need to set sda as out
 * 
 */
static inline void adc_sda_out(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
}

unsigned char CS1237Init(void)
{
	uint8_t buffer;
	buffer = 0x2C;
	CS1237WriteConfig(buffer);
	HAL_Delay(5);
	buffer = CS1237ReadConfig();
	if (buffer == 0x2C) {
		return 1;
	} else {
		return 0;
	}
}

void CS1237OneClock(void)
{
	int j = 0;
	adc_clk_h();
	for (j = 0; j < 150; j++) {}
	adc_clk_l();
	for (j = 0; j < 150; j++) {}
}

long CS1237Voltage(void)
{
	long voltage = 0;
	uint32_t time;		
	int i;
	adc_clk_l();
	adc_sda_in();	
	time = HAL_GetTick();
	while (!adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	while (adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	for (i = 0; i < 24; i++) {
		CS1237OneClock();
		voltage <<= 1;
		if (adc_read_din()) { voltage += 1; }
	}
	adc_clk_l();
	return voltage;
}

float CS1237Current(void)
{
	#define TEST_NUMBER  5
	long voltage;
	float current[TEST_NUMBER] = {0};
	int i, j;
	float result, min;
	for (i = 0; i < TEST_NUMBER; i++) {
		HAL_Delay(4);
		voltage = CS1237Voltage();
		if (voltage >= 8388608L) {
			voltage = voltage - 16777216L;
		}
		current[i] = (float)voltage * VOLTAGE_REF * SAMPLING_RESISTOR / MEASRUE_VGA / 16777216L ;
	}
	for (i = 0; i < TEST_NUMBER; i++) {
		min = current[i];
		for (j = i + 1; j < TEST_NUMBER; j++) {
			if (current[j] < min) {
				min = current[j];
				current[j] = current[i];
				current[i] = min;
			}
		}
	}
	result = 0;
	for (i = 1; i < TEST_NUMBER - 1; i++) {
		result += current[i];
	}
	return result / (TEST_NUMBER - 2);
}

unsigned char CS1237ReadConfig(void)
{
	int i = 0;
	uint32_t time;		
	unsigned char readBuffer = 0;
	unsigned char cmdWrite;
	adc_clk_l();
	adc_sda_in();
	time = HAL_GetTick();
	while (!adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	while (adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	for (i = 0; i < 27; i++) {
		CS1237OneClock();
	}
	adc_sda_out();
	adc_dout_h();
	CS1237OneClock();
	CS1237OneClock();
	cmdWrite = 0x56;
	for (i = 0; i < 7; i++) {
		if (cmdWrite & 0x40) { 
			adc_dout_h();
		} else {
			adc_dout_l();
		}
		CS1237OneClock();
		cmdWrite <<= 1;
	}
	adc_dout_h();
	adc_sda_in();
	CS1237OneClock();
	readBuffer = 0;
	for (i = 0; i < 8; i++) {
		CS1237OneClock();
		readBuffer <<= 1;
		if (adc_read_din()) { readBuffer += 1; }
	}
	adc_sda_out();
	adc_dout_h();
	CS1237OneClock();
	return readBuffer;
}

void CS1237WriteConfig(unsigned char tem)
{
	int i = 0;
	uint32_t time;
	unsigned char cmdWrite;
	adc_clk_l();
	adc_sda_in();
	time = HAL_GetTick();
	while (!adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	while (adc_read_din()) { 
		if (HAL_GetTick() > time + 100) { break;}
	}
	for (i = 0; i < 27; i++) {
		CS1237OneClock();
	}
	adc_sda_out();
	adc_dout_h();
	CS1237OneClock();
	CS1237OneClock();
	cmdWrite = 0x65;
	for (i = 0; i < 7; i++) {
		if (cmdWrite & 0x40) { 
			adc_dout_h();
		} else {
			adc_dout_l();
		}
		CS1237OneClock();
		cmdWrite <<= 1;
	}
	adc_dout_h();	
	CS1237OneClock();
	for (i = 0; i < 8; i++) {
		if (tem & 0x80) { 
			adc_dout_h();
		} else {
			adc_dout_l();
		}
		CS1237OneClock();
		tem <<= 1;
	}
	CS1237OneClock();
}
