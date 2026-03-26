#include "command.h"
#include "usart.h"
#include "flash.h"
#include "storage.h"
#include "fatfs.h"
#include "rtc.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern FATFS   USERFatFS;
extern FIL     USERFile;
extern FRESULT retUSER;
extern FatFs_Time_t g_fatfs_time;

#define MAX_FILENAME_LEN     31
#define MAX_WRITE_DATA_LEN   511
#define INVALID_CHARS        "/\\:*?\"<>|"

// USB USART1 接收
uint16_t usb_Rx_Cnt = 0;
uint8_t  usbRxBuf = 0;
uint8_t  g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t  g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t  g_UsbATStatus = 0;

char Command_buf[200] = {0};

#define MAX_COMMAND_NUMBER    40
#define MAX_COMMAND_LENGTH    20

static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
    "AT^HELP",
    "AT^TEST",
    "AT^W25Q64_EraseChip",
    "AT^FATFS_ListDir",
    "AT^FATFS_DelFile",
    "AT^FATFS_MOUNT",
    "AT^FATFS_FORMAT",
    "AT^FATFS_WRITE",
    "AT^FATFS_READ",
    "AT^FATFS_CluWrite",
    "AT^FATFS_GetTime",
};


int  strSearch(const char* source, const char* target)
{
    int len_source = strlen(source);
    int len_target = strlen(target);
    int i, j;

    if (len_source < len_target || len_target == 0)
        return -1;

    for (i = 0; i <= len_source - len_target; i++)
    {
        for (j = 0; j < len_target; j++)
        {
            if (source[i + j] != target[j])
                break;
        }
        if (j == len_target)
            return i;
    }
    return -1;
}

uint16_t CharToInt(char* source, uint8_t start, int* number)
{
    uint16_t length = strlen(source);
    uint16_t i;
    int sum = 0;
    int state = 1;
    uint8_t hex = 0;

    *number = 0;
    if (start >= length) return 0;

    if (source[start] == '-')
    {
        state = -1;
        start++;
    }

    if (start + 1 < length)
    {
        if (source[start] == '0' && (source[start + 1] == 'x' || source[start + 1] == 'X'))
        {
            start += 2;
            hex = 1;
        }
    }

    for (i = start; i < length; i++)
    {
        if (source[i] >= '0' && source[i] <= '9')
        {
            sum = hex ? (sum * 16 + (source[i] - '0')) : (sum * 10 + (source[i] - '0'));
        }
        else if (source[i] >= 'a' && source[i] <= 'f')
        {
            if (!hex) return 0;
            sum = sum * 16 + (source[i] - 'a' + 10);
        }
        else if (source[i] >= 'A' && source[i] <= 'F')
        {
            if (!hex) return 0;
            sum = sum * 16 + (source[i] - 'A' + 10);
        }
        else if (source[i] == ' ' || source[i] == ',')
        {
            break;
        }
        else
        {
            return 0;
        }
    }

    *number = state * sum;
    return 1;
}

uint8_t CharToFloat(char* source, uint8_t start, float* number)
{
    uint8_t length = strlen(source);
    uint8_t i;
    float sum = 0;
    int state = 1;
    float small_num = 0.1f;

    *number = 0.0f;
    if (length <= start) return 0;

    if (source[start] == '-')
    {
        state = -1;
        start++;
    }

    for (i = start; i < length; i++)
    {
        if (source[i] >= '0' && source[i] <= '9')
        {
            sum = sum * 10 + (source[i] - '0');
        }
        else if (source[i] == '.')
        {
            i++;
            break;
        }
        else if (source[i] == ' ' || source[i] == ',')
        {
            break;
        }
        else
        {
            return 0;
        }
    }

    for (; i < length; i++)
    {
        if (source[i] >= '0' && source[i] <= '9')
        {
            sum += small_num * (source[i] - '0');
            small_num *= 0.1f;
        }
        else if (source[i] == ' ' || source[i] == ',')
        {
            break;
        }
        else
        {
            return 0;
        }
    }

    *number = state * sum;
    return 1;
}

