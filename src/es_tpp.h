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
/**
 * @brief Switch (43) inputs and outputs. All the stuff which is simply On or Off
 * 
 * The '43' refers to the breakdown of the transmitted switch status (Input/Output)
 * information. This message identifier indicates that there are 2^4 addressable
 * nodes (IO Node Address) each with potentially 2^3 Input or Output channels.
 * 
 * This implies that the Frames carry 8 bit data structures defined below. A future
 * addition may add a new structure with for example 4 addressable nodes each
 * with potentially 32 Input or Output channels. Another frame identifier might
 * use more then 8 bits to encode the carried information, (reducing the number
 * of inputs or output which can be combined in a single CAN Frame). That is for
 * other Identifiers and associated structures. The following CAN Identifier and 
 * associated C structure are for the 16/8 encoding.
 * 
 */
#define SWITCH_43_INPUT_STATUS                0x140
#define SWITCH_43_INPUT_STATUS_REQ            0x141
#define SWITCH_43_INPUT_STATUS_RESP           0x142

#define SWITCH_43_OUTPUT_STATUS               0x146
#define SWITCH_43_OUTPUT_STATUS_REQ           0x147
#define SWITCH_43_OUTPUT_STATUS_RESP          0x148

/**
 * @brief The structure for transmission of '43' Switch Information
 * 
 * This particular structure allows for up to 16 Nodes each with 8 Input or 
 * Output Channels, so a total of 128 Input and 128 Outputs in the system.
 * 
 * NOTE :- There can be more then one I/O Nodes with an address. There can for 
 * example be two CAN Nodes with the same I/O Node address so long as only one 
 * of the nodes with that address is processing Switch Inputs messages. So there
 * can be an I/O Node address 0x01 processing Switch Input frames and another
 * node with I/O Node address 0x01 processing Switch Output frames. 
 */
union switch_43_status {
	struct {
		uint8_t   status:1;
		uint8_t   channel:3;
		uint8_t   io_node:4;
	} bitfield;
	uint8_t byte;
};


/**
 * @brief Switch (52) inputs and outputs. All the stuff which is simply On or Off
 * 
 * The '52' refers to the breakdown of the transmitted switch status (Input/Output)
 * information. This message identifier indicates that there are 2^5 addressable
 * nodes (IO Node Address) each with potentially 2^2 Input or Output channels.
 * 
 */
#define SWITCH_52_INPUT_STATUS                0x240
#define SWITCH_52_INPUT_STATUS_REQ            0x241
#define SWITCH_52_INPUT_STATUS_RESP           0x242

#define SWITCH_52_OUTPUT_STATUS               0x246
#define SWITCH_52_OUTPUT_STATUS_REQ           0x247
#define SWITCH_52_OUTPUT_STATUS_RESP          0x248

/**
 * @brief The structure for transmission of '52' Switch Information
 * 
 * This particular structure allows for up to 2^5 Nodes each with 2^2 Input or 
 * Output Channels.
 */
union switch_52_status {
	struct {
		uint8_t   status:1;
		uint8_t   channel:2;
		uint8_t   io_node:5;
	} bitfield;
	uint8_t byte;
};


/**
 * @brief byte (53) inputs and outputs. All the stuff which an 8 bit Byte data type
 * 
 * The '53' refers to the breakdown of the transmitted byte status (Input/Output)
 * information. This message identifier indicates that there are 2^5 addressable
 * nodes (IO Node Address) each with potentially 2^3 Input or Output channels.
 * 
 * As in programming an 8 bit byte can encode a wide variety of information,
 * an unsigned 8 bit number, a signed 8 bit number, a colour, an ascii character.
 * The origin for its inclusion in the protocol is the windscreen wipers of the
 * New Holland 8360. In a wiper control system the electronics are not as simple
 * as simply on and off. The Input switch allows the user to select a number
 * of speeds of operation. Similarly the actual Output Motor sets a speed as
 * required. In the case of the NH8360 there are only 2 active settings, slow 
 * and fast, and an off position. These will be encoded on the Input as positions
 * 0,1 & 2 Similarly sending a Output of 2 to the wiper motor will set the fast
 * speed of operation.
 */
#define BYTE_53U8_INPUT_STATUS                0x250
#define BYTE_52U8_INPUT_STATUS_REQ            0x251
#define BYTE_52U8_INPUT_STATUS_RESP           0x252

#define BYTE_53U8_OUTPUT_STATUS               0x256
#define BYTE_53U8_OUTPUT_STATUS_REQ           0x257
#define BYTE_52U8_OUTPUT_STATUS_RESP          0x258

/**
 * @brief The structure for transmission of '53' Byte Information
 * 
 * This particular structure allows for up to 2^5 Nodes each with 2^3 Input or 
 * Output Channels.
 */
union byte_53u8_status {
	struct {
		uint16_t   byte:8;
		uint16_t   channel:3;
		uint16_t   io_node:5;
	} bitfield;
	uint16_t bytes[2];
};







#define ANALOG_42_INPUT_STATUS            0x150
#define ANALOG_42_INPUT_STATUS_REQ        0x151

#define ANALOG_42_OUTPUT_STATUS           0x156
#define ANALOG_42_OUTPUT_STATUS_REQ       0x157
#define ANALOG_42_OUTPUT_STATUS_RESP      0x158
#define ANALOG_42_OUTPUT_ERROR            0x159

union analog_42_status {
	struct {
		uint16_t   value:10;
		uint16_t   channel:2;
		uint16_t   io_node:4;
	} bitfield;
	uint16_t bytes[2];
};

