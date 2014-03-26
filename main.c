/**
 * \file main.c
 *
 * \brief main entry point for the DongleNode project
 *
 * \author John Whitmore
 *
 * \date 12th Sept 2013
 */
//#include "USB/usb.h"
//#include "USB/usb_function_cdc.h"

#include "os_api.h"
#include "system.h"
#include "logging.h"
#include "main.h"
#include "hw_config.h"
#include "timers/timer_sys.h"
#include "utils/utils.h"
#include "can/es_can.h"

// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
//
// See /opt/microchip/mplabc30/v3.30c/support/PIC24F/h/p24FJ64GB106.h
//
// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
_CONFIG2(FNOSC_PRIPLL & POSCMOD_NONE & PLL_96MHZ_ON & PLLDIV_DIV2) // Primary HS OSC with PLL, USBPLL /2
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

#if LOG_LEVEL < NO_LOGGING
#define TAG "MAIN"
#endif

extern bool timer_tick;
#if defined(CAN_LAYER_3)
static u8 l3_address = 1;
result_t get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

// C30 and C32 Exception Handlers
// If your code gets here, you either tried to read or write
// a NULL pointer, or your application overflowed the stack
// by having too many local variables or parameters declared.
void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
{
#if LOG_LEVEL <= LOG_ERROR
    serial_log(Error, TAG, "Address error");
#endif
    while (1)
    {
    }
}

void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
{
#if LOG_LEVEL <= LOG_ERROR
    serial_log(Error, TAG, "Stack error");
#endif
    while (1)
    {
    }
}


void err(char *string)
{
#if LOG_LEVEL <= LOG_ERROR
    serial_log(Error, TAG, string);
#endif
}

#ifdef TEST
static void expiry(u8 *);
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
        if (timer_tick) {
            timer_tick = FALSE;
            tick();
        }

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
#if DEBUG_LEVEL <= LOG_DEBUG
    serial_log(Debug, TAG, "status_handler()\n\r");
#endif
}
#endif
