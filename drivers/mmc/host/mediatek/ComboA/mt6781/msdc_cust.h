/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _MSDC_CUST_MT6781_H_
#define _MSDC_CUST_MT6781_H_
#ifdef CONFIG_FPGA_EARLY_PORTING
#define FPGA_PLATFORM
#else
/* #define MTK_MSDC_BRINGUP_DEBUG */
#endif

#include <dt-bindings/mmc/mt6781-msdc.h>
//#define CONFIG_MTK_MSDC_BRING_UP_BYPASS
#if !defined(FPGA_PLATFORM)
#include <dt-bindings/clock/mt6781-clk.h>
#endif
/**************************************************************/
/* Section 1: Device Tree                                     */
/**************************************************************/
/* Names used for device tree lookup */
#define DT_COMPATIBLE_NAME      "mediatek,msdc"
#define MSDC0_CLK_NAME          "msdc0-clock"
#if defined(CONFIG_MTK_HW_FDE) || defined(CONFIG_MMC_CRYPTO)
#define MSDC0_AES_CLK_NAME          "msdc0-aes-clock"
#endif
#define MSDC0_SRC_HCLK_NAME     "msdc0-src-hclock"
#define MSDC0_HCLK_NAME         "msdc0-hclock"
#define MSDC1_CLK_NAME          "msdc1-clock"
#define MSDC1_HCLK_NAME         "msdc1-hclock"
#define MSDC0_IOCFG_NAME        "mediatek,iocfg_tl"
#define MSDC1_IOCFG_NAME        "mediatek,iocfg_lb"

/**************************************************************/
/* Section 2: Power                                           */
/**************************************************************/
#define SD_POWER_DEFAULT_ON     (0)

#if !defined(FPGA_PLATFORM)

#include <mt-plat/upmu_common.h>

#define REG_VEMC_VOCAL          PMIC_RG_VEMC_VOCAL_ADDR
#define MASK_VEMC_VOCAL         PMIC_RG_VEMC_VOCAL_MASK
#define SHIFT_VEMC_VOCAL        PMIC_RG_VEMC_VOCAL_SHIFT
#define FIELD_VEMC_VOCAL        (MASK_VEMC_VOSEL_CAL << SHIFT_VEMC_VOSEL_CAL)

#define REG_VEMC_VOSEL          PMIC_RG_VEMC_VOSEL_ADDR
#define MASK_VEMC_VOSEL         PMIC_RG_VEMC_VOSEL_MASK
#define SHIFT_VEMC_VOSEL        PMIC_RG_VEMC_VOSEL_SHIFT
#define FIELD_VEMC_VOSEL        (MASK_VEMC_VOSEL << SHIFT_VEMC_VOSEL)

#define REG_VEMC_EN             PMIC_RG_LDO_VEMC_EN_ADDR
#define MASK_VEMC_EN            PMIC_RG_LDO_VEMC_EN_MASK
#define SHIFT_VEMC_EN           PMIC_RG_LDO_VEMC_EN_SHIFT
#define FIELD_VEMC_EN           (MASK_VEMC_EN << SHIFT_VEMC_EN)

#define REG_VMC_VOCAL           PMIC_RG_VMC_VOCAL_ADDR
#define MASK_VMC_VOCAL          PMIC_RG_VMC_VOCAL_MASK
#define SHIFT_VMC_VOCAL         PMIC_RG_VMC_VOCAL_SHIFT
#define FIELD_VMC_VOCAL         (MASK_VMC_VOSEL_CAL << SHIFT_VMC_VOSEL_CAL)

#define REG_VMC_VOSEL           PMIC_RG_VMC_VOSEL_ADDR
#define MASK_VMC_VOSEL          PMIC_RG_VMC_VOSEL_MASK
#define SHIFT_VMC_VOSEL         PMIC_RG_VMC_VOSEL_SHIFT
#define FIELD_VMC_VOSEL         (MASK_VMC_VOSEL << SHIFT_VMC_VOSEL)

#define REG_VMC_EN              PMIC_RG_LDO_VMC_EN_ADDR
#define MASK_VMC_EN             PMIC_RG_LDO_VMC_EN_MASK
#define SHIFT_VMC_EN            PMIC_RG_LDO_VMC_EN_SHIFT
#define FIELD_VMC_EN            (MASK_VMC_EN << SHIFT_VMC_EN)

