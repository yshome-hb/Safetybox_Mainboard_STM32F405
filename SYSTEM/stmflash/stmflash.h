#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stdbool.h"
#include "sys.h" 

#define ADDR_FLASH_BASE 				0x08000000 	//STM32 FLASH的起始地址
#define ADDR_FLASH_END 					0x080FFFFF 	//STM32 FLASH的起始地址

#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes  


uint16_t stmflash_get_sector(uint32_t addr);
bool stmflash_write(uint32_t addr, const uint8_t *pdata, uint32_t len);
bool stmflash_read(uint32_t addr, uint8_t *pdata, uint32_t len);

						   
#endif

























//********************************************************************************

//DevEBox  大越电子（嵌入式开发网）
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

//********************************************************************************


















