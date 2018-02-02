/**
 *
 * \file main.c
 *
 * \brief main entry point for the CAN Node OS project
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
#include "libesoup_config.h"

#ifdef SYS_SERIAL_LOGGING
#define DEBUG_FILE
static const char *TAG = "Main";
#include "libesoup/logger/serial_log.h"
#endif // SYS_SERIAL_LOGGING

#include "libesoup/timers/sw_timers.h"
#include "libesoup/comms/can/can.h"
//#include "es_lib/usb/android/android_comms.h"
//#include "es_lib/usb/android/state_idle.h"

#include "libesoup/hardware/eeprom.h"
//#include "es_lib/utils/flash.h"
//#include "es_lib/utils/rand.h"
#include "os_api.h"

//#include "es_lib/can/dcncp/cinnamonbun_info.h"

/*
 *  Microchip USB Includes
 */
#include "usb/inc/usb.h"
#include "usb/inc/usb_host_android.h"

#include "libesoup/firmware/firmware.h"

DEF_FIRMWARE_AUTHOR_40("electronicSoup")
DEF_FIRMWARE_DESCRIPTION_50("CAN Node OS")
DEF_FIRMWARE_VERSION_10("v1.0")
DEF_FIRMWARE_URL_50("http://www.electronicsoup.com")

//
// Hardware Info
//
// This memory will not be copied across with the app to the Hardware but
// is hear to allow us to build.
//
#define HARDWARE_INFO_BASE 0x200

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

#if 0
__prog__ char app_author[40]   __attribute__((space(prog), address(APP_AUTHOR_40_ADDRESS)));
__prog__ char app_software[50] __attribute__((space(prog), address(APP_SOFTWARE_50_ADDRESS)));
__prog__ char app_version[10]  __attribute__((space(prog), address(APP_VERSION_10_ADDRESS)));
__prog__ char app_uri[50]      __attribute__((space(prog), address(APP_URI_50_ADDRESS)));
#endif

void status_handler(can_status_t status, can_baud_rate_t baud);

/*
 * Device handle for the connected USB Device
 */
//static void* device_handle = NULL;

int main(void)
{
	ANDROID_ACCESSORY_INFORMATION android_device_info;

	char            manufacturer[40] = "";
	char            model[50] = "";
	char            version[10] = "";
	char            uri[50] = "";
	can_baud_rate_t baud_rate;
	uint8_t         magic_1;
	uint8_t         magic_2;
	uint16_t        length;
	uint8_t         wdt_status;
	result_t        rc = SUCCESS;

	libesoup_init();
#if (defined(SYS_SERIAL_LOGGING) && defined(DEBUG_FILE) && (SYS_LOG_LEVEL <= LOG_DEBUG))
	LOG_D("************************\n\r");
	LOG_D("***   CAN Bus Node   ***\n\r");
	LOG_D("************************\n\r");
#endif
	/*
	 * Check to see has the Bootloader reset by the Watchdog and if so
	 * Invlaidate the Installed App!
	 */
	rc = eeprom_read(EEPROM_WDR_PROTOCOL_ADDR, &wdt_status);
	if(rc != SUCCESS) {
#if (defined(SYS_SERIAL_LOGGING) && (SYS_LOG_LEVEL <= LOG_ERROR))
		LOG_E("Failed to read EEPROM\n\r");
#endif
	}

	if(wdt_status & WDR_PROCESSOR_RESET_BY_WATCHDOG) {
		/*
		 * Watchdog timer reset to invalidate App
		 */
		eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_1, 0x00);
		eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_2, 0x00);
		LOG_I("Application is Watchdog was reset!\n\r");
		app_valid = FALSE;
	} else {
		length = 40;
		flash_strcpy(manufacturer, app_author, &length);
		LOG_D("App Author:%s strlen %d\n\r", manufacturer, strlen(manufacturer));

		asm ("CLRWDT");

		if (strlen(manufacturer) == 0) {
			eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_1, 0x00);
			eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_2, 0x00);
			LOG_I("Application is NOT Valid Bad String!\n\r");
			app_valid = FALSE;
		} else {
			eeprom_read(EEPROM_APP_VALID_MAGIC_ADDR_1, &magic_1);
			eeprom_read(EEPROM_APP_VALID_MAGIC_ADDR_2, &magic_2);
			LOG_D("Read Magic 1 0x%x-0x%x\n\r", magic_1, APP_VALID_MAGIC_VALUE);
			LOG_D("Read Magic 2 0x%x-0x%x\n\r", magic_2, (uint8_t) (~APP_VALID_MAGIC_VALUE));
			if ((magic_1 == APP_VALID_MAGIC_VALUE)
				&& (magic_2 == (uint8_t) (~APP_VALID_MAGIC_VALUE))) {
				app_valid = TRUE;
				LOG_I("Application is Valid\n\r");
			} else {
				LOG_I("Application is NOT Valid Bad Magic! 0x%x-0x%x\n\r", magic_1, magic_2);
				app_valid = FALSE;
			}
		}
	}

	asm ("CLRWDT");

	rc = eeprom_write(EEPROM_WDR_PROTOCOL_ADDR, WDR_DO_NOT_INVALIDATE_FIRMWARE);
	if(rc != SUCCESS) {
#if (defined(SYS_SERIAL_LOGGING) && (SYS_LOG_LEVEL <= LOG_ERROR))
		LOG_E("Failed to write to EEPROM\n\r");
#endif
	}

	asm ("CLRWDT");

        /*
	 * Initialise the OS structures.
	 */
	os_init_data();

	/**
	 * Initialise the current state of the Android Sate machine to Idle
	 * [The link text](@ref set_idle_state)
	 */
	set_idle_state();

	asm ("CLRWDT");
	/*
	 * Set up the details that we're going to pass to a connected Android
	 * Device.
	 */
	length = 40;
	flash_strcpy(manufacturer, firmware_author, &length);

	length = 50;
	flash_strcpy(model, firmware_description, &length);

	length = 10;
	flash_strcpy(version, firmware_version, &length);

	length = 50;
	flash_strcpy(uri, firmware_uri, &length);

	LOG_D("manufacturer - %s\n\r", manufacturer);
	LOG_D("model - %s\n\r", model);
	LOG_D("version - %s\n\r", version);
	LOG_D("uri - %s\n\r", uri);

        LOG_D("Setup android Device Info\n\r");
	android_device_info.manufacturer = manufacturer;
	android_device_info.manufacturer_size = sizeof(manufacturer);
	android_device_info.model = model;
	android_device_info.model_size = sizeof(model);
	android_device_info.description = model;
	android_device_info.description_size = sizeof(model);
	android_device_info.version = version;
	android_device_info.version_size = sizeof(version);
	android_device_info.URI = uri;
	android_device_info.URI_size = sizeof(uri);

	asm ("CLRWDT");
	/*
	 * Turn on the power to the USB Port as we're going to use Host Mode
	 */
	USB_HOST

	/*
	 * These next two lines call the Microchip USB Stack functions to
	 * initialise the USB Host and Android functionality
	 */
	AndroidAppStart(&android_device_info);

	asm ("CLRWDT");

