
/*
 * board/bananapi/bananapi_cm4/bananapi_cm4.c
 *
 * Copyright (C) 2019 Khadas, Inc. All rights reserved.
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
#include <asm/cpu_id.h>
#include <asm/arch/secure_apb.h>
#ifdef CONFIG_SYS_I2C_AML
#include <aml_i2c.h>
#endif
#ifdef CONFIG_SYS_I2C_MESON
#include <amlogic/i2c.h>
#endif
#ifdef CONFIG_PWM_MESON
#include <pwm.h>
#include <amlogic/pwm.h>
#endif
#ifdef CONFIG_AML_VPU
#include <vpu.h>
#endif
#include <vpp.h>
#ifdef CONFIG_AML_HDMITX20
#include <amlogic/hdmi.h>
#endif
#ifdef CONFIG_AML_LCD
#include <amlogic/aml_lcd.h>
#endif
#include <asm/arch/eth_setup.h>
#include <phy.h>
#include <linux/mtd/partitions.h>
#include <linux/sizes.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#ifdef CONFIG_AML_SPIFC
#include <amlogic/spifc.h>
#endif
#ifdef CONFIG_AML_SPICC
#include <amlogic/spicc.h>
#endif
#include <asm/arch/timer.h>
#include <bananapi-common.h>
#include <rpi_mcu.h>

DECLARE_GLOBAL_DATA_PTR;

//new static eth setup
struct eth_board_socket*  eth_board_skt;


int serial_set_pin_port(unsigned long port_base)
{
    //UART in "Always On Module"
    //GPIOAO_0==tx,GPIOAO_1==rx
    //setbits_le32(P_AO_RTI_PIN_MUX_REG,3<<11);
    return 0;
}

int dram_init(void)
{
	gd->ram_size = (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4);
	return 0;
}

int enableCameraVcc(void)
{
    /* set gpioa_13 high to enable lcd vcc*/
    writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 13)), PREG_PAD_GPIO5_EN_N);
    writel(readl(PREG_PAD_GPIO5_O) | (1 << 13), PREG_PAD_GPIO5_O);
    writel(readl(PERIPHS_PIN_MUX_E) & (~(0xf << 20)), PERIPHS_PIN_MUX_E);
    return 0;
}

/* secondary_boot_func
 * this function should be write with asm, here, is is only for compiling pass
 * */
void secondary_boot_func(void)
{
}
#ifdef  ETHERNET_INTERNAL_PHY
void internalPhyConfig(struct phy_device *phydev)
{
}

static int dwmac_meson_cfg_pll(void)
{
	writel(0x39C0040A, P_ETH_PLL_CTL0);
	writel(0x927E0000, P_ETH_PLL_CTL1);
	writel(0xAC5F49E5, P_ETH_PLL_CTL2);
	writel(0x00000000, P_ETH_PLL_CTL3);
	udelay(200);
	writel(0x19C0040A, P_ETH_PLL_CTL0);
	return 0;
}

static int dwmac_meson_cfg_analog(void)
{
	/*Analog*/
	writel(0x20200000, P_ETH_PLL_CTL5);
	writel(0x0000c002, P_ETH_PLL_CTL6);
	writel(0x00000023, P_ETH_PLL_CTL7);

	return 0;
}

static int dwmac_meson_cfg_ctrl(void)
{
	/*config phyid should between  a 0~0xffffffff*/
	/*please don't use 44000181, this has been used by internal phy*/
	writel(0x33000180, P_ETH_PHY_CNTL0);

	/*use_phy_smi | use_phy_ip | co_clkin from eth_phy_top*/
	writel(0x260, P_ETH_PHY_CNTL2);

	writel(0x74043, P_ETH_PHY_CNTL1);
	writel(0x34043, P_ETH_PHY_CNTL1);
	writel(0x74043, P_ETH_PHY_CNTL1);
	return 0;
}

