// ****************************************************************************
// FILENAME    RX200def.h CONTENTS: This is a 90nm(RC03F) FLASH module define program for the 4000,5000 test system COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED 2008/11/01  rev.00  1'st created by H.Kobori
// ****************************************************************************
#ifndef DEF_H
#define DEF_H

#include "RC04EX_pro.h"

 // **************** TESTER ****************
#define CPUFAIL   -100
// ** CPU Error Fail **
#define RAMFAIL   -200
// ** RAM tansfer Fail **
#define FLGFAIL   -300
// ** Flag Error Fail **
#define NOPATFAIL -400
// ** File Open Fail **
#define HWFAIL    -500
// ** Tester resouce Fail **
#define TOVER     -600
// ** Tester resouce Fail **
#define TRIM_FAIL -700
#define REG_FAIL  -800
#define P_FAIL    -900

#define RAM16KB   0x4000

// detalog array element number
#define LOGPARAM_MAX  500

// CPU CHECK CODE
#define CHECK_CODE 0x23

// IV-SA SELECT FLAG
#define IVSA_NONE 0
#define IVSA_1UA  1
#define IVSA_3UA  2
#define IVSA_5UA  3
#define IVSA_8UA  4
#define IVSA_10UA  5
#define IVSA_13UA  6
#define IVSA_15UA  7
#define IVSA_18UA  8
#define IVSA_20UA  9
#define IVSA_23UA  10
#define IVSA_30UA  11
#define IVSA_40UA  12
#define IVSA_50UA  13
#define IVSA_70UA  14
#define IVSA_HARF_I  15


 // **************** BOARD & Latch RelayON/OFF 0xffff = {lb7,lb6...lb0} ****************
#define RVCLMON   0xfff0
// relay all off & SW VCL monitor

// Add 2018/0306 RC04X relay
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
#define RNORMAL        0xfff8
#define RVSSMON        0xfff9
#define RVSSMON_20     0xfffd
#define RVCCMON        0xfff8
#define RVRSGMON       0xfffa
#define RVSSMON_CH     0xfff8
#define RNORMAL_CRS    0xfff0
#define RVSSMON_CRS    0xfff1
#define RVSSMON_20_CRS 0xfff5
#define RVCCMON_CRS    0xfff0
#define RVRSGMON_CRS   0xfff2
#define RVSSMON_CH_CRS 0xfff0
#elif ( PRODUCT_NAME==PRODUCT_IO )
#define RNORMAL        0xfff0
#define RVSSMON        0xfff1
#define RVSSMON_20     0xfff5
#define RVCCMON        0xfff0
#define RVRSGMON       0xfff2
#define RVSSMON_CH     0xfff0
#define RNORMAL_CRS    0xfff8
#define RVSSMON_CRS    0xfff9
#define RVSSMON_20_CRS 0xfffd
#define RVCCMON_CRS    0xfff8
#define RVRSGMON_CRS   0xfffa
#define RVSSMON_CH_CRS 0xfff8
#endif


 // ********************** CpuMode~~Repeat(bit32-0) ***********************
 
#define INITIAL_SET        0x00000001
#define INIF_SET           0x00000010
#define RAMBOOT_CHANGE     0x00000100
#define OUTIF_READ         0x00001000
#define END_SET            0x10000000

 // ***************************** Minori Read-Current Mode *****************************
#define READ_ON            0x00000001
#define READ_OFF           0x00000002
#define WHILE1_ALL0        0x00000010
#define WHILE1_ALL1        0x00000020
#define NORMAL_READ        0x00000040
#define RANDOM_READ        0x00000080


 // **************** VOLTAGE MODE(bit32-0) ****************
  //bit16 crystal
  //bit17 apply
  //bit18 monitor

  //bit0  normal
  //bit1  vssmon
  //bit2  vccmon
  //bit3  vrsgmon
  //bit4  vssmon_delay
  //bit5  vssmon_20k
#define MNORMAL         0x00000001
// normal mode
#define MVSSMON         0x00000002
// vssmon mode
#define MVCCMON         0x00000004
// vccmon mode
#define MVRSGMON        0x00000008
// vrsgmon mode
#define MVSSMON_CH      0x00000010
// vssmon DelayCheck mode
#define MVSSMON_20K     0x00000020
// vssmon 20K ohrm mode
#define MNORMAL_CRY     0x00010001
// normal mode(crystal)

