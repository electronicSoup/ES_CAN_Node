/**
 *
 * \file main.c
 *
 * \brief main entry point for the DongleNode project
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
#include <stdio.h>
//#define MAIN
//#include "main.h"
//#undef MAIN
#include "es_lib/logger/serial_port.h"
#define DEBUG_FILE
#include "es_lib/logger/serial_log.h"

#include "es_lib/timers/timers.h"
#include "es_lib/can/es_can.h"
#include "es_lib/usb/android/android.h"
#include "es_lib/usb/android/android_state.h"
#include "es_lib/usb/android/states/states.h"
#include "es_lib/utils/eeprom.h"
#include "es_lib/utils/spi.h"
#include "es_lib/utils/flash.h"
#include "es_lib/utils/rand.h"
#include "os/os_api.h"

/*
 *  Microchip USB Includes
 */
#include "usb/usb.h"
#include "usb/usb_host_android.h"

#include "es_lib/firmware/firmware.h"

DEF_FIRMWARE_AUTHOR_40("me@mail.com")
DEF_FIRMWARE_DESCRIPTION_50("Serial_Firmware")
DEF_FIRMWARE_VERSION_10("v1.0")
DEF_FIRMWARE_URL_50("www.test.com")

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

#define APPLICATION_STRINGS_BASE 0x18000

__prog__ char app_author[40] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE)));
__prog__ char app_software[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40)));
__prog__ char app_version[10] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50)));
__prog__ char app_uri[50] __attribute__((space(prog), address(APPLICATION_STRINGS_BASE + 40 + 50 + 10)));

#define TAG "MAIN"

#if defined(CAN_LAYER_3)
static u8 l3_address = 1;
void get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
{
	LOG_E("Address error");
	while (1) {
	}
}

void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
{
	LOG_E("Stack error");
	while (1)  {
	}
}

/*
 * Interrupt Handler for the USB Host Functionality of Microchip USB Stack.
 */
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
        USB_HostInterruptHandler();
}

#if defined(CAN)
void status_handler(can_status_t status, baud_rate_t baud);
#endif

/*
 * Device handle for the connected USB Device
 */
static void* device_handle = NULL;

/*
 * This Bootloader holds a state machine which manages the comms with the
 * connected Android device. Activity is State dependant
 */
state_t current_state;

int main(void)
{
	char manufacturer[24] = "";
	char model[24] = "";
	char description[50] = "";
	char version[10] = "";
	char uri[50] = "";
	ANDROID_ACCESSORY_INFORMATION android_device_info;
#if defined(CAN)
        baud_rate_t baud_rate;
#endif // CAN
	BYTE error_code;
        BYTE magic_1;
        BYTE magic_2;

	serial_init();

	LOG_D("************************\n\r");
	LOG_D("***   CAN Bus Node   ***\n\r");
	LOG_D("************************\n\r");
        random_init();
        spi_init();

        if(RCONbits.WDTO) {
            LOG_E("Watch Dog Reset!\n\r");
        }

        psv_strcpy(manufacturer, firmware_author, 40);
        LOG_D("App Author:%s strlen %d\n\r", manufacturer, strlen(manufacturer));

        asm ("CLRWDT");

        if(strlen(manufacturer) == 0) {
		eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR, 0x00);
		eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR + 1, 0x00);
		LOG_I("Applicaiton is NOT Valid Bad String!\n\r");
                app_valid = FALSE;
        } else {
		eeprom_read(EEPROM_APP_VALID_MAGIC_ADDR, &magic_1);
		eeprom_read(EEPROM_APP_VALID_MAGIC_ADDR + 1, &magic_2);
		LOG_D("Read Magic 1 0x%x-0x%x\n\r", magic_1, APP_VALID_MAGIC_VALUE);
		LOG_D("Read Magic 2 0x%x-0x%x\n\r", magic_2, (u8)(~APP_VALID_MAGIC_VALUE));
		if(  (magic_1 == APP_VALID_MAGIC_VALUE)
		   &&(magic_2 == (u8)(~APP_VALID_MAGIC_VALUE)) ) {
			app_valid = TRUE;
			LOG_I("Applicaiton is Valid\n\r");
		} else {
			LOG_I("Applicaiton is NOT Valid Bad Magic! 0x%x-0x%x\n\r", magic_1, magic_2);
			app_valid = FALSE;
		}
        }

        /*
	 * Reset the Watch Dog timer flag so we can detect the next one.
	 */
        RCONbits.WDTO = 0;

	asm ("CLRWDT");

#ifdef HEARTBEAT
	HEARTBEAT_LED_DIRECTION = OUTPUT_PIN;
	heartbeat_off((BYTE *)NULL);