static void setup_net_chip(void)
{
	eth_aml_reg0_t eth_reg0;

	eth_reg0.d32 = 0;
	eth_reg0.b.phy_intf_sel = 4;
	eth_reg0.b.rx_clk_rmii_invert = 0;
	eth_reg0.b.rgmii_tx_clk_src = 0;
	eth_reg0.b.rgmii_tx_clk_phase = 0;
	eth_reg0.b.rgmii_tx_clk_ratio = 4;
	eth_reg0.b.phy_ref_clk_enable = 1;
	eth_reg0.b.clk_rmii_i_invert = 1;
	eth_reg0.b.clk_en = 1;
	eth_reg0.b.adj_enable = 1;
	eth_reg0.b.adj_setup = 0;
	eth_reg0.b.adj_delay = 9;
	eth_reg0.b.adj_skew = 0;
	eth_reg0.b.cali_start = 0;
	eth_reg0.b.cali_rise = 0;
	eth_reg0.b.cali_sel = 0;
	eth_reg0.b.rgmii_rx_reuse = 0;
	eth_reg0.b.eth_urgent = 0;
	setbits_le32(P_PREG_ETH_REG0, eth_reg0.d32);// rmii mode

	dwmac_meson_cfg_pll();
	dwmac_meson_cfg_analog();
	dwmac_meson_cfg_ctrl();

	/* eth core clock */
	setbits_le32(HHI_GCLK_MPEG1, (0x1 << 3));
	/* eth phy clock */
	setbits_le32(HHI_GCLK_MPEG0, (0x1 << 4));

	/* eth phy pll, clk50m */
	setbits_le32(HHI_FIX_PLL_CNTL3, (0x1 << 5));

	/* power on memory */
	clrbits_le32(HHI_MEM_PD_REG0, (1 << 3) | (1<<2));
}
#endif

#ifdef ETHERNET_EXTERNAL_PHY

static int dwmac_meson_cfg_drive_strength(void)
{
	writel(0xaaaaaaa5, P_PAD_DS_REG4A);
	return 0;
}

static void setup_net_chip_ext(void)
{
	eth_aml_reg0_t eth_reg0;
	writel(0x11111111, P_PERIPHS_PIN_MUX_6);
	writel(0x111111, P_PERIPHS_PIN_MUX_7);

	eth_reg0.d32 = 0;
	eth_reg0.b.phy_intf_sel = 1;
	eth_reg0.b.rx_clk_rmii_invert = 0;
	eth_reg0.b.rgmii_tx_clk_src = 0;
	eth_reg0.b.rgmii_tx_clk_phase = 1;
	eth_reg0.b.rgmii_tx_clk_ratio = 4;
	eth_reg0.b.phy_ref_clk_enable = 1;
	eth_reg0.b.clk_rmii_i_invert = 0;
	eth_reg0.b.clk_en = 1;
	eth_reg0.b.adj_enable = 0;
	eth_reg0.b.adj_setup = 0;
	eth_reg0.b.adj_delay = 0;
	eth_reg0.b.adj_skew = 0;
	eth_reg0.b.cali_start = 0;
	eth_reg0.b.cali_rise = 0;
	eth_reg0.b.cali_sel = 0;
	eth_reg0.b.rgmii_rx_reuse = 0;
	eth_reg0.b.eth_urgent = 0;
	setbits_le32(P_PREG_ETH_REG0, eth_reg0.d32);// rmii mode

	setbits_le32(HHI_GCLK_MPEG1, 0x1 << 3);
	/* power on memory */
	clrbits_le32(HHI_MEM_PD_REG0, (1 << 3) | (1<<2));
}
#endif
extern struct eth_board_socket* eth_board_setup(char *name);
extern int designware_initialize(ulong base_addr, u32 interface);

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_ETHERNET_NONE
	return 0;
#endif

#ifdef ETHERNET_EXTERNAL_PHY
	dwmac_meson_cfg_drive_strength();
	setup_net_chip_ext();
#endif
#ifdef ETHERNET_INTERNAL_PHY
	setup_net_chip();
#endif
	udelay(1000);
	designware_initialize(ETH_BASE, PHY_INTERFACE_MODE_RMII);
	return 0;
}

