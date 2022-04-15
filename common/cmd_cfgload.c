/*
 * Copyright (C) 2017 Wesion Co,. Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/ctype.h>    /* isalpha, isdigit */
#include <linux/sizes.h>

#ifdef CONFIG_SYS_HUSH_PARSER
#include <cli_hush.h>
#endif

#define SZ_BOOTINI		SZ_64K

/* Nothing to proceed with zero size string or comment.
 *
 * FIXME: Do we really need to strip the line start with '#' or ';',
 *        since any U-boot command does not start with punctuation character.
 */
static int valid_command(const char* p)
{
	char *q;

	for (q = (char*)p; *q; q++) {
		if (isblank(*q)) continue;
		if (isalnum(*q)) return 1;
		if (ispunct(*q))
			return (*q != '#') && (*q != ';');
	}

	return !(p == q);
}

/* Read boot.ini from FAT partition
 */
#define MAX_DEV_NUM		2
static char* read_cfgload(void)
{
	char cmd[128];
	unsigned long filesize;
	char *p;

	const char *device_pairs[MAX_DEV_NUM] = {"mmc", "usb"};
	const char *partition_pairs[][2] = {
	    //partition		 boot.ini
		{"0:1", 		"/boot.ini"},
		{"0:1",			"/boot/boot.ini"},
		{"1:1", 		"/boot.ini"},
		{"1:1",			"/boot/boot.ini"},
		{"1:5", 		"/boot/boot.ini"}
	};
	int partition_array_len = sizeof(partition_pairs) / sizeof(partition_pairs[0]);
	int i = 0;
	int j = 0;
	char magic[32];
	char buf[64] = {0};
	int size;

	p = (char *)simple_strtoul(getenv("loadaddr"), NULL, 16);
	if (NULL == p)
		p = (char *)CONFIG_SYS_LOAD_ADDR;

	for (j=0; j<MAX_DEV_NUM; j++) {
		/* set devtype for bootscript use */
		sprintf(buf, "setenv devtype %s", device_pairs[j]);
		run_command(buf, 0);

		for (i=0; i<partition_array_len; i++) {
			/* set devno for bootscript use */
			if (i<2)
				setenv("devno", "0");
			else
				setenv("devno", "1");

			setenv("filesize", "0");
			printf("cfgload: reading %s from %s %s ...\n", partition_pairs[i][1], device_pairs[j], partition_pairs[i][0]);
			sprintf(cmd, "load %s %s 0x%p %s", device_pairs[j], partition_pairs[i][0], (void *)p, partition_pairs[i][1]);
			run_command(cmd, 0);

			filesize = getenv_ulong("filesize", 16, 0);
			if (0 != filesize)
				goto got_bootscript;

			printf("cfgload: no %s or empty file on %s %s\n", partition_pairs[i][1], device_pairs[j], partition_pairs[i][0]);
		}
	}

	if (i == partition_array_len && j == MAX_DEV_NUM) {
		printf("cfgload: failed to read boot.ini on all partitions!\n");

		return NULL;
	}

got_bootscript:
	if (filesize > SZ_BOOTINI) {
		printf("cfgload: 'boot.ini' exceeds %d, size=%ld\n",
				SZ_BOOTINI, filesize);
		return NULL;
    }

	/* Terminate the read buffer with '\0' to be treated as string */
	*(char *)(p + filesize) = '\0';

	/* Scan MAGIC string, readed boot.ini must start with exact magic string.
	 * Otherwise, we will not proceed at all.
	 */

	size = snprintf(magic, sizeof(magic), "%s-UBOOT-CONFIG", CONFIG_DEVICE_PRODUCT);
	while (*p) {
		char *s = strsep(&p, "\n");
		if (!valid_command(s))
			continue;

		/* MAGIC string is discovered, return the buffer address of next to
		 * proceed the commands.
		 */
		if (!strncmp(s, magic, size))
			return memcpy(malloc(filesize), p, filesize);
	}

	printf("cfgload: MAGIC NAME, %s, is not found!!\n", CONFIG_DEVICE_PRODUCT);

	return NULL;
}

static int do_load_cfgload(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	char *p;
	char cmd[64];

	printf("cfgload: start ...\n");

	p = read_cfgload();
	if (NULL == p)
		return 0;

	printf("cfgload: applying boot.ini...\n");

	sprintf(cmd, "script %p %ld 0", p, strlen(p));
	run_command(cmd, 0);

	return 0;
}

U_BOOT_CMD(
		cfgload,		1,		0,		do_load_cfgload,
		"read 'boot.ini' from FAT/EXT4 partition",
		"\n"
		"    - read boot.ini from the boot partition"
);
