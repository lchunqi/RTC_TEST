#include "communicate.h"
#include "cmsis_os.h"
#include "control.h"
#include "lcd_driver.h"
#include "sd2506.h"
#include "stdio.h"
#include "flash.h"
#include "string.h"
#include "soft_uart.h"
#include "devices.h"

KeyState g_keys[SYSTEM_KEY_NUMBER] = {0};
#define FUNCTION_NAME_LENGTH    20
#define MAX_LIST_NAMEBER        9
char funtionList[MAX_LIST_NAMEBER][FUNCTION_NAME_LENGTH] = {
    "[管理存储]", "[接触测试]", "[配置温度]", "[温箱测试]", "[温度补偿]", "[芯片烧写]", "[老化测试]", "[机台测试]", "[取料记录]"
};

/*
修改提示：
如果想新增/删除1个子项功能：
1）修改FUNCTION(x)_NUMBER 的数量；
2）修改selectList列表的对应位置显示；
3）修改devices.c文件里面的接收显示函数(大部分不用修改)；
4）修改烧写板对应的接收命令列表
*/
#define FUNCTION0_NUMBER        1
#define FUNCTION1_NUMBER        FUNCTION0_NUMBER + 4    //flash
#define FUNCTION2_NUMBER        FUNCTION1_NUMBER + 3    //connect
#define FUNCTION3_NUMBER        FUNCTION2_NUMBER + 2    //set temp offset
#define FUNCTION4_NUMBER        FUNCTION3_NUMBER + 3    //cap adjust
#define FUNCTION5_NUMBER        FUNCTION4_NUMBER + 2    //calculate
#define FUNCTION6_NUMBER        FUNCTION5_NUMBER + 4    //download
#define FUNCTION7_NUMBER        FUNCTION6_NUMBER + 4
#define FUNCTION8_NUMBER        FUNCTION7_NUMBER + 4
#define FUNCTION9_NUMBER        FUNCTION8_NUMBER + 1
#define MAX_SELECT_NUMBER     FUNCTION9_NUMBER
char selectList[MAX_SELECT_NUMBER][FUNCTION_NAME_LENGTH] = {
    "[选择功能]",
    "[读取内存]", "[清除内存]", "[查询版本]", "[查询编号]",
    "[IIC接触测试]", "[温度读取]", "[频率计连通测试]",
    "[配置温度]", "[查看配置结果]", 
    "[温度测试(累加)]", "[温度测试(重复)]", "[读取测试结果]",
    "[计算温补值]", "[查看计算结果]",
    "[配置批号]", "[查询批次号]", "[烧写数据]", "[查看烧写结果]",
    "[低温同步时间]", "[低温误差验证]", "[高温同步时间]", "[高温误差验证]",
    "[通用测试]", "[检烧模式/累加]", "[检烧模式/重复]", "[查看测试结果]",
    "[查看取料信息]"
};

extern RealTime g_systemTime;
extern char g_ipName[20];
extern uint8_t g_wifiState;
extern uint8_t g_mesState;

extern float g_12Voltage;
extern float g_6Voltage;
uint8_t g_screenId = 0;

// extern uint8_t g_rxFlag[MAX_DEVICES_CHANNEL];
extern uint32_t g_rxTick[MAX_DEVICES_CHANNEL];
ExcelMessage g_deviceMessage[MAX_DEVICES_CHANNEL] = {0}; 
static uint8_t g_operateRunFlag = 0;
uint16_t g_inputNumber = 2401;

void check_keys_state(void)
{
    int i;
    uint8_t lastState;
    for (i = 0; i < SYSTEM_KEY_NUMBER; i++) {
        lastState = g_keys[i].state;
        g_keys[i].state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9 >> i);
        if (!g_keys[i].state) {
            if (!lastState) {
                g_keys[i].time += COMMUNICATE_TASK_DELAY;
            } else {
                g_keys[i].falling = 1;
            } 
        } else {
            g_keys[i].time = 0;
            g_keys[i].falling = 0;
        }
    }
}

uint8_t judge_key_effect(uint8_t key, uint16_t time)
{
    if (key > KEY_ENTER) {return 0;}
    if (time <= KEY_SELECT_SHORT) {
        if (g_keys[key].falling && g_keys[key].time >= time) {
            g_keys[key].falling = 0;
            return 1;
        }
    } else {
        if (g_keys[key].time >= time) {
            g_keys[key].falling = 0;
            return 1;
        }
    }
    return 0;
}