#if CONFIG_AML_SD_EMMC
#include <mmc.h>
#include <asm/arch/sd_emmc.h>
static int  sd_emmc_init(unsigned port)
{
    switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//todo add card detect
			/* check card detect */
			clrbits_le32(P_PERIPHS_PIN_MUX_9, 0xF << 24);
			setbits_le32(P_PREG_PAD_GPIO1_EN_N, 1 << 6);
			setbits_le32(P_PAD_PULL_UP_EN_REG1, 1 << 6);
			setbits_le32(P_PAD_PULL_UP_REG1, 1 << 6);
			break;
		case SDIO_PORT_C:
			//enable pull up
			//clrbits_le32(P_PAD_PULL_UP_REG3, 0xff<<0);
			break;
		default:
			break;
	}

	return cpu_sd_emmc_init(port);
}

extern unsigned sd_debug_board_1bit_flag;


static void sd_emmc_pwr_prepare(unsigned port)
{
	cpu_sd_emmc_pwr_prepare(port);
}

static void sd_emmc_pwr_on(unsigned port)
{
	switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			break;
		case SDIO_PORT_C:
			break;
		default:
			break;
	}
	return;
}
static void sd_emmc_pwr_off(unsigned port)
{
	/// @todo NOT FINISH
	switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//            setbits_le32(P_PREG_PAD_GPIO5_O,(1<<31)); //CARD_8
			//            clrbits_le32(P_PREG_PAD_GPIO5_EN_N,(1<<31));
			break;
		case SDIO_PORT_C:
			break;
		default:
			break;
	}
	return;
}

// #define CONFIG_TSD      1
static void board_mmc_register(unsigned port)
{
	struct aml_card_sd_info *aml_priv=cpu_sd_emmc_get(port);
	if (aml_priv == NULL)
		return;

	aml_priv->sd_emmc_init=sd_emmc_init;
	aml_priv->sd_emmc_detect=sd_emmc_detect;
	aml_priv->sd_emmc_pwr_off=sd_emmc_pwr_off;
	aml_priv->sd_emmc_pwr_on=sd_emmc_pwr_on;
	aml_priv->sd_emmc_pwr_prepare=sd_emmc_pwr_prepare;
	aml_priv->desc_buf = malloc(NEWSD_MAX_DESC_MUN*(sizeof(struct sd_emmc_desc_info)));

	if (NULL == aml_priv->desc_buf)
		printf(" desc_buf Dma alloc Fail!\n");
	else
		printf("aml_priv->desc_buf = 0x%p\n",aml_priv->desc_buf);

	sd_emmc_register(aml_priv);
}

static void board_mmc_power_enable(void)
{
        printf("BPI-sd: set sd power on\n");

	/* set gpioAO_3 output/high to set sd_power enable */
	writel(readl(AO_GPIO_O) | (1 << 3), AO_GPIO_O);
	writel(readl(AO_GPIO_O_EN_N) | (1 << 3), AO_GPIO_O_EN_N);
	writel(readl(AO_RTI_PINMUX_REG0) & (~(0xf << 12)), AO_RTI_PINMUX_REG0);

	/* set gpioAO_8 output/high to set sd_power enable */
        writel(readl(AO_GPIO_O) | (1 << 8), AO_GPIO_O);
        writel(readl(AO_GPIO_O_EN_N) | (1 << 8), AO_GPIO_O_EN_N);
        writel(readl(AO_RTI_PINMUX_REG1) & (~(0xf << 0)), AO_RTI_PINMUX_REG1);

        /* set gpioAO_9 output/low to set sd 3v3_1v8_en to 3v3 default */
        writel(readl(AO_GPIO_O) & (~(1 << 9)), AO_GPIO_O);
        writel(readl(AO_GPIO_O_EN_N) & (~(1 << 9)), AO_GPIO_O_EN_N);
        writel(readl(AO_RTI_PINMUX_REG1) & (~(0xf << 4)), AO_RTI_PINMUX_REG1);
}

int board_mmc_init(bd_t	*bis)
{
	board_mmc_power_enable();
	board_mmc_register(SDIO_PORT_B);  //sd
	board_mmc_register(SDIO_PORT_C);  //emmc
	return 0;
}
#endif

