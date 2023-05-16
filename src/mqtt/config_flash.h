#ifndef CONFIG_FLASH_H_
#define CONFIG_FLASH_H_
#include "c_types.h"
#include <spi_flash.h>
void set_retain_flash_sector();
void ICACHE_FLASH_ATTR blob_save(uint32_t *data, uint16_t len);
void ICACHE_FLASH_ATTR blob_load(uint32_t *data, uint16_t len);

#endif /* CONFIG_FLASH_H_ */