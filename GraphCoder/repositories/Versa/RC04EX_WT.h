/*******************************************************************************
*  FILENAME RC04EX_WT.h
*
  CONTENTS: This is the RC04EEX(SOTB) FLASH module function program
  for the 5000 test system
  COPYRIGHT 2018 Renesas Technology ALL RIGHTS RESERVED
  1'st created by Y.Aoki(SRM13)
 ******************************************************************************/
#ifndef MPT_H
#define MPT_H
#include "RC04EX_def.h"
/*---- Flow Test Data ----*/
typedef struct {
  int sno;
  int tno;
  int fk_on;
  int fk_off;
  char *timming;
  char exp;
  char cat;
  char *item;
  int vcc;
  int vref;
  int vrefh;
  int extal;
  int time_out;
  char pon;
  char poff;
  char *mon_type;
  int limit_l;
  int limit_h;
  int supply;
  char *pat_file;
  char *param_file;
  char *trans_file;
  char *item2;
} FLOW_COND_ST;

/*-- テスト結果構造体 ----------------------------------------------------------------------------*/
typedef struct{
  char          first_cat;
  int           first_tno;
  char          fail_cat;
  int           fail_tno;
  int           judge_kind;
  int           monitor_1;
  int           monitor_2;
  int           test_time;
  int           boot_time;
  int           tap_shift;
  int           ram_out[4];
} TEST_RESULT_ST;


/*-- インテグレータ構造体 ------------------------------------------------------------------------*/
typedef struct{
  char*   t_no;
  int     id;
  int     testno;
  char*   name;
  char*   mode;
  int     address;
  int     size;
  int     calculation;
  char*   unit;
  char*   lolimit;
  char*   hilimit;
  int     value;
  int     exe_flag;
} INTEGRATOR_PARAM_ST;
extern INTEGRATOR_PARAM_ST integ_WT0[];
extern INTEGRATOR_PARAM_ST integ_WT1[];
extern INTEGRATOR_PARAM_ST integ_WT2[];
extern INTEGRATOR_PARAM_ST integ_WT4[];
extern INTEGRATOR_PARAM_ST integ_WT8[];
extern INTEGRATOR_PARAM_ST integ_WT9[];
extern INTEGRATOR_PARAM_ST integ_FT1[];
extern INTEGRATOR_PARAM_ST integ_FT2[];


/*----------------------------------------------------------------------------*/
/*---- MPT.c Function Prototype Declaration ----*/
/*----------------------------------------------------------------------------*/
extern int BIST_Test_Func( void );
extern int BIST_RELAY_Set( void );
extern int BIST_RandomDataBM_Set( void );
extern char* BIST_Apg_Set( void );
extern int BIST_Supply_Set( void );
extern int BIST_Moniter( int );
extern int BIST_Istby( int );
extern int BIST_OpensShorts( int );
extern int BIST_DC_RAM_Judge( void );

extern int BIST_Start_Setup( int );
extern int BIST_TestExe_Check( int );
extern int BIST_Integ_Store( void );
extern int PrintLogOutINTEG( int );

extern int DispLogic( int );

/*---- MPT.c Variable Declaration ----*/
extern FLOW_COND_ST   exe_test;
extern FLOW_COND_ST   sub_test;
extern int hw_err_flag;

extern char logfile[];


#endif
