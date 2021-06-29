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
#include "mode.h"
#include "id.h"

#define FREQUENCY 921.2

#define TX_BUF_SIZE 100

#define GPS_LOCK_TIMEOUT (30 * 1000)

//#define GPS_OFF_TIME (5 * 60 * 1000)
#define GPS_OFF_TIME (30 * 1000)

static RH_RF95 rf95(SS_PIN, INTERRUPT_PIN);

#if TRACKER_MODE == TRANSMITTER

static TinyGPSPlus gps;

static struct pt gps_pt;

static PT_THREAD(gps_thread()) {
    PT_BEGIN(&gps_pt);
    static uint32_t time;
    while (1) {
        gps_enable(true);
        static uint8_t attempts;
        for (attempts = 0; attempts < 5; ++attempts) {
            time = system_time_get_ms();
            do {
                gps_encode_from_uart(gps);
                PT_YIELD(&gps_pt);
            } while (!gps_have_new_value(gps) && system_time_get_ms() - time < GPS_LOCK_TIMEOUT);
            if (gps_have_new_value(gps)) {
                char tx_buf[TX_BUF_SIZE];
                uint64_t id = id_get(gps);
                int n = snprintf(tx_buf, TX_BUF_SIZE, "%014llx,%06ld,%08ld,%f,%f,%f",
                    id,
                    gps.date.value(), gps.time.value(), 
                    gps.location.lat(), gps.location.lng(), 
                    gps.hdop.hdop()
                );
                if (n > 0 && n < TX_BUF_SIZE) {
                    rf95.send((uint8_t*)tx_buf, strlen(tx_buf));
                }
                break;
            } else {
            }
        }
        gps_enable(false);
        rf95.sleep();
        rtc_set_timeout_ms(GPS_OFF_TIME);
        standby();
        PT_YIELD(&gps_pt);
    }
    PT_END(&gps_pt);
}

#endif

#if TRACKER_MODE == RECEIVER

static struct pt rx_pt;

static PT_THREAD(rx_thread(void)) {
    PT_BEGIN(&rx_pt);
    while (1) {
        if (rf95.available()) {
            uint8_t buf[RH_RF95_MAX_MESSAGE_LEN + 1] = {0};
            uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
            if (rf95.recv(buf, &len)) {
                printf("%s\r\n", (const char *)buf);
                fflush(stdout);
            }
        }
        PT_YIELD(&rx_pt);
    }
    PT_END(&rx_pt);
}

#endif

int main(void)
{
	atmel_start_init();

    system_time_init();

    if (!rf95.init()) {
        while (1) {}
    }
    rf95.setFrequency(FREQUENCY);
    rf95.setTxPower(14, false);

#if TRACKER_MODE == TRANSMITTER
    rf95.sleep();
    rtc_init();
    uart_init();
#endif

#if TRACKER_MODE == TRANSMITTER
    PT_INIT(&gps_pt);
#elif TRACKER_MODE == RECEIVER
    PT_INIT(&rx_pt);
#endif

	while (1) {
#if TRACKER_MODE == TRANSMITTER
        PT_SCHEDULE(gps_thread());
#elif TRACKER_MODE == RECEIVER
        PT_SCHEDULE(rx_thread());
        cdc_device_acm_update();
#endif
	}
}
