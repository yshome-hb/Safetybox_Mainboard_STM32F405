#include "font.h"

#ifdef FLASH_FONT

const uint8_t BMP_UNLOCK_SUCC[] __attribute__((at(FLASH_ADDR_BMP_UNLOCK_SUCC))) = {		//32
0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x0E,
0x00,0x1C,0x00,0x38,0x30,0x70,0x78,0xE0,
0x3F,0xC0,0x1F,0x80,0x0F,0x00,0x06,0x00,
0x00,0x00,0x00,0x00,
};


const uint8_t BMP_UNLOCK_ERROR[] __attribute__((at(FLASH_ADDR_BMP_UNLOCK_ERROR))) = {	//32
0x00,0x00,0x10,0x08,0x38,0x1C,0x1C,0x38,
0x0E,0x70,0x07,0xE0,0x03,0xC0,0x03,0xC0,
0x07,0xE0,0x0E,0x70,0x1C,0x38,0x38,0x1C,
0x10,0x08,0x00,0x00,
};


const uint8_t BMP_UNLOCK_WARN[] __attribute__((at(FLASH_ADDR_BMP_UNLOCK_WARN))) = {		//32
0x00,0x00,0x00,0x00,0x01,0x80,0x03,0xC0,
0x03,0xC0,0x03,0xC0,0x03,0xC0,0x01,0x80,
0x01,0x80,0x00,0x00,0x01,0x80,0x01,0x80,
0x00,0x00,0x00,0x00,
};


const uint8_t BMP_MODE_OPTIONAL[] __attribute__((at(FLASH_ADDR_BMP_MODE_OPTIONAL))) = {   //8
0xFF,0x83,0xC3,0xE3,0xF3,0xFB,0xFF,
};


const uint8_t BMP_MENU_BACK[] __attribute__((at(FLASH_ADDR_BMP_MENU_BACK))) = {     //32
0x00,0x0F,0x08,0x0F,0x1F,0xEF,0x20,0x2F,
0x40,0x2F,0x20,0x2F,0x1F,0x2F,0x09,0x2F,
0x01,0x2F,0x01,0x2F,0x01,0xEF,0x00,0x0F,
};


const uint8_t BMP_MAIN_WIFI0[] __attribute__((at(FLASH_ADDR_BMP_MAIN_WIFI0))) = {
0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,
0x10,0x27,0x18,0x67,0x0C,0xC7,0x07,0x87,
0x03,0x07,0x07,0x87,0x0C,0xC7,0xD8,0x67,
0xD0,0x27,
};


const uint8_t BMP_MAIN_WIFI1[] __attribute__((at(FLASH_ADDR_BMP_MAIN_WIFI1))) = {
0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,
0x00,0x07,0x00,0x07,0x00,0x07,0xC0,0x07,
0x70,0x07,0x18,0x07,0x08,0x07,0xCC,0x07,
0xC4,0x07,
};


const uint8_t BMP_MAIN_WIFI2[] __attribute__((at(FLASH_ADDR_BMP_MAIN_WIFI2))) = {
0x00,0x07,0x00,0x07,0x00,0x07,0xC0,0x07,
0x70,0x07,0x1C,0x07,0x06,0x07,0xC3,0x07,
0x71,0x07,0x19,0x87,0x08,0x87,0xCC,0xC7,
0xC4,0x47,
};


const uint8_t BMP_MAIN_WIFI3[] __attribute__((at(FLASH_ADDR_BMP_MAIN_WIFI3))) = {
0xF0,0x07,0x1C,0x07,0x07,0x07,0xC1,0x87,
0x70,0xC7,0x1C,0x67,0x06,0x27,0xC3,0x37,
0x71,0x17,0x19,0x9F,0x08,0x8F,0xCC,0xCF,
0xC4,0x4F,
};



#endif

bmp_t BMP_ARRAY[BMP_MAX] =
{
    { BMP_UNLOCK_SUCC_16X14, 16, 14, FLASH_ADDR_BMP_UNLOCK_SUCC },
    { BMP_UNLOCK_ERROR_16X14, 16, 14, FLASH_ADDR_BMP_UNLOCK_ERROR },
    { BMP_UNLOCK_WARN_16X14, 16, 14, FLASH_ADDR_BMP_UNLOCK_WARN },
    { BMP_MODE_OPTIONAL_7X7, 7, 7, FLASH_ADDR_BMP_MODE_OPTIONAL },
    { BMP_MENU_BACK_12X12, 12, 12, FLASH_ADDR_BMP_MENU_BACK },    
    { BMP_MAIN_WIFI0_13X13, 13, 13, FLASH_ADDR_BMP_MAIN_WIFI0 },
    { BMP_MAIN_WIFI1_13X13, 13, 13, FLASH_ADDR_BMP_MAIN_WIFI1 }, 
    { BMP_MAIN_WIFI2_13X13, 13, 13, FLASH_ADDR_BMP_MAIN_WIFI2 }, 
    { BMP_MAIN_WIFI3_13X13, 13, 13, FLASH_ADDR_BMP_MAIN_WIFI3 },   
};

bmp_t* bmp_get(uint8_t bidx)
{
    bidx = ((bidx >= BMP_MAX) ? (BMP_MAX-1) : bidx);
    return &(BMP_ARRAY[bidx]);
}

