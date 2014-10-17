/**
 *
 * \file isr.c
 *
 * The Interrupt Service Routines in this file pass the Interrupts
 * unused by the Node OS onto any installed Application.
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
#include "system.h"
#define DEBUG_FILE
#include "es_lib/logger/serial_log.h"
#include "es_lib/os/os_api.h"

#define TAG "ISR"

#define APP_ISR_TABLE      0x1809E

#define RESERVED_TRAP0        APP_ISR_TABLE
#define OSCILLATOR_FAIL       APP_ISR_TABLE + (1 * 4)
#define ADDRESS_ERROR         APP_ISR_TABLE + (2 * 4)
#define STACK_ERROR           APP_ISR_TABLE + (3 * 4)
#define MATH_ERROR            APP_ISR_TABLE + (4 * 4)
#define RESERVED_TRAP5        APP_ISR_TABLE + (5 * 4)
#define RESERVED_TRAP6        APP_ISR_TABLE + (6 * 4)
#define RESERVED_TRAP7        APP_ISR_TABLE + (7 * 4)
#define INT0_INTERRUPT        APP_ISR_TABLE + (8 * 4)
#define IC1_INTERRUPT         APP_ISR_TABLE + (9 * 4)
#define OC1_INTERRUPT         APP_ISR_TABLE + (10 * 4)
#define T1_INTERRUPT          APP_ISR_TABLE + (11 * 4)
#define INTERRUPT_4           APP_ISR_TABLE + (12 * 4)
#define IC2_INTERRUPT         APP_ISR_TABLE + (13 * 4)
#define OC2_INTERRUPT         APP_ISR_TABLE + (14 * 4)
#define T2_INTERRUPT          APP_ISR_TABLE + (15 * 4)
#define T3_INTERRUPT          APP_ISR_TABLE + (16 * 4)
#define SPI1_ERR_INTERRUPT    APP_ISR_TABLE + (17 * 4)
#define SPI1_INTERRUPT        APP_ISR_TABLE + (18 * 4)
#define U1RX_INTERRUPT        APP_ISR_TABLE + (19 * 4)
#define U1TX_INTERRUPT        APP_ISR_TABLE + (20 * 4)
#define ADC1_INTERRUPT        APP_ISR_TABLE + (21 * 4)
#define INTERRUPT_14          APP_ISR_TABLE + (22 * 4)
#define INTERRUPT_15          APP_ISR_TABLE + (23 * 4)
#define SI2C1_INTERRUPT       APP_ISR_TABLE + (24 * 4)
#define MI2C1_INTERRUPT       APP_ISR_TABLE + (25 * 4)
#define COMP_INTERRUPT        APP_ISR_TABLE + (26 * 4)
#define CN_INTERRUPT          APP_ISR_TABLE + (27 * 4)
#define INT1_INTERRUPT        APP_ISR_TABLE + (28 * 4)
#define INTERRUPT_21          APP_ISR_TABLE + (29 * 4)
#define IC7_INTERRUPT         APP_ISR_TABLE + (30 * 4)
#define IC8_INTERRUPT         APP_ISR_TABLE + (31 * 4)
#define INTERRUPT_24          APP_ISR_TABLE + (32 * 4)
#define OC3_INTERRUPT         APP_ISR_TABLE + (33 * 4)
#define OC4_INTERRUPT         APP_ISR_TABLE + (34 * 4)
#define T4_INTERRUPT          APP_ISR_TABLE + (35 * 4)
#define T5_INTERRUPT          APP_ISR_TABLE + (36 * 4)
#define INT2_INTERRUPT        APP_ISR_TABLE + (37 * 4)
#define U2RX_INTERRUPT        APP_ISR_TABLE + (38 * 4)
#define U2TX_INTERRUPT        APP_ISR_TABLE + (39 * 4)
#define SPI2_ERR_INTERRUPT    APP_ISR_TABLE + (40 * 4)
#define SPI2_INTERRUPT        APP_ISR_TABLE + (41 * 4)
#define INTERRUPT_34          APP_ISR_TABLE + (42 * 4)
#define INTERRUPT_35          APP_ISR_TABLE + (43 * 4)
#define INTERRUPT_36          APP_ISR_TABLE + (44 * 4)
#define IC3_INTERRUPT         APP_ISR_TABLE + (45 * 4)
#define IC4_INTERRUPT         APP_ISR_TABLE + (46 * 4)
#define IC5_INTERRUPT         APP_ISR_TABLE + (47 * 4)
#define IC6_INTERRUPT         APP_ISR_TABLE + (48 * 4)
#define OC5_INTERRUPT         APP_ISR_TABLE + (49 * 4)
#define OC6_INTERRUPT         APP_ISR_TABLE + (50 * 4)
#define OC7_INTERRUPT         APP_ISR_TABLE + (51 * 4)
#define OC8_INTERRUPT         APP_ISR_TABLE + (52 * 4)
#define PMP_INTERRUPT         APP_ISR_TABLE + (53 * 4)
#define INTERRUPT_46          APP_ISR_TABLE + (54 * 4)
#define INTERRUPT_47          APP_ISR_TABLE + (55 * 4)
#define INTERRUPT_48          APP_ISR_TABLE + (56 * 4)
#define SI2C2_INTERRUPT       APP_ISR_TABLE + (57 * 4)
#define MI2C2_INTERRUPT       APP_ISR_TABLE + (58 * 4)
#define INTERRUPT_51          APP_ISR_TABLE + (59 * 4)
#define INTERRUPT_52          APP_ISR_TABLE + (60 * 4)
#define INT3_INTERRUPT        APP_ISR_TABLE + (61 * 4)
#define INT4_INTERRUPT        APP_ISR_TABLE + (62 * 4)
#define INTERRUPT_55          APP_ISR_TABLE + (63 * 4)
#define INTERRUPT_56          APP_ISR_TABLE + (64 * 4)
#define INTERRUPT_57          APP_ISR_TABLE + (65 * 4)
#define INTERRUPT_58          APP_ISR_TABLE + (66 * 4)
#define INTERRUPT_59          APP_ISR_TABLE + (67 * 4)
#define INTERRUPT_60          APP_ISR_TABLE + (68 * 4)
#define INTERRUPT_61          APP_ISR_TABLE + (69 * 4)
#define RTCC_INTERRUPT        APP_ISR_TABLE + (70 * 4)
#define INTERRUPT_63          APP_ISR_TABLE + (71 * 4)
#define INTERRUPT_64          APP_ISR_TABLE + (72 * 4)
#define U1_ERR_INTERRUPT      APP_ISR_TABLE + (73 * 4)
#define U2_ERR_INTERRUPT      APP_ISR_TABLE + (74 * 4)
#define CRC_INTERRUPT         APP_ISR_TABLE + (75 * 4)
#define INTERRUPT_68          APP_ISR_TABLE + (76 * 4)
#define INTERRUPT_69          APP_ISR_TABLE + (77 * 4)
#define INTERRUPT_70          APP_ISR_TABLE + (78 * 4)
#define INTERRUPT_71          APP_ISR_TABLE + (79 * 4)
#define LVD_INTERRUPT         APP_ISR_TABLE + (80 * 4)
#define INTERRUPT_73          APP_ISR_TABLE + (81 * 4)
#define INTERRUPT_74          APP_ISR_TABLE + (82 * 4)
#define INTERRUPT_75          APP_ISR_TABLE + (83 * 4)
#define INTERRUPT_76          APP_ISR_TABLE + (84 * 4)
#define CTMU_INTERRUPT        APP_ISR_TABLE + (85 * 4)
#define INTERRUPT_78          APP_ISR_TABLE + (86 * 4)
#define INTERRUPT_79          APP_ISR_TABLE + (87 * 4)
#define INTERRUPT_80          APP_ISR_TABLE + (88 * 4)
#define U3_ERR_INTERRUPT      APP_ISR_TABLE + (89 * 4)
#define U3_RX_INTERRUPT       APP_ISR_TABLE + (90 * 4)
#define U3_TX_INTERRUPT       APP_ISR_TABLE + (91 * 4)
#define SI2C3_INTERRUPT       APP_ISR_TABLE + (92 * 4)
#define MI2C3_INTERRUPT       APP_ISR_TABLE + (93 * 4)
#define USB1_INTERRUPT        APP_ISR_TABLE + (94 * 4)
#define U4_ERR_INTERRUPT      APP_ISR_TABLE + (95 * 4)
#define U4_RX_INTERRUPT       APP_ISR_TABLE + (96 * 4)
#define U4_TX_INTERRUPT       APP_ISR_TABLE + (97 * 4)
#define SPI3_ERR_INTERRUPT    APP_ISR_TABLE + (98 * 4)
#define SPI3_INTERRUPT        APP_ISR_TABLE + (99 * 4)
#define OC9_INTERRUPT         APP_ISR_TABLE + (100 * 4)
#define IC9_INTERRUPT         APP_ISR_TABLE + (101 * 4)
#define INTERRUPT_94          APP_ISR_TABLE + (102 * 4)
#define INTERRUPT_95          APP_ISR_TABLE + (103 * 4)
#define INTERRUPT_96          APP_ISR_TABLE + (104 * 4)
#define INTERRUPT_97          APP_ISR_TABLE + (105 * 4)
#define INTERRUPT_98          APP_ISR_TABLE + (106 * 4)
#define INTERRUPT_99          APP_ISR_TABLE + (107 * 4)
#define INTERRUPT_100         APP_ISR_TABLE + (108 * 4)
#define INTERRUPT_101         APP_ISR_TABLE + (109 * 4)
#define INTERRUPT_102         APP_ISR_TABLE + (110 * 4)
#define INTERRUPT_103         APP_ISR_TABLE + (111 * 4)
#define INTERRUPT_104         APP_ISR_TABLE + (112 * 4)
#define INTERRUPT_105         APP_ISR_TABLE + (113 * 4)
#define INTERRUPT_106         APP_ISR_TABLE + (114 * 4)
#define INTERRUPT_107         APP_ISR_TABLE + (115 * 4)
#define INTERRUPT_108         APP_ISR_TABLE + (116 * 4)
#define INTERRUPT_109         APP_ISR_TABLE + (117 * 4)
#define INTERRUPT_110         APP_ISR_TABLE + (118 * 4)
#define INTERRUPT_111         APP_ISR_TABLE + (119 * 4)
#define INTERRUPT_112         APP_ISR_TABLE + (120 * 4)
#define INTERRUPT_113         APP_ISR_TABLE + (121 * 4)
#define INTERRUPT_114         APP_ISR_TABLE + (122 * 4)
#define INTERRUPT_115         APP_ISR_TABLE + (123 * 4)
#define INTERRUPT_116         APP_ISR_TABLE + (124 * 4)
#define INTERRUPT_117         APP_ISR_TABLE + (125 * 4)

void __attribute__((__interrupt__, __no_auto_psv__)) _ReservedTrap0(void)
{
    void (*isr)(void) = (void (*)(void))RESERVED_TRAP0;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _ReservedTrap0\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OscillatorFail(void)
{
    void (*isr)(void) = (void (*)(void))OSCILLATOR_FAIL;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _OscillatorFail\n\r");
}

/*
 * Node is Handling this Interrupt
 */
