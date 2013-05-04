/*
 * Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <mach/rpm-regulator.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_bus.h>

#include "acpuclock.h"
#include "acpuclock-krait.h"

static struct hfpll_data hfpll_data __initdata = {
	.mode_offset = 0x00,
	.l_offset = 0x08,
	.m_offset = 0x0C,
	.n_offset = 0x10,
	.config_offset = 0x04,
	.config_val = 0x7845C665,
	.has_droop_ctl = true,
	.droop_offset = 0x14,
	.droop_val = 0x0108C000,
	.low_vdd_l_max = 22,
	.nom_vdd_l_max = 42,
	.vdd[HFPLL_VDD_NONE] =       0,
	.vdd[HFPLL_VDD_LOW]  =  945000,
	.vdd[HFPLL_VDD_NOM]  = 1050000,
	.vdd[HFPLL_VDD_HIGH] = 1150000,
};

static struct scalable scalable[] __initdata = {
	[CPU0] = {
		.hfpll_phys_base = 0x00903200,
		.aux_clk_sel_phys = 0x02088014,
		.aux_clk_sel = 3,
		.sec_clk_sel = 2,
		.l2cpmr_iaddr = 0x4501,
		.vreg[VREG_CORE] = { "krait0", 1300000 },
		.vreg[VREG_MEM]  = { "krait0_mem", 1150000 },
		.vreg[VREG_DIG]  = { "krait0_dig", 1150000 },
		.vreg[VREG_HFPLL_A] = { "krait0_s8", 2050000 },
		.vreg[VREG_HFPLL_B] = { "krait0_l23", 1800000 },
	},
	[CPU1] = {
		.hfpll_phys_base = 0x00903300,
		.aux_clk_sel_phys = 0x02098014,
		.aux_clk_sel = 3,
		.sec_clk_sel = 2,
		.l2cpmr_iaddr = 0x5501,
		.vreg[VREG_CORE] = { "krait1", 1300000 },
		.vreg[VREG_MEM]  = { "krait1_mem", 1150000 },
		.vreg[VREG_DIG]  = { "krait1_dig", 1150000 },
		.vreg[VREG_HFPLL_A] = { "krait1_s8", 2050000 },
		.vreg[VREG_HFPLL_B] = { "krait1_l23", 1800000 },
	},
	[L2] = {
		.hfpll_phys_base = 0x00903400,
		.aux_clk_sel_phys = 0x02011028,
		.aux_clk_sel = 3,
		.sec_clk_sel = 2,
		.l2cpmr_iaddr = 0x0500,
		.vreg[VREG_HFPLL_A] = { "l2_s8", 2050000 },
		.vreg[VREG_HFPLL_B] = { "l2_l23", 1800000 },
	},
};

static struct msm_bus_paths bw_level_tbl[] __initdata = {
	[0] =  BW_MBPS(640), /* At least  80 MHz on bus. */
	[1] = BW_MBPS(1064), /* At least 133 MHz on bus. */
	[2] = BW_MBPS(1600), /* At least 200 MHz on bus. */
	[3] = BW_MBPS(2128), /* At least 266 MHz on bus. */
	[4] = BW_MBPS(3200), /* At least 400 MHz on bus. */
	[5] = BW_MBPS(3600), /* At least 450 MHz on bus. */
	[6] = BW_MBPS(3936), /* At least 492 MHz on bus. */
};

static struct msm_bus_scale_pdata bus_scale_data __initdata = {
	.usecase = bw_level_tbl,
	.num_usecases = ARRAY_SIZE(bw_level_tbl),
	.active_only = 1,
	.name = "acpuclk-8960",
};

static struct l2_level l2_freq_tbl[] __initdata = {
	[0]  = { {  384000, PLL_8, 0, 0x00 }, 1050000, 1050000, 1 },
	[1]  = { {  432000, HFPLL, 2, 0x20 }, 1050000, 1050000, 2 },
	[2]  = { {  486000, HFPLL, 2, 0x24 }, 1050000, 1050000, 2 },
	[3]  = { {  540000, HFPLL, 2, 0x28 }, 1050000, 1050000, 2 },
	[4]  = { {  594000, HFPLL, 1, 0x16 }, 1050000, 1050000, 2 },
	[5]  = { {  648000, HFPLL, 1, 0x18 }, 1050000, 1050000, 4 },
	[6]  = { {  702000, HFPLL, 1, 0x1A }, 1050000, 1050000, 4 },
	[7]  = { {  756000, HFPLL, 1, 0x1C }, 1150000, 1150000, 4 },
	[8]  = { {  810000, HFPLL, 1, 0x1E }, 1150000, 1150000, 4 },
	[9]  = { {  864000, HFPLL, 1, 0x20 }, 1150000, 1150000, 4 },
	[10] = { {  918000, HFPLL, 1, 0x22 }, 1150000, 1150000, 6 },
	[11] = { {  972000, HFPLL, 1, 0x24 }, 1150000, 1150000, 6 },
	[12] = { { 1026000, HFPLL, 1, 0x26 }, 1150000, 1150000, 6 },
	[13] = { { 1080000, HFPLL, 1, 0x28 }, 1150000, 1150000, 6 },
	[14] = { { 1134000, HFPLL, 1, 0x2A }, 1150000, 1150000, 6 },
	[15] = { { 1188000, HFPLL, 1, 0x2C }, 1150000, 1150000, 6 },
	[16] = { { 1242000, HFPLL, 1, 0x2E }, 1150000, 1150000, 6 },
	[17] = { { 1296000, HFPLL, 1, 0x30 }, 1150000, 1150000, 6 },
	[18] = { { 1350000, HFPLL, 1, 0x32 }, 1150000, 1150000, 6 },
	{ }
};

#define AVS(x) .avsdscr_setting = (x)