// Add 2018/0306
#define MVSSMON_20      0x00000020
#define MNORMAL_CRS     0x00010001
#define MVSSMON_CRS     0x00010002
#define MVSSMON_20_CRS  0x00010020
#define MVCCMON_CRS     0x00010004
#define MVRSGMON_CRS    0x00010008
#define MVSSMON_CH_CRS  0x00010010
// Add 2018/0306
#define MVCLMON         0x00000040
// monitor VCL1,VCL2

#define CRYSTAL_MODE    0x00010000
// CRYSTAL MODE
#define APPLY_MODE      0x00020000
// APPLY   MODE
#define MONITOR_MODE    0x00040000
// MONITOR MODE
#define CP_SUPPLY_MODE  0x00080000
// CP current monitor MODE
#define CP_LEAK_MODE    0x00100000
// CP leak monitor Mode


#define  VSSMON_APP      APPLY_MODE   | MVSSMON
#define  VSSMON_20K_APP  APPLY_MODE   | MVSSMON_20K
#define  VCCMON_APP      APPLY_MODE   | MVCCMON
#define  VRSGMON_APP     APPLY_MODE   | MVRSGMON
#define  VSSMON_CH_APP   APPLY_MODE   | MVSSMON_CH
#define  MNORMAL_APP     APPLY_MODE   | MNORMAL
// Delay, OSC trimming

#define  VSSMON_MON      MONITOR_MODE | MVSSMON
#define  VSSMON_20K_MON  MONITOR_MODE | MVSSMON_20K
#define  VCCMON_MON      MONITOR_MODE | MVCCMON
#define  VRSGMON_MON     MONITOR_MODE | MVRSGMON
#define  VSSMON_CH_MON   MONITOR_MODE | MVSSMON_CH
#define  VCLMON_MON      MONITOR_MODE | MVCLMON

#define  VSSMON_CPSUP      CP_SUPPLY_MODE | MVSSMON
#define  VSSMON_20K_CPSUP  CP_SUPPLY_MODE | MVSSMON_20K
#define  VCCMON_CPSUP      CP_SUPPLY_MODE | MVCCMON
#define  VRSGMON_CPSUP     CP_SUPPLY_MODE | MVRSGMON
#define  VSSMON_CH_CPSUP   CP_SUPPLY_MODE | MVSSMON_CH

#define  VSSMON_CPLK       CP_LEAK_MODE | MVSSMON
#define  VSSMON_20K_CPLK   CP_LEAK_MODE | MVSSMON_20K
#define  VCCMON_CPLK       CP_LEAK_MODE | MVCCMON
#define  VRSGMON_CPLK      CP_LEAK_MODE | MVRSGMON
#define  VSSMON_CH_CPLK    CP_LEAK_MODE | MVSSMON_CH

#define  VSSMON_APP_CRY      VSSMON_APPLY        | CRYSTAL_MODE
#define  VSSMON_20K_APP_CRY  VSSMON_20K_APPLY    | CRYSTAL_MODE
#define  VCCMON_APP_CRY      VCCMON_APPLY        | CRYSTAL_MODE
#define  VRSGMON_APP_CRY     VRSGMON_APPLY       | CRYSTAL_MODE
#define  VSSMON_CH_APP_CRY   VSSMON_CH_APPLY     | CRYSTAL_MODE
#define  VSSMON_MON_CRY      VSSMON_MONITOR      | CRYSTAL_MODE
#define  VSSMON_20K_MON_CRY  VSSMON_20K_MONITOR  | CRYSTAL_MODE
#define  VCCMON_MON_CRY      VCCMON_MONITOR      | CRYSTAL_MODE
#define  VRSGMON_MON_CRY     VRSGMON_MONITOR     | CRYSTAL_MODE
#define  VSSMON_CH_MON_CRY   VSSMON_CH_MONITOR   | CRYSTAL_MODE


 // ***************************** TESTSEL DEFINE *****************************


