// ****************************************************************************
// FILENAME    RX200pro.h CONTENTS: This is a 0.13um(RC01SN) FLASH module define program for the 4000,5000 test system COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED 2011/08/29  rev.00  1'st created by K.Watanabe
// ****************************************************************************
#ifndef PRO_H
#define PRO_H

// ----------------------------------------------------------------------------
// i`
// ----------------------------------------------------------------------------
#define PRODUCT_EUROPA 1
#define PRODUCT_IO 2

#define PRODUCT_NAME  PRODUCT_EUROPA
//#define PRODUCT_NAME  PRODUCT_IO

// ----------------------------------------------------------------------------
// iύX̏K{
// ----------------------------------------------------------------------------
#define CURRENTDR          "E:\\SOTB"
// p^̑i[郋[gtH_pX
#define CURRENTDR_D_DRIVE  "D:\\SOTB"
// V5000-3/4 [gtH_pX
#define DEVICENAME         "RC04EX_EUROPA"
// foCX 16ȓ ***.pln t@CDEVICE_TYPElɕύX
#define PROGRAMNAME        "RC04EX Rev.0.0"
#define RAM_SIZE    0x00004000
// 0Ԓnn܂ RAM̎gpTCY
#define INIT_VCC    3300
// iVCCdl
#define INIT_VREF    0
#define INIT_VREFH   0
#define INIT_RELAY  5000
// Relay͓̓dx


// ----------------------------------------------------------------------------
// ipmF
// ----------------------------------------------------------------------------
#define NORMAL_EXTAL 78
// ʏgpEXTAL̃TCN(ns)
#define NORMAL_PLL    8
// ʏgpPLL̔{
#define NORMAL_ICLK_DIVIDE_PLLON 2
// ʏgpICLK̕
#define NORMAL_FCLK_DIVIDE_PLLON 4
// ʏgpFCLK̕
#define PLL_MOUNT     1
// PLLH=1  񓋍=0

// ----------------------------------------------------------------------------
// FLI̓ږȂΑTsv
// ----------------------------------------------------------------------------
#define FLI_TOTALCOUNT 3
// FLP/FLIW[̓ڐ
#define FLP0_TOP    0x00000000
// FLP0̐擪AhX
#define FLP0_END    0x0007FFFF
// FLP0̍ŏIAhX
#define FLP0_SIZE   0x00080000
// FLP0̍vTCY
#define FLI1_TOP    0x00000000
// FLI1̐擪AhX
#define FLI1_END    0x0007FFFF
// FLI1̍ŏIAhX
#define FLI1_SIZE   0x00080000
// FLI1̍vTCY
#define FLI2_TOP    0x00000000
// FLI2̐擪AhX
#define FLI2_END    0x0007FFFF
// FLI2̍ŏIAhX
#define FLI2_SIZE   0x00080000
// FLI2̃TCY

// ----------------------------------------------------------------------------
// I[gg~Õ^[Qbgd {ύXsv
// ----------------------------------------------------------------------------
#define TARGET_BGR08_25C      800

#define TARGET_VDD_25C        790

#define TARGET_VDDH_25C      1250

#define TARGET_VREF10_25C    1000

#define TARGET_VPP_E_25C    -2500

#define TARGET_VPP_P_25C    -3000

#define TARGET_VPP_PW_25C   -2000

#define TARGET_VHH_E_25C     7500

#define TARGET_VHH_P_25C     7500

#define TARGET_VRSG_25C      3400

#define TARGET_V33R_25C      3300

#define TARGET_VWI_25C       2500

#define TARGET_VDEMG_25C     -600

#define TARGET_VNOEMI_25C    3600


#define TARGET_IREF_25C      7350
// Iref=2.5uA Target (2.5uAx3-0.15(adjust)
#define TARGET_IREF_F_25C    7350
// Iref=2.5uA Target (2.5uAx3-0.15(adjust)
#define TARGET_IREF_T_25C    7350
// Iref=2.5uA Target (2.5uAx3-0.15(adjust)
#define TARGET_IREF_T_F_25C  7350
// Iref=2.5uA Target (2.5uAx3-0.15(adjust)

#define TARGET_ROSC_PH   3205ul
// ROSC_ChgPump
#define TARGET_ROSC_LOG  3205ul
// ROSC_Logic
#define TARGET_ROSC_SUB  9866ul
// ROSC_SubACT

#define TARGET_OSC32          270
// 270us=2160cycle
#define TARGET_OSC1           100
// 100us= 100cycle
#define TARGET_DELAY8_25C     114

#define TARGET_DELAY10_25C     95

#define TARGET_VPP_TF_25C     100
// 100~200us

// ----------------------------------------------------------------------------
// ̑DAXS]Ȃ  {ύXsv
// ----------------------------------------------------------------------------
#define INIT_DAXS_WR_RATE  400
// 2.5MHz
#define INIT_DAXS_RD_RATE  400
// 2.5MHz

#define INIT_DAXS_VOH  INIT_VCC*3/10
#define INIT_DAXS_VTT  INIT_VCC*5/10
// V5000
#define INIT_DAXS_VOL  INIT_VCC*3/10

#define MODEENTRY_SPEED   50
// 20MHz

#endif
