
/*
 * include/configs/bananapi-g12-common.h
 *
 * (C) Copyright 2018 Bananapi Co., Ltd
 * * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __BANANAPI_G12B_COMMON_H__
#define __BANANAPI_G12B_COMMON_H__

#include <asm/arch/cpu.h>
#include <linux/sizes.h>

#define CONFIG_SYS_GENERIC_BOARD  1
#ifndef CONFIG_AML_MESON
#warning "include warning"
#endif

#ifndef CONFIG_DEVICE_PRODUCT
#error CONFIG_DEVICE_PRODUCT is missing!!
#endif

/*
 * platform power init config
 */
#define CONFIG_PLATFORM_POWER_INIT
#define CONFIG_VCCK_INIT_VOLTAGE	800		// VCCK power up voltage
#define CONFIG_VDDEE_INIT_VOLTAGE	880		// VDDEE power up voltage
#define CONFIG_VDDEE_SLEEP_VOLTAGE	770		// VDDEE suspend voltage

/* SMP Definitinos */
#define CPU_RELEASE_ADDR		secondary_boot_func

/* config saradc*/
#define CONFIG_CMD_SARADC 1
#define CONFIG_SARADC_CH  2

/* ADC keys */
#define CONFIG_ADC_KEY
#ifdef CONFIG_ADC_KEY
#define CONFIG_ADC_POWER_KEY_CHAN		2  /* channel range: 0-7*/
#define CONFIG_ADC_POWER_KEY_VAL		0  /* sample value range: 0-1023*/
#endif


/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200
#define CONFIG_AML_MESON_SERIAL   1
#define CONFIG_SERIAL_MULTI		1

/* Enable ir remote wake up for bl30 */
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL1  0xff00fe01
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL2  0xf10eaf40
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL3	0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL4	0XFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL5	0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL6	0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL7	0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL8	0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL9	0xFFFFFFFF

/* args/envs */
#define CONFIG_SYS_MAXARGS  64
#define CONFIG_EXTRA_ENV_SETTINGS \
        "jtag=disable\0"\
        "mmc_list=0 1\0"\
        "loadaddr=1080000\0"\
        "panel_type=lcd_0\0" \
		"lcd_exist=1\0" \
        "outputmode=panel\0" \
        "hdmimode=1080p60hz\0" \
	    "colorattribute=444,8bit\0"\
        "cvbsmode=576cvbs\0" \
        "display_width=1920\0" \
        "display_height=1080\0" \
        "display_bpp=24\0" \
        "display_color_index=24\0" \
        "display_layer=osd0\0" \
        "display_color_fg=0xffff\0" \
        "display_color_bg=0\0" \
        "dtb_mem_addr=0x1000000\0" \
        "fb_addr=0x3d800000\0" \
        "fb_width=1920\0" \
        "fb_height=1080\0" \
        "frac_rate_policy=1\0" \
        "sdr2hdr=2\0" \
        "hdmi_read_edid=1\0" \
        "otg_device=1\0"\
        "fdt_high=0x20000000\0"\
        "cvbs_drv=0\0"\
        "lock=10001000\0"\
        "osd_reverse=0\0"\
        "video_reverse=0\0"\
        "active_slot=normal\0"\
        "boot_part=boot\0"\
        "Irq_check_en=0\0"\
        "fs_type=""rootfstype=ext4""\0"\
        "initargs="\
            "root=LABEL=ROOTFS rootflags=data=writeback rw rootfstype=ext4 console=tty0 console=ttyS0,115200n8 no_console_suspend consoleblank=0 fsck.repair=yes net.ifnames=0 "\
            "\0"\
        "storeargs="\
            "setenv bootargs ${initargs} logo=${display_layer},loaded,${fb_addr} vout=${outputmode},enable hdmitx=${cecconfig},${colorattribute} hdmimode=${hdmimode} cvbsmode=${cvbsmode} osd_reverse=${osd_reverse} video_reverse=${video_reverse} jtag=${jtag}; "\
	        "setenv bootargs ${bootargs} androidboot.hardware=amlogic reboot_mode=${reboot_mode} ddr_size=${ddr_size};"\
            "\0"\
        "switch_bootmode="\
            "get_rebootmode;"\
            "if test ${reboot_mode} = factory_reset; then "\
                    "run storeargs;"\
                    "run storeboot;"\
            "else if test ${reboot_mode} = cold_boot; then "\
                    "run storeargs;"\
            "else if test ${reboot_mode} = fastboot; then "\
					"fastboot;"\
            "fi;fi;fi;"\
            "\0" \
        "storeboot="\
            "boot_cooling;"\
            "cfgload;" \
            "\0"\
        "init_display="\
            "get_rebootmode;"\
            "echo reboot_mode:::: ${reboot_mode};"\
            "setenv reboot_mode_android ""normal"";"\
            "run storeargs;"\
            "hdmitx get_preferred_mode;"\
            "hdmitx edid;"\
            "hdmitx hpd;"\
            "if test ${lcd_exist} = 1 && test ${outputmode} = panel; then "\
                "setenv fb_width 800;"\
                "setenv fb_height 1280;"\
            "fi;"\
            "if test ${lcd_exist} = 0 && test ${outputmode} = custombuilt; then "\
                "setenv fb_width ${customwidth};"\
                "setenv fb_height ${customheight};"\
            "fi;"\
            "osd open;"\
            "osd clear;"\
            "for n in ${mmc_list}; do "\
                "if load mmc ${n} ${loadaddr} /boot-logo.bmp; then "\
                    "bmp display ${loadaddr};"\
                    "bmp scale;"\
                "fi;"\
            "done;"\
            "vout output ${outputmode};"\
            "vpp hdrpkt;"\
            "\0"\

