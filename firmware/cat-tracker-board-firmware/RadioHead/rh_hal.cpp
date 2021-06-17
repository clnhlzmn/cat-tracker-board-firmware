/*
 * rh_hal.c
 *
 * Created: 6/16/2021 11:43:20 AM
 *  Author: Colin
 */ 

#include "rh_hal.h"
#include "sam.h"

static void (*rh_interrupt_cb)(void);

void attachInterrupt(int interrupt_number, void(*cb)(void), int mode) {
    if (interrupt_number == INTERRUPT_PIN && mode == RISING) {
        rh_interrupt_cb = cb;
        //turn on apba mask for eic
        PM->APBAMASK.reg |= PM_APBAMASK_EIC;
        //enable eic clock from generator 0
        GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_ID(0x05) | GCLK_CLKCTRL_GEN(0x0);
        //set pa14 to eic function
        PORT->Group[0].PINCFG[14].bit.PMUXEN = 1;
        PORT->Group[0].PMUX[14/2].bit.PMUXE = MUX_PA14A_EIC_EXTINT14;
        //extint14 is sense6 because there are two groups of config registers with each having half of
        //the available configurations, 6+8==14
        EIC->CONFIG[1].bit.SENSE6 = EIC_CONFIG_SENSE6_RISE_Val;
        EIC->INTENSET.reg = EIC_INTENSET_EXTINT14;
        NVIC_EnableIRQ(EIC_IRQn);
        EIC->CTRL.bit.ENABLE = 1;
        while (EIC->STATUS.bit.SYNCBUSY) {}
    }
}

extern "C" void EIC_Handler(void) {
    if (rh_interrupt_cb) {
        rh_interrupt_cb();
    }
}

void pinMode(int pin_number, int mode) {
    if (pin_number == INTERRUPT_PIN && mode == INPUT) {
        /*Don't have to do anything, rh interrupt pin is configured in attachInterrupt*/
    } else if (pin_number == SS_PIN && mode == OUTPUT) {
        PORT->Group[0].DIRSET.reg = PORT_PA10;
    }
}

void digitalWrite(int pin_number, bool value) {
    if (pin_number == SS_PIN) {
        if (value) {
            PORT->Group[0].OUTSET.reg = PORT_PA10;
        } else {
            PORT->Group[0].OUTCLR.reg = PORT_PA10;
        }
    }
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

void SPI_Impl::begin() {

}

uint8_t SPI_Impl::transfer(uint8_t) {
    return 0;
}

void SPI_Impl::end() {

}
