/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "command.h"

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
#define LED_PIN    GPIO_PIN_5
#define LED_PORT   GPIOB
// 全局变量：记录FatFs执行状态（供调试查看）
uint8_t fatfs_status = 0; // 0=未执行，1=驱动注册完成，2=格式化完成，3=挂载完成，255=失败
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
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);

  //RTC_SetDateTime(2026, 3, 19, 10, 6, 30);
	UsbPrintf("===== Data:%s Time:%s ! =====\r\n",__DATE__ , __TIME__);
  uint32_t systemTick = 0;//

  //W25Q64_FullTest();  //W25Q64功能测试
  //FatFs_Test();       //fatfs读写测试

  uint8_t hh, mm, ss;
  //uint8_t u2_buf[256];

  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // // 读取一整行（遇到回车/换行停止）
    //uint16_t len = UART2_ReadLine(u2_buf, sizeof(u2_buf));
    // // 如果读到数据
    // if (len > 0)
    // {
    //     // 在这里处理你从串口2发来的数据
    //     UsbPrintf("USART2 收到：%s\r\n", u2_buf);
    // }

    if (systemTick != HAL_GetTick()) 
    {
      CheckCommandList();
      FatFs_WriteUart2ByTimeStamp();
      if (systemTick % 5 == 0)
      {
        
        //button_ticks();
      }
      if (systemTick % 1000 == 0)
      {
        RTC_GetCurrentTime(&hh, &mm, &ss);
        //UsbPrintf("RTC时间：%02d:%02d:%02d\n", hh, mm, ss);
      }
      systemTick = HAL_GetTick();
    }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  // ? 开启外部 32768Hz
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void UsbPrintf(char* fmt,...)  
{  
	va_list ap;
	memset(g_UsbTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);
	vsnprintf((char*)g_UsbTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);
	va_end(ap);
	HAL_UART_Transmit(&huart1, g_UsbTxBuff, strlen((char*)g_UsbTxBuff), 1000);
}



/************************ 官方规范版 FatFs 挂载+读写测试函数 ************************/
#define TEST_FILE_NAME  "test.txt"  // 官方规范：不硬编码盘符，由驱动分配
#define TEST_WRITE_DATA "Hello FatFs! W25Q64 Flash Test OK! "  // 测试数据
#define TEST_READ_BUF_SIZE  128U    // 读缓冲区大小（官方推荐不超过256）

// 官方FatFs错误码解析（与ff.c/R0.11源码完全对齐，含中文说明）
static const char* fatfs_error_str(FRESULT res)
{
    switch(res) {
        case FR_OK:             return "FR_OK (操作成功)";
        case FR_DISK_ERR:       return "FR_DISK_ERR (磁盘I/O错误)";
        case FR_INT_ERR:        return "FR_INT_ERR (断言失败)";
        case FR_NOT_READY:      return "FR_NOT_READY (设备未就绪/未初始化)";
        case FR_NO_FILE:        return "FR_NO_FILE (文件不存在)";
        case FR_NO_PATH:        return "FR_NO_PATH (路径不存在)";
        case FR_INVALID_NAME:   return "FR_INVALID_NAME (文件名/路径无效)";
        case FR_DENIED:         return "FR_DENIED (访问被拒绝/已锁定)";
        case FR_EXIST:          return "FR_EXIST (文件已存在)";
        case FR_INVALID_OBJECT: return "FR_INVALID_OBJECT (文件对象无效)";
        case FR_WRITE_PROTECTED: return "FR_WRITE_PROTECTED (介质写保护)";
        case FR_INVALID_DRIVE: return "FR_INVALID_DRIVE (驱动器号无效/未注册)";
        case FR_NOT_ENABLED:   return "FR_NOT_ENABLED (驱动器未启用/IOCTL指令缺失)";
        case FR_NO_FILESYSTEM: return "FR_NO_FILESYSTEM (无有效文件系统)";
        case FR_MKFS_ABORTED:  return "FR_MKFS_ABORTED (格式化终止/参数错误)";
        case FR_TIMEOUT:       return "FR_TIMEOUT (通信超时)";
        case FR_LOCKED:        return "FR_LOCKED (文件被多任务锁定)";
        case FR_NOT_ENOUGH_CORE: return "FR_NOT_ENOUGH_CORE (内存不足)";
        case FR_TOO_MANY_OPEN_FILES: return "FR_TOO_MANY_OPEN_FILES (打开文件数超限)";
        case FR_INVALID_PARAMETER:return "FR_INVALID_PARAMETER (参数错误)";
        default:               return "UNKNOWN_ERROR (未知错误)";
    }
}

