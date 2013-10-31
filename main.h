#ifndef MAIN_H
#define MAIN_H

//
// Bootloader info
//
const char boot_manufacturer[24] __attribute__ ((space(prog),address(0x200)));
const char boot_model[24] __attribute__ ((space(prog),address(0x218)));
const char boot_description[24]  __attribute__ ((space(prog),address(0x230)));
const char boot_version[10]  __attribute__ ((space(prog),address(0x248)));
const char boot_uri[50]  __attribute__ ((space(prog),address(0x252)));
const char boot_serial[14] __attribute__ ((space(prog),address(0x284)));


//
// Application Info  BOOTCODE_SIZE
//
const char app_manufacturer[24] __attribute__ ((space(prog),address(0x79F8))) = "electronicSoup";
const char app_description[24]  __attribute__ ((space(prog),address(0x7a10))) = "CANBus Node OS";
const char app_version[10]  __attribute__ ((space(prog),address(0x7a28))) = "1.0";
const char app_uri[50]  __attribute__ ((space(prog),address(0x7a32)))  = "http://www.electronicsoup.com/";

#endif
