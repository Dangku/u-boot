/*
 * (C) Copyright 2001-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <miiphy.h>
#include <phy.h>
#include <asm/errno.h>
#include <amlogic/keyunify.h>
#include <net.h>
#include <asm/cpu_id.h>
#ifdef CONFIG_RANDOM_ETHADDR
#include <asm/arch/io.h>
#include <asm/arch/secure_apb.h>
#endif
#include <asm/arch/efuse.h>
#include <i2c.h>

void eth_parse_enetaddr(const char *addr, uchar *enetaddr)
{
	char *end;
	int i;

	for (i = 0; i < 6; ++i) {
		enetaddr[i] = addr ? simple_strtoul(addr, &end, 16) : 0;
		if (addr)
			addr = (*end) ? end + 1 : end;
	}
}

int eth_getenv_enetaddr(char *name, uchar *enetaddr)
{
	eth_parse_enetaddr(getenv(name), enetaddr);
	return is_valid_ether_addr(enetaddr);
}

int eth_setenv_enetaddr(char *name, const uchar *enetaddr)
{
	char buf[20];

	sprintf(buf, "%pM", enetaddr);

	return setenv(name, buf);
}

int eth_getenv_enetaddr_by_index(const char *base_name, int index,
				 uchar *enetaddr)
{
	char enetvar[32];
	sprintf(enetvar, index ? "%s%daddr" : "%saddr", base_name, index);
	return eth_getenv_enetaddr(enetvar, enetaddr);
}

static inline int eth_setenv_enetaddr_by_index(const char *base_name, int index,
				 uchar *enetaddr)
{
	char enetvar[32];
	sprintf(enetvar, index ? "%s%daddr" : "%saddr", base_name, index);
	return eth_setenv_enetaddr(enetvar, enetaddr);
}


static int eth_mac_skip(int index)
{
	char enetvar[15];
	char *skip_state;
	sprintf(enetvar, index ? "eth%dmacskip" : "ethmacskip", index);
	return ((skip_state = getenv(enetvar)) != NULL);
}

/*
 * CPU and board-specific Ethernet initializations.  Aliased function
 * signals caller to move on
 */
static int __def_eth_init(bd_t *bis)
{
	return -1;
}
int cpu_eth_init(bd_t *bis) __attribute__((weak, alias("__def_eth_init")));
int board_eth_init(bd_t *bis) __attribute__((weak, alias("__def_eth_init")));

#ifdef CONFIG_API
static struct {
	uchar data[PKTSIZE];
	int length;
} eth_rcv_bufs[PKTBUFSRX];

static unsigned int eth_rcv_current, eth_rcv_last;
#endif

static struct eth_device *eth_devices;
struct eth_device *eth_current;

struct eth_device *eth_get_dev_by_name(const char *devname)
{
	struct eth_device *dev, *target_dev;

	BUG_ON(devname == NULL);

	if (!eth_devices)
		return NULL;

	dev = eth_devices;
	target_dev = NULL;
	do {
		if (strcmp(devname, dev->name) == 0) {
			target_dev = dev;
			break;
		}
		dev = dev->next;
	} while (dev != eth_devices);

	return target_dev;
}

struct eth_device *eth_get_dev_by_index(int index)
{
	struct eth_device *dev, *target_dev;

	if (!eth_devices)
		return NULL;

	dev = eth_devices;
	target_dev = NULL;
	do {
		if (dev->index == index) {
			target_dev = dev;
			break;
		}
		dev = dev->next;
	} while (dev != eth_devices);

	return target_dev;
}

int eth_get_dev_index(void)
{
	if (!eth_current)
		return -1;

	return eth_current->index;
}

static void eth_current_changed(void)
{
	char *act = getenv("ethact");
	/* update current ethernet name */
	if (eth_current) {
		if (act == NULL || strcmp(act, eth_current->name) != 0)
			setenv("ethact", eth_current->name);
	}
	/*
	 * remove the variable completely if there is no active
	 * interface
	 */
	else if (act != NULL)
		setenv("ethact", NULL);
}

static int eth_address_set(unsigned char *addr)
{
	return memcmp(addr, "\0\0\0\0\0\0", 6);
}

#ifdef CONFIG_RANDOM_ETHADDR
static inline void eth_hw_addr_random(struct eth_device *dev)
{
	unsigned int tmp = readl(P_RNG_DATA);

	dev->enetaddr[0] = tmp&0xFF;
	dev->enetaddr[1] = (tmp>>8)&0xFF;
	dev->enetaddr[2] = (tmp>>16)&0xFF;
	dev->enetaddr[3] = (tmp>>24)&0xFF;

	tmp = readl(P_RNG_DATA);

	dev->enetaddr[4] = tmp&0xFF;
	dev->enetaddr[5] = (tmp>>8)&0xFF;

	dev->enetaddr[0] &= 0xFE;
	dev->enetaddr[0] |= 0x02;
}
#endif