#define AUTOTRIM_VPP_E              0x00
#define AUTOTRIM_VPP_P              0x01
#define AUTOTRIM_VPP_PW             0x02
#define AUTOTRIM_VHH_E              0x03
#define AUTOTRIM_VHH_P              0x04
#define AUTOTRIM_VRSG               0x05
#define AUTOTRIM_VREF10             0x06
#define AUTOTRIM_VWI                0x07
#define AUTOTRIM_VDEMG              0x08
// `(g~O)
#define AUTOTRIM_V33R               0x09
#define AUTOTRIM_VNOEMI             0x0A
#define AUTOTRIM_IMONI_I01U_NOTEMP  0x0B
#define AUTOTRIM_IMONI_FLASH_NOTEMP 0x0C
#define AUTOTRIM_IMONI_I01U_TEMP    0x0D
#define AUTOTRIM_IMONI_FLASH_TEMP   0x0E
#define AUTOTRIM_READDELAY8         0x0F
#define AUTOTRIM_READDELAY10        0x10
#define AUTOTRIM_VNOEMI_PE          0x0A
#define AUTOTRIM_VNOEMI_R           0x12
#define AUTOTRIM_OSC32              0x14
#define AUTOTRIM_OSC1               0x15
#define AUTOTRIM_VPP_TF             0x1E
// VPP_TF͊̈pending
#define AUTOTRIM_BGR08              0x30
// ܂dlȂpending
#define AUTOTRIM_VDD                0x31
// ܂dlȂpending
#define AUTOTRIM_VDDH               0x32
// ܂dlȂpending

#define MONITOR_VPP_E               0x00
#define MONITOR_VPP_P               0x01
#define MONITOR_VPP_PW              0x02
#define MONITOR_VHH_E               0x03
#define MONITOR_VHH_P               0x04
#define MONITOR_VRSG                0x05
#define MONITOR_VREG20              0x06
// `(j^)
#define MONITOR_VWI                 0x07
#define MONITOR_VDEMG               0x08
#define MONITOR_V33R                0x09
// 6->9ɕύX
#define MONITOR_VNOEMI              0x0A
#define MONITOR_IMONI_I01U_NOTEMP   0x0B
#define MONITOR_IMONI_FLASH_NOTEMP  0x0C
#define MONITOR_IMONI_I01U_TEMP     0x0D
#define MONITOR_IMONI_FLASH_TEMP    0x0E
#define MONITOR_READDELAY8          0x0F
#define MONITOR_READDELAY10         0x10
#define MONITOR_VNOEMI_PE           0x0A
#define MONITOR_VNOEMI_R            0x12
#define MONITOR_OSC32               0x14
#define MONITOR_OSC1                0x15
#define MONITOR_VDD                 0x16
#define MONITOR_VDDH                0x17
#define MONITOR_BGR                 0x18
#define MONITOR_VREF10              0x19

#define  VTHREAD_DIS                0x23
#define  VTHREAD_EDGE               0x24


 // **************** OERATION MODE(bit13-0) ****************
#define MSNORMAL   0
#define MSDISTRI   1
// Get Distribution
#define MSFBM      2
// Get FBM
#define MSBOOT     3
// Boot W/R
#define MSRANDOM  10
// All Area W/R
#define MSDATA_ALIGN  23
// Random Data ALIGN
#define MSBGO     30
// BGO
#define MSRAMALL  35
// All RAM
#define BM_LCHKA1  0x40
// Logical Checker 1Byte-A
#define BM_LCHKB1  0x41
// Logical Checker 1Byte-B
#define BM_LCHKA2  0x42
// Logical Checker 2Byte-A
#define BM_LCHKB2  0x43
// Logical Checker 2Byte-B
#define BM_LCHKA4  0x44
// Logical Checker 4Byte-A
#define BM_LCHKB4  0x45
// Logical Checker 4Byte-B
#define LOG_WRITE 0x01000000
// Program Count info output
#define LOG_ERASE 0x02000000
// Erase Count info output


 // ****************** param Display MODE ******************
#define PDIN_REG    0x01
// IF̃WX^o
#define PDIN_BDATA  0x02
// IFBDATAo
#define PDIN_LDATA  0x04
// IFLDATAo
#define PDIN_OTHER  0x08
// IF̂̑o
#define PDOUT_REG   0x10
// oIF̃WX^o
#define PDOUT_BDATA 0x20
// oIFBDATAo
#define PDOUT_LDATA 0x40
// oIFLDATAo
#define PDOUT_OTHER 0x80
// oIF̂̑o

#define PDIN_ALL    0x0F
// IFׂ̂Ăo
#define PDOUT_ALL   0xF0
// oIFׂ̂Ăo
#define PDALL       0xFF
// Paramׂ̂Ăo

#define NO_BMECR    0x00
// BM,ECȐo͖
#define DP_BM       0x01
// BM̂ݏo
#define DP_ECR      0x02
// ECR̂ݏo
#define DP_BM_ECR   0x03
// BM,ECRo

 // **************** CHECKER SELECT FLAG ****************
#define CHK_NONE    0
#define CHK_A       1
#define CHK_B       2

