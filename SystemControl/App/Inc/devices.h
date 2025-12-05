#ifndef __DEVICES_H
#define __DEVICES_H

#ifdef __cplusplus
 extern "C" {
#endif
    // "[选择功能]",
    // "[读取内存]", "[清除内存]", "[查询版本]", "[查询编号]",
    // "[IIC接触测试]", "[温度读取]", "[频率计连通测试]",
    // "[配置温度]", "[查看配置结果]", 
    // "[单点测试(累加)]", "[单点测试(重复)]", "[读取测试结果]",
    // "[计算温补值]", "[查看计算结果]",
    // "[配置批号]", "[烧写补偿]", "[查看烧写结果]",
    // "[低温同步时间]", "[低温误差验证]", "[高温同步时间]", "[高温误差验证]",
    // "[通用测试]", "[检烧模式/累加]", "[检烧模式/重复]", "[查看测试结果]",
    // "[查看取料信息]"
typedef enum  {
	OPERATE_FREE_MODE = 0,
	OPERATE_READ_FLASH,
	OPERATE_ERASE_FLASH,
    OPERATE_READ_VERSION,
    OPERATE_READ_BSN,
	OPERATE_IIC_TEST,
	OPERATE_READ_TEMP,
	OPERATE_CONNECT,
	OPERATE_SET_CONFIG9,
	OPERATE_SHOW_CONFIG9,
	OPERATE_CAPADJUST_ADD,
	OPERATE_CAPADJUST_COVER,
	OPERATE_CAPADJUST_RESULT,
	OPERATE_CALCULATE,
	OPERATE_CALCUL_RESULT,
    OPERATE_SET_ID,
    OPERATE_READ_CHIPID,
	OPERATE_DOWNLOAD,
	OPERATE_LOAD_RESULT,
	OPERATE_COLD_SYNC,
	OPERATE_COLD_ERROR,
	OPERATE_HOT_SYNC,
	OPERATE_HOT_ERROR,
	OPERATE_MACHINE_TEST,
	OPERATE_MACHINE_WRITE,
	OPERATE_MACHINE_COVER,//and write
	OPERATE_MACHINE_RESULT,
	OPERATE_TAKE_OUT_MESSAGE,
} OperoteCommand;

void StartDevicesTask(void const * argument);
void operate_to_devices(uint8_t command);
void selete_vddmode_devices(uint8_t command);
void set_chipid_devices(uint16_t command);

#ifdef __cplusplus
}
#endif

#endif 