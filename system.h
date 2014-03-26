#ifndef SYSTEM_H
#define SYSTEM_H

#define L2_CAN_INTERRUPT_DRIVEN
//#define CAN_LAYER_3

// EEPROM Address MAP
#define NODE_ADDRESS   0x00
#define APP_CODE_VALID 0x02
#define NETWORK_BAUD_RATE 0x03

#define TEST

#if defined(CAN_LAYER_3)
result_t get_l3_node_address(u8 *address);
result_t get_new_l3_node_address(u8 *address);
#endif

#endif //SYSTEM_H
