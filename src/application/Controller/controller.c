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
#include "libesoup/comms/can/es_control/es_control.h"
#include "libesoup/status/status.h"
#include "libesoup/timers/sw_timers.h"


static uint8_t   node_address;

void node1_chan0_bool431_input(uint8_t data)
{
	result_t               rc;
	union bool_431         es_bool_in;
	union bool_431         es_bool_out;
	uint8_t                loop;
	can_frame              tx_frame;
	union es_control_id    es_ctrl_id;

	es_bool_in.byte  = data;

	/*
	 * Create a frame with the current status of the inputs
	 */
	es_ctrl_id.word = 0;
	es_ctrl_id.fields.es_type = bool_431_output;
	
	tx_frame.can_id  = es_ctrl_id.word;
	tx_frame.can_dlc = 0;
	
	if (es_bool_in.bitfield.es_bool) {
//		es_bool_out.bitfield.node = 0x01;
		es_bool_out.bitfield.es_bool = 0b1;

		for (loop = 0; loop < 8; loop++) {
			es_bool_out.bitfield.node = 0x01;
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}

			es_bool_out.bitfield.node = 0x02;
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}
		}
	} else {
//		es_bool_out.bitfield.node = 0x01;
		es_bool_out.bitfield.es_bool = 0b0;

		for (loop = 0; loop < 8; loop++) {
			es_bool_out.bitfield.node = 0x01;
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}
			
			es_bool_out.bitfield.node = 0x02;
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}
		}					
	}

	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("CAN Tx\n\r");
		tx_frame.can_dlc = 0;
	}
}

void node1_chan1_bool431_input(uint8_t data)
{
	result_t               rc;
	union bool_431         es_bool_in;
	union bool_431         es_bool_out;
	uint8_t                loop;
	can_frame              tx_frame;
	union es_control_id    es_ctrl_id;

	es_bool_in.byte  = data;

	/*
	 * Create a frame with the current status of the inputs
	 */
	es_ctrl_id.word = 0;
	es_ctrl_id.fields.es_type = bool_431_output;
	
	tx_frame.can_id  = es_ctrl_id.word;
	tx_frame.can_dlc = 0;
	
	if (es_bool_in.bitfield.es_bool) {
		es_bool_out.bitfield.node = 0x02;
		es_bool_out.bitfield.es_bool = 0b1;

		for (loop = 0; loop < 8; loop++) {
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}
		}
	} else {
		es_bool_out.bitfield.node = 0x02;
		es_bool_out.bitfield.es_bool = 0b0;

		for (loop = 0; loop < 8; loop++) {
			es_bool_out.bitfield.chan = loop;
			tx_frame.data[tx_frame.can_dlc++] = es_bool_out.byte;

			if(tx_frame.can_dlc == 8) {
				rc = can_l2_tx_frame(&tx_frame);
				RC_CHECK_PRINT_VOID("CAN Tx\n\r");
				tx_frame.can_dlc = 0;
			}
		}					
	}

	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("CAN Tx\n\r");
		tx_frame.can_dlc = 0;
	}
}

void node1_bool431_input(uint8_t data)
{
	union bool_431         es_bool_in;

	es_bool_in.byte = data;
	
	if (es_bool_in.bitfield.chan == 0x00) {
		node1_chan0_bool431_input(data);
	} else if (es_bool_in.bitfield.chan == 0x01) {
		node1_chan1_bool431_input(data);
	}
}

void process_bool431_input(can_frame *rx_frame)
{
	uint8_t                loop;
	union bool_431         es_bool_in;

	for (loop = 0; loop < rx_frame->can_dlc; loop++) {
		es_bool_in.byte = rx_frame->data[loop];
		LOG_D("Input 0x%x:0x%x:0x%x\n\r", es_bool_in.bitfield.node, es_bool_in.bitfield.chan, es_bool_in.bitfield.es_bool);
		
		if (es_bool_in.bitfield.node == 0x01) {
			node1_bool431_input(es_bool_in.byte);
		}
	}
}

result_t app_init(uint8_t address, status_handler_t handler)
{
	can_l2_target_t        target;
	union es_control_id    es_ctrl_id;


	LOG_D("Master app_init(0x%x)\n\r", address);	
	node_address = address;

	/*
	 * Register a CAN Frame handler for the Switch (43) Input frames
	 */
	es_ctrl_id.word = 0;
	es_ctrl_id.fields.es_type = bool_431_input;
	target.filter  = es_ctrl_id.word;
	target.mask    = es_type_mask;
	target.handler = process_bool431_input;
	return(frame_dispatch_reg_handler(&target));
}

result_t app_main(void)
{
	return(0);
}
