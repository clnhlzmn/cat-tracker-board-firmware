/*
 * gps.h
 *
 * Created: 6/18/2021 6:16:49 PM
 *  Author: colin
 */ 


#ifndef GPS_H_
#define GPS_H_

#include "TinyGPS++.h"

void gps_enable(bool);

void gps_encode_from_uart(TinyGPSPlus &);

bool gps_have_new_value(TinyGPSPlus &);

#endif /* GPS_H_ */
