/*
 * Copyright (c) 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/backlight.h>
#include <drm/drmP.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>
#include <linux/of_graph.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <soc/oplus/device_info.h>

#define CONFIG_MTK_PANEL_EXT
#if defined(CONFIG_MTK_PANEL_EXT)
#include "../mediatek/mtk_panel_ext.h"
#include "../mediatek/mtk_log.h"
#include "../mediatek/mtk_drm_graphics_base.h"
#endif

#ifdef CONFIG_MTK_ROUND_CORNER_SUPPORT
#include "../mediatek/mtk_corner_pattern/mtk_data_hw_roundedpattern.h"
#endif

#include <mt-plat/mtk_boot_common.h>
#include <soc/oppo/oppo_project.h>
static char bl_tb0[] = { 0x51, 0xff };
extern int tp_control_irq(bool enable, int mode);
extern int __attribute((weak)) tp_gesture_enable_flag(void) { return 0; };
extern unsigned int __attribute((weak)) is_project(int project)  { return 0; }
/*****************************************************************************
 * Function Prototype
 *****************************************************************************/

extern unsigned long esd_flag;
static int esd_brightness = 1023;
extern void __attribute((weak)) lcd_queue_load_tp_fw(void) { return; };
extern bool __attribute__((weak)) tp_boot_mode_normal() {return true;};
extern unsigned long oplus_max_normal_brightness;
extern void __attribute((weak)) disp_aal_set_dre_en(int enable) { return; };
extern int _20015_lcm_i2c_write_bytes(unsigned char addr, unsigned char value);
/* #ifdef OPLUS_BUG_STABILITY */
/* shiyaqiang@RM.MM.LCD.Driver 20180621 add for keep cabc mode after resume*/
static int cabc_lastlevel = 0;
static int last_brightness = 0;
static void cabc_switch(void *dsi, dcs_write_gce cb,
                void *handle, unsigned int cabc_mode);
/* #endif */
static int backlight_gamma = 0;
extern int tp_control_reset_gpio(bool enable);
extern unsigned int g_shutdown_flag;

struct lcm {
	struct device *dev;
	struct drm_panel panel;
	struct backlight_device *backlight;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *bias_pos, *bias_neg;

//	struct gpio_desc *bias_en;
	bool prepared;
	bool enabled;

	int error;

    bool is_normal_mode;
};

#define lcm_dcs_write_seq(ctx, seq...) \
({\
	const u8 d[] = { seq };\
	BUILD_BUG_ON_MSG(ARRAY_SIZE(d) > 64, "DCS sequence too big for stack");\
	lcm_dcs_write(ctx, d, ARRAY_SIZE(d));\
})

#define lcm_dcs_write_seq_static(ctx, seq...) \
({\
	static const u8 d[] = { seq };\
	lcm_dcs_write(ctx, d, ARRAY_SIZE(d));\
})

static inline struct lcm *panel_to_lcm(struct drm_panel *panel)
{
	return container_of(panel, struct lcm, panel);
}

static void lcm_dcs_write(struct lcm *ctx, const void *data, size_t len)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	ssize_t ret;
	char *addr;

	if (ctx->error < 0)
		return;

	addr = (char *)data;
	if ((int)*addr < 0xB0)
		ret = mipi_dsi_dcs_write_buffer(dsi, data, len);
	else
		ret = mipi_dsi_generic_write(dsi, data, len);
	if (ret < 0) {
		dev_err(ctx->dev, "error %zd writing seq: %ph\n", ret, data);
		ctx->error = ret;
	}
}

#ifdef PANEL_SUPPORT_READBACK
static int lcm_dcs_read(struct lcm *ctx, u8 cmd, void *data, size_t len)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	ssize_t ret;

	if (ctx->error < 0)
		return 0;

	ret = mipi_dsi_dcs_read(dsi, cmd, data, len);
	if (ret < 0) {
		dev_err(ctx->dev, "error %d reading dcs seq:(%#x)\n", ret, cmd);
		ctx->error = ret;
	}

	return ret;
}

