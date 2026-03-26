/*
 * @Author: lchunqi 847597316@qq.com
 * @Date: 2026-02-26 18:57:02
 * @LastEditors: lchunqi 847597316@qq.com
 * @LastEditTime: 2026-02-26 19:10:54
 * @FilePath: \dma-doubao\Core\Inc\command.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


extern uint8_t usbRxBuf;
extern uint8_t deviceRxBuf;
void CheckCommandList(void);
void DeviceMassage(void);



#ifdef __cplusplus
}
#endif

#endif 
