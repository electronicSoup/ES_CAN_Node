/**
 * @file application/byte_input.c
 *
 * @author John Whitmore
 * 
 * @brief Application entry points for a Byte Input Board
 *
 * Copyright 2018 electronicSoup
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
 */
#include "libesoup_config.h"
#ifdef SYS_SERIAL_LOGGING
#define DEBUG_FILE
static const char *TAG = "BYTI";
#include "libesoup/logger/serial_log.h"
#endif
#include "libesoup/errno.h"
#include "libesoup/gpio/gpio.h"
#include "libesoup/comms/can/can.h"
#include "libesoup/status/status.h"

#define NUM_INPUTS   1

static uint8_t   io_address;

result_t app_init(uint8_t address, status_handler_t handler)
{
	can_frame                 frame;
	union byte_53u8_status    byte_data;
	
	LOG_D("app_init(0x%x)\n\r", address);

	io_address = address;
	
	/*
	 * Create a frame with the current status of the inputs
	 */
	frame.can_id                   = BYTE_53U8_INPUT_STATUS;
	frame.can_dlc                  = NUM_INPUTS * 2;
	byte_data.bitfield.io_node   = io_address;
        byte_data.bitfield.byte      = 0x00;
        byte_data.bitfield.channel   = 0x00;
        
        frame.data[0]                = byte_data.bytes[0];
        frame.data[1]                = byte_data.bytes[1];

	/*
	 * Send the initial state out on the CAN Bus
	 */
	return(can_l2_tx_frame(&frame));
}

result_t app_main(void)
{
	return(0);
}
