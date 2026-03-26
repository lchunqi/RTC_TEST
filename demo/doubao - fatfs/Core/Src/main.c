/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../FatFs/Inc/diskio.h"
#include "../FatFs/Inc/ff.h"
#include "../FatFs/Inc/ffconf.h"
#include "flash.h"
#include "command.h"
#include "multi_button.h"
#include "storage.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
  uint8_t send_test[] = "USART1 DMA Idle Test\r\n";
  uint8_t recv_buff[USART1_DMA_RX_BUFF_SIZE] = {0};  // 用户数据缓冲区


 // FATFS fs;        // FatFs文件系统对象
 // FRESULT res;     // 操作结果


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  HAL_Delay(50);  // 硬件稳定延时
  /* USER CODE BEGIN 2 */
  uint32_t systemTick = 0;
  //Storage_Init();//存储系统初始化
  Button_Manage();//按键函数

  /*//spi测试
  //UsbPrintf("SPI TEST\n");   
  uint32_t FLASH_ID=0;
  FLASH_ID=SPI1_ReadID();//EF 40 17
  UsbPrintf("%x\n",FLASH_ID);
  //SPI2_Send(); //25 07 29 30 24 26 82 96
  */
 
  ///启动USART1 DMA接收
  if (USART1_DMA_StartRx() != HAL_OK) {
    Error_Handler();
  }
  //uint8_t recv_buff[USART1_DMA_RX_BUFF_SIZE] = {0};  // 用户数据缓冲区
 
 
  //flash_device_id();
 
  UsbPrintf("----- Data:%s Time:%s -----\r\n",__DATE__, __TIME__);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

/* //flash测试
  // 1. 读取Flash ID
  uint16_t flash_id = flash_device_id();
  UsbPrintf("Flash ID: 0x%04X\r\n", flash_id); // 

  // 2. 测试扇区擦除+读写
  uint8_t test_buf[4096] = {0x55};
  int erase_res = flash_erase_sector(0); // 擦除0号扇区
  UsbPrintf("Erase Sector 0: %d\r\n", erase_res); // 必须返回1（成功）
  int write_res = flash_write_data(0, test_buf, 4096); // 写入0号扇区
  UsbPrintf("Write Sector 0: %d\r\n", write_res); // 必须返回1（成功）
  memset(test_buf, 0, 4096);
  int read_res = flash_read_data(0, test_buf, 4096); // 读取0号扇区
  UsbPrintf("Read Sector 0: %d, Data[0]: 0x%02X\r\n", read_res, test_buf[0]); // 必须返回1，Data[0]=0x55
*/

 /*//diskio测试
  // 1. 检查设备状态
   
  DSTATUS stat = disk_status(0);
  UsbPrintf("Disk Status: %d\r\n", stat); // 必须返回0（STA_OK）

  // 2. 检查扇区参数
  WORD sector_size;
  DWORD sector_count, block_size;
  disk_ioctl(0, GET_SECTOR_SIZE, &sector_size);
  disk_ioctl(0, GET_SECTOR_COUNT, &sector_count);
  disk_ioctl(0, GET_BLOCK_SIZE, &block_size);
  UsbPrintf("Sector Size: %d, Total Sector: %d, Block Size: %d\r\n", sector_size, sector_count, block_size);
  // 必须打印：4096, 2048, 16
  */


  // StorageError initStatus = Storage_Init();
  // if (initStatus == STORAGE_OK) {
  //     UsbPrintf("Storage system initialized successfully.\n");
  //     //Storage_TestWriteRead();
  // } else {
  //     UsbPrintf("Failed to initialize storage system.\n");
  // }

  W25Q64_Init();

  // 先执行底层读写测试，验证硬件
  Flash_Print("\n===== 开始W25Q64底层硬件测试 =====\n");
  uint8_t test_result = W25Q64_ReadWriteTest(0x00001000);
  Flash_Print("底层测试结果码: %d → ", test_result);
  switch(test_result) {
      case 0: Flash_Print("硬件正常！\n"); break;
      case 1: Flash_Print("硬件写保护未关闭（WP引脚问题）\n"); break;
      case 2: Flash_Print("扇区擦除失败\n"); break;
      case 3: Flash_Print("数据写入失败\n"); break;
      case 4: Flash_Print("数据读取失败\n"); break;
      case 5: Flash_Print("数据校验失败\n"); break;
      case 0xFF: Flash_Print("SPI通信失败\n"); break;
      default: Flash_Print("未知错误\n"); break;
  }
  Flash_Print("===== 底层硬件测试结束 =====\n");

  // 底层测试通过后，再初始化FatFs
  if (test_result == 0) {
      Flash_Print("\n===== 底层正常，开始挂载FatFs =====\n");
      //AT_FATFS_MOUNT_Handle(NULL);
  }



  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    //HAL_Delay(2000);
  
    /*// 3. 检查是否有数据接收完成 DMA接收完成标志
    if (g_usart1_rx_done == 1) 
    {
      //memcpy(recv_buff, g_usart1_dma_rx_buff, g_usart1_rx_len);
      ///UsbPrintf("%s ", recv_buff);

      ///USART1_DMA_SendData(recv_buff, g_usart1_rx_len);
      HAL_UART_Transmit_DMA(&huart1, g_usart1_dma_rx_buff, g_usart1_rx_len);
      //memset(recv_buff, 0, sizeof(recv_buff));

      g_usart1_rx_done = 0;
      g_usart1_rx_len = 0;
      //memset(recv_buff, 0, g_usart1_rx_len);
      //
    }
    */
    CheckCommandList(); 
    
    if (systemTick != HAL_GetTick()) 
    {
      if (systemTick % 5 == 0)
      {
        button_ticks();
      }
      if (systemTick % 1000 == 0)
      {
        /*SPI2 测试
        //uint8_t i=0;
        // CS2_HIGH;
        // uint8_t send=0xB8;
        // uint8_t rece[7]={0x08};
        // if(HAL_SPI_Transmit(&hspi2, &send,  1, 1000) != HAL_OK)
        // { 
        //   UsbPrintf("send2 error ");
        // }
    
        // if(HAL_SPI_Receive(&hspi2, rece,  7, 1000)!= HAL_OK)
        // {
        //   UsbPrintf("receive2 error ");
        // }
        // for(i = 0; i < 7; i++) {
        //   UsbPrintf("%02x ",rece[i]);   
        // }
        // CS2_LOW;
        */
    


      }
      systemTick = HAL_GetTick();
    }
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#define UART_TRANSMIT_BUFF_NUMBER   256
uint8_t g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
void UsbPrintf(char* fmt,...)  
{  
	va_list ap;
	memset(g_UsbTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);
	vsnprintf((char*)g_UsbTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);
	va_end(ap);
	HAL_UART_Transmit(&huart1, g_UsbTxBuff, strlen((char*)g_UsbTxBuff), 1000);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: UsbPrintf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
