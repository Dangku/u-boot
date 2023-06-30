/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __BOARD_CFG_H__
#define __BOARD_CFG_H__

#include <asm/arch/cpu.h>

/*
 * platform power init config
 */

#define AML_VCCK_A_INIT_VOLTAGE      1009       // VCCK A power up voltage
#define AML_VCCK_B_INIT_VOLTAGE      1009       // VCCK B power up voltage
#define AML_VDDEE_INIT_VOLTAGE       830       // VDDEE power up voltage
#define AML_VDDGPU_INIT_VOLTAGE      830       // VDDGPU power up voltage
#define AML_VDDNPU_INIT_VOLTAGE      830       // VDDNPU power up voltage
#define AML_VDDDDR_INIT_VOLTAGE      830       // VDDDDR power up voltage
/*Distinguish whether to use efuse to adjust vddee*/
#define CONFIG_PDVFS_ENABLE

/* SMP Definitions */
#define CPU_RELEASE_ADDR        secondary_boot_func

/* Bootloader Control Block function
   That is used for recovery and the bootloader to talk to each other
  */
#if 0
#define CONFIG_BOOTLOADER_CONTROL_BLOCK
#endif

/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200

/*low console baudrate*/
#define CONFIG_LOW_CONSOLE_BAUD            0

/* Enable ir remote wake up for bl30 */
#define AML_IR_REMOTE_POWER_UP_KEY_VAL1 0xef10fe01 //amlogic tv ir --- power
#define AML_IR_REMOTE_POWER_UP_KEY_VAL2 0XBB44FB04 //amlogic tv ir --- ch+
#define AML_IR_REMOTE_POWER_UP_KEY_VAL3 0xF20DFE01 //amlogic tv ir --- ch-
#define AML_IR_REMOTE_POWER_UP_KEY_VAL4 0XBA45BD02 //amlogic small ir--- power
#define AML_IR_REMOTE_POWER_UP_KEY_VAL5 0xe51afb04
#define AML_IR_REMOTE_POWER_UP_KEY_VAL6 0xFFFFFFFF
#define AML_IR_REMOTE_POWER_UP_KEY_VAL7 0xFFFFFFFF
#define AML_IR_REMOTE_POWER_UP_KEY_VAL8 0xFFFFFFFF
#define AML_IR_REMOTE_POWER_UP_KEY_VAL9 0xFFFFFFFF

/*config the default parameters for adc power key*/
#define AML_ADC_POWER_KEY_CHAN   2  /*channel range: 0-7*/
#define AML_ADC_POWER_KEY_VAL    0  /*sample value range: 0-1023*/

//#define CONFIG_BOOTLOADER_CONTROL_BLOCK

#ifdef CONFIG_DTB_LOAD
#undef CONFIG_DTB_LOAD
#endif

/* load dtb from boot for uboot */
#define CONFIG_DTB_LOAD  \
        "if test ${boot_source} = emmc; then "\
            "echo Load dtb/${fdtfile} from eMMC (1:1) ...;" \
            "load mmc 1:1 ${dtb_mem_addr} dtb/${fdtfile};" \
        "else if test ${boot_source} = sd; then "\
            "echo Load dtb/${fdtfile} from SD (0:1) ...;" \
            "load mmc 0:1 ${dtb_mem_addr} dtb/${fdtfile};" \
        "fi;fi;"

/* args/envs */
#define CONFIG_SYS_MAXARGS  64

#ifdef CONFIG_CMD_USB
#define BOOT_TARGET_DEVICES_USB(func) func(USB, usb, 0)
#else
#define BOOT_TARGET_DEVICES_USB(func)
#endif

#ifndef BOOT_TARGET_DEVICES
#define BOOT_TARGET_DEVICES(func) \
        func(MMC, mmc, 0) \
        func(MMC, mmc, 1) \
        BOOT_TARGET_DEVICES_USB(func) \
        func(PXE, pxe, na) \
        func(DHCP, dhcp, na)
#endif

#include <config_distro_bootcmd.h>

