/**
 * @file master.c
 *
 * @author John Whitmore
 * 
 * @brief Master Application entry points
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
static const char *TAG = "Master";
#include "libesoup/logger/serial_log.h"
#endif
#include "libesoup/errno.h"
#include "libesoup/gpio/gpio.h"
#include "libesoup/comms/can/can.h"
#include "libesoup/status/status.h"

#include "es_tpp.h"

static uint8_t   io_address;

void switch_43_input_status(can_frame *rx_frame)
{
	result_t                  rc;
	uint8_t                   loop;
	union switch_43_status    switch_data;
	can_frame                 tx_frame;

	/*
	 * Create a frame with the current status of the inputs
	 */
	tx_frame.can_id  = SWITCH_43_OUTPUT_STATUS;
	tx_frame.can_dlc = 0;
	
	for(loop = 0; loop < rx_frame->can_dlc; loop++) {
		switch_data.byte = rx_frame->data[loop];
		LOG_D("Input 0x%x:0x%x:0x%x\n\r", switch_data.bitfield.io_node, switch_data.bitfield.channel, switch_data.bitfield.status);
		tx_frame.data[tx_frame.can_dlc++] = switch_data.byte;
		switch_data.bitfield.io_node      = 2;
		tx_frame.data[tx_frame.can_dlc++] = switch_data.byte;
	}

	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("CAN Tx\n\r");
	}
}

void analog_42_input_status(can_frame *rx_frame)
{
	result_t                  rc;
	can_frame                 tx_frame;
	union analog_42_status    analog_data;
	union switch_43_status    switch_data;

	/*
	 * Pull the data out of the received frame
	 */
	analog_data.bytes[0] = rx_frame->data[0];
	analog_data.bytes[1] = rx_frame->data[1];
	
	LOG_D("Analog 0x%x\n\r", analog_data.bitfield.value);

	/*
	 * Create a tx frame to light LED
	 */
	switch_data.bitfield.io_node = 1;
	switch_data.bitfield.channel = 0;

	if(analog_data.bitfield.value < 0x080) {
		switch_data.bitfield.status  = 0;
	} else {
		switch_data.bitfield.status  = 1;
	}

	tx_frame.can_id      = SWITCH_43_OUTPUT_STATUS;
	tx_frame.can_dlc     = 1;
	tx_frame.data[0]     = switch_data.byte;
	
	rc = can_l2_tx_frame(&tx_frame);
	RC_CHECK_PRINT_VOID("Tx!\n\r");
}

result_t app_init(uint8_t address, status_handler_t handler)
{
	result_t               rc;
	can_l2_target_t        target;

	LOG_D("Master app_init(0x%x)\n\r", address);	
	io_address = address;

	/*
	 * Register a CAN Frame handler for the Switch (43) Input frames
	 */
	target.filter  = SWITCH_43_INPUT_STATUS;
	target.mask    = CAN_SFF_MASK;
	target.handler = switch_43_input_status;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK

	/*
	 * Register a CAN Frame handler for the Analog (42) Input frames
	 */
	target.filter  = ANALOG_42_INPUT_STATUS;
	target.mask    = CAN_SFF_MASK;
	target.handler = analog_42_input_status;
	return(frame_dispatch_reg_handler(&target));
}

result_t app_main(void)
{
	return(0);
}