#define REG_VMCH_VOCAL          PMIC_RG_VMCH_VOCAL_ADDR
#define MASK_VMCH_VOCAL         PMIC_RG_VMCH_VOCAL_MASK
#define SHIFT_VMCH_VOCAL        PMIC_RG_VMCH_VOCAL_SHIFT
#define FIELD_VMCH_VOCAL        (MASK_VMCH_VOSEL_CAL << SHIFT_VMCH_VOSEL_CAL)

#define REG_VMCH_VOSEL          PMIC_RG_VMCH_VOSEL_ADDR
#define MASK_VMCH_VOSEL         PMIC_RG_VMCH_VOSEL_MASK
#define SHIFT_VMCH_VOSEL        PMIC_RG_VMCH_VOSEL_SHIFT
#define FIELD_VMCH_VOSEL        (MASK_VMCH_VOSEL << SHIFT_VMCH_VOSEL)

#define REG_VMCH_EN             PMIC_RG_LDO_VMCH_EN_ADDR
#define MASK_VMCH_EN            PMIC_RG_LDO_VMCH_EN_MASK
#define SHIFT_VMCH_EN           PMIC_RG_LDO_VMCH_EN_SHIFT
#define FIELD_VMCH_EN           (MASK_VMCH_EN << SHIFT_VMCH_EN)

#define REG_VMCH_OC_RAW_STATUS      PMIC_RG_INT_RAW_STATUS_VMCH_OC_ADDR
#define MASK_VMCH_OC_RAW_STATUS     PMIC_RG_INT_RAW_STATUS_VMCH_OC_MASK
#define SHIFT_VMCH_OC_RAW_STATUS    PMIC_RG_INT_RAW_STATUS_VMCH_OC_SHIFT
#define FIELD_VMCH_OC_RAW_STATUS    (MASK_VMCH_OC_RAW_STATUS \
					<< SHIFT_VMCH_OC_RAW_STATUS)

#define REG_VMCH_OC_STATUS      PMIC_RG_INT_STATUS_VMCH_OC_ADDR
#define MASK_VMCH_OC_STATUS     PMIC_RG_INT_STATUS_VMCH_OC_MASK
#define SHIFT_VMCH_OC_STATUS    PMIC_RG_INT_STATUS_VMCH_OC_SHIFT
#define FIELD_VMCH_OC_STATUS    (MASK_VMCH_OC_STATUS << SHIFT_VMCH_OC_STATUS)

#define VEMC_VOSEL_CAL_mV(cal)  ((cal >= 0) ? ((cal)/10) : 0)
#define VEMC_VOSEL_2V9          (10)
#define VEMC_VOSEL_3V           (11)
#define VEMC_VOSEL_3V3          (13)
#define VMC_VOSEL_CAL_mV(cal)   ((cal >= 0) ? ((cal)/10) : 0)
#define VMC_VOSEL_1V8           (4)
#define VMC_VOSEL_2V9           (10)
#define VMC_VOSEL_3V            (11)
#define VMC_VOSEL_3V3           (13)
#define VMCH_VOSEL_CAL_mV(cal)  ((cal >= 0) ? ((cal)/10) : 0)
#define VMCH_VOSEL_2V9          (2)
#define VMCH_VOSEL_3V           (3)
#define VMCH_VOSEL_3V3          (5)
#endif


#define EMMC_VOL_ACTUAL         VOL_3000
#define SD_VOL_ACTUAL           VOL_3000


/**************************************************************/
/* Section 3: Clock                                           */
/**************************************************************/
#if !defined(FPGA_PLATFORM)
/* MSDCPLL register offset */
#define MSDCPLL_CON0_OFFSET     (0x260)
#define MSDCPLL_CON1_OFFSET     (0x264)
#define MSDCPLL_CON2_OFFSET     (0x268)
#define MSDCPLL_PWR_CON0_OFFSET (0x26C)
#endif

#define MSDCPLL_FREQ            400000000

#define MSDC0_SRC_0             26000000
#define MSDC0_SRC_1             MSDCPLL_FREQ
#define MSDC0_SRC_2             (MSDCPLL_FREQ/2)
#define MSDC0_SRC_3             156000000
#define MSDC0_SRC_4             182000000
#define MSDC0_SRC_5             312000000

#define MSDC1_SRC_0             26000000
#define MSDC1_SRC_1             208000000
#define MSDC1_SRC_2             182000000
#define MSDC1_SRC_3             156000000
#define MSDC1_SRC_4             (MSDCPLL_FREQ/2)

