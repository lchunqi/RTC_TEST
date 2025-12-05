
#include "string.h"
#include "flash.h"





uint32_t FLASH_ReadWord(uint32_t Addr) 
{
	return *(uint32_t *)Addr;
}

void BSP_EnableIrq() {
	__set_PRIMASK(0);//
}

void BSP_DisableIrq() {
	__set_PRIMASK(1);
}

void ClearUpdateStateFlash(void)
{
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	uint32_t PageError = 0;
	HAL_FLASH_Unlock();
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector        = 3;
	EraseInitStruct.NbSectors     = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	HAL_FLASH_Lock();
}

void SetUpdateStateFlash(uint32_t name)
{
	HAL_FLASH_Unlock();

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_UPDATE_STATE_ADDR, name);
	
	HAL_FLASH_Lock();
}


void SaveStringToFlash(char* s, uint32_t addr)
{
	int length = strlen(s);
	uint16_t j;
	uint32_t data = 0;
	uint32_t address = addr;
	if (length <= 0) {return;}

	HAL_FLASH_Unlock();
	for (j = 0; j < length; j += 4) {
		if (j + 4 <= length) {
			data = s[j] + ((uint32_t)s[j + 1] << 8) + ((uint32_t)s[j + 2] << 16)+ ((uint32_t)s[j + 3] << 24);
		} else {
			if (length == j + 1) { 
				data = s[j];
			} else if (length == j + 2) { 
				data =  s[j] + ((uint32_t)s[j + 1] << 8);
			} else if (length == j + 3) { 
				data =  s[j] + ((uint32_t)s[j + 1] << 8) + ((uint32_t)s[j + 2] << 16);
			} 
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
		address += 4;
	}
	HAL_FLASH_Lock();
}

void GetStringFromFlash(char* target, uint16_t length, uint32_t addr)
{
	uint16_t i;
	uint32_t value;

	for (i = 0; i < length; i++) {
		value = *(uint8_t*)(addr + i);
		if (value == 0xff || value == 0) {
			target[i] = 0;
			break;
		}
		target[i] = value;
	}
}


void ClearStateFlash(uint8_t mode)
{
	char bsn[BSN_NAME_LENGTH] = {0};
	char version[VERSION_NAME_LENGTH] = {0};
	switch (mode) {
		case CLEAR_ALL://直接清理，不保存
			ClearUpdateStateFlash();
		break;
		case CLEAR_ONLY_BSN://仅擦除BSN
			GetStringFromFlash(version, VERSION_NAME_LENGTH - 1, FLASH_VERSION_ADDR);
			ClearUpdateStateFlash();
			if (strlen(version) > 0) {
				SaveStringToFlash(version, FLASH_VERSION_ADDR);
			}
		break;
		case CLEAR_ONLY_VERSION://仅擦除版本号
			GetStringFromFlash(bsn, BSN_NAME_LENGTH - 1, FLASH_BSN_ADDR);
			ClearUpdateStateFlash();
			if (strlen(bsn) > 0) {
				SaveStringToFlash(bsn, FLASH_BSN_ADDR);
			}
		break;
		case CLEAR_ONLY_UPDATE://仅擦除升级标志
			GetStringFromFlash(version, VERSION_NAME_LENGTH - 1, FLASH_VERSION_ADDR);
			GetStringFromFlash(bsn, BSN_NAME_LENGTH - 1, FLASH_BSN_ADDR);
			ClearUpdateStateFlash();
			if (strlen(bsn) > 0) {
				SaveStringToFlash(bsn, FLASH_BSN_ADDR);
			}
			if (strlen(version) > 0) {
				SaveStringToFlash(version, FLASH_VERSION_ADDR);
			}
		break;//wifi、tcp
		default:break;
	}

}