static void lcm_panel_get_data(struct lcm *ctx)
{
	u8 buffer[3] = {0};
	static int ret;

	if (ret == 0) {
		ret = lcm_dcs_read(ctx,  0x0A, buffer, 1);
		dev_info(ctx->dev, "return %d data(0x%08x) to dsi engine\n",
			 ret, buffer[0] | (buffer[1] << 8));
	}
}
#endif

#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
static struct regulator *disp_bias_pos;
static struct regulator *disp_bias_neg;


static int lcm_panel_bias_regulator_init(void)
{
	static int regulator_inited;
	int ret = 0;

	if (regulator_inited)
		return ret;

	/* please only get regulator once in a driver */
	disp_bias_pos = regulator_get(NULL, "dsv_pos");
	if (IS_ERR(disp_bias_pos)) { /* handle return value */
		ret = PTR_ERR(disp_bias_pos);
		pr_err("get dsv_pos fail, error: %d\n", ret);
		return ret;
	}

	disp_bias_neg = regulator_get(NULL, "dsv_neg");
	if (IS_ERR(disp_bias_neg)) { /* handle return value */
		ret = PTR_ERR(disp_bias_neg);
		pr_err("get dsv_neg fail, error: %d\n", ret);
		return ret;
	}

	regulator_inited = 1;
	return ret; /* must be 0 */

}

