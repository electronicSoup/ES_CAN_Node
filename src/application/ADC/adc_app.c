/**
 * @file application/ADC/adc_app.c
 *
 * @author John Whitmore
 * 
 * @brief Application entry points for ADC test board
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
static const char *TAG = "ADC_APP";
#include "libesoup/logger/serial_log.h"
#endif
#include "libesoup/errno.h"
#include "libesoup/gpio/gpio.h"
#include "libesoup/gpio/adc.h"
#include "libesoup/timers/sw_timers.h"
#include "libesoup/comms/can/can.h"
#include "libesoup/status/status.h"

#include "es_tpp.h"

static uint8_t   io_address;
static uint16_t  last_sample = 0xffff;

void adc_result(enum pin_t pin, uint16_t sample)
{
	LOG_D("Result 0x%x\n\r", sample);
	if(last_sample != sample) {
		LOG_D("send frame\n\r");
		last_sample = sample;
	}
}

void sample(timer_id timer, union sigval data)
{
	result_t rc;
	
	LOG_D("sample\n\r");
	
	rc = adc_sample(RB0, adc_result);
	RC_CHECK_PRINT_VOID("adc_sample\n\r");
}

result_t app_init(uint8_t address, status_handler_t handler)
{
	struct timer_req       request;

	LOG_D("app_init(0x%x)\n\r", address);
	io_address = address;

	request.units = Seconds;
	request.duration = 10;
	request.exp_fn = sample;
	request.type = repeat;
	
	return(sw_timer_start(&request));
}

result_t app_main(void)
{
	return(0);
}
