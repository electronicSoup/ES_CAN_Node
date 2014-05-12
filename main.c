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
#include "es_lib/core.h"
#include "system.h"
#define MAIN
#include "main.h"
#undef MAIN
#define DEBUG_FILE
#include "es_lib/logger/serial.h"
#undef DEBUG_FILE
#include "es_lib/timers/timer_sys.h"
#include "es_lib/utils/utils.h"
#include "es_lib/android/states/states.h"
#ifdef CAN
#include "es_lib/can/es_can.h"
#endif //CAN
#include "es_lib/android/android.h"
#include "es_lib/android/states/states.h"
#include "os_api.h"

#include "usb/usb.h"
#include "usb/usb_host_android.h"


// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
//
// See /opt/microchip/mplabc30/v3.30c/support/PIC24F/h/p24FJ64GB106.h
//
// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
#if defined(PIC24FJ256GB110)
_CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2) // Primary HS OSC with PLL, USBPLL /2
#elif defined(PIC24FJ256GB106)
_CONFIG2(FNOSC_FRCPLL & POSCMOD_NONE & OSCIOFNC_ON & PLL_96MHZ_ON & PLLDIV_NODIV & DISUVREG_OFF)  // CLOCK 16000000
#endif
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & FWPSA_PR32 & WDTPS_PS4096 & WINDIS_OFF & ICS_PGx2)   // JTAG off, watchdog timer on


#define TAG "MAIN"

void (*app_init)(void) = (void (*)(void))0x18096;
void (*app_main)(void) = (void (*)(void))0x1809A;

#if defined(CAN_LAYER_3)
static u8 l3_address = 1;
void get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

static void process_msg_from_android(void);

void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
{
	DEBUG_E("Address error");
	while (1) {
	}
}

