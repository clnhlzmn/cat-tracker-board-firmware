/*
 * rh_hal.c
 *
 * Created: 6/16/2021 11:43:20 AM
 *  Author: Colin
 */ 

#include "rh_hal.h"
#include "sam.h"
#include "system_time.h"
#include <stdlib.h>
#include "hal_delay.h"
#include "hal_atomic.h"

static void (*rh_interrupt_cb)(void);

void attachInterrupt(int interrupt_number, void(*cb)(void), int mode) {
    if (interrupt_number == INTERRUPT_PIN && mode == RISING) {
        rh_interrupt_cb = cb;
        //turn on apba mask for eic
        PM->APBAMASK.reg |= PM_APBAMASK_EIC;
        //enable eic clock from generator 0
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_EIC;
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
    if (EIC->INTFLAG.reg & EIC_INTFLAG_EXTINT14) {
        EIC->INTFLAG.reg |= EIC_INTFLAG_EXTINT14;
        if (rh_interrupt_cb) {
            rh_interrupt_cb();
        }
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
    return system_time_get_ms();
}

long random(long min, long max) {
    if (min >= max) {
        return min;
    }
    return rand() % (max - min) + min;
}

void delay(long ms) {
    delay_ms(ms);
}

SPI_Impl SPI;

void SPI_Impl::begin() {
    //enable bus
    PM->APBCMASK.bit.SERCOM0_ = 1;
    //assign sercom function to pins
    PORT->Group[0].PINCFG[8].bit.PMUXEN = 1;
    PORT->Group[0].PMUX[8/2].bit.PMUXE = MUX_PA08C_SERCOM0_PAD0;
    PORT->Group[0].PINCFG[9].bit.PMUXEN = 1;
    PORT->Group[0].PMUX[9/2].bit.PMUXO = MUX_PA09C_SERCOM0_PAD1;
    PORT->Group[0].PINCFG[11].bit.PMUXEN = 1;
    PORT->Group[0].PMUX[11/2].bit.PMUXO = MUX_PA11C_SERCOM0_PAD3;
    //enable clock
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM0_CORE;
    //reset sercom
    SERCOM0->SPI.CTRLA.bit.SWRST = 1;
    while (SERCOM0->SPI.SYNCBUSY.bit.SWRST) {}
    //disable sercom
    SERCOM0->SPI.CTRLA.bit.ENABLE = 0;
    while (SERCOM0->SPI.SYNCBUSY.bit.ENABLE) {}
    //configure sercom
    SERCOM0->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_DIPO(0x3) | SERCOM_SPI_CTRLA_DOPO(0) | SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
    SERCOM0->SPI.CTRLB.bit.RXEN = 1;
    while (SERCOM0->SPI.SYNCBUSY.bit.CTRLB) {}
    //1 MHz clock
    SERCOM0->SPI.BAUD.reg = 23;
    //enable sercom
    SERCOM0->SPI.CTRLA.bit.ENABLE = 1;
    while (SERCOM0->SPI.SYNCBUSY.bit.ENABLE) {}
}

uint8_t SPI_Impl::transfer(uint8_t data) {
    while (!SERCOM0->SPI.INTFLAG.bit.DRE) {}
    SERCOM0->SPI.DATA.reg = data;
    while (!SERCOM0->SPI.INTFLAG.bit.TXC) {}
    while (!SERCOM0->SPI.INTFLAG.bit.RXC) {}
    return SERCOM0->SPI.DATA.reg;
}

void SPI_Impl::end() {
    //disable sercom
    SERCOM0->SPI.CTRLA.bit.ENABLE = 0;
    while (SERCOM0->SPI.SYNCBUSY.bit.ENABLE) {}
}
