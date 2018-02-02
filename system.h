/**
 *
 * \file system.h
 *
 * es_lib Configuraiton for CAN_Node Project.
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
#ifndef _SYSTEM_H
#define _SYSTEM_H

/*
 * The configuration of the Serial Port there are 3 pins, the Gnd pin is fixed
 * so you define the configuration of the other two pins. Uncomment as required
 */
//#define SERIAL_PORT_GndTxRx
#define SERIAL_PORT_GndRxTx

#define SYSTEM_TICK_ms 5

#if defined(__dsPIC33EP256MU806__)
#include "es_lib/boards/cb-dsPIC33EP256MU806.h"
#elif defined(__PIC24FJ256GB106__) || defined(__PIC24FJ64GB106__)
#include "es_lib/boards/cb-PIC24FJ256GB106.h"
#endif

#include "es_lib/core.h"

/**
 * @brief The required Instruction clock frequency of the device. 
 * 
 * The actual Hardware clock frequency is defined by the MACRO CRYSTAL_FREQ in
 * core.h. That constant is used in conjunction with this required CLOCK_FREQ 
 * in the function clock_init() to set the desired frequency with the PLL.
 *  
 * The function clock_init() has to be called on entry to main() to 
 * initialise the device to this clock frequency.
 */
#define CLOCK_FREQ 8000000     // 8MHz
//#define CLOCK_FREQ 60000000    // 60MHz

/*
 * The size of the Transmit buffer to be used on the USART Serial port.
 */
#define USART_TX_BUFFER_SIZE 300

/*
 * Baud rate of the serial debug port
 */
#define SERIAL_LOGGING_BAUD 19200

/*
 * Log level. these are defined in es_lib/core.h as they are included everywhere
 */
#define LOG_LEVEL LOG_DEBUG
//#define LOG_LEVEL LOG_INFO
/*
 * NUMBER_OF_TIMERS
 *
 * Definition of the number of system timers required in the system. Increasing
 * the number of timers uses more system RAM. This will depend on the estimated
 * demand for timers in your design.
 */
#define NUMBER_OF_TIMERS 20

/*
 * CAN Definitions
 */
#define CAN

#ifdef CAN
/*
 * The number of Handlers that can be registered with Layer 2
 */
#define CAN_L2_HANDLER_ARRAY_SIZE 5

/*
 * If Layer 2 is required to listen for the Connected Baud enable
 * CAN_BAUD_AUTO_DETECT. In that case pass the baudrate no_baud to the Layer
 * 2 Initialise function
 */
//#define CAN_BAUD_AUTO_DETECT

#ifdef CAN_BAUD_AUTO_DETECT
/*
 * If Layer 2 is required to listen for the Connected Baud rate this
 * macro defines the listening period.
 */
#define CAN_BAUD_AUTO_DETECT_LISTEN_PERIOD    SECONDS_TO_TICKS(20)
#endif // CAN_BAUD_AUTO_DETECT

/*
 * The Ping Protocol.
 *
 * If the network will be used with baud rate auto detect then Ping
 * Protocol keeps activity on the CAN Network.
 *
 * Random must be initialised to use Ping Protocol!
 */
#define CAN_L2_IDLE_PING

/*
 * The Ping Period will be between CAN_IDLE_PERIOD - 0.5 Seconds and +0.5
 * Seconds As a result this shold probably be greater then 1 Second. If
 * a node picks a random value of Zero then it'll do nothing but ping!
 */
#ifdef CAN_L2_IDLE_PING
//#define CAN_L2_PING_LOGGING
#define CAN_L2_IDLE_PING_PERIOD     SECONDS_TO_TICKS(5)
#endif  // CAN_L2_IDLE_PING

/*
 * DCNCP - Dynamic Can Node Configuration Protocol
 */
#define DCNCP_CAN

#ifdef DCNCP_CAN

/*
 * If DCNCP includes code to change Baud Rate on the fly include
 * DCNCP_CAN_BAUDRATE
 */
#define DCNCP_CAN_BAUDRATE

/*
 * Include ISO15765 Protocol functionality
 *
 * If you enable CAN Bus ISO15765 protocol then you must include utils/rand.c
 * in your project. Random numbers are used. In addition provide a function
 * "node_get_address" which returns an Address for the protocol
 * to use. Note that the address you give may well be rejected if it is already
 * being used not the network so the function can be recalled until an unused
 * address is obtained
 *
 * u8 node_get_address(void)
 * {
 *	return(0x01);
 * }
 */
#define ISO15765

#ifdef ISO15765
/*
 * The number of different ISO-15763 protocols that can be registered
 * in the system or applicaiton.
 */
#define ISO15765_REGISTER_ARRAY_SIZE 2

/*
 * Project uses the ISO15765 Logging functionality to send information across
 * the network
 */
#define ISO15765_LOGGING

/*
 * If this node can act as a CAN Bus Network logger define CAN_NET_LOGGER
 *
 * Without this definition the node can still send network logging messages but
 * does not include the code to act as the network logger itself.
 */
//#define ISO15765_LOGGER