// *************************************************
// Module SELECT
// *************************************************
#define FLP0          0x00
#define FLI1          0x01
#define FLI2          0x02
#define EXTRA1        0x03
#define EXTRA2        0x04
#define EXTRA3        0x05
#define EXTRA4        0x06
#define EXTRA5        0x07
#define EXTRA6        0x08

#define W_MAT          0x06
// EXTRA5

#define CODE_FLASH     0x00
#define DATA_FLASH     0x01



// set reg data flag
#define CHIPDATA  0x00
// ** RAM of R-Mat data Transfer Reg **
#define NO_REG    0x00
#define TRIM_REG  0x01
#define TEST_REG  0x02

// #define COND_REG  0x04

// set tap data flag
#define TAP_OFF     0x00
// ** TAPCTL_OFF **
#define TAP_ON      0x01
// ** TAPCTL_ON  **

// set pll pattern
#define PLL_X1    1
#define PLL_X2    2
#define PLL_X4    4
#define PLL_X6    6
#define PLL_X8    8
#define PLL_X10   10
#define PLL_X16   16

// FBM target
#define FBM_FLASH        0x01
#define FBM_RAM          0x03

 // ********************** Current monitor Func **********************
#define ADC_DIGI      0x00000001
#define ADC_AVE       0x00000002
#define CRY_CLK       0x00000100
#define DPASS_COMP    0x00001000
#define PIN_JUMP      0x00100000





 // **************** DEVICE ****************

#define YDECNUM     8
// y_decoder number
#define WORD_END    ((1<<YDECNUM)-1)
// 0xFF

// --------- Vccmax limiter ----------------------------
#define WAR_MAX_VCC       7000
// limit vcc
#define WAR_MAX_VDD        900
// limit vdd
#define WAR_MAX_VDDH       900
// limit vddh
#define WAR_MIN_VCC       1250
// limit vcc
#define WAR_MIN_VDD        200
// limit vdd
#define WAR_MIN_VDDH       200
// limit vddh


// --------- RAM Top&End address --------------------------------
#define RAM_TOP     0x00000000
// RAM start address
#define RAM_END     RAM_SIZE-1
// RAM end address
#define RAM_OFFSET  0x20000000

#define FBMFILENAME "X%03dY%03d_%s_%s_W%02d_P1_no_T000_%s_%d_%d_%d_%s_%s_000.bin"
// ͗pFBMt@C`

#define FBM_TOP     0x00101880
// FBM start address
#define FBM_END     0x00401880
// FBM start address
#define DATA_PAT_ECR_TOP 0x4000
// Random data ECR Load address( data.pat offset )
#define FCUFIRM_SIZE     0x1BFF
// FCU Firm size
#define DISPATCH_SIZE    0x2000
// fBXpb`̈TCY
#define WMAT_STRUCT_SIZE  0x200
// WmatʎYgp̈TCY

#define SUSPENDLOG_1ST_TOP ( 0x00001A00 )
// !< TXyhOf[^(1)擪AhX
#define SUSPENDLOG_2ND_TOP ( 0x00001C00 )
// !< TXyhOf[^(2)擪AhX
#define SUSPENDLOG_3RD_TOP ( 0x00001E00 )
// !< TXyhOf[^(3)擪AhX
#define FCUERASELOG_EBMAX_TOP ( 0x00001880 )
// !< FCUO(ubNTCYMAX)擪AhX
#define FCUERASELOG_EBMIN_TOP ( 0x000018A0 )
// !< TXyhOf[^(1)擪AhX


// ****************************
// linked with HEW src define
// ****************************

#define FBM_READSIZE      0x1000
// analyzepattern.h
#define FBM_READTOP       0x2000
#define FBM_READSIZE2      0x2800
// readdump_RAM1_ALL
#define FBM_READTOP2       0x1000
#define RANDOMDATA_SIZE   0x2000
// INPUTIF_TOP > RANDOMDATA_TOP+RANDOMDATA_SIZE
#define FCU_TRANSSIZE     0x2000
// productparameter.h
#define BISTDATA_SIZE     0x1000
// massproduction.h
// ** RAM Map Address Define **
#define INPUTIF_TOP       0x00003E00
// testcond.h / massproduction.h
#define OUTPUTIF_TOP      0x00003F00
// testres.h / massproduction.h
#define BISTOUTPUTIF_TOP  0x00003D00

#define INPUTIF_END      INPUTIF_TOP+0x8F
// testcond.h / massproduction.h
#define OUTPUTIF_END     OUTPUTIF_TOP+0x5F
// testres.h  / massproduction.h
#define BISTOUTPUTIF_END BISTOUTPUTIF_TOP+0xFF
#define DISTRIBUTION_TOP 0x00001A00
// testres.h
#define DISTRIBUTION_END 0x00001C3F
// DISTRIBUTION_TOP+144*4

