/*
 * id.cpp
 *
 * Created: 6/29/2021 7:11:46 PM
 *  Author: colin
 */ 

#include "id.h"
#include "system_time.h"
#include "nvm.h"

union id_data {
    uint64_t u64; 
    uint8_t bytes[8];
};

uint64_t id_get(TinyGPSPlus &gps) {
    id_data id;
    nvm_read(id.bytes, 8);
    if (id.u64 == 0xffffffffffffffff) {
        uint32_t date = gps.date.value();
        uint32_t time = gps.time.value() / 100;
        uint16_t ms = system_time_get_ms() & 0xffff;
        id.u64 = (uint64_t)date << 36 | (uint64_t)time << 16 | ms;
        nvm_write(id.bytes, 8);
    }
    return id.u64;
}
