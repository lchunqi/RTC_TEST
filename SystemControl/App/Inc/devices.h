#ifndef __DEVICES_H
#define __DEVICES_H

#ifdef __cplusplus
 extern "C" {
#endif
    // "[ѡ����]",
    // "[��ȡ�ڴ�]", "[����ڴ�]", "[��ѯ�汾]", "[��ѯ���]",
    // "[IIC�Ӵ�����]", "[�¶ȶ�ȡ]", "[Ƶ�ʼ���ͨ����]",
    // "[�����¶�]", "[�鿴���ý��]", 
    // "[�������(�ۼ�)]", "[�������(�ظ�)]", "[��ȡ���Խ��]",
    // "[�����²�ֵ]", "[�鿴������]",
    // "[��������]", "[��д����]", "[�鿴��д���]",
    // "[����ͬ��ʱ��]", "[���������֤]", "[����ͬ��ʱ��]", "[���������֤]",
    // "[ͨ�ò���]", "[����ģʽ/�ۼ�]", "[����ģʽ/�ظ�]", "[�鿴���Խ��]",
    // "[�鿴ȡ����Ϣ]"
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