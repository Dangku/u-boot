#include <common.h>
#include <asm/io.h>
#include <asm/arch/secure_apb.h>
#include <linux/kernel.h>
#include <asm/cpu_id.h>
#include <bananapi-common.h>
const char *boot_device_name(int n)
{
	struct names {
		int id;
		const char* name;
	} names[] = {
		{ BOOT_DEVICE_RESERVED, "reserved" },
		{ BOOT_DEVICE_EMMC, "emmc" },
		{ BOOT_DEVICE_NAND, "nand" },
		{ BOOT_DEVICE_SPI, "spi" },
		{ BOOT_DEVICE_SD, "sd" },
		{ BOOT_DEVICE_USB, "usb" },
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(names); i++)
		if (names[i].id == n)
			return names[i].name;

	return NULL;
}

int get_boot_device(void)
{
	return readl(AO_SEC_GP_CFG0) & 0xf;
}

static int board_rev = -1;

#define IS_RANGE(x, min, max)   ((x) > (min) && (x) < (max))

#if 0
static void get_board_type(void)
{
	int adc;

	adc = get_adc_value(BOARD_TYPE_CHANNEL);
#if defined(CONFIG_BANANAPI_M2S)
	if (IS_RANGE(adc, 80, 100)) {
		/* board is s922x_m2s */
	} else if (IS_RANGE(adc, 900, 1100)) {
		/* board is a311d_m2s */
	}
#endif
}
#endif

static unsigned int get_hw_revision(void)
{
	int hwrev = -1;
	int adc;

	adc = get_adc_value(BOARD_REV_CHANNEL);
#if defined(CONFIG_BANANAPI_M2S)
	if (IS_RANGE(adc, 900, 1100))
		hwrev = BOARD_REVISION(2022, 04, 20);
#endif

	printf("ADC=%d, hwrev=0x%x\n", adc, hwrev);

	return hwrev;
}

static void board_chip_id(void)
{
	int i;
	uint8_t chipid[16];
	char buf[16];

	memset(chipid, 0, sizeof(chipid));
	memset(buf, 0, sizeof(buf));

	printf("chipid: ");

	if (get_chip_id(chipid, sizeof(chipid)) == 0) {
		for (i = 0; i < sizeof(chipid); i++)
			sprintf(buf + 2 * i, "%02x", chipid[i]);
	}
	else {
		printf("get chip id error\n");
	}

	printf("%s\n", buf);
	setenv("chipid", buf);
}

void get_board_serial(void)
{
	board_chip_id();
	setenv("serial", getenv("chipid"));
}

int board_revision(void)
{
	if (board_rev == -1)
		board_rev = get_hw_revision();

	return board_rev;
}

#if defined(CONFIG_BANANAPI_M2S)
int board_is_bananapi_m2s(void)
{
	return (board_revision() == 0x20220420);
}
#endif