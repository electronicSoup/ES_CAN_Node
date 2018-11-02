/**
 * @file main.c
 *
 * @author John Whitmore
 * 
 * @brief main entry point for the CAN Node
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
#include "libesoup/gpio/gpio.h"
#include "libesoup/timers/sw_timers.h"
#include "libesoup/timers/delay.h"
#include "libesoup/comms/can/can.h"
#ifdef SYS_CAN_DCNCP
#include "libesoup/comms/can/dcncp/dcncp_can.h"
#endif

#ifdef SYS_EEPROM
#include "libesoup/hardware/eeprom.h"
#endif
#include "libesoup/status/status.h"

#include "app.h"

static boolean   can_connected = FALSE;
static boolean   app_valid     = FALSE;

static uint8_t          io_address;
#ifdef SYS_CAN_BUS
static can_baud_rate_t  baud_rate;
#endif
#ifdef SYS_CAN_ISO15765
static uint8_t          l3_address;
#endif // SYS_CAN_ISO15765

void system_status_handler(status_source_t source, int16_t status, int16_t data);

#ifdef SYS_CAN_BUS
static void frame_handler(can_frame *);
#endif

/*
 * Device handle for the connected USB Device
 */
//static void* device_handle = NULL;

int main(void)
{
	boolean          watchdog = FALSE;
	uint8_t          node_status;
	result_t         rc = 0;
	struct period    period = {mSeconds, 500};
#ifdef SYS_CAN_BUS
	can_l2_target_t  target;
#endif
	libesoup_init();
	
	can_connected = FALSE;
        node_status   = 0x00;

	delay(&period);
	LOG_D("************************\n\r");
	LOG_D("***   CAN Bus Node   ***\n\r");
	LOG_D("***   %ldMHz         ***\n\r", sys_clock_freq);
	LOG_D("************************\n\r");

	rc = eeprom_read(EEPROM_NODE_IO_ADDRESS);
	RC_CHECK_PRINT_CONT("EEPROM Read")
	io_address = (uint8_t)rc;
	if(io_address == 0xff) {
		LOG_D("Writing a new node address\n\r");
		rc = eeprom_write(EEPROM_NODE_IO_ADDRESS, 0x01);
		RC_CHECK_PRINT_CONT("EEPROM Write")
		io_address = 0x01;
	}
	LOG_D("Node IO Address 0x%x\n\r", io_address);
	if(watchdog) {
		LOG_E("Watch Dog Timed out!\n\r");
		app_valid = FALSE;

		rc = eeprom_write(EEPROM_NODE_STATUS_ADDR, node_status & ~(NODE_STATUS_APP_VALID));
		RC_CHECK_PRINT_CONT("Failed to write to EEPROM\n\r");
	} else {
		LOG_D("Application assumed good!\n\r");
		rc = eeprom_read(EEPROM_NODE_STATUS_ADDR);
		if(rc >= 0) {
			node_status = (uint8_t)rc;
			LOG_D("Node Status 0x%x\n\r", node_status);
			app_valid = node_status & NODE_STATUS_APP_VALID;
		}
	}
	rc = delay(&period);
	RC_CHECK_PRINT_CONT("Failed to delay\n\r");

#ifdef SYS_CAN_BUS
	baud_rate = eeprom_read(EEPROM_NODE_CAN_BAUD_RATE_ADDR);
        if( (baud_rate >= no_baud) || (baud_rate < 0) ) {
		baud_rate = baud_250K;
		LOG_W("No CAN Baud Rate set so storing 250KBit/s\n\r");
		rc = eeprom_write(EEPROM_NODE_CAN_BAUD_RATE_ADDR, baud_rate);
		RC_CHECK_PRINT_CONT("Failed to write to EEPROM\n\r");
        }
#endif
#ifdef SYS_CAN_ISO15765
	rc = eeprom_read(EEPROM_NODE_L3_ADDRESS);
	RC_CHECK_PRINT_CONT("EEPROM Read")
	l3_address = (uint8_t)rc;
#endif
	rc = delay(&period);
	RC_CHECK_PRINT_CONT("Failed to delay()\n\r");
#ifdef SYS_CAN_BUS
#ifdef SYS_CAN_ISO15765
 	rc = can_init(baud_rate, l3_address, system_status_handler, normal);
	RC_CHECK_PRINT_CONT("Failed to initialise CAN Bus\n\r");
#else
 	rc = can_init(baud_rate, system_status_handler, normal);
	RC_CHECK_PRINT_CONT("Failed to initialise CAN Bus\n\r");
#endif  // SYS_CAN_ISO15765
#endif
	asm ("CLRWDT");
	
	/*
	 * Register a frame handler
	 */
#ifdef SYS_CAN_BUS
	target.filter = 0x555;
	target.mask   = CAN_SFF_MASK;
	target.handler = frame_handler;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK_PRINT_CONT("Failed to register frame handler\n\r");
#endif
#ifndef SYS_CAN_BUS
	if(app_valid) {
		LOG_D("Call App Init as Application is valid\n\r");
		rc = app_init(io_address, system_status_handler);
		if(rc < 0) app_valid = FALSE;
	}
#endif
	/*
	 * Enter the main loop
	 */
	LOG_D("Entering the main loop\n\r");
	LOG_D("***   %ldMHz         ***\n\r", sys_clock_freq);
	while(TRUE) {
		libesoup_tasks();
                asm ("CLRWDT");

#ifdef SYS_CAN_BUS
		if (app_valid && can_connected) {
			rc = app_main();
			if(rc < 0) app_valid = FALSE;
		}
#else
		if (app_valid) {
			rc = app_main();
			if(rc < 0) app_valid = FALSE;
		}
#endif
	}
}

void system_status_handler(status_source_t source, int16_t status, int16_t data)
{
	result_t rc  __attribute__((unused));
	
	LOG_D("status_handler()\n\r");
	switch(source) {
#ifdef SYS_CAN_BUS
	case can_bus_l2_status:
		switch(status) {
		case can_l2_detecting_baud:
			LOG_D("Bit Rate Auto Detect\n\r");
			break;
		case can_l2_connecting:
			LOG_D("Connecting\n\r");
			break;
		case can_l2_connected:
			LOG_D("Connected - %s\n\r", can_baud_rate_strings[data]);
			can_connected = TRUE;
			if(app_valid) {
				LOG_D("Call App Init as Application is valid\n\r");
				rc = app_init(io_address, system_status_handler);
				if(rc < 0) app_valid = FALSE;
			} else {
				LOG_E("App is not valid\n\r");
			}

			break;
		default:
			LOG_E("Status? %d\n\r", status);
			break;
		}
		break;
#endif
#if defined(SYS_CAN_DCNCP)
	case can_bus_dcncp_status:
		switch(status) {
		case can_dcncp_l3_address_registered:
			LOG_D("CAN L3 Address registered 0x%x\n\r", (uint8_t)data);
			if (l3_address != (uint8_t)data) {
				l3_address = (uint8_t)data;
				rc = eeprom_write(EEPROM_NODE_L3_ADDRESS, l3_address);
			}
			break;
		}
		break;
#endif
#if defined(SYS_CAN_ISO15765)
	case iso15765_status:
		CAN_LOG_D("Testing");
		break;
#endif
#if defined(ISO11783)
	case iso11783_status:
		break;
#endif
	default:
		LOG_E("Status Src? %d\n\r", source);
	}
}

#ifdef SYS_CAN_BUS
static void frame_handler(can_frame *frame)
{
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("handle(0x%lx)\n\r", frame->can_id);
#endif
}
#endif