#define MSDC_SRC_FPGA           12000000

/**************************************************************/
/* Section 4: GPIO and Pad                                    */
/**************************************************************/
/*--------------------------------------------------------------------------*/
/* MSDC0~1 GPIO and IO Pad Configuration Base                               */
/*--------------------------------------------------------------------------*/
/* 0x10005000 */
#define MSDC_GPIO_BASE              gpio_base
/*0x11F3_0000*/
#define MSDC0_IO_PAD_BASE           (msdc_io_cfg_bases[0])
/*0x11E2_0000*/
#define MSDC1_IO_PAD_BASE           (msdc_io_cfg_bases[1])

/*--------------------------------------------------------------------------*/
/* MSDC GPIO Related Register                                               */
/*--------------------------------------------------------------------------*/
/* MSDC0 */
#define MSDC0_GPIO_MODE8            (MSDC_GPIO_BASE + 0x0380)
#define MSDC0_GPIO_MODE9            (MSDC_GPIO_BASE + 0x0390)
#define MSDC0_GPIO_DRV_CFG0         (MSDC0_IO_PAD_BASE + 0x0000)
#define MSDC0_GPIO_DRV_CFG1         (MSDC0_IO_PAD_BASE + 0x0010)
#define MSDC0_GPIO_IES_CFG0         (MSDC0_IO_PAD_BASE + 0x0020)
#define MSDC0_GPIO_PUPD_CFG0        (MSDC0_IO_PAD_BASE + 0x0030)
#define MSDC0_GPIO_R0_CFG0          (MSDC0_IO_PAD_BASE + 0x0040)
#define MSDC0_GPIO_R1_CFG0          (MSDC0_IO_PAD_BASE + 0x0050)
#define MSDC0_GPIO_RDSEL_CFG0       (MSDC0_IO_PAD_BASE + 0x0060)
#define MSDC0_GPIO_RDSEL_CFG1       (MSDC0_IO_PAD_BASE + 0x0070)
#define MSDC0_GPIO_RDSEL_CFG2       (MSDC0_IO_PAD_BASE + 0x0080)
#define MSDC0_GPIO_SMT_CFG0         (MSDC0_IO_PAD_BASE + 0x0090)
#define MSDC0_GPIO_TDSEL_CFG0       (MSDC0_IO_PAD_BASE + 0x00a0)
#define MSDC0_GPIO_TDSEL_CFG1       (MSDC0_IO_PAD_BASE + 0x00b0)

/* MSDC1 */
#define MSDC1_GPIO_MODE10           (MSDC_GPIO_BASE + 0x03A0)
#define MSDC1_GPIO_MODE11           (MSDC_GPIO_BASE + 0x03B0)
#define MSDC1_GPIO_DRV_CFG0         (MSDC1_IO_PAD_BASE + 0x0000)
#define MSDC1_GPIO_IES_CFG0         (MSDC1_IO_PAD_BASE + 0x0010)
#define MSDC1_GPIO_PUPD_CFG0        (MSDC1_IO_PAD_BASE + 0x0030)
#define MSDC1_GPIO_R0_CFG0          (MSDC1_IO_PAD_BASE + 0x0040)
#define MSDC1_GPIO_R1_CFG0          (MSDC1_IO_PAD_BASE + 0x0050)
#define MSDC1_GPIO_RDSEL_CFG0       (MSDC1_IO_PAD_BASE + 0x0060)
#define MSDC1_GPIO_RDSEL_CFG1       (MSDC1_IO_PAD_BASE + 0x0070)
#define MSDC1_GPIO_SMT_CFG0         (MSDC1_IO_PAD_BASE + 0x0080)
#define MSDC1_GPIO_TDSEL_CFG0       (MSDC1_IO_PAD_BASE + 0x00A0)


/*
 * MSDC0 GPIO and PAD register and bitfields definition
 */