#define CONFIG_EXTRA_BOOT_ENV_SETTINGS \
        "fdt_addr_r=0x01000000\0"\
        "fdtoverlay_addr_r=0x00a00000\0"\
        "kernel_addr_r=0x01080000\0"\
        "pxefile_addr_r=0x00010000\0"\
        "scriptaddr=0x00010000\0" \
        "ramdisk_addr_r=0x10000000\0"\
        "fdtfile=amlogic/" CONFIG_DEFAULT_DEVICE_TREE ".dtb\0" \
		BOOTENV

/*"hdmitx hpd;hdmitx get_preferred_mode;hdmitx get_parse_edid;hdmitx edid;dovi process;" \*/
#define CONFIG_EXTRA_HDMI_ENV_SETTINGS \
	"mipi_lcd_exist=0\0" \
	"panel_type=vbyone_0\0" \
	"panel1_type=mipi_0\0" \
	"panel2_type=lvds_1\0" \
	"lcd1_ctrl=0x00000000\0" \
	"lcd2_ctrl=0x00000000\0" \
	"outputmode=panel1\0" \
	"outputmode2=1080p60hz\0" \
	"cvbsmode=576cvbs\0" \
	"storeargs_hdmitx="\
		"if test ${mipi_lcd_exist} != 1; then "\
			"setenv vout2_args ;"\
		"else "\
			"setenv vout2_args vout2=${outputmode2},enable;"\
		"fi;"\
		"setenv bootargs ${bootargs} "\
		"lcd_ctrl=${lcd_ctrl} lcd_debug=${lcd_debug} "\
		"outputmode=${outputmode} hdmitx=${cecconfig},${colorattribute} "\
		"${vout2_args} panel1_type=${panel1_type} "\
		"lcd1_ctrl=${lcd1_ctrl} panel2_type=${panel2_type} lcd2_ctrl=${lcd2_ctrl} "\
		"hdr_policy=${hdr_policy} hdr_priority=${hdr_priority};"\
		"\0"\
	"init_display_hdmitx="\
	    "hdmitx edid;dovi process;" \
		"setenv outputmode2 ${hdmimode};" \
		"osd dual_logo;" \
		"dovi set;dovi pkg;vpp hdrpkt;" \
		"\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
        CONFIG_EXTRA_BOOT_ENV_SETTINGS \
        "silent=1\0" \
        "loadaddr=0x00020000\0" \
        "loadaddr_kernel=0x01080000\0" \
		"dv_fw_addr=0xa00000\0" \
        "otg_device=1\0" \
        "lcd_ctrl=0x00000000\0" \
        "lcd_debug=0x00000000\0" \
        "hdmimode=1080p60hz\0" \
        "cvbsmode=dummy_l\0" \
		"colorattribute=444,8bit\0"\
		"vout_init=enable\0" \
        "display_width=1920\0" \
        "display_height=1080\0" \
		"hdmichecksum=0x00000000\0" \
        "dolby_status=0\0" \
        "dolby_vision_on=0\0" \
        "dv_fw_dir_odm_ext=/odm_ext/firmware/dovi_fw.bin\0" \
        "dv_fw_dir_vendor=/vendor/firmware/dovi_fw.bin\0" \
        "dv_fw_dir=/oem/firmware/dovi_fw.bin\0" \
		"dv_fw_dir=/reserved/firmware/dovi_fw.bin\0" \
        "display_bpp=24\0" \
        "display_color_index=24\0" \
        "display_layer=osd0\0" \
        "display_color_fg=0xffff\0" \
        "display_color_bg=0\0" \
        "dtb_mem_addr=0x01000000\0" \
		"common_dtb_load=" CONFIG_DTB_LOAD "\0"\
        "fb_addr=0x00300000\0" \
        "fb_width=1920\0" \
        "fb_height=1080\0" \
        "frac_rate_policy=1\0" \
        "hdr_policy=0\0" \
        "fdtaddr=${fdt_addr_r}\0"\
        "fdt_high=0x20000000\0" \
        "board=bananapi_cm5\0" \
        "cvbs_drv=0\0" \
        "osd_reverse=0\0" \
        "video_reverse=0\0" \
        "edid_14_dir=/odm/etc/tvconfig/hdmi/port1_14.bin\0" \
        "edid_20_dir=/odm/etc/tvconfig/hdmi/port1_20.bin\0" \
        "edid_select=0\0" \
        "port_map=0x4321\0" \
        "cec_fun=0x2F\0" \
        "logic_addr=0x0\0" \
        "cec_ac_wakeup=1\0" \
		"disable_ir=0\0" \
	CONFIG_EXTRA_HDMI_ENV_SETTINGS \
        "initargs="\
            "rootfstype=ext4 rw loglevel=8 console=tty1 console=ttyS0,921600 no_console_suspend earlycon=aml-uart,0xfe078000 fsck.mode=force fsck.repair=yes net.ifnames=0 "\
            "board=${board} boot_source=${boot_source} scsi_mod.scan=async xhci_hcd.quirks=0x800000 "\
            "\0"\
        "storeargs_base=" \
		    "get_bootloaderversion;" \
		    "setenv bootargs ${initargs} otg_device=${otg_device} "\
		    "logo=${display_layer},loaded,${fb_addr} "\
		    "vout=${outputmode},${vout_init} panel_type=${panel_type} "\
		    "hdmitx=${cecconfig},${colorattribute} hdmimode=${hdmimode} "\
		    "hdmichecksum=${hdmichecksum} dolby_vision_on=${dolby_vision_on} "\
		    "hdr_policy=${hdr_policy} hdr_priority=${hdr_priority} "\
		    "frac_rate_policy=${frac_rate_policy} hdmi_read_edid=${hdmi_read_edid} "\
		    "cvbsmode=${cvbsmode} "\
		    "osd_reverse=${osd_reverse} video_reverse=${video_reverse} "\
		    "disable_ir=${disable_ir}; "\
		    "\0"\
		"storeargs=" \
			"run storeargs_base;" \
			"run storeargs_hdmitx;" \
            "\0"\
        "switch_bootmode="\
            "get_rebootmode;"\
            "echo reboot_mode : ${reboot_mode};"\
            "\0" \
		"load_bmp_logo="\
            "if test ${boot_source} = emmc; then "\
                "load mmc 1:1 ${loadaddr} /boot-logo.bmp;" \
            "else if test ${boot_source} = sd; then "\
                "load mmc 0:1 ${loadaddr} /boot-logo.bmp;" \
            "fi;fi;" \
            "bmp display ${loadaddr};" \
            "bmp scale;" \
			"\0"\
		"init_display="\
			"run init_display_hdmitx;"\
			"\0"\