uint8_t Data_copy_from_pos(char *dest, const char *src, uint16_t start_pos, uint16_t copy_len)
{
    if (dest == NULL || src == NULL)
    {
        UsbPrintf("[command] error: pointer null !\r\n");
        return 0;
    }

    int src_len = strlen(src);
    if (start_pos < 0 || start_pos >= src_len)
    {
        UsbPrintf("[command] error: start pos invalid !\r\n");
        return 0;
    }

    if (copy_len < 0 || (start_pos + copy_len) > src_len)
    {
        UsbPrintf("[command] error: copy len invalid !\r\n");
        return 0;
    }

    memcpy(dest, src + start_pos, copy_len);
    dest[copy_len] = '\0';
    return 1;
}

uint8_t isFilenameValid(const char* filename)
{
    if (filename == NULL || strlen(filename) == 0)
        return 0;

    for (int i = 0; filename[i] != '\0'; i++)
    {
        if (strchr(INVALID_CHARS, filename[i]))
            return 0;
    }
    return 1;
}

void AT_HELP_Handle(char* command)
{
    (void)command;
    int i = 0;
    int num = 0;

    UsbPrintf("\r\n===== AT Command List =====\r\n");
    for (i = 0; i < MAX_COMMAND_NUMBER; i++)
    {
        if (strlen(CommandList[i]) > 0)
        {
            UsbPrintf("%d. %s\r\n", num++, CommandList[i]);
        }
    }
    UsbPrintf("===========================\r\n");
    UsbPrintf("Total command number: %d\r\n", num);
}

void AT_TEST_Handle(char* command)
{
    (void)command;
}

void AT_W25Q64_EraseChip_Handle(char* command)
{
    (void)command;
    W25Q64_ChipErase();
}

void AT_FATFS_ListDir_Handle(char* command)
{
    (void)command;
    uint8_t hh, mm, ss;
    RTC_GetCurrentTime(&hh, &mm, &ss);
    UsbPrintf("\r\n--- 时间：%02d:%02d:%02d ---", hh, mm, ss);
    FatFs_ListDir();
}

void AT_FATFS_DelFile_Handle(char* command)
{
    // 1. 空指令检查
    if (command == NULL || strlen(command) == 0)
    {
        UsbPrintf("[command] 错误：指令为空！\r\n");
        return;
    }

    // 2. 定义指令头并校验格式
    char cmd_header[] = "AT^FATFS_DelFile=";
    uint16_t header_len = strlen(cmd_header);
    
    if (strSearch(command, cmd_header) != 0)
    {
        UsbPrintf("[command] 错误：无效格式！\r\n");
        UsbPrintf("[command] 用法：AT^FATFS_DelFile=<filename>\r\n");
        return;
    }

    // 3. 提取文件名参数
    uint16_t filename_start = header_len;
    uint16_t cmd_len = strlen(command);
    
    if (filename_start >= cmd_len)
    {
        UsbPrintf("[command] 错误：参数为空！\r\n");
        return;
    }

    // 4. 读取并校验文件名
    char filename[MAX_FILENAME_LEN + 1] = {0};
    if (!Data_copy_from_pos(filename, command, filename_start, cmd_len - filename_start))
    {
        UsbPrintf("[command] 错误：提取文件名失败\r\n");
        return;
    }

    // 5. 校验文件名合法性
    if (!isFilenameValid(filename))
    {
        UsbPrintf("[command] 错误：文件名包含非法字符（/\\:*?\"<>|）\r\n");
        return;
    }

    // 6. 检查FATFS是否挂载
    FatFs_Info_t fs_info = {0};
    if (!FatFs_GetInfo(&fs_info))
    {
        UsbPrintf("[command] 错误：FATFS 未挂载！\r\n");
        return;
    }

    // 7. 关闭文件（防止文件被占用）
    f_close(&USERFile);

    // 8. 执行删除操作
    UsbPrintf("\r\n******[command] 开始文件删除 ******\r\n");
    UsbPrintf("[command] 待删除文件名: %s\r\n", filename);

    FRESULT del_result = FatFs_DeleteFile(filename); // 调用文件删除函数
    if (del_result == FR_OK)
    {
        UsbPrintf("[command] 文件删除成功！\r\n");
        UsbPrintf("******[command] 删除完成 ******\r\n");
    }
    else
    {
        UsbPrintf("[command] 文件删除失败！err=%d \r\n", del_result);
        UsbPrintf("******[command] 删除失败 ******\r\n");
    }
}


void AT_FATFS_MOUNT_Handle(char* command)
{
    (void)command;
    FatFs_Mount();
}

void AT_FATFS_FORMAT_Handle(char* command)
{
    (void)command;
    FatFs_Format();
}