#define CONFIG_PREBOOT  \
            "run init_display;"\
            "run storeargs;"

#define CONFIG_BOOTCOMMAND "run storeboot"

#define CONFIG_BOOTAREA_SIZE			(1 * SZ_1M)
#define CONFIG_MBR_SIZE				512
#define CONFIG_ENV_SIZE				(64 * SZ_1K)
#define CONFIG_UBOOT_SIZE			(CONFIG_BOOTAREA_SIZE - \
		(CONFIG_MBR_SIZE + CONFIG_ENV_SIZE))
#define CONFIG_PTABLE_SIZE			(4 * SZ_1K)
#define CONFIG_ENV_OFFSET			(CONFIG_MBR_SIZE + CONFIG_UBOOT_SIZE)
#define CONFIG_PTABLE_OFFSET			(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)

#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_ANDROID_IMG 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* cpu */
#define CONFIG_CPU_CLK					1200 //MHz. Range: 360-2000, should be multiple of 24

#define CONFIG_NR_DRAM_BANKS			1

/* ddr functions */
#define CONFIG_DDR_FULL_TEST			0 //0:disable, 1:enable. ddr full test
#define CONFIG_CMD_DDR_D2PLL			0 //0:disable, 1:enable. d2pll cmd
#define CONFIG_CMD_DDR_TEST				0 //0:disable, 1:enable. ddrtest cmd
#define CONFIG_DDR_LOW_POWER			0 //0:disable, 1:enable. ddr clk gate for lp
#define CONFIG_DDR_ZQ_PD				0 //0:disable, 1:enable. ddr zq power down
#define CONFIG_DDR_USE_EXT_VREF			0 //0:disable, 1:enable. ddr use external vref
#define CONFIG_DDR4_TIMING_TEST			0 //0:disable, 1:enable. ddr4 timing test function
#define CONFIG_DDR_PLL_BYPASS			0 //0:disable, 1:enable. ddr pll bypass function

/* storage: emmc/nand/sd */
#define CONFIG_ENV_IS_IN_MMC			1
#define 	CONFIG_ENV_OVERWRITE
#define 	CONFIG_CMD_SAVEENV
/* fixme, need fix*/