static int lcm_panel_bias_enable(void)
{
	int ret = 0;
	int retval = 0;

	lcm_panel_bias_regulator_init();

	/* set voltage with min & max*/
	ret = regulator_set_voltage(disp_bias_pos, 6000000, 6000000);
	if (ret < 0)
		pr_err("set voltage disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_set_voltage(disp_bias_neg, 6000000, 6000000);
	if (ret < 0)
		pr_err("set voltage disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	/* enable regulator */
	ret = regulator_enable(disp_bias_pos);
	if (ret < 0)
		pr_err("enable regulator disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_enable(disp_bias_neg);
	if (ret < 0)
		pr_err("enable regulator disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	return retval;
}

static int lcm_panel_bias_disable(void)
{
	int ret = 0;
	int retval = 0;

	lcm_panel_bias_regulator_init();

	ret = regulator_disable(disp_bias_neg);
	if (ret < 0)
		pr_err("disable regulator disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_disable(disp_bias_pos);
	if (ret < 0)
		pr_err("disable regulator disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	return retval;
}
#endif

static void lcm_panel_init(struct lcm *ctx)
{
	pr_info("SYQ %s+\n", __func__);
        lcm_dcs_write_seq_static(ctx, 0xB0, 0x04);
        lcm_dcs_write_seq_static(ctx, 0xD6, 0x80);
        lcm_dcs_write_seq_static(ctx, 0xF5, 0xE6, 0x3C);
        usleep_range(20 * 1000, 20 * 1000);
        lcm_dcs_write_seq_static(ctx, 0xB0, 0x04);
        lcm_dcs_write_seq_static(ctx, 0xB8, 0x02, 0x4a, 0x06, 0x00, 0x04, 0x4A);
        lcm_dcs_write_seq_static(ctx, 0xB9, 0x02, 0x4a, 0x18, 0x00, 0x09, 0x99);
        lcm_dcs_write_seq_static(ctx, 0xBA, 0x02, 0x4a, 0x18, 0x00, 0x09, 0xD3);
        lcm_dcs_write_seq_static(ctx, 0xCE, 0x16, 0x40, 0x60, 0x78, 0x87, 0x94, \
				0x9e, 0xab, 0xb8, 0xc4, 0xcd, 0xd6, 0xde, 0xe5, 0xec, 0xf3, 0xff, 0x07, 0x0F, 0x04, 0x04, 0x00, 0x04, 0x8C); //LEDPWM=30KHz and 12bit DBV
        lcm_dcs_write_seq_static(ctx, 0xEB, 0x07, 0xd0, 0x7d, 0x0e, 0x11, 0x01, 0x08, 0x00);
        lcm_dcs_write_seq_static(ctx, 0xED, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x67, 0xe6, 0xfb, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x10, 0x00);
        lcm_dcs_write_seq_static(ctx, 0xF9, 0x20, 0x3F, 0xe0, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x07, 0xff);
        /* usleep_range(20 * 1000, 20 * 1000);  */
        lcm_dcs_write_seq_static(ctx, 0x51, 0x00, 0x00);
        lcm_dcs_write_seq_static(ctx, 0x53, 0x24);
        lcm_dcs_write_seq_static(ctx, 0x55, 0x01);
        lcm_dcs_write_seq_static(ctx, 0x35, 0x00);
        lcm_dcs_write_seq_static(ctx, 0xB0, 0x03);
        /*CABC end*/
        tp_control_irq(true, 1);
        lcm_dcs_write_seq_static(ctx, 0x11, 0x00);
        usleep_range(80 * 1000, 80 * 1000);
        lcm_dcs_write_seq_static(ctx, 0x29, 0x00);
        usleep_range(20 * 1000, 20 * 1000);
        pr_info("%s-\n", __func__);
}

static int lcm_disable(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);

	if (!ctx->enabled)
		return 0;

	if (ctx->backlight) {
		ctx->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(ctx->backlight);
	}

	ctx->enabled = false;

	return 0;
}

static int lcm_unprepare(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	if (!ctx->prepared)
		return 0;
	//lcm_dcs_write_seq_static(ctx, 0xFF, 0x98, 0x83, 0x00);
	lcm_dcs_write_seq_static(ctx, 0x28);
	usleep_range(20 * 1000, 20 * 1000);
	lcm_dcs_write_seq_static(ctx, 0x10);
	usleep_range(80 * 1000, 80 * 1000);

	ctx->error = 0;
	ctx->prepared = false;

#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
	lcm_panel_bias_disable();
#else
	pr_info("%s: tp_gesture_enable_flag = %d \n", __func__, tp_gesture_enable_flag());
	if ((0 == tp_gesture_enable_flag()) || (esd_flag == 1) || (g_shutdown_flag == 1)) {
	pr_info("%s:  \n", __func__);
	ctx->reset_gpio =
		devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio)) {
		dev_err(ctx->dev, "%s: cannot get reset_gpio %ld\n",
			__func__, PTR_ERR(ctx->reset_gpio));
		return PTR_ERR(ctx->reset_gpio);
	}
	gpiod_set_value(ctx->reset_gpio, 0);
	devm_gpiod_put(ctx->dev, ctx->reset_gpio);
	usleep_range(2 * 1000, 2 * 1000);

	tp_control_reset_gpio(false);
	usleep_range(2 * 1000, 2 * 1000);

	ctx->bias_neg = devm_gpiod_get_index(ctx->dev,
		"bias", 1, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_neg)) {
		dev_err(ctx->dev, "%s: cannot get bias_neg %ld\n",
			__func__, PTR_ERR(ctx->bias_neg));
		return PTR_ERR(ctx->bias_neg);
	}
	gpiod_set_value(ctx->bias_neg, 0);
	devm_gpiod_put(ctx->dev, ctx->bias_neg);

	usleep_range(1 * 1000, 1 * 1000);

	ctx->bias_pos = devm_gpiod_get_index(ctx->dev,
		"bias", 0, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_pos)) {
		dev_err(ctx->dev, "%s: cannot get bias_pos %ld\n",
			__func__, PTR_ERR(ctx->bias_pos));
		return PTR_ERR(ctx->bias_pos);
	}
	gpiod_set_value(ctx->bias_pos, 0);
	devm_gpiod_put(ctx->dev, ctx->bias_pos);
	usleep_range(30 * 1000, 30 * 1000);
/*        usleep_range(2 * 1000, 2 * 1000);
        ctx->bias_en = devm_gpiod_get(ctx->dev, "ldo", GPIOD_OUT_HIGH);
        gpiod_set_value(ctx->bias_en, 0);
        devm_gpiod_put(ctx->dev, ctx->bias_en);*/
	}
#endif
	return 0;
}

static int lcm_panel_poweron(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	int ret;
	pr_info("%s+\n", __func__);
	tp_control_irq(false, 1);

	ctx->bias_pos = devm_gpiod_get_index(ctx->dev,
		"bias", 0, GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->bias_pos, 1);
	devm_gpiod_put(ctx->dev, ctx->bias_pos);

	usleep_range(3 * 1000, 3 * 1000);
	ctx->bias_neg = devm_gpiod_get_index(ctx->dev,
		"bias", 1, GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->bias_neg, 1);
	devm_gpiod_put(ctx->dev, ctx->bias_neg);
	_20015_lcm_i2c_write_bytes(0x0, 0x14);
	usleep_range(1 * 1000, 1 * 1000);
	_20015_lcm_i2c_write_bytes(0x1, 0x14);

	pr_info("%s-\n", __func__);
	/* #ifdef OPLUS_BUG_STABILITY */
  	//lcd_queue_load_tp_fw();
  	/* #endif */ /* OPLUS_BUG_STABILITY */
	return 0;
}

static int lcm_panel_poweroff(struct drm_panel *panel)
{
	return 0;
}

static int lcm_prepare(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	int ret;

	pr_info("%s\n", __func__);
	if (ctx->prepared)
		return 0;

#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
	lcm_panel_bias_enable();
#else
	pr_info("%s-\n", __func__);
/*	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	usleep_range(5 * 1000, 5 * 1000);
	gpiod_set_value(ctx->reset_gpio, 0);
	usleep_range(5 * 1000, 5 * 1000);*/
/*	ctx->bias_pos = devm_gpiod_get_index(ctx->dev,
		"bias", 0, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_pos)) {
		dev_err(ctx->dev, "%s: cannot get bias_pos %ld\n",
			__func__, PTR_ERR(ctx->bias_pos));
		return PTR_ERR(ctx->bias_pos);
	}
	gpiod_set_value(ctx->bias_pos, 1);
	devm_gpiod_put(ctx->dev, ctx->bias_pos);

	udelay(2000);

	ctx->bias_neg = devm_gpiod_get_index(ctx->dev,
		"bias", 1, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_neg)) {
		dev_err(ctx->dev, "%s: cannot get bias_neg %ld\n",
			__func__, PTR_ERR(ctx->bias_neg));
		return PTR_ERR(ctx->bias_neg);
	}
	gpiod_set_value(ctx->bias_neg, 1);
	devm_gpiod_put(ctx->dev, ctx->bias_neg);
	_lcm_i2c_write_bytes(0x0, 0xf);
	_lcm_i2c_write_bytes(0x1, 0xf);
    msleep(10);*/
#endif
	tp_control_reset_gpio(true);
	usleep_range(2 * 1000, 2 * 1000);
	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->reset_gpio, 0);
	usleep_range(5 * 1000, 5 * 1000);
	gpiod_set_value(ctx->reset_gpio, 1);
	devm_gpiod_put(ctx->dev, ctx->reset_gpio);
	usleep_range(22 * 1000, 22 * 1000);
	if (tp_boot_mode_normal()) {
		lcd_queue_load_tp_fw();
	}
	lcm_panel_init(ctx);

	ret = ctx->error;
	if (ret < 0)
		lcm_unprepare(panel);

	ctx->prepared = true;

#if defined(CONFIG_MTK_PANEL_EXT)
	mtk_panel_tch_rst(panel);
#endif
#ifdef PANEL_SUPPORT_READBACK
	lcm_panel_get_data(ctx);
#endif

	return ret;
}