/* MSDC0_GPIO_MODE8 mask, 0b001 is msdc mode */
#define MSDC0_MODE_DSL_MASK         (0x7 << 16)
#define MSDC0_MODE_CLK_MASK         (0x7 << 20)
#define MSDC0_MODE_CMD_MASK         (0x7 << 24)
#define MSDC0_MODE_RSTB_MASK        (0x7 << 28)
/* MSDC0_GPIO_MODE9 mask, 0b001 is msdc mode */
#define MSDC0_MODE_DAT0_MASK        (0x7 << 0)
#define MSDC0_MODE_DAT1_MASK        (0x7 << 4)
#define MSDC0_MODE_DAT2_MASK        (0x7 << 8)
#define MSDC0_MODE_DAT3_MASK        (0x7 << 12)
#define MSDC0_MODE_DAT4_MASK        (0x7 << 16)
#define MSDC0_MODE_DAT5_MASK        (0x7 << 20)
#define MSDC0_MODE_DAT6_MASK        (0x7 << 24)
#define MSDC0_MODE_DAT7_MASK        (0x7 << 28)

/* MSDC0_GPIO_IES_CFG0 mask */
#define MSDC0_IES_CLK_MASK          (0x1 << 0)
#define MSDC0_IES_CMD_MASK          (0x1 << 1)
#define MSDC0_IES_DAT0_MASK         (0x1 << 2)
#define MSDC0_IES_DAT1_MASK         (0x1 << 3)
#define MSDC0_IES_DAT2_MASK         (0x1 << 4)
#define MSDC0_IES_DAT3_MASK         (0x1 << 5)
#define MSDC0_IES_DAT4_MASK         (0x1 << 6)
#define MSDC0_IES_DAT5_MASK         (0x1 << 7)
#define MSDC0_IES_DAT6_MASK         (0x1 << 8)
#define MSDC0_IES_DAT7_MASK         (0x1 << 9)
#define MSDC0_IES_DSL_MASK          (0x1 << 10)
#define MSDC0_IES_RSTB_MASK         (0x1 << 11)
#define MSDC0_IES_DAT_MASK          (0xFF << 2)
#define MSDC0_IES_ALL_MASK          (0x7FF << 0)

/* MSDC0_GPIO_SMT_CFG0 mask */
#define MSDC0_SMT_CLK_MASK          (0x1 << 0)
#define MSDC0_SMT_CMD_MASK          (0x1 << 1)
#define MSDC0_SMT_DAT0_MASK         (0x1 << 2)
#define MSDC0_SMT_DAT1_MASK         (0x1 << 3)
#define MSDC0_SMT_DAT2_MASK         (0x1 << 4)
#define MSDC0_SMT_DAT3_MASK         (0x1 << 5)
#define MSDC0_SMT_DAT4_MASK         (0x1 << 6)
#define MSDC0_SMT_DAT5_MASK         (0x1 << 7)
#define MSDC0_SMT_DAT6_MASK         (0x1 << 8)
#define MSDC0_SMT_DAT7_MASK         (0x1 << 9)
#define MSDC0_SMT_DSL_MASK          (0x1 << 10)
#define MSDC0_SMT_RSTB_MASK         (0x1 << 11)
#define MSDC0_SMT_DAT_MASK          (0xFF << 2)
#define MSDC0_SMT_ALL_MASK          (0x7FF << 0)

/* MSDC0_GPIO_TDSEL_CFG0 mask */
#define MSDC0_TDSEL_CLK_MASK        (0xF << 0)
#define MSDC0_TDSEL_CMD_MASK        (0xF << 4)
#define MSDC0_TDSEL_DAT0_MASK       (0xF << 8)
#define MSDC0_TDSEL_DAT1_MASK       (0xF << 12)
#define MSDC0_TDSEL_DAT2_MASK       (0xF << 16)
#define MSDC0_TDSEL_DAT3_MASK       (0xF << 20)
#define MSDC0_TDSEL_DAT4_MASK       (0xF << 24)
#define MSDC0_TDSEL_DAT5_MASK       (0xF << 28)
#define MSDC0_TDSEL_DAT_MASK0       (0xFFFFFF << 8)
#define MSDC0_TDSEL_ALL_MASK0       (0xFFFFFFFF << 0)
/* MSDC0_GPIO_TDSEL_CFG1 mask */
#define MSDC0_TDSEL_DAT6_MASK       (0xF << 0)
#define MSDC0_TDSEL_DAT7_MASK       (0xF << 4)
#define MSDC0_TDSEL_DSL_MASK        (0xF << 8)
#define MSDC0_TDSEL_RSTB_MASK       (0xF << 12)
#define MSDC0_TDSEL_DAT_MASK1       (0xFF << 0)
#define MSDC0_TDSEL_ALL_MASK1       (0xFFF << 0)