/*
 * The Network Logger will periodically repeat it's register request messages
 * in case any of the nodes are late to the party.
 */
#ifdef ISO15765_LOGGER
#define ISO15765_LOGGER_PING_PERIOD SECONDS_TO_TICKS(60)
#endif // ISO15765_LOGGER

/*
 * Include Dynamic Can Node COnfiguration Protocol in ISO15765 layer.
 *
 * Used for management across the CAN Network
 */
#define DCNCP_ISO15765

#endif // ISO15765

/*
 * Include the ISO11783 Protocol
 */
#define ISO11783

#ifdef ISO11783
/*
 * The number of different ISO-11783 handlers that can be registered
 * in the system or applicaiton.
 */
#define ISO11783_REGISTER_ARRAY_SIZE 10

#endif // ISO11783

#endif // DCNCP_CAN
#endif // CAN

/*
 * Android Definitions:
 *
 */
#define ANDROID

#ifdef ANDROID
/*
 * Definition of the function used to change State to the App connected
 * state. If you are going to use an Android connected device you must
 * define a function for moving into the App Connected state. See example
 * below:
 *
 *   extern void example_set_app_connected_state(void);
 *   #define ANDROID_SET_APPLICATION_CONNECTED_STATE example_set_app_connected_state();
 */
extern void set_app_connected_state(void);
#define ANDROID_SET_APPLICATION_CONNECTED_STATE set_app_connected_state();

/*
 * Android main processing when the Android Application is NOT Connected.
 * If different processing is required when the Android Applicaiton is not
 * connected to your device then define the function to perform this
 * functionality and the MACRO.
 * This is NOT Mandatory. If you project does not require it then don't
 * define the MACRO.
 *
 *  extern void example_no_android_app_function(void);
 *  #define NO_ANDROID_APP_FN example_no_android_app_function();
 */
//extern void example_no_android_app_function(void);
//#define NO_ANDROID_APP_FN example_no_android_app_function();

#endif // ANDROID

/*
 * If project is to use the BOOT Page of EEPROM then define this option.
 */
//#define EEPROM_USE_BOOT_PAGE

/*
 *******************************************************************************
 *
 * Project Specific Defines
 *
 *******************************************************************************
 */

/*
 * Android IPC Messages.
 *
 * App message is transmitted from the Android Device App to the Cinnamom Bun
 * Bun message is transmitted from the Cinnamon Bun to the Android Device App
 *
 * Messages for communications with an Android App should be defined as
 * project specific constants relative to USER_OFFSETS.
 *
 * #define MY_FIRST_BUN_MSG     BUN_MSG_USER_OFFSET
 * #define MY_SECOND_BUN_MSG    BUN_MSG_USER_OFFSET + 1
 *
 * #define MY_FIRST_APP_MSG     APP_MSG_USER_OFFSET
 * #define MY_SECOND_APP_MSG    APP_MSG_USER_OFFSET + 1
 *
 */

/*
 * Value stored in EEPROM to indicate valid Application Installed.
 */
#define APP_VALID_MAGIC_VALUE  0x55

/*
 *  EEPROM Address MAP
 */
#define EEPROM_APP_VALID_MAGIC_ADDR_1  0x00
#define EEPROM_APP_VALID_MAGIC_ADDR_2  0x01
#define EEPROM_NODE_ADDRESS            0x02
#define EEPROM_CAN_BAUD_RATE_ADDR      0x03
#define EEPROM_IO_ADDRESS_ADDR         0x04
#define EEPROM_NODE_DESCRIPTION_ADDR   0x05  // Array of 30 Bytes!
#define EEPROM_NEXT_FREEEEEEEEEEE      0x23

#define EEPROM_CAN_NODE_PAGE_SIZE      36

/*
 * The Installed Application can use EEPROM from this address.
 */
#define APP_EEPROM_START  EEPROM_CAN_NODE_PAGE_SIZE

/*
 * Application addresses in Flash memory
 */
#define APP_HANDLE_FLASH_ADDRESS 0x400
#define APP_START_FLASH_ADDRESS  0x18000

/*
 * Macros for executing Application code.
 */
#define CALL_APP_INIT()      asm("call 0x18096")
#define CALL_APP_MAIN()      asm("call 0x1809A")
//#define CALL_APP_STATUS()    asm("call 0x1809E")
#define APP_STATUS_ADDRESS  0x1809E

/*
 * APP Messages from the Connected Android Applicaiton
 * BUN Messages from this CinnamonBun
 */
#define APP_NODE_CONFIG_INFO_REQ         APP_MSG_USER_OFFSET
#define APP_NODE_CONFIG_INFO_UPDATE     (APP_MSG_USER_OFFSET + 1)
#define APP_APPLICATION_INFO_REQ        (APP_MSG_USER_OFFSET + 2)

#define BUN_NODE_CONFIG_INFO_RESP        BUN_MSG_USER_OFFSET
#define BUN_APPLICATION_INFO_RESP        (BUN_MSG_USER_OFFSET + 1)

#define CAN_NODE_OS

#endif //SYSTEM_H
