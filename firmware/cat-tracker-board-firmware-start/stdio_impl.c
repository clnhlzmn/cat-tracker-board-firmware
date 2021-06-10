/*
 * stdio_impl.c
 *
 * Created: 2/6/2020 10:39:29 AM
 *  Author: Colin
 */ 

#include <stdlib.h>
#include <stdint.h>
#include <err_codes.h>
#include "cdcdf_acm.h"

int _write(int fd, const void *buf, size_t count) {
    if (fd != 1 && fd != 2 && fd != 3) return -1;
    int status = cdcdf_acm_write((uint8_t*)buf, count);
    if (status == ERR_NONE) {
        return count;
    } else {
        return 0;
    }
}

int _read(int fd, const void *buf, size_t count) {
    if (fd != 1 && fd != 2 && fd != 3) return -1;
    return 0;
}