/* MSDC0_GPIO_RDSEL_CFG0 mask */
#define MSDC0_RDSEL_CLK_MASK        (0x3F << 0)
#define MSDC0_RDSEL_CMD_MASK        (0x3F << 6)
#define MSDC0_RDSEL_DAT0_MASK       (0x3F << 12)
#define MSDC0_RDSEL_DAT1_MASK       (0x3F << 18)
#define MSDC0_RDSEL_DAT2_MASK       (0x3F << 24)
#define MSDC0_RDSEL_DAT_MASK0       (0x3FFFF << 12)
#define MSDC0_RDSEL_ALL_MASK0       (0x3FFFFFFF << 0)
/* MSDC0_GPIO_RDSEL_CFG1 mask */
#define MSDC0_RDSEL_DAT3_MASK       (0x3F << 0)
#define MSDC0_RDSEL_DAT4_MASK       (0x3F << 6)
#define MSDC0_RDSEL_DAT5_MASK       (0x3F << 12)
#define MSDC0_RDSEL_DAT6_MASK       (0x3F << 18)
#define MSDC0_RDSEL_DAT7_MASK       (0x3F << 24)
#define MSDC0_RDSEL_DAT_MASK1       (0x3FFFFFFF << 0)
#define MSDC0_RDSEL_ALL_MASK1       (0x3FFFFFFF << 0)
/* MSDC0_GPIO_RDSEL_CFG2 mask */
#define MSDC0_RDSEL_DSL_MASK        (0x3F << 0)
#define MSDC0_RDSEL_RSTB_MASK       (0x3F << 6)
#define MSDC0_RDSEL_ALL_MASK2       (0x3F << 0)

/* MSDC0_GPIO_DRV_CFG0 mask */
#define MSDC0_DRV_CLK_MASK          (0x7 << 0)
#define MSDC0_DRV_CMD_MASK          (0x7 << 3)
#define MSDC0_DRV_DAT0_MASK         (0x7 << 6)
#define MSDC0_DRV_DAT1_MASK         (0x7 << 9)
#define MSDC0_DRV_DAT2_MASK         (0x7 << 12)
#define MSDC0_DRV_DAT3_MASK         (0x7 << 15)
#define MSDC0_DRV_DAT4_MASK         (0x7 << 18)
#define MSDC0_DRV_DAT5_MASK         (0x7 << 21)
#define MSDC0_DRV_DAT6_MASK         (0x7 << 24)
#define MSDC0_DRV_DAT7_MASK         (0x7 << 27)
#define MSDC0_DRV_DAT_MASK          (0xFFFFFF << 6)
#define MSDC0_DRV_ALL_MASK1         (0x3FFFFFFF << 0)
/* MSDC0_GPIO_DRV_CFG1 mask */
#define MSDC0_DRV_DSL_MASK          (0x7 << 0)
#define MSDC0_DRV_RSTB_MASK         (0x7 << 3)
#define MSDC0_DRV_ALL_MASK2         (0x7 << 0)

/* MSDC0_GPIO_PUPD_CFG0 mask */
#define MSDC0_PUPD_CLK_MASK         (0x1 << 0)
#define MSDC0_PUPD_CMD_MASK         (0x1 << 1)
#define MSDC0_PUPD_DAT0_MASK        (0x1 << 2)
#define MSDC0_PUPD_DAT1_MASK        (0x1 << 3)
#define MSDC0_PUPD_DAT2_MASK        (0x1 << 4)
#define MSDC0_PUPD_DAT3_MASK        (0x1 << 5)
#define MSDC0_PUPD_DAT4_MASK        (0x1 << 6)
#define MSDC0_PUPD_DAT5_MASK        (0x1 << 7)
#define MSDC0_PUPD_DAT6_MASK        (0x1 << 8)
#define MSDC0_PUPD_DAT7_MASK        (0x1 << 9)
#define MSDC0_PUPD_DSL_MASK         (0x1 << 10)
#define MSDC0_PUPD_RSTB_MASK        (0x1 << 11)
#define MSDC0_PUPD_DAT_MASK         (0xFF << 2)
#define MSDC0_PUPD_ALL_MASK         (0x7FF << 0)

