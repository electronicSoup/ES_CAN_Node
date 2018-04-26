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
#include "libesoup/status/status.h"

#include "es_tpp.h"

static uint8_t   io_address;

void byte_output_status(can_frame *frame)
{
	uint8_t                   loop;
	union byte_53u8_status    byte_data;
	
        /*
         * We'd expect an even number of data bytes in this payload.
         * If it's not just return there's an error which will have to be
         * raised
         */
        if((frame->can_dlc) % 2 != 0) {
                /*
                 * Todo - Raise an error with the status Handler.
                 */
                return;
        }
        
	for(loop = 0; loop < (frame->can_dlc / 2); loop++) {
		byte_data.bytes[loop]     = frame->data[loop * 2];
		byte_data.bytes[loop + 1] = frame->data[(loop * 2) + 1];
		
		if(byte_data.bitfield.io_node == io_address) {
                        /*
                         * Set the output to the received value.
                         */
		}
	}
}

void byte_output_status_req(can_frame *rx_frame)
{
	result_t                  rc;
	uint8_t                   loop;
	union switch_43_status    rx_switch_data;
	union switch_43_status    tx_switch_data;
	can_frame                 tx_frame;
	
	tx_frame.can_id  = SWITCH_43_OUTPUT_STATUS_RESP;
	tx_frame.can_dlc = 0;
	tx_switch_data.bitfield.io_node = io_address;
	
	for(loop = 0; loop < rx_frame->can_dlc; loop++) {
		rx_switch_data.byte = rx_frame->data[loop];
		
		if(rx_switch_data.bitfield.io_node == io_address) {
			rc = gpio_get(RD0 + rx_switch_data.bitfield.channel);
			RC_CHECK_PRINT_VOID("gpio_get")
			tx_switch_data.bitfield.channel = rx_switch_data.bitfield.channel;
			tx_switch_data.bitfield.status  = rc;
			tx_frame.data[tx_frame.can_dlc++] = tx_switch_data.byte;
		}
	}
	if(tx_frame.can_dlc > 0) {
		rc = can_l2_tx_frame(&tx_frame);
		RC_CHECK_PRINT_VOID("can_tx")
	}
}


result_t app_init(uint8_t address, status_handler_t handler)
{
	can_l2_target_t        target;

	LOG_D("app_init(0x%x)\n\r", address);	
	io_address = address;

	/*
	 * Register a CAN Frame handler for the status update frame
	 */
	target.filter  = BYTE_53U8_OUTPUT_STATUS;
	target.mask    = CAN_SFF_MASK;
	target.handler = byte_output_status;
	return(frame_dispatch_reg_handler(&target));
}

result_t app_main(void)
{
	return(0);
}