static struct acpu_level acpu_freq_tbl_slow[] __initdata = {
	{ 1, {   384000, PLL_8, 0, 0x00 }, L2(0),   950000, AVS(0x40001F) },
	{ 0, {   432000, HFPLL, 2, 0x20 }, L2(6),   975000 },
	{ 1, {   486000, HFPLL, 2, 0x24 }, L2(6),   975000 },
	{ 0, {   540000, HFPLL, 2, 0x28 }, L2(6),  1000000 },
	{ 1, {   594000, HFPLL, 1, 0x16 }, L2(6),  1000000 },
	{ 0, {   648000, HFPLL, 1, 0x18 }, L2(6),  1025000 },
	{ 1, {   702000, HFPLL, 1, 0x1A }, L2(6),  1025000 },
	{ 0, {   756000, HFPLL, 1, 0x1C }, L2(6),  1075000 },
	{ 1, {   810000, HFPLL, 1, 0x1E }, L2(6),  1075000 },
	{ 0, {   864000, HFPLL, 1, 0x20 }, L2(6),  1100000 },
	{ 1, {   918000, HFPLL, 1, 0x22 }, L2(6),  1100000 },
	{ 0, {   972000, HFPLL, 1, 0x24 }, L2(6),  1125000 },
	{ 1, {  1026000, HFPLL, 1, 0x26 }, L2(6),  1125000 },
	{ 0, {  1080000, HFPLL, 1, 0x28 }, L2(18), 1175000, AVS(0x400015) },
	{ 1, {  1134000, HFPLL, 1, 0x2A }, L2(18), 1175000, AVS(0x400015) },
	{ 0, {  1188000, HFPLL, 1, 0x2C }, L2(18), 1200000, AVS(0x400015) },
	{ 1, {  1242000, HFPLL, 1, 0x2E }, L2(18), 1200000, AVS(0x400015) },
	{ 0, {  1296000, HFPLL, 1, 0x30 }, L2(18), 1225000, AVS(0x400015) },
	{ 1, {  1350000, HFPLL, 1, 0x32 }, L2(18), 1225000, AVS(0x400015) },
	{ 0, {  1404000, HFPLL, 1, 0x34 }, L2(18), 1237500, AVS(0x400015) },
	{ 1, {  1458000, HFPLL, 1, 0x36 }, L2(18), 1237500, AVS(0x100018) },
	{ 1, {  1512000, HFPLL, 1, 0x38 }, L2(18), 1250000, AVS(0x400012) },
	{ 0, { 0 } }
};

static struct acpu_level acpu_freq_tbl_nom[] __initdata = {
	{ 1, {   384000, PLL_8, 0, 0x00 }, L2(0),   900000, AVS(0x40007F) },
	{ 0, {   432000, HFPLL, 2, 0x20 }, L2(6),   925000 },
	{ 1, {   486000, HFPLL, 2, 0x24 }, L2(6),   925000 },
	{ 0, {   540000, HFPLL, 2, 0x28 }, L2(6),   950000 },
	{ 1, {   594000, HFPLL, 1, 0x16 }, L2(6),   950000 },
	{ 0, {   648000, HFPLL, 1, 0x18 }, L2(6),   975000 },
	{ 1, {   702000, HFPLL, 1, 0x1A }, L2(6),   975000 },
	{ 0, {   756000, HFPLL, 1, 0x1C }, L2(6),  1025000 },
	{ 1, {   810000, HFPLL, 1, 0x1E }, L2(6),  1025000 },
	{ 0, {   864000, HFPLL, 1, 0x20 }, L2(6),  1050000 },
	{ 1, {   918000, HFPLL, 1, 0x22 }, L2(6),  1050000 },
	{ 0, {   972000, HFPLL, 1, 0x24 }, L2(6),  1075000 },
	{ 1, {  1026000, HFPLL, 1, 0x26 }, L2(6),  1075000 },
	{ 0, {  1080000, HFPLL, 1, 0x28 }, L2(18), 1125000, AVS(0x400015) },
	{ 1, {  1134000, HFPLL, 1, 0x2A }, L2(18), 1125000, AVS(0x400015) },
	{ 0, {  1188000, HFPLL, 1, 0x2C }, L2(18), 1150000, AVS(0x400015) },
	{ 1, {  1242000, HFPLL, 1, 0x2E }, L2(18), 1150000, AVS(0x400015) },
	{ 0, {  1296000, HFPLL, 1, 0x30 }, L2(18), 1175000, AVS(0x400015) },
	{ 1, {  1350000, HFPLL, 1, 0x32 }, L2(18), 1175000, AVS(0x400015) },
	{ 0, {  1404000, HFPLL, 1, 0x34 }, L2(18), 1187500, AVS(0x400015) },
	{ 1, {  1458000, HFPLL, 1, 0x36 }, L2(18), 1187500, AVS(0x100018) },
	{ 1, {  1512000, HFPLL, 1, 0x38 }, L2(18), 1200000, AVS(0x400012) },
	{ 0, { 0 } }
};

