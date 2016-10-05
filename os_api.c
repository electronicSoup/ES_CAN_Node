/**
 *
 * \file os_api.c
 *
 * Functions provided by the OS to the Application.
 *
 * Copyright 2014 John Whitmore <jwhitmore@electronicsoup.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 2 of the GNU General Public License
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
#include "system.h"
#include <stdio.h>
#include <string.h>

#include "es_lib/timers/timers.h"
#include "es_lib/utils/eeprom.h"
#include "es_lib/can/es_can.h"
#include "es_lib/can/dcncp/dcncp_can.h"
#include "es_lib/logger/iso15765_log.h"
#include "es_lib/utils/flash.h"

#define DEBUG_FILE
#include "es_lib/logger/serial_log.h"
#include "os_api.h"

static result_t os_clear_app_eeprom(void);
static result_t os_cancel_all_timers(void);
static void os_clear_all_can_l2_handlers(void);
static void os_clear_all_iso15765_handlers(void);

#define TAG "OS"

BOOL app_valid;

/*
 * Have to keep a track of timers being used by the applicaton
 * in order to cancel them if the APP is uninstalled.
 */
typedef struct {
	BOOL active;
	expiry_function function;
} os_timer_t;

os_timer_t os_timers[NUMBER_OF_TIMERS];

/*
 * Have to keep a track of CAN Layer 2 handers being used by the applicaton
 * in order to remove them if the APP is uninstalled.
 */
typedef struct {
	u8 active;
} os_can_l2_handler;

#ifdef CAN
os_can_l2_handler os_can_l2_handlers[CAN_L2_HANDLER_ARRAY_SIZE];
#endif // CAN

/*
 * Have to keep a track of ISO-15765 handers being used by the applicaton
 * in order to remove them if the APP is uninstalled.
 */
typedef struct {
	u8 active;
} os_iso15765_handler;

#if defined(ISO15765)
os_iso15765_handler os_iso15765_handlers[ISO15765_REGISTER_ARRAY_SIZE];
#endif // ISO15765

/*
 * Have to keep a track of ISO-11783 handers being used by the applicaton
 * in order to remove them if the APP is uninstalled.
 */
typedef struct {
	u8 active;
} os_iso11783_handler;

#if defined(ISO11783)
os_iso11783_handler os_iso11783_handlers[ISO11783_REGISTER_ARRAY_SIZE];

static void os_clear_all_iso11783_handlers();
#endif // ISO15765

void os_init_data(void)
{
	u16 loop;

	for (loop = 0; loop < NUMBER_OF_TIMERS; loop++) {
		os_timers[loop].active = FALSE;
	}

#ifdef CAN
	for (loop = 0; loop < CAN_L2_HANDLER_ARRAY_SIZE; loop++) {
		os_can_l2_handlers[loop].active = 0x00;
	}
#endif // CAN

#if defined(ISO15765)
	for (loop = 0; loop < ISO15765_REGISTER_ARRAY_SIZE; loop++) {
		os_iso15765_handlers[loop].active = 0x00;
	}
#endif // ISO15765
}

void os_remove_current_app(void)
{
	os_cancel_all_timers();
	os_clear_app_eeprom();
	os_clear_all_can_l2_handlers();
	os_clear_all_iso15765_handlers();
	os_clear_all_iso11783_handlers();
}

void exp_os_timer(timer_t timer_id, union sigval data)
{
	if (os_timers[timer_id].active) {
		os_timers[timer_id].active = FALSE;
		os_timers[timer_id].function(timer_id, data);
	} else {
		LOG_E("Inactive timer expired!\n\r");
	}
}

/*
** start_timer
 */
result_t os_timer_start(u16 ticks,
	expiry_function function,
        union sigval data,
        es_timer *timer)
{
	result_t rc;

	LOG_D("os_timer_start()\n\r");

	rc = timer_start(ticks, exp_os_timer, data, timer);

	if(rc == SUCCESS) {
		os_timers[timer->timer_id].active = TRUE;
		os_timers[timer->timer_id].function = function;
	}

	return(rc);
}

/*
** os_timer_cancel
*/
result_t os_timer_cancel(es_timer *timer)
{
	LOG_D("os_timer_cancel()\n\r");
	os_timers[timer->timer_id].active = FALSE;
	return(timer_cancel(timer));
}

/*
** os_cancel_all_timers
*/
static result_t os_cancel_all_timers(void)
{
	u8 loop;
	es_timer timer;

	LOG_D("os_cancel_all_timers()\n\r");

	for (loop = 0; loop < NUMBER_OF_TIMERS; loop++) {
		if (os_timers[loop].active) {
			os_timers[loop].active = FALSE;
			timer.status = ACTIVE;
			timer.timer_id = loop;

			timer_cancel(&timer);
		}
	}
	return (SUCCESS);
}

