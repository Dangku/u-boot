#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/secure_apb.h>
#include <asm-generic/gpio.h>
#include <asm/arch/gpio.h>
#include <linux/kernel.h>
#include <asm/arch/efuse.h>
#include <i2c.h>

#include <odroid-common.h>

#define GPIO_LED_ALIVE		11	// GPIOAO_11

int board_led_alive(int status)
{
	int ret = gpio_request(GPIO_LED_ALIVE, "alive");
	if (ret < 0) {
		printf("Failed to request GPIO pin %u\n", GPIO_LED_ALIVE);
		return -EINVAL;
	}

	return gpio_direction_output(GPIO_LED_ALIVE, status & 0);
}

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

static unsigned int get_hw_revision(void)
{
	int hwrev = -1;
	int adc = get_adc_value(1);

#if defined(CONFIG_BANANAPI_FIREWALLA)
	if (IS_RANGE(adc, 80, 100))	/* avg : 90 */
		hwrev = BOARD_REVISION(2021, 04, 26);
#elif defined(CONFIG_BANANAPI_M5)
	if (IS_RANGE(adc, 80, 100))	/* avg : 90 */
		hwrev = BOARD_REVISION(2020, 10, 26);
	else if (IS_RANGE(adc, 240, 260))  /* avg : 250 */
		hwrev = BOARD_REVISION(2021, 01, 29);
#endif

	debug("ADC=%d, hwrev=0x%x\n", adc, hwrev);

	return hwrev;
}

#if defined(CONFIG_I2C_EEPROM_SN)
int get_i2c_eeprom_board_serial(void)
{
	char buf[32];	// sn address buffer
	int ret;

	memset(buf, 0, sizeof(buf));

	ret = i2c_read(CONFIG_I2C_EEPROM_ADDR, CONFIG_I2C_EEPROM_SN_POS, 1, (uchar*)buf, CONFIG_I2C_EEPROM_SN_LEN);
    if (ret) {
        printf("Error reading the i2c eeprom: %d\n",ret);
		memset(buf, 0, sizeof(buf));
		return -EINVAL; 
    }

	printf("BPI: (from i2c eeprom)serial: %s\n", buf);

	setenv("serial", buf);
	
	return 0;
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

	printf("BPI: (from efuse)set serial to: %s\n", buf);

	setenv("serial", buf);

	return 0;
}
#endif

int board_revision(void)
{
	if (board_rev == -1)
		board_rev = get_hw_revision();

	return board_rev;
}

void board_set_dtbfile(const char *format)
{
	char s[128];
	char *ep = getenv("variant");

	snprintf(s, sizeof(s), format, ep ? ep : "");
	setenv("fdtfile", s);
}

#if defined(CONFIG_BANANAPI_FIREWALLA)
int board_is_bananapi_firewalla(void)
{
	return (board_revision() == 0x20210426);
}
#elif defined(CONFIG_BANANAPI_M5)
int board_is_bananapi_m5(void)
{
	return (board_revision() == 0x20201026);
}

int board_is_bananapi_m2_pro(void)
{
	return (board_revision() == 0x20210129);
}
#endif