#define CONFIG_PREBOOT  \
            "run init_display;"\
            "run storeargs;"\
            "run switch_bootmode;"

#ifndef CONFIG_HDMITX_ONLY
/* dual logo, normal boot */
#define CONFIG_DUAL_LOGO \
    "setenv outputmode2 ${hdmimode};"\
    "setenv display_layer viu2_osd0;vout2 prepare ${outputmode2};"\
    "osd open;osd clear;run load_bmp_logo;vout2 output ${outputmode2};bmp scale;"\
    "setenv display_layer osd0;osd open;osd clear;"\
	"run load_bmp_logo;bmp scale;vout output ${outputmode};"\
    "\0"\

/* dual logo, factory_reset boot, recovery always displays on panel */
#define CONFIG_RECOVERY_DUAL_LOGO \
    "setenv outputmode2 ${hdmimode};"\
    "setenv display_layer viu2_osd0;vout2 prepare ${outputmode2};"\
    "osd open;osd clear;run load_bmp_logo;vout2 output ${outputmode2};bmp scale;"\
    "setenv display_layer osd0;osd open;osd clear;"\
	"run load_bmp_logo;bmp scale;vout output ${outputmode};"\
    "\0"\

/* single logo */
#define CONFIG_SINGLE_LOGO \
    "setenv display_layer osd0;osd open;osd clear;"\
	"run load_bmp_logo;bmp scale;vout output ${outputmode};"\
    "\0"
#endif

/* #define CONFIG_ENV_IS_NOWHERE  1 */
#define CONFIG_ENV_SIZE   (64*1024)
#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* ATTENTION */
/* DDR configs move to board/amlogic/[board]/firmware/timing.c */

/* running in sram */
//#define UBOOT_RUN_IN_SRAM
#ifdef UBOOT_RUN_IN_SRAM
#define CONFIG_SYS_INIT_SP_ADDR                (0x00200000)
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN                (256*1024)
#else
#define CONFIG_SYS_INIT_SP_ADDR                (0x00200000)
#define CONFIG_SYS_MALLOC_LEN                (96*1024*1024)
#endif