/*
** os_eeprom_read
*/
result_t os_eeprom_read(u16 address, u8 *data)
{
	u16 eeprom_address;

	eeprom_address = APP_EEPROM_START + address;
	if (eeprom_address <= EEPROM_MAX_ADDRESS) {
		return (eeprom_read(eeprom_address, data));
	} else {
		return (ERR_ADDRESS_RANGE);
	}
	return(SUCCESS);
}

/*
** os_eeprom_write
*/
result_t os_eeprom_write(u16 address, u8 data)
{
	u16 eeprom_address;

	eeprom_address = APP_EEPROM_START + address;
	if (eeprom_address <= EEPROM_MAX_ADDRESS) {
		return (eeprom_write(eeprom_address, data));
	} else {
		return (ERR_ADDRESS_RANGE);
	}
	return(SUCCESS);
}

result_t os_clear_app_eeprom(void)
{
	LOG_D("os_clear_app_eeprom()\n\r");

	return(eeprom_erase(APP_EEPROM_START));
}

/*
** os_can_l2_tx_frame
*/
result_t os_can_l2_tx_frame(can_frame * frame)
{
	LOG_D("os_can_l2_tx_frame()\n\r");
#ifdef CAN
	return(can_l2_tx_frame(frame));
#else
	return(SUCCESS);
#endif // CAN
}

/*
** os_can_l2_dispatch_reg_handler
*/
result_t os_can_l2_dispatch_reg_handler(can_l2_target_t *target)
{
	result_t rc = SUCCESS;

	LOG_D("os_can_l2_dispatch_reg_handler()\n\r");
#ifdef CAN
	rc = can_l2_dispatch_reg_handler(target);
	if(rc == SUCCESS) {
		os_can_l2_handlers[target->handler_id].active = 0x01;
	} else {
		LOG_E("Failed to register CAN L2 Handler\n\r");
	}
#endif // CAN
	return (rc);
}

/*
** os_can_l2_dispatch_unreg_handler
*/
result_t os_can_l2_dispatch_unreg_handler(u8 handler_id)
{
	result_t rc = SUCCESS;

	LOG_D("os_can_l2_unreg_handler()\n\r");
#ifdef CAN
	if(handler_id >= CAN_L2_HANDLER_ARRAY_SIZE) {
		return(ERR_BAD_INPUT_PARAMETER);
	}
	rc = can_l2_dispatch_unreg_handler(handler_id);
	if(rc == SUCCESS) {
		os_can_l2_handlers[handler_id].active = 0x00;
	}
#endif // CAN
	return (rc);
}

result_t os_can_l2_dispatch_set_unhandled_handler(can_l2_frame_handler_t handler)
{
	LOG_D("os_can_l2_dispatch_set_unhandled_handler()\n\r");
	return(can_l2_dispatch_set_unhandled_handler(handler));
}

void os_clear_all_can_l2_handlers()
{
#ifdef CAN
	u8 loop;

	for (loop = 0; loop < CAN_L2_HANDLER_ARRAY_SIZE; loop++) {
		if(os_can_l2_handlers[loop].active) {
			can_l2_dispatch_unreg_handler(loop);
			os_can_l2_handlers[loop].active = 0x00;
		}
	}

	can_l2_dispatch_set_unhandled_handler((can_l2_frame_handler_t)NULL);
#endif //CAN
}

/*
** os_dcncp_get_node_address
*/
u8 os_dcncp_get_node_address()
{
	LOG_D("os_dcncp_get_node_address()\n\r");
#if defined(ISO15765) || defined(ISO11783)
	return(dcncp_get_node_address());
#else
	return(BROADCAST_NODE_ADDRESS);
#endif // ISO15765 || ISO11783
}

