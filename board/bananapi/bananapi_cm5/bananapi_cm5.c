// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <errno.h>
#include <environment.h>
#include <fdt_support.h>
#include <linux/libfdt.h>
#include <amlogic/cpu_id.h>
#include <asm/arch/secure_apb.h>
#include <asm/arch/pinctrl_init.h>
#include <linux/sizes.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#include <asm/armv8/mmu.h>
#include <linux/mtd/partitions.h>
#include <asm/arch/bl31_apis.h>
#ifdef CONFIG_AML_VPU
#include <amlogic/media/vpu/vpu.h>
#endif
#ifdef CONFIG_AML_VPP
#include <amlogic/media/vpp/vpp.h>
#endif
#ifdef CONFIG_AML_VOUT
#include <amlogic/media/vout/aml_vout.h>
#endif
#ifdef CONFIG_AML_HDMITX21
#include <amlogic/media/vout/hdmitx21/hdmitx_module.h>
#endif
#ifdef CONFIG_AML_LCD
#include <amlogic/media/vout/lcd/lcd_vout.h>
#endif
#ifdef CONFIG_RX_RTERM
#include <amlogic/aml_hdmirx.h>
#endif
#include <amlogic/storage.h>
#include <asm/arch-meson/boot.h>

DECLARE_GLOBAL_DATA_PTR;

void sys_led_init(void)
{
	/* green on, active low */
	run_command("gpio clean GPIOD_7", 0);
	/* blue off, active low*/
	run_command("gpio set GPIOH_5", 0);
}

int serial_set_pin_port(unsigned long port_base)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = (((readl(SYSCTRL_SEC_STATUS_REG4)) & ~0xffffUL) << 4);
	return 0;
}

/* secondary_boot_func
 * this function should be write with asm, here, is only for compiling pass
 */