static int lcm_enable(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);

	if (ctx->enabled)
		return 0;

	if (ctx->backlight) {
		ctx->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(ctx->backlight);
	}

	ctx->enabled = true;

	return 0;
}

#define VAC (1612)
#define HAC (720)

static struct drm_display_mode default_mode = {
	.clock = 136709,
	.hdisplay = HAC,
	.hsync_start = HAC + 55,//HFP
	.hsync_end = HAC + 55 + 4,//HSA
	.htotal = HAC + 55 + 4 + 23,//HBP
	.vdisplay = VAC,
	.vsync_start = VAC + 1205,//VFP
	.vsync_end = VAC + 1205 + 4,//VSA
	.vtotal = VAC + 1205 + 4 + 20,//VBP
	.vrefresh = 60,
};

static struct drm_display_mode performance_mode = {
	.clock = 136853,
	.hdisplay = HAC,
	.hsync_start = HAC + 55,//HFP
	.hsync_end = HAC + 55 + 4,//HSA
	.htotal = HAC + 55 + 4 + 23,//HBP
	.vdisplay = VAC,
	.vsync_start = VAC + 260,//VFP
	.vsync_end = VAC + 260 + 4,//VSA
	.vtotal = VAC + 260 + 4 + 20,//VBP
	.vrefresh = 90,
};