void AT_FATFS_WRITE_Handle(char* command)
{
    if (command == NULL || strlen(command) == 0)
    {
        UsbPrintf("[command] 错误：指令为空！\r\n");
        return;
    }

    char cmd_header[] = "AT^FATFS_WRITE=";
    uint16_t header_len = strlen(cmd_header);

    if (strSearch(command, cmd_header) != 0)
    {
        UsbPrintf("[command] 错误：无效格式！\r\n");
        UsbPrintf("[command] 用法：AT^FATFS_WRITE=<filename>,<data>\r\n");
        return;
    }

    uint16_t cmd_len = strlen(command);
    uint16_t data_start = header_len;

    if (data_start >= cmd_len)
    {
        UsbPrintf("[command] 错误：参数为空！\r\n");
        return;
    }

    char* param_part = command + data_start;
    if (strlen(param_part) == 0)
    {
        UsbPrintf("[command] 错误：缺少参数！\r\n");
        return;
    }

    int comma_pos = strSearch(param_part, ",");
    if (comma_pos == -1)
    {
        UsbPrintf("[command] 错误：缺少逗号分隔符！\r\n");
        return;
    }

    char filename[MAX_FILENAME_LEN + 1] = {0};
    if (comma_pos > MAX_FILENAME_LEN)
    {
        UsbPrintf("[command] 错误：文件名过长（最大 %d）\r\n", MAX_FILENAME_LEN);
        return;
    }

    if (!Data_copy_from_pos(filename, command, data_start, comma_pos))
    {
        UsbPrintf("[command] 错误：提取文件名失败\r\n");
        return;
    }

    if (!isFilenameValid(filename))
    {
        UsbPrintf("[command] 错误：文件名包含非法字符\r\n");
        return;
    }

    uint16_t data_pos = data_start + comma_pos + 1;
    uint16_t data_len = cmd_len - data_pos;

    if (data_len == 0)
    {
        UsbPrintf("[command] 错误：写入数据为空\r\n");
        return;
    }

    if (data_len > MAX_WRITE_DATA_LEN)
    {
        UsbPrintf("[command] 警告：数据过长，自动截断\r\n");
        data_len = MAX_WRITE_DATA_LEN;
    }

    char write_data[MAX_WRITE_DATA_LEN + 1] = {0};
    if (!Data_copy_from_pos(write_data, command, data_pos, data_len))
    {
        UsbPrintf("[command] 错误：提取数据失败\r\n");
        return;
    }

    FatFs_Info_t fs_info = {0};
    if (!FatFs_GetInfo(&fs_info))
    {
        UsbPrintf("[command] 错误：FATFS 未挂载！\r\n");
        return;
    }

    UsbPrintf("\r\n******[command] 开始文件写入 ******\r\n");
    UsbPrintf("[command] 文件名: %s\r\n", filename);
    UsbPrintf("[command] 数据: %s\r\n", write_data);
    UsbPrintf("[command] 长度: %d 字节\r\n", data_len);

    f_close(&USERFile);
    File_Write_Result_t write_result = {0};
    retUSER = FatFs_FileWrite(filename, write_data, (UINT)data_len, &write_result);

    if (retUSER == FR_OK)
    {
        UsbPrintf("[command] 写入成功！字节：%u\r\n", write_result.write_len);
        UsbPrintf("[command] 文件起始簇号：%lu  当前写入簇号：%lu\r\n",
                  write_result.start_cluster, write_result.curr_cluster);
        UsbPrintf("******[command] 写入完成 ******\r\n");
    }
    else
    {
        UsbPrintf("[command] 写入失败！err=%d \r\n", retUSER);
        UsbPrintf("******[command] 写入失败 ******\r\n");
    }
}

void AT_FATFS_READ_Handle(char* command)
{
    if (command == NULL || strlen(command) == 0)
    {
        UsbPrintf("[command] 错误：指令为空！\r\n");
        return;
    }

    char filename[32] = {0};
    if (!FatFs_ParseReadCmd(command, filename, sizeof(filename)))
    {
        UsbPrintf("[command] 错误：指令解析失败\r\n");
        return;
    }

    FatFs_Info_t fs_info = {0};
    if (!FatFs_GetInfo(&fs_info))
    {
        UsbPrintf("[command] 错误：FATFS 未挂载\r\n");
        return;
    }

    f_close(&USERFile);
    File_Read_Result_t read_result = {0};

    if (!FatFs_ReadFile(filename, &read_result, &fs_info))
    {
        UsbPrintf("[command] 读取文件失败，错误码：%d \r\n", read_result.res);
        return;
    }

    FatFs_PrintReadResult(filename, &fs_info, &read_result);
}

