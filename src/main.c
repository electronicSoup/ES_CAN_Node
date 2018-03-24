/**
 *
 * \file main.c
 *
 * \brief main entry point for the CAN Node
 *
 * Copyright 2014-2018 electronicSoup
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 3 of the GNU General Public License
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

#include "libesoup/errno.h"
#include "libesoup/timers/sw_timers.h"
#include "libesoup/timers/delay.h"
#include "libesoup/comms/can/can.h"

#include "libesoup/hardware/eeprom.h"
#include "libesoup/status/status.h"

#include "app.h"

static boolean app_valid = FALSE;

void system_status_handler(union ty_status status);

#ifdef SYS_SW_TIMERS
static void expiry(timer_id timer, union sigval);
#endif
static void frame_handler(can_frame *);

/*
 * Device handle for the connected USB Device
 */
//static void* device_handle = NULL;

int main(void)
{
	can_baud_rate_t  baud_rate;
	uint8_t          node_status;
	boolean          watchdog;
	result_t         rc = 0;
	can_l2_target_t  target;
	can_baud_rate_t  bit_rate;
#ifdef SYS_SW_TIMERS
	timer_id         timer;
	struct timer_req request;
#endif

	/*
	 * Store the watchdog reset status for later.
	 */
	watchdog  = RCONbits.WDTO;
	
	libesoup_init();

	delay(mSeconds, 500);
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("************************\n\r");
	LOG_D("***   CAN Bus Node   ***\n\r");
	LOG_D("***   %ldMHz         ***\n\r", sys_clock_freq);
	LOG_D("************************\n\r");
#endif
	
	if(watchdog) {
#if (defined(SYS_SERIAL_LOGGING) && (LOG_LEVEL < NO_LOGGING))
		LOG_E("Watch Dog Timed out!\n\r");
#endif
		app_valid = FALSE;

		rc = eeprom_write(EEPROM_NODE_STATUS_ADDR, node_status & ~(NODE_STATUS_APP_VALID));
		RC_CHECK_PRINT("Failed to write to EEPROM\n\r");
	} else {
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("Application assumed good!\n\r");
#endif		
	}
	
	asm ("CLRWDT");

	rc = delay(mSeconds, 500);
	RC_CHECK_PRINT("Failed to delay\n\r");
#if 0
	/*
	 * Quick test of CAN Bus bit rates
	 */
	for(bit_rate = baud_10K; bit_rate < no_baud; bit_rate++) {
		rc = can_l2_bitrate(bit_rate, FALSE);

		if(rc >= 0) {
			LOG_D("%s GOOD!\n\r", can_baud_rate_strings[bit_rate]);
		} else if (rc == -ERR_CAN_BITRATE_HIGH) {
			LOG_D("%s Too High!\n\r", can_baud_rate_strings[bit_rate]);
		} else if (rc == -ERR_CAN_BITRATE_LOW) {
			LOG_D("%s Too Low!\n\r", can_baud_rate_strings[bit_rate]);
		}
	}
#endif
	baud_rate = eeprom_read(EEPROM_NODE_CAN_BAUD_RATE_ADDR);
        if( (baud_rate >= no_baud) || (baud_rate < 0) ) {
		baud_rate = baud_250K;
		LOG_W("No CAN Baud Rate set so storing 10KBit/s\n\r");
		rc = eeprom_write(EEPROM_NODE_CAN_BAUD_RATE_ADDR, baud_rate);
		RC_CHECK_PRINT("Failed to write to EEPROM\n\r");
        }
	rc = delay(mSeconds, 500);
	RC_CHECK_PRINT("Failed to delay()\n\r");
 	rc = can_init(baud_rate, system_status_handler);
	RC_CHECK_PRINT("Failed to initialise CAN Bus\n\r");

	asm ("CLRWDT");
	
	/*
	 * Register a frame handler
	 */
	target.filter = 0x555;
	target.mask   = CAN_SFF_MASK;
	target.handler = frame_handler;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK_PRINT("Failed to register frame handler\n\r");

        if(app_valid) {
		LOG_D("Call App Init as Application is valid\n\r");
		rc = app_init();
		if(rc < 0) app_valid = FALSE;
        }

	/*
	 * Enter the main loop
	 */
	LOG_D("Entering the main loop\n\r");
	LOG_D("***   %ldMHz         ***\n\r", sys_clock_freq);
	while(TRUE) {
		asm ("CLRWDT");
#ifdef SYS_SW_TIMERS
		CHECK_TIMERS();
#endif

		can_tasks();

                asm ("CLRWDT");

		if (app_valid) {
			rc = app_main();
			if(rc < 0) app_valid = FALSE;
		}
	}
}

void system_status_handler(union ty_status status)
{
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("status_handler()\n\r");
#endif
}

/*
 * Expiry Function if SYS_SW_TIMERS defined
 */
#ifdef SYS_SW_TIMERS
static void expiry(timer_id timer, union sigval data)
{
	result_t  rc;
	can_frame frame;

	frame.can_id = 0x555;
	frame.can_dlc = 0x00;

	rc = can_l2_tx_frame(&frame);
	RC_CHECK_PRINT("Failed to send CAN Frame\n\r");
}
#endif // SYS_SW_TIMERS

static void frame_handler(can_frame *frame)
{
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("handle(0x%lx)\n\r", frame->can_id);
#endif
}