/* MSDC0_GPIO_R0_CFG0 mask */
#define MSDC0_R0_CLK_MASK           (0x1 << 0)
#define MSDC0_R0_CMD_MASK           (0x1 << 1)
#define MSDC0_R0_DAT0_MASK          (0x1 << 2)
#define MSDC0_R0_DAT1_MASK          (0x1 << 3)
#define MSDC0_R0_DAT2_MASK          (0x1 << 4)
#define MSDC0_R0_DAT3_MASK          (0x1 << 5)
#define MSDC0_R0_DAT4_MASK          (0x1 << 6)
#define MSDC0_R0_DAT5_MASK          (0x1 << 7)
#define MSDC0_R0_DAT6_MASK          (0x1 << 8)
#define MSDC0_R0_DAT7_MASK          (0x1 << 9)
#define MSDC0_R0_DSL_MASK           (0x1 << 10)
#define MSDC0_R0_RSTB_MASK          (0x1 << 11)
#define MSDC0_R0_DAT_MASK           (0xFF << 2)
#define MSDC0_R0_ALL_MASK           (0x7FF << 0)

/* MSDC0_GPIO_R1_CFG0 mask */
#define MSDC0_R1_CLK_MASK           (0x1 << 0)
#define MSDC0_R1_CMD_MASK           (0x1 << 1)
#define MSDC0_R1_DAT0_MASK          (0x1 << 2)
#define MSDC0_R1_DAT1_MASK          (0x1 << 3)
#define MSDC0_R1_DAT2_MASK          (0x1 << 4)
#define MSDC0_R1_DAT3_MASK          (0x1 << 5)
#define MSDC0_R1_DAT4_MASK          (0x1 << 6)
#define MSDC0_R1_DAT5_MASK          (0x1 << 7)
#define MSDC0_R1_DAT6_MASK          (0x1 << 8)
#define MSDC0_R1_DAT7_MASK          (0x1 << 9)
#define MSDC0_R1_DSL_MASK           (0x1 << 10)
#define MSDC0_R1_RSTB_MASK          (0x1 << 11)
#define MSDC0_R1_DAT_MASK           (0xFF << 2)
#define MSDC0_R1_ALL_MASK           (0x7FF << 0)

/*
 * MSDC1 GPIO and PAD register and bitfields definition
 */
/* MSDC1_GPIO_MODE10 mask, 0b001 is msdc mode */
#define MSDC1_MODE_CLK_MASK         (0x7 << 20)
#define MSDC1_MODE_CMD_MASK         (0x7 << 24)
#define MSDC1_MODE_DAT0_MASK        (0x7 << 28)
/* MSDC1_GPIO_MODE11 mask, 0b001 is msdc mode */
#define MSDC1_MODE_DAT1_MASK        (0x7 << 0)
#define MSDC1_MODE_DAT2_MASK        (0x7 << 4)
#define MSDC1_MODE_DAT3_MASK        (0x7 << 8)

/* MSDC1_GPIO_IES_CFG0 mask */
#define MSDC1_IES_CLK_MASK          (0x1 << 0)
#define MSDC1_IES_CMD_MASK          (0x1 << 1)
#define MSDC1_IES_DAT0_MASK         (0x1 << 2)
#define MSDC1_IES_DAT1_MASK         (0x1 << 3)
#define MSDC1_IES_DAT2_MASK         (0x1 << 4)
#define MSDC1_IES_DAT3_MASK         (0x1 << 5)
#define MSDC1_IES_DAT_MASK          (0xF << 2)
#define MSDC1_IES_ALL_MASK          (0x3F << 0)

/* MSDC1_GPIO_SMT_CFG0 mask */
#define MSDC1_SMT_CLK_MASK          (0x1 <<  0)
#define MSDC1_SMT_CMD_MASK          (0x1 <<  1)
#define MSDC1_SMT_DAT0_MASK         (0x1 <<  2)
#define MSDC1_SMT_DAT1_MASK         (0x1 <<  3)
#define MSDC1_SMT_DAT2_MASK         (0x1 <<  4)
#define MSDC1_SMT_DAT3_MASK         (0x1 <<  5)
#define MSDC1_SMT_DAT_MASK          (0xF <<  2)
#define MSDC1_SMT_ALL_MASK          (0x3F << 0)