static struct acpu_level acpu_freq_tbl_fast[] __initdata = {
	{ 1, {   384000, PLL_8, 0, 0x00 }, L2(0),   850000, AVS(0x4000FF) },
	{ 0, {   432000, HFPLL, 2, 0x20 }, L2(6),   875000 },
	{ 1, {   486000, HFPLL, 2, 0x24 }, L2(6),   875000 },
	{ 0, {   540000, HFPLL, 2, 0x28 }, L2(6),   900000 },
	{ 1, {   594000, HFPLL, 1, 0x16 }, L2(6),   900000 },
	{ 0, {   648000, HFPLL, 1, 0x18 }, L2(6),   925000 },
	{ 1, {   702000, HFPLL, 1, 0x1A }, L2(6),   925000 },
	{ 0, {   756000, HFPLL, 1, 0x1C }, L2(6),   975000 },
	{ 1, {   810000, HFPLL, 1, 0x1E }, L2(6),   975000 },
	{ 0, {   864000, HFPLL, 1, 0x20 }, L2(6),  1000000 },
	{ 1, {   918000, HFPLL, 1, 0x22 }, L2(6),  1000000 },
	{ 0, {   972000, HFPLL, 1, 0x24 }, L2(6),  1025000 },
	{ 1, {  1026000, HFPLL, 1, 0x26 }, L2(6),  1025000 },
	{ 0, {  1080000, HFPLL, 1, 0x28 }, L2(18), 1075000, AVS(0x10001B) },
	{ 1, {  1134000, HFPLL, 1, 0x2A }, L2(18), 1075000, AVS(0x10001B) },
	{ 0, {  1188000, HFPLL, 1, 0x2C }, L2(18), 1100000, AVS(0x10001B) },
	{ 1, {  1242000, HFPLL, 1, 0x2E }, L2(18), 1100000, AVS(0x10001B) },
	{ 0, {  1296000, HFPLL, 1, 0x30 }, L2(18), 1125000, AVS(0x10001B) },
	{ 1, {  1350000, HFPLL, 1, 0x32 }, L2(18), 1125000, AVS(0x400012) },
	{ 0, {  1404000, HFPLL, 1, 0x34 }, L2(18), 1137500, AVS(0x400012) },
	{ 1, {  1458000, HFPLL, 1, 0x36 }, L2(18), 1137500, AVS(0x400012) },
	{ 1, {  1512000, HFPLL, 1, 0x38 }, L2(18), 1150000, AVS(0x400012) },
	{ 0, { 0 } }
};

static struct pvs_table pvs_tables[NUM_SPEED_BINS][NUM_PVS] __initdata = {
[0][PVS_SLOW]    = { acpu_freq_tbl_slow, sizeof(acpu_freq_tbl_slow),     0 },
[0][PVS_NOMINAL] = { acpu_freq_tbl_nom,  sizeof(acpu_freq_tbl_nom),  25000 },
[0][PVS_FAST]    = { acpu_freq_tbl_fast, sizeof(acpu_freq_tbl_fast), 25000 },
};

static struct acpuclk_krait_params acpuclk_8960_params __initdata = {
	.scalable = scalable,
	.scalable_size = sizeof(scalable),
	.hfpll_data = &hfpll_data,
	.pvs_tables = pvs_tables,
	.l2_freq_tbl = l2_freq_tbl,
	.l2_freq_tbl_size = sizeof(l2_freq_tbl),
	.bus_scale = &bus_scale_data,
	.pte_efuse_phys = 0x007000C0,
	.stby_khz = 384000,
};

<<<<<<< HEAD
=======
static struct acpu_level *max_acpu_level;

static unsigned long acpuclk_8960_get_rate(int cpu)
{
	return scalable[cpu].current_speed->khz;
}

static int get_pri_clk_src(struct scalable *sc)
{
	uint32_t regval;

	regval = get_l2_indirect_reg(sc->l2cpmr_iaddr);
	return regval & 0x3;
}

static void set_pri_clk_src(struct scalable *sc, uint32_t pri_src_sel)
{
	uint32_t regval;

	regval = get_l2_indirect_reg(sc->l2cpmr_iaddr);
	regval &= ~0x3;
	regval |= (pri_src_sel & 0x3);
	set_l2_indirect_reg(sc->l2cpmr_iaddr, regval);
	
	mb();
	udelay(1);
}

static int get_sec_clk_src(struct scalable *sc)
{
	uint32_t regval;

	regval = get_l2_indirect_reg(sc->l2cpmr_iaddr);
	return (regval >> 2) & 0x3;
}

static void set_sec_clk_src(struct scalable *sc, uint32_t sec_src_sel)
{
	uint32_t regval;

	
	regval = get_l2_indirect_reg(sc->l2cpmr_iaddr);
	regval |= SECCLKAGD;
	set_l2_indirect_reg(sc->l2cpmr_iaddr, regval);

	
	regval &= ~(0x3 << 2);
	regval |= ((sec_src_sel & 0x3) << 2);
	set_l2_indirect_reg(sc->l2cpmr_iaddr, regval);

	
	mb();
	udelay(1);

	
	regval &= ~SECCLKAGD;
	set_l2_indirect_reg(sc->l2cpmr_iaddr, regval);
}

static void hfpll_enable(struct scalable *sc, bool skip_regulators)
{
	int rc;

	if (!skip_regulators) {
		if (cpu_is_msm8960()) {
			rc = rpm_vreg_set_voltage(
					sc->vreg[VREG_HFPLL_A].rpm_vreg_id,
					sc->vreg[VREG_HFPLL_A].rpm_vreg_voter,
					2050000,
					sc->vreg[VREG_HFPLL_A].max_vdd, 0);
			if (rc)
				pr_err("%s regulator enable failed (%d)\n",
					sc->vreg[VREG_HFPLL_A].name, rc);
		}
		rc = rpm_vreg_set_voltage(sc->vreg[VREG_HFPLL_B].rpm_vreg_id,
				sc->vreg[VREG_HFPLL_B].rpm_vreg_voter, 1800000,
				sc->vreg[VREG_HFPLL_B].max_vdd, 0);
		if (rc)
			pr_err("%s regulator enable failed (%d)\n",
				sc->vreg[VREG_HFPLL_B].name, rc);
	}
	
	writel_relaxed(0x2, sc->hfpll_base + HFPLL_MODE);

	mb();
	udelay(10);

	
	writel_relaxed(0x6, sc->hfpll_base + HFPLL_MODE);

	
	mb();
	udelay(60);

	
	writel_relaxed(0x7, sc->hfpll_base + HFPLL_MODE);
}

static void hfpll_disable(struct scalable *sc, bool skip_regulators)
{
	int rc;

	writel_relaxed(0, sc->hfpll_base + HFPLL_MODE);

	if (!skip_regulators) {
		rc = rpm_vreg_set_voltage(sc->vreg[VREG_HFPLL_B].rpm_vreg_id,
				sc->vreg[VREG_HFPLL_B].rpm_vreg_voter, 0,
				0, 0);
		if (rc)
			pr_err("%s regulator enable failed (%d)\n",
				sc->vreg[VREG_HFPLL_B].name, rc);

		if (cpu_is_msm8960()) {
			rc = rpm_vreg_set_voltage(
					sc->vreg[VREG_HFPLL_A].rpm_vreg_id,
					sc->vreg[VREG_HFPLL_A].rpm_vreg_voter,
					0, 0, 0);
			if (rc)
				pr_err("%s regulator enable failed (%d)\n",
					sc->vreg[VREG_HFPLL_A].name, rc);
		}
	}
}