#define MINORI_BM_TOP_ADDR       0x00000000
// BM Address For MINORI
#define MINORI_BM_END_ADDR       MINORI_BM_TOP_ADDR + 0x00000000
// BM Address For MINORI
#define MINORI_BM_TOP_ADDR_MAIN  0x00000000
// BM Address For MINORI
#define MINORI_BM_END_ADDR_MAIN  MINORI_BM_TOP_ADDR_MAIN + 0x000003FF
// BM Address For MINORI
#define MINORI_BM_TOP_ADDR_OUT   0x00010000
// BM Address For MINORI
#define MINORI_BM_END_ADDR_OUT   MINORI_BM_TOP_ADDR_OUT + 0x000001FF
// BM Address For MINORI


// *********************************************************
// Monitor define
// *********************************************************

#define V_BGR08               2
#define V_VDD                 3
#define V_VDDH                4
#define V_VREF10              5
#define V_VPP_E               7
#define V_VPP_P               8
#define V_VPP_PW              9
#define V_VHH_E              21
#define V_VHH_P              22
#define V_VRSG               30
#define V_V33R               31
#define V_VWI                32
#define V_VDEMG              33
#define V_VREG20             34
#define V_VNOEMI             14
#define V_VNOEMI_PE          36
#define V_VNOEMI_R           37
#define I_IMONI_I01U_NOTEMP  40
#define I_IMONI_FLASH_NOTEMP 41
#define I_IMONI_I01U_TEMP    42
#define I_IMONI_FLASH_TEMP   43
#define T_OSC32              50
#define T_OSC1               51
#define T_READDELAY8         60
#define T_READDELAY10        61
#define T_READDELAY8_FLP0    62
#define T_READDELAY8_FLI1    63
#define T_READDELAY8_FLI2    64
#define T_READDELAY10_FLP0   65
#define T_READDELAY10_FLI1   66
#define T_READDELAY10_FLI2   67
#define V_VPP_TF             70
#define V_VHH_E_TEMP         221
// d(Vhh)x⏞ 721221
#define V_VHH_P_TEMP         222
// d(Vhh)x⏞ 722222
#define V_VHH_P_TEMPV        223
// d(Vhh)x⏞d⏞
#define I_IMONI_TEMP         243
// d(Flash)x⏞ 743243
#define V_VHH_E_LEAK 121
#define V_VHH_P_LEAK 122

 // **************** SETTING ****************
#define INIT_COND   0x10000
#define INIT_FFPT   1
// print&disp=2 disp=1 noout=0
#define INIT_VIH_RELAY 5000
#define INIT_WAIT   30
#define MIN_VDD     0x0E
#define MAX_VDD     0x07
#define NUM_POWERSUPPLY 3
#define NUM_CHIPINFO    32
#define VTH_FREQ    100
#define EXTRA5_TEMP_ON  1234
#define EXTRA5_TEMP_OFF    0

#define VDD_DELTA   3
#define VDDH_DELTA  4


 // ******************* BGR WT Setting *******************
#define BGR_ARRAY_VCC     5
#define BGR_ARRAY_REPEAT 10
#define VCC_ARRAY0   1400
#define VCC_ARRAY1   1600
#define VCC_ARRAY2   2800
#define VCC_ARRAY3   3300
#define VCC_ARRAY4   3600
#define WS1LOT_NUM   4


 // ****************************************************************************** ex Manu Define *****************************************************************************
#define BWT_FL0  0x2000
#define BWT_FL1  0x2001
#define BWT_FL2  0x2002
#define BWT_FL4  0x2004
#define BWT_FL8  0x2008
#define BWT_FL9  0x2009
#define BFT_FL1   0x2011
#define BFT_FL2   0x2012
#define BWT_FL0D  0x3000
#define BWT_FL1D  0x3001
#define BWT_FL2D  0x3002
#define BWT_FL4D  0x3004
#define BWT_FL8D  0x3008
#define BWT_FL9D  0x3009
#define BFT_FL1D  0x3011
#define BFT_FL2D  0x3012
#define BGR_WT1   0x3020
#define BGR_WT2   0x3030
#define BGR_WT4   0x3040
#define BGR_WT5   0x3050
#define QT_SNO    0x3060
#define WT_F_WS   0x3070
#define VTHTRIM_WT 0x4000
#define AE_WT     0x4100
#define AE_WT2    0x4102