#if defined(CONFIG_I2C_EEPROM_MAC)
static inline int eth_get_i2c_eeprom_mac(struct eth_device *dev)
{
	char buf[CONFIG_I2C_EEPROM_MAC_LEN];
	int ret;

	ret = i2c_read(CONFIG_I2C_EEPROM_ADDR, CONFIG_I2C_EEPROM_MAC_POS, 1, (uchar*)buf, CONFIG_I2C_EEPROM_MAC_LEN);
    if (ret) {
        printf("Error reading the i2c eeprom: %d\n",ret);
		memset(buf, 0, sizeof(buf));
		return -EINVAL; 
    }

	printf("\nBPI: (from i2c eeprom) mac: %s\n", buf);

	int i;
	char s[3];
	char *p = buf;
	for (i = 0; i < 6; i++) {
		s[0] = *p++;
		s[1] = *p++;
		s[2] = 0;
		dev->enetaddr[i] = simple_strtoul(s, NULL, 16);
	}

	return 0;
}
#endif

#if defined(CONFIG_EFUSE_MAC)
static inline int eth_get_efuse_mac(struct eth_device *dev)
{
	char buf[CONFIG_EFUSE_MAC_LEN];	// MAC address buffer
	loff_t pos = CONFIG_EFUSE_MAC_POS;	// offset of the first byte for MAC address
	int ret;

	ret = efuse_read_usr(buf, sizeof(buf), &pos);
	if (ret < 0) {
		memset(buf, 0, sizeof(buf));
		return -EINVAL;
	}

	printf("\nBPI: (from efuse)mac: %s\n", buf);

	int i;
	char s[3];
	char *p = buf;
	for (i = 0; i < 6; i++) {
		s[0] = *p++;
		s[1] = *p++;
		s[2] = 0;
		dev->enetaddr[i] = simple_strtoul(s, NULL, 16);
	}

	return 0;
}
#endif

int eth_write_hwaddr(struct eth_device *dev, const char *base_name,
		   int eth_number)
{
	unsigned char env_enetaddr[6];
	int ret = 0;

#if defined(CONFIG_I2C_EEPROM_MAC)
	eth_get_i2c_eeprom_mac(dev);
#elif defined(CONFIG_EFUSE_MAC)
	eth_get_efuse_mac(dev);
#endif

	if (is_valid_ether_addr(dev->enetaddr)) {
		eth_setenv_enetaddr_by_index(base_name, eth_number,
						dev->enetaddr);
	} else {
		eth_getenv_enetaddr_by_index(base_name, eth_number, env_enetaddr);
		if(env_enetaddr[0] == 0x0 && env_enetaddr[1] == 0x15 && env_enetaddr[2] == 0x18
		&& env_enetaddr[3] == 0x01 && env_enetaddr[4] == 0x81 && env_enetaddr[5] == 0x31)
		{
#ifdef CONFIG_RANDOM_ETHADDR
		eth_hw_addr_random(dev);
		eth_setenv_enetaddr_by_index(base_name, eth_number,
				dev->enetaddr);
#endif
		}

		/* random mac from chipid */
		uint8_t buff[16];
		if (get_chip_id(&buff[0], sizeof(buff)) == 0) {
			sprintf((char *)env_enetaddr,"02:%02x:%02x:%02x:%02x:%02x",buff[8],
				buff[7],buff[6],buff[5],buff[4]);
			printf("\nBPI: (from chipid)mac: %s\n", env_enetaddr);
			setenv("ethaddr",(const char *)env_enetaddr);
		}

		eth_getenv_enetaddr_by_index(base_name, eth_number, env_enetaddr);

		if (eth_address_set(env_enetaddr)) {
			if (eth_address_set(dev->enetaddr) &&
					memcmp(dev->enetaddr, env_enetaddr, 6)) {
				printf("\nWarning: %s MAC addresses don't match:\n",
						dev->name);
				printf("Address in SROM is         %pM\n",
						dev->enetaddr);
				printf("Address in environment is  %pM\n",
						env_enetaddr);
			}

			memcpy(dev->enetaddr, env_enetaddr, 6);
		} else if (is_valid_ether_addr(dev->enetaddr)) {
			eth_setenv_enetaddr_by_index(base_name, eth_number,
					dev->enetaddr);
			printf("\nWarning: %s using MAC address from net device\n",
					dev->name);
		} else if (!(eth_address_set(dev->enetaddr))) {
			printf("\nError: %s address not set.\n",
					dev->name);
			return -EINVAL;
		}
	}

	if (dev->write_hwaddr && !eth_mac_skip(eth_number)) {
		if (!is_valid_ether_addr(dev->enetaddr)) {
			printf("\nError: %s address %pM illegal value\n",
				 dev->name, dev->enetaddr);
			return -EINVAL;
		}

		ret = dev->write_hwaddr(dev);
		if (ret)
			printf("\nWarning: %s failed to set MAC address\n", dev->name);
	}

	return ret;
}

