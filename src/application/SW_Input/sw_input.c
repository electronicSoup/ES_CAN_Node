/**
 * @file dummy_app.c
 *
 * @author John Whitmore
 * 
 * @brief Application entry points for Switch Input Board
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
static const char *TAG = "SWI";
#include "libesoup/logger/serial_log.h"
#endif
#include "libesoup/errno.h"
#include "libesoup/gpio/gpio.h"
#include "libesoup/comms/can/can.h"
#include "libesoup/status/status.h"

#include "es_tpp.h"

#define BOUNCE_LIMIT 4
#define NUM_INPUTS   4

static uint8_t   io_address;

struct sw_input {
	uint8_t   debounce_count:4;
	uint8_t   reported_state:1;
};

static struct sw_input input_switch[NUM_INPUTS];

void switch_input_status_req(can_frame *rx_frame)
{
	result_t                  rc;
	uint8_t                   loop;
	can_frame                 tx_frame;
	union switch_43_status    switch_data;

	/*
	 * Create a frame with the current status of the inputs
	 */
	tx_frame.can_id  = SWITCH_43_INPUT_STATUS;
	tx_frame.can_dlc = 0;
	
	for(loop = 0; loop < rx_frame->can_dlc; loop++) {
		switch_data.byte = rx_frame->data[loop];
		if(switch_data.bitfield.io_node == io_address) {
			switch_data.bitfield.status    = input_switch[switch_data.bitfield.channel].reported_state;
			tx_frame.data[tx_frame.can_dlc++] = switch_data.byte;
		}
	}

	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("Status resp\n\r");
	}
}


result_t app_init(uint8_t address, status_handler_t handler)
{
	result_t                  rc;
	uint8_t                   loop;
	can_l2_target_t           target;
	can_frame                 frame;
	union switch_43_status    switch_data;
	
	LOG_D("app_init(0x%x)\n\r", address);

	io_address = address;
	
	/*
	 * Register a CAN Frame handler for the status_request frame
	 */
	target.filter  = SWITCH_43_INPUT_STATUS_REQ;
	target.mask    = CAN_SFF_MASK;
	target.handler = switch_input_status_req;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK

	/*
	 * Create a frame with the current status of the inputs
	 */
	frame.can_id                   = SWITCH_43_INPUT_STATUS;
	frame.can_dlc                  = NUM_INPUTS;
	switch_data.bitfield.io_node   = io_address;
	
	/*
	 * Set the GPIO of the input pins
	 */
	for(loop = 0; loop < NUM_INPUTS; loop++) {
		rc = gpio_set(RD0 + loop, GPIO_MODE_DIGITAL_INPUT, 0);
		RC_CHECK
		input_switch[loop].debounce_count = 0;
		rc = gpio_get(RD0 + loop);
		RC_CHECK
		input_switch[loop].reported_state = rc;
		switch_data.bitfield.channel = loop;
		switch_data.bitfield.status  = input_switch[loop].reported_state;
		frame.data[loop]             = switch_data.byte;
	}

	/*
	 * Send the initial state out on the CAN Bus
	 */
	return(can_l2_tx_frame(&frame));
}

result_t app_main(void)
{
	result_t                  rc;
	uint8_t                   loop;
	can_frame                 frame;
	boolean                   current_state;
	union switch_43_status    switch_data;

	/*
	 * Create a frame with the current status of the inputs
	 */
	frame.can_id                  = SWITCH_43_INPUT_STATUS;
	frame.can_dlc                 = 0;
	switch_data.bitfield.io_node  = io_address;

	for(loop = 0; loop < NUM_INPUTS; loop++) {
		rc = gpio_get(RD0 + loop);
		RC_CHECK
		current_state = rc;
		if(input_switch[loop].reported_state != current_state) {
//			LOG_D("++\n\r");
			input_switch[loop].debounce_count++;
			
			if(input_switch[loop].debounce_count >= BOUNCE_LIMIT) {
//				LOG_D("Snd\n\r");
				input_switch[loop].reported_state = current_state;
				input_switch[loop].debounce_count = 0;
				switch_data.bitfield.channel      = loop;
				switch_data.bitfield.status       = current_state;
				frame.data[frame.can_dlc++]       = switch_data.byte;
				LOG_D("Status 0x%x:0x%x:0x%x\n\r", switch_data.bitfield.io_node, switch_data.bitfield.channel, switch_data.bitfield.status);
			}
		} else {
			input_switch[loop].debounce_count = 0;
		}

	}

	if(frame.can_dlc > 0) {
		return(can_l2_tx_frame(&frame));		
	}
	
	return(0);
}
