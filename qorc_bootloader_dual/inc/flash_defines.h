/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

/*==========================================================
*                                                          
*    File   : flash_defines.h
*    Purpose: Contains definitions for QuickFeather on board Flash
*                                                          
*=========================================================*/

#ifndef __FLASH_DEFINES_H
#define __FLASH_DEFINES_H

/* 
*  QuickFeather Flash is 2MB. First 1MB is fixed
*  for use with Bootloader, FPGA, FFE, APP images
*
*  0x0000_0000	0x0000_FFFF	QuickFeather Boot Loader (raw flash)
*  0x0001_0000	0x0001_0007	{CRC,ByteCount} USB to serial flash FPGA image metadata
*  0x0001_1000	0x0001_1007	{CRC,ByteCount} Application FPGA image metadata
*  0x0001_2000	0x0001_2007	{CRC,ByteCount} Application FFE image metadata
*  0x0001_3000	0x0001_3007	{CRC,ByteCount} Application M4 image metadata
*  0x0001_4000	0x0001_FFFF	Unused
*  0x0002_0000	0x0003_FFFF	USB to serial flash FPGA image
*  0x0004_0000	0x0005_FFFF	Application FPGA image 
*  0x0006_0000	0x0007_FFFF	Application FFE image 
*  0x0008_0000	0x000F_FFFF	Application M4 image 
*  0x0010_0000	0x001F_FFFF	Unused
*/
//These are the start addresses for different images
#define FLASH_BOOTLOADER_ADDRESS            0x000000 //64K
#define FLASH_USBFPGA_META_ADDRESS          0x010000 //4K segment
#define FLASH_APPFPGA_META_ADDRESS          0x011000 //4K segment
#define FLASH_APPFFE_META_ADDRESS           0x012000 //4K segment
#define FLASH_APP_META_ADDRESS              0x013000 //4K segment
#define FLASH_UNUSED_1_ADDRESS              0x014000 //44K reserved for future
#define FLASH_BOOTLOADER_META_ADDRESS       0x01F000 //4k segment
#define FLASH_USBFPGA_ADDRESS               0x020000 //128K 
#define FLASH_APPFPGA_ADDRESS               0x040000 //128K 
#define FLASH_APPFFE_ADDRESS                0x060000 //128K 
#define FLASH_APP_ADDRESS                   0x080000 //440K 
#define FLASH_UNUSED_2_ADDRESS              0x0EE000 //72k 
#define FLASH_UNUSED_3_ADDRESS              0x100000 //1024K

//These are the maximum sizes for different images
#define FLASH_BOOTLOADER_SIZE               0x010000 //64K
#define FLASH_USBFPGA_META_SIZE             0x001000 //4K segment
#define FLASH_APPFPGA_META_SIZE             0x001000 //4K segment
#define FLASH_APPFFE_META_SIZE              0x001000 //4K segment
#define FLASH_APP_META_SIZE                 0x001000 //4K segment
#define FLASH_UNUSED_1_SIZE                 0x00B000 //44k reserved for future
#define FLASH_BOOTLOADER_META_SIZE          0x001000 //4k segment
#define FLASH_USBFPGA_SIZE                  0x020000 //128K 
#define FLASH_APPFPGA_SIZE                  0x020000 //128K 
#define FLASH_APPFFE_SIZE                   0x020000 //128K 
#define FLASH_APP_SIZE                      0x06E000 //440K - last 64K + 8K is used by the bootloader for copy
#define FLASH_UNUSED_2_SIZE                 0x012000 //72K 
#define FLASH_UNUSED_3_SIZE                 0x100000 //1024K


#endif //__FLASH_DEFINES_H
