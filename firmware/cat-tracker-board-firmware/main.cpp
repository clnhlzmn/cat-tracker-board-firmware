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
#include "TinyGPS++.h"

RH_RF95 rf95(SS_PIN, INTERRUPT_PIN);

TinyGPSPlus gps;

#define FREQUENCY 921.2

#define TX_BUF_SIZE 100

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

    char tx_buf[TX_BUF_SIZE];
    bool new_message = false;

	while (1) {
        uint8_t rx_buf[100+1];
        memset(rx_buf, 0, 100+1);
        int n = uart_receive(rx_buf, 100);
        if (n > 0) {
            printf((const char *)rx_buf);
            for (int i = 0; i < n; ++i) {
                gps.encode(rx_buf[i]);
            }
        }
        if (gps.location.isValid() && gps.location.isUpdated()) {
            int n = snprintf(tx_buf, TX_BUF_SIZE, "%ld,%ld,%f,%f,%f", gps.date.value(), gps.time.value(), gps.location.lat(), gps.location.lng(), gps.hdop.hdop());
            if (n > 0 && n < TX_BUF_SIZE) {
                new_message = true;
            }
        }
        if (system_time_get_ms() - tx_time >= 1000 && new_message) {
            tx_time = system_time_get_ms();
            new_message = false;
            rf95.send((uint8_t*)tx_buf, strlen(tx_buf));
        }
        cdc_device_acm_update();
	}
}