/* MSDC1_GPIO_TDSEL_CFG0 mask */
#define MSDC1_TDSEL_CLK_MASK        (0xF << 0)
#define MSDC1_TDSEL_CMD_MASK        (0xF << 4)
#define MSDC1_TDSEL_DAT0_MASK       (0xF << 8)
#define MSDC1_TDSEL_DAT1_MASK       (0xF << 12)
#define MSDC1_TDSEL_DAT2_MASK       (0xF << 16)
#define MSDC1_TDSEL_DAT3_MASK       (0xF << 20)
#define MSDC1_TDSEL_DAT_MASK        (0xFFFF << 8)
#define MSDC1_TDSEL_ALL_MASK        (0xFFFFFF << 0)

/* MSDC1_GPIO_RDSEL_CFG0 mask */
#define MSDC1_RDSEL_CLK_MASK        (0x3F << 0)
#define MSDC1_RDSEL_CMD_MASK        (0x3F << 6)
#define MSDC1_RDSEL_DAT0_MASK       (0x3F << 12)
#define MSDC1_RDSEL_DAT1_MASK       (0x3F << 18)
#define MSDC1_RDSEL_DAT2_MASK       (0x3F << 24)
#define MSDC1_RDSEL_DAT_MASK0       (0x3FFFF << 12)
#define MSDC1_RDSEL_ALL_MASK0       (0x3FFFFFFF << 0)
/* MSDC1_GPIO_RDSEL_CFG0 mask */
#define MSDC1_RDSEL_DAT3_MASK       (0x3F << 0)
#define MSDC1_RDSEL_DAT_MASK1       (0x3F << 0)
#define MSDC1_RDSEL_ALL_MASK1       (0x3F << 0)

/* MSDC1_GPIO_DRV_CFG0 mask */
#define MSDC1_DRV_CLK_MASK         (0x7 << 0)
#define MSDC1_DRV_CMD_MASK         (0x7 << 3)
#define MSDC1_DRV_DAT0_MASK        (0x7 << 6)
#define MSDC1_DRV_DAT1_MASK        (0x7 << 9)
#define MSDC1_DRV_DAT2_MASK        (0x7 << 12)
#define MSDC1_DRV_DAT3_MASK        (0x7 << 15)
#define MSDC1_DRV_DAT_MASK         (0xFFF << 6)
#define MSDC1_DRV_ALL_MASK         (0x3FFFF << 0)

/* MSDC1_GPIO_PUPD_CFG0 mask */
#define MSDC1_PUPD_CLK_MASK         (0x1 << 0)
#define MSDC1_PUPD_CMD_MASK         (0x1 << 1)
#define MSDC1_PUPD_DAT0_MASK        (0x1 << 2)
#define MSDC1_PUPD_DAT1_MASK        (0x1 << 3)
#define MSDC1_PUPD_DAT2_MASK        (0x1 << 4)
#define MSDC1_PUPD_DAT3_MASK        (0x1 << 5)
#define MSDC1_PUPD_DAT_MASK         (0xF << 2)
#define MSDC1_PUPD_ALL_MASK         (0x3F << 0)

/* MSDC1_GPIO_R0_CFG0 mask */
#define MSDC1_R0_CLK_MASK           (0x1 << 0)
#define MSDC1_R0_CMD_MASK           (0x1 << 1)
#define MSDC1_R0_DAT0_MASK          (0x1 << 2)
#define MSDC1_R0_DAT1_MASK          (0x1 << 3)
#define MSDC1_R0_DAT2_MASK          (0x1 << 4)
#define MSDC1_R0_DAT3_MASK          (0x1 << 5)
#define MSDC1_R0_DAT_MASK           (0xF << 2)
#define MSDC1_R0_ALL_MASK           (0x3F << 0)

/* MSDC1_GPIO_R1_CFG0 mask */
#define MSDC1_R1_CLK_MASK           (0x1 << 0)
#define MSDC1_R1_CMD_MASK           (0x1 << 1)
#define MSDC1_R1_DAT0_MASK          (0x1 << 2)
#define MSDC1_R1_DAT1_MASK          (0x1 << 3)
#define MSDC1_R1_DAT2_MASK          (0x1 << 4)
#define MSDC1_R1_DAT3_MASK          (0x1 << 5)
#define MSDC1_R1_DAT_MASK           (0xF << 2)
#define MSDC1_R1_ALL_MASK           (0x3F << 0)