#endif
	timer_init();
	android_init();

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
	psv_strcpy(manufacturer, hardware_manufacturer, 24);
	psv_strcpy(model, hardware_model, 24);
	psv_strcpy(description, hardware_description, 50);
	psv_strcpy(version, bootcode_version, 10);
	psv_strcpy(uri, firmware_uri, 50);

	LOG_D("manufacturer - %s\n\r", manufacturer);
	LOG_D("model - %s\n\r", model);
	LOG_D("description -%s\n\r", description);
	LOG_D("version - %s\n\r", version);
	LOG_D("uri - %s\n\r", uri);

        LOG_D("Setup android Device Info\n\r");
	android_device_info.manufacturer = manufacturer;
	android_device_info.manufacturer_size = sizeof(manufacturer);
	android_device_info.model = model;
	android_device_info.model_size = sizeof(model);
	android_device_info.description = description;
	android_device_info.description_size = sizeof(description);
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
#if defined(CAN)
        /* ToDo sort out baudRate */
        eeprom_read(EEPROM_CAN_BAUD_RATE_ADDR, (BYTE *) & baud_rate);
        if(baud_rate >= no_baud) {
		baud_rate = baud_10K;
		LOG_W("No CAN Baud Rate set so storing 10KBit/s\n\r");
		eeprom_write(EEPROM_CAN_BAUD_RATE_ADDR, baud_rate);
        }

	// Send in null we're not defining a default handler for messages
	// if nothing's regestered an interest we're just not interested
	can_init(baud_10K, status_handler);
#endif // CAN
	//    enable_interrupts();
#ifdef HEARTBEAT
	heartbeat_on(NULL);
#endif
	asm ("CLRWDT");
        if(app_valid) {
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
#if defined(CAN)
		canTasks();
                asm ("CLRWDT");
#endif
		error_code = android_tasks(device_handle);
		if ((error_code != USB_SUCCESS) && (error_code != USB_ENDPOINT_UNRESOLVED_STATE)) {
			LOG_D("Error from androidPoll %x\n\r", error_code);
		}

                if(app_valid) {
			CALL_APP_MAIN();
                }
	}
}

#if defined(CAN_LAYER_3)
void get_l3_node_address(u8 *address)
{
        eeprom_read(EEPROM_L3_NODE_ADDRESS_ADDR, address);
}
#endif

#if defined(CAN_LAYER_3)
result_t get_new_l3_node_address(u8 *address)
{
	*address = ++l3_address;
	return(SUCCESS);
//        sys_eeprom_write(NODE_ADDRESS, (u8) rand());
}
#endif


#if defined(CAN)
void status_handler(can_status_t status, baud_rate_t baud)
{
	LOG_D("status_handler(0x%x, 0x%x)\n\r", status, baud);
}
#endif // CAN

bool USB_ApplicationDataEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
	return FALSE;
}

bool USB_ApplicationEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
	LOG_D("USB_ApplicationEventHandler()\n\r");
	switch( event) {
        case EVENT_VBUS_REQUEST_POWER:
		LOG_D("EVENT_VBUS_REQUEST_POWER current %d\n\r", ((USB_VBUS_POWER_EVENT_DATA*)data)->current);
		// The data pointer points to a byte that represents the amount of power
		// requested in mA, divided by two.  If the device wants too much power,
		// we reject it.
		if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2)) {
			return TRUE;
		} else {
			LOG_E("\r\n***** USB Error - device requires too much current *****\r\n");
		}
		break;

        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
        case EVENT_DETACH:              // USB cable has been detached (data: BYTE, address of device)
        case EVENT_ANDROID_DETACH:
		/*
		 * Pass a Detach event on to the state machine for processing.
		 */
		LOG_D("Device Detached\n\r");
#if defined(MCP_ANDROID)
		device_attached = FALSE;
#endif
		current_state.process_usb_event(EVENT_ANDROID_DETACH);
		device_handle = NULL;
		return TRUE;
		break;

	// Android Specific events
        case EVENT_ANDROID_ATTACH:
		/*
		 * Pass an Attach even on to the State machine.
		 */
		LOG_D("Device Attached\n\r");
#if defined(MCP_ANDROID)
		device_attached = TRUE;
#endif
		current_state.process_usb_event(EVENT_ANDROID_ATTACH);
		device_handle = data;
		return TRUE;
                break;

        case EVENT_OVERRIDE_CLIENT_DRIVER_SELECTION:
		LOG_D("Ignoring EVENT_OVERRIDE_CLIENT_DRIVER_SELECTION\n\r");
		break;

        default :
		LOG_W("Unknown Event 0x%x\n\r", event);
		break;
	}
	return FALSE;
}