#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _AddressError(void)
{
    void (*isr)(void) = (void (*)(void))ADDRESS_ERROR;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _AddressError\n\r");
}
#endif // 0

/*
 * Node is Handling this Interrupt
 */
#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _StackError(void)
{
    void (*isr)(void) = (void (*)(void))STACK_ERROR;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _StackError\n\r");
}
#endif

void __attribute__((__interrupt__, __no_auto_psv__)) _MathError(void)
{
    void (*isr)(void) = (void (*)(void))MATH_ERROR;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _MathError\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _ReservedTrap5(void)
{
    void (*isr)(void) = (void (*)(void))RESERVED_TRAP5;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _ReservedTrap5\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _ReservedTrap6(void)
{
    void (*isr)(void) = (void (*)(void))RESERVED_TRAP6;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _ReservedTrap6\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _ReservedTrap7(void)
{
    void (*isr)(void) = (void (*)(void))RESERVED_TRAP7;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _ReservedTrap7\n\r");
}

/*
 * INT0 is used by the MCP2515 CAN Driver
 */
#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _INT0Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))INT0_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _INT0Interrupt\n\r");
}
#endif

void __attribute__((__interrupt__, __no_auto_psv__)) _IC1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _IC1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _OC1Interrupt\n\r");
}

/*
 * Node is Handling this Interrupt
 */
#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))T1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _T1Interrupt\n\r");
}
#endif

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt4(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_4;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _Interrupt4\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _IC2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _OC2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))T2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _T2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))T3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _T3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI1_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _SPI1ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _SPI1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U1RX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _U1RXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U1TX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _U1TXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _ADC1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))ADC1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _ADC1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt14(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_14;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _Interrupt14\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt15(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_15;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _Interrupt15\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SI2C1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SI2C1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _SI2C1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _MI2C1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))MI2C1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _MI2C1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _CompInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))COMP_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _CompInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _CNInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))CN_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _CNInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _INT1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))INT1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _INT1Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt21(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_21;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _Interrupt21\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC7Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC7_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _IC7Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC8Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC8_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _IC8Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt24(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_24;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _Interrupt24\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _OC3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC4Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC4_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _OC4Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T4Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))T4_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _T4Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T5Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))T5_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _T5Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _INT2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))INT2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _INT2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U2RX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _U2RXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U2TX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _U2TXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI2ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI2_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _SPI2ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected Interrupt on _SPI2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt34(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_34;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt34\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt35(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_35;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt35\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt36(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_36;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt36\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _IC3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC4Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC4_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _IC4Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC5Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC5_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _IC5Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC6Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC6_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _IC6Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC5Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC5_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _OC5Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC6Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC6_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _OC6Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC7Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC7_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _OC7Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC8Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC8_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _OC8Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _PMPInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))PMP_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _PMPInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt46(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_46;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt46\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt47(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_47;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt47\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt48(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_48;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt48\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SI2C2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SI2C2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _SI2C2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _MI2C2Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))MI2C2_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _MI2C2Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt51(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_51;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt51\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt52(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_52;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt52\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _INT3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))INT3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _INT3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _INT4Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))INT4_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _INT4Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt55(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_55;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt55\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt56(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_56;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt56\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt57(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_57;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt57\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt58(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_58;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt58\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt59(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_59;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt59\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt60(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_60;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt60\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt61(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_61;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt61\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _RTCCInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))RTCC_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _RTCCInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt63(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_63;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt63\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt64(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_64;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt64\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U1_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U1ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U2_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U2ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _CRCInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))CRC_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _CRCInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt68(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_68;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt68\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt69(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_69;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt69\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt70(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_70;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt70\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt71(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_71;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt71\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _LVDInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))LVD_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _LVDInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt73(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_73;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt73\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt74(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_74;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt74\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt75(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_75;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt75\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt76(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_76;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt76\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _CTMUInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))CTMU_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _CTMUInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt78(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_78;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt78\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt79(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_79;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt79\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt80(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_80;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt80\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U3ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U3_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U3ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U3RXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U3_RX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U3RXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U3TXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U3_TX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U3TXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SI2C3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SI2C3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _SI2C3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _MI2C3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))MI2C3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _MI2C3Interrupt\n\r");
}

/*
 * Node is Handling this Interrupt
 */
#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _USB1Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))USB1_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _USB1Interrupt\n\r");
}
#endif //0

void __attribute__((__interrupt__, __no_auto_psv__)) _U4ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U4_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U4ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U4RXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U4_RX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U4RXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U4TXInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))U4_TX_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _U4TXInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI3ErrInterrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI3_ERR_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _SPI3ErrInterrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI3Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))SPI3_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _SPI3Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _OC9Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))OC9_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _OC9Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _IC9Interrupt(void)
{
    void (*isr)(void) = (void (*)(void))IC9_INTERRUPT;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _IC9Interrupt\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt94(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_94;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt94\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt95(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_95;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt95\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt96(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_96;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt96\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt97(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_97;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt97\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt98(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_98;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt98\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt99(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_99;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt99\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt100(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_100;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt100\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt101(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_101;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt101\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt102(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_102;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt102\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt103(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_103;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt103\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt104(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_104;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt104\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt105(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_105;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt105\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt106(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_106;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt106\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt107(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_107;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt107\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt108(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_108;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt108\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt109(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_109;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt109\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt110(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_110;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt110\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt111(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_111;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt111\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt112(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_112;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt112\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt113(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_113;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt113\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt114(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_114;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt114\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt115(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_115;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt115\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt116(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_116;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt116\n\r");
}

void __attribute__((__interrupt__, __no_auto_psv__)) _Interrupt117(void)
{
    void (*isr)(void) = (void (*)(void))INTERRUPT_117;
    if(app_valid)
        isr();
    else
        LOG_E("Unexpected ISR _Interrupt117\n\r");
}