// 完全遵循ST官方ff_gen_drv.c + FatFs R0.11规范的测试函数
void FatFs_Test(void)
{
    FRESULT f_res;               // FatFs原生结果类型（官方规范）
    FATFS fs;                    // FatFs文件系统对象（全局/静态，避免栈溢出）
    FIL file;                    // FatFs文件对象（官方规范）
    UINT f_wr_len = 0;           // FatFs标准无符号整型（适配ff.c）
    UINT f_rd_len = 0;           // FatFs标准无符号整型（适配ff.c）
    uint8_t f_read_buf[TEST_READ_BUF_SIZE] = {0};  // 读缓冲区
    char fatfs_path[4] = {0};    // 官方规范：路径缓冲区（4字节足够存储x:/）
    fatfs_status = 0;            // 重置全局状态

    UsbPrintf("\r\n===== FatFs Mount & Read/Write Test Start (ST Official Spec) =====\r\n");
    UsbPrintf("FatFs Version: R0.11 | Flash: W25Q64 (8MB) | FS: FAT16\r\n");

    /************************ 步骤0：官方驱动注册（ff_gen_drv.c规范） ************************/
    UsbPrintf("\r\n[Step 0] Link W25Q64 Driver to FatFs ... \r\n");
    uint8_t link_res = FATFS_LinkDriver(&USER_Driver, fatfs_path);
    if(link_res != 0)
    {
        UsbPrintf("ERROR: Driver Link FAILED! Code: %d\r\n", link_res);
        fatfs_status = 255;
        goto test_exit;  // 统一退出，确保资源释放
    }
    UsbPrintf("SUCCESS: Driver Link OK! Logical Drive: %s\r\n", fatfs_path);
    fatfs_status = 1;

    /************************ 步骤1：官方挂载文件系统（f_mount规范） ************************/
    UsbPrintf("\r\n[Step 1] Mount FatFs File System ... \r\n");
    // 官方参数：&fs=文件系统对象，fatfs_path=驱动路径，1=立即挂载（不延迟）
    f_res = f_mount(&fs, fatfs_path, 1);
    UsbPrintf("Mount Result: %d -> %s\r\n", f_res, fatfs_error_str(f_res));

    // 仅当【无文件系统/驱动器无效】时执行格式化，排除硬件错误
    if (f_res == FR_NO_FILESYSTEM || f_res == FR_INVALID_DRIVE || f_res == FR_NOT_READY)
    {
        UsbPrintf("Tips: No valid FAT16, start format flash ... \r\n");
        
        /************************ 步骤2：官方格式化（f_mkfs R0.11规范） ************************/
        UsbPrintf("\r\n[Step 2] Format Flash to FAT16 (ST Official) ... \r\n");
        // 官方参数：路径=fatfs_path，类型=FS_FAT16，簇大小=0（自动适配8MB）
        f_res = f_mkfs(fatfs_path, FS_FAT12, 4096);
        UsbPrintf("Format Result: %d -> %s\r\n", f_res, fatfs_error_str(f_res));
        
        if(f_res != FR_OK)
        {
            UsbPrintf("-----错误码 %d----\r\n", fatfs_status);
            UsbPrintf("ERROR: Format FAILED! Check: user_diskio.c/CTRL_TRIM / ffconf.h/IOCTL\r\n");
            //fatfs_status = 255;
            goto test_exit;
        }
        UsbPrintf("SUCCESS: Format Flash to FAT16 OK!\r\n");
        fatfs_status = 2;

        /************************ 步骤3：格式化后重新挂载（官方规范） ************************/
        UsbPrintf("\r\n[Step 3] Remount FatFs After Format ... \r\n");
        f_mount(NULL, fatfs_path, 1); // 先卸载，再重新挂载（官方推荐）
        f_res = f_mount(&fs, fatfs_path, 1);
        UsbPrintf("Remount Result: %d -> %s\r\n", f_res, fatfs_error_str(f_res));
        
        if(f_res != FR_OK)
        {
            UsbPrintf("ERROR: Remount FAILED! FatFs Test Abort!\r\n");
            fatfs_status = 255;
            goto test_exit;
        }
    }
    UsbPrintf("SUCCESS: FatFs Mount OK! Drive: %s\r\n", fatfs_path);
    fatfs_status = 3;

    /************************ 步骤4：创建+写入文件（f_open/f_write官方规范） ************************/
    UsbPrintf("\r\n[Step 4] Create & Write File: %s%s\r\n", fatfs_path, TEST_FILE_NAME);
    // 官方模式：FA_CREATE_ALWAYS(覆盖创建)+FA_WRITE(写权限)，适配FAT16
    f_res = f_open(&file, TEST_FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE);
    if (f_res != FR_OK)
    {
        UsbPrintf("ERROR: Create File FAILED! %d -> %s\r\n", f_res, fatfs_error_str(f_res));
        goto test_exit;
    }

    // 官方f_write：必须传【实际写入长度指针】，否则会触发断言失败
    f_res = f_write(&file, TEST_WRITE_DATA, strlen(TEST_WRITE_DATA), &f_wr_len);
    if (f_res == FR_OK && f_wr_len == strlen(TEST_WRITE_DATA))
    {
        UsbPrintf("SUCCESS: Write Data OK! Length: %d Byte\r\n", f_wr_len);
        UsbPrintf("Write Content: %s\r\n", TEST_WRITE_DATA);
    }
    else
    {
        UsbPrintf("ERROR: Write Data FAILED! %d -> %s (Actual: %d Byte)\r\n", 
                  f_res, fatfs_error_str(f_res), f_wr_len);
    }
    f_close(&file); // 官方强制：写操作后必须close，确保数据刷入Flash
    memset(f_read_buf, 0, TEST_READ_BUF_SIZE);

    /************************ 步骤5：打开+读取+校验文件（官方规范） ************************/
    UsbPrintf("\r\n[Step 5] Open & Read & Check File: %s%s\r\n", fatfs_path, TEST_FILE_NAME);
    // 官方模式：FA_OPEN_EXISTING(打开已有文件)+FA_READ(读权限)
    f_res = f_open(&file, TEST_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    if (f_res != FR_OK)
    {
        UsbPrintf("ERROR: Open File FAILED! %d -> %s\r\n", f_res, fatfs_error_str(f_res));
        goto test_exit;
    }

    // 读取数据：预留1字节给\0，避免字符串越界（C语言+FatFs官方规范）
    f_res = f_read(&file, f_read_buf, TEST_READ_BUF_SIZE - 1, &f_rd_len);
    f_read_buf[f_rd_len] = '\0'; // 强制字符串终止
    if (f_res == FR_OK && f_rd_len > 0)
    {
        UsbPrintf("SUCCESS: Read Data OK! Length: %d Byte\r\n", f_rd_len);
        UsbPrintf("Read Content: %s\r\n", f_read_buf);
        
        // 数据一致性校验
        if (strcmp((char*)f_read_buf, TEST_WRITE_DATA) == 0)
        {
            UsbPrintf("SUCCESS: Data Check PASS! Read = Write\r\n");
        }
        else
        {
            UsbPrintf("ERROR: Data Check FAIL! Read != Write\r\n");
        }
    }
    else
    {
        UsbPrintf("ERROR: Read Data FAILED! %d -> %s (Actual: %d Byte)\r\n", 
                  f_res, fatfs_error_str(f_res), f_rd_len);
    }
    f_close(&file); // 官方强制：读操作后必须close，释放文件对象

    /************************ 统一退出逻辑（官方资源释放规范） ************************/
test_exit:
    // 官方强制：测试结束后必须卸载文件系统，释放卷资源（避免内存泄漏）
    f_mount(NULL, fatfs_path, 1);
    // 卸载驱动（可选，单驱动场景可省略，多驱动场景必须加）
    FATFS_UnLinkDriver(fatfs_path);
    UsbPrintf("\r\n===== FatFs Test End | Status: %d =====\r\n", fatfs_status);
    UsbPrintf("Status Note: 0=未执行,1=驱动注册OK,2=格式化OK,3=挂载+读写OK,255=失败\r\n");
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
#ifdef USE_FULL_ASSERT
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
