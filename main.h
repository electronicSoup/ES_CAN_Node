/**
 *
 * \file main.h
 *
 * main definitions for the DongleNode project
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
#ifndef MAIN_H
#define MAIN_H

#define HARDWARE_INFO_BASE 0x200
#ifdef MAIN
//
// Hardware Info
//
const char hardware_manufacturer[24] __attribute__ ((space(prog),address(HARDWARE_INFO_BASE)));
const char hardware_model[24]        __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24)));
const char hardware_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24)));
const char hardware_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50)));
const char hardware_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10)));
//
// Bootloader Info
//
const char bootcode_author[40]       __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50)));
const char bootcode_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40)));
const char bootcode_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50)));
const char bootcode_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50 + 10)));
#else
//
// Hardware Info
//
extern const char hardware_manufacturer[24] __attribute__ ((space(prog),address(HARDWARE_INFO_BASE)));
extern const char hardware_model[24]        __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24)));
extern const char hardware_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24)));
extern const char hardware_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50)));
extern const char hardware_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10)));
//
// Bootloader Info
//
extern const char bootcode_author[40]       __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50)));
extern const char bootcode_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40)));
extern const char bootcode_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50)));
extern const char bootcode_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50 + 10)));
/*
 * Next Free
 */
#endif

/*
 * Firmware Info
 */
#define APP_STRINGS_BASE 0x7E00
#ifdef MAIN
const char firmware_author[40]       __attribute__ ((space(prog),address(APP_STRINGS_BASE))) = "electronicSoup";
const char firmware_description[50]  __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40))) = "CAN Node nano OS";
const char firmware_version[10]      __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40 + 50))) = "0.0.1";
const char firmware_uri[50]          __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40 + 50 + 10))) = "http://www.electronicsoup.com/can_node_os";
#else
extern const char firmware_author[40]       __attribute__ ((space(prog),address(APP_STRINGS_BASE)));
extern const char firmware_description[50]  __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40)));
extern const char firmware_version[10]      __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40 + 50)));
extern const char firmware_uri[50]          __attribute__ ((space(prog),address(APP_STRINGS_BASE + 40 + 50 + 10)));
#endif
/*
 * Next Free Address 0x807E
 */

#endif // ifndef MAIN_H