void show_screen_real_time(uint8_t screen, uint8_t displayId)
{
    char time[26] = {0};
    static uint8_t sec = 0;
    if (sec != g_systemTime.sec) {
        sec = g_systemTime.sec;
        snprintf(time, 26, "20%02d/%02d/%02d %02d:%02d:%02d", g_systemTime.year, g_systemTime.mon, g_systemTime.day, g_systemTime.hour, g_systemTime.min, g_systemTime.sec);
        SetTextValue(screen, displayId, time);
    }
}

void show_operate_run_time(uint8_t screen, uint8_t displayId)
{
    static uint16_t time = 0;
    static uint8_t sec = 0;
    char buffer[20] = {0};
    if (g_operateRunFlag == 1) {
        g_operateRunFlag = 2;
        time = 0;
    } 
    if (g_operateRunFlag == 2) {
        if (sec != g_systemTime.sec) {
            sec = g_systemTime.sec;
            time++;
            snprintf(buffer, sizeof(buffer), "运行：%02d:%02d", time / 60, time % 60);
            SetTextValue(screen, displayId, buffer);
        }
    }
}

void show_borad_message(void)
{
    char buffer[100] = {0};
    char s1[15] = {0}, s2[15] = {0};
	char BSN[BSN_NAME_LENGTH] = {0};
    static uint8_t sec = 0;
    if (sec != g_systemTime.sec) {
        sec = g_systemTime.sec;
        if (g_wifiState == 4) {
            strncpy(s1, "已开启透传", 15);
        } else {
            strncpy(s1, "未就绪", 15);
        }
        if (g_mesState > 0) {
            strncpy(s2, "mes正常握手\n", 15);
        } else {
            strncpy(s2, "mes握手错误\n", 15);
        }
        GetStringFromFlash(BSN, BSN_NAME_LENGTH - 1, FLASH_BSN_ADDR);
        snprintf(buffer, 100, "wifi%s\nBSN: %s\nip地址:\n%s\n%s", s1, BSN, g_ipName, s2);
        SetTextValue(SCREEN_FUNCTION_SELECT, WIFI_TEXT_ID, buffer);

        memset(buffer, 0, 100);
        snprintf(buffer, 100, "电源电压\n12V: %.2f\n 6.5V: %.2f\n", g_12Voltage, g_6Voltage);
        SetTextValue(SCREEN_FUNCTION_SELECT, VOLTAGE_TEXT_ID, buffer);
    }
}
void clear_device_message(void)
{
    int i;
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        g_deviceMessage[i].board = i;
        // g_deviceMessage[i].systemTemp = 0;
        g_deviceMessage[i].okNumber = 0;
        g_deviceMessage[i].chipName = 0;
        g_deviceMessage[i].chipTemp = 0;
        // memset(g_deviceMessage[i].state, 0, sizeof(g_deviceMessage[i].state));
        memset(g_deviceMessage[i].message, 0, sizeof(g_deviceMessage[i].message));
    }
}
void show_device_message(void)
{
    int i;
    static uint32_t lastTick[MAX_DEVICES_CHANNEL] = {0};
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (g_rxTick[i] > lastTick[i]) {
            lastTick[i] = g_rxTick[i];
            excel_change_one_line(EXCEL_OF_SCREEN, EXCEL_TEXT_ID, g_deviceMessage[i]);
        }
    }
}

//show_function_message函数，显示每个大项功能下的子项功能
void show_function_message(uint8_t listNumber, uint8_t* functionNumber)
{
    uint8_t number[MAX_LIST_NAMEBER + 1] = {
        FUNCTION0_NUMBER, FUNCTION1_NUMBER, FUNCTION2_NUMBER, FUNCTION3_NUMBER, FUNCTION4_NUMBER,
        FUNCTION5_NUMBER, FUNCTION6_NUMBER, FUNCTION7_NUMBER, FUNCTION8_NUMBER, FUNCTION9_NUMBER};
    (*functionNumber)++;
    if (*functionNumber > number[listNumber] - 1) {
        *functionNumber = 0;
    } else if (*functionNumber < number[listNumber - 1]) {
        *functionNumber = number[listNumber - 1];
    }
    SetTextValue(SCREEN_MASSAGE_SHOW, FUNCTION_SELECT_ID, selectList[*functionNumber]);
}

void show_chip_id(void)
{
    char showId[30] = {0};
    snprintf(showId, 29, "%02d %02d %02d XX %02d %02d XX XX", g_systemTime.year, g_systemTime.mon, g_systemTime.day, g_inputNumber / 100,  g_inputNumber % 100);
    SetTextValue(SCREEN_INPUT_NUMBER, 9, showId);
}

