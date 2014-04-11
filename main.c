/**
 *
 * \file main.c
 *
 * \brief main entry point for the DongleNode project
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
#include "es_lib/core.h"
#include "system.h"
#define MAIN
#include "main.h"
#undef MAIN
#define DEBUG_FILE
#include "es_lib/logger/serial.h"
#undef DEBUG_FILE
#include "es_lib/timers/timer_sys.h"
#include "es_lib/utils/utils.h"
#include "es_lib/can/es_can.h"

// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
//
// See /opt/microchip/mplabc30/v3.30c/support/PIC24F/h/p24FJ64GB106.h
//
// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
#if defined(PIC24FJ256GB110)
_CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2) // Primary HS OSC with PLL, USBPLL /2
#elif defined(PIC24FJ256GB106)
_CONFIG2(FNOSC_FRCPLL & POSCMOD_NONE & OSCIOFNC_ON & PLL_96MHZ_ON & PLLDIV_NODIV & DISUVREG_OFF)  // CLOCK 16000000
#endif
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

#if LOG_LEVEL < NO_LOGGING
#define TAG "MAIN"
#endif

#if defined(CAN_LAYER_3)
static u8 l3_address = 1;
result_t get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
{
    DEBUG_E("Address error");
    while (1)
    {
    }
}

void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
{
    DEBUG_E("Stack error");
    while (1)
    {
    }
}


void err(char *string)
{
    DEBUG_E(string);
}

#ifdef TEST
//static void expiry(u8 *);
void status_handler(can_status_t status, baud_rate_t baud);
#endif

int main(void)
{
    baud_rate_t baudRate;
    result_t result;
    u8 l3_address;
#ifdef TEST
    BYTE test_byte;
    UINT16 loop;
#endif
    USB_HOST_POWER_PIN_DIRECTION = OUTPUT_PIN;

    serial_init();

#if LOG_LEVEL <= LOG_DEBUG
    serial_log(Debug, TAG, "************************\n\r");
    serial_log(Debug, TAG, "***   CAN Bus Node   ***\n\r");
    serial_log(Debug, TAG, "************************\n\r");
#endif

#ifdef HEARTBEAT
    HEARTBEAT_LED_DIRECTION = OUTPUT_PIN;
    heartbeat_off((BYTE *)NULL);
#endif

    init_timer();
    spi_init();

    /* ToDo sort out baudRate */
//    sys_eeprom_read(NETWORK_BAUD_RATE, (BYTE *) & baudRate);
    baudRate = no_baud;

#if defined(CAN_LAYER_3)
    result = get_l3_node_address(&l3_address);
#endif

    // Send in null we're not defining a default handler for messages
    // if nothing's regestered an interest we're just not interested
#if defined(CAN_LAYER_3)
    can_init(baud_10K, l3_address, status_handler);
#else
    can_init(baud_10K, status_handler);
#endif

    //    enable_interrupts();
#ifdef HEARTBEAT
    heartbeat_on(NULL);
#endif

    /*
     * Enter the main loop
     */
#if LOG_LEVEL <= LOG_DEBUG
    serial_log(Debug, TAG, "Entering the main loop\n\r");
#endif
    while(TRUE) {
        CHECK_TIMERS();

        canTasks();
#if 0
        /*
         * A bit of test code for testing that the EEPROM is reading and
         * Writing.
         */
#ifdef TEST
        loop++;
        
        if(loop == 0) {
            sys_eeprom_read(0x04, (BYTE *) & test_byte);
#if DEBUG_LEVEL <= LOG_DEBUG
            serial_log(Debug, TAG, "Test Read of EEPROM 0x%x\n\r", test_byte);
#endif
            test_byte++;
            sys_eeprom_write(0x04, test_byte);

            /*
             * Simple test code for the MCP2515 Ouput pins
             */
#if 0
#ifdef MCP2515_OUTPUT_0
            if ((test_byte - 1) & 0x01) {
                set_output_0(0x01);
            } else {
                set_output_0(0x00);
            }
#endif

#ifdef MCP2515_OUTPUT_1
            if ((test_byte - 1) & 0x02) {
                set_output_1(0x01);
            } else {
                set_output_1(0x00);
            }
#endif
#endif
        }
#endif
#endif
    }
}

#if defined(CAN_LAYER_3)
result_t get_l3_node_address(u8 *address)
{
	*address = l3_address;
	return(SUCCESS);
}
#endif

#if defined(CAN_LAYER_3)
result_t get_new_l3_node_address(u8 *address)
{
	*address = ++l3_address;
	return(SUCCESS);
//        sys_eeprom_write(NODE_ADDRESS, (u8) rand());
}
#endif

#ifdef TEST
void status_handler(can_status_t status, baud_rate_t baud)
{
    DEBUG_D("status_handler()\n\r");
}
#endif
