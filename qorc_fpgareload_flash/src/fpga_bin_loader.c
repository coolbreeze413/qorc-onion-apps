// This set of functionality should be moved to
// fpga load library so any app can reuse it easily
// Currently it is in beta, hence only here.

#include "Fw_global_config.h"
#include "eoss3_dev.h"
#include "eoss3_hal_spi.h"
#include "spi_flash.h"
#include "crc32.h"
#include "bootloader_defines.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "fpga_loader.h"
#include "eoss3_hal_pads.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_fpga_usbserial.h"
#include "dbg_uart.h"

// structure of the fpga binary in flash memory:
// at specified binary address (FPGA_BIN_FLASH_ADDR), we have metadata of 4kB
// where currently, we use:
// image_size   : 4B
// image_crc    : 4B
// rest is unused as of now.
// This is followed by the actual fpga binary (produced from Symbiflow)
// of image_size bytes

uint32_t image_metadata[FLASH_APP_META_SIZE / 4];
uint8_t image_buffer[100 * 1024];


// Note that this function is a "naive" version where the entire fpga image is
// loaded in one go into SRAM, and then used to configure the FPGA.
// Further improvement TBD is to use only a 4kB buffer in SRAM to load, which
// would be enough
// as it stands today, we need to allocate and keep 100kB of SRAM dedicated
// for loading FPGA image from flash!

int load_appfpga(uint32_t fpga_binary_flash_addr)
{
    unsigned char *image_buffer_ptr;

    // fpga binary metadata
    uint32_t image_crc32, image_size;

    // fpga binary data
    uint32_t *fpga_bin_header;
    uint32_t fpga_bin_header_size;
    uint32_t fpga_bin_version;
    uint32_t *fpga_bitstream_ptr;
    uint32_t fpga_bitstream_size;
    uint32_t fpga_bitstream_crc;
    uint32_t *fpga_meminit_ptr;
    uint32_t fpga_meminit_size;
    uint32_t fpga_meminit_crc;
    uint32_t *fpga_iomux_ptr;
    uint32_t fpga_iomux_size;
    uint32_t fpga_iomux_crc;

    // crc calculation
    uint32_t image_crc32_calc = 0xFFFFFFFF;

    // read fpga binary metadata from flash
    image_buffer_ptr = (unsigned char *)image_metadata;
    
    read_flash((unsigned char *)fpga_binary_flash_addr, 
                FLASH_APPFPGA_META_SIZE, 
                image_buffer_ptr);
    
    image_crc32 = image_metadata[0];
    image_size = image_metadata[1];
    
    if (image_size > FLASH_APPFPGA_SIZE)
    {
        dbg_str("App FPGA Image size exceeded expected maximum! \n");
        return 1;
    }

    // read fpga binary data from flash (right after the metadata in flash)
    image_buffer_ptr = (unsigned char *)image_buffer;
    read_flash((unsigned char *)(fpga_binary_flash_addr + FLASH_APPFPGA_META_SIZE)), 
                image_size, 
                image_buffer_ptr);

    //check crc of fpga binary data
    image_crc32_calc = xcrc32((const unsigned char *)image_buffer_ptr, 
                            image_size, 
                            image_crc32_calc);

    if (image_crc32_calc != image_crc32)
    {
        dbg_str("AppFPGA image CRC mismatch! \n");
        return 1;
    }

    // disable FPGA domain clocks
    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);

    // enable FPGA configuration clocks
    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);

    // The FPGA bin now contains HEADER + BITSTREAM + MEMINIT + IOMUX
    // HEADER = 8 x 4B
    fpga_bin_header = (uint32_t *)image_buffer_ptr;
    fpga_bin_header_size = 8 * 4; // 32B

    fpga_bin_version = *(fpga_bin_header);

    fpga_bitstream_size = *(fpga_bin_header + 1);
    fpga_bitstream_crc = *(fpga_bin_header + 2);

    fpga_meminit_size = *(fpga_bin_header + 3);
    fpga_meminit_crc = *(fpga_bin_header + 4);

    fpga_iomux_size = *(fpga_bin_header + 5);
    fpga_iomux_crc = *(fpga_bin_header + 6);

    // word 7 of the fpga bin header is reserved for future use.

    fpga_bitstream_ptr = fpga_bin_header + 8;
    fpga_meminit_ptr = fpga_bitstream_ptr + (fpga_bitstream_size / 4);
    fpga_iomux_ptr = fpga_meminit_ptr + (fpga_meminit_size / 4);

    //   dbg_str("\r\n");
    //   dbg_hex32(fpga_bin_version);dbg_str("\r\n");
    //   dbg_int(fpga_bitstream_size);dbg_str("\r\n");
    //   dbg_hex32(fpga_bitstream_crc);dbg_str("\r\n");
    //   dbg_int(fpga_meminit_size);dbg_str("\r\n");
    //   dbg_hex32(fpga_meminit_crc);dbg_str("\r\n");
    //   dbg_int(fpga_iomux_size);dbg_str("\r\n");
    //   dbg_hex32(fpga_iomux_crc);dbg_str("\r\n");dbg_str("\r\n");

    load_fpga_with_mem_init(fpga_bitstream_size, fpga_bitstream_ptr, fpga_meminit_size, fpga_meminit_ptr);
    fpga_iomux_init(fpga_iomux_size, fpga_iomux_ptr);

    // enable FPGA domain clocks
    S3x_Clk_Enable(S3X_FB_21_CLK);
    S3x_Clk_Enable(S3X_FB_16_CLK);

    dbg_str("AppFPGA Loaded\r\n");

    return 0;
}
