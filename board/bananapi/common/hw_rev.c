// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <asm/io.h>
#include <amlogic/cpu_id.h>
#include <amlogic/saradc.h>
#include <asm/arch/secure_apb.h>
#include <bananapi-common.h>

#define MAINBOARD_ADC_CHANNEL		1
#define IOBOARD_ADC_CHANNEL		6

struct main_boards {
       unsigned int adc_min;
       unsigned int adc_max;
       char *variant;
} main_boards[] = {
       /* CM5-V00 2023,03,24 */
       { 40, 120, "cm5" },
};

struct io_boards {
       unsigned int adc_min;
       unsigned int adc_max;
       char *variant;
} io_boards[] = {
       /* BPI CM5IO-V00 2023,08,16 */
       { 0, 50, "cm5io" },
        /* BPI CM4IO-V10 2022,12,13 */
       { 0, 50, "cm4io" },
        /* CM5IO-V00 2023,08,16 */
       { 980, 1060, "rpicm4io" },
};

static void board_set_fdtfile(char *main_board, char *io_board)
{
       char s[128];

       snprintf(s, sizeof(s), "amlogic/bananapi_%s.dtb",  io_board);
       env_set("fdtfile", s);
}

int board_detect(void)
{
	char *main_board = "", *io_board = "";
	unsigned int main_adcval, io_adcval = 0;
	int ret, i = 0;

	ret = adc_channel_single_shot_mode("adc", ADC_MODE_HIGH_PRECISION,
			MAINBOARD_ADC_CHANNEL, &main_adcval);
	if (ret)
		return ret;

	printf("main board adc val is %d\n", main_adcval);
	for (i = 0 ; i < ARRAY_SIZE(main_boards) ; ++i) {
		if (main_adcval >= main_boards[i].adc_min && main_adcval < main_boards[i].adc_max) {
			main_board = main_boards[i].variant;
			break;
		}
	}

	ret = adc_channel_single_shot_mode("adc", ADC_MODE_HIGH_PRECISION,
			IOBOARD_ADC_CHANNEL, &io_adcval);
	if (ret)
		return ret;

	printf("io board adc val is %d\n", io_adcval);
	for (i = 0 ; i < ARRAY_SIZE(io_boards) ; ++i) {
		if (io_adcval >= io_boards[i].adc_min && 	io_adcval < io_boards[i].adc_max) {
			io_board = io_boards[i].variant;
			break;
		}
	}

	printf("Board is: %s\n", main_board);
	printf("IO Board is: %s\n", io_board);

	board_set_fdtfile(main_board, io_board);

	return 0;
}

static void get_chipid(void)
{
	int i;
	uint8_t chipid[16];
	char buf[33];

	memset(chipid, 0, sizeof(chipid));
	memset(buf, 0, sizeof(buf));

	if (get_chip_id(chipid, sizeof(chipid)) == 0) {
		for (i = 0; i < 16; i++)
			sprintf(buf + 2 * i, "%02x", chipid[i]);
	}
	else {
		printf("get chip id error\n");
	}

	printf("serial:%s(from chipid)\n", buf);

	env_set("chipid", buf);
	env_set("serial", buf);
}

void get_board_serial(void)
{
        get_chipid();
}