void AT_FATFS_CluWrite_Handle(char* command)
{
    // 1. 空指令检查
    if (command == NULL || strlen(command) == 0)
    {
        UsbPrintf("[command] 错误：指令为空！\r\n");
        return;
    }

    // 2. 指令头校验
    char cmd_header[] = "AT^FATFS_CluWrite=";
    uint16_t header_len = strlen(cmd_header);
    if (strSearch(command, cmd_header) != 0)
    {
        UsbPrintf("[command] 错误：无效格式！\r\n");
        UsbPrintf("[command] 用法：AT^FATFS_CluWrite=<簇号>,<文件名>,<数据>\r\n");
        UsbPrintf("[command] 示例：AT^FATFS_CluWrite=2,test.txt,hello_fatfs\r\n");
        return;
    }

    // 3. 提取参数段并检查分隔符
    char* param_part = command + header_len;
    if (strlen(param_part) == 0)
    {
        UsbPrintf("[command] 错误：参数为空！\r\n");
        return;
    }

    // 查找两个逗号分隔符
    char* comma1 = strchr(param_part, ',');
    if (comma1 == NULL)
    {
        UsbPrintf("[command] 错误：缺少第一个逗号分隔符！\r\n");
        return;
    }
    char* comma2 = strchr(comma1 + 1, ',');
    if (comma2 == NULL)
    {
        UsbPrintf("[command] 错误：缺少第二个逗号分隔符！\r\n");
        return;
    }

    // 检查参数段完整性（防止最后一段数据为空）
    if (strlen(comma2 + 1) == 0)
    {
        UsbPrintf("[command] 错误：写入数据不能为空！\r\n");
        return;
    }

    // 4. 提取并校验簇号（复用CharToInt工具函数，支持十进制/十六进制）
    int cluster_num = 0;
    uint16_t cluster_valid = CharToInt(param_part, 0, &cluster_num);
    if (!cluster_valid || cluster_num < 0)
    {
        UsbPrintf("[command] 错误：簇号格式无效！仅支持数字/十六进制（0x开头）\r\n");
        return;
    }
    DWORD cluster = (DWORD)cluster_num;

    // 5. 提取并校验文件名
    char filename[MAX_FILENAME_LEN + 1] = {0};
    uint16_t filename_start = (uint16_t)(comma1 - param_part + 1);
    uint16_t filename_len = (uint16_t)(comma2 - comma1 - 1);
    
    // 检查文件名长度
    if (filename_len == 0 || filename_len > MAX_FILENAME_LEN)
    {
        UsbPrintf("[command] 错误：文件名长度无效（最大%d字符）！\r\n", MAX_FILENAME_LEN);
        return;
    }
    
    // 拷贝并校验文件名
    if (!Data_copy_from_pos(filename, param_part, filename_start, filename_len))
    {
        UsbPrintf("[command] 错误：提取文件名失败！\r\n");
        return;
    }
    if (!isFilenameValid(filename))
    {
        UsbPrintf("[command] 错误：文件名包含非法字符（/\\:*?\"<>|）\r\n");
        return;
    }

    // 6. 提取并校验写入数据
    uint16_t data_start = (uint16_t)(comma2 - param_part + 1);
    uint16_t data_len = (uint16_t)strlen(param_part) - data_start;
    
    if (data_len == 0)
    {
        UsbPrintf("[command] 错误：写入数据为空！\r\n");
        return;
    }
    if (data_len > MAX_WRITE_DATA_LEN)
    {
        UsbPrintf("[command] 警告：数据过长，自动截断为%d字节\r\n", MAX_WRITE_DATA_LEN);
        data_len = MAX_WRITE_DATA_LEN;
    }
    
    char write_data[MAX_WRITE_DATA_LEN + 1] = {0};
    if (!Data_copy_from_pos(write_data, param_part, data_start, data_len))
    {
        UsbPrintf("[command] 错误：提取写入数据失败！\r\n");
        return;
    }

    // 7. 检查FATFS挂载状态
    FatFs_Info_t fs_info = {0};
    if (!FatFs_GetInfo(&fs_info))
    {
        UsbPrintf("[command] 错误：FATFS 未挂载！\r\n");
        return;
    }

    // 8. 簇号合法性校验（基于FATFS信息）
    if (cluster < 2 || cluster > fs_info.last_cluster)
    {
        UsbPrintf("[command] 警告：簇号%lu越界（有效范围2~%lu），自动使用簇2\r\n", 
                  cluster, fs_info.last_cluster);
        cluster = 2;
    }

    // 9. 执行簇写入操作
    UsbPrintf("\r\n******[command] 开始簇写入操作 ******\r\n");
    UsbPrintf("[command] 目标簇号：%lu\r\n", cluster);
    UsbPrintf("[command] 目标文件：%s\r\n", filename);
    UsbPrintf("[command] 写入数据：%s\r\n", write_data);
    UsbPrintf("[command] 数据长度：%d 字节\r\n", data_len);

    // 关闭文件防止占用
    f_close(&USERFile);
    
    File_Write_Result_t write_result = {0};
    FRESULT write_res = FatFs_FileWrite_Cluster(filename, write_data, (UINT)data_len, cluster, &write_result);

    // 10. 结果反馈
    if (write_res == FR_OK && write_result.write_len > 0)
    {
        UsbPrintf("[command] 簇写入成功！\r\n");
        UsbPrintf("[command] 实际写入字节：%u\r\n", write_result.write_len);
        UsbPrintf("[command] 文件起始簇号：%lu  当前写入簇号：%lu\r\n",
                  write_result.start_cluster, write_result.curr_cluster);
        UsbPrintf("******[command] 簇写入完成 ******\r\n");
    }
    else
    {
        UsbPrintf("[command] 簇写入失败！错误码：%d\r\n", write_res);
        UsbPrintf("******[command] 簇写入失败 ******\r\n");
    }
}

