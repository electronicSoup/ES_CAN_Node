/**
 *
 * @file es_lib/example/system.h
 *
 * @author John Whitmore
 *
 * This file contains an example es_lib system.h configuration file. 
 *
 * The es_lib library of source code expects a system.h header file to exist
 * in your include path. The file contains the various switches and definitions
 * which configure the various features of the library.
 *
 * Copyright 2017 electronicSoup Limited
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _LIBESOUP_CONFIG_H
#define _LIBESOUP_CONFIG_H

#include <xc.h>

/**
 * @brief The required Instruction clock frequency of the device. 
 * 
 * The actual Hardware clock frequency is defined by the MACRO CRYSTAL_FREQ in
 * core.h. That constant is used in conjunction with this required SYS_CLOCK_FREQ 
 * in the function clock_init() to set the desired frequency with the PLL.
 *  
 * The function clock_init() has to be called on entry to main() to 
 * initialise the device to this clock frequency.
 */
#if defined(__PIC24FJ256GB106__) || defined(__PIC24FJ64GB106__)
#define SYS_CLOCK_FREQ 4000000     // 4MHz
#elif defined(__dsPIC33EP256MU806__)
//#define SYS_CLOCK_FREQ 8000000     // 8MHz
#define SYS_CLOCK_FREQ 60000000    // 60MHz
#elif defined(__18F4585)
#define SYS_CLOCK_FREQ 16000000     // 8MHz
#endif

#define SYS_SERIAL_LOGGING

#if defined(SYS_SERIAL_LOGGING)

/**
 * @brief Physical Pin configuration of the Serial Logging port.
 *
 * On the cinnamonBun the the Gnd pin is physically fixed but the remaining two
 * pins use Microchip peripheral select functionality to configure which pin is
 * the Receive pin and which is the Transmit.
 *
 * There are two valid settings SYS_SERIAL_PORT_GndTxRx or SERIAL_PORT_GndRXTx 
 * 
 * Default : SYS_SERIAL_PORT_GndRxTx
 */
//#define SYS_SERIAL_PORT_GndTxRx
#define SYS_SERIAL_PORT_GndRxTx

/**
 * @brief Serial logging level
 *
 * The valid log levels are defined in es_lib/core.h.
 * Default set to Debug logging level. This switch is used in conjunction
 * with es_lib/logging/serial_log.h. That file defines Logging macros which 
 * expect a logging level to be defined and a #define of "DEBUG_FILE" at the 
 * top of the file for logging to be enabled.
 */
#define SYS_LOG_LEVEL LOG_DEBUG

#define SYS_UART
        
/**
 * @brief Baud rate of the serial logging port
 *
 * This is Baud rate is set by the function serial_init(). This call to 
 * initialise the baud rate has to be preceeded by a call to clock_init() so
 * that the device's instruction clock speed is know, and the correct scaling
 * can be preformed for the required baud rate.
 * 
 * Default set to 19k2
 */
#define SYS_SERIAL_LOGGING_BAUD           19200
//#define SYS_SERIAL_LOGGING_BAUD           38400
//#define SYS_SERIAL_LOGGING_BAUD           76800
//#define SYS_SERIAL_LOGGING_BAUD           115200

/**
 * @brief The size of the Transmit buffer to be used by the UARTs.
 *
 * Default set to 300 Bytes as the serial port
 */
#define SYS_UART_TX_BUFFER_SIZE 300

#else  // defined(SYS_SERIAL_LOGGING)
#define SYS_UART
/**
 * @brief The size of the Transmit buffer to be used by the UARTs.
 *
 * Default set to 300 Bytes as the serial port
 */
#define SYS_UART_TX_BUFFER_SIZE 300
#endif // defined(SYS_SERIAL_LOGGING)

#define SYS_HW_TIMERS
#define SYS_SW_TIMERS
#define SYS_NUMBER_OF_SW_TIMERS    10
#define SYS_SW_TIMER_TICK_ms        5

/*
 * EEPROM Chip is connected via the SPI Interface so enable
 */
#define SYS_SPI_BUS

/*
 * Include access to the EEPROM Chip in the project
 */
#define SYS_EEPROM

/*
 * Include random number generation
 */
#define SYS_RAND

/*
 * Include a board file
 */
#if defined(__dsPIC33EP256MU806__)
#include "libesoup/boards/cb-dsPIC33EP256MU806.h"
#elif defined (__PIC24FJ256GB106__)
#include "libesoup/boards/cb-PIC24FJ256GB106/cb-PIC24FJ256GB106.h"
#elif defined(__18F4585)
#include "libesoup/boards/gauge-PIC18F4585.h"
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
/**
 * @brief EEPROM Watch Dog Reset info Address
 *
 * The Bootloader's Watch Dog Reset Protocol Byte address is used for 
 * communication between the Bootloader and the installed Firmware. 
 *
 * The byte contains a bit field of data, indicating the Power Up status of the
 * device.
 *
 * Only two bits of the Byte are used.
 */
#define EEPROM_WDR_PROTOCOL_ADDR           0x00

/**
 * @brief The processor was reset by a Watch Dog Reset.
 *
 * This bit of the @see EEPROM_WDR_PROTOCOL_ADDR EEPROM Byte is set by the
 * bootloader if it detects that the processor has been reset by a Watch Dog
 * Reset. Normally the bootload will invalidate the installed firmware if a 
 * Watch Dog Reset is detected on power up.
 *
 * The invalidation of installed firmware can be countermanded if the 
 * @see WDR_DO_NOT_INVALIDATE_FIRMWARE bit of the @see EEPROM_WDR_PROTOCOL_ADDR
 * has been set by the installed firmware.
 */
#define WDR_PROCESSOR_RESET_BY_WATCHDOG    0x01

/**
 * @brief Force Bootloader NOT to invalidate firmware on Watch Dog Reset.
 *
 * This bit of the @see EEPROM_WDR_PROTOCOL_ADDR Byte can be set by the 
 * installed firware. Normally on power up if a Watch Dog Reset condition is 
 * detected the bootload will invalidate the installed firmware. If however this
 * bit has been set by the installed firmware then the bootload will only set 
 * the @see WDR_PROCESSOR_RESET_BY_WATCHDOG bit to indicate that the WDR has
 * occured, and will not invalidate firmware. The firmware should, when it
 * starts executing, check the @see WDR_PROCESSOR_RESET_BY_WATCHDOG bit to see
 * if a WDR has occured and proceed accordingly.
 *
 */
#define WDR_DO_NOT_INVALIDATE_FIRMWARE     0x02

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

#endif // _LIBESOUP_CONFIG_H
