#include "tool.h"

// 精准延时（72MHz）
static inline void delay_us(uint32_t us)
{
    uint32_t count = us * (SystemCoreClock / 1000000UL);
    while(count--) {__NOP();}
}

void Soft_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // TX初始化
    GPIO_InitStruct.Pin = SOFT_UART_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_UART_TX_GPIO_Port, &GPIO_InitStruct);
    
    // RX初始化
    GPIO_InitStruct.Pin = SOFT_UART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SOFT_UART_RX_GPIO_Port, &GPIO_InitStruct);
    
    // TX默认高电平
    HAL_GPIO_WritePin(SOFT_UART_TX_GPIO_Port, SOFT_UART_TX_Pin, GPIO_PIN_SET);
}

void Soft_UART_SendByte(uint8_t data)
{
    uint8_t i;
    // 起始位
    HAL_GPIO_WritePin(SOFT_UART_TX_GPIO_Port, SOFT_UART_TX_Pin, GPIO_PIN_RESET);
    delay_us(BAUD_DELAY_US);
    
    // 数据位(低位先行)
    for(i=0; i<8; i++)
    {
        HAL_GPIO_WritePin(SOFT_UART_TX_GPIO_Port, SOFT_UART_TX_Pin,
            (data&1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data >>= 1;
        delay_us(BAUD_DELAY_US);
    }
    
    // 停止位
    HAL_GPIO_WritePin(SOFT_UART_TX_GPIO_Port, SOFT_UART_TX_Pin, GPIO_PIN_SET);
    delay_us(BAUD_DELAY_US);
}

// 带长度限制的字符串发送
void Soft_UART_SendStr(uint8_t *str)
{
    while(*str ) 
    {
        Soft_UART_SendByte(*str++);
    }
}

uint8_t Soft_UART_ReceiveReady(void)
{
    return (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) == GPIO_PIN_RESET);
}

uint8_t Soft_UART_ReceiveByte(uint32_t timeout)
{
    uint8_t data = 0;
    uint32_t tick = 0;

    // 1. 等待起始位（下降沿）
    while (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) != GPIO_PIN_RESET)
    {
        if (++tick > timeout) return 0xFF;
    }

    // 2. 关中断，保证时序不被打断
    __disable_irq();

    // 3. 【关键修复】延时 1.5 位周期，对准数据位中心
    delay_us(BAUD_DELAY_US);
    delay_us(BAUD_DELAY_US / 2);

    // 4. 采样8位数据（低位先行）
    for (uint8_t i = 0; i < 8; i++)
    {
        if (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) == GPIO_PIN_SET)
            data |= (1 << i);
        delay_us(BAUD_DELAY_US);
    }

    // 5. 开中断
    __enable_irq();

    // 6. 等待停止位
    tick = 0;
    while (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) == GPIO_PIN_RESET)
    {
        if (++tick > timeout) return 0xFF;
    }

    return data;
}


uint8_t Soft_UART_ReceiveData(uint8_t *data)
{
    uint32_t timeout=1000;
    uint32_t tick = 0;

    // 1. 等待起始位（下降沿）
    while (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) != GPIO_PIN_RESET)
    {
        if (++tick > timeout) return 0xFF;
    }

    // 2. 关中断，保证时序不被打断
    __disable_irq();

    // 3. 【关键修复】延时 1.5 位周期，对准数据位中心
    delay_us(BAUD_DELAY_US);
    delay_us(BAUD_DELAY_US / 2);

    // 4. 采样8位数据（低位先行）
    for (uint8_t i = 0; i < 8; i++)
    {
        if (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) == GPIO_PIN_SET)
            *data |= (1 << i);
        delay_us(BAUD_DELAY_US);
    }

    // 5. 开中断
    __enable_irq();

    // 6. 等待停止位
    tick = 0;
    while (HAL_GPIO_ReadPin(SOFT_UART_RX_GPIO_Port, SOFT_UART_RX_Pin) == GPIO_PIN_RESET)
    {
        if (++tick > timeout) return 0xFF;
    }

    return 1;
}