#if (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC)) && defined(CONFIG_STORE_COMPATIBLE)
#error env in amlnand/mmc already be compatible;
#endif

#define		CONFIG_AML_SD_EMMC 1
#ifdef		CONFIG_AML_SD_EMMC
	#define 	CONFIG_GENERIC_MMC 1
	#define 	CONFIG_CMD_MMC 1
	#define CONFIG_CMD_GPT 1
	#define	CONFIG_SYS_MMC_ENV_DEV 1
	#define CONFIG_EMMC_DDR52_EN 0
	#define CONFIG_EMMC_DDR52_CLK 35000000
#endif
#define		CONFIG_PARTITIONS 1
#define 	CONFIG_SYS_NO_FLASH  1

/* meson SPI */
//#define CONFIG_AML_SPIFC
//#define CONFIG_AML_SPICC
#if defined CONFIG_AML_SPIFC || defined CONFIG_AML_SPICC
	#define CONFIG_OF_SPI
	#define CONFIG_DM_SPI
	#define CONFIG_CMD_SPI
#endif
/* SPI flash config */
#ifdef CONFIG_AML_SPIFC
	#define CONFIG_SPI_FLASH
	#define CONFIG_DM_SPI_FLASH
	#define CONFIG_CMD_SF
	/* SPI flash surpport list */
	#define CONFIG_SPI_FLASH_ATMEL
	#define CONFIG_SPI_FLASH_EON
	#define CONFIG_SPI_FLASH_GIGADEVICE
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_SPI_FLASH_SPANSION
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_SST
	#define CONFIG_SPI_FLASH_WINBOND
	#define CONFIG_SPI_FRAM_RAMTRON
	#define CONFIG_SPI_M95XXX
	#define CONFIG_SPI_FLASH_ESMT
	/* SPI nand flash support */
	#define CONFIG_BL2_SIZE (64 * 1024)
	#define CONFIG_ENV_IS_IN_SPI_FLASH		1
#endif

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SECT_SIZE			(64 * SZ_1K)
#if (CONFIG_ENV_SIZE < CONFIG_ENV_SECT_SIZE)
#error 'CONFIG_ENV_SIZE' must be larger or equal than 'CONFIG_ENV_SECT_SIZE'
#endif
#endif

#if defined(CONFIG_SPI_NAND)
	#define CONFIG_MTD_DEVICE y
	#define CONFIG_RBTREE
	#define CONFIG_CMD_MTDPARTS   1
	#define CONFIG_MTD_PARTITIONS 1
#endif

/* vpu */
#define CONFIG_AML_VPU 1
//#define CONFIG_VPU_CLK_LEVEL_DFT 7

/* DISPLAY & HDMITX */
#define CONFIG_AML_HDMITX20			1
#define CONFIG_AML_CANVAS			1
#define CONFIG_AML_VOUT				1
#define CONFIG_AML_OSD				1
#define CONFIG_OSD_SCALE_ENABLE			1
#define CONFIG_CMD_BMP				1

#if defined(CONFIG_AML_VOUT)
#define CONFIG_AML_CVBS 1
#endif

#define CONFIG_AML_LCD    1
#define CONFIG_AML_LCD_TABLET 1
//#define CONFIG_AML_LCD_EXTERN 1


/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC		1 */
#define CONFIG_CMD_USB 1
#if defined(CONFIG_CMD_USB)
	#define CONFIG_GXL_XHCI_BASE            0xff500000
	#define CONFIG_GXL_USB_PHY2_BASE        0xffe09000
	#define CONFIG_GXL_USB_PHY3_BASE        0xffe09080
	#define CONFIG_USB_PHY_20				0xff636000
	#define CONFIG_USB_PHY_21				0xff63A000
	#define CONFIG_USB_STORAGE      1
	#define CONFIG_USB_XHCI		1
	#define CONFIG_USB_XHCI_AMLOGIC_V2 1
	#define CONFIG_USB_GPIO_PWR  			GPIOEE(GPIOH_8)
	#define CONFIG_USB_GPIO_PWR_NAME		"GPIOH_8"
	//#define CONFIG_USB_XHCI_AMLOGIC_USB3_V2		1
