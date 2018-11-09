/**
 * @file dummy_app.c
 *
 * @author John Whitmore
 * 
 * @brief Application entry points for Switch Output board
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
static const char *TAG = "SWO";
#include "libesoup/logger/serial_log.h"
#endif
#include "libesoup/errno.h"
#include "libesoup/gpio/gpio.h"
#include "libesoup/comms/can/can.h"
#include "libesoup/comms/can/es_control/es_control.h"
#include "libesoup/status/status.h"
#ifndef SYS_CAN_BUS
#include "libesoup/timers/sw_timers.h"
#endif

static uint8_t   io_address;

#ifdef SYS_CAN_BUS
void switch_output_status(can_frame *frame)
{
	result_t          rc;
	uint8_t           loop;
	union bool_431    es_bool;
	
	LOG_D("\n\r");
	for(loop = 0; loop < frame->can_dlc; loop++) {
		es_bool.byte = frame->data[loop];
		LOG_D("\t[%d] %d-%d\n\r", loop, es_bool.bitfield.chan, es_bool.bitfield.es_bool);
		
		if(es_bool.bitfield.node == io_address) {
			rc = gpio_set(RD0 + es_bool.bitfield.chan, GPIO_MODE_DIGITAL_OUTPUT, es_bool.bitfield.es_bool);
			RC_CHECK_PRINT_VOID("gpio_set")
		}
	}
}
#endif

#ifdef SYS_CAN_BUS
void switch_output_rtr(can_frame *rx_frame)
{
	result_t                  rc;
	uint8_t                   loop;
	union bool_431            es_bool;
	union es_control_id       es_ctrl_id;
	can_frame                 tx_frame;

	es_ctrl_id.word = 0x0000;
	es_ctrl_id.fields.priority = priority_3;
	es_ctrl_id.fields.es_type  = bool_431_output;
	tx_frame.can_id  = es_ctrl_id.word;
	tx_frame.can_dlc = 0;
	
	for(loop = 0; loop < rx_frame->can_dlc; loop++) {
		es_bool.byte = rx_frame->data[loop];
		
		if(es_bool.bitfield.node == io_address) {
			rc = gpio_get(RD0 + es_bool.bitfield.chan);
			RC_CHECK_PRINT_VOID("gpio_get")
			es_bool.bitfield.es_bool  = rc;
			tx_frame.data[tx_frame.can_dlc++] = es_bool.byte;
		}
	}
	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("can_tx")
	}
}
#endif

#ifndef SYS_CAN_BUS
void toggle(timer_id timer, union sigval data)
{
	result_t rc;
	
	LOG_D("toggle\n\r");
	
	LATDbits.LATD0 = ~PORTDbits.RD0;
	
}
#endif

result_t app_init(uint8_t address, status_handler_t handler)
{
	result_t               rc;
	uint8_t                loop;
	can_l2_target_t        target;

	LOG_D("app_init(0x%x)\n\r", address);	
	io_address = address;

	/*
	 * Set the GPIO of the output pins
	 */
	for(loop = RD0; loop < RD8; loop++) {
		rc = gpio_set(loop, GPIO_MODE_DIGITAL_OUTPUT, 0);
	}

	/*
	 * Register a CAN Frame handler for the status_request frame
	 */
	target.filter  = es_rtr_mask | bool_431_output;
	target.mask    = es_rtr_mask | es_type_mask;
	target.handler = switch_output_rtr;
	rc = frame_dispatch_reg_handler(&target);
	RC_CHECK

	/*
	 * Register a CAN Frame handler for the status update frame
	 */
	target.filter  = bool_431_output;
	target.mask    = es_rtr_mask | es_type_mask;
	target.handler = switch_output_status;
	return(frame_dispatch_reg_handler(&target));
}

result_t app_main(void)
{
	return(0);
}
