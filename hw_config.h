#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include "p24Fxxxx.h"

#define CLOCK_FREQ 32000000


/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

#if (cpu == PIC24FJ256GB110)
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
