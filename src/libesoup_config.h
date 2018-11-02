/**
 * @file libesoup_config.h
 *
 * @author John Whitmore
 * 
 * @brief libesoup configuration file for the CAN Node
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
 *
 */
#ifndef _LIBESOUP_CONFIG_H
#define _LIBESOUP_CONFIG_H


#if defined(__dsPIC33EP256MU806__)
//#define SYS_CLOCK_FREQ 8000000     // 8MHz
#define SYS_CLOCK_FREQ 60000000    // 60MHz
#endif

#define SYS_DEBUG_BUILD

//#define SYS_CHANGE_NOTIFICATION
//#define SYS_CHANGE_NOTIFICATION_MAX_PINS    8

#define SYS_SERIAL_LOGGING

#if defined(SYS_SERIAL_LOGGING)

//#define SYS_SERIAL_PORT_GndTxRx
#define SYS_SERIAL_PORT_GndRxTx

#define SYS_LOG_LEVEL LOG_DEBUG

#define SYS_UART
        
#define SYS_SERIAL_LOGGING_BAUD           19200
//#define SYS_SERIAL_LOGGING_BAUD           38400
//#define SYS_SERIAL_LOGGING_BAUD           76800
//#define SYS_SERIAL_LOGGING_BAUD           115200

#define SYS_UART_TX_BUFFER_SIZE 300

#endif // defined(SYS_SERIAL_LOGGING)

#define SYS_HW_TIMERS
#define SYS_SW_TIMERS
#define SYS_NUMBER_OF_SW_TIMERS    10
#define SYS_SW_TIMER_TICK_ms        5

/*
 * EEPROM Chip is connected via the SPI Interface so enable
 */
#if !defined(__18F4585)
#define SYS_SPI_BUS
#define SYS_SPI_NUM_CHANNELS 1
#define SYS_SPI_NUM_DEVICES 1

/*
 * Include access to the EEPROM Chip in the project
 */
#define SYS_EEPROM
#endif

/*
 * Include random number generation
 */
#define SYS_RAND

/*
 * CAN Bus depends on System Status Code
 */
#define SYS_SYSTEM_STATUS

/*
 * CAN Bus switches
 */
#define SYS_CAN_BUS
#ifdef SYS_CAN_BUS
#define SYS_CAN_FRAME_HANDLER_ARRAY_SIZE     10
#define SYS_CAN_L2_HANDLER_ARRAY_SIZE         5
#define SYS_CAN_RX_CIR_BUFFER_SIZE            5
#define SYS_CAN_PING_PROTOCOL
#define SYS_CAN_PING_FRAME_ID             0x3FF
#define SYS_CAN_PING_IDLE_SPREAD         (1000)    // 1,000m Second Spread around
#define SYS_CAN_PING_IDLE_INTERVAL       (5000)    // A 5,000 mSecond Idle time
//#define SYS_CAN_DYNAMIC_BAUD_RATE
//#define SYS_CAN_DCNCP

//#define SYS_CAN_ISO15765
#ifdef SYS_CAN_ISO15765
#define SYS_CAN_ISO15765_REGISTER_ARRAY_SIZE     5
#define SYS_CAN_ISO15765_MAX_MSG               256
#define SYS_CAN_ISO15765_LOG
#endif // SYS_CAN_ISO15765
#endif // SYS_CAN_BUS
/*
 * Include a board file
 */
#if defined(__dsPIC33EP256MU806__)
#include "libesoup/boards/cinnamonBun/dsPIC33/cb-dsPIC33EP256MU806.h"
#elif defined (__PIC24FJ256GB106__)
#include "libesoup/boards/cinnamonBun/pic24FJ/cb-PIC24FJ256GB106.h"
#elif defined(__18F4585)
#include "libesoup/boards/gauge/gauge-PIC18F4585.h"
#elif defined(__RPI)
#include "libesoup/boards/rpi.h"
#endif

/*
 *******************************************************************************
 *
 * Project Specific Defines
 *
 *******************************************************************************
 */
/*
 *  EEPROM Address MAP
 */
#define EEPROM_NODE_STATUS_ADDR             0x00
#define NODE_STATUS_APP_VALID              (1 << 0)

#define EEPROM_NODE_IO_ADDRESS              0x01
#define EEPROM_NODE_CAN_BAUD_RATE_ADDR      0x02
#define EEPROM_NODE_L3_ADDRESS              0x03



#if 0
#define EEPROM_IO_ADDRESS_ADDR         0x04
#define EEPROM_NODE_DESCRIPTION_ADDR   0x05  // Array of 30 Bytes!
#define EEPROM_NEXT_FREE               0x23

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
 * APP Messages from the Connected Android Applicaiton
 * BUN Messages from this CinnamonBun
 */
#define APP_NODE_CONFIG_INFO_REQ         APP_MSG_USER_OFFSET
#define APP_NODE_CONFIG_INFO_UPDATE     (APP_MSG_USER_OFFSET + 1)
#define APP_APPLICATION_INFO_REQ        (APP_MSG_USER_OFFSET + 2)

#define BUN_NODE_CONFIG_INFO_RESP        BUN_MSG_USER_OFFSET
#define BUN_APPLICATION_INFO_RESP        (BUN_MSG_USER_OFFSET + 1)

#define CAN_NODE_OS

#endif  // 0

#endif // _LIBESOUP_CONFIG_H
