#ifndef __BANANAPI_COMMON_H
#define __BANANAPI_COMMON_H

/*
 * Discover the boot device within MicroSD or eMMC
 * and return 1 for eMMC, otherwise 0.
 */
#define BOOT_DEVICE_RESERVED    0
#define BOOT_DEVICE_EMMC        1
#define BOOT_DEVICE_NAND        2
#define BOOT_DEVICE_SPI         3
#define BOOT_DEVICE_SD          4
#define BOOT_DEVICE_USB         5

const char *boot_device_name(int n);
int get_boot_device(void);

extern int get_adc_value(int channel);

/*
 * Board revision in the form of YYYYMMDD as hexadecimal
 * ex) BOARD_REVISION(2018, 07, 16)  -> 0x20180716
 */

#define BOARD_REVISION(y,m,d)	(((0x##y & 0xffff) << 16) \
		| ((0x##m & 0xff) << 8) | ((0x##d & 0xff) << 0))

#define IS_RANGE(x, min, max)   ((x) >= (min) && (x) < (max))

#define BOARD_TYPE_CHANNEL		0
#define BOARD_REV_CHANNEL		1
#define CM4IO_BOARD_TYPE_CHANNEL	3

#define BOARD_S922X_M2S                 0
#define BOARD_A311D_M2S                 1
#define BOARD_A311D_CM4                 2

#define BOARD_CM4IO_BPI			0
#define BOARD_CM4IO_RPI			1

int board_revision(void);
void get_board_serial(void);

#if defined(CONFIG_BANANAPI_M2S)
int board_is_bananapi_s922x_m2s(void);
int board_is_bananapi_a311d_m2s(void);
#elif defined(CONFIG_BANANAPI_CM4)
int board_is_bananapi_cm4(void);
int board_is_rpi_cm4io(void);
int board_is_bpi_cm4io(void);
#endif
#endif