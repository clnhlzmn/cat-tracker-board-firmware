/*
 * nvm.h
 *
 * Created: 6/29/2021 6:22:31 PM
 *  Author: colin
 */ 


#ifndef NVM_H_
#define NVM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//exposes no more than 0x100 bytes of flash memory

//read no more than n bytes from flash, returns number of bytes read
void nvm_read(uint8_t *dst, int n);

//write n bytes from address src to flash, returns number of bytes written
void nvm_write(const uint8_t *src, int n);

#ifdef __cplusplus
}
#endif

#endif /* NVM_H_ */
