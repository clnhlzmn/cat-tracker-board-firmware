/*
 * system_time.c
 *
 * Created: 10/16/2019 3:15:01 PM
 *  Author: Colin
 */ 
 
#include <stdbool.h>
#include "system_time.h"
#include "sam.h"

#define SYSTEM_CLOCK_HZ (48E6)

#define SYS_TICK_COUNTS (SYSTEM_CLOCK_HZ / 10000)

static volatile uint32_t time;

void system_time_init(void) {
    SysTick_Config(SYS_TICK_COUNTS);
    /*set sys tick to lowest priority*/
    NVIC_SetPriority(SysTick_IRQn, 3);
}

uint32_t system_time_get_ms(void) {
    return time / 10;
}

void SysTick_Handler(void) {
    time++;
}
