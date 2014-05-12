/**
 *
 * \file os.c
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

#define DEBUG_FILE
#include "es_lib/logger/serial.h"
#undef DEBUG_FILE
#include "os_api.h"

#define TAG "OS"

BOOL application_invalid;

typedef struct {
    BOOL active;
    expiry_function function;
} os_timer_t;

os_timer_t os_timers[NUMBER_OF_TIMERS];

void os_init_data(void)
{
    u16 loop;

    for(loop = 0; loop < NUMBER_OF_TIMERS; loop++) {
        os_timers[loop].active = FALSE;
    }

}

void exp_os_timer(timer_t timer_id, union sigval data)
{
    if(os_timers[timer_id].active) {
        os_timers[timer_id].active = FALSE;
        os_timers[timer_id].function(timer_id, data);
    } else {
        DEBUG_E("Inactive timer expired!");
    }
}

/*
** start_timer
 */
result_t os_start_timer(UINT16 ticks,
        expiry_function function,
        union sigval data,
        es_timer *timer)
{
    start_timer(ticks, exp_os_timer, data, timer);

    os_timers[timer->timer_id].function = function;
}

/*
** os_cancel_timer
*/
result_t os_cancel_timer(es_timer *timer)
{
    os_timers[timer->timer_id].active = FALSE;
    cancel_timer(timer);
}

/*
** os_eeprom_read
*/
result_t os_eeprom_read(UINT16 address, BYTE *data)
{

}

/*
** os_eeprom_write
*/
result_t os_eeprom_write(UINT16 address, BYTE data)
{

}

/*
** os_l2_tx_frame
*/
result_t os_l2_tx_frame()
{

}

/*
** os_l2_reg_handler
*/
result_t os_l2_reg_handler()
{

}

/*
** os_l2_unreg_handler
*/
result_t os_l2_unreg_handler()
{

}

/*
** os_l3_get_address
*/
result_t os_l3_get_address()
{

}

/*
** os_l3_tx_msg
*/
result_t os_l3_tx_msg()
{

}

/*
** os_l3_reg_handler
*/
result_t os_l3_reg_handler()
{

}

/*
** os_l3_unreg_handler
*/
result_t os_l3_unreg_handler()
{

}

/*
** os_node_log
*/
result_t os_node_log()
{
    DEBUG_D("os_node_log\n\r");
}

/*
** os_net_log
*/
result_t os_net_log()
{

}

/*
** os_net_reg_handler
*/
result_t os_net_reg_handler()
{

}

/*
** os_net_unreg_handler
*/
result_t os_net_unreg_handler()
{

}

/*
** os_invalidate_app
*/
result_t os_invalidate_app_isr(void)
{
    eeprom_write(APP_VALID_MAGIC, 0x00);
    eeprom_write(APP_VALID_MAGIC + 1, 0x00);
    application_invalid |= APP_ISR_INVALID;
}
