/*
 * rtc.cpp
 *
 * Created: 6/19/2021 9:05:01 PM
 *  Author: colin
 */ 

#include "rtc.h"
#include "sam.h"

void rtc_init(void) {
    PM->APBAMASK.bit.RTC_ = 1;

    /*set GCLKGEN2 to use OSCULP32K with run in standby enabled*/
    GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_ID(2);

    /*set RTC GCLK to use GCLKGEN2*/
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_RTC;

    /*enable rtc in mode 0 with match clear*/
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0) | RTC_MODE0_CTRL_ENABLE;

    NVIC_EnableIRQ(RTC_IRQn);
}

void rtc_set_timeout_ms(uint32_t ms) {
    RTC->MODE0.COMP[0].reg = ms * 33;
    RTC->MODE0.COUNT.reg = 0;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
}

void RTC_Handler(void) {
    if (RTC->MODE0.INTFLAG.bit.CMP0) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_CMP0;
    }
}