void secondary_boot_func(void)
{
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

int active_clk(void)
{
	struct udevice *clk = NULL;
	int err;

	err = uclass_get_device_by_name(UCLASS_CLK,
			"xtal-clk", &clk);
	if (err) {
		pr_err("Can't find xtal-clk clock (%d)\n", err);
		return err;
	}
	err = uclass_get_device_by_name(UCLASS_CLK,
			"clock-controller@0", &clk);
	if (err) {
		pr_err("Can't find clock-controller@0 clock (%d)\n", err);
		return err;
	}

	return 0;
}

unsigned int get_romcode_boot_id(void)
{
	const cpu_id_t cpuid = get_cpu_id();
	const int familyId   = cpuid.family_id;

	unsigned int boot_id = 0;
#ifdef SYSCTRL_SEC_STATUS_REG2
	if (MESON_CPU_MAJOR_ID_SC2 <= familyId && MESON_CPU_MAJOR_ID_C2 != familyId) {
		boot_id = readl(SYSCTRL_SEC_STATUS_REG2);
		debug("boot_id 0x%x\n", boot_id);
		boot_id = (boot_id >> 4) & 0xf;
	}
	debug("boot_id 0x%x\n", boot_id);
#endif// #ifdef SYSCTRL_SEC_STATUS_REG2

	return boot_id;
}

const char *get_boot_source_str(unsigned int boot_id)
{
	const char *source;

	switch (boot_id) {
	case BOOT_DEVICE_EMMC:
		source = "emmc";
		break;

	case BOOT_DEVICE_NAND:
		source = "nand";
		break;

	case BOOT_DEVICE_SPI:
		source = "spi";
		break;

	case BOOT_DEVICE_SD:
		source = "sd";
		break;

	case BOOT_DEVICE_USB:
		source = "usb";
		break;

	default:
		source = "unknown";
	}

	return source;
}

static void set_boot_source(void)
{
	const char *source;

	source = get_boot_source_str(get_romcode_boot_id());

	env_set("boot_source", source);
}

static void set_fdtfile(void)
{
	//todo, detect io board and set correct dtb file here

	env_set("fdtfile", "amlogic/bananapi_cm5io.dtb");
	//env_set("fdtfile", "amlogic/bananapi_rpicm4io.dtb");
}

static void load_fdtfile(void)
{
	if (run_command("run common_dtb_load", 0) ) {
		printf("Fail in load dtb with cmd[%s]\n", env_get("common_dtb_load"));
	}
}

static void set_chipid(void)
{
	unsigned char chipid[16];

	memset(chipid, 0, 16);

	if (get_chip_id(chipid, 16) != -1) {
		char chipid_str[32];
		int i, j;
		char buf_tmp[4];

		memset(chipid_str, 0, 32);

		char *buff = &chipid_str[0];

		for (i = 0, j = 0; i < 12; ++i) {
			sprintf(&buf_tmp[0], "%02x", chipid[15 - i]);
			if (strcmp(buf_tmp, "00") != 0) {
				sprintf(buff + j, "%02x", chipid[15 - i]);
				j = j + 2;
			}
		}
		env_set("cpu_id", chipid_str);
		printf("cpu_id: %s\n", buff);
	} else {
		env_set("cpu_id", "1234567890");
	}
}

#ifdef CONFIG_AML_HDMITX21
static void hdmitx_set_hdmi_5v(void)
{
	/*Power on VCC_5V for HDMI_5V*/
	writel(readl(PADCTRL_GPIOH_OEN) & (~(1 << 1)), PADCTRL_GPIOH_OEN);
	writel(readl(PADCTRL_GPIOH_O) | (1 << 1), PADCTRL_GPIOH_O);
}
#endif

#ifdef CONFIG_AML_LCD
#define BANANAPI_CM5_HDMI     0       /*hdmi only*/
#define BANANAPI_CM5_LCD1     1       /*800x1280 dsi*/
#define BANANAPI_CM5_LCD2     2       /*1200x1920 dsi*/

void lcd_detect(void)
{
	unsigned int val;

    if (0) {
        printf("800x1280 dsi connected\n");
        val = BANANAPI_CM5_LCD1;
		env_set("panel1_type", "mipi_0");
    }

    if (0) {
        printf("1200x1920 dsi connected\n");
        val = BANANAPI_CM5_LCD2;
		env_set("panel1_type", "mipi_1");
    }

    if (1) {
        printf("lcd_detect: lcd disconnected\n");
        val = BANANAPI_CM5_HDMI;
    }

	env_set_hex("lcd_exist", val);
}

#endif

void board_init_mem(void)
{
	/* config bootm low size, make sure whole dram/psram space can be used */
	phys_size_t ram_size;
	char *env_tmp;

	env_tmp = env_get("bootm_size");
	if (!env_tmp) {
		ram_size = ((0x100000000 <= ((readl(SYSCTRL_SEC_STATUS_REG4) &
				0xFFFFFFFF0000) << 4)) ? 0xe0000000 :
					(((readl(SYSCTRL_SEC_STATUS_REG4)) &
					0xFFFFFFFF0000) << 4));
		env_set_hex("bootm_low", 0);
		env_set_hex("bootm_size", ram_size);
	}
}

int board_init(void)
{
	printf("board init\n");

	/* The non-secure watchdog is enabled in BL2 TEE, disable it */
	run_command("watchdog off", 0);
	printf("watchdog disable\n");

	pinctrl_devices_active(PIN_CONTROLLER_NUM);

	sys_led_init();

	return 0;
}

int board_late_init(void)
{
	printf("board late init\n");

	board_init_mem();
	set_chipid();
	set_boot_source();
	set_fdtfile();
    load_fdtfile();

#ifdef CONFIG_AML_VPU
	vpu_probe();
#endif
#ifdef CONFIG_AML_HDMITX21
	hdmitx_set_hdmi_5v();
	hdmitx21_init();
#endif
#ifdef CONFIG_AML_VPP
	vpp_init();
#endif
#ifdef CONFIG_RX_RTERM
	rx_set_phy_rterm();
#endif
#ifdef CONFIG_AML_VOUT
	vout_probe();
#endif
#ifdef CONFIG_AML_LCD
	lcd_detect();
	lcd_probe();
	/* cm4io backlight on because pin 41 (GPIOY_5) not support pwm */
	run_command("gpio set GPIOY_5", 0);
#endif

	return 0;
}

phys_size_t get_effective_memsize(void)
{
	phys_size_t ddr_size;

	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	ddr_size = (readl(SYSCTRL_SEC_STATUS_REG4) & ~0xffffUL) << 4;
	if (ddr_size >= 0xe0000000)
		ddr_size = 0xe0000000;
	return (ddr_size - CONFIG_SYS_MEM_TOP_HIDE);
#else
	ddr_size = (readl(SYSCTRL_SEC_STATUS_REG4) & ~0xffffUL) << 4;
	if (ddr_size >= 0xe0000000)
		ddr_size = 0xe0000000;
	return ddr_size
#endif /* CONFIG_SYS_MEM_TOP_HIDE */
}

phys_size_t get_ddr_info_size(void)
{
	phys_size_t ddr_size = (((readl(SYSCTRL_SEC_STATUS_REG4)) & ~0xffffUL) << 4);

	return ddr_size;
}

ulong board_get_usable_ram_top(ulong total_size)
{
	unsigned long top = gd->ram_top;

	if (top >= 0xE0000000UL)
		return 0xE0000000UL;

	return top;
}

static struct mm_region bd_mem_map[] = {
	{
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0xe0000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0xe0000000UL,
		.phys = 0xe0000000UL,
		.size = 0x20000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = bd_mem_map;

int mach_cpu_init(void)
{
	//printf("\nmach_cpu_init\n");
#ifdef CONFIG_UPDATE_MMU_TABLE
	unsigned long nddrSize = ((0x100000000 <= ((readl(SYSCTRL_SEC_STATUS_REG4) &
				0xFFFFFFFF0000) << 4)) ? 0xe0000000 :
				(((readl(SYSCTRL_SEC_STATUS_REG4)) &
				0xFFFFFFFF0000) << 4));

	if (nddrSize <= 0xe0000000)
		bd_mem_map[0].size = nddrSize;

#endif
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	/* eg: bl31/32 rsv */
	return 0;
}