#endif //#if defined(CONFIG_CMD_USB)

#define CONFIG_TXLX_USB        1
#define CONFIG_USB_DEVICE_V2    1
#define USB_PHY2_PLL_PARAMETER_1	0x09400414
#define USB_PHY2_PLL_PARAMETER_2	0x927e0000
#define USB_PHY2_PLL_PARAMETER_3	0xAC5F69E5
#define USB_G12x_PHY_PLL_SETTING_1	(0xfe18)
#define USB_G12x_PHY_PLL_SETTING_2	(0xfff)
#define USB_G12x_PHY_PLL_SETTING_3	(0x78000)
#define USB_G12x_PHY_PLL_SETTING_4	(0xe0004)
#define USB_G12x_PHY_PLL_SETTING_5	(0xe000c)

//UBOOT fastboot config
#define CONFIG_CMD_FASTBOOT 1
#define CONFIG_FASTBOOT_FLASH_MMC_DEV 0
#define CONFIG_FASTBOOT_FLASH 1
#define CONFIG_USB_GADGET 1
#define CONFIG_USBDOWNLOAD_GADGET 1
#define CONFIG_SYS_CACHELINE_SIZE 64
#define CONFIG_FASTBOOT_MAX_DOWN_SIZE	0x8000000

/* net */
#define CONFIG_CMD_NET   1
/*#define CONFIG_PHY_REALTEK 1*/
#if defined(CONFIG_CMD_NET)
	#define CONFIG_DESIGNWARE_ETH 1
	#define CONFIG_PHYLIB	1
	#define CONFIG_NET_MULTI 1
	#define CONFIG_CMD_PING 1
	#define CONFIG_CMD_DHCP 1
	#define CONFIG_CMD_RARP 1
	#define CONFIG_HOSTNAME        Bananapi
//	#define CONFIG_RANDOM_ETHADDR  1				   /* use random eth addr, or default */
	#define CONFIG_ETHADDR         00:15:18:01:81:31   /* Ethernet address */
	#define CONFIG_IPADDR          192.168.1.200          /* Our ip address */
	#define CONFIG_GATEWAYIP       192.168.1.1           /* Our getway ip address */
	#define CONFIG_SERVERIP        192.168.1.230         /* Tftp server ip address */
	#define CONFIG_NETMASK         255.255.255.0
#endif /* (CONFIG_CMD_NET) */

/* other devices */
/* I2C DM driver*/
//#define CONFIG_DM_I2C
#if defined(CONFIG_DM_I2C)
#define CONFIG_SYS_I2C_MESON		0
#else
#define CONFIG_SYS_I2C_AML			1
#define CONFIG_SYS_I2C_SPEED		400000
#endif

/* PWM DM driver*/
//#define CONFIG_DM_PWM
//#define CONFIG_PWM_MESON

//#define CONFIG_EFUSE 1
#if defined(CONFIG_EFUSE)
#define CONFIG_EFUSE_MAC			1
#define CONFIG_EFUSE_SN				1
#if defined(CONFIG_EFUSE_MAC)
#define CONFIG_EFUSE_MAC_POS			0
#define CONFIG_EFUSE_MAC_LEN			12
#endif
#if defined(CONFIG_EFUSE_SN)
#define CONFIG_EFUSE_SN_POS			12
#define CONFIG_EFUSE_SN_LEN			16
#endif
#endif

