#include <common.h>
#include <asm/io.h>
#include <asm/arch/secure_apb.h>
#include <linux/kernel.h>
#include <asm/arch/efuse.h>
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

static int get_board_type(void)
{
	int board_type = -1;
	int adc;

	adc = get_adc_value(BOARD_TYPE_CHANNEL);
	printf("BPI, saradc_ch0=%d\n", adc);
#if defined(CONFIG_BANANAPI_M2S)
	if (IS_RANGE(adc, 0, 50))		/* board is s922x_m2s */
		board_type = BOARD_S922X_M2S;
	else if (IS_RANGE(adc, 900, 1100))	/* board is a311d_m2s */
		board_type = BOARD_A311D_M2S;
#elif defined(CONFIG_BANANAPI_CM4)
	if (IS_RANGE(adc, 470, 550))		/* board is a311d_cm4 */
		board_type = BOARD_A311D_CM4;
#endif

	return board_type;
}

#if defined(CONFIG_BANANAPI_CM4)
static int get_cm4io_board_type(void)
{
	int board_type = -1;
	int adc;

	adc = get_adc_value(CM4IO_BOARD_TYPE_CHANNEL);
	printf("BPI, saradc_ch3=%d\n", adc);
	if (IS_RANGE(adc, 0, 50))		/* board is bpi cm4io */
		board_type = BOARD_CM4IO_BPI;
	else if (IS_RANGE(adc, 900, 1100))	/* board is rpi cm4io */
		board_type = BOARD_CM4IO_RPI;

	return board_type;
}
#endif

#if 0
static unsigned int get_hw_revision(void)
{
	int hwrev = -1;
	int adc;

	adc = get_adc_value(BOARD_REV_CHANNEL);
#if defined(CONFIG_BANANAPI_M2S)
	if (IS_RANGE(adc, 900, 1100))
		hwrev = BOARD_REVISION(2022, 04, 20);
#elif defined(CONFIG_BANANAPI_CM4)
	if (IS_RANGE(adc, 0, 50))
                hwrev = BOARD_REVISION(2022, 08, 20);
#endif

	printf("ADC=%d, hwrev=0x%x\n", adc, hwrev);

	return hwrev;
}

static int board_rev = -1;
int board_revision(void)
{
	if (board_rev == -1)
		board_rev = get_hw_revision();

	return board_rev;
}
#endif

#if defined(CONFIG_BANANAPI_M2S)
int board_is_bananapi_s922x_m2s(void)
{
	return (get_board_type() == BOARD_S922X_M2S);
}
int board_is_bananapi_a311d_m2s(void)
{
	return (get_board_type() == BOARD_A311D_M2S);
}
#elif defined(CONFIG_BANANAPI_CM4)
int board_is_bananapi_cm4(void)
{
	return (get_board_type() == BOARD_A311D_CM4);
}

int board_is_bpi_cm4io(void)
{
	return (get_cm4io_board_type() == BOARD_CM4IO_BPI);
}

int board_is_rpi_cm4io(void)
{
	return (get_cm4io_board_type() == BOARD_CM4IO_RPI);
}
#endif

#if defined(CONFIG_EFUSE_SN)
int get_efuse_board_serial(void)
{
	char buf[EFUSE_BYTES];	// sn address buffer
	uint32_t size = CONFIG_EFUSE_SN_LEN;
	loff_t pos = CONFIG_EFUSE_SN_POS;	// offset of the first byte for sn address
	int ret;

	memset(buf, 0, sizeof(buf));

	ret = efuse_read_usr(buf, size, &pos);
	if (ret < 0) {
		memset(buf, 0, sizeof(buf));
		error("read serial from efuse failed\n");
		return -EINVAL;
	}

	printf("serial:%s(from efuse)\n", buf);

	setenv("serial", buf);

	return 0;
}
#endif

void board_chip_id(void)
{
	int i;
	uint8_t chipid[16];
	char buf[16];

	memset(chipid, 0, sizeof(chipid));
	memset(buf, 0, sizeof(buf));


	if (get_chip_id(chipid, sizeof(chipid)) == 0) {
		for (i = 0; i < sizeof(chipid); i++)
			sprintf(buf + 2 * i, "%02x", chipid[i]);
	}
	else {
		printf("get chip id error\n");
	}

    printf("serial:%s(from chipid)\n", buf);

	printf("%s\n", buf);
	setenv("chipid", buf);
	setenv("serial", buf);
}

void get_board_serial(void)
{
#if defined(CONFIG_EFUSE_SN)
	get_efuse_board_serial();
#else
	board_chip_id();
#endif
}