#ifdef CONFIG_SYS_I2C_AML

#ifdef CONFIG_SYS_I2C_AML_I2C3
static void board_i2c_set_pinmux(void)
{
	/* i2c_3 GPIO_A14/A15 */
	clrbits_le32(PERIPHS_PIN_MUX_E, 0xf << 24 | 0xf << 28);
	setbits_le32(PERIPHS_PIN_MUX_E, MESON_I2C_MASTER_D_GPIOA_14_BIT | MESON_I2C_MASTER_D_GPIOA_15_BIT);
}

struct aml_i2c_platform g_aml_i2c_plat = {
	.wait_count         = 1000000,
	.wait_ack_interval  = 5,
	.wait_read_interval = 5,
	.wait_xfer_interval = 5,
	.master_no          = AML_I2C_MASTER_D,
	.use_pio            = 0,
	.master_i2c_speed   = AML_I2C_SPPED_400K,
	.master_d_pinmux = {
		.scl_reg    = (unsigned long)MESON_I2C_MASTER_D_GPIOA_15_REG,
		.scl_bit    = MESON_I2C_MASTER_D_GPIOA_15_BIT,
		.sda_reg    = (unsigned long)MESON_I2C_MASTER_D_GPIOA_14_REG,
		.sda_bit    = MESON_I2C_MASTER_D_GPIOA_14_BIT,
	}
};
#endif

#ifdef CONFIG_SYS_I2C_AML_I2C2
static void board_i2c_set_pinmux(void)
{	
	/* i2c_2 GPIO_X17/X18  */
	clrbits_le32(PERIPHS_PIN_MUX_5, 0xf << 4 | 0xf << 8);
	setbits_le32(PERIPHS_PIN_MUX_5, MESON_I2C_MASTER_C_GPIOX_17_BIT | MESON_I2C_MASTER_C_GPIOX_18_BIT);
}

struct aml_i2c_platform g_aml_i2c_plat = {
	.wait_count         = 1000000,
	.wait_ack_interval  = 5,
	.wait_read_interval = 5,
	.wait_xfer_interval = 5,
	.master_no          = AML_I2C_MASTER_C,
	.use_pio            = 0,
	.master_i2c_speed   = AML_I2C_SPPED_400K,
	.master_c_pinmux = {
		.scl_reg    = (unsigned long)MESON_I2C_MASTER_C_GPIOX_18_REG,
		.scl_bit    = MESON_I2C_MASTER_C_GPIOX_18_BIT,
		.sda_reg    = (unsigned long)MESON_I2C_MASTER_C_GPIOX_17_REG,
		.sda_bit    = MESON_I2C_MASTER_C_GPIOX_17_BIT,
	}
};
#endif

#ifdef CONFIG_SYS_I2C_AML_I2C1
static void board_i2c_set_pinmux(void)
{
	/* i2c_1 GPIO_H6/H7  */
	clrbits_le32(PERIPHS_PIN_MUX_B, 0xf << 24 | 0xf << 28);
	setbits_le32(PERIPHS_PIN_MUX_B, MESON_I2C_MASTER_B_GPIOH_6_BIT | MESON_I2C_MASTER_B_GPIOH_7_BIT);
}

struct aml_i2c_platform g_aml_i2c_plat = {
	.wait_count         = 1000000,
	.wait_ack_interval  = 5,
	.wait_read_interval = 5,
	.wait_xfer_interval = 5,
	.master_no          = AML_I2C_MASTER_B,
	.use_pio            = 0,
	.master_i2c_speed   = AML_I2C_SPPED_400K,
	.master_b_pinmux = {
		.scl_reg    = (unsigned long)MESON_I2C_MASTER_B_GPIOH_7_REG,
		.scl_bit    = MESON_I2C_MASTER_B_GPIOH_7_BIT,
		.sda_reg    = (unsigned long)MESON_I2C_MASTER_B_GPIOH_6_REG,
		.sda_bit    = MESON_I2C_MASTER_B_GPIOH_6_BIT,
	}
};
#endif