static void hfpll_set_rate(struct scalable *sc, struct core_speed *tgt_s)
{
	writel_relaxed(tgt_s->pll_l_val, sc->hfpll_base + HFPLL_L_VAL);
}

static struct l2_level *compute_l2_level(struct scalable *sc,
					 struct l2_level *vote_l)
{
	struct l2_level *new_l;
	int cpu;

	
	BUG_ON(vote_l >= (l2_freq_tbl + l2_freq_tbl_size));

	
	sc->l2_vote = vote_l;
	new_l = l2_freq_tbl;
	for_each_present_cpu(cpu)
		new_l = max(new_l, scalable[cpu].l2_vote);

	return new_l;
}

static void set_bus_bw(unsigned int bw)
{
	int ret;

	
	if (bw >= ARRAY_SIZE(bw_level_tbl)) {
		pr_err("invalid bandwidth request (%d)\n", bw);
		return;
	}

	
	ret = msm_bus_scale_client_update_request(bus_perf_client, bw);
	if (ret)
		pr_err("bandwidth request failed (%d)\n", ret);
}

static void set_speed(struct scalable *sc, struct core_speed *tgt_s,
		      enum setrate_reason reason)
{
	struct core_speed *strt_s = sc->current_speed;

	if (tgt_s == strt_s)
		return;

	if (strt_s->src == HFPLL && tgt_s->src == HFPLL) {
		set_sec_clk_src(sc, SEC_SRC_SEL_AUX);
		set_pri_clk_src(sc, PRI_SRC_SEL_SEC_SRC);

		
		hfpll_disable(sc, 1);
		hfpll_set_rate(sc, tgt_s);
		hfpll_enable(sc, 1);

		
		set_pri_clk_src(sc, tgt_s->pri_src_sel);
	} else if (strt_s->src == HFPLL && tgt_s->src != HFPLL) {
		if (reason != SETRATE_HOTPLUG || sc == &scalable[L2]) {
			set_sec_clk_src(sc, tgt_s->sec_src_sel);
			set_pri_clk_src(sc, tgt_s->pri_src_sel);
			hfpll_disable(sc, 0);
		} else if (reason == SETRATE_HOTPLUG
			   && msm_pm_verify_cpu_pc(SCALABLE_TO_CPU(sc))) {
			hfpll_disable(sc, 0);
		}
	} else if (strt_s->src != HFPLL && tgt_s->src == HFPLL) {
		if (reason != SETRATE_HOTPLUG || sc == &scalable[L2]) {
			hfpll_set_rate(sc, tgt_s);
			hfpll_enable(sc, 0);
			set_pri_clk_src(sc, tgt_s->pri_src_sel);
		} else if (reason == SETRATE_HOTPLUG
			   && msm_pm_verify_cpu_pc(SCALABLE_TO_CPU(sc))) {
			
			hfpll_set_rate(sc, tgt_s);
			hfpll_enable(sc, 0);
		}
	} else {
		if (reason != SETRATE_HOTPLUG || sc == &scalable[L2])
			set_sec_clk_src(sc, tgt_s->sec_src_sel);
	}

	sc->current_speed = tgt_s;
}

static int increase_vdd(int cpu, unsigned int vdd_core, unsigned int vdd_mem,
			unsigned int vdd_dig, enum setrate_reason reason)
{
	struct scalable *sc = &scalable[cpu];
	int rc = 0;

	if (vdd_mem > sc->vreg[VREG_MEM].cur_vdd) {
		rc = rpm_vreg_set_voltage(sc->vreg[VREG_MEM].rpm_vreg_id,
				sc->vreg[VREG_MEM].rpm_vreg_voter, vdd_mem,
				sc->vreg[VREG_MEM].max_vdd, 0);
		if (rc) {
			pr_err("%s increase failed (%d)\n",
				sc->vreg[VREG_MEM].name, rc);
			return rc;
		}
		 sc->vreg[VREG_MEM].cur_vdd = vdd_mem;
	}

	
	if (vdd_dig > sc->vreg[VREG_DIG].cur_vdd) {
		rc = rpm_vreg_set_voltage(sc->vreg[VREG_DIG].rpm_vreg_id,
				sc->vreg[VREG_DIG].rpm_vreg_voter, vdd_dig,
				sc->vreg[VREG_DIG].max_vdd, 0);
		if (rc) {
			pr_err("%s increase failed (%d)\n",
				sc->vreg[VREG_DIG].name, rc);
			return rc;
		}
		sc->vreg[VREG_DIG].cur_vdd = vdd_dig;
	}

	if (vdd_core > sc->vreg[VREG_CORE].cur_vdd
						&& reason != SETRATE_HOTPLUG) {
		rc = regulator_set_voltage(sc->vreg[VREG_CORE].reg, vdd_core,
					   sc->vreg[VREG_CORE].max_vdd);
		if (rc) {
			pr_err("%s increase failed (%d)\n",
				sc->vreg[VREG_CORE].name, rc);
			return rc;
		}
		sc->vreg[VREG_CORE].cur_vdd = vdd_core;
	}

	return rc;
}