#if defined(CONFIG_MTK_PANEL_EXT)
static struct mtk_panel_params ext_params = {
	.pll_clk = 449,
	//.vfp_low_power = 2528,//45hz
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0A, .count = 1, .para_list[0] = 0x9c, .mask_list[0] = 0x9c,
	},
	.data_rate = 898,
	.dyn_fps = {
		.switch_en = 1,
		.vact_timing_fps = 90,
	},
#if 0
	.dyn = {
		.switch_en = 1,
		.hbp = 27,
		.pll_clk = 451,
	},
#endif
	.oplus_display_global_dre = 1,
	//.oplus_teot_ns_multiplier = 90,
};

static struct mtk_panel_params ext_params_90hz = {
	.pll_clk = 449,
	//.vfp_low_power = VFP_60HZ,
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0a, .count = 1, .para_list[0] = 0x9c, .mask_list[0] = 0x9c,
	},
	.data_rate = 898,
	.dyn_fps = {
		.switch_en = 1,
		.vact_timing_fps = 90,
	},
#if 0
	.dyn = {
		.switch_en = 1,
		.hbp = 27,
		.pll_clk = 451,
	},
#endif
	.oplus_display_global_dre = 1,
};

static int map_exp[4096] = {0};

static void init_global_exp_backlight(void)
{
	int lut_index[41] = {0, 4, 99, 144, 187, 227, 264, 300, 334, 366, 397, 427, 456, 484, 511, 537, 563, 587, 611, 635, 658, 680,
						702, 723, 744, 764, 784, 804, 823, 842, 861, 879, 897, 915, 933, 950, 967, 984, 1000, 1016, 1023};
	int lut_value1[41] = {4, 4, 6, 14, 24, 37, 52, 69, 87, 107, 128, 150, 173, 197, 222, 248, 275, 302, 330, 358, 387, 416, 446, 
						477, 507, 539, 570, 602, 634, 667, 700, 733, 767, 801, 835, 869, 903, 938, 973, 1008, 1023};
	int index_start = 0, index_end = 0;
	int value1_start = 0, value1_end = 0;
	int i,j;
	int index_len = sizeof(lut_index) / sizeof(int);
	int value_len = sizeof(lut_value1) / sizeof(int);
	if (index_len == value_len) {
		for (i = 0; i < index_len - 1; i++) {
			index_start = lut_index[i] * oplus_max_normal_brightness / 1023;
			index_end = lut_index[i+1] * oplus_max_normal_brightness / 1023;
			value1_start = lut_value1[i] * oplus_max_normal_brightness / 1023;
			value1_end = lut_value1[i+1] * oplus_max_normal_brightness / 1023;
			for (j = index_start; j <= index_end; j++) {
				map_exp[j] = value1_start + (value1_end - value1_start) * (j - index_start) / (index_end - index_start);
			}
		}
	}
}