static uint8_t is_leap_year(uint16_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 1 : 0;
}

static uint8_t get_month_max_day(uint16_t year, uint8_t month)
{
    const uint8_t day_max_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month < 1 || month > 12) return 0;
    if (month == 2 && is_leap_year(year)) return 29;
    return day_max_table[month - 1];
}

void AT_FATFS_GetTime_Handle(char* command)
{
    if (command == NULL || strlen(command) == 0)
    {
        UsbPrintf("[command] 错误：指令为空！\r\n");
        return;
    }

    const char cmd_header[] = "AT^FATFS_GetTime=";
    const uint16_t header_len = strlen(cmd_header);

    if (strSearch(command, cmd_header) != 0)
    {
        UsbPrintf("[command] 错误：格式错误！用法：\r\n");
        UsbPrintf("  完整格式：AT^FATFS_GetTime=年,月,日,时,分,秒\r\n");
        UsbPrintf("  简化格式：AT^FATFS_GetTime=年,月,日（时分秒默认0）\r\n");
        return;
    }

    char* param_start = command + header_len;
    if (strlen(param_start) == 0)
    {
        UsbPrintf("[command] 错误：至少填写年,月,日！\r\n");
        return;
    }

    char* p = param_start;
    char* comma[5] = {NULL};
    uint8_t comma_cnt = 0;

    while ((p = strchr(p, ',')) != NULL && comma_cnt < 5)
    {
        comma[comma_cnt++] = p;
        p++;
    }

    FatFs_Time_t temp_time;
    temp_time.hour = 0;
    temp_time.minute = 0;
    temp_time.second = 0;

    if (comma_cnt == 2)
    {
        char* comma1 = comma[0];
        char* comma2 = comma[1];
        char bak1 = *comma1;
        char bak2 = *comma2;

        *comma1 = '\0';
        *comma2 = '\0';

        temp_time.year = atoi(param_start);
        temp_time.month = atoi(comma1 + 1);
        temp_time.day = atoi(comma2 + 1);

        *comma1 = bak1;
        *comma2 = bak2;
    }
    else if (comma_cnt == 5)
    {
        char bak[5] = {0};
        for (uint8_t i=0; i<5; i++)
        {
            bak[i] = *comma[i];
            *comma[i] = '\0';
        }

        temp_time.year = atoi(param_start);
        temp_time.month = atoi(comma[0] + 1);
        temp_time.day = atoi(comma[1] + 1);
        temp_time.hour = atoi(comma[2] + 1);
        temp_time.minute = atoi(comma[3] + 1);
        temp_time.second = atoi(comma[4] + 1);

        for (uint8_t i=0; i<5; i++)
        {
            *comma[i] = bak[i];
        }
    }
    else
    {
        UsbPrintf("[command] 错误：参数个数错误！\r\n");
        return;
    }

    if (temp_time.year < 1980 || temp_time.year > 2107)
    {
        UsbPrintf("[command] 错误：年份无效！范围1980~2107\r\n");
        return;
    }

    if (temp_time.month < 1 || temp_time.month > 12)
    {
        UsbPrintf("[command] 错误：月份无效！范围1~12\r\n");
        return;
    }

    uint8_t max_day = get_month_max_day(temp_time.year, temp_time.month);
    if (temp_time.day < 1 || temp_time.day > max_day)
    {
        UsbPrintf("[command] 错误：日期无效！%d年%d月最大天数为%d\r\n",
                  temp_time.year, temp_time.month, max_day);
        return;
    }

    if (comma_cnt == 5)
    {
        if (temp_time.hour > 23 || temp_time.minute > 59 || temp_time.second > 59)
        {
            UsbPrintf("[command] 错误：时间超出范围！\r\n");
            return;
        }
    }

    g_fatfs_time = temp_time;
    UsbPrintf("[command] 时间设置成功！\r\n");
    UsbPrintf("[command] 当前时间：%04d-%02d-%02d %02d:%02d:%02d\r\n",
              g_fatfs_time.year, g_fatfs_time.month, g_fatfs_time.day,
              g_fatfs_time.hour, g_fatfs_time.minute, g_fatfs_time.second);
}

