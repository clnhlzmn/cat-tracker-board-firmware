/*
 * rh_hal.h
 *
 * Created: 6/15/2021 7:51:50 PM
 *  Author: colin
 */ 


#ifndef RH_HAL_H_
#define RH_HAL_H_

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#define PROGMEM

#define FALLING 0
#define RISING 1

void attachInterrupt(int, void(*)(void), int);

void pinMode(int, int);

void digitalWrite(int, bool);

unsigned long millis();

long random(long, long);

void delay(long);

#define memcpy_P memcpy

#endif /* RH_HAL_H_ */