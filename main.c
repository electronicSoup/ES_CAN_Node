/**
 *
 * \file main.c
 *
 * \brief main entry point for the CAN Node OS project
 *
 * Copyright 2014 John Whitmore <jwhitmore@electronicsoup.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 2 of the GNU General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "libesoup_config.h"

#include <stdlib.h>

#ifdef SYS_SERIAL_LOGGING
#define DEBUG_FILE
static const char *TAG = "Main";
#include "libesoup/logger/serial_log.h"
#endif // SYS_SERIAL_LOGGING

#include "libesoup/timers/sw_timers.h"
#include "libesoup/comms/can/can.h"
//#include "es_lib/usb/android/android_comms.h"
//#include "es_lib/usb/android/state_idle.h"

#include "libesoup/hardware/eeprom.h"
//#include "es_lib/utils/flash.h"
//#include "es_lib/utils/rand.h"
//#include "os_api.h"

//#include "es_lib/can/dcncp/cinnamonbun_info.h"

#include "libesoup/status/status.h"

void system_status_handler(union ty_status status);

/*
 * Device handle for the connected USB Device
 */
//static void* device_handle = NULL;

int main(void)
{
	can_baud_rate_t baud_rate;
	uint8_t         node_status;
	uint16_t        reset_status;
	result_t        rc = SUCCESS;

	/*
	 * Read and store the Reset condition to check once initialised.
	 */
        reset_status = RCON;
	
	libesoup_init();

#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("************************\n\r");
	LOG_D("***   CAN Bus Node   ***\n\r");
	LOG_D("************************\n\r");
#endif
	
	/*
	 * Read the saved Node Status from EEPROM
	 */
	rc = eeprom_read(EEPROM_NODE_STATUS_ADDR, &node_status);
	if(rc != SUCCESS) {
#if (defined(SYS_SERIAL_LOGGING) && (SYS_LOG_LEVEL <= LOG_ERROR))
		LOG_E("Failed to write to EEPROM\n\r");
#endif
	}
	
	if(reset_status & RCON_WDTO) {
#if (defined(SYS_SERIAL_LOGGING) && (LOG_LEVEL < NO_LOGGING))
		LOG_E("Watch Dog Timed out!\n\r");
#endif
//		app_valid = FALSE;

		rc = eeprom_write(EEPROM_NODE_STATUS_ADDR, node_status & ~(NODE_STATUS_APP_VALID));
		if(rc != SUCCESS) {
#if (defined(SYS_SERIAL_LOGGING) && (SYS_LOG_LEVEL <= LOG_ERROR))
			LOG_E("Failed to write to EEPROM\n\r");
#endif
		}
	}

	asm ("CLRWDT");

        /*
	 * Initialise the OS structures.
	 */
//	os_init_data();

	asm ("CLRWDT");

	eeprom_read(EEPROM_NODE_CAN_BAUD_RATE_ADDR, (uint8_t *) & baud_rate);
        if(baud_rate >= no_baud) {
		baud_rate = baud_10K;
		LOG_W("No CAN Baud Rate set so storing 10KBit/s\n\r");
		eeprom_write(EEPROM_NODE_CAN_BAUD_RATE_ADDR, baud_rate);
        }

	can_init(baud_rate, 0x00, system_status_handler);

	asm ("CLRWDT");

//        if(app_valid) {
//		LOG_D("Call App Init as Application is valid\n\r");
//		CALL_APP_INIT();
//        }

	/*
	 * Enter the main loop
	 */
	LOG_D("Entering the main loop\n\r");
	while(TRUE) {
		asm ("CLRWDT");
		CHECK_TIMERS();

		can_tasks();

                asm ("CLRWDT");

//		if (app_valid) {
//			CALL_APP_MAIN();
//		}
	}
}

/**
 *
 *
 * Don't let the ISO11783 Broadcast address be used.
 *
 */
uint8_t node_get_address(void)
{
	static uint8_t retry_count = 0;
	result_t rc;
	uint8_t address;

	if(retry_count == 0) {
		rc = eeprom_read(EEPROM_NODE_ADDRESS, &address);
		if (rc != SUCCESS) {
			LOG_E("address Failed to read from eeprom return code 0x%x\n\r", rc);
			do {
				address = (uint8_t) (rand() & 0x0ff);
			} while (address == BROADCAST_NODE_ADDRESS);

			rc = eeprom_write(EEPROM_NODE_ADDRESS, address);
			if (rc != SUCCESS) {
				LOG_E("Failed to write from eeprom return code 0x%x\n\r", rc);
			}
		} else {
			LOG_D("address value read back from EEPROM address 0x02 = 0x%x\n\r", address);
		}
	} else {
		do {
			address = (uint8_t) (rand() & 0x0ff);
		} while (address == BROADCAST_NODE_ADDRESS);

		rc = eeprom_write(EEPROM_NODE_ADDRESS, address);
		if (rc != SUCCESS) {
			LOG_E("Failed to write from eeprom return code 0x%x\n\r", rc);
		}
	}

	retry_count++;

	LOG_D("node_get_address() Retry %d try address 0x%x\n\r", retry_count, address);
	return(address);
}

void system_status_handler(union ty_status status)
{
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("status_handler()\n\r");
#endif
}