// **** Fbm Log bit define ****
#define CHIP        0x00080000


// --------- define wait time control ---------------------------
#define W1SEC       5
// wait time 1sec(10ms x 100)
#define W2SEC       W1SEC*2
// wait time 2sec(10ms x 200)
#define W3SEC       W1SEC*3
// wait time 3sec(10ms x 300)
#define W5SEC       W1SEC*5
// wait time 5sec(10ms x 500)
#define W10SEC      W1SEC*10
// wait time 10sec(10ms x 1000)
#define W30SEC      W1SEC*30
// wait time 30sec(10ms x 3000)
#define W1MIN       W1SEC*60
// wait time 1min(10ms x 6000)
#define W2MIN       W1MIN*2
// wait time 1min(10ms x 12000)
#define W10MIN      W1MIN*10
// wait time 1min(10ms x 60000)
#define LIMIT16     65000
// max H'FFFF=65535*10ms
#define INFINI      9999999
// wait time infinite
#define TM_SET_NS_MAX   20000

// ***** Cpu Test Flag *****
// NotInclude Flag that CPU drive recklessly
#define CPASS       0
// select pass at cpu mode
#define CFAIL       0xFFFF
// select fail at cpu mode
#define	READFAIL	0x0006
#define CFLAG       2
// select flag errer at cpu mode
#define EXIT        10001
#define QUIT        10002
#define FAIL_TAP_TOP  3
#define FAIL_TAP_END  4


// wafer test
#define L_FAIL         3
#define H_FAIL         4
#define TESTERR_FAIL   5
#define RAM_FAIL       6
#define TIME_OUT_FAIL  7
#define ILLEGAL_FAIL   8
#define SKIP_PASS      9


#ifndef EOF
#define EOF         -1
#endif
#ifndef NIL
#define NIL         0
#endif

// --------- Number data set ------------------------------------------
#define SIZE_B      1
#define SIZE_W      2
#define SIZE_L      4


#define SET_CVCC    0x80010
// set vcc
#define SET_CVDDH   0x80011
#define SET_CVDD    0x80012
#define SET_VDDHT   0x80013
#define SET_VDDT    0x80014
#define SET_CVMAX   0x80015
#define SET_CVMIN   0x80016
#define SET_CVTYP   0x80017
#define SET_VCC_MODE    0x80020
// set vccmode
#define SET_LOG     0x80040
// set log file flag
#define SET_REG     0x80060
// set reg
#define SET_INDATA  0x80070
// set in bdata , in ldata
#define SET_PLL     0x80080
// set PLL
#define SET_CLKFCG  0x80090
// set CG(clkf)

#define SET_PERCR   0x80100
// set PERCR
#define SET_LK      0x80200
// set LK bit
#define SET_DATA    0x80220
// set read pat1 flag
#define SET_MODULE  0x80250
// set MODULE caution!!
#define SET_PAGE    0x80300
// set PAGE flag
#define SET_WSIZE   0x80310
// set Write Size
#define SET_LPREAD  0x80320
// set LowPowerRead
#define SET_CLK     0x80330
// set CLK mode
#define SET_MICRO   0x80340
// set CLK mode
#define SET_LOOP     0x80350
// set CLK mode
#define SET_FSTOP   0x80400
// set fail stop
#define SET_ADR     0x80410
// set FLTR2 reg.
#define SET_PRCNT   0x80420
// set loop
#define SET_ERCNT   0x80430
// set write overwrite pulse 1'st
#define SET_MDL     0x80500
// set make file
#define SET_SPEED   0x80510
// set speed
#define SET_WAIT    0x80520
// set wait
#define SET_CONFIG  0x80530
// set configration
#define SET_VSTEP   0x80540
// set Vth-Read Step
#define SET_MON     0x80550
// set Monitor  Step
#define SET_PULS    0x80560
// set Puls Step
#define SET_IREF    0x80570
// set Iref Step
#define SET_MUL     0x80580
// set Multi Select
#define SET_TAP     0x80590
// set Multi Select
#define SET_EDGE    0x80600
// set Multi Select
#define SAVE_REG    0x80600
// Save Register
#define LOAD_REG    0x80610
// Load Register
#define SET_BI      0x80700
// set BI-Mode select
#define SET_BGR     0x80710
// set BI-Mode select
#define SET_CUT     0x80720
// set BI-Mode select

#define SET_BIVDDSEL            0x80800
// BIE/VDDSELE/VDDSEL4-0