//#define CONFIG_NR_DRAM_BANKS            1
/* ddr functions */
#define DDR_FULL_TEST            0 //0:disable, 1:enable. ddr full test
#define DDR_LOW_POWER            0 //0:disable, 1:enable. ddr clk gate for lp
#define DDR_ZQ_PD                0 //0:disable, 1:enable. ddr zq power down
#define DDR_USE_EXT_VREF         0 //0:disable, 1:enable. ddr use external vref
#define DDR4_TIMING_TEST         0 //0:disable, 1:enable. ddr4 timing test function
#define DDR_PLL_BYPASS           0 //0:disable, 1:enable. ddr pll bypass function

/* storage: emmc/nand/sd */
#define CONFIG_ENV_OVERWRITE
/* #define     CONFIG_CMD_SAVEENV */
/* fixme, need fix*/

#if (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC)) && defined(CONFIG_STORE_COMPATIBLE)
#error env in amlnand/mmc already be compatible;
#endif

/*
*                storage
*        |---------|---------|
*        |                    |
*        emmc<--Compatible-->nand
*                    |-------|-------|
*                    |        |
*                    MTD<-Exclusive->NFTL
*                    |
*            |***************|***************|
*            slc-nand    SPI-nand    SPI-nor
*            (raw nand)
*/
/* axg only support slc nand */
/* swither for mtd nand which is for slc only. */


#if defined(CONFIG_AML_NAND) && defined(CONFIG_MESON_NFC)
#error CONFIG_AML_NAND/CONFIG_MESON_NFC can not support at the sametime;
#endif

#if (defined(CONFIG_AML_NAND) || defined(CONFIG_MESON_NFC)) && defined(CONFIG_MESON_FBOOT)
#error CONFIG_AML_NAND/CONFIG_MESON_NFC CONFIG _MESON_FBOOT can not support at the sametime;
#endif

#if defined(CONFIG_SPI_NAND) && defined(CONFIG_MTD_SPI_NAND) && defined(CONFIG_MESON_NFC)
#error CONFIG_SPI_NAND/CONFIG_MTD_SPI_NAND/CONFIG_MESON_NFC can not support at the sametime;
#endif

/* #define        CONFIG_AML_SD_EMMC 1 */
#ifdef        CONFIG_AML_SD_EMMC
    #define     CONFIG_GENERIC_MMC 1
    #define     CONFIG_CMD_MMC 1
    #define CONFIG_CMD_GPT 1
    #define    CONFIG_SYS_MMC_ENV_DEV 1
    #define CONFIG_EMMC_DDR52_EN 0
    #define CONFIG_EMMC_DDR52_CLK 35000000
#endif
#define        CONFIG_PARTITIONS 1
#if 0
#define     CONFIG_SYS_NO_FLASH  1
#endif

#if defined CONFIG_MESON_NFC || defined CONFIG_SPI_NAND || defined CONFIG_MTD_SPI_NAND
    #define CONFIG_SYS_MAX_NAND_DEVICE  2
#endif

/* vpu */
#define AML_VPU_CLK_LEVEL_DFT 7
/* LCD */

/* osd */
#define OSD_SCALE_ENABLE
#define AML_OSD_HIGH_VERSION
#define AML_T7_DISPLAY
#define VEHICLE_CONFIG

/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC        1 */
/* #define CONFIG_CMD_USB 1 */

#define USB_PHY2_PLL_PARAMETER_1    0x09400414
#define USB_PHY2_PLL_PARAMETER_2    0x927e0000
#define USB_PHY2_PLL_PARAMETER_3    0xAC5F49E5

#define USB_G12x_PHY_PLL_SETTING_1    (0xfe18)
#define USB_G12x_PHY_PLL_SETTING_2    (0xfff)
#define USB_G12x_PHY_PLL_SETTING_3    (0x78000)
#define USB_G12x_PHY_PLL_SETTING_4    (0xe0004)
#define USB_G12x_PHY_PLL_SETTING_5    (0xe000c)

