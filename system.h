/**
 *
 * \file system.h
 *
 * System definitions for the DongleNode Project
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
#ifndef SYSTEM_H
#define SYSTEM_H

#include "es_lib/core.h"

#define LOG_LEVEL LOG_DEBUG

/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

// EEPROM Address MAP
#define NODE_ADDRESS   0x00
#define APP_CODE_VALID 0x02
#define NETWORK_BAUD_RATE 0x03

#define TEST


/*
 * Define the procesor being used
 */
#if defined( __PIC24FJ256GB110__ )
#define PIC24FJ256GB110
#elif defined( __PIC24FJ256GB106__ )
#define PIC24FJ256GB106
#endif

#define ENABLE_USB
#define HW_SPI
#undef  EEPROM
//#define L2_CAN_INTERRUPT_DRIVEN
//#define CAN_LAYER_3

#if defined(PIC24FJ256GB110)
#define CLOCK_FREQ 16000000
#elif defined(PIC24FJ256GB106)
#define CLOCK_FREQ 16000000
#endif

#define SERIAL_BAUD 19200

#if defined(__C30__)
#define SPI_RW_FINISHED SPI1STATbits.SPIRBF
#endif


// RP26, RP20 and RP6 are Valid pins so 26, 20 or 6
//
// 6 is the Programming port ICSP
#if defined(PIC24FJ256GB110)
#define SERIAL_PORT 26
#elif defined(PIC24FJ256GB106)
    #define USB_HOST_POWER_PIN_DIRECTION    TRISDbits.TRISD8
    #define USB_HOST_POWER                  LATDbits.LATD8


    #define SERIAL_PORT 20

    /*
     *  RD1/RP24  - SCK
     *  RD2       - SO  (Pin 14 of 18 DIP 2515)
     *  RD3       - SI  (Pin 15 of 18 DIP 2515)
     *  RD7       - /EEPROM CS
     *  RD6       - /CAN CS
     *  RD0       - /CAN INT
     */

    //  RD7 - /EEPROM CS
#if defined(EEPROM)
    #define EEPROM_MAX_ADDRESS 0x7F
    #define EEPROM_CS_PIN_DIRECTION    TRISDbits.TRISD7
    #define EEPROM_CS                  LATDbits.LATD7

    #define EEPROM_Select()            EEPROM_CS = 0
    #define EEPROM_DeSelect()          EEPROM_CS = 1
#endif

    //  RD0  - /CAN INT
    #define CAN_INTERRUPT_PIN_DIRECTION    TRISDbits.TRISD0
    #define CAN_INTERRUPT_PIN                  PORTDbits.RD0
    #define CAN_INTERRUPT                  !CAN_INTERRUPT_PIN

    //  RD6 - /CAN CS
    #define CAN_CS_PIN_DIRECTION    TRISDbits.TRISD6
    #define CAN_CS                  LATDbits.LATD6

    #define CAN_Select()            CAN_CS = 0;
    #define CAN_DeSelect()          CAN_CS = 1

     //  RD1  - SCK
     //  RD2  - SO
     //  RD3  - SI
    #define SPI_SCK_DIRECTION   TRISDbits.TRISD1
    #define SPI_MISO_DIRECTION  TRISDbits.TRISD2
    #define SPI_MOSI_DIRECTION  TRISDbits.TRISD3
#endif

#if defined(PIC18F4585)
#define EEPROM_MAX_ADDRESS 0x3ff
#elif defined(PIC24FJ256GB106)
#define EEPROM_MAX_ADDRESS 0x7f
#endif


#if defined(PIC24FJ256GB106)
// HEARTBEAT uses the system timers. This Simple Bootloader doesn't use
// them. So can't be used.
//#define HEARTBEAT

#define HEARTBEAT_ON_TIME    SECONDS_TO_TICKS(0x02)
#define HEARTBEAT_OFF_TIME   SECONDS_TO_TICKS(0x02)

#define HEARTBEAT_PIN_DIRECTION   TRISCbits.TRISC14
#define HEARTBEAT_PIN             LATCbits.LATC14

#define Heartbeat_on() HEARTBEAT_PIN = 1
#define Heartbeat_off() HEARTBEAT_PIN = 0

#endif

#if defined(CAN_LAYER_3)
result_t get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

#endif //SYSTEM_H
