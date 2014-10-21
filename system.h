/**
 *
 * \file es_lib/example_system.h
 *
 * This file contains the various switches which can be used with the
 * es_lib source code library. 
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

/*
 * Baud rate of the serial debug port
 */
#define SERIAL_BAUD 19200

/*
 * The configuration of the Serial Port there are 3 pins, the Gnd pin is fixed
 * so you define the configuration of the other two pins. Uncomment as required
 */
//#define SERIAL_PORT_GndTxRx
#define SERIAL_PORT_GndRxTx

/*
 * Log level. these are defined in es_lib/core.h as they are included everywhere
 */
#define LOG_LEVEL LOG_DEBUG

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
/*
 * The number of Handlers that can be registered with Layer 2
 */
#define CAN
#define CAN_BAUD_AUTO_DETECT_LISTEN_PERIOD    SECONDS_TO_TICKS(10)
#define CAN_L2_HANDLER_ARRAY_SIZE 5
#define CAN_LAYER_3

#define CAN_L2_IDLE_PING
#define CAN_L2_IDLE_PING_FRAME_ID 0x666
/*
 * The Ping Period will be between CAN_IDLE_PERIOD - 0.5 Seconds and +0.5 Seconds
 * As a result this shold probably be greater then 1 Second. If a node picks a 
 * random value of Zero then it'll do nothing but ping!
 */
#define CAN_L2_IDLE_PING_PERIOD     SECONDS_TO_TICKS(2)

/*
 * Android Definitions:
 *
 * When using the library Android states one of these switches should be
 * enabled.
 */
//#define ANDROID_BOOT
#define ANDROID_NODE
//#define ANDROID_DONGLE

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
 * Value stored in EEPROM to indicate valid Application Installed.
 */
#define APP_VALID_MAGIC_VALUE  0x55

/*
 *  EEPROM Address MAP
 */
#define EEPROM_APP_VALID_MAGIC_ADDR   0x00  // Magic takes two bytes.
#define EEPROM_L3_NODE_ADDRESS_ADDR   0x02
#define EEPROM_CAN_BAUD_RATE_ADDR     0x03
#define EEPROM_IO_ADDRESS_ADDR        0x04
#define EEPROM_NODE_DESCRIPTION_ADDR  0x05

/*
 * The Installed Application can use EEPROM from this address.
 */
#define APP_EEPROM_START  0x40

/* 
 * Application addresses in Flash memory
 */
#define APP_HANDLE_FLASH_ADDRESS 0x400
#define APP_START_FLASH_ADDRESS  0x18000

/*
 * Macros for executing Application code.
 */
#define CALL_APP_INIT()    asm("call 0x18096")
#define CALL_APP_MAIN()    asm("call 0x1809A")

#if defined(CAN_LAYER_3)
extern void get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif
