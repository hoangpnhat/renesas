/*****************************************************************************/
/*    FILENAME     FV2000.H                                                  */
/*                                                                           */
/*    CONTENTS     TESTER HARDWARE HEADER FILE                               */
/*                 FOR V2100                                                 */
/*                 COPYRIGHT 1996 HITACHI LTD , ALL RIGHT RESERVED           */
/*****************************************************************************/
#ifndef __V1000_H_
#define __V1000_H_
/* system defines */
#define NA2500      0   /* 2.5 uamp range for PMU */
#define UA25        1   /* 25 uamp range for PMU */
#define UA250       2   /* 250 uamp range for PMU */
#define UA2500      3   /* 2.5 mamp range for PMU */
#define MA25        4   /* 25 mamp range for PMU */
#define MA250       5   /* 250 mamp range for PMU */
#define PPMUA2      6
#define PPMUA20     7
#define PPMUA200    8
#define NA2500F     0x10
#define UA25F       0x11
#define UA250F      0x12
#define UA2500F     0x13
#define MA25F       0x14
#define MA250F      0x15
/****** ppmu macros and defines *********/
#define SET_PPMU_RANGE(pinlist, mode) set_ppmu_range(pinlist,mode);\
set_pmu_range(mode+6)
#define UA2_PPMU    0   /* 2 ua range for PPMUs */
#define UA20_PPMU   1   /* 20 ua range for PPMUs (really 10 ua now) */
#define UA200_PPMU  2   /* 200 ua range for PPMUs */
#define PPMU2LIM    1900    /* autorange limit for 2 ua = 1.9 ua */
#define PPMU20LIM   9500    /* autorange limit for 10 ua = 9.5 ua */
#define PPMU200LIM  190000  /* autorange limit for 200 ua = 190 ua */
/* Write ports for power supplies */
#define VCC_WR      0x3000
#define VPP_WR      0x3001
#define V3_WR       0x3002
#define VLIMIT_WR   0x3003
#define ILIMIT_WR   0x3004
#define VIH1_WR     0x3005
#define VIH2_WR     0x3006
#define VIL1_WR     0x3007
#define VIL2_WR     0x3008
#define VOH_WR      0x3009
#define VOL_WR      0x300A
#define VCOM_WR     0x300B
#define PPIPARH_WR  0x300C
#define PPIPARL_WR  0x300D
#define PVPAR_WR    0x300E
#define VHH_WR      0x300F
/* ADI Write ports for power supplies */
#define VCC_ADI_WR  0x3010
#define VPP_ADI_WR  0x3011
#define V3_ADI_WR   0x3012
#define VLIMIT_ADI_WR   0x3013
#define ILIMIT_ADI_WR   0x3014
#define VIH1_ADI_WR 0x3015
#define VIH2_ADI_WR 0x3016
#define VIL1_ADI_WR 0x3017
#define VIL2_ADI_WR 0x3018
#define VOH_ADI_WR  0x3019
#define VOL_ADI_WR  0x301A
#define VCOM_ADI_WR 0x301B
#define PPIPARH_ADI_WR  0x301C
#define PPIPARL_ADI_WR  0x301D
#define PVPAR_ADI_WR    0x301E
#define VHH_ADI_WR  0x301F
/* Read ports for power supplies */
#define VCC_RD      0x3000
#define VPP_RD      0x3001
#define V3_RD       0x3002
#define VLIMIT_RD   0x3003
#define ILIMIT_RD   0x3004
#define VIH1_RD     0x3005
#define VIH2_RD     0x3006
#define VIL1_RD     0x3007
#define VIL2_RD     0x3008
#define VOH_RD      0x3009
#define VOL_RD      0x300A
#define VCOM_RD     0x300B
#define PPIPARH_RD  0x300C
#define PPIPARL_RD  0x300D
#define PVPAR_RD    0x300E
#define VHH_RD      0x300F
/** macros to read power supply ports and return millivolts and ??amps **/
#define READ_VCC()  ((read_hardware_reg(VCC_RD)&0xfff)*10)
#define READ_VPP()  ((read_hardware_reg(VPP_RD)&0xfff)*10)
#define READ_V3()   ((read_hardware_reg(V3_RD)&0xfff)*10)
#define READ_VLIMIT()   ((read_hardware_reg(VLIMIT_RD)&0xfff)*10)
#define READ_ILIMIT()   ((read_hardware_reg(ILIMIT_RD)&0xfff)*10)
#define READ_VIH1() ((read_hardware_reg(VIH1_RD)&0xfff)*10)
#define READ_VIH2() ((read_hardware_reg(VIH2_RD)&0xfff)*10)
#define READ_VIL1() ((read_hardware_reg(VIL1_RD)&0xfff)*10)
#define READ_VIL2() ((read_hardware_reg(VIL2_RD)&0xfff)*10)
#define READ_VOH()  ((read_hardware_reg(VOH_RD)&0xfff)*10)
#define READ_VOL()  ((read_hardware_reg(VOL_RD)&0xfff)*10)
#define READ_VCOM() ((read_hardware_reg(VCOM_RD)&0xfff)*10)
#define READ_PPIPARH()  ((read_hardware_reg(PPIPARH_RD)&0xfff)*10)
#define READ_PPIPARL()  ((read_hardware_reg(PPIPARL_RD)&0xfff)*10)
#define READ_PVPAR()    ((read_hardware_reg(PVPAR_RD)&0xfff)*10)
#define READ_VHH()  ((read_hardware_reg(VHH_RD)&0xfff)*10)
/****************************************************************************/
/*** defines for ppmu current measurements. the ppmu works based on pass **/
/*** conditions     **/
#define PPI_PASS_GT(current)    set_ppiparh(current);set_ppiparl(-current)
#define PPI_PASS_LT(current)    set_ppiparl(current);set_ppiparh(-current)
#define PPI_PASS_WIN(lo,hi) set_ppiparl(lo);set_ppiparh(hi)
/****************************************************************************/
#define SET_IPARL(current)  set_ppiparl(current)
#define SET_IPARH(current)  set_ppiparh(current)
#define SET_VPAR(volt)      set_ppvpar(volt)
/************ defines for 96 pin mode***************************************/
#define SLV_SET_IPARL(current) set_ppiparl(current); slv_set_ppiparl(current)
#define SLV_SET_IPARH(current) set_ppiparh(current); slv_set_ppiparh(current)
#define SLV_SET_VPAR(volt)  set ppvpar(volt); slv_set_ppvpar(volt)
#define SLV_VLIMIT(volt)    set_vlimit(volt); slv_set_vlimit(volt)
#define SLV_ILIMIT(current) set_ilimit(current); slv_set_ilimit(current)
/****************************************************************************/
#define SET_VCC(volt)   set_v1(volt)
#define SET_VPP(volt)   set_v2(volt)
#define SET_VSB(volt)   slv_set_v1(volt)
#define SET_VLOAD(volt) set_vcom(volt)
#define SET_VCSL(volt) set_vil2(volt)   /* use vil2 and vih2 for chip sel */
#define SET_VCSH(volt) set_vih2(volt)
#define SET_VADDL(volt) set_vil1(volt)  /* use vil1 and vih1 for add + data */
#define SET_VADDH(volt) set_vih1(volt)
#define SET_VCSL_MASK(volt) set_vil2_mask(volt)
#define SET_VCSH_MASK(volt) set_vih2_mask(volt)
#define SET_VADDL_MASK(volt) set_vil1_mask(volt)
#define SET_VADDH_MASK(volt) set_vih1_mask(volt)
/************ defines for 96 pin mode***************************************/
#define SLV_SET_VCSL(volt) set_vil2(volt); slv_set_vil2(volt)
#define SLV_SET_VCSH(volt) set_vih2(volt); slv_set_vih2(volt)
#define SLV_SET_VADDL(volt) set_vil1(volt); slv_set_vil1(volt)
#define SLV_SET_VADDH(volt) set_vih1(volt); slv_set_vih1(volt)
#define SLV_SET_VCSL_MASK(xxx) set_vil2_mask(xxx)
#define SLV_SET_VCSH_MASK(xxx) set_vih2_mask(xxx)
#define SLV_SET_VADDL_MASK(xxx) set_vil1_mask(xxx)
#define SLV_SET_VADDH_MASK(xxx) set_vih1_mask(xxx)
/***************************************************************************/
#define PERRMASK    0x3107
#define X_COUNT_RD  0x4103
#define Y_COUNT_RD  0x4103
/** ecr ports **/
#define WR_ECR_DATA 0x600B
#define WR_ECR_CNT  0x600C
#define RD_ECR_DATA 0x600B
#define RD_ECR_ERR  0x6009
#define RD_ECR_CNT  0x600D
#define READ_ECR_COUNTER()  (read_hardware_reg(RD_ECR_CNT)&0xff)
#define READ_ECR_DATA()     (read_hardware_reg(RD_ECR_DATA)&0xff)
/** asr ports **/
#define WR_X_ASR    0x4024
#define RD_X_ASR    0x4022
#define WR_Y_ASR    0x4023
#define RD_Y_ASR    0x4024
/** APG generator, counter, and register ports **/
#define WR_Y_GEN    0x4145
#define WR_Y1_REG   0x4146
#define WR_Y2_REG   0x4147
#define RD_Y_GEN    0x4140
#define RD_Y1_REG   0x4143
#define RD_Y2_REG   0x4144
#define WR_X_GEN    0x4165
#define WR_X1_REG   0x4166
#define WR_X2_REG   0x4167
#define RD_X_GEN    0x4160
#define RD_X1_REG   0x4163
#define RD_X2_REG   0x4164
#define WR_D_GEN    0x4185
#define WR_D1_REG   0x4186
#define WR_D2_REG   0x4187
#define RD_D_GEN    0x4180
#define RD_D1_REG   0x4183
#define RD_D2_REG   0x4184
#define WR_A_CNT    0x41C5
#define WR_A1_REG   0x41C6
#define WR_A2_REG   0x41C7
#define RD_A_CNT    0x41C0
#define RD_A1_REG   0x41C3
#define RD_A2_REG   0x41C4
#define WR_B_CNT    0x41A5
#define WR_B1_REG   0x41A6
#define WR_B2_REG   0x41A7
#define RD_B_CNT    0x41A0
#define RD_B1_REG   0x41A3
#define RD_B2_REG   0x41A4
#define WR_C_CNT    0x41E5
#define WR_C1_REG   0x41E6
#define WR_C2_REG   0x41E7
#define RD_C_CNT    0x41E0
#define RD_C1_REG   0x41E3
#define RD_C2_REG   0x41E4
/** macros to read counters, generators, and registers **/
#define READ_X_GEN()    (read_hardware_reg(RD_X_GEN)&0xffff)
#define READ_X1_REG()   (read_hardware_reg(RD_X1_REG)&0xffff)
#define READ_X2_REG()   (read_hardware_reg(RD_X2_REG)&0xffff)
#define READ_Y_GEN()    (read_hardware_reg(RD_Y_GEN)&0xffff)
#define READ_Y1_REG()   (read_hardware_reg(RD_Y1_REG)&0xffff)
#define READ_Y2_REG()   (read_hardware_reg(RD_Y2_REG)&0xffff)
#define READ_D_GEN()    (read_hardware_reg(RD_D_GEN)&0xffff)
#define READ_D1_REG()   (read_hardware_reg(RD_D1_REG)&0xffff)
#define READ_D2_REG()   (read_hardware_reg(RD_D2_REG)&0xffff)
#define READ_A_CNT()    (read_hardware_reg(RD_A_CNT)&0xffff)
#define READ_A1_REG()   (read_hardware_reg(RD_A1_REG)&0xffff)
#define READ_A2_REG()   (read_hardware_reg(RD_A2_REG)&0xffff)
#define READ_B_CNT()    (read_hardware_reg(RD_B_CNT)&0xffff)
#define READ_B1_REG()   (read_hardware_reg(RD_B1_REG)&0xffff)
#define READ_B2_REG()   (read_hardware_reg(RD_B2_REG)&0xffff)
#define READ_C_CNT()    (read_hardware_reg(RD_C_CNT)&0xffff)
#define READ_C1_REG()   (read_hardware_reg(RD_C1_REG)&0xffff)
#define READ_C2_REG()   (read_hardware_reg(RD_C2_REG)&0xffff)
#define RD_UADDR     0x4101
#define RD_NXTUINST  0x4003
#define READ_APG_ADDR() read_hardware_reg(RD_UADDR)
#define CONT_APG()  run_apg_abs(read_hardware_reg(RD_UADDR)+1)
#define PMU_FILTER_ON() write_hardware_reg(0x3106,(read_shadow_ram(0x3106) & 0xfd))
#define PMU_OFF()   write_hardware_reg(0x3106,0xff)
/** adc read source selects **/
#define ADC_20V      0
#define ADC_15V      1
#define ADC_5V       2
#define ADC_VIH1     3
#define ADC_VIH2     4
#define ADC_VIL1     5
#define ADC_VIL2     6
#define ADC_99_17V   7
#define ADC_VCOM     8
#define ADC_PVPAR    9
#define ADC_VHH     0xa
#define ADC_V1S     0xb
#define ADC_V2S     0xc
#define ADC_VSENSE  0x0d
#define ADC_PMUH    0xe
#define ADC_V3S     0x10
#define ADC_IMEAS   0x11
#define ADC_PIPARH  0x12
#define ADC_PIPARL  0x13
#define ADC_VOH     0x14
#define ADC_VOL     0x15
#define ADC_99_10V  0x16
#define ADC_M99_10V 0x17
#define ADC_NIST_10V    0x18
#define ADC_M15V    0x19
#define ADC_M10V    0x1a
#define ADC_PPIMEAS 0x1B
#define ADC_IOL     0x1c
#define ADC_IOH     0x1d
#define ADC_IDAC    0x1e
#define ADC_VDAC    0x20
#define ADC_NIST_17V    0x24
#define ADC_TEMP    0x26
#define ADC_24V     0x27
#define NO_ARG      0   /* no argument */
#define GT_IPARH    0x8 /* pmu fail current greater than iparh */
#define LT_IPARL    0x4 /* pmu fail current less than iparl */
#define LT_VPAR  0x10   /* pmu fail voltage less than vpar */
#define GT_VPAR  0x20   /* pmu fail voltage greater than vpar */
/*****  #defines for ECR routines                   ******/
#define OVERWRITE   0
#define OVERLAY  1
#define CLEAR       2
#define DISABLE  3
#define ON      1
#define OFF     0
#define NO_MASK  0
#define CLEAR_MASK  0xffff
#define WIDTH_16    0xffff
#define WIDTH_8  0x00ff
/***************************************************************************/
/*****  other useful equates for v1000              ******/
#define PAR_FAILS   0x2
#define FUNCT_FAILS 0x4
#define PASS        0   /* indicates a passing die for a function */
#define FAIL        -1  /* hard fail, something unexpected occured */
#define NOTPASS  1  /* indicates a failing die for a function */
#define FALSE       0
#define TRUE        1
/** #define EMPTY       -1  **/
#endif
