/******************************************************************************/
/*
  FILENAME    RX200pro.h
  CONTENTS: This is a 0.13um(RC01SN) FLASH module define program for the
            4000,5000 test system
  COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED
  2011/08/29  rev.00  1'st created by K.Watanabe
*/
/******************************************************************************/
#ifndef PRO_H
#define PRO_H

/*----------------------------------------------------------------------------*/
/* 製品定義                                                                   */
/*----------------------------------------------------------------------------*/
#define PRODUCT_EUROPA 1
#define PRODUCT_IO 2

#define PRODUCT_NAME  PRODUCT_EUROPA
//#define PRODUCT_NAME  PRODUCT_IO

/*----------------------------------------------------------------------------*/
/* 製品変更時の書き換え必須項目                                               */
/*----------------------------------------------------------------------------*/
#define CURRENTDR          "E:\\SOTB"                     /* パタンその他を格納するルートフォルダパス */
#define CURRENTDR_D_DRIVE  "D:\\SOTB"                     /* V5000-3/4 ルートフォルダパス */
#define DEVICENAME         "RC04EX_EUROPA"                /* デバイス名 16文字以内 ***.pln ファイルのDEVICE_TYPEも同値に変更 */
#define PROGRAMNAME        "RC04EX Rev.0.0"
#define RAM_SIZE    0x00004000                        /* 0番地から始まる RAMの使用サイズ *//* SRAM0: 2000 0000 ～ 2000 7FFFh  SRAM1: 2000 8000 ～ 2003 FFFFh */
#define INIT_VCC    3300                              /* 製品VCC仕様          */
#define INIT_VREF    0
#define INIT_VREFH   0
#define INIT_RELAY  5000     /* Relayの入力電圧レベル */


/*----------------------------------------------------------------------------*/
/* 製品動作条件用確認                                                         */
/*----------------------------------------------------------------------------*/
#define NORMAL_EXTAL 78                 /* 通常使用するEXTALのサイクル時間(ns)*/
#define NORMAL_PLL    8                 /* 通常使用するPLLの倍率              */
#define NORMAL_ICLK_DIVIDE_PLLON 2      /* 通常使用するICLKの分周率           */
#define NORMAL_FCLK_DIVIDE_PLLON 4      /* 通常使用するFCLKの分周率           */
#define PLL_MOUNT     1                 /* PLL回路搭載=1  非搭載=0            */

/*----------------------------------------------------------------------------*/
/* FLIの搭載枚数が同じならば大概書き換え不要                                  */
/*----------------------------------------------------------------------------*/
#define FLI_TOTALCOUNT 3                /* FLP/FLIモジュールの搭載数          */
#define FLP0_TOP    0x00000000          /* FLP0の先頭アドレス                 */
#define FLP0_END    0x0007FFFF          /* FLP0の最終アドレス                 */
#define FLP0_SIZE   0x00080000          /* FLP0の合計メモリサイズ             */
#define FLI1_TOP    0x00000000          /* FLI1の先頭アドレス                 */
#define FLI1_END    0x0007FFFF          /* FLI1の最終アドレス                 */
#define FLI1_SIZE   0x00080000          /* FLI1の合計メモリサイズ             */
#define FLI2_TOP    0x00000000          /* FLI2の先頭アドレス                 */
#define FLI2_END    0x0007FFFF          /* FLI2の最終アドレス                 */
#define FLI2_SIZE   0x00080000          /* FLI2のメモリサイズ                 */

/*----------------------------------------------------------------------------*/
/* オートトリミングのターゲット電圧 基本変更不要                              */
/*----------------------------------------------------------------------------*/
#define TARGET_BGR08_25C      800       /*                                    */
#define TARGET_VDD_25C        790       /*                                    */
#define TARGET_VDDH_25C      1250       /*                                    */
#define TARGET_VREF10_25C    1000       /*                                    */
#define TARGET_VPP_E_25C    -2500       /*                                    */
#define TARGET_VPP_P_25C    -3000       /*                                    */
#define TARGET_VPP_PW_25C   -2000       /*                                    */
#define TARGET_VHH_E_25C     7500       /*                                    */
#define TARGET_VHH_P_25C     7500       /*                                    */
#define TARGET_VRSG_25C      3400       /*                                    */
#define TARGET_V33R_25C      3300       /*                                    */
#define TARGET_VWI_25C       2500       /*                                    */
#define TARGET_VDEMG_25C     -600       /*                                    */
#define TARGET_VNOEMI_25C    3600       /*                                    */

#define TARGET_IREF_25C      7350       /* Iref=2.5uA Target (2.5uAx3-0.15(adjust)    */
#define TARGET_IREF_F_25C    7350       /* Iref=2.5uA Target (2.5uAx3-0.15(adjust)    */
#define TARGET_IREF_T_25C    7350       /* Iref=2.5uA Target (2.5uAx3-0.15(adjust)    */
#define TARGET_IREF_T_F_25C  7350       /* Iref=2.5uA Target (2.5uAx3-0.15(adjust)    */

#define TARGET_ROSC_PH   3205ul         /* ROSC_ChgPump                       */
#define TARGET_ROSC_LOG  3205ul         /* ROSC_Logic                         */
#define TARGET_ROSC_SUB  9866ul         /* ROSC_SubACT                        */

#define TARGET_OSC32          270       /* 270us=2160cycle                    */
#define TARGET_OSC1           100       /* 100us= 100cycle                    */
#define TARGET_DELAY8_25C     114       /*                                    */
#define TARGET_DELAY10_25C     95       /*                                    */
#define TARGET_VPP_TF_25C     100       /* 100~200us                          */

/*----------------------------------------------------------------------------*/
/* その他DAXS転送など  基本変更不要                                           */
/*----------------------------------------------------------------------------*/
#define INIT_DAXS_WR_RATE  400   /* 2.5MHz */
#define INIT_DAXS_RD_RATE  400   /* 2.5MHz */

#define INIT_DAXS_VOH  INIT_VCC*3/10
#define INIT_DAXS_VTT  INIT_VCC*5/10  /* V5000 */
#define INIT_DAXS_VOL  INIT_VCC*3/10

#define MODEENTRY_SPEED   50   /* 20MHz */

#endif
