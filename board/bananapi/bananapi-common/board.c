#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/secure_apb.h>
#include <asm-generic/gpio.h>
#include <asm/arch/gpio.h>
#include <linux/kernel.h>

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

	if (IS_RANGE(adc, 80, 100))	/* avg : 90 */
		hwrev = BOARD_REVISION(2020, 10, 26);
	else if (IS_RANGE(adc, 240, 260))  /* avg : 250 */
		hwrev = BOARD_REVISION(2021, 01, 29);

	debug("ADC=%d, hwrev=0x%x\n", adc, hwrev);

	return hwrev;
}

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

int board_is_bananapi_m5(void)
{
	return (board_revision() == 0x20201026);
}

int board_is_bananapi_m2_pro(void)
{
	return (board_revision() == 0x20210129);
}
