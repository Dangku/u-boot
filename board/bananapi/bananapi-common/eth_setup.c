/*
 * board/bananapi/bananapi-common/eth_setup.c
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <environment.h>
#include <fdt_support.h>
#include <libfdt.h>
#include <asm/arch/eth_setup.h>

#define MAC_LENGHT            6

/*
 * setup eth device board socket
 */
struct eth_board_socket* eth_board_setup(char *name)
{
	struct eth_board_socket* new_board;

	new_board = (struct eth_board_socket*)malloc(sizeof(struct eth_board_socket));
	if (NULL == new_board)
		return NULL;

	if (name != NULL) {
		new_board->name = (char*)malloc(strlen(name));
		strncpy(new_board->name, name, strlen(name));
	}else{
		new_board->name = "gxb";
	}

	new_board->eth_pinmux_setup = NULL ;
	new_board->eth_clock_configure = NULL;
	new_board->eth_hw_reset = NULL;

	return new_board;
}

#if 0
static unsigned char chartonum(char c)
{
        if (c >= '0' && c <= '9')
                return c - '0';
        if (c >= 'A' && c <= 'F')
                return (c - 'A') + 10;
        if (c >= 'a' && c <= 'f')
                return (c - 'a') + 10;
        return 0;
}

void get_mac_i2c_eeprom(void)
{

}

void get_mac_spi_eeprom(void)
{

}

void get_mac_efuse(void)
{
	char mac[64];
	int mac_addr[MAC_LENGHT] = {0};
	int i;

	//get mac from efuse and set eth_mac env
	run_command("efuse mac", 0);

	char *s = getenv("eth_mac");
	if ((s != NULL) && (strcmp(s, "00:00:00:00:00:00") != 0)) {
		for (i = 0; i < 6 && s[0] != '\0' && s[1] != '\0'; i++) {
			mac_addr[i] = chartonum(s[0]) << 4 | chartonum(s[1]);
			s +=3;
		}
	} else {
		
	}

	printf("mac address: ");

	for (i=0; i<MAC_LENGHT; i++) {
		if (i == (MAC_LENGHT-1))
			printf("%02x",mac_addr[i]);
		else
			printf("%02x:",mac_addr[i]);
	}

	printf("\n");
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	setenv("eth_mac", mac);
}
#endif
