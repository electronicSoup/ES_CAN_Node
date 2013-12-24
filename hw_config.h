#ifndef HW_CONFIG_H
#define HW_CONFIG_H

//#include "p24Fxxxx.h"

/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

#if (cpu == PIC24FJ256GB106)

    #define CLOCK_FREQ 4000000

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
    #define EEPROM_CS_PIN_DIRECTION    TRISDbits.TRISD7
    #define EEPROM_CS                  LATDbits.LATD7
    #define EEPROM_Select()            EEPROM_CS = 0
    #define EEPROM_DeSelect()          EEPROM_CS = 1

    //  RD0  - /CAN INT
    #define CAN_INTERRUPT_PIN_DIRECTION    TRISDbits.TRISD0
    #define CAN_INTERRUPT                  PORTDbits.RD0
    #define CAN_Interrupt()                (CAN_INTERRUPT == 0)

    //  RD6 - /CAN CS
    #define CAN_CS_PIN_DIRECTION    TRISDbits.TRISD6
    #define CAN_CS                  LATDbits.LATD6
    #define CAN_Select()            CAN_CS = 0
    #define CAN_DeSelect()          CAN_CS = 1

     //  RD1  - SCK
     //  RD2  - SO
     //  RD3  - SI
    #define SPI_SCK_DIRECTION   TRISDbits.TRISD1
    #define SPI_SO_DIRECTION    TRISDbits.TRISD2
    #define SPI_SI_DIRECTION    TRISDbits.TRISD3
#elif (cpu == PIC24FJ256GB110)

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
