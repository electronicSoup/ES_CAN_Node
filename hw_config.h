#ifndef HW_CONFIG_H
#define HW_CONFIG_H

//#include "p24Fxxxx.h"
#define PIC24FJ256GB106

/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

#define HW_SPI
#define SPI_RW_FINISHED SPI1STATbits.SPIRBF

#if defined(PIC24FJ256GB106)

    #define EEPROM

    #define CLOCK_FREQ 4000000
    #define SERIAL_BAUD 19200

    // RP20 and RP6 are Valid pins so 20 or 6
    //
    // 6 is the Programming port ICSP
    #define SERIAL_PORT 20

    #undef HEARTBEAT


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
#if 0
    #define EEPROM_Select()            SPI1STATbits.SPIEN = 0; \
                                       SPI1CON1bits.CKE = 0; \
                                       SPI1CON1bits.CKP = 1; \
                                       SPI1STATbits.SPIEN = 1; \
                                       Nop(); \
                                       EEPROM_CS = 0
#else
    #define EEPROM_Select()            EEPROM_CS = 0
#endif
    #define EEPROM_DeSelect()          EEPROM_CS = 1
#endif
    //  RD0  - /CAN INT
    #define CAN_INTERRUPT_PIN_DIRECTION    TRISDbits.TRISD0
    #define CAN_INTERRUPT_PIN                  PORTDbits.RD0
    #define CAN_INTERRUPT                  !CAN_INTERRUPT_PIN

    //  RD6 - /CAN CS
    #define CAN_CS_PIN_DIRECTION    TRISDbits.TRISD6
    #define CAN_CS                  LATDbits.LATD6
#if 0
#define CAN_Select()            SPI1STATbits.SPIEN = 0; \
                                SPI1CON1bits.CKE = 0; \
                                SPI1CON1bits.CKP = 1; \
                                SPI1STATbits.SPIEN = 1; \
                                Nop(); \
                                CAN_CS = 0
#else
    #define CAN_Select()            CAN_CS = 0;
#endif
    #define CAN_DeSelect()          CAN_CS = 1

     //  RD1  - SCK
     //  RD2  - SO
     //  RD3  - SI
    #define SPI_SCK_DIRECTION   TRISDbits.TRISD1
    #define SPI_MISO_DIRECTION  TRISDbits.TRISD2
    #define SPI_MOSI_DIRECTION  TRISDbits.TRISD3

/*
 * MCP2515 Setup
 */
#if 0
    #define MCP2515_INPUT_0
    #define MCP2515_INPUT_1
    #define MCP2515_INPUT_2

    #define MCP2515_OUTPUT_0
    #define MCP2515_OUTPUT_1
#endif // 0

    #define USB_HOST_POWER_PIN_DIRECTION    TRISDbits.TRISD8
    #define USB_HOST_POWER                  LATDbits.LATD8

#elif defined(PIC24FJ256GB110)

    #define CLOCK_FREQ 32000000

    /*
     *  PINS on my CAN expansion board
     *
     *  RD10 - SCK
     *  RD9  - SO  (Pin 14 of 18 DIP 2515)
     *  RD8  - SI  (Pin 15 of 18 DIP 2515)
     *  RA15 - /CS
     *  RA14 - /INT
     *  RA5  - /RESET
     *  RA4  - LED
     *
     */
    //  RA14  - /INT
    #define CAN_INTERRUPT_PIN_DIRECTION    TRISAbits.TRISA14
    #define CAN_INTERRUPT                  PORTAbits.RA14
    #define CAN_Interrupt()                (CAN_INTERRUPT == 0)

    //  RA15 - /CS
    #define CAN_CS_PIN_DIRECTION    TRISAbits.TRISA15
    #define CAN_CS                  LATAbits.LATA15
    #define CAN_Select()            CAN_CS = 0
    #define CAN_DeSelect()          CAN_CS = 1

    /*
     * LED is on RB9
     */
    #define HEARTBEAT_LED_DIRECTION   TRISAbits.TRISA4
    #define HEARTBEAT_LED             LATAbits.LATA4
    #define Heartbeat_on()             HEARTBEAT_LED = 0
    #define Heartbeat_off()            HEARTBEAT_LED = 1


     //  RA14 - /RESET
    #define RESET_PIN_DIRECTION   TRISAbits.TRISA5
    #define RESET_PIN             LATAbits.LATA5
    #define RESET_On()        RESET_PIN  = 0
    #define RESET_Off()        RESET_PIN  = 1

     //  RD10 - SCK
     //  RD9  - SO
     //  RD8  - SI
    #define CAN_SPI_SCK_DIRECTION   TRISDbits.TRISD10
    #define CAN_SPI_SO_DIRECTION    TRISDbits.TRISD9
    #define CAN_SPI_SI_DIRECTION    TRISDbits.TRISD8
#endif

#endif // HW_CONFIG_H