/* commands */
#define CONFIG_CMD_CACHE 1
#define CONFIG_CMD_BOOTI 1
#define CONFIG_CMD_BOOTM			1
#if defined(CONFIG_EFUSE)
#define CONFIG_CMD_EFUSE 1
#endif
#define CONFIG_CMD_I2C 1
#define CONFIG_CMD_MEMORY 1
#define CONFIG_CMD_FAT 1
#define CONFIG_CMD_EXT2 1
#define CONFIG_CMD_EXT4 1
#define CONFIG_CMD_GPIO 1
#define CONFIG_CMD_RUN  1
#define CONFIG_CMD_REBOOT 1
#define CONFIG_CMD_ECHO 1
#define CONFIG_CMD_JTAG	1
#define CONFIG_CMD_AUTOSCRIPT 1
#define CONFIG_CMD_MISC 1
#define CONFIG_CMD_BDI 1
#define CONFIG_CMD_FS_GENERIC 1
#define CONFIG_CMD_CHIPID 1
#define CONFIG_CMD_PART 1

/*file system*/
#define CONFIG_DOS_PARTITION 1
#define CONFIG_EFI_PARTITION 1
#define CONFIG_MPT_PARTITION 1
#define CONFIG_MMC 1
#define CONFIG_FS_FAT 1
#define CONFIG_FS_EXT4 1
#define CONFIG_LZO 1
#define CONFIG_CMD_UNZIP    1
#define CONFIG_LZMA         1
#define CONFIG_FAT_WRITE	1
#define CONFIG_CMD_EXT4_WRITE	1

/* Cache Definitions */
//#define CONFIG_SYS_DCACHE_OFF
//#define CONFIG_SYS_ICACHE_OFF

/* other functions */
#define CONFIG_NEED_BL301	1
#define CONFIG_NEED_BL32	1
#define CONFIG_CMD_RSVMEM	1
#define CONFIG_FIP_IMG_SUPPORT	1
#define CONFIG_BOOTDELAY	3 //delay 1s
#define CONFIG_SYS_LONGHELP 1
#define CONFIG_CMD_MISC     1
#define CONFIG_CMD_ITEST    1
#define CONFIG_CMD_CPU_TEMP 1
#define CONFIG_SYS_MEM_TOP_HIDE 0x08000000 //hide 128MB for kernel reserve
#define CONFIG_CMD_LOADB    1

#define CONFIG_CMDLINE_EDITING 1
#define CONFIG_AUTO_COMPLETE 1

/* debug mode defines */
//#define CONFIG_DEBUG_MODE           1
#ifdef CONFIG_DEBUG_MODE
#define CONFIG_DDR_CLK_DEBUG        636
#define CONFIG_CPU_CLK_DEBUG        600
#endif

//support secure boot
#define CONFIG_AML_SECURE_UBOOT   1

#ifdef CONFIG_AML_SECURE_UBOOT

//unify build for generate encrypted bootloader "u-boot.bin.encrypt"
#define CONFIG_AML_CRYPTO_UBOOT   1

//unify build for generate encrypted kernel image
//SRC : "board/amlogic/(board)/boot.img"
//DST : "fip/boot.img.encrypt"
//#define CONFIG_AML_CRYPTO_IMG       1

#endif //CONFIG_AML_SECURE_UBOOT

//build with uboot auto test
//#define CONFIG_AML_UBOOT_AUTO_TEST 1

//board customer ID
//#define CONFIG_CUSTOMER_ID  (0x6472616F624C4D41)

#if defined(CONFIG_CUSTOMER_ID)
  #undef CONFIG_AML_CUSTOMER_ID
  #define CONFIG_AML_CUSTOMER_ID  CONFIG_CUSTOMER_ID
#endif

/* Choose One of Ethernet Type */
#undef CONFIG_ETHERNET_NONE
#undef ETHERNET_INTERNAL_PHY
#define ETHERNET_EXTERNAL_PHY

/* u-boot memory test */
#define CONFIG_CMD_MEMTEST
#ifdef CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START		(128 << 20)	/* 128MB */
#define CONFIG_SYS_MEMTEST_END			(2048 << 20)	/* 2GB */
#endif

#define CONFIG_HIGH_TEMP_COOL 90
#endif

