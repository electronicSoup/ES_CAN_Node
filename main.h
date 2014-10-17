/**
 *
 * \file main.h
 *
 * Definitions for strings in FLASH memory
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
// This memory will not be copied across with the app to the Hardware but
// is hear to allow us to build.
//
__prog__ char hardware_manufacturer[24] __attribute__ ((space(prog),address(HARDWARE_INFO_BASE))) = "electronicSoup";
__prog__ char hardware_model[24]        __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24))) = "Cinnamon Bun";
__prog__ char hardware_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24))) = "CAN Bus Node dev Platform";
__prog__ char hardware_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50))) = "1.0.0";
__prog__ char hardware_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10))) = "http://www.electronicsoup.com/cinnamon_bun";
//
// Bootloader Info
//
__prog__ char bootcode_author[40]       __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50))) = "electronicSoup";
__prog__ char bootcode_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40))) = "Android Bootloader";
__prog__ char bootcode_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50))) = "1.0.0";
__prog__ char bootcode_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50 + 10))) = "http://www.electronicsoup.com/bootloader";
#else
//
// Hardware Info
//
extern __prog__ char hardware_manufacturer[24] __attribute__ ((space(prog),address(HARDWARE_INFO_BASE)));
extern __prog__ char hardware_model[24]        __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24)));
extern __prog__ char hardware_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24)));
extern __prog__ char hardware_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50)));
extern __prog__ char hardware_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10)));
//
// Bootloader Info
//
extern __prog__ char bootcode_author[40]       __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50)));
extern __prog__ char bootcode_description[50]  __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40)));
extern __prog__ char bootcode_version[10]      __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50)));
extern __prog__ char bootcode_uri[50]          __attribute__ ((space(prog),address(HARDWARE_INFO_BASE + 24 + 24 + 50 + 10 + 50 + 40 + 50 + 10)));
/*
 * Next Free
 */
#endif

/*
 * Firmware Info
 */
#define FIRMWARE_STRINGS_BASE 0x8800
#ifdef MAIN
__prog__ char firmware_author[40]       __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE))) = "electronicsoup.com";
__prog__ char firmware_description[50]  __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40))) = "Cinnamon Bun";
__prog__ char firmware_version[10]      __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40 + 50))) = "1.0";
__prog__ char firmware_uri[50]          __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40 + 50 + 10))) = "www.electronicsoup.com/can_node_os";
#else
extern __prog__ char firmware_author[40]       __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE)));
extern __prog__ char firmware_description[50]  __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40)));
extern __prog__ char firmware_version[10]      __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40 + 50)));
extern __prog__ char firmware_uri[50]          __attribute__ ((space(prog),address(FIRMWARE_STRINGS_BASE + 40 + 50 + 10)));
#endif

#define APPLICATION_STRINGS_BASE 0x18000
#ifdef MAIN
__prog__ char app_author[40] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE)));
__prog__ char app_software[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40)));
__prog__ char app_version[10] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50)));
__prog__ char app_uri[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50 + 10)));
#else
extern __prog__ char app_author[40] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE)));
extern __prog__ char app_software[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40)));
extern __prog__ char app_version[10] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50)));
extern __prog__ char app_uri[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50 + 10)));
#endif //MAIN

#endif // ifndef MAIN_H
