/**
 *
 * \file state_android_app_connected.c
 *
 * The Android Application Connected state for the Android Communications.
 *
 * State for processing comms from the Android Application.
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
#include <stdio.h>
#include "system.h"
//#include "usb/usb.h"
//#include "usb/usb_host_android.h"
#include "usb/inc/usb.h"
#include "usb/inc/usb_host_android.h"
#include "es_lib/usb/android/state.h"
#include "es_lib/usb/android/state_idle.h"
#include "es_lib/utils/flash.h"
#include "es_lib/utils/eeprom.h"
#include "es_lib/usb/android/android_comms.h"
#include "es_lib/can/dcncp/cinnamonbun_info.h"
#include "os_api.h"

#define DEBUG_FILE
#include "es_lib/logger/serial_log.h"

#define TAG "Android-NodeConnected"

void app_connected_process_msg(u8, void *, u16);
void app_connected_main(void);
void app_connected_process_usb_event(USB_EVENT event);

static void transmit_ready(void);
//static result_t transmit_hardware_info(void);
//static result_t transmit_bootcode_info(void);
//static result_t transmit_firmware_info(void);
static result_t transmit_application_info(void);
static result_t transmit_node_config_info(void);


void set_app_connected_state(void)
{
	LOG_D("State -> node_connected\n\r");
	// Android App connected so cancel the timer.
	// Android App now controls booting

	android_state.process_msg = app_connected_process_msg;
	android_state.main = app_connected_main;
	android_state.process_usb_event = app_connected_process_usb_event;
}

void app_connected_process_msg(u8 cmd, void *data, u16 data_len)
{
	u32 address;
	u8 *byte_data;
	u8 loop;
	result_t rc;
	u16 length;

	switch (cmd) {
		case APP_MSG_FLASH_REPROGRAM:
			LOG_D("COMMAND_BEGIN_FLASH\n\r");
			os_remove_current_app();
			rc = eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_1, 0x00);
			if(rc != SUCCESS) {
				LOG_E("Error writing to eeprom\n\r");
			}
			rc = eeprom_write((EEPROM_APP_VALID_MAGIC_ADDR_2), 0x00);
			if(rc != SUCCESS) {
				LOG_E("Error writing to eeprom\n\r");
			}
			app_valid = FALSE;
			transmit_ready();
			break;

		case APP_MSG_FLASH_ERASE_PAGE:
			if (data != NULL) {
				byte_data = (u8 *) data;

				address = 0x00;
				for (loop = 0; loop < 4; loop++) {
					address = (address << 8) | (byte_data[loop] & 0xff);
				}

				LOG_I("COMMAND_ERASE 0x%lx\n\r", address);
				if(  (address < APP_START_FLASH_ADDRESS)
				   &&(address != APP_HANDLE_FLASH_ADDRESS)) {
					LOG_E("Bad address to Erase\n\r");
				} else {
					if (!flash_page_empty(address)) {
						rc = flash_erase_page(address);
						if(rc != SUCCESS) {
							LOG_E("Erase page 0x%lx", address);
						}
					}
					transmit_ready();
				}
			}
			break;

		case APP_MSG_FLASH_WRITE_ROW:
			if (data != NULL) {
				byte_data = (u8 *) data;

				address = 0x00;
				for (loop = 0; loop < 4; loop++) {
					address = (address << 8) | (byte_data[loop] & 0xff);
				}

				if(  (address < APP_START_FLASH_ADDRESS)
				   &&((address < APP_HANDLE_FLASH_ADDRESS) || (address >= APP_HANDLE_FLASH_ADDRESS + FLASH_PAGE_SIZE))) {
					LOG_E("Bad address to Write to row\n\r");
				} else {
					rc = flash_write_row(address, &byte_data[4]);
					if(rc != SUCCESS) {
						LOG_E("write row to address 0x%lx\n\r", address);
					}
					transmit_ready();
				}
			}
			break;

		case APP_MSG_FLASH_REFLASHED:
			LOG_D("COMMAND_REFLASHED\n\r");
			CALL_APP_INIT();
			LOG_D("Back from app_init() call app_main()\n\r");
			CALL_APP_MAIN();
			LOG_D("Application is valid\n\r");
			app_valid = TRUE;

			if (eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_1, APP_VALID_MAGIC_VALUE) != SUCCESS) {
				LOG_E("Bad EEPROM Write\n\r");
			}
			if (eeprom_write(EEPROM_APP_VALID_MAGIC_ADDR_2, (u8) (~APP_VALID_MAGIC_VALUE)) != SUCCESS) {
				LOG_E("BAD EEPROM Write\n\r");
			}

			break;
#if 0
		case APP_HARDWARE_INFO_REQ:
			LOG_D("HARDWARE_INFO_REQ\n\r");
			rc = transmit_hardware_info();
			if(rc != SUCCESS) {
				LOG_E("Failed Tx\n\r");
			}
			break;

		case APP_BOOTCODE_INFO_REQ:
			LOG_D("BOOTCODE_INFO_REQ\n\r");
			rc = transmit_bootcode_info();
			if(rc != SUCCESS) {
				LOG_E("Failed Tx\n\r");
			}
			break;

		case APP_FIRMWARE_INFO_REQ:
			LOG_D("FIRMWARE_INFO_REQ\n\r");
			rc = transmit_firmware_info();
			if(rc != SUCCESS) {
				LOG_E("Failed Tx\n\r");
			}
			break;
#endif
		case APP_APPLICATION_INFO_REQ:
			LOG_D("APPLICATION_INFO_REQ\n\r");
			rc = transmit_application_info();
			if(rc != SUCCESS) {
				LOG_E("Failed Tx\n\r");
			}
			break;

		case APP_NODE_CONFIG_INFO_REQ:
			LOG_D("NODE_CONFIG_INFO_REQ\n\r");
			rc = transmit_node_config_info();
			if(rc != SUCCESS) {
				LOG_E("Failed Tx\n\r");
			}
			break;

		case APP_NODE_CONFIG_INFO_UPDATE:
			byte_data = (u8 *) data;
			LOG_D("NODE_CONFIG_UPDATE\n\r");
			LOG_D("data[0] = 0x%x\n\r", byte_data[0]);
			LOG_D("data[1] = 0x%x\n\r", byte_data[1]);
			LOG_D("data[2] = 0x%x\n\r", byte_data[2]);
			LOG_D("descrioption String is %s\n\r", &byte_data[3]);

			rc = eeprom_write(EEPROM_NODE_ADDRESS, byte_data[0]);
			if(rc != SUCCESS) {
				LOG_E("Error writing to eeprom\n\r");
			}

			rc = eeprom_write(EEPROM_CAN_BAUD_RATE_ADDR, byte_data[1]);
			if(rc != SUCCESS) {
				LOG_E("Error writing to eeprom\n\r");
			}

			rc = eeprom_write(EEPROM_IO_ADDRESS_ADDR, byte_data[2]);
			if(rc != SUCCESS) {
				LOG_E("Error writing to eeprom\n\r");
			}

			/*
			 * The Node Descrition is limited to being a 30 Byte string
			 * including the null terminator!
			 */
			length = 30;
			rc = eeprom_str_write(EEPROM_NODE_DESCRIPTION_ADDR, &byte_data[3], &length);
			if(rc != SUCCESS) {
				LOG_E("Error writing node description to eeprom\n\r");
			}
			LOG_D("%d bytes written to EEPROM\n\r", length);
			break;

		default:
			LOG_W("Unprocessed 0x%x\n\r", cmd);
			break;
	}
}