static void decrease_vdd(int cpu, unsigned int vdd_core, unsigned int vdd_mem,
			 unsigned int vdd_dig, enum setrate_reason reason)
{
	struct scalable *sc = &scalable[cpu];
	int ret;

	if (vdd_core < sc->vreg[VREG_CORE].cur_vdd
					&& reason != SETRATE_HOTPLUG) {
		ret = regulator_set_voltage(sc->vreg[VREG_CORE].reg, vdd_core,
					    sc->vreg[VREG_CORE].max_vdd);
		if (ret) {
			pr_err("%s decrease failed (%d)\n",
			       sc->vreg[VREG_CORE].name, ret);
			return;
		}
		sc->vreg[VREG_CORE].cur_vdd = vdd_core;
	}

	
	if (vdd_dig < sc->vreg[VREG_DIG].cur_vdd) {
		ret = rpm_vreg_set_voltage(sc->vreg[VREG_DIG].rpm_vreg_id,
				sc->vreg[VREG_DIG].rpm_vreg_voter, vdd_dig,
				sc->vreg[VREG_DIG].max_vdd, 0);
		if (ret) {
			pr_err("%s decrease failed (%d)\n",
				sc->vreg[VREG_DIG].name, ret);
			return;
		}
		sc->vreg[VREG_DIG].cur_vdd = vdd_dig;
	}

	if (vdd_mem < sc->vreg[VREG_MEM].cur_vdd) {
		ret = rpm_vreg_set_voltage(sc->vreg[VREG_MEM].rpm_vreg_id,
				sc->vreg[VREG_MEM].rpm_vreg_voter, vdd_mem,
				sc->vreg[VREG_MEM].max_vdd, 0);
		if (ret) {
			pr_err("%s decrease failed (%d)\n",
				sc->vreg[VREG_MEM].name, ret);
			return;
		}
		 sc->vreg[VREG_MEM].cur_vdd = vdd_mem;
	}
}

static unsigned int calculate_vdd_mem(struct acpu_level *tgt)
{
	return tgt->l2_level->vdd_mem;
}

static unsigned int calculate_vdd_dig(struct acpu_level *tgt)
{
	unsigned int pll_vdd_dig;

	if (tgt->l2_level->speed.src != HFPLL)
		pll_vdd_dig = scalable[L2].hfpll_vdd_tbl[HFPLL_VDD_NONE];
	else if (tgt->l2_level->speed.pll_l_val > HFPLL_LOW_VDD_PLL_L_MAX)
		pll_vdd_dig = scalable[L2].hfpll_vdd_tbl[HFPLL_VDD_NOM];
	else
		pll_vdd_dig = scalable[L2].hfpll_vdd_tbl[HFPLL_VDD_LOW];

	return max(tgt->l2_level->vdd_dig, pll_vdd_dig);
}

static unsigned int calculate_vdd_core(struct acpu_level *tgt)
{
	return tgt->vdd_core;
}

static int acpuclk_8960_set_rate(int cpu, unsigned long rate,
				 enum setrate_reason reason)
{
	struct core_speed *strt_acpu_s, *tgt_acpu_s;
	struct l2_level *tgt_l2_l;
	struct acpu_level *tgt;
	unsigned int vdd_mem, vdd_dig, vdd_core;
	unsigned long flags;
	int rc = 0;

	if (reason == SETRATE_CPUFREQ) {
		if (cpu_is_offline(cpu)) {
			pr_info("don't change frequency for an offline CPU%d\n", cpu);
			return 0;
		} else if (smp_processor_id() != cpu) {
			pr_info("CPU%d don't change frequency for CPU%d\n", smp_processor_id(), cpu);
			return 0;
		}
	}

	set_acpuclk_foot_print(cpu, 0x1);

	if (cpu > num_possible_cpus())
		return -EINVAL;

	if (reason == SETRATE_CPUFREQ || reason == SETRATE_HOTPLUG)
		mutex_lock(&driver_lock);

	set_acpuclk_foot_print(cpu, 0x2);

	strt_acpu_s = scalable[cpu].current_speed;

	
	if (rate == strt_acpu_s->khz)
		goto out;

	
	for (tgt = acpu_freq_tbl; tgt->speed.khz != 0; tgt++) {
		if (tgt->speed.khz == rate) {
			tgt_acpu_s = &tgt->speed;
			break;
		}
	}
	if (tgt->speed.khz == 0) {
		rc = -EINVAL;
		goto out;
	}

	
	vdd_mem  = calculate_vdd_mem(tgt);
	vdd_dig  = calculate_vdd_dig(tgt);
	vdd_core = calculate_vdd_core(tgt);

	
	if (reason == SETRATE_CPUFREQ || reason == SETRATE_HOTPLUG) {
		rc = increase_vdd(cpu, vdd_core, vdd_mem, vdd_dig, reason);
		if (rc)
			goto out;
	}

	pr_debug("Switching from ACPU%d rate %u KHz -> %u KHz\n",
		cpu, strt_acpu_s->khz, tgt_acpu_s->khz);
	udelay(60);
	set_acpuclk_foot_print(cpu, 0x3);

	if (reason == SETRATE_CPUFREQ) {
		if (cpu_is_offline(cpu)) {
			pr_info("don't set_speed for an offline CPU%d\n", cpu);
			goto out;
		} else if (smp_processor_id() != cpu) {
			pr_info("CPU%d don't set_speed for CPU%d\n", smp_processor_id(), cpu);
			goto out;
		}
	}

	
	set_speed(&scalable[cpu], tgt_acpu_s, reason);

	set_acpuclk_cpu_freq_foot_print(cpu, tgt_acpu_s->khz);
	set_acpuclk_foot_print(cpu, 0x4);

	spin_lock_irqsave(&l2_lock, flags);
	tgt_l2_l = compute_l2_level(&scalable[cpu], tgt->l2_level);
	set_speed(&scalable[L2], &tgt_l2_l->speed, reason);

	set_acpuclk_L2_freq_foot_print(tgt_l2_l->speed.khz);
	set_acpuclk_foot_print(cpu, 0x5);

	spin_unlock_irqrestore(&l2_lock, flags);

	
	if (reason == SETRATE_PC || reason == SETRATE_SWFI)
		goto out;

	
	set_bus_bw(tgt_l2_l->bw_level);

	set_acpuclk_foot_print(cpu, 0x6);

	
	decrease_vdd(cpu, vdd_core, vdd_mem, vdd_dig, reason);