#define AML_TXLX_USB        1
#define AML_USB_V2             1
#define USB_GENERAL_BIT         3
#define USB_PHY21_BIT           4

/* net */
#define CONFIG_CMD_NET   1
#define CONFIG_ETH_DESIGNWARE
#if defined(CONFIG_CMD_NET)
    #define CONFIG_DESIGNWARE_ETH 1
    #define CONFIG_PHYLIB    1
    #define CONFIG_NET_MULTI 1
    #define CONFIG_CMD_PING 1
    #define CONFIG_CMD_DHCP 1
    #define CONFIG_CMD_RARP 1
    #define CONFIG_HOSTNAME        "arm_gxbb"
#if 0
    #define CONFIG_RANDOM_ETHADDR  1                   /* use random eth addr, or default */
#endif
    #define CONFIG_ETHADDR         00:15:18:01:81:31   /* Ethernet address */
    #define CONFIG_IPADDR          10.18.9.97          /* Our ip address */
    #define CONFIG_GATEWAYIP       10.18.9.1           /* Our getway ip address */
    #define CONFIG_SERVERIP        10.18.9.113         /* Tftp server ip address */
    #define CONFIG_NETMASK         255.255.255.0
#endif /* (CONFIG_CMD_NET) */

#define MAC_ADDR_NEW  1

/* other devices */
#define CONFIG_SHA1 1
#define CONFIG_MD5 1

/* commands */
/* #define CONFIG_CMD_PLLTEST 1*/

/*file system*/
#define CONFIG_DOS_PARTITION 1
#define CONFIG_EFI_PARTITION 1
/* #define CONFIG_MMC 1 */
#define CONFIG_FS_FAT 1
#define CONFIG_FS_EXT4 1
#define CONFIG_LZO 1

#define CONFIG_FAT_WRITE 1
#define CONFIG_AML_FACTORY_PROVISION 1

/* Cache Definitions */
/* #define CONFIG_SYS_DCACHE_OFF */
/* #define CONFIG_SYS_ICACHE_OFF */

/* other functions */
#define CONFIG_LIBAVB        1

/* define CONFIG_SYS_MEM_TOP_HIDE 8M space for free buffer */
#define CONFIG_SYS_MEM_TOP_HIDE        0x00800000

#define CONFIG_CPU_ARMV8

//use sha2 command
#define CONFIG_CMD_SHA2

//use startdsp command
#define CONFIG_CMD_STARTDSP

//use cache command
#define CONFIG_CMD_CACHE

//use remapset command
#define CONFIG_CMD_REMAPSET

//use hardware sha2
//#define CONFIG_AML_HW_SHA2

//Replace avb2 software SHA256 to utilize armce
#define CONFIG_AVB2_UBOOT_SHA256

// use auto select DTB table
#ifdef CONFIG_MULTI_DTB
    #define CONFIG_T7_3G_SIZE   0xC0000000
    #define CONFIG_T7_4G_SIZE   0x100000000
    #define CONFIG_T7_6G_SIZE   0x180000000
    #define CONFIG_T7_8G_SIZE   0x200000000
#endif

#define CONFIG_RX_RTERM        1
#define CONFIG_CMD_HDMIRX   1
#define CONFIG_CMD_CEC        1
/* define CONFIG_UPDATE_MMU_TABLE for need update mmu */
#define    CONFIG_UPDATE_MMU_TABLE

/* support secure boot */
#define CONFIG_AML_SECURE_UBOOT   1

#if defined(CONFIG_AML_SECURE_UBOOT)

/* unify build for generate encrypted bootloader "u-boot.bin.encrypt" */
#define CONFIG_AML_CRYPTO_UBOOT   1
//#define CONFIG_AML_SIGNED_UBOOT   1
/* unify build for generate encrypted kernel image
   SRC : "board/amlogic/(board)/boot.img"
   DST : "fip/boot.img.encrypt" */
/* #define CONFIG_AML_CRYPTO_IMG       1 */

#endif /* CONFIG_AML_SECURE_UBOOT */

#define CONFIG_FIP_IMG_SUPPORT  1

#define BL32_SHARE_MEM_SIZE  0x800000
//#define CONFIG_AML_KASLR_SEED

#endif

/* increase for bootargs */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE 4096

