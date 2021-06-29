/*
 * rh_hal.h
 *
 * Created: 6/15/2021 7:51:50 PM
 *  Author: colin
 */ 


#ifndef RH_HAL_H_
#define RH_HAL_H_

#include <stdint.h>

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#define PROGMEM

#define FALLING 0
#define RISING 1

//arduino style pin numbers that will be passed 
//to RH_RF95 ctor and then to functions below
#define SS_PIN 10
#define INTERRUPT_PIN 2

void attachInterrupt(int, void(*)(void), int);

void pinMode(int, int);

void digitalWrite(int, bool);

unsigned long millis();

long random(long, long);

void delay(long);

#define memcpy_P memcpy

class SPI_Impl {
public:
    static void begin();
    static uint8_t transfer(uint8_t);
    static void end();
private:
};

extern SPI_Impl SPI;

#include "hal_atomic.h"

#define ATOMIC_BLOCK_START CRITICAL_SECTION_ENTER();
#define ATOMIC_BLOCK_END CRITICAL_SECTION_LEAVE();

#endif /* RH_HAL_H_ */