typedef void(*CmdFunc)(char*);

static CmdFunc UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
    AT_TEST_Handle,
    AT_W25Q64_EraseChip_Handle,
    AT_FATFS_ListDir_Handle,
    AT_FATFS_DelFile_Handle,
    AT_FATFS_MOUNT_Handle,
    AT_FATFS_FORMAT_Handle,
    AT_FATFS_WRITE_Handle,
    AT_FATFS_READ_Handle,
    AT_FATFS_CluWrite_Handle,
    AT_FATFS_GetTime_Handle,
};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (usb_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_UsbATStatus == 0)
        {
            g_UsbRxBuff[usb_Rx_Cnt++] = usbRxBuf;
        }

        if (usb_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER || usbRxBuf == '\r' || usbRxBuf == '\n')
        {
            g_UsbATStatus = 1;
        }

        HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);
    }

    if(huart->Instance == USART2)
    {
        // 计算下一个 head 位置
        uint16_t next_head = (uart2_rx_ringbuf.head + 1) % UART2_RX_BUF_SIZE;

        // 缓冲区不满就存
        if(next_head != uart2_rx_ringbuf.tail)
        {
            uart2_rx_ringbuf.head = next_head;
        }

        // 重新开启下一次接收
        HAL_UART_Receive_IT(&huart2, &uart2_rx_ringbuf.buffer[uart2_rx_ringbuf.head], 1);
    }
}

void CheckCommandList(void)
{
    static uint16_t usbTick = 0;
    static uint16_t lastRx = 0;
    uint8_t i;

    if (g_UsbATStatus == 0 && usb_Rx_Cnt == 0)
    {
        usbTick = 0;
        lastRx = 0;
        return;
    }

    if (usb_Rx_Cnt > lastRx)
    {
        lastRx = usb_Rx_Cnt;
        usbTick = 0;
    }
    else
    {
        usbTick++;
    }

    if (usbTick >= USB_UART_TICK_CLEAR || g_UsbATStatus == 1)
    {
        for (i = 0; i < MAX_COMMAND_NUMBER; i++)
        {
            if (strlen(CommandList[i]) > 0 &&
                strSearch((char*)g_UsbRxBuff, CommandList[i]) == 0)
            {
                UsbCommand[i]((char*)g_UsbRxBuff);
                break;
            }
        }

        if (i >= MAX_COMMAND_NUMBER)
        {
            UsbPrintf("[command] 错误：未知指令！输入 AT^HELP 查看帮助\r\n");
        }

        memset(g_UsbRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        usb_Rx_Cnt = 0;
        g_UsbATStatus = 0;
        usbTick = 0;
        lastRx = 0;
    }
}

