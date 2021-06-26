/*
 * standby.c
 *
 * Created: 6/20/2021 9:09:38 AM
 *  Author: colin
 */ 

#include "standby.h"
#include "sam.h"
#include "hal_atomic.h"

void standby(void) {
    CRITICAL_SECTION_ENTER();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
    __WFI();
    CRITICAL_SECTION_LEAVE();
}
