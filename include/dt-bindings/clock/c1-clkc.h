/*
 * include/dt-bindings/clock/a1-clkc.h
 *
 * Copyright (C) 2018 Amlogic, Inc. All rights reserved.
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
 */

#ifndef __DT_BINDINGS_CLOCK_C1_H
#define __DT_BINDINGS_CLOCK_C1_H
/*
 * CLKID index values
 */
#define CLKID_XTAL		0
#define CLKID_FIXED_PLL         1
#define CLKID_FCLK_DIV2         2
#define CLKID_FCLK_DIV3         3
#define CLKID_FCLK_DIV5         4
#define CLKID_FCLK_DIV7         5
#define CLKID_FCLK_DIV2P5       6
#define CLKID_FCLK_DIV4       	7
#define CLKID_SYS_CLK		8
#define CLKID_SYS_PLL		9

#define CLKID_GATE_BASE		10
#define CLKID_SPIFC_GATE	(CLKID_GATE_BASE + 0)
#define CLKID_SYS_SPIFC		(CLKID_GATE_BASE + 1)
#define CLKID_SAR_ADC_GATE	(CLKID_GATE_BASE + 2)
#define CLKID_SPICC_A_GATE	(CLKID_GATE_BASE + 3)
#define CLKID_SPICC_B_GATE	(CLKID_GATE_BASE + 4)

#define CLKID_MUX_BASE		(CLKID_GATE_BASE + 5)
#define CLKID_SPIFC_MUX		(CLKID_MUX_BASE + 0)
#define CLKID_SARADC_MUX	(CLKID_MUX_BASE + 1)
#define CLKID_SPICC_A_MUX	(CLKID_MUX_BASE + 2)
#define CLKID_SPICC_B_MUX	(CLKID_MUX_BASE + 3)

#define CLKID_DIV_BASE		(CLKID_MUX_BASE + 4)
#define CLKID_SPIFC_DIV		(CLKID_DIV_BASE + 0)
#define CLKID_SARADC_DIV	(CLKID_DIV_BASE + 1)
#define CLKID_SPICC_A_DIV	(CLKID_DIV_BASE + 2)
#define CLKID_SPICC_B_DIV	(CLKID_DIV_BASE + 3)

/* CLKTREE_SYS_CLK_EN0 gates*/
#define GATE_BASE0		(CLKID_DIV_BASE + 4)
#define CLKID_CLKTREE		(GATE_BASE0 + 0)
#define CLKID_RESET_CTRL	(GATE_BASE0 + 1)
#define CLKID_ANALOG_CTRL	(GATE_BASE0 + 2)
#define CLKID_PWR_CTRL		(GATE_BASE0 + 3)
#define CLKID_PAD_CTRL		(GATE_BASE0 + 4)
#define CLKID_SYS_CTRL		(GATE_BASE0 + 5)
#define CLKID_TEMP_SENSOR	(GATE_BASE0 + 6)
#define CLKID_AM2AXI_DIV	(GATE_BASE0 + 7)
#define CLKID_SPICC_B		(GATE_BASE0 + 8)
#define CLKID_SPICC_A		(GATE_BASE0 + 9)
#define CLKID_CLK_MSR		(GATE_BASE0 + 10)
#define CLKID_AUDIO		(GATE_BASE0 + 11)
#define CLKID_JTAG_CTRL		(GATE_BASE0 + 12)
#define CLKID_SARADC		(GATE_BASE0 + 13)
#define CLKID_PWM_EF		(GATE_BASE0 + 14)
#define CLKID_PWM_CD		(GATE_BASE0 + 15)
#define CLKID_PWM_AB		(GATE_BASE0 + 16)
#define CLKID_CEC		(GATE_BASE0 + 17)
#define CLKID_I2C_S		(GATE_BASE0 + 18)
#define CLKID_IR_CTRL		(GATE_BASE0 + 19)
#define CLKID_I2C_M_D		(GATE_BASE0 + 20)
#define CLKID_I2C_M_C		(GATE_BASE0 + 21)
#define CLKID_I2C_M_B		(GATE_BASE0 + 22)
#define CLKID_I2C_M_A		(GATE_BASE0 + 23)
#define CLKID_ACODEC		(GATE_BASE0 + 24)
#define CLKID_OTP		(GATE_BASE0 + 25)
#define CLKID_SD_EMMC_A		(GATE_BASE0 + 26)
#define CLKID_USB_PHY		(GATE_BASE0 + 27)
#define CLKID_USB_CTRL		(GATE_BASE0 + 28)
#define CLKID_SYS_DSPB		(GATE_BASE0 + 29)
#define CLKID_SYS_DSPA		(GATE_BASE0 + 30)
#define CLKID_DMA		(GATE_BASE0 + 31)
/* CLKTREE_SYS_CLK_EN1 gates*/
#define GATE_BASE1		(GATE_BASE0 + 32)
#define CLKID_IRQ_CTRL		(GATE_BASE1 + 0)
#define CLKID_NIC		(GATE_BASE1 + 1)
#define CLKID_GIC		(GATE_BASE1 + 2)
#define CLKID_UART_C		(GATE_BASE1 + 3)
#define CLKID_UART_B		(GATE_BASE1 + 4)
#define CLKID_UART_A		(GATE_BASE1 + 5)
#define CLKID_SYS_PSRAM		(GATE_BASE1 + 6)
#define CLKID_RSA		(GATE_BASE1 + 7)
#define CLKID_CORESIGHT		(GATE_BASE1 + 8)
/* CLKTREE_AXI_CLK_EN gates */
#define GATE_BASE2		(GATE_BASE1 + 9)
#define CLKID_AM2AXI_VAD	(GATE_BASE2 + 0)
#define CLKID_AUDIO_VAD		(GATE_BASE2 + 1)
#define CLKID_AXI_DMC		(GATE_BASE2 + 2)
#define CLKID_AXI_PSRAM		(GATE_BASE2 + 3)
#define CLKID_RAMB		(GATE_BASE2 + 4)
#define CLKID_RAMA		(GATE_BASE2 + 5)
#define CLKID_AXI_SPIFC		(GATE_BASE2 + 6)
#define CLKID_AXI_NIC		(GATE_BASE2 + 7)
#define CLKID_AXI_DMA		(GATE_BASE2 + 8)
#define CLKID_CPU_CTRL		(GATE_BASE2 + 9)
#define CLKID_ROM		(GATE_BASE2 + 10)
#define CLKID_PROC_I2C		(GATE_BASE2 + 11)

#define CLKID_UNREALIZED	100

#endif /* __A1_CLKC_H */