/* FOR msdc_io_check() */
#define MSDC1_PUPD_DAT0_ADDR        MSDC1_GPIO_PUPD_CFG0
#define MSDC1_PUPD_DAT1_ADDR        MSDC1_GPIO_PUPD_CFG0
#define MSDC1_PUPD_DAT2_ADDR        MSDC1_GPIO_PUPD_CFG0
#define MSDC1_R0_DAT0_ADDR          MSDC1_GPIO_R0_CFG0
#define MSDC1_R0_DAT1_ADDR          MSDC1_GPIO_R0_CFG0
#define MSDC1_R0_DAT2_ADDR          MSDC1_GPIO_R0_CFG0
#define MSDC1_R1_DAT0_ADDR          MSDC1_GPIO_R1_CFG0
#define MSDC1_R1_DAT1_ADDR          MSDC1_GPIO_R1_CFG0
#define MSDC1_R1_DAT2_ADDR          MSDC1_GPIO_R1_CFG0
#define MSDC1_PU                    (0)
#define MSDC1_PD                    (1)
#define MSDC1_8K                    (1)


/**************************************************************/
/* Section 5: Adjustable Driver Parameter                     */
/**************************************************************/
#define HOST_MAX_BLKSZ			(2048)

#define MSDC_OCR_AVAIL\
	(MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 \
	| MMC_VDD_31_32 | MMC_VDD_32_33)
/* data timeout counter. 1048576 * 3 sclk. */
#define DEFAULT_DTOC			(3)

#define MAX_DMA_CNT				(4 * 1024 * 1024)
/* a WIFI transaction may be 50K */
#define MAX_DMA_CNT_SDIO		(0xFFFFFFFF - 255)
/* a LTE  transaction may be 128K */

#define MAX_HW_SGMTS			(MAX_BD_NUM)
#define MAX_PHY_SGMTS			(MAX_BD_NUM)
#define MAX_SGMT_SZ				(MAX_DMA_CNT)
#define MAX_SGMT_SZ_SDIO		(MAX_DMA_CNT_SDIO)

#define HOST_MAX_NUM			(2)
#ifdef CONFIG_PWR_LOSS_MTK_TEST
#define MAX_REQ_SZ              (512 * 65536)
#else
#define MAX_REQ_SZ              (512 * 1024)
#endif

#ifdef FPGA_PLATFORM
#define HOST_MAX_MCLK           (200000000)
#define HOST_MIN_MCLK           (100000)
#else
#define HOST_MAX_MCLK           (200000000)
#define HOST_MIN_MCLK           (260000)
#endif


/* SD card, bad card handling settings */

/* if continuous data timeout reach the limit */
/* driver will force remove card */
#define MSDC_MAX_DATA_TIMEOUT_CONTINUOUS (100)

/* if continuous power cycle fail reach the limit */
/* driver will force remove card */
#define MSDC_MAX_POWER_CYCLE_FAIL_CONTINUOUS (3)

/* sdcard esd recovery */
/* power reset sdcard when sdcard hang from esd */
#define SDCARD_ESD_RECOVERY

/* #define MSDC_HQA */
/* #define SDIO_HQA */

/* sd read/write crc error happen in mt6885 when vcore changes,
 * sd can't support autok merge by fix vcore(like emmc),
 * so add runtime autok merge function
 */
#define SD_RUNTIME_AUTOK_MERGE

/* fix vcore in kernel will affect other module,
 * use emmc runtime autok merge intead of the original emmc autok
 */
#define EMMC_RUNTIME_AUTOK_MERGE

/* nano memory card support flag by platform */
//#define NMCARD_SUPPORT

/* support new tx */
#define SUPPORT_NEW_TX

/**************************************************************/
/* Section 6: BBChip-depenent Tunnig Parameter                */
/**************************************************************/
#define EMMC_MAX_FREQ_DIV               4 /* lower frequence to 12.5M */
#define MSDC_CLKTXDLY                   0

#define MSDC0_DDR50_DDRCKD              1 /* FIX ME: may be removed */

#define VOL_CHG_CNT_DEFAULT_VAL         0x1F4 /* =500 */

#define MSDC_PB0_DEFAULT_VAL			0x403C0007
#define MSDC_PB1_DEFAULT_VAL            0xFFE64309


#define MSDC_PB2_DEFAULT_RESPWAITCNT    0x3
#define MSDC_PB2_DEFAULT_RESPSTENSEL    0x1
#define MSDC_PB2_DEFAULT_CRCSTSENSEL    0x1

#endif /* _MSDC_CUST_MT6873_H_ */