void app_connected_main()
{
}

void app_connected_process_usb_event(USB_EVENT event)
{
	switch (event) {
		case EVENT_ANDROID_ATTACH:
			break;

		case EVENT_ANDROID_DETACH:
			set_idle_state();
			break;

		default:
			break;
	}
}

static void transmit_ready(void)
{
	u8 buffer[3];

	buffer[0] = 0;
	buffer[1] = 1;
	buffer[2] = BUN_COMMAND_READY;

	android_transmit(buffer, 3);
}
#if 0
static void transmit_app_type_info(void)
{
	char buffer[3];

	buffer[0] = 2;
	buffer[1] = BUN_ANDROID_APP_TYPE_RESP;
//	buffer[2] = NODE_CONFIG_APP;
	android_transmit((u8 *) buffer, 3);
}
#endif

#if 0
result_t transmit_hardware_info(void)
{
	char buffer[160];
	u8 index = 0;
	UINT16 length;
	result_t rc;

	buffer[1] = BUN_HARDWARE_INFO_RESP;

	index = 2;

	length = 24;
	rc = flash_strcpy(&buffer[index], hardware_manufacturer, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 24;
	rc = flash_strcpy(&buffer[index], hardware_model, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	rc = flash_strcpy(&buffer[index], hardware_description, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 10;
	rc = flash_strcpy(&buffer[index], hardware_version, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	rc = flash_strcpy(&buffer[index], hardware_uri, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	LOG_D("Tx boot info length %d\n\r", index);
	buffer[0] = index;

	android_transmit((u8 *) buffer, index);
	return(SUCCESS);
}
#endif

#if 0
result_t transmit_bootcode_info(void)
{
	//    char string[50];
	char buffer[152];
	u8 index = 0;
	UINT16 length;
	result_t rc;

	buffer[1] = BUN_BOOTCODE_INFO_RESP;

	index = 2;

	length = 40;
	rc = flash_strcpy(&buffer[index], bootcode_author, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	index += flash_strcpy(&buffer[index], bootcode_description, &length) + 1;
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 10;
	index += flash_strcpy(&buffer[index], bootcode_version, &length) + 1;
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	rc = flash_strcpy(&buffer[index], bootcode_uri, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	LOG_D("Tx boot info length %d\n\r", index);
	buffer[0] = index;

	android_transmit((u8 *) buffer, index);
	return(SUCCESS);
}
#endif

#if 0
result_t transmit_firmware_info(void)
{
	char buffer[152];
	UINT16 index = 0;
	UINT16 length;
	result_t rc;

	LOG_D("transmit_firmware_info()\n\r");
	buffer[1] = BUN_FIRMWARE_INFO_RESP;

	index = 2;

	length = 40;
	rc = flash_strcpy(&buffer[index], firmware_author, &length);
	LOG_D("firmware author length %d\n\r", length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	rc = flash_strcpy(&buffer[index], firmware_description, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 10;
	rc = flash_strcpy(&buffer[index], firmware_version, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	length = 50;
	rc = flash_strcpy(&buffer[index], firmware_uri, &length);
	if(rc != SUCCESS) {
		return(rc);
	}
	index += length + 1;

	LOG_D("Tx Firmware info length %d\n\r", index);
	buffer[0] = index;

	android_transmit((u8 *) buffer, index);
	return(SUCCESS);
}
#endif

result_t transmit_application_info(void)
{
	u8       buffer[200];
	u16      length;
	result_t rc;

	buffer[2] = BUN_APPLICATION_INFO_RESP;

	buffer[3] = app_valid;

	if(app_valid) {
		length = 195;
		rc = cb_get_application_info(&buffer[4], &length);
		if(rc != SUCCESS) {
			return(rc);
		}
	}
	LOG_D("Tx Application info length %d\n\r", length + 2);
	buffer[0] = 0x00;
	buffer[1] = length + 2;

	android_transmit(buffer, length + 4);
	return (SUCCESS);
}

result_t transmit_node_config_info(void)
{
	u8  buffer[56];
	u16 length;
	result_t rc;

	buffer[2] = BUN_NODE_CONFIG_INFO_RESP;

	length = 195;
	rc = cb_get_node_config_info(&buffer[4], &length);
	if (rc != SUCCESS) {
		return (rc);
	}

	LOG_D("Tx Node Config info length %d\n\r", length + 2);
	buffer[0] = 0x00;
	buffer[1] = length + 1;

	android_transmit(buffer, length + 3);
	return (SUCCESS);
}