static void board_i2c_init(void)
{
#if defined(CONFIG_SYS_I2C_AML_I2C2) || defined(CONFIG_SYS_I2C_AML_I2C3) || defined(CONFIG_SYS_I2C_AML_I2C1)
	board_i2c_set_pinmux();
	i2c_plat_init();
#endif
	aml_i2c_init();

	udelay(10);
}
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void){
	/*add board early init function here*/
	return 0;
}
#endif

#ifdef CONFIG_USB_XHCI_AMLOGIC_V2
#include <asm/arch/usb-v2.h>
#include <asm/arch/gpio.h>
#define CONFIG_GXL_USB_U2_PORT_NUM	2

#ifdef CONFIG_USB_XHCI_AMLOGIC_USB3_V2
#define CONFIG_GXL_USB_U3_PORT_NUM	1
#else
#define CONFIG_GXL_USB_U3_PORT_NUM	0
#endif

static void gpio_set_vbus_power(char is_power_on)
{
	if (is_power_on) {
		/* set gpioH_8 output/external pull high to power on vbus */
		writel(readl(PREG_PAD_GPIO3_EN_N) | (1 << 8), PREG_PAD_GPIO3_EN_N);
		writel(readl(PERIPHS_PIN_MUX_C) & (~(0xf)), PERIPHS_PIN_MUX_C);
	} else {
		/* set gpioH_8 low to power off vbus */
		writel(readl(PREG_PAD_GPIO3_EN_N) & (~(1 << 8)), PREG_PAD_GPIO3_EN_N);
		writel(readl(PERIPHS_PIN_MUX_C) & (~(0xf)), PERIPHS_PIN_MUX_C);
	}
}

struct amlogic_usb_config g_usb_config_GXL_skt={
	CONFIG_GXL_XHCI_BASE,
	USB_ID_MODE_HARDWARE,
	gpio_set_vbus_power,//gpio_set_vbus_power, //set_vbus_power
	CONFIG_GXL_USB_PHY2_BASE,
	CONFIG_GXL_USB_PHY3_BASE,
	CONFIG_GXL_USB_U2_PORT_NUM,
	CONFIG_GXL_USB_U3_PORT_NUM,
	.usb_phy2_pll_base_addr = {
		CONFIG_USB_PHY_20,
		CONFIG_USB_PHY_21,
	}
};

#endif /*CONFIG_USB_XHCI_AMLOGIC*/

#ifdef CONFIG_AML_HDMITX20
static void hdmi_tx_set_hdmi_5v(void)
{
}
#endif

#ifdef CONFIG_AML_SPIFC
/*
 * BOOT_3: NOR_HOLDn:reg0[15:12]=3
 * BOOT_4: NOR_D:reg0[19:16]=3
 * BOOT_5: NOR_Q:reg0[23:20]=3
 * BOOT_6: NOR_C:reg0[27:24]=3
 * BOOT_7: NOR_WPn:reg0[31:28]=3
 * BOOT_14: NOR_CS:reg1[27:24]=3
 */
#define SPIFC_NUM_CS 1
static int spifc_cs_gpios[SPIFC_NUM_CS] = {54};

static int spifc_pinctrl_enable(void *pinctrl, bool enable)
{
	unsigned int val;

	val = readl(P_PERIPHS_PIN_MUX_0);
	val &= ~(0xfffff << 12);
	if (enable)
		val |= 0x33333 << 12;
	writel(val, P_PERIPHS_PIN_MUX_0);

	val = readl(P_PERIPHS_PIN_MUX_1);
	val &= ~(0xf << 24);
	writel(val, P_PERIPHS_PIN_MUX_1);
	return 0;
}

static const struct spifc_platdata spifc_platdata = {
	.reg = 0xffd14000,
	.mem_map = 0xf6000000,
	.pinctrl_enable = spifc_pinctrl_enable,
	.num_chipselect = SPIFC_NUM_CS,
	.cs_gpios = spifc_cs_gpios,
};