	set_acpuclk_foot_print(cpu, 0x7);

	pr_debug("ACPU%d speed change complete\n", cpu);

out:
	if (reason == SETRATE_CPUFREQ || reason == SETRATE_HOTPLUG)
		mutex_unlock(&driver_lock);

	set_acpuclk_foot_print(cpu, 0x8);

	return rc;
}

static void hfpll_init(struct scalable *sc, struct core_speed *tgt_s)
{
	pr_debug("Initializing HFPLL%d\n", sc - scalable);

	
	hfpll_disable(sc, 1);

	
	writel_relaxed(0x7845C665, sc->hfpll_base + HFPLL_CONFIG_CTL);
	writel_relaxed(0, sc->hfpll_base + HFPLL_M_VAL);
	writel_relaxed(1, sc->hfpll_base + HFPLL_N_VAL);

	
	writel_relaxed(0x0108C000, sc->hfpll_base + HFPLL_DROOP_CTL);

	
	hfpll_set_rate(sc, tgt_s);
	hfpll_enable(sc, 0);
}

static void regulator_init(int cpu, struct acpu_level *lvl)
{
	int ret;
	struct scalable *sc = &scalable[cpu];
	unsigned int vdd_mem, vdd_dig, vdd_core;

	vdd_mem = calculate_vdd_mem(lvl);
	vdd_dig = calculate_vdd_dig(lvl);

	
	ret = rpm_vreg_set_voltage(sc->vreg[VREG_MEM].rpm_vreg_id,
			sc->vreg[VREG_MEM].rpm_vreg_voter, vdd_mem,
			sc->vreg[VREG_MEM].max_vdd, 0);
	if (ret) {
		pr_err("%s initialization failed (%d)\n",
			sc->vreg[VREG_MEM].name, ret);
		BUG();
	}
	sc->vreg[VREG_MEM].cur_vdd  = vdd_mem;

	
	ret = rpm_vreg_set_voltage(sc->vreg[VREG_DIG].rpm_vreg_id,
			sc->vreg[VREG_DIG].rpm_vreg_voter, vdd_dig,
			sc->vreg[VREG_DIG].max_vdd, 0);
	if (ret) {
		pr_err("%s initialization failed (%d)\n",
			sc->vreg[VREG_DIG].name, ret);
		BUG();
	}
	sc->vreg[VREG_DIG].cur_vdd  = vdd_dig;

	
	sc->vreg[VREG_CORE].reg = regulator_get(NULL,
				  sc->vreg[VREG_CORE].name);
	if (IS_ERR(sc->vreg[VREG_CORE].reg)) {
		pr_err("regulator_get(%s) failed (%ld)\n",
		       sc->vreg[VREG_CORE].name,
		       PTR_ERR(sc->vreg[VREG_CORE].reg));
		BUG();
	}
	vdd_core = calculate_vdd_core(lvl);
	ret = regulator_set_voltage(sc->vreg[VREG_CORE].reg, vdd_core,
				    sc->vreg[VREG_CORE].max_vdd);
	if (ret) {
		pr_err("%s initialization failed (%d)\n",
			sc->vreg[VREG_CORE].name, ret);
		BUG();
	}
	sc->vreg[VREG_CORE].cur_vdd = vdd_core;
	ret = regulator_enable(sc->vreg[VREG_CORE].reg);
	if (ret) {
		pr_err("regulator_enable(%s) failed (%d)\n",
		       sc->vreg[VREG_CORE].name, ret);
		BUG();
	}
	sc->regulators_initialized = true;
}

static void init_clock_sources(struct scalable *sc,
				      struct core_speed *tgt_s)
{
	uint32_t regval;

	
	writel_relaxed(0x3, sc->aux_clk_sel);

	
	set_sec_clk_src(sc, SEC_SRC_SEL_AUX);
	set_pri_clk_src(sc, PRI_SRC_SEL_SEC_SRC);
	hfpll_init(sc, tgt_s);

	
	regval = get_l2_indirect_reg(sc->l2cpmr_iaddr);
	regval &= ~(0x3 << 6);
	set_l2_indirect_reg(sc->l2cpmr_iaddr, regval);

	
	set_sec_clk_src(sc, tgt_s->sec_src_sel);
	set_pri_clk_src(sc, tgt_s->pri_src_sel);
	sc->current_speed = tgt_s;
}

static void per_cpu_init(void *data)
{
	int cpu = smp_processor_id();

	init_clock_sources(&scalable[cpu], &max_acpu_level->speed);
	scalable[cpu].l2_vote = max_acpu_level->l2_level;
	scalable[cpu].clocks_initialized = true;
}

static void __init bus_init(unsigned int init_bw)
{
	int ret;

	bus_perf_client = msm_bus_scale_register_client(&bus_client_pdata);
	if (!bus_perf_client) {
		pr_err("unable to register bus client\n");
		BUG();
	}

	ret = msm_bus_scale_client_update_request(bus_perf_client, init_bw);
	if (ret)
		pr_err("initial bandwidth request failed (%d)\n", ret);
}

#ifdef CONFIG_CPU_VOLTAGE_TABLE

#define HFPLL_MIN_VDD		 800000
#define HFPLL_MAX_VDD		1350000

ssize_t acpuclk_get_vdd_levels_str(char *buf) {

	int i, len = 0;

	if (buf) {
		mutex_lock(&driver_lock);

		for (i = 0; acpu_freq_tbl[i].speed.khz; i++) {
			/* updated to use uv required by 8x60 architecture - faux123 */
			len += sprintf(buf + len, "%8u: %8d\n", acpu_freq_tbl[i].speed.khz,
				acpu_freq_tbl[i].vdd_core);
		}

		mutex_unlock(&driver_lock);
	}
	return len;
}