#define SET_SECT    0x80900
// set security test

// --------- Global define selection ----------------------------------
#ifndef FALSE
#define FALSE           0
// init false
#endif

#ifndef TURE
#define TURE            1
// init ture
#endif



// --------- ECR MODE DEFINE ------------------------------------------
#define ECR_OVERW       0
// ECR mode set up  (Over Write Mode)
#define ECR_OVERL       1
// ECR mode set up  (Over Lay Mode)
#define ECR_CLEAR       2
// ECR mode set up  (Clear Mode)
#define ECR_DISAB       3
// ECR mode set up  (Analysis Mode)

// --------- OTHER DEFINE ---------------------------------------------
#define EXT_DATA0       1
// dummy data 0
#define EXT_DATA1       1
// dummy data 1
#define EXT_DATA2       1
// dummy data 2
#define EXT_DATA3       1
// dummy data 3
#define EXT_DATA4       1
// dummy data 4
#define EXT_DATA5       1
// dummy data 5
#define EXT_DATA6       1
// dummy data 6
#define EXT_DATA7       1
// dummy data 7
#define EXT_DATA8       1
// dummy data 8
#define EXT_DATA9       1
// dummy data 9
#define EXT_DATA10      1
// dummy data 10
#define EXT_DATA11      1
// dummy data 11
#define EXT_DATA12      1
// dummy data 12
#define EXT_DATA13      1
// dummy data 13
#define EXT_DATA14      1
// dummy data 14
#define EXT_DATA15      1
// dummy data 15
#define EXT_DATA16      1
// dummy data 16
#define EXT_DATA17      1
// dummy data 17
#define EXT_DATA18      1
// dummy data 18
#define EXT_DATA19      1
// dummy data 19
#define EXT_DATA20      1
// dummy data 20
#define EXT_DATA21      1
// dummy data 21
#define EXT_DATA22      1
// dummy data 22
#define EXT_DATA23      1
// dummy data 23
#define EXT_DATA24      1
// dummy data 24
#define EXT_DATA25      1
// dummy data 25
#define EXT_DATA26      1
// dummy data 26
#define EXT_DATA27      1
// dummy data 27
#define EXT_DATA28      1
// dummy data 28
#define EXT_DATA29      1
// dummy data 29
#define EXT_DATA30      1
// dummy data 30
#define EXT_ME_VREF_FLAG     0x00
// dummy ModeEntry Vref Flag 0x00:ExVrefOFF

// *** PIN define for C file ****
#define CH0     100,0,0,0
#define CH1     101,1,0,0
#define CH2     102,2,0,0
#define CH3     103,3,0,0
#define CH4     104,4,0,0
#define CH5     105,5,0,0
#define CH6     106,6,0,0
#define CH7     107,7,0,0
#define CH8     108,8,0,0
#define CH9     109,9,0,0
#define CH10    110,10,0,0
#define CH11    111,11,0,0
#define CH12    112,12,0,0
#define CH13    113,13,0,0
#define CH14    114,14,0,0
#define CH15    115,15,0,0
#define CH16    116,16,0,0
#define CH17    117,17,0,0
#define CH18    118,18,0,0
#define CH19    119,19,0,0
#define CH20    120,20,0,0
#define CH21    121,21,0,0
#define CH22    122,22,0,0
#define CH23    123,23,0,0
#define CH24    124,24,0,0
#define CH25    125,25,0,0
#define CH26    126,26,0,0
#define CH27    127,27,0,0
#define CH28    128,28,0,0
#define CH29    129,29,0,0
#define CH30    130,30,0,0
#define CH31    131,31,0,0
#define CH32    132,32,0,0
#define CH33    133,33,0,0
#define CH34    134,34,0,0
#define CH35    135,35,0,0
#define CH36    136,36,0,0
#define CH37    137,37,0,0
#define CH38    138,38,0,0
#define CH39    139,39,0,0
#define CH40    140,40,0,0
#define CH41    141,41,0,0
#define CH42    142,42,0,0
#define CH43    143,43,0,0
#define CH44    144,44,0,0
#define CH45    145,45,0,0
#define CH46    146,46,0,0
#define CH47    147,47,0,0
#define CH48    148,48,0,0
#define CH49    149,49,0,0
#define CH50    150,50,0,0
#define CH51    151,51,0,0
#define CH52    152,52,0,0
#define CH53    153,53,0,0
#define CH54    154,54,0,0
#define CH55    155,55,0,0
#define CH56    156,56,0,0
#define CH57    157,57,0,0
#define CH58    158,58,0,0
#define CH59    159,59,0,0
#define CH60    160,60,0,0
#define CH61    161,61,0,0
#define CH62    162,62,0,0
#define CH63    163,63,0,0

