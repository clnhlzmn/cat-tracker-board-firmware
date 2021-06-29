#include <atmel_start.h>
#include "mode.h"

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();
#if TRACKER_MODE == RECEIVER
	usb_init();
#endif
}