int eth_register(struct eth_device *dev)
{
	struct eth_device *d;
	static int index;

	assert(strlen(dev->name) < sizeof(dev->name));

	if (!eth_devices) {
		eth_current = eth_devices = dev;
		eth_current_changed();
	} else {
		for (d = eth_devices; d->next != eth_devices; d = d->next)
			;
		d->next = dev;
	}

	dev->state = ETH_STATE_INIT;
	dev->next  = eth_devices;
	dev->index = index++;

	return 0;
}

int eth_unregister(struct eth_device *dev)
{
	struct eth_device *cur;

	/* No device */
	if (!eth_devices)
		return -1;

	for (cur = eth_devices; cur->next != eth_devices && cur->next != dev;
	     cur = cur->next)
		;

	/* Device not found */
	if (cur->next != dev)
		return -1;

	cur->next = dev->next;

	if (eth_devices == dev)
		eth_devices = dev->next == eth_devices ? NULL : dev->next;

	if (eth_current == dev) {
		eth_current = eth_devices;
		eth_current_changed();
	}

	return 0;
}

static void eth_env_init(bd_t *bis)
{
	const char *s;

	if ((s = getenv("bootfile")) != NULL)
		copy_filename(BootFile, s, sizeof(BootFile));
}

int eth_initialize(bd_t *bis)
{
	int num_devices = 0;
	eth_devices = NULL;
	eth_current = NULL;

	bootstage_mark(BOOTSTAGE_ID_NET_ETH_START);
#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) || defined(CONFIG_PHYLIB)
	miiphy_init();
#endif

#ifdef CONFIG_PHYLIB
	phy_init();
#endif

	eth_env_init(bis);

	/*
	 * If board-specific initialization exists, call it.
	 * If not, call a CPU-specific one
	 */
	if (board_eth_init != __def_eth_init) {
		if (board_eth_init(bis) < 0)
			printf("Board Net Initialization Failed\n");
	} else if (cpu_eth_init != __def_eth_init) {
		if (cpu_eth_init(bis) < 0)
			printf("CPU Net Initialization Failed\n");
	} else
		printf("Net Initialization Skipped\n");

	if (!eth_devices) {
		puts("No ethernet found.\n");
		bootstage_error(BOOTSTAGE_ID_NET_ETH_START);
	} else {
		struct eth_device *dev = eth_devices;
		char *ethprime = getenv("ethprime");

		bootstage_mark(BOOTSTAGE_ID_NET_ETH_INIT);
		do {
			if (dev->index)
				puts(", ");

			printf("%s", dev->name);

			if (ethprime && strcmp(dev->name, ethprime) == 0) {
				eth_current = dev;
				puts(" [PRIME]");
			}

			if (strchr(dev->name, ' '))
				puts("\nWarning: eth device name has a space!"
					"\n");

			eth_write_hwaddr(dev, "eth", dev->index);

			dev = dev->next;
			num_devices++;
		} while (dev != eth_devices);

		eth_current_changed();
		putc('\n');
	}

	return num_devices;
}

#ifdef CONFIG_MCAST_TFTP
/* Multicast.
 * mcast_addr: multicast ipaddr from which multicast Mac is made
 * join: 1=join, 0=leave.
 */
int eth_mcast_join(IPaddr_t mcast_ip, u8 join)
{
	u8 mcast_mac[6];
	if (!eth_current || !eth_current->mcast)
		return -1;
	mcast_mac[5] = htonl(mcast_ip) & 0xff;
	mcast_mac[4] = (htonl(mcast_ip)>>8) & 0xff;
	mcast_mac[3] = (htonl(mcast_ip)>>16) & 0x7f;
	mcast_mac[2] = 0x5e;
	mcast_mac[1] = 0x0;
	mcast_mac[0] = 0x1;
	return eth_current->mcast(eth_current, mcast_mac, join);
}

/* the 'way' for ethernet-CRC-32. Spliced in from Linux lib/crc32.c
 * and this is the ethernet-crc method needed for TSEC -- and perhaps
 * some other adapter -- hash tables
 */