void lcd_manage(void)
{
    static uint8_t listNumber = 0;
    static uint8_t selectNumber = 1;
    static uint8_t functionNumber = 0;//选项选择控制
    // static int8_t selectVNumber = 0;//电压选项选择控制

    if (judge_key_effect(KEY_ENTER, KEY_SELECT_LONG)) {
        g_screenId = SCREEN_FUNCTION_SELECT;
        SetScreen(g_screenId);
        // listNumber = 1;
        clear_function_select();
        function_one_select(listNumber);
        clear_device_message();
        g_operateRunFlag = 0;
    }
    switch (g_screenId) {
        case DEFAULT_START:
            if (judge_key_effect(KEY_ENTER, KEY_SELECT_SHORT)) {
                g_screenId = SCREEN_FUNCTION_SELECT;
                SetScreen(g_screenId);
                listNumber = 1;
                clear_function_select();
                function_one_select(listNumber);
            }
            show_screen_real_time(g_screenId, 1);
        break;
        case SCREEN_FUNCTION_SELECT:
            show_screen_real_time(g_screenId, 11);
            show_borad_message();
            if (judge_key_effect(KEY_UP, KEY_SELECT_SHORT)) {
                listNumber--;
                if (listNumber < 1) {listNumber = MAX_LIST_NAMEBER;}
                function_one_select(listNumber);
            } else if (judge_key_effect(KEY_DOWN, KEY_SELECT_SHORT)) {
                listNumber++;
                if (listNumber > MAX_LIST_NAMEBER) {listNumber = 1;}
                function_one_select(listNumber);
            }

            if (judge_key_effect(KEY_ENTER, KEY_SELECT_SHORT)) {
                if (listNumber == 3 || listNumber == 4) {//配置温度计和电容测试可以进入选择电压界面
                    g_screenId = SCREEN_VOLTAGE_SELECT;
                    SetScreen(g_screenId);
                    function_one_select1(selectNumber);
                } else {
                    g_screenId = SCREEN_MASSAGE_SHOW;
                    SetTextValue(g_screenId, 1, funtionList[listNumber - 1]);
                }
                SetScreen(g_screenId);
                functionNumber = 0;
                SetTextValue(SCREEN_MASSAGE_SHOW, FUNCTION_SELECT_ID, selectList[functionNumber]);
                lcd_init_message_excel();
            }
        break;
        case SCREEN_VOLTAGE_SELECT:
            if (judge_key_effect(KEY_UP, KEY_SELECT_SHORT)) {
                selectNumber--;
                if (selectNumber < 1) {selectNumber = 2;}
                function_one_select1(selectNumber);
            } else if (judge_key_effect(KEY_DOWN, KEY_SELECT_SHORT)) {
                selectNumber++;
                if (selectNumber > 2) {selectNumber = 1;}
                function_one_select1(selectNumber);
            }

            if (judge_key_effect(KEY_ENTER, KEY_SELECT_SHORT)) {//enter键,发出消息
                g_screenId = SCREEN_MASSAGE_SHOW;
                selete_vddmode_devices(selectNumber);//发送电压选择命令
                SetScreen(g_screenId);
                SetTextValue(g_screenId, 1, funtionList[listNumber - 1]);
            }
        break;
        case SCREEN_MASSAGE_SHOW:
            show_screen_real_time(g_screenId, 4);
            if (judge_key_effect(KEY_SELECT, KEY_SELECT_SHORT)) {//切换功能
                show_function_message(listNumber, &functionNumber);
                g_operateRunFlag = 0;
            }
            if (judge_key_effect(KEY_ENTER, KEY_SELECT_SHORT)) {//enter键,发出消息
                if (functionNumber == OPERATE_SET_ID) {
                    show_chip_id();
                    g_screenId = SCREEN_INPUT_NUMBER;
                    SetScreen(g_screenId);
                    break;
                }
                if (functionNumber != 0) {
                    operate_to_devices(functionNumber);
                    g_operateRunFlag = 1;
                }
            }
            show_device_message();  //有新消息时才更新
            if (g_operateRunFlag > 0) {
                show_operate_run_time(SCREEN_MASSAGE_SHOW, 5);
            }
        break;
        default:break;
    }
}

void StartCommunicateTask(void const * argument)
{
    control_usart_init();
    while (1) {
        osDelay(COMMUNICATE_TASK_DELAY);
        check_keys_state();
        lcd_manage();
        CheckControlList();//LCD 接收串口信息
    }
}

