/*
 * include/configs/bananapi_cm4.h
 *
 * (C) Copyright 2018 Hardkernel Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#ifndef __BANANAPI_CM4_H__
#define __BANANAPI_CM4_H__

#define CONFIG_DEVICE_PRODUCT		"BANANAPI_CM4"

/* configs for CEC */
#define CONFIG_CEC_OSD_NAME		"BANANAPI_CM4"
//#define CONFIG_CEC_WAKEUP


#include "bananapi_g12b_common.h"

#endif

/* only one i2c master initial?? */
/* i2c_2 GPIO_X10/X11  */
//#define CONFIG_SYS_I2C_AML_I2C1

/* i2c_2 GPIO_X17/X18  */
#define CONFIG_SYS_I2C_AML_I2C2

/* i2c_3 GPIO_A14/A15 */
//#define CONFIG_SYS_I2C_AML_I2C3

