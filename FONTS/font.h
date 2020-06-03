#ifndef __FONT_CODE_H_
#define __FONT_CODE_H_

#include "sys.h"
#include "stmflash.h"

//#define FLASH_FONT

#define FLASH_ADDR_GB2312_16x16     (ADDR_FLASH_SECTOR_6)
#define FLASH_ADDR_ASCII_5x8        (ADDR_FLASH_SECTOR_5)
#define FLASH_ADDR_ASCII_8x16       (FLASH_ADDR_ASCII_5x8+0x400)
#define FLASH_ADDR_ASCII_7x10       (FLASH_ADDR_ASCII_8x16+0x700)
#define FLASH_ADDR_ASCII_10x12      (FLASH_ADDR_ASCII_7x10+0x400)
#define FLASH_ADDR_ASCII_14x16      (FLASH_ADDR_ASCII_10x12+0xA00)
#define FLASH_ADDR_ASCII_17x20      (FLASH_ADDR_ASCII_14x16+0xD00)

#define FLASH_ADDR_BMP_UNLOCK_SUCC		(FLASH_ADDR_ASCII_17x20+0x1800)
#define FLASH_ADDR_BMP_UNLOCK_ERROR		(FLASH_ADDR_BMP_UNLOCK_SUCC+32)
#define FLASH_ADDR_BMP_UNLOCK_WARN		(FLASH_ADDR_BMP_UNLOCK_ERROR+32)
#define FLASH_ADDR_BMP_MODE_OPTIONAL	(FLASH_ADDR_BMP_UNLOCK_WARN+32)
#define FLASH_ADDR_BMP_MENU_BACK		(FLASH_ADDR_BMP_MODE_OPTIONAL+8)
#define FLASH_ADDR_BMP_MAIN_WIFI0		(FLASH_ADDR_BMP_MENU_BACK+32)
#define FLASH_ADDR_BMP_MAIN_WIFI1		(FLASH_ADDR_BMP_MAIN_WIFI0+32)
#define FLASH_ADDR_BMP_MAIN_WIFI2		(FLASH_ADDR_BMP_MAIN_WIFI1+32)
#define FLASH_ADDR_BMP_MAIN_WIFI3		(FLASH_ADDR_BMP_MAIN_WIFI2+32)


#define ASCII_EN_MIN ' '//32  -- 0
#define ASCII_EN_MAX '~'//126 -- 94

typedef enum 
{
    ASCII_5X8,
    ASCII_8X16,
    ASCII_7X10,
    ASCII_10X12,
    ASCII_14X16,
    ASCII_17X20,
    ASCII_MAX,
} ascii_type_e;


typedef enum 
{
    BMP_UNLOCK_SUCC_16X14,
    BMP_UNLOCK_ERROR_16X14,
    BMP_UNLOCK_WARN_16X14,
	BMP_MODE_OPTIONAL_7X7,
	BMP_MENU_BACK_12X12,
    BMP_MAIN_WIFI0_13X13,
    BMP_MAIN_WIFI1_13X13,
    BMP_MAIN_WIFI2_13X13,
    BMP_MAIN_WIFI3_13X13,
    BMP_MAX,
} bmp_type_e;


typedef struct
{
    int32_t  name;
    int32_t  width;
    int32_t  height;
    uint8_t  cmin;
    uint8_t  cmax;
    uint32_t addr;
} font_t;

typedef struct
{
    int32_t  name;
    int32_t  width;
    int32_t  height;
    uint32_t addr;
} bmp_t;


extern font_t* font_get(uint8_t fidx);
extern bmp_t* bmp_get(uint8_t bidx);

#endif