U_BOOT_DEVICE(spifc) = {
	.name = "spifc",
	.platdata = &spifc_platdata,
};
#endif /* CONFIG_AML_SPIFC */

#ifdef CONFIG_AML_SPICC
/* generic config in arch gpio/clock.c */
extern int spicc0_clk_set_rate(int rate);
extern int spicc0_clk_enable(bool enable);
extern int spicc0_pinctrl_enable(bool enable);

static const struct spicc_platdata spicc0_platdata = {
	.compatible = "amlogic,meson-g12a-spicc",
	.reg = (void __iomem *)0xffd13000,
	.clk_rate = 666666666,
	.clk_set_rate = spicc0_clk_set_rate,
	.clk_enable = spicc0_clk_enable,
	.pinctrl_enable = spicc0_pinctrl_enable,
	/* case one slave without cs: {"no_cs", 0} */
	.cs_gpio_names = {"GPIOX_10", 0},
};

U_BOOT_DEVICE(spicc0) = {
	.name = "spicc",
	.platdata = &spicc0_platdata,
};
#endif /* CONFIG_AML_SPICC */

#ifdef CONFIG_SYS_I2C_MESON
static const struct meson_i2c_platdata i2c_data[] = {
	{ 0, 0xffd1f000, 166666666, 3, 15, 100000 },
	{ 1, 0xffd1e000, 166666666, 3, 15, 100000 },
	{ 2, 0xffd1d000, 166666666, 3, 15, 100000 },
	{ 3, 0xffd1c000, 166666666, 3, 15, 100000 },
	{ 4, 0xff805000, 166666666, 3, 15, 100000 },
};

U_BOOT_DEVICES(meson_i2cs) = {
	{ "i2c_meson", &i2c_data[0] },
	{ "i2c_meson", &i2c_data[1] },
	{ "i2c_meson", &i2c_data[2] },
	{ "i2c_meson", &i2c_data[3] },
	{ "i2c_meson", &i2c_data[4] },
};

/*
 *GPIOH_6 I2C_SDA_M1
 *GPIOH_7 I2C_SCK_M1
 *pinmux configuration seperated with i2c controller configuration
 * config it when you use
 */
void set_i2c_m1_pinmux(void)
{
	/*ds =3 */
	clrbits_le32(PAD_DS_REG3A, 0xf << 12);
	setbits_le32(PAD_DS_REG3A, 0x3 << 12 | 0x3 << 14);
	/*pull up en*/
	clrbits_le32(PAD_PULL_UP_EN_REG3, 0x3 << 6);
	setbits_le32(PAD_PULL_UP_EN_REG3, 0x3 << 6 );
	/*pull up*/
	clrbits_le32(PAD_PULL_UP_REG3, 0x3 << 6);
	setbits_le32(PAD_PULL_UP_REG3, 0x3 << 6 );
	/*pin mux to i2cm1*/
	clrbits_le32(PERIPHS_PIN_MUX_B, 0xff << 24);
	setbits_le32(PERIPHS_PIN_MUX_B, 0x4 << 24 | 0x4 << 28);

	return;
}

#endif /*end CONFIG_SYS_I2C_MESON*/

#ifdef CONFIG_PWM_MESON
static const struct meson_pwm_platdata pwm_data[] = {
	{ PWM_AB, 0xffd1b000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWM_CD, 0xffd1a000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWM_EF, 0xffd19000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWMAO_AB, 0xff807000, IS_DOUBLE_CHANNEL, IS_BLINK },
	{ PWMAO_CD, 0xff802000, IS_DOUBLE_CHANNEL, IS_BLINK },
};

U_BOOT_DEVICES(meson_pwm) = {
	{ "amlogic,general-pwm", &pwm_data[0] },
	{ "amlogic,general-pwm", &pwm_data[1] },
	{ "amlogic,general-pwm", &pwm_data[2] },
	{ "amlogic,general-pwm", &pwm_data[3] },
	{ "amlogic,general-pwm", &pwm_data[4] },
};
#endif /*end CONFIG_PWM_MESON*/