#ifdef CAN
	eeprom_read(EEPROM_CAN_BAUD_RATE_ADDR, (u8 *) & baud_rate);
        if(baud_rate >= no_baud) {
		baud_rate = baud_10K;
		LOG_W("No CAN Baud Rate set so storing 10KBit/s\n\r");
		eeprom_write(EEPROM_CAN_BAUD_RATE_ADDR, baud_rate);
        }

	can_init(baud_10K, status_handler);
#endif
	asm ("CLRWDT");
        if(app_valid) {
		LOG_D("Call App Init as Application is valid\n\r");
		CALL_APP_INIT();
        }

	/*
	 * Enter the main loop
	 */
	LOG_D("Entering the main loop\n\r");
	while(TRUE) {
		asm ("CLRWDT");
		CHECK_TIMERS();

		//Keep the USB stack running
		USBTasks();
#ifdef CAN
		can_tasks();
#endif
                asm ("CLRWDT");

		if (app_valid) {
			CALL_APP_MAIN();
		}
	}
}

/**
 *
 *
 * Don't let the ISO11783 Broadcast address be used.
 *
 */
uint8_t node_get_address(void)
{
	static uint8_t retry_count = 0;
	result_t rc;
	uint8_t address;

	if(retry_count == 0) {
		rc = eeprom_read(EEPROM_NODE_ADDRESS, &address);
		if (rc != SUCCESS) {
			LOG_E("address Failed to read from eeprom return code 0x%x\n\r", rc);
			do {
				address = (uint8_t) (rand() & 0x0ff);
			} while (address == BROADCAST_NODE_ADDRESS);

			rc = eeprom_write(EEPROM_NODE_ADDRESS, address);
			if (rc != SUCCESS) {
				LOG_E("Failed to write from eeprom return code 0x%x\n\r", rc);
			}
		} else {
			LOG_D("address value read back from EEPROM address 0x02 = 0x%x\n\r", address);
		}
	} else {
		do {
			address = (uint8_t) (rand() & 0x0ff);
		} while (address == BROADCAST_NODE_ADDRESS);

		rc = eeprom_write(EEPROM_NODE_ADDRESS, address);
		if (rc != SUCCESS) {
			LOG_E("Failed to write from eeprom return code 0x%x\n\r", rc);
		}
	}

	retry_count++;

	LOG_D("node_get_address() Retry %d try address 0x%x\n\r", retry_count, address);
	return(address);
}

void status_handler(can_status_t status, can_baud_rate_t baud)
{
#ifdef SYS_CAN
	LOG_D("status_handler()\n\r");
	LOG_D("Baud Rate is : %s\n\r", can_baud_rate_strings[baud]);
	LOG_D("Layer 2 status : %s\n\r", can_l2_status_strings[status.bit_field.l2_status]);

	if(status.bit_field.dcncp_initialised) {
		LOG_D("DCNCP Status : Initialised\n\r");
	}
#endif
}

