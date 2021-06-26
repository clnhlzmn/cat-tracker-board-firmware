#include "atmel_start.h"
#include "atmel_start_pins.h"
#include <stdio.h>
#include "RH_RF95.h"
#include "uart.h"
#include "system_time.h"
#include "TinyGPS++.h"
#include "pt.h"
#include "gps.h"
#include "rtc.h"
#include "standby.h"

#define FREQUENCY 921.2

#define TX_BUF_SIZE 100

#define GPS_LOCK_TIMEOUT (30 * 1000)

//#define GPS_OFF_TIME (5 * 60 * 1000)
#define GPS_OFF_TIME (30 * 1000)

static RH_RF95 rf95(SS_PIN, INTERRUPT_PIN);

static TinyGPSPlus gps;

static struct pt pt;

static PT_THREAD(gps_thread()) {
    PT_BEGIN(&pt);
    static uint32_t time;
    while (1) {
        printf("enabling gps\r\n");
        gps_enable(true);
        time = system_time_get_ms();
        do {
            gps_encode_from_uart(gps);
            PT_YIELD(&pt);
        } while (!gps_have_new_value(gps) && system_time_get_ms() - time < GPS_LOCK_TIMEOUT);
        if (gps_have_new_value(gps)) {
            char tx_buf[TX_BUF_SIZE];
            int n = snprintf(tx_buf, TX_BUF_SIZE, "%ld,%ld,%f,%f,%f", gps.date.value(), gps.time.value(), gps.location.lat(), gps.location.lng(), gps.hdop.hdop());
            if (n > 0 && n < TX_BUF_SIZE) {
                printf("%s\r\n", tx_buf);
                rf95.send((uint8_t*)tx_buf, strlen(tx_buf));
            }
        } else {
            printf("didn't get gps lock\r\n");
        }
        printf("disabling gps\r\n");
        gps_enable(false);
        rf95.sleep();
        if (cdc_device_enabled()) {
            time = system_time_get_ms();
            PT_WAIT_UNTIL(&pt, system_time_get_ms() - time >= GPS_OFF_TIME);
        } else {
            rtc_set_timeout_ms(GPS_OFF_TIME);
            standby();
            PT_YIELD(&pt);
        }
    }
    PT_END(&pt);
}

int main(void)
{
	atmel_start_init();

    system_time_init();

    rtc_init();

    bool radio_init = rf95.init();
    if (!radio_init) {
        printf("failed to initialize radio");
        while (1) {}
    }
    rf95.setFrequency(FREQUENCY);
    rf95.setTxPower(14, false);
    rf95.sleep();

    uart_init();

    PT_INIT(&pt);

	while (1) {
        PT_SCHEDULE(gps_thread());
        cdc_device_acm_update();
	}
}