static int lcm_setbacklight_cmdq(void *dsi, dcs_write_gce cb,
		void *handle, unsigned int level)
{
	/*char bl_tb0[] = {0x51, 0x07, 0xff};
	if (level > 2047)
		level = 2047;
	pr_err("%s backlight = %d\n", __func__, level);
	bl_tb0[1] = level >> 8;
	bl_tb0[2] = level & 0xFF;
	esd_brightness = level;
	if (!cb)
		return -1;

	pr_err("%s bl_tb0[1]=%x, bl_tb0[2]=%x\n", __func__, bl_tb0[1], bl_tb0[2]);
	cb(dsi, handle, bl_tb0, ARRAY_SIZE(bl_tb0));*/

	char bl_tb0[] = {0x51, 0xFF, 0xFF};
	char bl_tb1[] = {0x55, 0x00};
	//char bl_tb2[] = {0xFF, 0x98, 0x83, 0x00};
	char bl_tb3[] = {0x53, 0x24};
#if 0
	//char bl_tb4[] = {0xFF,0x98,0x82,0x08};
	char bl_tb5[] = {0xE0,0x54,0xFD,0x0B,0x16,0x2B,0x55,0x45,0x5A,0x77,0x92,0xA5,0xBF,0xEA,0x0F,0x37,0xAA,0x66,0xA0,0xC3,0xEF,0xFF,0x14,0x42,0x79,0xA8,0x03,0xEC};
	char bl_tb6[] = {0xE1,0x54,0xFD,0x0B,0x16,0x2B,0x55,0x45,0x5A,0x77,0x92,0xA5,0xBF,0xEA,0x0F,0x37,0xAA,0x66,0xA0,0xC3,0xEF,0xFF,0x14,0x42,0x79,0xA8,0x03,0xEC};
	char bl_tb8[] = {0xE0,0x00,0x22,0x4C,0x6E,0x9F,0x50,0xCC,0xF2,0x22,0x4B,0x95,0x8C,0xC2,0xF3,0x23,0xAA,0x55,0x8F,0xB4,0xE0,0xFF,0x06,0x35,0x6E,0x9E,0x03,0xEC};
	char bl_tb9[] = {0xE1,0x00,0x22,0x4C,0x6E,0x9F,0x50,0xCC,0xF2,0x22,0x4B,0x95,0x8C,0xC2,0xF3,0x23,0xAA,0x55,0x8F,0xB4,0xE0,0xFF,0x06,0x35,0x6E,0x9E,0x03,0xEC};
#endif
	pr_err("%s backlight = %d\n", __func__, level);
	if (level > 4095)
                level = 4095;
	if ((level > 0) && (level < oplus_max_normal_brightness)) {
		level = map_exp[level];
	}
	if (get_boot_mode() == KERNEL_POWER_OFF_CHARGING_BOOT && level > 0) {
		level = 2047;
	}
#if 0
	if(level < 14 && level > 0){
		backlight_gamma = 1;
		cb(dsi, handle, bl_tb4, ARRAY_SIZE(bl_tb4));
		cb(dsi, handle, bl_tb5, ARRAY_SIZE(bl_tb5));
		cb(dsi, handle, bl_tb6, ARRAY_SIZE(bl_tb6));
	}else if(level > 13 && backlight_gamma == 1){
		backlight_gamma = 0;
		cb(dsi, handle, bl_tb4, ARRAY_SIZE(bl_tb4));
		cb(dsi, handle, bl_tb8, ARRAY_SIZE(bl_tb8));
		cb(dsi, handle, bl_tb9, ARRAY_SIZE(bl_tb9));
	}
#endif
        bl_tb0[1] = level >> 8;
        bl_tb0[2] = level & 0xFF;
	esd_brightness = level;
	if (!cb)
		return -1;
	if (last_brightness == 0)
		usleep_range(15 * 1000, 15 * 1000);
	pr_err("%s SYQ bl_tb0[1]=%x, bl_tb0[2]=%x\n", __func__, bl_tb0[1], bl_tb0[2]);
	//cb(dsi, handle, bl_tb2, ARRAY_SIZE(bl_tb2));
	cb(dsi, handle, bl_tb0, ARRAY_SIZE(bl_tb0));
/* #ifdef OPLUS_BUG_STABILITY */
/* shiyaqiang@RM.MM.LCD.Driver 20180621 add for keep cabc mode after resume*/
	if ((last_brightness == 0) && (cabc_lastlevel != 0)) {
		bl_tb1[1] = cabc_lastlevel;
		cb(dsi, handle, bl_tb1, ARRAY_SIZE(bl_tb1));
		cb(dsi, handle, bl_tb3, ARRAY_SIZE(bl_tb3));
	}
	last_brightness = level;
/* #endif */
	return 0;
}

static int oppo_esd_backlight_check(void *dsi, dcs_write_gce cb,
		void *handle)
{
	char bl_tb0[] = {0x51, 0x07, 0xff};

	pr_err("%s esd_backlight = %d\n", __func__, esd_brightness);
	bl_tb0[1] = esd_brightness >> 8;
	bl_tb0[2] = esd_brightness & 0xFF;
	if (!cb)
		return -1;
	pr_err("%s bl_tb0[1]=%x, bl_tb0[2]=%x\n", __func__, bl_tb0[1], bl_tb0[2]);
	cb(dsi, handle, bl_tb0, ARRAY_SIZE(bl_tb0));

	return 1;
}

