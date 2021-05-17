/*
 * include/configs/bananapi_firewalla.h
 *
 * (C) Copyright 2018 Hardkernel Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#ifndef __BANANAPI_M5_H__
#define __BANANAPI_M5_H__

#define CONFIG_DEVICE_PRODUCT		"bananapi_firewalla"
#define BANANAPI_BOARD_UUID		"9098004a-a1dd-11e8-98d0-529269fb1459"

/* configs for CEC */
#define CONFIG_CEC_OSD_NAME		"BANANAPI-FIREWALLA"
#define CONFIG_CEC_WAKEUP

//#define CONFIG_AML_SPICC
//#define CONFIG_I2C_EEPROM

#include "bananapi-g12-common.h"

/* vpu */
#undef CONFIG_AML_VPU

/* DISPLAY & HDMITX */
#undef CONFIG_AML_HDMITX20
#undef CONFIG_AML_CANVAS
#undef CONFIG_AML_VOUT
#undef CONFIG_AML_OSD
#undef CONFIG_OSD_SCALE_ENABLE
#undef CONFIG_CMD_BMP
#undef CONFIG_AML_CVBS

#if defined(CONFIG_CMD_USB)
	#define CONFIG_USB_HUB_RST_N            -1
        #define CONFIG_USB_HUB_RST_N_NAME       ""
        /* USB Host Hub Enable */
        #define CONFIG_USB_HUB_CHIP_EN          -1
        #define CONFIG_USB_HUB_CHIP_EN_NAME     ""

	/* USB OTG Power Enable */
	#define CONFIG_USB_GPIO_PWR		-1
	#define CONFIG_USB_GPIO_PWR_NAME	""
#endif

/* only one i2c master initial?? */
/* i2c_2 GPIO_X10/X11  */
//#define CONFIG_SYS_I2C_AML_I2C1

/* i2c_2 GPIO_X17/X18  */
//#define CONFIG_SYS_I2C_AML_I2C2

/* i2c_3 GPIO_A14/A15 */
#define CONFIG_SYS_I2C_AML_I2C3

#endif