#if defined(CONFIG_AML_LCD)
// detect whether the LCD is exist
void board_lcd_detect(void)
{
	unsigned int val;

	/* set gpioa_5 input(lcd_rst)*/
	writel(readl(PREG_PAD_GPIO5_EN_N) | (1 << 5), PREG_PAD_GPIO5_EN_N);
	writel(readl(PAD_PULL_UP_EN_REG5) | (1 << 5), PREG_PAD_GPIO5_EN_N);
	writel(readl(PAD_PULL_UP_REG5) | (1 << 5), PREG_PAD_GPIO5_EN_N);
	writel(readl(PERIPHS_PIN_MUX_E) | (0xf << 4), PERIPHS_PIN_MUX_E);

	/* set gpioa_9 output high to enable lcd power */
	writel(readl(PREG_PAD_GPIO5_EN_N) & (~(1 << 9)), PREG_PAD_GPIO5_EN_N);
	writel(readl(PREG_PAD_GPIO5_O) | (1 << 9), PREG_PAD_GPIO5_O);
	writel(readl(PERIPHS_PIN_MUX_E) & (~(0xf << 4)), PERIPHS_PIN_MUX_E);

	mdelay(100);

	val = readl(PREG_PAD_GPIO5_I);
	//printf("lcd_detect: val=%x\n", val);
	val &= ~0xffdf;
	val = val >> 5;
	if (val)
		printf("lcd_detect: lcd connected\n");
	else
		printf("lcd_detect: lcd disconnected\n");

	setenv_ulong("lcd_exist", val);
}
#endif /* CONFIG_AML_LCD */

void board_set_dtb(void)
{
	if (board_is_bananapi_cm4()) {
		printf("BPI: board is Bananapi CM4\n");
		setenv("board", "bananapi_cm4");

		if (board_is_bpi_cm4io()) {
			printf("BPI: io board is bpi cm4io\n");
			setenv("fdtfile", "bananapi_cm4io.dtb");
		} else if(board_is_rpi_cm4io()) {
			printf("BPI: io board is rpi cm4io\n");
			setenv("fdtfile", "raspberrypi_cm4io.dtb");
		}
	}
}

int board_get_panel_type(void)
{
	char cmd[64];
	char *addr_r;
	unsigned long filesize;
	int mmc_dev = board_current_mmc();

	addr_r = getenv("loadaddr");
	setenv("filesize", "0");

	sprintf(cmd, "fatload mmc %d:1 %s lcd_env.txt", mmc_dev, addr_r);
	run_command(cmd, 0);

	filesize = getenv_ulong("filesize", 16, 0);
	if (filesize > SZ_64K) {
		printf("lcd_env.txt exceeds %d, size=%ld\n", SZ_64K, filesize);
		return 0;
	} else {
		sprintf(cmd, "env import -t %s %lu", addr_r, filesize);
		run_command(cmd, 0);
	}

	return 0;
}

int board_init(void)
{
#ifdef CONFIG_USB_XHCI_AMLOGIC_V2
	board_usb_pll_disable(&g_usb_config_GXL_skt);
	board_usb_init(&g_usb_config_GXL_skt,BOARD_USB_MODE_HOST);
#endif /*CONFIG_USB_XHCI_AMLOGIC*/

#ifdef CONFIG_SYS_I2C_MESON
	set_i2c_m1_pinmux();
#endif
#ifdef CONFIG_SYS_I2C_AML
	board_i2c_init();
#endif

	//rpi_mcu_init();

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifdef CONFIG_AML_VPU
	vpu_probe();
#endif
	vpp_init();
#ifdef CONFIG_AML_HDMITX20
	hdmi_tx_set_hdmi_5v();
	hdmi_tx_init();
#endif
#ifdef CONFIG_AML_CVBS
	run_command("cvbs init", 0);
#endif
#ifdef CONFIG_AML_LCD
	board_lcd_detect();
	board_get_panel_type();
	lcd_probe();
#endif

    //enable camera power
	enableCameraVcc();

	get_board_serial();
	board_set_dtb();

	return 0;
}
#endif


phys_size_t get_effective_memsize(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4);
#endif
}