/*
 * nvm.c
 *
 * Created: 6/29/2021 6:23:46 PM
 *  Author: colin
 */

#include <string.h>
#include "nvm.h"
#include "sam.h"

#define NVM_ADDR (0x3ff00)

#define PAGE_SIZE (0x40)

//from https://github.com/cmaglie/FlashStorage/blob/master/src/FlashStorage.cpp

static inline uint32_t read_unaligned_uint32(const void *data)
{
    union {
        uint32_t u32;
        uint8_t u8[4];
    } res;
    const uint8_t *d = (const uint8_t *)data;
    res.u8[0] = d[0];
    res.u8[1] = d[1];
    res.u8[2] = d[2];
    res.u8[3] = d[3];
    return res.u32;
}

static void nvm_unlock(void) {
    if (NVMCTRL->LOCK.reg & 1 << 15) {
        //locked, unlock
        NVMCTRL->ADDR.reg = NVM_ADDR / 2;
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
        while (!NVMCTRL->INTFLAG.bit.READY) { }
    }
}

//erase n bytes, return number of bytes erased
static void nvm_erase(void) {
    NVMCTRL->ADDR.reg = NVM_ADDR / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    while (!NVMCTRL->INTFLAG.bit.READY) { }
}

void nvm_read(uint8_t *dst, int n) {
    memcpy((char *)dst, (const char *)NVM_ADDR, n);
}

void nvm_write(const uint8_t *src, int n) {
    //unlock first
    nvm_unlock();
     
    //erase the row
    nvm_erase();
     
    //adjust size for 32 bit writes
    int size = (n + 3) / 4;

    //destination address
    uint32_t *dst = (uint32_t*)NVM_ADDR;

    // Disable automatic page write
    NVMCTRL->CTRLB.bit.MANW = 1;

    while (size) {
        // Execute "PBC" Page Buffer Clear
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
        while (NVMCTRL->INTFLAG.bit.READY == 0) { }

        //fill page buffer
        //write PAGE_SIZE / sizeof(uint32_t) = 16 chunks of uint32_t to fill a page
        for (uint32_t i = 0; i < (PAGE_SIZE / 4) && size; i++) {
            *dst = read_unaligned_uint32(src);
            src += 4;
            dst++;
            size--;
        }
        // Execute "WP" Write Page
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
        while (NVMCTRL->INTFLAG.bit.READY == 0) { }
    }
}