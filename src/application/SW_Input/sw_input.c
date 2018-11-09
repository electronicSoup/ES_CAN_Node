/**
 * @file application/sw_input.c
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
#include "libesoup/comms/can/es_control/es_control.h"
#include "libesoup/status/status.h"

#define BOUNCE_LIMIT 4
#define NUM_INPUTS   4

static uint8_t   node_address;

struct sw_input {
	uint8_t   debounce_count:4;
	uint8_t   reported_state:1;
};

static struct sw_input input_switch[NUM_INPUTS];

#ifdef SYS_CAN_BUS
void switch_input_rtr(can_frame *rx_frame)
{
	result_t                  rc;
	uint8_t                   loop;
	can_frame                 tx_frame;
	union es_control_id       es_id;
	union bool_431            es_bool;

	/*
	 * Create a frame with the current status of the inputs
	 */
	es_id.fields.priority = priority_2;
	es_id.fields.rtr      = 0;
	es_id.fields.es_type  = bool_431_input;
	
	tx_frame.can_id  = es_id.word;
	tx_frame.can_dlc = 0;
	
	for(loop = 0; loop < rx_frame->can_dlc; loop++) {
		es_bool.byte = rx_frame->data[loop];
		if(es_bool.bitfield.node == node_address) {
			es_bool.bitfield.es_bool = input_switch[es_bool.bitfield.chan].reported_state;
			tx_frame.data[tx_frame.can_dlc++] = es_bool.byte;
		}
	}

	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("Status resp\n\r");
	}
}
#endif

result_t app_init(uint8_t address, status_handler_t handler)
{
	result_t                  rc;
	uint8_t                   loop;
#ifdef SYS_CAN_BUS
	can_l2_target_t           target;
	can_frame                 frame;
#endif
	union es_control_id       es_id;
	union bool_431            es_bool;

	LOG_D("app_init(0x%x)\n\r", address);

	node_address = address;
	
	/*
	 * Register a CAN Frame handler for the status_request frame
	 */
#ifdef SYS_CAN_BUS
	target.filter  = es_rtr_mask | bool_431_input;
	target.mask    = es_rtr_mask | es_type_mask;
	target.handler = switch_input_rtr;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK
#endif
	/*
	 * Create a frame with the current status of the inputs
	 */
#ifdef SYS_CAN_BUS
	es_id.word            = 0x0000;
	es_id.fields.priority = priority_3;
	es_id.fields.es_type  = bool_431_input;
	frame.can_id          = es_id.word;
	frame.can_dlc         = NUM_INPUTS;
#endif
	es_bool.bitfield.node   = node_address;
	
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
		es_bool.bitfield.chan   = loop;
		es_bool.bitfield.es_bool = ~input_switch[loop].reported_state;
#ifdef SYS_CAN_BUS
		frame.data[loop]        = es_bool.byte;
#endif
	}

	/*
	 * Send the initial state out on the CAN Bus
	 */
#ifdef SYS_CAN_BUS
	return(can_l2_tx_frame(&frame));
#else
	return(0);
#endif
}

result_t app_main(void)
{
	result_t                  rc;
	uint8_t                   loop;
#ifdef SYS_CAN_BUS
	can_frame                 frame;
#endif
	boolean                   current_state;
	union es_control_id       es_id;
	union bool_431            es_bool;

	/*
	 * Create a frame with the current status of the inputs
	 */
#ifdef SYS_CAN_BUS
	es_id.word            = 0x0000;
	es_id.fields.priority = priority_3;
	es_id.fields.es_type  = bool_431_input;
	frame.can_id          = es_id.word;
	frame.can_dlc         = 0;
#endif
	es_bool.bitfield.node  = node_address;

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
				es_bool.bitfield.chan             = loop;
				es_bool.bitfield.es_bool             = ~current_state;
#ifdef SYS_CAN_BUS
				frame.data[frame.can_dlc++]       = es_bool.byte;
#endif
				LOG_D("Status 0x%x:0x%x:0x%x 0x%x\n\r", es_bool.bitfield.node, es_bool.bitfield.chan, es_bool.bitfield.es_bool, (PORTD & 0x07));
			}
		} else {
			input_switch[loop].debounce_count = 0;
		}

	}
#ifdef SYS_CAN_BUS
	if(frame.can_dlc > 0) {
		return(can_l2_tx_frame(&frame));		
	}
#endif
	return(0);
}
