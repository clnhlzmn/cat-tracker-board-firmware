/*
 * rh_hal.c
 *
 * Created: 6/16/2021 11:43:20 AM
 *  Author: Colin
 */ 

#include "rh_hal.h"

void attachInterrupt(int interrupt_number, void(*cb)(void), int mode) {

}

void pinMode(int pin_number, int mode) {

}

void digitalWrite(int pin_number, bool value) {

}

unsigned long millis() {
    return 0;
}

long random(long min, long max) {
    return 0;
}

void delay(long ms) {
    
}

SPI_Impl SPI;

uint8_t SPI_Impl::transfer(uint8_t) {
    return 0;
}

void SPI_Impl::end() {

}
