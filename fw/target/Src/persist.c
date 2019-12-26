#include "persist.h"
#include <stdint.h>
#include "main.h"
#include "common/dpy_hw.h"
#include "common/sfp_hw.h"
#include "common/auto_off.h"
#include <string.h>

extern CRC_HandleTypeDef hcrc;

#define PERSIST_REV 1
#define PERSIST_ADDR 0x801f800 // end of flash

typedef struct {
    uint32_t crc;
    uint8_t rev;
    uint8_t brightness;
    uint8_t auto_off;
    uint8_t auto_on_sfp;
    uint8_t _pad[1];
} persist_t;

static persist_t *persist_data = (void *)(PERSIST_ADDR);

static uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
        /* Bank 1 */
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else {
        /* Bank 2 */
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

void persist_save()
{
    persist_t pdata = {
            .rev = PERSIST_REV,
            .brightness = dpy_hw_get_brightness(),
			.auto_off = auto_off_get_timeout_min(),
			.auto_on_sfp = sfp_hw_get_auto_on()
    };
    uint32_t crc = HAL_CRC_Calculate(&hcrc, (void *)((uint8_t *)&pdata + sizeof(uint32_t)),
                                     sizeof(persist_t) - sizeof(uint32_t));
    pdata.crc = crc;
    if (memcmp(&pdata, persist_data, sizeof(persist_t)) != 0) { // store


        HAL_FLASH_Unlock();

        uint32_t PageError = 0;
        FLASH_EraseInitTypeDef eraseinitstruct;

        /* Clear OPTVERR bit set on virgin samples */
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

        /* Get the number of sector to erase from 1st sector*/
        eraseinitstruct.Banks = FLASH_BASE;
        eraseinitstruct.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseinitstruct.Page = GetPage(PERSIST_ADDR);
        eraseinitstruct.NbPages = 1;
        if (HAL_FLASHEx_Erase(&eraseinitstruct, &PageError) != HAL_OK)
            goto lock;

        for (uint8_t i = 0; i < sizeof(persist_t); i += 8) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(((uint8_t *)persist_data) + i),
                                  *(uint64_t *)(((uint8_t *)&pdata) + i))
                == HAL_OK) {
                /* Check the written value */
                if (*(uint64_t *)(((uint8_t *)&pdata) + i) != *(uint64_t *)(((uint8_t *)persist_data) + i)) {
                    /* Flash content doesn't match SRAM content */
                    goto lock;
                }
            }
            else {
                /* Error occurred while writing data in Flash memory */
                goto lock;
            }
        }

    lock:
        HAL_FLASH_Lock();
    }
}
void persist_recall()
{
    uint32_t crc_from_flash = persist_data->crc;
    uint32_t crc_calc =
            HAL_CRC_Calculate(&hcrc, (void *)((uint8_t *)persist_data + sizeof(uint32_t)), sizeof(persist_t) - sizeof(uint32_t));
    if (crc_calc == crc_from_flash) {
        if (persist_data->rev == PERSIST_REV) {
            dpy_hw_set_brightness(persist_data->brightness);
            sfp_hw_set_auto_on(persist_data->auto_on_sfp);
            auto_off_set_timeout_min(persist_data->auto_off);
        }
    }
}
