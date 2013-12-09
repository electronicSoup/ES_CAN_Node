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
#include "logging.h"
#include "main.h"
#include "hw_config.h"
#include "timers/timer_sys.h"
#include "utils/utils.h"
#include "can/l2_can.h"

// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
//
// See /opt/microchip/mplabc30/v3.30c/support/PIC24F/h/p24FJ64GB106.h
//
// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
_CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2) // Primary HS OSC with PLL, USBPLL /2
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

#if LOG_LEVEL < NO_LOGGING
#define TAG "MAIN"
#endif

extern BOOL timer_tick;

void err(char *string)
{
#if LOG_LEVEL <= LOG_ERROR
    serial_log(Error, TAG, string);
#endif
}

int main(void)
{
    baud_rate_t baudRate;
#if 0
    HEARTBEAT_LED_DIRECTION = OUTPUT_PIN;
    heartbeat_off((BYTE *)NULL);
#endif
    init_timer();

    /**
     * Hardware init the Serial Port Pin Direction.
     * Set RPOR13 PIN D13 = RP26 to Output Function 3 - UART1 Transmit
     */
    RPOR13bits.RP26R = 3;

    U1MODE = 0x8800;
    U1STA = 0x0410;
    /*
     * Desired Baud Rate = FCY/(16 (UxBRG + 1))
     *
     * UxBRG = ((FCY/Desired Baud Rate)/16) ? 1
     *
     * for 19200 Serial Connection wiht 16M Clock
     *
     * UxBRG = ((16000000/19200)/16) -1
     *
     */
    //U1BRG = 103; // 9600
    U1BRG = 51;

#if LOG_LEVEL <= LOG_DEBUG
    serial_log(Debug, TAG, "************************\n\r");
    serial_log(Debug, TAG, "***   CAN Bus Node   ***\n\r");
    serial_log(Debug, TAG, "************************\n\r");
#endif

#if 0
    /*
     * The PIC24FJ265GB110 HAS NOT FLASH!!!
     */

    /* ToDo sort out baudRate */
    pri_eeprom_read(NETWORK_BAUD_RATE, &baudRate);

    baudRate = baud_10K;

    if(baudRate > BAUD_MAX)
    {
#if DEBUG_LEVEL <= LOG_ERROR
        serial_log(Error, TAG, "No Baud Rate Stored reset to slowest (10K)\n\r");
#endif
        baudRate = baud_10K;
    }
    else
    {
#if DEBUG_LEVEL <= LOG_DEBUG
        serial_log(Debug, TAG, "CAN Baud Rate stored in Flash %s\n\r", baud_rate_strings[baudRate]);
#endif
    }
#endif

    baudRate = baud_10K;

    // Send in null we're not defining a default handler for messages
    // if nothing's regestered an interest we're just not interested

    can_init(baudRate, (l2_msg_handler_t)NULL, (l3_msg_handler_t)NULL, FALSE);

//    enable_interrupts();

//    heartbeat_on(NULL);

    /*
     * Enter the main loop
     */
    while(TRUE)
    {
        if (timer_tick)
        {
            timer_tick = FALSE;
            tick();
        }

        canTasks();
    }
}
