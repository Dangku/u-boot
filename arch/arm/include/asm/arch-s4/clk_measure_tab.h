/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef _ARCH_MESON_S4_MEASURE_H_
#define _ARCH_MESON_S4_MEASURE_H_

static const char* clk_msr_table[] = {
	[0] = "cts_sys_clk",
	[1] = "cts_axi_clk",
	[2] = "cts_rtc_clk",
	[5] = "cts_mali_clk",
	[6] = "sys_cpu_clk_div16",
	[7] = "cts_ceca_clk",
	[8] = "cts_cecb_clk",
	[10] = "fclk_div5",
	[11] = "mp0_clk_out",
	[12] = "mp1_clk_out",
	[13] = "mp2_clk_out",
	[14] = "mp3_clk_out",
	[15] = "mpll_clk_50m",
	[18] = "mpll_clk_test_out",
	[19] = "hifi_pll_clk",
	[20] = "gp0_pll_clk",
	[22] = "eth_mppll_50m_ckout",
	[23] = "sys_pll_div16",
	[24] = "ddr_dpll_pt_clk",
	[30] = "mod_eth_phy_ref_clk",
	[31] = "mod_eth_tx_clk",
	[32] = "cts_eth_clk125Mhz",
	[33] = "cts_eth_clk_rmii",
	[34] = "co_clkin_to_mac",
	[35] = "mod_eth_rx_clk_rmii",
	[36] = "co_rx_clk",
	[37] = "co_tx_clk",
	[38] = "eth_phy_rxclk",
	[39] = "eth_phy_plltxclk",
	[40] = "ephy_test_clk",
	[50] = "vid_pll_div_clk_out",
	[51] = "cts_enci_clk",
	[52] = "cts_encp_clk",
	[53] = "cts_encl_clk",
	[54] = "cts_vdac_clk",
	[55] = "cts_cdac_clk_c",
	[56] = "mod_tcon_clko",
	[57] = "lcd_an_clk_ph2",
	[58] = "lcd_an_clk_ph3",
	[59] = "cts_hdmi_tx_pixel_clk",
	[60] = "cts_vdin_meas_clk",
	[61] = "cts_vpu_clk",
	[62] = "cts_vpu_clkb",
	[63] = "cts_vpu_clkb_tmp",
	[64] = "cts_vpu_clkc",
	[65] = "cts_vid_lock_clk",
	[66] = "cts_vapbclk",
	[67] = "cts_ge2d_clk",
	[68] = "cts_hdcp22_esmclk",
	[69] = "cts_hdcp22_skpclk",
	[76] = "hdmitx_tmds_clk",
	[77] = "cts_hdmitx_sys_clk",
	[78] = "cts_hdmitx_fe_clk",
	[79] = "cts_rama_clk",
	[93] = "cts_vdec_clk",
	[99] = "cts_hevcf_clk",
	[100] = "cts_demod_core_clk",
	[101] = "adc_extclk_in",
	[103] = "adc_dpll_intclk",
	[104] = "adc_dpll_clk_b3",
	[105] = "s2_adc_clk",
	[106] = "deskew_pll_clk_div32_out",
	[110] = "cts_sc_clk(smartcard)",
	[111] = "cts_sar_adc_clk",
	[113] = "cts_sd_emmc_C_clk(nand)",
	[114] = "cts_sd_emmc_B_clk",
	[115] = "cts_sd_emmc_A_clk",
	[116] = "gpio_msr_clk",
	[118] = "cts_spicc_0_clk",
	[121] = "cts_ts_clk(temp sensor)",
	[130] = "audio_vad_clk",
	[131] = "acodec_dac_clk_x128",
	[132] = "audio_locker_in_clk",
	[133] = "audio_locker_out_clk",
	[134] = "audio_tdmout_c_sclk",
	[135] = "audio_tdmout_b_sclk",
	[136] = "audio_tdmout_a_sclk",
	[137] = "audio_tdmin_lb_sclk",
	[138] = "audio_tdmin_c_sclk",
	[139] = "audio_tdmin_b_sclk",
	[140] = "audio_tdmin_a_sclk",
	[141] = "audio_resamplea_clk",
	[142] = "audio_pdm_sysclk",
	[143] = "audio_spdifout_b_mst_clk",
	[144] = "audio_spdifout_mst_clk",
	[145] = "audio_spdifin_mst_clk",
	[146] = "audio_pdm_dclk",
	[147] = "audio_resampleb_clk",
	[160] = "pwm_j_clk",
	[161] = "pwm_i_clk",
	[162] = "pwm_h_clk",
	[163] = "pwm_g_clk",
	[164] = "pwm_f_clk",
	[165] = "pwm_e_clk",
	[166] = "pwm_d_clk",
	[167] = "pwm_c_clk",
	[168] = "pwm_b_clk",
	[169] = "pwm_a_clk",
	[176] = "rng_ring_0",
	[177] = "rng_ring_1",
	[178] = "rng_ring_2",
	[179] = "rng_ring_3",
	[180] = "dmc_osc_ring(LVT16)",
	[181] = "gpu_osc_ring0(LVT16)",
	[182] = "gpu_osc_ring1(ULVT16)",
	[183] = "gpu_osc_ring2(SVT16)",
	[184] = "vpu_osc_ring0(SVT24)",
	[185] = "vpu_osc_ring1(LVT20)",
	[186] = "vpu_osc_ring2(LVT16)",
	[187] = "dos_osc_ring0(SVT24)",
	[188] = "dos_osc_ring1(SVT16)",
	[189] = "dos_osc_ring2(LVT16)",
	[190] = "dos_osc_ring3(ULVT20)",
	[192] = "axi_sram_osc_ring(SVT16)",
	[193] = "demod_osc_ring0",
	[194] = "demod_osc_ring1",
	[195] = "sar_osc_ring",
	[196] = "sys_cpu_osc_ring0",
	[197] = "sys_cpu_osc_ring1",
	[198] = "sys_cpu_osc_ring2",
	[199] = "sys_cpu_osc_ring3",
	[200] = "sys_cpu_osc_ring4",
	[201] = "sys_cpu_osc_ring5",
	[202] = "sys_cpu_osc_ring6",
	[203] = "sys_cpu_osc_ring7",
	[204] = "sys_cpu_osc_ring8",
	[205] = "sys_cpu_osc_ring9",
	[206] = "sys_cpu_osc_ring10",
	[207] = "sys_cpu_osc_ring11",
	[208] = "sys_cpu_osc_ring12",
	[209] = "sys_cpu_osc_ring13",
	[210] = "sys_cpu_osc_ring14",
	[211] = "sys_cpu_osc_ring15",
	[212] = "sys_cpu_osc_ring16",
	[213] = "sys_cpu_osc_ring17",
	[214] = "sys_cpu_osc_ring18",
	[215] = "sys_cpu_osc_ring19",
	[216] = "sys_cpu_osc_ring20",
	[217] = "sys_cpu_osc_ring21",
	[218] = "sys_cpu_osc_ring22",
	[219] = "sys_cpu_osc_ring23",
	[220] = "sys_cpu_osc_ring24",
	[221] = "sys_cpu_osc_ring25",
	[222] = "sys_cpu_osc_ring26",
	[223] = "sys_cpu_osc_ring27",
};

#endif