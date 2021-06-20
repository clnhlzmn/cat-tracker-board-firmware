/*
 * rtc.h
 *
 * Created: 6/19/2021 8:53:51 PM
 *  Author: colin
 */ 


#ifndef RTC_H_
#define RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void rtc_init(void);

void rtc_set_timeout_ms(uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* RTC_H_ */
