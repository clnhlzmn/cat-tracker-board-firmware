/*
 * rtc.cpp
 *
 * Created: 6/19/2021 9:05:01 PM
 *  Author: colin
 */ 

#include "rtc.h"
#include "sam.h"
#include <stdlib.h>
#include <stdbool.h>

static bool rtc_is_syncing(void) {
    return RTC->MODE0.STATUS.bit.SYNCBUSY;
}

void rtc_init(void) {
    PM->APBAMASK.bit.RTC_ = 1;

    /*set GCLKGEN3 to use OSCULP32K*/
    /*GCLKGEN3 should already be set to use OSCULP32K by atmel start code*/

    /*set RTC GCLK to use GCLKGEN3*/
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_RTC;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /*enable rtc in mode 0 with match clear*/
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0) | RTC_MODE0_CTRL_ENABLE;
    while (rtc_is_syncing()) {}

    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0x00);
}

void rtc_set_timeout_ms(uint32_t ms) {
    RTC->MODE0.COMP[0].reg = ms * 33;
    while (rtc_is_syncing()) {}
    RTC->MODE0.COUNT.reg = 0;
    while (rtc_is_syncing()) {}
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
}

void RTC_Handler(void) {
    if (RTC->MODE0.INTFLAG.bit.CMP0) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
    }
}