#define CRCPOLY_LE 0xedb88320
u32 ether_crc(size_t len, unsigned char const *p)
{
	int i;
	u32 crc;
	crc = ~0;
	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
	}
	/* an reverse the bits, cuz of way they arrive -- last-first */
	crc = (crc >> 16) | (crc << 16);
	crc = (crc >> 8 & 0x00ff00ff) | (crc << 8 & 0xff00ff00);
	crc = (crc >> 4 & 0x0f0f0f0f) | (crc << 4 & 0xf0f0f0f0);
	crc = (crc >> 2 & 0x33333333) | (crc << 2 & 0xcccccccc);
	crc = (crc >> 1 & 0x55555555) | (crc << 1 & 0xaaaaaaaa);
	return crc;
}

#endif


int eth_init(bd_t *bis)
{
	struct eth_device *old_current, *dev;

	if (!eth_current) {
		puts("No ethernet found.\n");
		return -1;
	}

	/* Sync environment with network devices */
	dev = eth_devices;
	do {
		uchar env_enetaddr[6];

		if (eth_getenv_enetaddr_by_index("eth", dev->index,
						 env_enetaddr))
			memcpy(dev->enetaddr, env_enetaddr, 6);

		dev = dev->next;
	} while (dev != eth_devices);

	old_current = eth_current;
	do {
		debug("Trying %s\n", eth_current->name);

		if (eth_current->init(eth_current, bis) >= 0) {
			eth_current->state = ETH_STATE_ACTIVE;

			return 0;
		}
		debug("FAIL\n");

		eth_try_another(0);
	} while (old_current != eth_current);

	return -1;
}

void eth_halt(void)
{
	if (!eth_current)
		return;

	eth_current->halt(eth_current);

	eth_current->state = ETH_STATE_PASSIVE;
}

int eth_send(void *packet, int length)
{
	if (!eth_current)
		return -1;

	return eth_current->send(eth_current, packet, length);
}

int eth_rx(void)
{
	if (!eth_current)
		return -1;

	return eth_current->recv(eth_current);
}

#ifdef CONFIG_API
static void eth_save_packet(void *packet, int length)
{
	char *p = packet;
	int i;

	if ((eth_rcv_last+1) % PKTBUFSRX == eth_rcv_current)
		return;

	if (PKTSIZE < length)
		return;

	for (i = 0; i < length; i++)
		eth_rcv_bufs[eth_rcv_last].data[i] = p[i];

	eth_rcv_bufs[eth_rcv_last].length = length;
	eth_rcv_last = (eth_rcv_last + 1) % PKTBUFSRX;
}

int eth_receive(void *packet, int length)
{
	char *p = packet;
	void *pp = push_packet;
	int i;

	if (eth_rcv_current == eth_rcv_last) {
		push_packet = eth_save_packet;
		eth_rx();
		push_packet = pp;

		if (eth_rcv_current == eth_rcv_last)
			return -1;
	}

	length = min(eth_rcv_bufs[eth_rcv_current].length, length);

	for (i = 0; i < length; i++)
		p[i] = eth_rcv_bufs[eth_rcv_current].data[i];

	eth_rcv_current = (eth_rcv_current + 1) % PKTBUFSRX;
	return length;
}
#endif /* CONFIG_API */

void eth_try_another(int first_restart)
{
	static struct eth_device *first_failed;
	char *ethrotate;

	/*
	 * Do not rotate between network interfaces when
	 * 'ethrotate' variable is set to 'no'.
	 */
	ethrotate = getenv("ethrotate");
	if ((ethrotate != NULL) && (strcmp(ethrotate, "no") == 0))
		return;

	if (!eth_current)
		return;

	if (first_restart)
		first_failed = eth_current;

	eth_current = eth_current->next;

	eth_current_changed();

	if (first_failed == eth_current)
		NetRestartWrap = 1;
}

void eth_set_current(void)
{
	static char *act;
	static int  env_changed_id;
	struct eth_device *old_current;
	int	env_id;

	if (!eth_current)	/* XXX no current */
		return;

	env_id = get_env_id();
	if ((act == NULL) || (env_changed_id != env_id)) {
		act = getenv("ethact");
		env_changed_id = env_id;
	}
	if (act != NULL) {
		old_current = eth_current;
		do {
			if (strcmp(eth_current->name, act) == 0)
				return;
			eth_current = eth_current->next;
		} while (old_current != eth_current);
	}

	eth_current_changed();
}

char *eth_get_name(void)
{
	return eth_current ? eth_current->name : "unknown";
}