/*
** os_iso15765_tx_msg
*/
result_t os_iso15765_tx_msg(iso15765_msg_t *msg)
{
	LOG_D("os_iso15765_tx_msg()\n\r");
#if defined(ISO15765)
	return(iso15765_tx_msg(msg));
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso15765_dispatch_reg_handler
 */
result_t os_iso15765_dispatch_reg_handler(iso15765_target_t *target)
{
#if defined(ISO15765)
	result_t rc;
#endif

	LOG_D("os_iso15765_dispatch_reg_handler()\n\r");
#if defined(ISO15765)
	rc = iso15765_dispatch_reg_handler(target);
	if(rc == SUCCESS) {
		os_iso15765_handlers[target->handler_id].active = 0x01;
	}
	return(rc);
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso15765_dispatch_unreg_handler
 */
result_t os_iso15765_dispatch_unreg_handler(u8 id)
{
#if defined(ISO15765)
	result_t rc;
#endif

	LOG_D("os_iso157655_reg_handler()\n\r");
#if defined(ISO15765)
	rc = iso15765_dispatch_unreg_handler(id);
	if(rc == SUCCESS) {
		os_iso15765_handlers[id].active = 0x00;
	}
	return(rc);
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso15765_dispatch_set_unhandled_handler
 */
result_t os_iso15765_dispatch_set_unhandled_handler(iso15765_msg_handler_t handler)
{
	LOG_D("os_iso157655_dispatch_set_unhandled_handler()\n\r");
#if defined(ISO15765)
	return(iso15765_dispatch_set_unhandled_handler(handler));
#else
	return (SUCCESS);
#endif
}

static void os_clear_all_iso15765_handlers()
{
#if defined(ISO15765)
	u8 loop;

	for (loop = 0; loop < ISO15765_REGISTER_ARRAY_SIZE; loop++) {
		if(os_iso15765_handlers[loop].active) {
			iso15765_dispatch_unreg_handler(loop);
			os_iso15765_handlers[loop].active = 0x00;
		}
	}

	iso15765_dispatch_set_unhandled_handler((iso15765_msg_handler_t)NULL);
#endif // ISO15765
}

/*
** os_iso11783_tx_msg
*/
result_t os_iso11783_tx_msg(iso11783_msg_t *msg)
{
	LOG_D("os_iso11783_tx_msg()\n\r");
#if defined(ISO11783)
	return(iso11783_tx_msg(msg));
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso11783_dispatch_reg_handler
 */
result_t os_iso11783_dispatch_reg_handler(iso11783_target_t *target)
{
#if defined(ISO11783)
	result_t rc;
#endif

	LOG_D("os_iso11783_dispatch_reg_handler()\n\r");
#if defined(ISO11783)
	rc = iso11783_dispatch_reg_handler(target);
	if(rc == SUCCESS) {
		os_iso11783_handlers[target->handler_id].active = 0x01;
	}
	return(rc);
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso11783_dispatch_unreg_handler
 */
result_t os_iso11783_dispatch_unreg_handler(u8 id)
{
#if defined(ISO11783)
	result_t rc;
#endif

	LOG_D("os_iso11783_reg_handler()\n\r");
#if defined(ISO11783)
	rc = iso11783_dispatch_unreg_handler(id);
	if(rc == SUCCESS) {
		os_iso11783_handlers[id].active = 0x00;
	}
	return(rc);
#else
	return (SUCCESS);
#endif
}

/*
 ** os_iso15765_dispatch_set_unhandled_handler
 */
result_t os_iso11783_dispatch_set_unhandled_handler(iso11783_msg_handler_t handler)
{
	LOG_D("os_iso11783_dispatch_set_unhandled_handler()\n\r");
#if defined(ISO11783)
	return(iso11783_dispatch_set_unhandled_handler(handler));
#else
	return (SUCCESS);
#endif
}

static void os_clear_all_iso11783_handlers()
{
#if defined(ISO11783)
	u8 loop;

	for (loop = 0; loop < ISO11783_REGISTER_ARRAY_SIZE; loop++) {
		if(os_iso11783_handlers[loop].active) {
			iso11783_dispatch_unreg_handler(loop);
			os_iso11783_handlers[loop].active = 0x00;
		}
	}

	iso11783_dispatch_set_unhandled_handler((iso11783_msg_handler_t)NULL);
#endif // ISO11783
}

/*
** os_serial_log
*/
result_t os_serial_log(log_level_t level, char* tag, char* fmt, ...)
{
	va_list args;
	u16 psv_page;
	auto char colon[] = ":";

	switch (level) {
		case Debug:
			printf("App D :");
			break;

		case Info:
			printf("App I :");
			break;

		case Warning:
			printf("App W :");
			break;

		case Error:
			printf("App*E*:");
			break;
	}

	CORCONbits.PSV = 1;
	psv_page = PSVPAG;
	PSVPAG = 0x03;

	printf(tag);
	printf(colon);

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	PSVPAG = psv_page;
	return (SUCCESS);
}

/*
** os_iso15765_log
*/
void os_iso15765_log(log_level_t level, char *string)
{
	LOG_D("os_iso15765_log\n\r");
#if defined(ISO15765_LOGGING)
	iso15765_log(level, string);
#endif
}

/*
 * os_get_io_address
 */
result_t os_get_io_address(u8 *address)
{
	LOG_D("os_get_io_address()\n\r");
	eeprom_read(EEPROM_IO_ADDRESS_ADDR, address);
	return (SUCCESS);
}

result_t os_flash_strcpy(char *dst, __prog__ char *src, u16 *length)
{
	LOG_D("os_flash_strcpy()\n\r");
	return(flash_strcpy(dst, src, length));
}