/* updated to use uv required by 8x60 architecture - faux123 */
void acpuclk_set_vdd(unsigned int khz, int vdd_uv) {

	int i;
	unsigned int new_vdd_uv;

	mutex_lock(&driver_lock);

	for (i = 0; acpu_freq_tbl[i].speed.khz; i++) {
		if (khz == 0)
			new_vdd_uv = min(max((unsigned int)(acpu_freq_tbl[i].vdd_core + vdd_uv),
				(unsigned int)HFPLL_MIN_VDD), (unsigned int)HFPLL_MAX_VDD);
		else if (acpu_freq_tbl[i].speed.khz == khz)
			new_vdd_uv = min(max((unsigned int)vdd_uv,
				(unsigned int)HFPLL_MIN_VDD), (unsigned int)HFPLL_MAX_VDD);
		else 
			continue;

		acpu_freq_tbl[i].vdd_core = new_vdd_uv;
	}
	pr_warn("faux123: user voltage table modified!\n");
	mutex_unlock(&driver_lock);
}
#endif	/* CONFIG_CPU_VOLTAGE_TABLE */

#ifdef CONFIG_CPU_FREQ_MSM
static struct cpufreq_frequency_table freq_table[NR_CPUS][30];

static void __init cpufreq_table_init(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		int i, freq_cnt = 0;
		
		for (i = 0; acpu_freq_tbl[i].speed.khz != 0
				&& freq_cnt < ARRAY_SIZE(*freq_table); i++) {
			if (acpu_freq_tbl[i].use_for_scaling) {
				freq_table[cpu][freq_cnt].index = freq_cnt;
				freq_table[cpu][freq_cnt].frequency
					= acpu_freq_tbl[i].speed.khz;
				freq_cnt++;
			}
		}
		
		BUG_ON(acpu_freq_tbl[i].speed.khz != 0);

		freq_table[cpu][freq_cnt].index = freq_cnt;
		freq_table[cpu][freq_cnt].frequency = CPUFREQ_TABLE_END;

		pr_info("CPU%d: %d scaling frequencies supported.\n",
			cpu, freq_cnt);

		
		cpufreq_frequency_table_get_attr(freq_table[cpu], cpu);
	}
}
#else
static void __init cpufreq_table_init(void) {}
#endif

