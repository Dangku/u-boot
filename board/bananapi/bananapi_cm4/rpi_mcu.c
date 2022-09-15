/*
 *
 * rpi 7 lcd Touchscreen MCU driver.
 *
 * Copyright (c) 2016 ASUSTek Computer Inc.
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#include <errno.h>
#include <common.h>
#include <i2c.h>
#include <rpi_mcu.h>

#define MCU_I2C_ADDR	0x45

static int connected = 0;

int rpi_mcu_screen_power_up(void)
{
	u8 val[1];

	if (!connected)
		return -ENODEV;

	val[0] = 0x00;
	if(i2c_write(MCU_I2C_ADDR, 0x85, 1, val, 1))
		printf("send 8500 failed\n");
	mdelay(800);

	val[0] = 0x01;
	if(i2c_write(MCU_I2C_ADDR, 0x85, 1, val, 1))
		printf("send 8501 failed\n");
	mdelay(800);

	val[0] = 0x04;
	if(i2c_write(MCU_I2C_ADDR, 0x81, 1, val, 1))
		printf("send 8104 failed\n");

	return 0;
}

int rpi_mcu_set_bright(int bright)
{
	u8 val[1];

	if (!connected)
		return -ENODEV;

	if (bright > 0xff || bright < 0)
		return -EINVAL;

	val[0] = bright;
	if(i2c_write(MCU_I2C_ADDR, 0x86, 1, val, 1))
		printf("send bright failed\n");

	return 0;
}

static int rpi_mcu_detect(void)
{
	uchar val[1];

	if(i2c_read(MCU_I2C_ADDR, 0x80, 1, val, 1)){
		printf("rpi mcu i2c read fail\n");
		return -ENODEV;
	}
	
	printf("%s, recv_cmds: 0x%X\n", __func__, val[0]);
	if (val[0] != 0xDE && val[0] != 0xC3) {
		printf("read wrong info\n");
		return -EINVAL;
	}

	return 0;
}

int rpi_mcu_init(void)
{
	int ret;
 
	printf("%s\n", __func__);
 
	ret = rpi_mcu_detect();
	if (ret < 0) {
		printf("init_cmd_check failed, %d\n", ret);
		setenv_ulong("lcd_exist", 0);
		return -EINVAL;
	}
	connected = 1;
	mdelay(20);

	rpi_mcu_screen_power_up();
	mdelay(800);
	rpi_mcu_set_bright(0xFF);

	setenv_ulong("lcd_exist", 1);
 
	return 0;
 }

