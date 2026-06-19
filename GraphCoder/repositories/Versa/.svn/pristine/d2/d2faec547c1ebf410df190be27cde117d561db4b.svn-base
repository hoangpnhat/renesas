/******************************************************************************/
/*
  FILENAME    RX200tst.h
  CONTENTS: This is a RC01SN FLASH module voltage monitor subroutine program
            heddar for the V3300,4000,5000 test system
  COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED
  2010/07/22  rev.00  1'st created by S.Jinbo
*/
/******************************************************************************/
#ifndef TST_H
#define TST_H

#include "RC04EX_man.h"


/************************************************************************
 * Difine Include fine
 ************************************************************************/
extern int SetPERCR( int mode, interface_t* param);

extern int PVEVvsTapTrmZ(int,int[],interface_t*,char*);
extern int PVEVFLDvsTapTrmZ(int,int[],interface_t*,char*);
extern int AutoTrimManage(int,int,int[],interface_t*);
extern int MonitorManage(int,int[],interface_t*);
extern int CpuReadMHz(int[],int,interface_t* param);

extern int VMONIvsTap_1(int[],interface_t* param, int);
extern int VMONIvsTap_2(int[],interface_t* param, int);
extern int IMONIvsTap(int[],interface_t* param, int);
extern int ROSCvsTap(int[],interface_t* param, int);
extern int VMONIvsVcc(int[],interface_t* param, int);
extern int IMONIvsVcc(int[],interface_t* param, int);

extern int MONIvsTap(int,int[],int waittime, interface_t* param); //Add 2018/0420
extern int CurrentMenu(int[],int,interface_t*);
extern int BGRMenu(int[],int,interface_t*);
extern int GetChipInfo(int[],interface_t*);
extern int VrsgMonitor_Repeat(interface_t*,int[]);
extern int Trim_VddVddh(int,interface_t*);
extern int Iref_Trim_ALL(int , interface_t* );
extern int Iref_Temp_moni_ALL(int , interface_t* );
extern int Iref_Trim_forRext(int , interface_t* );
extern int Shmoo_LPRead(int , interface_t* );
extern int Shmoo_LPRead_vs1MOSC(int , interface_t* ,int[]);
extern int MonitorALL_Tap(int [],interface_t* ,int ,int );
extern void CPcurrent_ALL(int , int [],int ,interface_t* ,int );
extern int ReadWindow_short(int,int[],int,interface_t*,int);
extern int MINORI_Shmoo_Loop(int , int[],int ,interface_t* ,int , char* );
extern int RomBoot_Shmoo_Loop(int , int[],interface_t* ,int );
extern int RomBoot_Shmoo_Menu(int[],interface_t*);

/*--------------------------------*/
/* ShmooAnalyzeFuncTest call_mode */
/*--------------------------------*/
#define SHM_DEBUG              0
#define SHM_CPUREADMHZ_VDD     1



/************************************************************************
 * Add define Use RX200AT.c RX200_Tokunin.c 
 ************************************************************************/
typedef struct {
    int tno;        /* Test No */
    int mask;       /* Test Mask Info */
    int wcat;       /* Wmap Cat */
    char fcat;      /* Fail Cat */
    char mat;       /* Target Mat */
    char mem;       /* Target mem DATA/ECC/LK/BP/ECCON/(Posi/Nega) = bit0/bit1/bit2/bit3/bit4/(bit5/bit6) */
    char func;      /* Target Function */ /* 1=OpensShorts2=Istby3=CpuModeFunc4=AutoTrimManage5=SetLogParam()6=MonitorManage7=JudgeVoltageP18=PVEVvsTapTrmZ9=PVEVFLDvsTapTrmZ10=SetWmatBm11=SetTmatBm12=JudgeVoltageMGShort */
    int vcc;        /* Test VCC */
    int speed;      /* Test Speed [MHz] */
    int pll;        /* PLL */
    int time_out;   /* time out [*10msec]*/
    char *item;     /* Test Name */
    int mode1;      /* Test Argument 1 */
    int mode2;      /* Test Argument 2 */
    char *pat;      /* Test Pattern File Name */
    char *in_para;  /* Test param info */
    char *out_para; /* Test result info */
} ATFLOW_COND_T;

extern void SetParamValueByMat( char mat, interface_t* param );
extern void SetParamValueBySW( char data, interface_t* param );
extern int SetInParamCond( char* str, interface_t* param );
extern int SetTrimLogParam( int mode, interface_t* param, int* logparam );
extern int SetWmatEcr( int mode, interface_t* param, int* logparam );
extern int SetTmatEcr( int mode, interface_t* param, int* logparam );
extern int SetResultLogParam( char* str, int result, interface_t* param, int* logparam );
extern int CheckWmatData( int mode1, int mode2 );
extern int DistributionProbeFunc( int mode );
extern int CheckLogSampling( int x, int y );
extern int JudgeVoltageP1(int[],interface_t*);
extern int JudgeVoltageP2(int[],interface_t*);

extern int Extra_VthRead(int[], int, interface_t*,int);
extern int Erase_VthvsIref(int[], int, interface_t*);
extern int Extra_Erase_Vth(int[], int, interface_t*);

#define BT_EXP_FAIL         0x00000001
#define BT_NO_JUDGE         0x00000002
#define BT_NO_STOR_PARAM    0x00000100
#define BT_SAMPLING         0x00000400  /* Sampling Data Logging */
#define BT_AT_FAIL_FUNC     0x00008000
#define BT_PASS_RETEST      0x00020000  /* Read P1 Code PASS Device Test */


#endif /*TST_H*/
