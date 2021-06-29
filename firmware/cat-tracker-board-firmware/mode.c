/*
 * mode.c
 *
 * Created: 6/28/2021 7:55:10 PM
 *  Author: colin
 */ 

#if MODE == TRANSMITTER
#pragma message("building for transmitter")
#elif MODE == RECEIVER
#pragma message("building for receiver")
#else
#error "mode not defined"
#endif
