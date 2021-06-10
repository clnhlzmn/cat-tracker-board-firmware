/*
 * system_time.h
 *
 * Created: 10/16/2019 3:14:44 PM
 *  Author: Colin
 */ 


#ifndef SYSTEM_TIME_H_
#define SYSTEM_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void system_time_init(void);

uint32_t system_time_get_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_TIME_H_ */