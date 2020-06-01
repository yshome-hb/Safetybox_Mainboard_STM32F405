#include "string.h"
#include "stmflash.h"


static inline uint32_t stmflash_word(uint32_t addr)
{
	return *(vu32*)addr; 
}  


uint16_t stmflash_get_sector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1) return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2) return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3) return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4) return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5) return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6) return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7) return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8) return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9) return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10) return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11) return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}


bool stmflash_write(uint32_t addr, const uint8_t *pdata, uint32_t len)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	uint32_t addr_index = addr;
	uint32_t addr_end = addr + len;	
	
  if(addr_index < ADDR_FLASH_BASE || addr_end > ADDR_FLASH_END || (addr&0x03))
		return false;
	
	FLASH_Unlock();
  FLASH_DataCacheCmd(DISABLE);
	
	while(addr_index < addr_end)
	{
		if(stmflash_word(addr_index) != 0xFFFFFFFF)
		{   
			status = FLASH_EraseSector(stmflash_get_sector(addr_index), VoltageRange_3);
			if(status != FLASH_COMPLETE)
				break;
		}
		else 
			addr_index += 4;
	} 	
	
	if(status == FLASH_COMPLETE)
	{
		addr_index = addr;	
		while(addr_index < addr_end)
		{
			status = FLASH_ProgramWord(addr_index, *((uint32_t *)pdata));
			if(status != FLASH_COMPLETE) 
				break;
			
			addr_index += 4;
			pdata += 4;
		} 
	}

  FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
	
	return (status == FLASH_COMPLETE);
} 


bool stmflash_read(uint32_t addr, uint8_t *pdata, uint32_t len)   	
{
	uint32_t i = 0;

  if(addr < ADDR_FLASH_BASE || (addr+i) > ADDR_FLASH_END || (addr&0x03))
		return false;	
	
	if(len > 4)
	{
		for(i = 0; i < (len-4); i+=4)
		{		
			(*((uint32_t *)(pdata+i))) = stmflash_word(addr+i);
		}
	}
	
	uint32_t temp = stmflash_word(addr+i);	
	memcpy((pdata+i), &temp, (len-i));
	
	return true;
}




