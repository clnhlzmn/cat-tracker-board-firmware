/*
 * standby.c
 *
 * Created: 6/20/2021 9:09:38 AM
 *  Author: colin
 */ 

#include "standby.h"
#include "sam.h"

void standby(void) {
    __DSB();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}