void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
{
	DEBUG_E("Stack error");
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
        result_t result;
        u8 l3_address;
	BYTE error_code;
        BYTE magic_1;
        BYTE magic_2;
#ifdef TEST
        char string[10] = "testing";
        char buffer[10];
	BYTE test_byte;
	UINT16 loop;
#endif
	USB_HOST_POWER_PIN_DIRECTION = OUTPUT_PIN;

	serial_init();

	DEBUG_D("************************\n\r");
	DEBUG_D("***   CAN Bus Node   ***\n\r");
	DEBUG_D("************************\n\r");

        psv_strcpy(manufacturer, app_author, 40);
        DEBUG_D("App Author:%s strlen %d\n\r", manufacturer, strlen(manufacturer));

        if(RCONbits.WDTO){
            DEBUG_E("Watch Dog Reset!!!\n\r");
        }

        if(RCONbits.WDTO || strlen(manufacturer) == 0) {
     asm ("CLRWDT");
        DEBUG_D("App Invalid\n\r");
		eeprom_write(APP_VALID_MAGIC, 0x00);
		eeprom_write(APP_VALID_MAGIC + 1, 0x00);
                application_invalid = (APP_INIT_INVALID | APP_MAIN_INVLAID | APP_ISR_INVALID);
        }

     asm ("CLRWDT");
        eeprom_read(APP_VALID_MAGIC, &magic_1);
        eeprom_read(APP_VALID_MAGIC + 1, &magic_2);
        if(  (magic_1 == APP_VALID_MAGIC_VALUE)
           &&(magic_2 == ~APP_VALID_MAGIC_VALUE)) {
            application_invalid = 0x00;
            DEBUG_I("Applicaiton is Valid\n\r");
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
	init_timer();
	spi_init();
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
	strcpypgmtoram(manufacturer, hardware_manufacturer, 24);
	strcpypgmtoram(model, hardware_model, 24);
	strcpypgmtoram(description, hardware_description, 50);
	strcpypgmtoram(version, bootcode_version, 10);
	strcpypgmtoram(uri, firmware_uri, 50);

	DEBUG_D("manufacturer - %s\n\r", manufacturer);
	DEBUG_D("model - %s\n\r", model);
	DEBUG_D("description -%s\n\r", description);
	DEBUG_D("version - %s\n\r", version);
	DEBUG_D("uri - %s\n\r", uri);

        DEBUG_D("Setup android Device Info\n\r");
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
	USB_HOST_POWER_PIN_DIRECTION = OUTPUT_PIN;
	USB_HOST_POWER = 1;

	/*
	 * These next two lines call the Microchip USB Stack functions to
	 * initialise the USB Host and Android functionality
	 */
	USBInitialize(0);
	AndroidAppStart(&android_device_info);

     asm ("CLRWDT");
#if defined(CAN)
        /* ToDo sort out baudRate */
        eeprom_read(CAN_BAUD_RATE, (BYTE *) & baud_rate);
        if(baud_rate >= no_baud) {
		baud_rate = baud_10K;
		DEBUG_W("No CAN Baud Rate set so storing 10KBit/s\n\r");
		eeprom_write(CAN_BAUD_RATE, baud_rate);
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
        if(!application_invalid) {
            app_init();
        }

	/*
	 * Enter the main loop
	 */
	DEBUG_D("Entering the main loop\n\r");
	while(TRUE) {
        asm ("CLRWDT");
		CHECK_TIMERS();

		//Keep the USB stack running
		USBTasks();
#if defined(CAN)
		canTasks();
#endif
		error_code = android_tasks(device_handle);
		if ((error_code != USB_SUCCESS) && (error_code != USB_ENDPOINT_UNRESOLVED_STATE)) {
			DEBUG_D("Error from androidPoll %x\n\r", error_code);
		}

		/*
		 * Check for any messages from the Android device.
		 */
		process_msg_from_android();

                /*
                 * Do whatever functionality is required of the current state.
                 */
		current_state.main();

                if(!application_invalid) {
                    app_main();
                }
	}
}

#if defined(CAN_LAYER_3)
void get_l3_node_address(u8 *address)
{
        eeprom_read(L3_NODE_ADDRESS, address);
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
	DEBUG_D("status_handler(0x%x, 0x%x)\n\r", status, baud);
}
#endif // CAN

/*!
 * \fn void process_msg_from_android(void)
 *
 * \brief processes messages from the android device
 *
 * This function polls the system for messages from the Android device
 */
static void process_msg_from_android(void)
{
	UINT16 size = 0;
	BYTE read_buffer[300];
	BYTE error_code = USB_SUCCESS;

	void *data = NULL;

	size = (UINT16)sizeof (read_buffer);
	while (android_receive((BYTE*) & read_buffer, (UINT16 *)&size, &error_code)) {
		if (error_code != USB_SUCCESS) {
			DEBUG_E("android_receive raised an error %x\n\r", error_code);
		}

		if (size > 1) {
			data = (void *) &read_buffer[1];
		} else {
			data = NULL;
		}

		/*
		 * Pass the received message onto the current state for processing.
		 */
		DEBUG_D("Process Received message in State Machine\n\r");
		current_state.process_msg(read_buffer[0], data, size - 1);
	}
}

#if 1
// Version 2013-12-20  of Microchip Application Libraries
bool USB_ApplicationDataEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
#else
BOOL USB_ApplicationDataEventHandler ( BYTE address, USB_EVENT event, void *data, DWORD size )
#endif
{
	return FALSE;
}

#if 1
// Version 2013-12-20 of Microchip Application Libraries
bool USB_ApplicationEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
#else
BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
#endif
{
	DEBUG_D("USB_ApplicationEventHandler()\n\r");
	switch( event) {
        case EVENT_VBUS_REQUEST_POWER:
		DEBUG_D("EVENT_VBUS_REQUEST_POWER current %d\n\r", ((USB_VBUS_POWER_EVENT_DATA*)data)->current);
		// The data pointer points to a byte that represents the amount of power
		// requested in mA, divided by two.  If the device wants too much power,
		// we reject it.
		if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2)) {
			return TRUE;
		} else {
			DEBUG_E("\r\n***** USB Error - device requires too much current *****\r\n");
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
		DEBUG_D("Device Detached\n\r");
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
		DEBUG_D("Device Attached\n\r");
#if defined(MCP_ANDROID)
		device_attached = TRUE;
#endif
		current_state.process_usb_event(EVENT_ANDROID_ATTACH);
		device_handle = data;
		return TRUE;
                break;

        case EVENT_OVERRIDE_CLIENT_DRIVER_SELECTION:
		DEBUG_D("Ignoring EVENT_OVERRIDE_CLIENT_DRIVER_SELECTION\n\r");
		break;

        default :
		DEBUG_W("Unknown Event 0x%x\n\r", event);
		break;
	}
	return FALSE;
}