static struct drm_display_mode *get_mode_by_id(struct drm_panel *panel,
	unsigned int mode)
{
	struct drm_display_mode *m;
	unsigned int i = 0;

	list_for_each_entry(m, &panel->connector->modes, head) {
		if (i == mode)
			return m;
		i++;
	}
	return NULL;
}

static int mtk_panel_ext_param_set(struct drm_panel *panel,
			 unsigned int mode)
{
	struct mtk_panel_ext *ext = find_panel_ext(panel);
	int ret = 0;
	struct drm_display_mode *m = get_mode_by_id(panel, mode);
	pr_err("%s ,%d\n", __func__,mode);
	if (mode == 0)
		ext->params = &ext_params;
	else if (mode == 1)
		ext->params = &ext_params_90hz;
	if (m->vrefresh == 60)
		ext->params = &ext_params;
	else if (m->vrefresh == 90)
		ext->params = &ext_params_90hz;
	else
		ret = 1;

	return ret;
}

static int mtk_panel_ext_param_get(struct mtk_panel_params *ext_para,
			 unsigned int mode)
{
	int ret = 0;
	pr_err("%s ,%d\n", __func__,mode);
	if (mode == 0)
		ext_para = &ext_params;
	else if (mode == 1)
		ext_para = &ext_params_90hz;
	else
		ret = 1;

	return ret;

}

static void cabc_switch(void *dsi, dcs_write_gce cb,
		void *handle, unsigned int cabc_mode)
{
	char bl_tb0[] = {0x55, 0x00};
	char bl_tb3[] = {0x53, 0x24};
	pr_err("%s cabc = %d\n", __func__, cabc_mode);
	bl_tb0[1] = (u8)cabc_mode;
    	msleep(5);
	cb(dsi, handle, bl_tb3, ARRAY_SIZE(bl_tb3));//FB 01
	cb(dsi, handle, bl_tb0, ARRAY_SIZE(bl_tb0));//55 0X
/* #ifdef OPLUS_BUG_STABILITY */
/* shiyaqiang@RM.MM.LCD.Driver 20180621 add for keep cabc mode after resume*/
	cabc_lastlevel = cabc_mode;
/* #endif */
}

static int panel_ext_reset(struct drm_panel *panel, int on)
{
	struct lcm *ctx = panel_to_lcm(panel);

	ctx->reset_gpio =
		devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->reset_gpio, on);
	devm_gpiod_put(ctx->dev, ctx->reset_gpio);

	return 0;
}

static struct mtk_panel_funcs ext_funcs = {
	.reset = panel_ext_reset,
	.set_backlight_cmdq = lcm_setbacklight_cmdq,
	.esd_backlight_recovery = oppo_esd_backlight_check,
	.panel_poweron = lcm_panel_poweron,
	.panel_poweroff = lcm_panel_poweroff,
	.ext_param_set = mtk_panel_ext_param_set,
	.ext_param_get = mtk_panel_ext_param_get,
	//.mode_switch = mode_switch,
	.cabc_switch = cabc_switch,
};
#endif

struct panel_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;
};

static int lcm_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;
	struct drm_display_mode *mode2;

	mode = drm_mode_duplicate(panel->drm, &default_mode);
	if (!mode) {
		dev_err(panel->drm->dev, "failed to add mode %ux%ux@%u\n",
			default_mode.hdisplay, default_mode.vdisplay,
			default_mode.vrefresh);
		return -ENOMEM;
	}

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(panel->connector, mode);

	mode2 = drm_mode_duplicate(panel->drm, &performance_mode);
	if (!mode2) {
		dev_info(panel->drm->dev, "failed to add mode %ux%ux@%u\n",
			 performance_mode.hdisplay, performance_mode.vdisplay,
			 performance_mode.vrefresh);
		return -ENOMEM;
	}

	drm_mode_set_name(mode2);
	mode2->type = DRM_MODE_TYPE_DRIVER;
	drm_mode_probed_add(panel->connector, mode2);
	panel->connector->display_info.width_mm = 68;
	panel->connector->display_info.height_mm = 152;

	return 1;
}

