/**
 * \file
 *
 * \brief Application implement
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "atmel_start.h"
#include "atmel_start_pins.h"
#include <stdio.h>
#include "RH_RF95.h"
#include "uart.h"
#include "system_time.h"

RH_RF95 rf95(SS_PIN, INTERRUPT_PIN);

#define FREQUENCY 921.2

int main(void)
{
	atmel_start_init();

    system_time_init();

    bool radio_init = rf95.init();
    if (!radio_init) {
        printf("failed to initialize radio");
        while (1) {}
    }
    rf95.setFrequency(FREQUENCY);
    rf95.setTxPower(14, false);

    uart_init();
    
    //set gps enable pin as output
    PORT->Group[0].DIRSET.reg = PORT_PA17;
    PORT->Group[0].OUTCLR.reg = PORT_PA17;

    uint32_t tx_time;

	while (1) {
        uint8_t rx_buf[100+1];
        memset(rx_buf, 0, 100+1);
        int n = uart_receive(rx_buf, 100);
        if (n > 0) {
            printf((const char *)rx_buf);
        }
        if (system_time_get_ms() - tx_time >= 1000) {
            tx_time = system_time_get_ms();
            const char *message = "hello world";
            rf95.send((uint8_t*)message, strlen(message));
        }
        cdc_device_acm_update();
	}
}