#ifdef V5KONLY
#define V1      200,PPS_CON_VCC1_A,0,0
#define V2      300,PPS_CON_VCC1_B,0,0
//#define V2      300,PPS_CON_VPP1,0,0  // ??? caution!!
#define V3      400,PPS_CON_VPP1,0,0
//#define V3      400,PPS_CON_VCC1_A,0,0    // PPS_SITE 1 ??? caution!!
#define V4      500,PPS_CON_VNN1,0,0
// ??? caution!!
//#define V4      500,PPS_CON_VPP1,0,0  // PPS_SITE 1 ??? caution!!
#define V5      600,PPS_CON_VNN1,0,0
// ??? caution!!
#define RATE_MIN_LIMIT  20
#endif
#ifdef V4KONLY
#define V1      200,-1,0,0
#define V2      300,-2,0,0
#define V3      400,-3,0,0
#define V4      500,-8,0,0
#define V5      600,-5,0,0
// ??? caution!!
#define RATE_MIN_LIMIT  10
#endif


#define ENDLST  0,0,0,0
// *** PIN define for APG file ****
#define P_CH0   0
#define P_CH1   1
#define P_CH2   2
#define P_CH3   3
#define P_CH4   4
#define P_CH5   5
#define P_CH6   6
#define P_CH7   7
#define P_CH8   8
#define P_CH9   9
#define P_CH10  10
#define P_CH11  11
#define P_CH12  12
#define P_CH13  13
#define P_CH14  14
#define P_CH15  15
#define P_CH16  16
#define P_CH17  17
#define P_CH18  18
#define P_CH19  19
#define P_CH20  20
#define P_CH21  21
#define P_CH22  22
#define P_CH23  23
#define P_CH24  24
#define P_CH25  25
#define P_CH26  26
#define P_CH27  27
#define P_CH28  28
#define P_CH29  29
#define P_CH30  30
#define P_CH31  31
#define P_CH32  32
#define P_CH33  33
#define P_CH34  34
#define P_CH35  35
#define P_CH36  36
#define P_CH37  37
#define P_CH38  38
#define P_CH39  39
#define P_CH40  40
#define P_CH41  41
#define P_CH42  42
#define P_CH43  43
#define P_CH44  44
#define P_CH45  45
#define P_CH46  46
#define P_CH47  47
#define P_CH48  48
#define P_CH49  49
#define P_CH50  50
#define P_CH51  51
#define P_CH52  52
#define P_CH53  53
#define P_CH54  54
#define P_CH55  55
#define P_CH56  56
#define P_CH57  57
#define P_CH58  58
#define P_CH59  59
#define P_CH60  60
#define P_CH61  61
#define P_CH62  62
#define P_CH63  63

 // ****************************************************************************** Function Key Define *****************************************************************************
#define  FK_DLOG_DEBUG         20
// WT/AT̊eeXgsʃt@C   \PROBE\BIST_WT%d_RESULTR_%04d%02d%02d_%d.csv  \PROBE\DebugLogAT%d_%s_%s_%04d%02d%02d_%d.csv
#define  FK_DISP_DEBUG         21
// vOfobOpprintf\ #define  FK_WT_LOGIC           22    // ʎYWT ̎QƃtH_WTLOGIC֕ύX #define  FK_NOSTOP_FAIL        23    // tFCĂXgbvȂ #define  FK_BREAK_KEY          24    // ʎYWT eɃL[{[h͗LΒf #define  FK_DOUT_RD_RAM        25    // ʎYWT u[gRAM_vAhXg\ #define  FK_DOUT_DISP          26    // ʎYWT RAM_vf[^2s #define  FK_VF_TRANS           27    // ʎYWT ]`FbN #define  FK_INPUT_SCOPE        30    // fobOŃIVg`擾 u[gOG^[L[҂XgbvgK   extern char ivsa_string[16][5];
extern char mat_string[9][8];
extern const char str_ivsa[16][5];
extern const char str_area[3][5];
extern const char str_posinega[3][7];
extern const char str_relief[3][8];
extern char mat_module[9];
extern char readpatname[3][16];
extern char readparamname[3][16];
extern char voltage_string[145][8];
extern int mat_size[9];
extern const char vthpoint_string[11][8];
extern char info_extra[7][14];
extern char ws1lot_string[WS1LOT_NUM][6];



#endif