#define HOT_UNPLUG_KHZ STBY_KHZ
static int acpuclock_cpu_callback(struct notifier_block *nfb,
					    unsigned long action, void *hcpu)
{
	static int prev_khz[NR_CPUS];
	static int prev_pri_src[NR_CPUS];
	static int prev_sec_src[NR_CPUS];
	int cpu = (int)hcpu;

	switch (action) {
	case CPU_DYING:
	case CPU_DYING_FROZEN:
		if (cpu_is_krait_v1()) {
			prev_sec_src[cpu] = get_sec_clk_src(&scalable[cpu]);
			prev_pri_src[cpu] = get_pri_clk_src(&scalable[cpu]);
			set_sec_clk_src(&scalable[cpu], SEC_SRC_SEL_QSB);
			set_pri_clk_src(&scalable[cpu], PRI_SRC_SEL_SEC_SRC);
		}
		break;
	case CPU_DEAD:
	case CPU_DEAD_FROZEN:
		prev_khz[cpu] = acpuclk_8960_get_rate(cpu);
		
	case CPU_UP_CANCELED:
	case CPU_UP_CANCELED_FROZEN:
		if (scalable[cpu].clocks_initialized)
			acpuclk_8960_set_rate(cpu, HOT_UNPLUG_KHZ,
					      SETRATE_HOTPLUG);
		break;
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		if (scalable[cpu].clocks_initialized)
			acpuclk_8960_set_rate(cpu, prev_khz[cpu],
					      SETRATE_HOTPLUG);
		if (!scalable[cpu].regulators_initialized)
			regulator_init(cpu, max_acpu_level);
		break;
	case CPU_STARTING:
	case CPU_STARTING_FROZEN:
		if (!scalable[cpu].clocks_initialized) {
			per_cpu_init(NULL);
		} else if (cpu_is_krait_v1()) {
			set_sec_clk_src(&scalable[cpu], prev_sec_src[cpu]);
			set_pri_clk_src(&scalable[cpu], prev_pri_src[cpu]);
		}
		break;
	default:
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block acpuclock_cpu_notifier = {
	.notifier_call = acpuclock_cpu_callback,
};

static const int krait_needs_vmin(void)
{
	unsigned int cpuid = read_cpuid_id();

	pr_info("%s: cpuid is 0x%x\n", __func__, cpuid);
	switch (cpuid) {
	case 0x511F04D0:
	case 0x511F04D1:
	case 0x510F06F0:
		return 1;
	default:
		return 0;
	};
}

static void kraitv2_apply_vmin(struct acpu_level *tbl)
{
	for (; tbl->speed.khz != 0; tbl++)
		if (tbl->vdd_core < 1150000)
			tbl->vdd_core = 1150000;
}

static enum pvs __init get_pvs(void)
{
	uint32_t pte_efuse, pvs;
	unsigned int kernel_flag = get_kernel_flag();

	
	if(kernel_flag & KERNEL_FLAG_PVS_SLOW_CPU){
		pr_info("ACPU PVS: Force SLOW by writeconfig\n");
		return PVS_SLOW;
	}
	else if (kernel_flag & KERNEL_FLAG_PVS_NOM_CPU){
		pr_info("ACPU PVS: Force NOMINAL by writeconfig\n");
		return PVS_NOM;
	}
	else if (kernel_flag & KERNEL_FLAG_PVS_FAST_CPU){
		pr_info("ACPU PVS: Force FAST by writeconfig\n");
		return PVS_FAST;
	}

	pte_efuse = readl_relaxed(QFPROM_PTE_EFUSE_ADDR);
	pvs = (pte_efuse >> 10) & 0x7;
	if (pvs == 0x7)
		pvs = (pte_efuse >> 13) & 0x7;

	switch (pvs) {
	case 0x0:
	case 0x7:
		pr_info("ACPU PVS: Slow\n");
		return PVS_SLOW;
	case 0x1:
		pr_info("ACPU PVS: Nominal\n");
		return PVS_NOM;
	case 0x3:
		pr_info("ACPU PVS: Fast\n");
		return PVS_FAST;
	case 0x4:
		if (cpu_is_apq8064()) {
			pr_info("ACPU PVS: Faster\n");
			return  PVS_FASTER;
		}
	default:
		pr_warn("ACPU PVS: Unknown. Defaulting to slow\n");
		return PVS_SLOW;
	}
}

static void __init select_freq_plan(void)
{
	struct acpu_level *l;
	int tbl_selected = 0;

	if (cpu_is_msm8960()) {
		enum pvs pvs_id = get_pvs();
#ifdef CONFIG_DEBUG_FS
		krait_chip_variant = pvs_id;
#endif
		scalable = scalable_8960;
		if (cpu_is_krait_v1()) {
			acpu_freq_tbl = acpu_freq_tbl_8960_v1[pvs_id];
			l2_freq_tbl = l2_freq_tbl_8960_kraitv1;
			l2_freq_tbl_size = ARRAY_SIZE(l2_freq_tbl_8960_kraitv1);
			tbl_selected = 1;
		} else {
			acpu_freq_tbl = acpu_freq_tbl_8960_v2[pvs_id];
			l2_freq_tbl = l2_freq_tbl_8960_kraitv2;
			l2_freq_tbl_size = ARRAY_SIZE(l2_freq_tbl_8960_kraitv2);
			tbl_selected = 2;
		}
	} else if (cpu_is_apq8064()) {
		enum pvs pvs_id = get_pvs();
#ifdef CONFIG_DEBUG_FS
		krait_chip_variant = pvs_id;
#endif
		scalable = scalable_8064;
		acpu_freq_tbl = acpu_freq_tbl_8064[pvs_id];
		l2_freq_tbl = l2_freq_tbl_8064;
		l2_freq_tbl_size = ARRAY_SIZE(l2_freq_tbl_8064);
	} else if (cpu_is_msm8627()) {
		scalable = scalable_8627;
		acpu_freq_tbl = acpu_freq_tbl_8627;
		l2_freq_tbl = l2_freq_tbl_8627;
		l2_freq_tbl_size = ARRAY_SIZE(l2_freq_tbl_8627);
	} else if (cpu_is_msm8930() || cpu_is_msm8930aa()) {
		enum pvs pvs_id = get_pvs();
#ifdef CONFIG_DEBUG_FS
		krait_chip_variant = pvs_id;
#endif
		scalable = scalable_8930;
		acpu_freq_tbl = acpu_freq_tbl_8930_pvs[pvs_id];
		l2_freq_tbl = l2_freq_tbl_8930;
		l2_freq_tbl_size = ARRAY_SIZE(l2_freq_tbl_8930);
	} else {
		BUG();
	}
#ifdef CONFIG_DEBUG_FS
	krait_version = tbl_selected;
#endif
	BUG_ON(!acpu_freq_tbl);
	if (krait_needs_vmin()) {
		pr_info("Applying min 1.15v fix for Krait Errata 26\n");
		kraitv2_apply_vmin(acpu_freq_tbl);
	} else {
		pr_info("No need to apply min 1.15v fix for Krait Errata 26\n");
	}

	
	for (l = acpu_freq_tbl; l->speed.khz != 0; l++)
#ifdef CONFIG_ARCH_APQ8064
		if (l->use_for_scaling && (l->speed.khz <= 918000))
#else
		if (l->use_for_scaling)
#endif
			max_acpu_level = l;
	BUG_ON(!max_acpu_level);
	pr_info("Max ACPU freq: %u KHz\n", max_acpu_level->speed.khz);
}

static struct acpuclk_data acpuclk_8960_data = {
	.set_rate = acpuclk_8960_set_rate,
	.get_rate = acpuclk_8960_get_rate,
	.power_collapse_khz = STBY_KHZ,
	.wait_for_irq_khz = STBY_KHZ,
};

#ifdef CONFIG_DEBUG_FS
static int krait_variant_debugfs_show(struct seq_file *s, void *data)
{
	seq_printf(s, "Your cpu is: \n");
	seq_printf(s, "[%s] Krait Version 1 \n", ((krait_version == 1) ? "X" : " "));
	seq_printf(s, "[%s] Krait Version 2 \n", ((krait_version == 2) ? "X" : " "));
	seq_printf(s, "Your krait chip uses table: \n");
	seq_printf(s, "[%s] SLOW \n", ((krait_chip_variant == PVS_SLOW) ? "X" : " "));
	seq_printf(s, "[%s] NOMINAL \n", ((krait_chip_variant == PVS_NOM) ? "X" : " "));
	seq_printf(s, "[%s] FAST \n", ((krait_chip_variant == PVS_FAST) ? "X" : " "));
	seq_printf(s, "[%s] FASTER \n", ((krait_chip_variant == PVS_FASTER) ? "X" : " "));

	return 0;
}

static int krait_variant_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, krait_variant_debugfs_show, inode->i_private);
}

static const struct file_operations krait_variant_debugfs_fops = {
	.open		= krait_variant_debugfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init krait_variant_debugfs_init(void) {
        struct dentry *d;
        d = debugfs_create_file("krait_variant", S_IRUGO, NULL, NULL,
        &krait_variant_debugfs_fops);
        if (!d)
                return -ENOMEM;
        return 0;
}
late_initcall(krait_variant_debugfs_init);
#endif

>>>>>>> 02740b5... voltage_control: initial voltage control for 8960 krait
static int __init acpuclk_8960_probe(struct platform_device *pdev)
{
	return acpuclk_krait_init(&pdev->dev, &acpuclk_8960_params);
}

static struct platform_driver acpuclk_8960_driver = {
	.driver = {
		.name = "acpuclk-8960",
		.owner = THIS_MODULE,
	},
};

static int __init acpuclk_8960_init(void)
{
	return platform_driver_probe(&acpuclk_8960_driver, acpuclk_8960_probe);
}
device_initcall(acpuclk_8960_init);