static const struct drm_panel_funcs lcm_drm_funcs = {
	.disable = lcm_disable,
	.unprepare = lcm_unprepare,
	.prepare = lcm_prepare,
	.enable = lcm_enable,
	.get_modes = lcm_get_modes,
};

static int lcm_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct lcm *ctx;
	struct device_node *backlight;
	int ret;
	struct device_node *dsi_node, *remote_node = NULL, *endpoint = NULL;

	dsi_node = of_get_parent(dev->of_node);
	if (dsi_node) {
		endpoint = of_graph_get_next_endpoint(dsi_node, NULL);
		if (endpoint) {
			remote_node = of_graph_get_remote_port_parent(endpoint);
			pr_info("device_node name:%s\n", remote_node->name);
                   }
	}

	if (remote_node != dev->of_node) {
		pr_info("%s+ skip probe due to not current lcm.\n", __func__);
		return 0;
	}

	pr_info("%s+\n", __func__);
	ctx = devm_kzalloc(dev, sizeof(struct lcm), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, ctx);

	ctx->dev = dev;
	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO
			 | MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_EOT_PACKET
			 | MIPI_DSI_CLOCK_NON_CONTINUOUS;

	backlight = of_parse_phandle(dev->of_node, "backlight", 0);
	if (backlight) {
		ctx->backlight = of_find_backlight_by_node(backlight);
		of_node_put(backlight);

		if (!ctx->backlight)
			return -EPROBE_DEFER;
	}

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio)) {
		dev_err(dev, "%s: cannot get reset-gpios %ld\n",
			__func__, PTR_ERR(ctx->reset_gpio));
		return PTR_ERR(ctx->reset_gpio);
	}
	devm_gpiod_put(dev, ctx->reset_gpio);

	ctx->bias_pos = devm_gpiod_get_index(dev, "bias", 0, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_pos)) {
		dev_err(dev, "%s: cannot get bias-pos 0 %ld\n",
			__func__, PTR_ERR(ctx->bias_pos));
		return PTR_ERR(ctx->bias_pos);
	}
	devm_gpiod_put(dev, ctx->bias_pos);

	ctx->bias_neg = devm_gpiod_get_index(dev, "bias", 1, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_neg)) {
		dev_err(dev, "%s: cannot get bias-neg 1 %ld\n",
			__func__, PTR_ERR(ctx->bias_neg));
		return PTR_ERR(ctx->bias_neg);
	}
	devm_gpiod_put(dev, ctx->bias_neg);

	ctx->prepared = true;
	ctx->enabled = true;

	drm_panel_init(&ctx->panel);
	ctx->panel.dev = dev;
	ctx->panel.funcs = &lcm_drm_funcs;

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0)
		return ret;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0)
		drm_panel_remove(&ctx->panel);

#if defined(CONFIG_MTK_PANEL_EXT)
	mtk_panel_tch_handle_reg(&ctx->panel);
	ret = mtk_panel_ext_create(dev, &ext_params, &ext_funcs, &ctx->panel);
	if (ret < 0)
		return ret;
#endif

	oplus_max_normal_brightness = 3330;
	init_global_exp_backlight();

	register_device_proc("lcd","td4160_inx","INX");
	pr_info("%s-\n", __func__);

	return ret;
}

static int lcm_remove(struct mipi_dsi_device *dsi)
{
	struct lcm *ctx = mipi_dsi_get_drvdata(dsi);

	mipi_dsi_detach(dsi);
	drm_panel_remove(&ctx->panel);

	return 0;
}
static const struct of_device_id jdi_of_match[] = {
	{
		.compatible = "oplus2163b,inx,td4160,dphy,vdo",
	},
	{} };

MODULE_DEVICE_TABLE(of, jdi_of_match);

static struct mipi_dsi_driver jdi_driver = {
	.probe = lcm_probe,
	.remove = lcm_remove,
	.driver = {
			.name = "oplus2163b_td4160_inx_vdo_lcm_drv",
			.owner = THIS_MODULE,
			.of_match_table = jdi_of_match,
		},
};

module_mipi_dsi_driver(jdi_driver);

MODULE_AUTHOR("Elon Hsu <elon.hsu@mediatek.com>");
MODULE_DESCRIPTION("jdi r66451 CMD AMOLED Panel Driver");
MODULE_LICENSE("GPL v2");
