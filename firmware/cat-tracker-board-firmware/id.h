/*
 * id.h
 *
 * Created: 6/29/2021 7:10:24 PM
 *  Author: colin
 */ 


#ifndef ID_H_
#define ID_H_

#include <stdint.h>
#include "TinyGPS++.h"

/*Returns a 56 bit id from flash memory. 
If there is no ID present in flash then one is generated 
based on the gps date/time and system ms clock and then
stored in flash for next time an id is needed.*/
uint64_t id_get(TinyGPSPlus &);

#endif /* ID_H_ */
