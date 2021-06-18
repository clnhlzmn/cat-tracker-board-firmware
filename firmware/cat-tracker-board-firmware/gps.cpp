/*
 * gps.cpp
 *
 * Created: 6/18/2021 6:17:20 PM
 *  Author: colin
 */ 

#include "gps.h"
#include "sam.h"
#include <string.h>
#include "uart.h"
#include <stdio.h>

void gps_enable(bool enable) {
    PORT->Group[0].DIRSET.reg = PORT_PA17;
    if (enable) {
        PORT->Group[0].OUTCLR.reg = PORT_PA17;
    } else {
        PORT->Group[0].OUTSET.reg = PORT_PA17;
    }
}

void gps_encode_from_uart(TinyGPSPlus &gps) {
    uint8_t rx_buf[100+1];
    memset(rx_buf, 0, 100+1);
    int n = uart_receive(rx_buf, 100);
    if (n > 0) {
        printf((const char *)rx_buf);
        for (int i = 0; i < n; ++i) {
            gps.encode(rx_buf[i]);
        }
    }
}

bool gps_have_new_value(TinyGPSPlus &gps) {
    return gps.location.isValid() && gps.location.isUpdated();
}
