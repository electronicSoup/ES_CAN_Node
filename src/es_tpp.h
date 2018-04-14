/**
 * @file es_tpp.h
 *
 * @author John Whitmore
 * 
 * @brief CAN Identifiers and structures for The Proposed Protocol 
 *
 * Copyright 2018 electronicSoup
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 3 of the GNU General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#define SWITCH_INPUT_STATUS                0x140
#define SWITCH_INPUT_STATUS_REQ            0x141
#define SWITCH_INPUT_STATUS_RESP           0x142

#define SWITCH_OUTPUT_STATUS               0x146
#define SWITCH_OUTPUT_STATUS_REQ           0x147
#define SWITCH_OUTPUT_STATUS_RESP          0x148
//#define SWITCH_OUTPUT_ERROR                0x149

union switch_status {
	struct {
		uint8_t   status:1;
		uint8_t   channel:3;
		uint8_t   node:4;
	} bitfield;
	uint8_t byte;
};


#define PERCENTAGE_INPUT_STATUS            0x150
#define PERCENTAGE_INPUT_STATUS_REQ        0x151
#define PERCENTAGE_INPUT_STATUS_RESP       0x152

#define PERCENTAGE_OUTPUT_STATUS           0x156
#define PERCENTAGE_OUTPUT_STATUS_REQ       0x157
#define PERCENTAGE_OUTPUT_STATUS_RESP      0x158
#define PERCENTAGE_OUTPUT_ERROR            0x159

union percentage_status {
	struct {
		uint8_t   precentage;
		uint8_t   channel:3;
		uint8_t   node:4;
	} bitfield;
	uint16_t byte;
};

