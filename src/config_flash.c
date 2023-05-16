#include "mqtt/config_flash.h"
#include "mqtt/debug.h"

uint32_t RETAINEDTOPIC_FLASH_SECTOR;
static uint32_t get_address(){
   return RETAINEDTOPIC_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
}

void ICACHE_FLASH_ATTR blob_save(uint32_t *data, uint16_t len)
{
   uint32_t address = get_address();
	spi_flash_erase_sector(RETAINEDTOPIC_FLASH_SECTOR);
	spi_flash_write(address, data, len);
}

void ICACHE_FLASH_ATTR blob_load(uint32_t *data, uint16_t len)
{
   uint32_t address = get_address();
	spi_flash_read(address, data, len);
}

void set_retain_flash_sector(){
	//Reference: https://github.com/espressif/esp8266-nonos-sample-code/blob/master/04Protocal/IoT_Demo_https/user/user_main.c
	RETAINEDTOPIC_FLASH_SECTOR=user_rf_cal_sector_set();
	if(RETAINEDTOPIC_FLASH_SECTOR>0){RETAINEDTOPIC_FLASH_SECTOR--;}//keep some distance to rf-sector
	//Alternative solutions for SDK 3.x? see discussions:
	//in https://www.esp8266.com/viewtopic.php?p=76325
    //or https://www.esp8266.com/viewtopic.php?f=159&t=23258&start=4 outdated with SDK 3.x
	//flash_size_map defintion https://www.espressif.com/sites/default/files/2c-esp8266_non_os_sdk_api_guide_en_v1.5.4.pdf
   DEBUG("RETAINEDTOPIC_FLASH_SECTOR :");
   DEBUG_u32(RETAINEDTOPIC_FLASH_SECTOR);
  }