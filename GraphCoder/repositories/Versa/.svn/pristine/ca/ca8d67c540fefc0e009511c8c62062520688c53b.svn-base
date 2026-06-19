/*****************************************************************************/
/*
  FILENAME    RX200tst.c
  CONTENTS: This is a 130nm(RC01SN) FLASH module voltage monitor subroutine program
            for the V3300,4000,5000 test system
  COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED
  2010/07/22  rev.00  1'st created by S.Yamaoka
*/
/******************************************************************************/
#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "RC04EX.hpg"
#include "RC04EX_CGR.h"
#include "RC04EX_cyc.h"
#include "compat.h"
#include "RC04EX_apg.h"
#include "RC04EX_tst.h"
#include "RC04EX_fnc.h"

//#include "RX200tst_MonitorManage.c"
//#include "RX200tst_AutoTrimManage.c"
//#include "RX200tst_Shmoo.c"
//#include "RX200tst_AT.c"


/******************************************************************************/
/*                                                                            */
/* int MONI vs Tap Add 2018/0419(devert D90T)                                 */
/*                                                                            */
/******************************************************************************/
int MONIvsTap(int mode,int vs[NUM_POWERSUPPLY], int waittime, interface_t* param)
{
  /*-----------------------------------------------------*/
  /* Values                                              */
  /*-----------------------------------------------------*/
  int i,j,mon, selvol,result;
  int change_reg,set_reg,reg_code[160],mask,shift,loop_st,loop_ed,init_reg,judge;
  int testsel_mode,terminal,tempo,tempo0,tempo1,tempo2,tempo3,stack0,stack1,stack2;	
  int module_loop,first_flag,pass_flag,err_flag,cnt,repeat_mode;
  int trimvalue[V_VPP_TF+1];
  int vol[V_VPP_TF+1];       /* Monitored Voltages  */
  int k;
//  double k;
  char sitenum[10],filename[256],filename2[256],delay_result[3];
  FILE_T *fpt;
  first_flag = 1;
  selvol = 0; //Initialize selvol
  tempo0 = SetValue(IN_BDATA15,param,UA250); //PMU range = 250[uA]

  /*-----------------------------------------------------*/
  /* Test                                                */
  /*-----------------------------------------------------*/
  switch(mode){
     case V_VREF10:
      sprintf(filename,"vref10(vdemg)_vsTAP");
      change_reg = IN_ETLR11;
      testsel_mode = MONITOR_VREF10;
      terminal = VSSMON_MON;
      //for ( i=0 ; i<=15 ; i++ )//ログ出力順変更の為、コメントアウト 
        reg_code[ 0] =  7 & 0x0F;
        reg_code[ 1] =  6 & 0x0F;
        reg_code[ 2] =  5 & 0x0F;
        reg_code[ 3] =  4 & 0x0F;
        reg_code[ 4] =  3 & 0x0F;
        reg_code[ 5] =  2 & 0x0F;
        reg_code[ 6] =  1 & 0x0F;
        reg_code[ 7] =  0 & 0x0F;
        reg_code[ 8] = 15 & 0x0F;
        reg_code[ 9] = 14 & 0x0F;
        reg_code[10] = 13 & 0x0F;
        reg_code[11] = 12 & 0x0F;
        reg_code[12] = 11 & 0x0F;
        reg_code[13] = 10 & 0x0F;
        reg_code[14] =  9 & 0x0F;
        reg_code[15] =  8 & 0x0F;
	
      mask  = 0x0F;
      shift = 4;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VDD:
      sprintf(filename,"vdd_vsVol");
      testsel_mode = MONITOR_VDD;
      break;
    case V_VDDH:
      sprintf(filename,"vddh_vsVol");
      testsel_mode = MONITOR_VDDH;
      break;
    case V_VPP_E:
      sprintf(filename,"vpp(erase)_vsTAP");
      testsel_mode = MONITOR_VPP_E;
      terminal = VSSMON_MON;
      change_reg = IN_ETLR20;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0x0F;
      shift = 4;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VPP_P:
      sprintf(filename,"vpp(write)_vsTAP");
      testsel_mode = MONITOR_VPP_P;
      terminal = VSSMON_MON;
      change_reg = IN_ETLR20;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0xF0;
      shift = 0;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VPP_PW:
      sprintf(filename,"vpp(pre-write)_vsTAP");
      testsel_mode = MONITOR_VPP_PW;
      terminal = VSSMON_MON;
      change_reg = IN_ETLR21;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0x0F;
      shift = 4;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VHH_E:
      sprintf(filename,"vhh(erase)_vsTAP");
      testsel_mode = MONITOR_VHH_E;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR16;
      for ( i=0 ; i<=63 ; i++ ) reg_code[i] = (0x0 + i) & 0x3F;
      mask  = 0xC0;
      shift = 0;
      loop_st = 0;
      loop_ed = 63;
      break;
    case V_VHH_P:
      sprintf(filename,"vhh(write)_vsTAP");
      testsel_mode = MONITOR_VHH_P;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR17;
      for ( i=0 ; i<=63 ; i++ ) reg_code[i] = (0x0 + i) & 0x3F;
      //for ( i=63 ; i<=0 ; i-- ) reg_code[i] = (0x0 + i) & 0x3F;
      mask  = 0xC0;
      shift = 0;
      loop_st = 0;
      loop_ed = 63;
      break;
    case V_VRSG:
      sprintf(filename,"vrsg_vsTAP");
      testsel_mode = MONITOR_VRSG;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR18;
      //for ( i=0 ; i<=7 ; i++ ) reg_code[i] = (0x0 + i) & 0x07;  /*ログ出力順変更の為、コメントアウト*/
      reg_code[0] = 0x1 & 0x07;
      reg_code[1] = 0x0 & 0x07;
      reg_code[2] = 0x3 & 0x07;
      reg_code[3] = 0x2 & 0x07;
      reg_code[4] = 0x5 & 0x07;
      reg_code[5] = 0x4 & 0x07;
      reg_code[6] = 0x7 & 0x07;
      reg_code[7] = 0x6 & 0x07;
      mask  = 0xF8;
      shift = 0;
      loop_st = 0;
      loop_ed = 7;
      break;
    case V_V33R:
      sprintf(filename,"v33r_vsTAP");
      testsel_mode = MONITOR_V33R;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR24;
      for ( i=0 ; i<=7 ; i++ ) reg_code[i] = (0x0 + i) & 0x07;
      mask  = 0xF8;
      shift = 0;
      loop_st = 0;
      loop_ed = 7;
      break;
    case V_VWI:
      sprintf(filename,"vwi_vsTAP");
      testsel_mode = MONITOR_VWI;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR19;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0x0F;
      shift = 4;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VDEMG:
      sprintf(filename,"vdemg_vsTAP");
      testsel_mode = MONITOR_VDEMG;
      terminal = VSSMON_MON;
      change_reg = IN_ETLR21;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0xF0;
      shift = 0;
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VNOEMI_PE:
      sprintf(filename,"vnoemi(PE)_vsTAP");
      testsel_mode = MONITOR_VNOEMI_PE;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR19;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      mask  = 0xF0;
      shift = 0;
      loop_st = 0;
      loop_ed = 15;
      break;
    case I_IMONI_I01U_NOTEMP:
    case I_IMONI_FLASH_NOTEMP:
      if( mode == I_IMONI_I01U_NOTEMP )  {sprintf(filename,"imoni-i01u(no-temp)_vsTAP");  testsel_mode = MONITOR_IMONI_I01U_NOTEMP;}
      if( mode == I_IMONI_FLASH_NOTEMP ) {sprintf(filename,"imoni-flash(no-temp)_vsTAP"); testsel_mode = MONITOR_IMONI_FLASH_NOTEMP;}
      terminal = VSSMON_MON;
      change_reg = IN_ETLR08;
      for ( i=0 ; i<=127 ; i++ ) reg_code[i] = (0x0 + i) & 0x7F;
      mask  = 0x80;
      shift = 0;
      loop_st = 0;
      loop_ed = 127;
      break;
    case I_IMONI_I01U_TEMP:
    case I_IMONI_FLASH_TEMP:
      if( mode == I_IMONI_I01U_TEMP )  {sprintf(filename,"imoni-i01u(temp)_vsTAP");  testsel_mode = MONITOR_IMONI_I01U_TEMP;}
      if( mode == I_IMONI_FLASH_TEMP ) {sprintf(filename,"imoni-flash(temp)_vsTAP"); testsel_mode = MONITOR_IMONI_FLASH_TEMP;}
      terminal = VSSMON_MON;
      change_reg = IN_ETLR09;
      for ( i=0 ; i<=127 ; i++ ) reg_code[i] = (0x0 + i) & 0x7F;
      mask  = 0x80;
      shift = 0;
      loop_st = 0;
      loop_ed = 127;
      break;
    case V_VHH_E_TEMP: /* 定電圧(Vhh)温度補償 */
      sprintf(filename,"vhh(erase)_vsTEMP");
      testsel_mode = MONITOR_VHH_E;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR13;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      shift = 4;       /* 温特は上位4bit */
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VHH_P_TEMP: /* 定電圧(Vhh)温度補償 */
      sprintf(filename,"vhh(write)_vsTEMP");
      testsel_mode = MONITOR_VHH_P;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR14;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      shift = 4;       /* 温特は上位4bit */
      loop_st = 0;
      loop_ed = 15;
      break;
    case V_VHH_P_TEMPV: /* 定電圧(Vhh)温度補償 */
      sprintf(filename,"vhh(write)_vsTEMP");
      testsel_mode = MONITOR_VHH_P;
      terminal = VCCMON_MON;
      change_reg = IN_ETLR14;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      shift = 0;       /* 温特電圧補償は下位4bit */
      loop_st = 0;
      loop_ed = 15;
      break;
    case I_IMONI_TEMP: /* 定電流(Flash)温度補償 */
      sprintf(filename,"imoni-flash(temp)_vsTEMP");
      testsel_mode = MONITOR_IMONI_FLASH_TEMP;
      terminal = VSSMON_MON;
      change_reg = IN_ETLR12;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i] = (0x0 + i) & 0x0F;
      shift = 0;       /* 温特は下位4bit */
      loop_st = 0;
      loop_ed = 15;
      break;
    case T_OSC32:
      sprintf(filename,"T_OSC32");
      change_reg = IN_ETLR10;
  	   testsel_mode = MONITOR_OSC32;
    	 SetValue(IN_LDATA0,param,268*10); /* Target=270us(10*100ns=1us) */
  	   for ( i=0 ; i<=127 ; i++ ) reg_code[i] = (0x0 + i) & 0x7F;
  	   mask  = 0x80;
      shift = 0;
      loop_st = 0;
      loop_ed = 127;
  	   break;
    case T_OSC1:
      sprintf(filename,"T_OSC1");
      change_reg = IN_ETLR11;
  	  	testsel_mode = MONITOR_OSC1;
      SetValue(IN_LDATA0,param,98*10); /* Target=100us(10*100ns=1us) */
	     for ( i=8 ; i<=15 ; i++ ) reg_code[i-8] = (0x0 + i) & 0x0F;
      for ( i=0 ; i<=7 ; i++ ) reg_code[i+8] = (0x0 + i) & 0x0F;
      mask  = 0xF0;
      shift = 0;
      loop_st = 0;
      loop_ed = 15;
      break;
    case T_READDELAY8_FLP0:
    case T_READDELAY8_FLI1:
    case T_READDELAY8_FLI2:
  	   if(mode ==T_READDELAY8_FLP0){
  	   	 sprintf(filename,"T_READDELAY8_FLP0");
        change_reg = IN_ETLR26;
  	   	 mask  = 0xF0;
        shift = 0;
  	   	 module_loop=0;
  	   }else if(mode ==T_READDELAY8_FLI1){
        sprintf(filename,"T_READDELAY8_FLI1");
        change_reg = IN_ETLR26;
  	   	 mask  = 0x0F;
        shift = 4;
  	   	 module_loop=1;
  	   }else if(mode ==T_READDELAY8_FLI2){
  	   	 sprintf(filename,"T_READDELAY8_FLI2");
        change_reg = IN_ETLR27;
        mask  = 0xF0;
        shift = 0;
  	   	 module_loop=2;
  	   }
	     for ( i=8 ; i<=15 ; i++ ) reg_code[i-8] = (0x0 + i) & 0x0F;
      for ( i=0 ; i<=7 ; i++ ) reg_code[i+8] = (0x0 + i) & 0x0F;
      loop_st = 0;
      loop_ed = 15;
     	testsel_mode =	AUTOTRIM_READDELAY8;
  	   selvol = 70;
      break;
    case T_READDELAY10_FLP0:
    case T_READDELAY10_FLI1:
    case T_READDELAY10_FLI2:
  	   if(mode ==T_READDELAY10_FLP0){
  	   	 sprintf(filename,"T_READDELAY10_FLP0");
        change_reg = IN_ETLR28;
  	   	 module_loop=0;
  	   }else if(mode ==T_READDELAY10_FLI1){
        sprintf(filename,"T_READDELAY10_FLI1");
        change_reg = IN_ETLR29;
  	   	 module_loop=1;
  	   }else if(mode ==T_READDELAY10_FLI2){
  	   	 sprintf(filename,"T_READDELAY10_FLI2");
        change_reg = IN_ETLR30;
  	   	 module_loop=2;
  	   }
      for ( i=16 ; i<=31 ; i++ ) reg_code[i-16] = (0x0 + i) & 0x1F;
      for ( i=0 ; i<=15 ; i++ ) reg_code[i+16] = (0x0 + i) & 0x1F;
      loop_st = 0;
      loop_ed = 31;
      mask  = 0xE0;
      shift = 0;
      testsel_mode = AUTOTRIM_READDELAY10;
      selvol = 40;
  	   break;
    default:  printf("!!!!!!!!!!! No select !!!!!!!!!!!!!\n");  return;  break;
  }/* switch */

  init_reg = GetValue(change_reg,param);

  

  /*-----------------------------------------------------*/
  /* Generate Data Log File Name & Open This File        */
  /*-----------------------------------------------------*/
  sprintf(sitenum,"_%d.csv",get_site_number());
  sprintf(filename2,"%s_W%d_X%dY%d_Vcc%d_Vddh_%d_Vdd%d_%dC",lotname,atoi(wafername),xy_location[0],xy_location[1],vs[0],vs[1],vs[2],GetValue(IN_LDATA4,param));
  strcpy(datapat,glob_cdp);
  strcat(datapat,glob_datalog);
  strcat(datapat,filename);
  strcat(datapat,filename2);
  strcat(datapat,sitenum);

  if((fpt=fopen(datapat,"at"))==NULL){
    ErrorPrint(datapat);
    return(FAIL);
  }

  /*-----------------------------------------------------*/
  /* Test                                                */
  /*-----------------------------------------------------*/
  if( mode<40 ){
    fprintf(fpt,"ETLR%02d,ETLR_Tap,Voltage[mV] (Vcc:%d Vddh:%d Vdd:%d ETLR%02d)\n",change_reg,vs[0],vs[1],vs[2],change_reg);
  }else if( mode==V_VHH_E_TEMP || mode==V_VHH_P_TEMP ){ /* 定電圧(Vhh)温度補償 */
    if( mode==V_VHH_E_TEMP ) fprintf(fpt,"ETLR16(Vhh_E):0x%02X\n",GetValue(IN_ETLR16,param));
    if( mode==V_VHH_P_TEMP ) fprintf(fpt,"ETLR17(Vhh_P):0x%02X\n",GetValue(IN_ETLR17,param));
    fprintf(fpt,"ETLR%02d,ETLR_Tap,Voltage[mV] (Vcc:%d Vddh:%d Vdd:%d ETLR%02d)\n",change_reg,vs[0],vs[1],vs[2],change_reg);
  }else if(mode <= 43 && 40 <= mode){ /* 電流モニタ */
    if( mode==I_IMONI_TEMP ) fprintf(fpt,"ETLR09(Imoni):0x%02X\n",GetValue(IN_ETLR09,param));
    fprintf(fpt,"ETLR%02d,ETLR_Tap,Current[nA] (Vcc:%d Vddh:%d Vdd:%d ETLR%02d)\n",change_reg,vs[0],vs[1],vs[2],change_reg);
  }else if(mode == 50 || mode == 51){
    fprintf(fpt,"ETLR%02d,ETLR_Tap,Current[cyc] (Vcc:%d Vddh:%d Vdd:%d ETLR%02d)\n",change_reg,vs[0],vs[1],vs[2],change_reg);	
  }else{
    fprintf(fpt,"ETLR%02d,ETLR_Tap,Current[ns] (Vcc:%d Vddh:%d Vdd:%d ETLR%02d)\n",change_reg,vs[0],vs[1],vs[2],change_reg);
  }

  tempo1 = SetValue(IN_REG,param,GetValue(IN_REG,param)|TRIM_REG);
  tempo2 = SetValue(IN_TESTSEL,param,testsel_mode);
  SetValue(IN_BDATA00,param,0);

  Ffpt = 0;

  if( mode<=T_OSC1/*<=61*/ && 4<mode ){
    for ( i=loop_st ; i<=loop_ed ; i++ ){
    
      if(first_flag==1){ /* Initial mode */
        first_flag = 0;
        repeat_mode =INITIAL_SET |  RAMBOOT_CHANGE;
	  }else{
        repeat_mode = INIF_SET | RAMBOOT_CHANGE;
      }

    
      set_reg = (init_reg & mask) | ((reg_code[i] << shift) & 0xFF);
      SetValue(change_reg,param,set_reg);
      printf("### ETLR%02d  Code:0x%02X\n",change_reg,reg_code[i]);
      if( mode<I_IMONI_I01U_NOTEMP/*<40*/ ){
        CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Tap_dependency",&selvol);
        //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Tap_dependency",&selvol);
        fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d,[mV]\n",change_reg,GetValue(change_reg,param),reg_code[i],selvol);
      }else if(I_IMONI_I01U_NOTEMP <= mode && mode <= I_IMONI_FLASH_TEMP){ /*40～43*/
        SetValue(IN_BDATA15,param,UA25); //set PMU range 25[uA]
        CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol); 
        //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
        if( selvol>=24000 ){
          SetValue(IN_BDATA15,param,UA250); //set PMU range 250[uA]
          CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
          //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
        }
        fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d,[nA]\n",change_reg,GetValue(change_reg,param),reg_code[i],selvol);
      }else if(mode == T_OSC32 ||mode == T_OSC1){
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("monitorcycle_at"),"MONITOR_Tap_dependency");
        if(mode == T_OSC32){
          result = 0xFFFF-GetValue(OUT_LDATA0,param);
          printf( " OSC(ETCR15&14) = 0x%04X ->> %d[cyc]\n", GetValue(OUT_LDATA0,param), result );
          fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d\n",change_reg,GetValue(change_reg,param),reg_code[i],result);
        }else{
          result = 0xFFFF-GetValue(OUT_LDATA1,param);
          printf( " OSC(ETCR15&14) = 0x%04X ->> %d[cyc]\n", GetValue(OUT_LDATA1,param), result );
          fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d\n",change_reg,GetValue(change_reg,param),reg_code[i],result);
        }
      }
    }
  }else if( mode == 3 /*VDD*/ ){
//    for ( k=386 ; k<=722 ; k=k+6.329 ){
    for ( k=386 ; k<=722 ; k=k+6 ){
      printf("MONITOR VDD\n");
      SetValue(IN_TESTSEL,param,MONITOR_VDD);
      SetValue(IN_BDATA00,param,0);
      vs[2] = k; //Vdd=0
//      vs[1] = i; //Vddh=0
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
      vol[V_VDD] = selvol;
      fprintf(fpt,"V_EXVREF_VDD =%.1f, V_VDD=%d,[mV]\n",k,selvol);
      }
  }else if( mode == 4 /*VDDH*/ ){
    for ( i=428 ; i<=560 ; i=i+4 ){
      printf("MONITOR VDDH\n");
      SetValue(IN_TESTSEL,param,MONITOR_VDDH);
      SetValue(IN_BDATA00,param,0);
//      vs[2] = i; //Vdd=0
      vs[1] = i; //Vddh=0
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDDH",&selvol);
      vol[V_VDDH] = selvol;
      fprintf(fpt,"V_EXVREF_VDDH=%d, V_VDDH=%d,[mV]\n",i,selvol);
      }
  }else if(T_READDELAY8_FLP0 <= mode&&mode <= T_READDELAY10_FLI2){/*DELAY*/
    SetValue(IN_LDATA0,param,100*10000); //100ms//
    stack0 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    stack1 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    stack2 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    for ( i=loop_st ; i<=loop_ed ; i++ ){
      set_reg = (init_reg & mask) | ((reg_code[i] << shift) & 0xFF);
      SetValue(change_reg,param,set_reg);
	     printf(" ETLR%02d  Code:0x%02X\n",change_reg,GetValue(change_reg,param));
      first_flag =1;
      pass_flag = err_flag = cnt =0;
      DeviceSpecificPowerUp();
      while(1){
        if(first_flag==1){ /* Initial mode */
          first_flag = 0;
          repeat_mode =INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
	       }else{
          repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
        }
        CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcycle_at"),"MONITOR_READDELAY(DELAYTEST)",&selvol);
        delay_result[module_loop] = (GetValue(OUT_JUDGE1,param)>>8)&(1<<module_loop);
        result = ((0x00FF&GetValue(OUT_JUDGE1,param))<<8)|((0xFF00&GetValue(OUT_JUDGE1,param))>>8);
        if(T_READDELAY8_FLP0 <= mode && mode <= T_READDELAY8_FLI2)    result = result & 0x170;
        if(T_READDELAY10_FLP0 <= mode && mode <= T_READDELAY10_FLI2)  result = result & 0x180;

        if( GetValue(OUT_JUDGE1,param) == 0x00 ){
          pass_flag = 1;
          if(err_flag!=1){								 
            printf("\n Monitor PASS timing => Result %dns -> %dns\n", selvol, selvol+1); 
            selvol=selvol+1;
          	 judge = 1;
	         }
        }else if(result == 0x170 || result == 0x180 ){
          if( delay_result[module_loop]==0x00 ){
            pass_flag = 1;
            if(err_flag!=1){
              printf("\n Monitor PASS timing => Result %dns -> %dns\n", selvol, selvol+1); 
              selvol=selvol+1;
            	 judge = 1;
	  	        }
          }else if( delay_result[module_loop]==(1<<module_loop)) {
            err_flag = 1;
            if(pass_flag!=1){
              printf("\n Monitor PASS timing => Result %dns -> %dns\n", selvol, selvol-1);            
              selvol=selvol-1;
             	judge = 0;
	  	        }
          }
        }else{
          printf("\n Monitor Error\n");
          selvol = 0;
          pass_flag=1;
          err_flag==1;
        }

        if( pass_flag==1&&err_flag==1)break;
        cnt++;
        if( selvol<21||cnt>100 ){
          selvol=0;
          printf( " Monitor Error !!\n" );
        	return(FAIL);
        }
      }
      DeviceLevelsPowerDown(); /* 各電源OFFコマンド */
      fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d\n",change_reg,GetValue(change_reg,param),reg_code[i],selvol-judge);
    }
    SetValue(IN_TESTSIZE0,param,stack0);
    SetValue(IN_TESTSIZE1,param,stack1);
    SetValue(IN_TESTSIZE2,param,stack2);
  }else if(V_VHH_E_TEMP<=mode){ /* 温度補償 */
    tempo3 = SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param)&0x3F)|0xC0);
    for ( j=loop_st ; j<=loop_ed ; j++ ){   /* 温特用ビットのループ   */
      for ( i=loop_st ; i<=loop_ed ; i++ ){ /* 絶対値用ビットのループ */
        if(first_flag==1){ /* Initial mode */
          first_flag = 0;
          repeat_mode =INITIAL_SET |  RAMBOOT_CHANGE;
	    }else{
          repeat_mode = INIF_SET | RAMBOOT_CHANGE;
        }

        set_reg = (reg_code[j] << shift) | (reg_code[i] << (shift ^ 0x4));
        SetValue(change_reg,param,set_reg);
        printf("### ETLR%02d  Code:0x%02X\n",change_reg,reg_code[i]);
        if( mode==I_IMONI_TEMP ){
          SetValue(IN_BDATA15,param,UA25); //set PMU range 25[uA]
          CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
          //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
          if( selvol>=24000 ){
            SetValue(IN_BDATA15,param,UA250); //set PMU range 250[uA]
            CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
            //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_Tap_dependency",&selvol);
          }
          printf("Monitor : %d[nA]\n",selvol);
          fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d,[nA]\n",change_reg,GetValue(change_reg,param),reg_code[i],selvol);
        }else{
          CpuModeMonitor_Repeat(repeat_mode,terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Tap_dependency",&selvol);
          //CpuModeMonitor(terminal,&vs[0],param,waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Tap_dependency",&selvol);
          printf("Monitor : %d[mV]\n",selvol);
          fprintf(fpt,"ETLR%02d:%02X, H'%02X, %d,[mV]\n",change_reg,GetValue(change_reg,param),reg_code[i],selvol);
        }
      }
    }
    SetValue(IN_ETLR15,param,tempo3);
  }

  fprintf(fpt,"\n");
  /*-----------------------------------------------------*/
  /* Close Data File                                     */
  /*-----------------------------------------------------*/
  if(fclose(fpt)){
    exit(-10);
  }

  Ffpt=1;

  SetValue(change_reg,param,init_reg);
  SetValue(IN_BDATA15,param,tempo0);
  SetValue(IN_REG,param,tempo1);
  SetValue(IN_TESTSEL,param,tempo2);
  
  DeviceLevelsPowerDown();
  DeviceSpecificPowerDown();
  return( PASS );
}



int CurrentMenu(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,j,k;
  int test_cond,selvol,freq_num,temp_check,ffpt_bak;
  int info[NUM_CHIPINFO];                    /* wmat chip information              */
  char chip_name[100],Lname[50],file_name[200],file_name2[200], buff[300],pat_name[80];
  int current_data[10][30];
  int result;

  tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = tempo6 = 0;
  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = 0;
  test_cond = 0;
  
  while( test_cond != EXIT){

    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|Read-Idle | Read-Idle[Flash PW-ON ] (1) \n");
    printf("|          | Read-Idle[Flash PW-OFF] (2) \n");
    printf("|  SSTBY   | SSTBY    [Flash PW-ON ] (3) \n");
    printf("|          | SSTBY    [Flash PW-OFF] (4) \n");
    printf("|  SLEEP   | SLEEP    [Flash PW-Idle](5) \n");
    printf("|          | SLEEP    [Flash PW-OFF] (6) \n");
    printf("| LP-Idle  | LP-Idle  [Flash PW-ON ] (7) \n");
    printf("|          | LP-Idle  [Flash PW-OFF] (8) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Read    | While-1  [ALL0  Read-ON ] (100) \n");
    printf("|          | While-1  [ALL0  Read-OFF] (101) \n");
    printf("|          | While-1  [ALL1  Read-ON ] (110) \n");
    printf("|          | While-1  [ALL1  Read-OFF] (111) \n");
    printf("|          | Normal   [CHKA  Read-ON ] (120) \n");
    printf("|          | Normal   [CHKA  Read-OFF] (121) \n");
    printf("|          | Ramdom   [Other Read-ON ] (130) \n");
    printf("|          | Ramdom   [Other Read-OFF] (131) \n");
    printf("|          | LPREAD   [CHKA  Read-ON ] (140) \n");
    printf("|          | LPREAD   [ALL0  Read-OFF] (141) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|PE-Current| Write(50) / Erase (30) / PreWrite (31)\n");
    printf("|          | Read-C for Write(52) / Read-C for Erase(32) \n");
    printf("|          | Bottom ALL(350) \n");
    printf("|PE-Peek   | Write(250) / Erase (230) / PreWrite (231) / Read-PE(200)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Other   | PowerOn(600)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Setting  | EXIT (e) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf(" Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();    

    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);

    switch(test_cond){
    case QUIT:
      DeviceSpecificPowerDown();
      if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
      PowerDown(1);
      break;

      case 1: //-- Flash : Read-Idle --//
      case 2: //-- Flash : Power Off --//
      case 3: //-- Flash : Standby   --//
      case 4: //-- Flash : Power Off --//
      case 5: //-- Flash : Read-Idle (Chip : SLEEP) --//
      case 6: //-- Flash : Power Off (Chip : SLEEP) --//
        stack0 = SetValue(IN_BDATA00,param,test_cond - 1);
        stack1 = SetValue(IN_BDATA01,param,1); //電流評価のため無限ループ
	
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("dc_current_io"),"dc_current");
	
     	  SetValue(IN_BDATA00,param,stack0);
        SetValue(IN_BDATA01,param,stack1);
      break;
      case 7: //-- Flash : LP-Mode(Chip:Sub-OSC) --//
      SetValue(IN_BDATA00,param,2);// Flash ON
      SetValue(IN_BDATA01,param,1);// Loop Mode ON
      SetValue(IN_EXTAL1,param,31*1000);// 32KHz
      CpuLPReadMode(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("LPRead_current"),"LP-Current");
      SetValue(IN_EXTAL1,param,31);// 32MHz      
      break;
      case 8: //-- Flash : LP-Mode(Chip:Sub-OSC) --//
      SetValue(IN_BDATA00,param,1);// Flash OFF
      SetValue(IN_BDATA01,param,1);// Loop Mode ON
      SetValue(IN_EXTAL1,param,31*1000);// 32KHz      
      CpuLPReadMode(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("LPRead_current"),"LP-Current");
      SetValue(IN_EXTAL1,param,31);// 32MHz      
      break;
      
      
      
      case 50:
        ChangeAddress(FLP0,0,0xFF,param);
        SetValue(IN_BDATA05,param,2);
		stack4 = SetValue(IN_BDATA10,param,5);
        stack0 = SetValue(IN_TESTSEL,param,0);
        stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("programall_code_at_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;
      case 52:
        ChangeAddress(FLP0,0,0xFF,param);
        SetValue(IN_BDATA05,param,3);
		stack4 = SetValue(IN_BDATA10,param,5);
        stack0 = SetValue(IN_TESTSEL,param,0);
        stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("programall_code_at_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;

      case 30:
      case 31:
	    if(test_cond == 31){
	      SetValue(IN_BDATA00,param,0);	//PreWrite
        }else{
	      SetValue(IN_BDATA00,param,1); //Erase 
        }
        ChangeAddress(FLP0,0,0xFF,param);
        stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        SetValue(IN_BDATA05,param,2);
		stack4 = SetValue(IN_BDATA10,param,5);
        stack0 = SetValue(IN_TESTSEL,param,0);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("erase_code_at_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;

      case 32:
	      SetValue(IN_BDATA00,param,0);	//PreWrite
        ChangeAddress(FLP0,0,0xFF,param);
        SetValue(IN_BDATA05,param,3);
        stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
		stack4 = SetValue(IN_BDATA10,param,5);
        stack0 = SetValue(IN_TESTSEL,param,0);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("erase_code_at_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;

      case 250:
        ChangeAddress(FLP0,0,0xFF,param);
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,1);
		    SetValue(IN_BDATA01,param,0);
		    SetValue(IN_BDATA02,param,1);
		stack4 = SetValue(IN_BDATA10,param,5);
        //stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_DIGI | CRY_CLK/*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("PE_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;
      case 230:
        ChangeAddress(FLP0,0,0xFF,param);
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,2);
		    SetValue(IN_BDATA01,param,0);
		    SetValue(IN_BDATA02,param,1);
		stack4 = SetValue(IN_BDATA10,param,5);
        //stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_DIGI | CRY_CLK/*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("PE_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;
      case 231:
        ChangeAddress(FLP0,0,0xFF,param);
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,3);
		    SetValue(IN_BDATA01,param,0);
		    SetValue(IN_BDATA02,param,1);
		stack4 = SetValue(IN_BDATA10,param,5);
        //stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_DIGI | CRY_CLK/*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("PE_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;
      case 200:
        ChangeAddress(FLP0,0,0xFF,param);
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,4);
		    SetValue(IN_BDATA01,param,0);
		    SetValue(IN_BDATA02,param,1);
		stack4 = SetValue(IN_BDATA10,param,5);
        //stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_DIGI | CRY_CLK/*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("PE_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
		printf("\nPE Current %d[nA]\n",selvol);
        SetValue(IN_TESTSEL,param,stack0);
		SetValue(IN_BDATA10,param,stack4);
		SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;



      case 350:
        printf("Input Temperature -40 / 25 / 95 ->");
        DigitInput(&tempo0);
        ChangeAddress(FLP0,0,0xFF,param);
	SetValue(IN_EXTAL1,param,125);
        SetValue(IN_BDATA05,param,2);
	stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02);
        stack0 = SetValue(IN_TESTSEL,param,1);
        result = CpuModeMonitor_current2_First(0,MVCCMON,&vs[0],param);
        if(result != PASS)break;
        result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,250,CreatePatFileName("programall_code_at_current"),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)break;
        for(k=1;k<=3;k++){
          switch (k){
            case 1:
              vs[0]=3300;
              vs[1]=500;
              vs[2]=500;
            break;
            case 2:
              vs[0]=3700;
              vs[1]=540;
              vs[2]=696;
            break;
            case 3:
              vs[0]=1500;
              vs[1]=452;
              vs[2]=456;
            break;
          }

          for(i=1;i<=10;i++){
            for(j=1;j<=4;j++){
		          switch(j){
		            case 1:
		              SetValue(IN_TESTSEL,param,0);
			            SetValue(IN_BDATA00,param,1);
		              SetValue(IN_BDATA01,param,0);
		              SetValue(IN_BDATA02,param,1);
			            sprintf(pat_name,"PE_current");
		            break;
		            case 2:
		              SetValue(IN_TESTSEL,param,0);
			            SetValue(IN_BDATA00,param,2);
		              SetValue(IN_BDATA01,param,0);
		              SetValue(IN_BDATA02,param,1);
			            sprintf(pat_name,"PE_current");
		            break;
		            case 3:
		              SetValue(IN_TESTSEL,param,0);
			            SetValue(IN_BDATA00,param,3);
		              SetValue(IN_BDATA01,param,0);
		              SetValue(IN_BDATA02,param,1);
			            sprintf(pat_name,"PE_current");
		            break;
		            case 4:
		              SetValue(IN_TESTSEL,param,0);
			            SetValue(IN_BDATA00,param,0);
		              SetValue(IN_BDATA01,param,0);
		              SetValue(IN_BDATA02,param,1);
			            sprintf(pat_name,"PE_current");
		            break;
              }		   
              ffpt_bak=Ffpt;
              Ffpt=0;
              result = CpuModeMonitor_current2((ADC_AVE /*| PIN_JUMP*/),MVCCMON,&vs[0],param,300,CreatePatFileName(pat_name),"Program_PE",&selvol);  /*ファーム次第で変更する可能性あり*/
              Ffpt = ffpt_bak;
              if(result != PASS)break;
              current_data[j][i]=selvol;
            }
		        if(result != PASS)break;
		      }
        
          // ********  File Out  ******* //
          sprintf(file_name,"%s_W%d_X%dY%d_PE_current_",lotname,atoi(wafername),xy_location[0],xy_location[1]);
          sprintf(file_name2,"Vcc%d_Vddh%d_Vdd%d_T%d",vs[0],vs[1],vs[2],tempo0);
          sprintf( buff, "%s%s%s%s%s.csv", glob_cdp, glob_datalog,glob_currentlog,file_name,file_name2 );
          if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
          delay_timer(5*1000);
          fprintf(Fptdata,"ALL0[nA],Erase[nA],PreWrite[nA],Read[nA]\n");
          for(i=1;i<=7;i++){
		        for(j=1;j<=4;j++){
              fprintf(Fptdata,"%d,",current_data[j][i]);
            }
            fprintf(Fptdata,"\n");
          }
          delay_timer(5*1000);
          if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
          delay_timer(200);
        }

        SetValue(IN_TESTSEL,param,stack0);
	      SetValue(IN_ETCR11,param,stack3);
        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();    
      break;
      case 100:
        MinoriRead_Kani(WHILE1_ALL0 | READ_ON , &vs[0], waittime, param, 0);
        break;
      case 101:
        MinoriRead_Kani(WHILE1_ALL0 | READ_OFF, &vs[0], waittime, param, 0);
        break;
      case 110:
        MinoriRead_Kani(WHILE1_ALL1 | READ_ON , &vs[0], waittime, param, 0);
        break;
      case 111:
        MinoriRead_Kani(WHILE1_ALL1 | READ_OFF, &vs[0], waittime, param, 0);
        break;
      case 120:
        MinoriRead_Kani(NORMAL_READ | READ_ON , &vs[0], waittime, param, 0);
        break;
      case 121:
        MinoriRead_Kani(NORMAL_READ | READ_OFF, &vs[0], waittime, param, 0);
        break;
      case 130:
        MinoriRead_Kani(RANDOM_READ | READ_ON , &vs[0], waittime, param, 0);
        break;
      case 131:
        MinoriRead_Kani(RANDOM_READ | READ_OFF, &vs[0], waittime, param, 0);
        break;
      case 140:
        SetValue(IN_EXTAL1,param,31*1000); // 31us->32KHz
        SetValue(IN_BDATA00,param,1);//LoopMode
        SetValue(IN_LDATA0,param,0x3F7E7);
        SetValue(IN_LDATA1,param,0x405E3);
        SetValue(IN_LDATA2,param,0x60650);
        SetValue(IN_LDATA3,param,0x61000);
        SetValue(IN_BDATA10,param,0);
        Minori_LPRead(MNORMAL,&vs[0],waittime,param,"FLP0_CHKA_MULTISCANI_YS_LPREAD_WS2_REPEAT","MINORI READ");
        SetValue(IN_EXTAL1,param,31); // 31ns->32MHz

        break;
      case 141:
        SetValue(IN_EXTAL1,param,31*1000); // 31us->32KHz
        SetValue(IN_BDATA00,param,1);//LoopMode
        SetValue(IN_LDATA0,param,0x3F7E7);
        SetValue(IN_LDATA1,param,0x405E3);
        SetValue(IN_LDATA2,param,0x60650);
        SetValue(IN_LDATA3,param,0x61000);
        SetValue(IN_BDATA10,param,0);
        Minori_LPRead(MNORMAL,&vs[0],waittime,param,"FLI3_ALL0_MULTISCANI_YS_LPREAD_WS2_REPEAT","MINORI READ");
        SetValue(IN_EXTAL1,param,31); // 31ns->32MHz

        break;
      case 600:
	    SetTimming_NS( daxs_trans.write_rate );
  select_vector_mode(all_pins,_PE_VECTOR_MODE);
        reconnect_pin(all_pins);
  disconnect_pin(monitor_vdd_pin);
  disconnect_pin(monitor_vddh_pin);

        comp_set_v1(vs[0]);
        set_vih1(vs[0]);
        set_voh(vs[0]*1/2);
        set_vol(vs[0]*1/2);
        set_vhh( INIT_RELAY );
        comp_write_lbreg_pe(RVCCMON);
        delay_timer(30*1000);

        run_apg(_POWERON_TEST);
        delay_timer(30*1000);
        run_apg(_POWERON_TEST2);
        delay_timer(30*1000);
        run_apg(_POWERON_TEST3);
        delay_timer(30*1000);

        DeviceLevelsPowerDown();
        DeviceSpecificPowerDown();

        break;


      default: break;
    
    }//switch(test_cond) 

  }//while( test_cond != EXIT)

}

int BGRMenu(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,k,j,monitor_pin;
  int test_cond,ffpt_bak,etlr00_bak,etlr01_bak;
  int info[NUM_CHIPINFO];                    /* wmat chip information              */
  int result,target,Vresult;
  int extra1_etlr00,extra1_etlr01;
  int loop,loop_max,loop_min,loop_delta;
  int first_flag,repeat_mode,inif_flag;
  char bgr_status[6];
  char buff[200];
  char testname[20];
  interface_t* param_ini;

  tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = tempo6 = 0;
  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = 0;
  test_cond = 0;
  

  while( test_cond != EXIT){
    if(Extra5_TempCheck == EXTRA5_TEMP_ON){
      sprintf(bgr_status," ON");
    }else{
      sprintf(bgr_status,"OFF");
    }
    result = PASS;

    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|BGR Status| Extra5 BGR Code %s(bgr)\n",bgr_status);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Sample   | L# : %s  W# : %3d\n",lotname,atoi(wafername));
    printf("| Info     |  X : %3d      Y : %3d\n",xy_location[0],xy_location[1]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Rext WT  | WT1 Pass Code : %2X\n",info[13]);
    printf("|          | WT2 Pass Code : %2X\n",info[14]);
    printf("|          | WT3 Pass Code : %2X\n",info[15]);
    printf("|          | WT4 Pass Code : %2X\n",info[16]);
    printf("|          | WT5 Pass Code : %2X\n",info[17]);
    printf("|          | WT6 Pass Code : %2X\n",info[18]);
    printf("|          | WT8 Pass Code : %2X\n",info[19]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Extra1  | ETLR00 : 0x%X   ETLR01 : 0x%X \n",extra1_etlr00,extra1_etlr01);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Extra5  |  Vcc    ETLR00  ETLR01\n");
    printf("| BGR Trim | %4dmV   0x%X    0x%X \n",VCC_ARRAY0,Extra5_ETLR00[0],Extra5_ETLR01[0]);
    printf("|(Temp=90C)| %4dmV   0x%X    0x%X \n",VCC_ARRAY1,Extra5_ETLR00[1],Extra5_ETLR01[1]);
    printf("|          | %4dmV   0x%X    0x%X \n",VCC_ARRAY2,Extra5_ETLR00[2],Extra5_ETLR01[2]);
    printf("|          | %4dmV   0x%X    0x%X \n",VCC_ARRAY3,Extra5_ETLR00[3],Extra5_ETLR01[3]);
    printf("|          | %4dmV   0x%X    0x%X \n",VCC_ARRAY4,Extra5_ETLR00[4],Extra5_ETLR01[4]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    if( (vs[0] != VCC_ARRAY0) && (vs[0] != VCC_ARRAY1) && (vs[0] != VCC_ARRAY2) && (vs[0] != VCC_ARRAY3) && (vs[0] != VCC_ARRAY4) ){
      printf("|   VCC    | VCC Not Correct!! (vcc)\n");
    }else{
      printf("|   VCC    | VCC = %d mV(vcc) \n",vs[0]);
    }
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| ETLR00-01| ETLR00 : 0x%X (0)  ETLR01 : 0x%X (1)\n",GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param));
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Command  | Extar1-Read(10) Extra1-ReWrite(11)\n");
    printf("|          | Extar5-Read(50) Extra5-ReWrite(51)\n");
    printf("|          | BGR Trim For Extra1&5(15)\n");
    printf("|          | BGRWT2 ReDo(200)\n");
    printf("|  Debug   | BGR-Trim(300) / BGR_V vs Tap(301) / BGR_V Dist(302)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Other   | EXIT (e) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    //TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf(" Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();    

    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);

    switch(test_cond){
      case EXIT:
          return(PASS);
        break;
      case QUIT:
        DeviceSpecificPowerDown();
        if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
        PowerDown(1);
        break;
	  case 0:
		printf("ETLR00 = 0x%X\n",GetValue(IN_ETLR00,param));
		printf(" Enter New Code -> ");DigitInput(&tempo1);
		SetValue(IN_ETLR00,param,tempo1);
	  break;
	  case 1:
		printf("ETLR01 = 0x%X\n",GetValue(IN_ETLR01,param));
		printf(" Enter New Code -> ");DigitInput(&tempo1);
		SetValue(IN_ETLR01,param,tempo1);
	  break;
	  case 10:
        DeviceSpecificPowerUp();

        stack0 = SetValue(IN_REG,param,CHIPDATA);
        stack1 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"Register Read") ) {
          SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
          SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
          SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param));
          SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param));
          SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param));
          SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param));
          SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);
		  
		  extra1_etlr00 = GetValue(IN_ETLR00,param);
          extra1_etlr01 = GetValue(IN_ETLR01,param);
		 }
	  	  SetValue(IN_TESTSEL,param,stack0);
	      SetValue(IN_REG,param,stack0);

	  break;
	  case 11:
        DeviceSpecificPowerUp();

        stack0 = SetValue(IN_REG,param,CHIPDATA);
        stack1 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"Register Read") ) {
          /*SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param));*/ SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
          SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
          SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param));
          SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param));
          SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param));
          SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param));
          SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);
		  
		  extra1_etlr00 = GetValue(IN_ETLR00,param);
          extra1_etlr01 = GetValue(IN_ETLR01,param);
		  DeviceSpecificPowerUp();
	      // ------ Extra1 Write ------ //
	      SetValue(IN_TESTSEL,param,0);
	      SetValue(IN_BDATA00,param,0);
	      SetValue(IN_REG,param,3);
	  
	      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("settrimingdata_code_at"),"Write EXTRA Triming REGISTER");
	  	  SetValue(IN_TESTSEL,param,stack0);
	      SetValue(IN_REG,param,stack0);
	      }

	  
	  break;


      case 50:
        GetChipInfo( &info[0], param);
	
	
      break;
      case 51:

        // ------ Vcc Check ------ //
	switch(vs[0]){
	  case VCC_ARRAY0:tempo1 = 0;break;
	  case VCC_ARRAY1:tempo1 = 1;break;
	  case VCC_ARRAY2:tempo1 = 2;break;
	  case VCC_ARRAY3:tempo1 = 3;break;
	  case VCC_ARRAY4:tempo1 = 4;break;
	  default:
	    printf("\n Vcc Not Correct!!! Please Change Vcc Level!!!\n\n");
	    result = FAIL;
	  break;
	}
	if(result == PASS){
          
	  // ------ Register Check ------ //
	  printf("\n");
          printf("====== Check Register!!! ======\n");
          printf("    *** Vcc = %4d mV  ***      \n",vs[0]);
          printf("    Before         After       \n");
	  printf("  ETLR00=0x%X -> ETLR00=0x%X   \n",Extra5_ETLR00[tempo1],GetValue(IN_ETLR00,param));
	  printf("  ETLR01=0x%X -> ETLR01=0x%X   \n",Extra5_ETLR01[tempo1],GetValue(IN_ETLR01,param));
	  printf("===============================\n");
	  printf(" Check OK(123) / NG(0) ->");DigitInput(&tempo2);
	  if(123 != tempo2 ) break;
          
	  
	  // SetValue
          stack0 = SetValue(IN_TESTSEL,param,0);
          stack1 = SetValue(IN_BDATA00,param,0);
          stack3 = SetValue(IN_REG,param,0);
        
	  
	  // ------ Register Read ------ //
	  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"Register Read");
          // SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param));
	  SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
          SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
          SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param));
          SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param));
          SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param));
          SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param));
	  
	  etlr00_bak = GetValue(OUT_ETLR00,param);
	  etlr01_bak = GetValue(OUT_ETLR01,param);
	  
	  // ------ Extra1&5 Write ------ //
	  SetValue(IN_TESTSEL,param,1);
	  SetValue(IN_BDATA00,param,1);
	  SetValue(IN_BDATA01,param,tempo1);
	  SetValue(IN_REG,param,3);
	  
	  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("settrimingdata_code_at"),"Write EXTRA Triming REGISTER");

	  // ------ Extra1 ReWrite ------ //
	  SetValue(IN_TESTSEL,param,0);
	  SetValue(IN_BDATA00,param,0);
	  SetValue(IN_REG,param,3);
	  
	  stack1 = SetValue(IN_ETLR00,param,etlr00_bak);
	  stack2 = SetValue(IN_ETLR01,param,etlr01_bak);
	  
	  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("settrimingdata_code_at"),"Write EXTRA Triming REGISTER");

          SetValue(IN_ETLR00,param,stack1);
	  SetValue(IN_ETLR01,param,stack2);
		Extra5_ETLR00[tempo1] = GetValue(IN_ETLR00,param);
		Extra5_ETLR01[tempo1] = GetValue(IN_ETLR01,param);
        }
	
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_BDATA00,param,stack1);
        SetValue(IN_BDATA01,param,stack2);
	SetValue(IN_REG,param,stack3);      
      break;
      case 15:

        // ------ Vcc Check ------ //
	switch(vs[0]){
	  case VCC_ARRAY0:tempo1 = 0;break;
	  case VCC_ARRAY1:tempo1 = 1;break;
	  case VCC_ARRAY2:tempo1 = 2;break;
	  case VCC_ARRAY3:tempo1 = 3;break;
	  case VCC_ARRAY4:tempo1 = 4;break;
	  default:
	    printf("\n Vcc Not Correct!!! Please Change Vcc Level!!!\n\n");
	    result = FAIL;
	  break;
	}
	
	if(result == PASS){
	  // ------ Target Input & Check ------ //
	  printf("\n");
          printf(" Enter Vrsg(BGR) Target =>");DigitInput(&target);
	  
	  printf("==== Check Vrsg Target ====\n");
	  printf("  Target = %d mV   \n",target);
	  printf("===========================\n");
	  printf(" Check OK(123) / NG(0) ->");DigitInput(&tempo2);
	  if( 123 != tempo2 ) break;
          
	  //  ------- BGR Trim & EXTRA Write ------- //
	  BgrTrim_RegWrite(0, tempo1, param, target);
	
	  //  ------- Chip Info Get ------- //
	  GetChipInfo( &info[0], param);
	  
	  //  ------- Vrsg Monitor ------- //
	  VrsgMonitor_Single(tempo1, param, &Vresult);
	  
	  //  ------- Trimming Pass/Fail ------- //
	  if( (Vresult >= (target-8)) && (Vresult <= (target+8))){
	    printf("**** Trimming PASS ****\n");
	  }else{
	    printf("---- Trimming FAIL !!! ----\n");
	  }
		  extra1_etlr00 = GetValue(IN_ETLR00,param);
          extra1_etlr01 = GetValue(IN_ETLR01,param);
	
	}
      break;
	  /*
      case 200:
        ParamInitialize(param_ini);
        BGR_WT_HT(param_ini);
      break;
	  */
      case 300:
        printf("Select Trimming Voltage Ver VRSG(0)/BGR(1)");
	DigitInput(&tempo1);
        printf("Input Tirimming Voltage[mV] -> ");
	DigitInput(&target);
	
        SetValue(IN_BDATA01,param,tempo1);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_BDATA02,param,3);
	SetValue(IN_TESTSEL,param,AUTOTRIM_BGR08);
	SetValue(IN_LDATA0,param,100);
	
	switch(tempo1){
	  case 0:
	  case 3:
	    monitor_pin = VRSGMON_APP;
	  break;
	  case 1:
	  case 2:
	  case 4:
	    monitor_pin = VSSMON_APP;
          break;
	}
	
        result = CpuModeMonitor_Vrsg(monitor_pin,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingbgr_flashcomp_at"),"BGR_Trim",&target);
	
	printf("\nResult = 0x%X\n",(GetValue(OUT_ETLR00,param) & 0x7F));
	
        SetValue(IN_LDATA0,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA00,param,0);
	SetValue(IN_TESTSEL,param,0);
	
      break;
      case 301:

	 for(k=1;k<=2;k++){
        
        switch(k){
          case 1:
            sprintf(testname,"BGR_Mode");
	    tempo1=1;
	    loop_max = 850;
	    loop_min = 750;
	    loop_delta = 2;
	    monitor_pin = VSSMON_APP;
          break;
          case 2:
            sprintf(testname,"Vrsg_Mode");
	    tempo1=0;
	    loop_max = 3600;
	    loop_min = 3000;
	    loop_delta = 5;
	    monitor_pin = VRSGMON_APP;
          break;
	  /*
          case 3:
            sprintf(testname,"Vreg20_Mode");
	    tempo1=2;
	    loop_max = 2300;
	    loop_min = 1800;
	    loop_delta = 2;
	    monitor_pin = VSSMON_APP;
          break;
	  */
	}  
	  
	    
        SetValue(IN_BDATA01,param,tempo1);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_BDATA02,param,3);
	SetValue(IN_TESTSEL,param,AUTOTRIM_BGR08);
	SetValue(IN_LDATA0,param,100);
	
	first_flag = 1;
	DeviceSpecificPowerUp();
	
	
        for(loop=loop_min;loop<=loop_max;loop+=loop_delta){
	    Ffpt= 0;
	repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
	  if(first_flag==1){// Initial mode
	    Ffpt = 1;
	    first_flag = 0;
            repeat_mode = repeat_mode | INITIAL_SET;
	  }
          CpuModeMonitor_Repeat(repeat_mode,monitor_pin, &vs[0], param,W1SEC*waittime,CreatePatFileName("trimingbgr_flashcomp_at"),"BGR_Trim",&loop);
	  
	  result = GetValue(OUT_JUDGE1,param);
	  tempo3 = (GetValue(OUT_ETLR00,param) & 0x7F); // tempo3 =result
	  printf("%s  Loop = %d  Result = 0x%X\n",testname,loop,tempo3);

          // ********  File Out  ******* //
          sprintf( buff, "%s%s%s_W%d_X%dY%d_BGRTrim_data_%s.csv", glob_cdp, glob_datalog,lotname,atoi(wafername),xy_location[0],xy_location[1],testname );
          if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
          delay_timer(5*1000);
	  if(loop==loop_min && i == 1){

	    fprintf(Fptdata,"inputVol[mV],ETLR00,ETLR01\n");
	  }
        
	  if(result == PASS){
	    fprintf(Fptdata,"%d,%X,%X\n",loop,GetValue(OUT_ETLR00,param),GetValue(OUT_ETLR01,param));
          }else{
	    fprintf(Fptdata,"%d,FAIL,FAIL",loop);
	  }
          delay_timer(5*1000);
          if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
          delay_timer(200);
	}// Loop
        
	
	DeviceLevelsPowerDown(); //各電源OFFコマンド
        DeviceSpecificPowerDown();
	Ffpt= 1;
	}//k Loop

      break;
      case 302:

        printf("Select Trimming Voltage Ver VRSG(0)/BGR(1)");
	DigitInput(&tempo1);
        printf("Input Tirimming Voltage[mV] -> ");
	DigitInput(&target);
	
        SetValue(IN_BDATA01,param,tempo1);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_BDATA02,param,1);
	SetValue(IN_TESTSEL,param,AUTOTRIM_BGR08);
	SetValue(IN_LDATA0,param,100);
	
        switch(tempo1){
          case 1:
            sprintf(testname,"BGR_Mode");
	    monitor_pin = VSSMON_APP;
          break;
          case 0:
            sprintf(testname,"Vrsg_Mode");
	    monitor_pin = VRSGMON_APP;
          break;
	  /*
          case 2:
            sprintf(testname,"Vreg20_Mode");
	    monitor_pin = VSSMON_APP;
          break;
	  */
	}  
	
	first_flag = 1;
	DeviceSpecificPowerUp();
	
	
        for(loop=1;loop<=100;loop+=1){
	    Ffpt= 0;
	repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
	  if(first_flag==1){// Initial mode
	    Ffpt = 1;
	    first_flag = 0;
            repeat_mode = repeat_mode | INITIAL_SET;
          }

          CpuModeMonitor_Repeat(repeat_mode,monitor_pin, &vs[0], param,W1SEC*waittime,CreatePatFileName("trimingbgr_flashcomp_at"),"BGR_Trim",&target);
	  
	  result = GetValue(OUT_JUDGE1,param);
	  tempo3 = (GetValue(OUT_ETLR00,param) & 0x7F); // tempo3 =result
	  printf("%s  Loop = %d  Result = 0x%X\n",testname,loop,tempo3);
          
	  // ********  File Out  ******* //
          sprintf( buff, "%s%s%s_W%d_X%dY%d_BGRTrim_Dist_%s.csv", glob_cdp, glob_datalog,lotname,atoi(wafername),xy_location[0],xy_location[1],testname );
          if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
          delay_timer(5*1000);
	  if(loop==loop_min && i == 1){

	    fprintf(Fptdata,"Loop,ETLR00,ETLR01\n");
	  }
        
	  if(result == PASS){
	    fprintf(Fptdata,"%d,%X,%X\n",loop,GetValue(OUT_ETLR00,param),GetValue(OUT_ETLR01,param));
          }else{
	    fprintf(Fptdata,"%d,FAIL,FAIL",loop);
	  }
          delay_timer(5*1000);
          if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
          delay_timer(200);
	}// Loop
        
	
	DeviceLevelsPowerDown(); //各電源OFFコマンド
        DeviceSpecificPowerDown();
	Ffpt= 1;

      break;
      
      default:
      break;


    }
  }
}





int CurrentAllDataOut(int mode, int vs[NUM_POWERSUPPLY], int Temp, interface_t* param, char *file_name_part)
{
  interface_t*  param1;
  int i,j,freq_num,result,waittime,current,data_num,ffpt_bak;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int current_data[20],current_array[20][50];
  int info[NUM_CHIPINFO];       /* wmat chip information    */
  char chip_name[100],Lotname[50],file_name[200], buff[300];
  char wgl_name[150],pat_name[100];  
  //Initial
  ChangeAddress(FLP0,0x00000,0x7FFFF,param);
  param1 = param;
  waittime = 30;
  result = PASS;

  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 =0;

  for(i=0;i<=19;i++){
    current_data[i]=0;
  }

  DeviceSpecificPowerUp();

  // ********  Chip Info Get  ******* //
  result = GetChipInfo(&info[0],param);
  sprintf(Lotname,"%c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
  sprintf(chip_name,"%s_W%dS%d",Lotname,info[10],info[20]);
  if(result !=PASS )return(FAIL);

  // ********  File Name Setting  ******* //
  sprintf(file_name,"%s_%s_Rev_%d",chip_name,file_name_part,0);
  sprintf( buff, "%s%s%s%s.csv", glob_cdp, glob_datalog,glob_currentlog, file_name );


  // ********  Freq Info Get  ******* //
  switch(get_site_number()){
    case 0: freq_num =  136; break; // 1.36MHz
    case 1: freq_num =  800; break; // 8.00MHz
    case 2: freq_num = 1300; break; //13.00MHz
    case 3: freq_num = 2000; break; //20.00MHz
  }

  switch(mode){
     case 0:
        // ********  Current Mesure  ******* //
        /* Read電流の自動取得をコメントアウト
	ChangeAddress(FLP0,0x00000,0xFFF,param);

        SetValue(IN_TESTSEL,param,0);
        DeviceSpecificPowerUp();
	result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programall_code_at"),"Program ALL");
	if(result !=PASS )return(FAIL);

        MinoriRead_Kani(WHILE1_ALL0 | READ_ON , &vs[0], param, &current);
        current_data[2] = current;
        MinoriRead_Kani(WHILE1_ALL0 | READ_OFF, &vs[0], param, &current);
        current_data[3] = current;
	
	ChangeAddress(FLP0,0x00000,0x7FFFF,param);

        SetValue(IN_BDATA00,param,0);
	DeviceSpecificPowerUp();
        result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*10,param,CreatePatFileName("erase_code_at"),"Erase");
	if(result !=PASS )return(FAIL);

        MinoriRead_Kani(WHILE1_ALL1 | READ_ON , &vs[0], param, &current);
        current_data[0] = current;
        MinoriRead_Kani(WHILE1_ALL1 | READ_OFF, &vs[0], param, &current);
        current_data[1] = current;

	ChangeAddress(FLP0,0x00000,0x7FFFF,param);
        
	SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
	DeviceSpecificPowerUp();
        result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programchecker_code_at"),"Program CHECKER");
	if(result !=PASS )return(FAIL);

        MinoriRead_Kani(NORMAL_READ | READ_ON , &vs[0], param, &current);
        current_data[4] = current;
        MinoriRead_Kani(NORMAL_READ | READ_OFF, &vs[0], param, &current);
        current_data[5] = current;




        // ********  File Out  ******* //
        if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
        delay_timer(5*1000);
        fprintf(Fptdata,"Vcc[mV],%d,Vddh[mV],%d,Vdd[mV],%d",vs[0],vs[1]*25/10,vs[2]*79/50);
        fprintf(Fptdata,"Temp,%d,Freq[MHz],%d.%d",Temp,freq_num/100,freq_num%100);
        fprintf(Fptdata,",While1_ALL0_ON ,%d",current_data[0]);
        fprintf(Fptdata,",While1_ALL0_OFF,%d",current_data[1]);
        fprintf(Fptdata,",While1_ALL1_ON ,%d",current_data[2]);
        fprintf(Fptdata,",While1_ALL1_OFF,%d",current_data[3]);
        fprintf(Fptdata,",Normal_CHKA_ON ,%d",current_data[4]);
        fprintf(Fptdata,",Normal_CHKA_OFF,%d",current_data[5]);
        fprintf(Fptdata,",Ramdom_CHKA_ON ,%d",current_data[6]);
        fprintf(Fptdata,",Ramdom_CHKA_OFF,%d",current_data[7]);
        fprintf(Fptdata,",\n");
        delay_timer(5*1000);
        if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
        delay_timer(200);
     break;
     */
     case 1:
     
     
        // Read-Idle 
        SetValue(IN_TESTSEL,param,0x00);/* 0x00 pass Only */
        SetValue(IN_BDATA00,param,0);
        CpuModeMonitor_current2((ADC_AVE | CRY_CLK),MNORMAL,&vs[0],200,param,CreatePatFileName("dc_current_at"),"Read-Idle-Current",&current);  /*ファーム次第で変更する可能性あり*/
        current_data[0]=current;
     
        // Flash PowerOFF
        SetValue(IN_TESTSEL,param,0x00);/* 0x00 pass Only */
        SetValue(IN_BDATA00,param,1);
        DeviceSpecificPowerUp();
	CpuModeMonitor_current2((ADC_AVE | CRY_CLK),MNORMAL,&vs[0],200,param,CreatePatFileName("dc_current_at"),"PowerOff-Current",&current);  /*ファーム次第で変更する可能性あり*/
        current_data[1]=current;
       
       
       
        // ********  File Out  ******* //
        if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
        delay_timer(5*1000);
        fprintf(Fptdata,"Vcc[mV],%d,Vddh[mV],%d,Vdd[mV],%d",vs[0],vs[1]*25/10,vs[2]*79/50);
        fprintf(Fptdata,"Temp,%d,Freq[MHz],%d.%d",Temp,freq_num/100,freq_num%100);
        fprintf(Fptdata,",Read-Idle_Flash_ON ,%d",current_data[0]);
        fprintf(Fptdata,",Read-Idle_Flash_OFF,%d",current_data[1]);
        fprintf(Fptdata,",\n");
        delay_timer(5*1000);
        if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
        delay_timer(200);
     
     
     break;
     
     case 2:
     
     
        // Flash-STBY 
        SetValue(IN_TESTSEL,param,0x00);/* 0x00 pass Only */
        SetValue(IN_BDATA00,param,2);
        CpuModeMonitor_current2((ADC_AVE),MNORMAL,&vs[0],200,param,CreatePatFileName("dc_current_at"),"Flash-STBY-Current",&current);  /*ファーム次第で変更する可能性あり*/
        current_data[0]=current;
     
        // Flash PowerOFF
        SetValue(IN_TESTSEL,param,0x00);/* 0x00 pass Only */
        SetValue(IN_BDATA00,param,3);
        DeviceSpecificPowerUp();
	CpuModeMonitor_current2((ADC_AVE),MNORMAL,&vs[0],200,param,CreatePatFileName("dc_current_at"),"SSTBY2-PowerOFF-Current",&current);  /*ファーム次第で変更する可能性あり*/
        current_data[1]=current;
       
       
       
        // ********  File Out  ******* //
        if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
        delay_timer(5*1000);
        fprintf(Fptdata,"Vcc[mV],%d,Vddh[mV],%d,Vdd[mV],%d",vs[0],vs[1]*25/10,vs[2]*79/50);
        fprintf(Fptdata,"Temp,%d,Freq[MHz],%d",Temp,0);
        fprintf(Fptdata,",Flash_STBY_ON ,%d",current_data[0]);
        fprintf(Fptdata,",Flash_STBY_OFF,%d",current_data[1]);
        fprintf(Fptdata,",\n");
        delay_timer(5*1000);
        if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
        delay_timer(200);
     
     
     break;
     
     case 3:
        stack3 = SetValue(IN_ETCR11,param,GetValue(IN_ETCR11,param) | 0x02); // PE-Bottom Stop

        data_num = 20;
        
        result = CpuModeMonitor_current2(0,MVCCMON,&vs[0],param,250,CreatePatFileName("programall_code_at_current"),"PE_Bottom_Dummy",&current);  /*ファーム次第で変更する可能性あり*/
        if(result != PASS)return(FAIL);
        for(i=1;i<=data_num;i++){
        for(j=1;j<=8;j++){
		switch(j){
		  case 1:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,2);
			sprintf(pat_name,"programall_code_at_current");
		    break;
		  case 2:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,3);
			sprintf(pat_name,"programall_code_at_current");
		    break;
		  case 3:
		    SetValue(IN_TESTSEL,param,1);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,2);
			sprintf(pat_name,"programall_code_at_current");
		    break;
		  case 4:
		    SetValue(IN_TESTSEL,param,1);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,3);
			sprintf(pat_name,"programall_code_at_current");
		    break;
		  case 5:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,1);
		    SetValue(IN_BDATA05,param,2);
			sprintf(pat_name,"erase_code_at_current");
		    break;
		  case 6:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,1);
		    SetValue(IN_BDATA05,param,3);
			sprintf(pat_name,"erase_code_at_current");
		    break;
		  case 7:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,2);
			sprintf(pat_name,"erase_code_at_current");
		    break;
		  case 8:
		    SetValue(IN_TESTSEL,param,0);
			SetValue(IN_BDATA00,param,0);
		    SetValue(IN_BDATA05,param,3);
			sprintf(pat_name,"erase_code_at_current");
		    break;
        }		   
        ffpt_bak=Ffpt;
        Ffpt=0;
          result = CpuModeMonitor_current2(0,MVCCMON,&vs[0],param,300,CreatePatFileName(pat_name),"PE_Bottom",&current);  /*ファーム次第で変更する可能性あり*/
        Ffpt = ffpt_bak;
          if(result != PASS)break;
        current_array[j][i]=current;
        }
		if(result != PASS)break;
		}
       
        // ********  File Out  ******* //
        if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "\n\nOpenDataOutFile Fptdata Open Err[%s]\n\n", buff ); exit( -99 ); }
        delay_timer(5*1000);
        fprintf(Fptdata,"Vcc[mV],%d,Vddh[mV],%d,Vdd[mV],%d",vs[0],vs[1]*25/10,vs[2]*79/50);
        fprintf(Fptdata,"Temp,%d,Freq[MHz],%d\n",Temp,0);
	for(i=1;i<=data_num;i++){
          fprintf(Fptdata,",ALL0_Write_Bottom_ON ,%d",current_array[1][i]);
          fprintf(Fptdata,",ALL0_Write_Bottom_OFF,%d",current_array[2][i]);
          fprintf(Fptdata,",ALL1_Write_Bottom_ON ,%d",current_array[3][i]);
          fprintf(Fptdata,",ALL1_Write_Bottom_OFF,%d",current_array[4][i]);
          fprintf(Fptdata,",Erase_Bottom_ON ,%d",  current_array[5][i]);
          fprintf(Fptdata,",Erase_Bottom_OFF,%d",  current_array[6][i]);
          fprintf(Fptdata,",PreWrite_Bottom_ON ,%d",     current_array[7][i]);
          fprintf(Fptdata,",PreWrite_Bottom_OFF,%d",     current_array[8][i]);
        }
	fprintf(Fptdata,",\n");
        delay_timer(5*1000);
        if( fclose( Fptdata ) ){ printf( "\n\nCloseDataOutFile Fptdata Close Err\n\n" ); exit( -99 ); }
        delay_timer(200);
     
	SetValue(IN_ETCR11,param,stack3);
     
     break;
     
     
     
  }
  return(PASS);


}


int GetChipInfo(int info[NUM_CHIPINFO],interface_t* param)
{
  int stack0,stack1,stack2,stack3,stack4,stack5;
  int vcc[NUM_POWERSUPPLY];
  int waittime,result;
  
  waittime = 5;
  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = 0;
  vcc[0] = 3300;         // Vcc = 3.3V
  vcc[1] = vcc[2] = 500; // Vddh/Vdd=typ

        stack0 = SetValue(IN_AREA,param,5);	 /* Extra5 */
        stack1 = SetValue(IN_TOPADDR0,param,0);
        stack2 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);

        result = CpuModeFunc(MNORMAL,MSNORMAL,&vcc[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"Chip-Info Get");  /*ファーム次第で変更する可能性あり*/



        info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B);
        info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
        info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B);
        info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
        info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B);
        info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
        info[6] = ReadEcrMode8(FBM_READTOP+0x0026,SIZE_B);
        info[10]= ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
        info[11]= ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B);
        info[12]= ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);
        info[13]= ReadEcrMode8(FBM_READTOP+0x0098,SIZE_B);
        info[14]= ReadEcrMode8(FBM_READTOP+0x0099,SIZE_B);
        info[15]= ReadEcrMode8(FBM_READTOP+0x009A,SIZE_B);
        info[16]= ReadEcrMode8(FBM_READTOP+0x009B,SIZE_B);
        info[17]= ReadEcrMode8(FBM_READTOP+0x009C,SIZE_B);
        info[18]= ReadEcrMode8(FBM_READTOP+0x009D,SIZE_B);
        info[19]= ReadEcrMode8(FBM_READTOP+0x009E,SIZE_B);
	
	Extra5_ETLR00[0] = ReadEcrMode8(FBM_READTOP+0x0310,SIZE_B);
	Extra5_ETLR01[0] = ReadEcrMode8(FBM_READTOP+0x0311,SIZE_B);
	Extra5_ETLR00[1] = ReadEcrMode8(FBM_READTOP+0x0312,SIZE_B);
	Extra5_ETLR01[1] = ReadEcrMode8(FBM_READTOP+0x0313,SIZE_B);
	Extra5_ETLR00[2] = ReadEcrMode8(FBM_READTOP+0x0314,SIZE_B);
	Extra5_ETLR01[2] = ReadEcrMode8(FBM_READTOP+0x0315,SIZE_B);
	Extra5_ETLR00[3] = ReadEcrMode8(FBM_READTOP+0x0316,SIZE_B);
	Extra5_ETLR01[3] = ReadEcrMode8(FBM_READTOP+0x0317,SIZE_B);
	Extra5_ETLR00[4] = ReadEcrMode8(FBM_READTOP+0x0318,SIZE_B);
	Extra5_ETLR01[4] = ReadEcrMode8(FBM_READTOP+0x0319,SIZE_B);
	
	sprintf(lotname,"%c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
	sprintf(wafername,"%d",info[10]);
	xy_location[0] = info[11];
	xy_location[1] = info[12];

	
	SetValue(IN_AREA,param,4);	 // Extra4 
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);

        result = CpuModeFunc(MNORMAL,MSNORMAL,&vcc[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"Chip-Info Get");

        info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
        SampleNo  = info[20];
        if(SampleNo >= 10000)SampleNo-=10000;

        SetValue(IN_AREA,param,stack0);
        SetValue(IN_TOPADDR0,param,stack1);
        SetValue(IN_TESTSIZE0,param,stack2);
        if(result == PASS) return(PASS);
        else{
          printf("\n --- Chip Info Read Fail !! --- \n");

	  return(result);
	  
        }

}

/******************************************************************************/
/* int CP vs Volall                                                              */
/******************************************************************************/
int CPvsVolall(int mode,int vs[NUM_POWERSUPPLY], int waittime, interface_t* param)
{
  int i,j,max_vol,min_vol,delta_vol,result,selvol,delta_vol2,drop_vol;
  int terminal,tempo1,change_reg,pmu_range,first_flag;
  char sitenum[10],filename[256],filename2[256];
  int  target_vol[256];
  FILE_T *fpt;
	
	
	for(j=0;j<=255;j++){
		target_vol[j]=0;
	}
  first_flag = 0;
  j = 0;
  SetValue(IN_BDATA15,param,UA250);
  pmu_range = GetValue(IN_BDATA15,param); 
  	
/*-----------------------------------------------------*/
/*  Initialize                                         */
/*-----------------------------------------------------*/
  switch(mode){
    case V_VPP_E: //mode=7
      sprintf(filename,"vpp(erase)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
      terminal = VSSMON_CPSUP;
	     change_reg = IN_ETLR20;
      tempo1 = SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param)&0x0F)|0xF0);
  	   max_vol = -3000;
  	   min_vol = -1500;
   	   delta_vol = 400;
   	   delta_vol2 = 100;
	   drop_vol = -2700;
  	   break;
    case V_VPP_P: //mode=8
      sprintf(filename,"vpp(write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
      terminal = VSSMON_CPSUP;
	     change_reg = IN_ETLR20;
      tempo1 = SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param)&0xF0)|0x0F);
  	   max_vol = -3600;
  	   min_vol = -2100;
  	   delta_vol = 400;
   	   delta_vol2 = 100;
	   drop_vol = -3300;
      break;
    case V_VPP_PW: //mode=9
      sprintf(filename,"vpp(pre-write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_PW);
      terminal = VSSMON_CPSUP;
      change_reg = IN_ETLR21;
      tempo1 = SetValue(IN_ETLR21,param,(GetValue(IN_ETLR21,param)&0x0F)|0xF0);
  	   max_vol = -2500;
  	   min_vol = -1000;
  	   delta_vol = 400;
   	   delta_vol2 = 100;
	   drop_vol = -2300;
      break;
    case V_VHH_E: //mode=21
      sprintf(filename,"vhh(erase)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR16;
      tempo1 = SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param)&0xC0)|0x3F);
  	   max_vol = 8400;
  	   min_vol = 5700;
  	   delta_vol = 600;
   	   delta_vol2 = 100;
	   drop_vol = 7500;
      break;
    case V_VHH_P: //mode=22
      sprintf(filename,"vhh(write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR17;
      tempo1 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x3F);
  	   max_vol = 9600;
  	   min_vol = 6400;
  	   delta_vol = 800;
   	   delta_vol2 = 200;
	   drop_vol = 8800;
      break;
    case V_VRSG: //mode=30
      sprintf(filename,"vrsgvsCP");
  	   SetValue(IN_TESTSEL,param,MONITOR_VRSG);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR18;
      tempo1 = SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param)&0xF8)|0x06);
  	   max_vol = 3400;
  	   min_vol = 2600;
  	   delta_vol = 200;
   	   delta_vol2 = 100;
	   drop_vol = 3100;
      break;
    case V_V33R: //mode=31
      sprintf(filename,"v33rvsCP");
      SetValue(IN_TESTSEL,param,MONITOR_V33R);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR24;
      tempo1 = SetValue(IN_ETLR24,param,(GetValue(IN_ETLR24,param)&0xF8)|0x07);
  	   max_vol = 3600;
  	   min_vol = 2900;
  	   delta_vol = 150;
   	   delta_vol2 = 150;
	   drop_vol = 3300;
      break;
    case V_VWI: //mode=32
      sprintf(filename,"vwivsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VWI);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR19;
      tempo1 = SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0x0F)|0xF0);
  	   max_vol = 3500;
  	   min_vol = 2000;
  	   delta_vol = 400;
   	   delta_vol2 = 100;
	   drop_vol = 3200;
      break;
    case V_VNOEMI_PE: //mode=36
      sprintf(filename,"vnoemi(PE)vsCP");
  	   SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR19;
      tempo1 = SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0xF0)|0x0F);
  	   max_vol = 3900;
  	   min_vol = 2400;
  	   delta_vol = 400;
   	   delta_vol2 = 100;
	   drop_vol = 3200;
      break;
    default:
  	   printf("!!!!!!!!!!! No select !!!!!!!!!!!!!\n");
    	 return(FAIL);  
  }

	 if(mode > 10){
	   for(selvol=min_vol; selvol<max_vol;selvol= selvol + delta_vol){
         target_vol[j] = selvol;
	     j++;
	   	if(selvol >= drop_vol){
	   		delta_vol = delta_vol2;
	   	}
	   }
	 	
       if(target_vol[j-1] != max_vol - 100){
	     target_vol[j] = max_vol - 100;
    	 target_vol[j+1] = max_vol;
	   }else{
         target_vol[j] = max_vol;
       }
	 }else{
	   target_vol[j] = max_vol;
       target_vol[j+1] = max_vol+100;
	   for(selvol=max_vol +100 + delta_vol2; selvol<min_vol;selvol= selvol + delta_vol2){

         target_vol[j+2] = selvol;
	     j++;
	   	if(selvol >= drop_vol){
	   		delta_vol2 = delta_vol;
	   	}
	   }
      target_vol[j+2] = min_vol;
	 }

/*-----------------------------------------------------*/
/* Generate Data Log File Name & Open This File        */
/*-----------------------------------------------------*/
sprintf(filename2,"%s_W%d_X%dY%d_",lotname,atoi(wafername),xy_location[0],xy_location[1]);
  sprintf(sitenum,"_%d.csv",get_site_number());
  strcpy(datapat,glob_cdp);
  strcat(datapat,glob_datalog);
  strcat(datapat,filename2);
  strcat(datapat,filename);
  strcat(datapat,sitenum);

  if((fpt=fopen(datapat,"at"))==NULL){
    ErrorPrint(datapat);
    return(FAIL);
  }

  fprintf(fpt,"MaxVol=%d[mV],MinVol=%d[mV]\n",max_vol,min_vol);
  fprintf(fpt,"ETLR%02d:%02X,chargepump[uA] (Vcc:%d Vddh:%d Vdd:%d)\n",change_reg,GetValue(change_reg,param),vs[0],vs[1],vs[2]);
  
  Ffpt = 0;
  
  for ( j = 0 ; j<20 ; j++ ){
    DeviceSpecificPowerUp();
	   selvol = target_vol[j];
    CpuModeMonitor(terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"CP_Voltage_dependency",&selvol);
  	 if(mode > 10){
  		  selvol = selvol * (-1);  //vpp以外は正負を反転する
  	 }
  	 
  	 if(pmu_range == UA25){  //UA25
  	 	 if(-24000 < selvol && selvol <= 24000){
  	 	   fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
  	 	 }else if(24000 <selvol && selvol < 240000){
  	 	 	 SetValue(IN_BDATA15,param,UA250);  //ChangeReg:UA250
			     pmu_range = GetValue(IN_BDATA15,param);
  	 	 	 j = j - 1;
			     if(first_flag == 1){
			       j = j + 1;
  	 	     fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
          first_flag = 0;
        }
        first_flag = 1;
  	 	 }else if(selvol < -24000){
		   	  SetValue(IN_BDATA15,param,UA250);
	   		  pmu_range = GetValue(IN_BDATA15,param);
  	 	 	 j = j - 1;
			     if(first_flag == 1){
			       j = j + 1;
  	 	     fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
          first_flag = 0;
        }
        first_flag = 1;
		    }
  	 }else if(pmu_range == UA250){
  	   if(24000 <=selvol && selvol < 240000){
  	 	   fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
  	   }else if(240000 < selvol){
  	     SetValue(IN_BDATA15,param,UA2500);
		      pmu_range = GetValue(IN_BDATA15,param);
  	    	j = j - 1;
			     if(first_flag == 1){
			       j = j + 1;
  	 	     fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
          first_flag = 0;
        }
        first_flag = 1;
				  }else if(-24000 < selvol && selvol < 24000){
				    SetValue(IN_BDATA15,param,UA25);
		      pmu_range = GetValue(IN_BDATA15,param);
				    j = j - 1;
			     if(first_flag == 1){
			       j = j + 1;
  	 	     fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
          first_flag = 0;
        }
        first_flag = 1;
				  }else if(selvol < -24000){
				    fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
  	   }
  	 }else if(pmu_range == UA2500){
	     if(240000 <=selvol || selvol < -240000){
  	 	   fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
		    }else if(selvol <240000 && -240000 < selvol){
			     SetValue(IN_BDATA15,param,UA250);
		      pmu_range = GetValue(IN_BDATA15,param);
			  	  j = j - 1;
			     if(first_flag == 1){
			       j = j + 1;
  	 	     fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",target_vol[j],selvol);
          first_flag = 0;
        }
        first_flag = 1;
		    }
    }
    printf("\nresult = %d[nA]\n\n",selvol);
    if(target_vol[j+1] == 0) break;
  }
  fprintf(fpt,"\n");
  /*-----------------------------------------------------*/
  /* Close Data File                                     */
  /*-----------------------------------------------------*/
  if(fclose(fpt)){
    exit(-10);
  }
  
  Ffpt =1;
  SetValue(change_reg,param,tempo1);
  return( PASS );

}

/******************************************************************************/
/*                                                                            */
/* int CP vs Vol Detail evaluation for PumpCurrent                            */
/*                                                                            */
/******************************************************************************/
int CPvsVol(int mode,int vs[NUM_POWERSUPPLY], int waittime, interface_t* param)
{
  int i,j,max_vol,min_vol,delta_vol,result,selvol,stack1,stack2;
  int terminal,tempo1,change_reg,pmu_range,first_flag;
  int start_volt,step_volt,meascon,next_volt;
  char sitenum[10],filename[256];
  int  target_vol[256];
  char pat_name[128];
  FILE_T *fpt;

  first_flag = 0;
  j = 0;
  
  start_volt = 0;
  step_volt = 0;
  meascon = 0;
  next_volt = 0;
  sprintf(pat_name,"monitorvoltage_at");

  stack1 = SetValue(IN_BDATA15,param,UA250);
  printf("xxxxxxxxxxxxxxxxxxxxx  BDATA00:%08X, LDATA0:%08X\n",GetValue(IN_BDATA00,param),GetValue(IN_LDATA0,param));  
  pmu_range = GetValue(IN_BDATA15,param); 
  	
/*-----------------------------------------------------*/
/*  Initialize                                         */
/*-----------------------------------------------------*/
  switch(mode){
    case V_VPP_E: //mode=7
      sprintf(filename,"vpp(erase)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
      terminal = VSSMON_CPSUP;
	     change_reg = IN_ETLR20;
      tempo1 = SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param)&0x0F)|0xF0);
  	   break;
    case V_VPP_P: //mode=8
      sprintf(filename,"vpp(write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
      terminal = VSSMON_CPSUP;
	     change_reg = IN_ETLR20;
      tempo1 = SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param)&0xF0)|0x0F);
      break;
    case V_VPP_PW: //mode=9
      sprintf(filename,"vpp(pre-write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_PW);
      terminal = VSSMON_CPSUP;
      change_reg = IN_ETLR21;
      tempo1 = SetValue(IN_ETLR21,param,(GetValue(IN_ETLR21,param)&0x0F)|0xF0);
      break;
    case V_VHH_E: //mode=21
      sprintf(filename,"vhh(erase)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR16;
      tempo1 = SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param)&0xC0)|0x3F);
      break;
    case V_VHH_P: //mode=22
      sprintf(filename,"vhh(write)vsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR17;
      tempo1 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x3F);
      break;
    case V_VRSG: //mode=30
      sprintf(filename,"vrsgvsCP");
 	  SetValue(IN_TESTSEL,param,MONITOR_VRSG);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR18;
      tempo1 = SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param)&0xF8)|0x06);
      break;
    case V_V33R: //mode=31
      sprintf(filename,"v33rvsCP");
      SetValue(IN_TESTSEL,param,MONITOR_V33R);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR24;
      tempo1 = SetValue(IN_ETLR24,param,(GetValue(IN_ETLR24,param)&0xF8)|0x07);
      break;
    case V_VWI: //mode=32
      sprintf(filename,"vwivsCP");
      SetValue(IN_TESTSEL,param,MONITOR_VWI);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR19;
      tempo1 = SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0x0F)|0xF0);
      break;
    case V_VNOEMI_PE: //mode=36
      sprintf(filename,"vnoemi(PE)vsCP");
  	   SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
      terminal = VCCMON_CPSUP;
      change_reg = IN_ETLR19;
      tempo1 = SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0xF0)|0x0F);
      break;
    case V_VHH_E_LEAK: //mode=121
  	   sprintf(filename,"vhh(erase)vsCP");
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_E);
      terminal = VCCMON_CPLK;
      change_reg = IN_ETLR16;
      tempo1 = SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param)&0xC0)|0x3F);
  	  stack2 = SetValue(IN_LDATA0,param,30*10000);
      sprintf(pat_name,"trimingvoltage_Vhh_at");
  	 break;
    case V_VHH_P_LEAK: //mode=122
      sprintf(filename,"vhh(write)vsCP");
  	   SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
      terminal = VCCMON_CPLK;
      change_reg = IN_ETLR17;
      tempo1 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x3F);
  	  stack2 = SetValue(IN_LDATA0,param,30*10000);
	  sprintf(pat_name,"trimingvoltage_Vhh_at");
  	 break;
  	
    default:
  	   printf("!!!!!!!!!!! No select !!!!!!!!!!!!!\n");
    	 return(FAIL);  
  }

/*-----------------------------------------------------*/
/* Generate Data Log File Name & Open This File        */
/*-----------------------------------------------------*/
  sprintf(sitenum,"_%d.csv",get_site_number());
  strcpy(datapat,glob_cdp);
  strcat(datapat,glob_datalog);
  strcat(datapat,filename);
  strcat(datapat,sitenum);

  if((fpt=fopen(datapat,"at"))==NULL){
    ErrorPrint(datapat);
    return(FAIL);
  }

      printf( "StartVoltage? (mV) -> ");
      DigitInput( &start_volt ); 
      printf( "StepVoltage? (mV) -> ");
      DigitInput( &step_volt ); 
      printf( "MesureNumber? (count) -> ");
      DigitInput( &meascon ); 

  fprintf(fpt,"Start_volt=%d[mV],StepVoltage=%d[mV],MesureNumber=%d\n",start_volt,step_volt,meascon);
  fprintf(fpt,"ETLR%02d:%02X,chargepump[uA] (Vcc:%d Vddh:%d Vdd:%d)\n",change_reg,GetValue(change_reg,param),vs[0],vs[1],vs[2]);

      next_volt =  start_volt; //Initial force voltage//
	
  for ( j = 0 ; j<meascon+1 ; j++ ){
    if(next_volt > 10000) break; //10V over Exit//
    if(next_volt < -5000) break; //-5V over Exit//
    selvol =  next_volt; //Extarnal force voltage//

    DeviceSpecificPowerUp();
    CpuModeMonitor(terminal,&vs[0],param,W1SEC*waittime,CreatePatFileName(pat_name),"CP_Voltage_dependency",&selvol);
  	 
  	if(pmu_range == UA2500){
  	  fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",next_volt,selvol);
	  SetValue(IN_BDATA15,param,UA250);
	  pmu_range = GetValue(IN_BDATA15,param);
  	}else if(pmu_range == UA250){
	  if(selvol <= 240000 && -240000 <= selvol){
  	    fprintf(fpt,"TargetVoltage:%d[mV], %d,[nA]\n",next_volt,selvol);
  	  }else if(240000 < selvol || selvol < -240000){
  	    SetValue(IN_BDATA15,param,UA2500);
		pmu_range = GetValue(IN_BDATA15,param);
  	    j = j - 1;
	  }
	}
    printf("\nresult = %d[nA]\n\n",selvol);
    next_volt = start_volt + step_volt * (j+1);
  }/*for*/ 
  fprintf(fpt,"\n");
  /*-----------------------------------------------------*/
  /* Close Data File                                     */
  /*-----------------------------------------------------*/
  if(fclose(fpt)){
    exit(-10);
  }
  SetValue(change_reg,param,tempo1);
	 SetValue(IN_BDATA15,param, stack1);
	
  if(mode == V_VHH_E_LEAK ||mode ==  V_VHH_P_LEAK){	
  	 SetValue(IN_LDATA0,param,stack2);
  }
  return( PASS );
}

int VrsgMonitor_Repeat(interface_t* param, int Vresult[BGR_ARRAY_VCC])
{
  int data_num,vcc_num,selvol,waittime,monitor_mode;
  int i,j,result;
  int ffpt_bak,vcc_bak;
  int vcc_array[BGR_ARRAY_VCC],vs[NUM_POWERSUPPLY],vrsg_buff[BGR_ARRAY_REPEAT],Vanalyze;
  char moni_pin[20];
   
	data_num = BGR_ARRAY_REPEAT-1;
	vcc_num = BGR_ARRAY_VCC-1;


    vcc_array[0] = VCC_ARRAY0;
    vcc_array[1] = VCC_ARRAY1;
    vcc_array[2] = VCC_ARRAY2;
    vcc_array[3] = VCC_ARRAY3;
    vcc_array[4] = VCC_ARRAY4;
    //vcc_array[5] = 3600;

	vs[1] =	vs[2] = 500;

    waittime = 200;

    sprintf(moni_pin,"Vrsgmon");
    monitor_mode = VRSGMON_MON;

    for(i=0;i<=vcc_num;i++){
      //for(j=0;j<=data_num;j++){
        Vresult[i]=0;
      //}
    }
   
	// -------- Minori Setting ---------//
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0); // スクランブラ M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,0); // CLK Setting 1:逆相 0:同相
	SetValue(IN_BDATA07,param,0); // Loop Times
        SetValue(IN_BDATA09,param,0); // MINORI-Vmonitor Mode
	SetValue(IN_BDATA08,param,2); // MULTINOREAD_NS
	
	
	vs[0]=vcc_array[1];
	MinoriMonitor_Vrsg_First(MVRSGMON,&vs[0],waittime,param,"USER_ALL0_M0_MULTISCANI_XS","MINORI Vrsg Monitor",&selvol);
        //if(result != PASS) break;  //Pass or Fail判定
	//data_array[1][1] = selvol;

      for(j=0;j<=vcc_num;j++){
      
        vs[0] = vcc_array[j];
        for(i=0;i<=data_num;i++){

          if( (j==0) && (i==0) ){
            vcc_bak = vcc_array[j];
          }else if(i == 0){
            vcc_bak = vcc_array[j-1];
	  }else{
	    vcc_bak = vcc_array[j];
	  }

	  ffpt_bak = Ffpt;
	  Ffpt = 0;
	
	  MinoriMonitor_Vrsg_Repeat(MVRSGMON,&vs[0],waittime,param,"USER_ALL0_M0_MULTISCANI_XS","MINORI Vrsg Monitor",vcc_bak,&selvol);
	  Ffpt = ffpt_bak;
	  //if(result != PASS) break;  //Pass or Fail判定
	//printf("VrsgMonitor_Repeat : selvol = %dmV\n",selvol);
	  vrsg_buff[i]=selvol;
        }
        result = VrsgAnalyze(&vrsg_buff[0], &Vanalyze);
		//printf("VrsgMonitor_Repeat : Vanalyze = %dmV\n",Vanalyze);
	
	if(result == PASS){
	  Vresult[j] = Vanalyze; 
        }else{
	  Vresult[j] = 0; 
	}

      }
      
      DeviceSpecificPowerDown();
      DeviceLevelsPowerDown();

  return(PASS);
}

int VrsgMonitor_Single(int vcc_num, interface_t* param, int* Vresult)
{
  int data_num,selvol,waittime;
  int i,j,result;
  int ffpt_bak,vcc_bak;
  int vcc_array[BGR_ARRAY_VCC],vs[NUM_POWERSUPPLY],vrsg_buff[BGR_ARRAY_REPEAT],Vanalyze;
   
	data_num = BGR_ARRAY_REPEAT-1;

    vcc_array[0] = VCC_ARRAY0;
    vcc_array[1] = VCC_ARRAY1;
    vcc_array[2] = VCC_ARRAY2;
    vcc_array[3] = VCC_ARRAY3;
    vcc_array[4] = VCC_ARRAY4;
    //vcc_array[5] = 3600;

	vs[1] =	vs[2] = 500;

    waittime = 200;

	// -------- Minori Setting ---------//
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0); // スクランブラ M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,0); // CLK Setting 1:逆相 0:同相
	SetValue(IN_BDATA07,param,0); // Loop Times
        SetValue(IN_BDATA09,param,0); // MINORI-Vmonitor Mode
	SetValue(IN_BDATA08,param,2); // MULTINOREAD_NS
	
	
	vs[0]=vcc_array[vcc_num];
	MinoriMonitor_Vrsg_First(MVRSGMON,&vs[0],waittime,param,"USER_ALL0_M0_MULTISCANI_XS","MINORI Vrsg Monitor",&selvol);
        //if(result != PASS) break;  //Pass or Fail判定
	//data_array[1][1] = selvol;

        for(i=0;i<=data_num;i++){

	  ffpt_bak = Ffpt;
	  Ffpt = 0;
	
	  MinoriMonitor_Vrsg_Repeat(MVRSGMON,&vs[0],waittime,param,"USER_ALL0_M0_MULTISCANI_XS","MINORI Vrsg Monitor",vs[0],&selvol);
	  Ffpt = ffpt_bak;
	  //if(result != PASS) break;  //Pass or Fail判定
	//printf("VrsgMonitor_Repeat : selvol = %dmV\n",selvol);
	  vrsg_buff[i]=selvol;
        }
        result = VrsgAnalyze(&vrsg_buff[0], &Vanalyze);
		//printf("VrsgMonitor_Repeat : Vanalyze = %dmV\n",Vanalyze);
	
	if(result == PASS){
	  *Vresult = Vanalyze; 
        }else{
	  *Vresult = 0; 
	}

      
      printf("VrsgMoni Vcc:%dmV - Result %dmV\n",vs[0],Vanalyze);
      DeviceSpecificPowerDown();
      DeviceLevelsPowerDown();

  return(PASS);
}


int BgrTrim_RegWrite(int mode, int vcc_num, interface_t* param, int Target)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int vcc_array[BGR_ARRAY_VCC],vs[NUM_POWERSUPPLY],Vanalyze;
  char moni_pin[20];
	Vanalyze = 0;

	data_num = BGR_ARRAY_REPEAT-1;

    vcc_array[0] = VCC_ARRAY0;
    vcc_array[1] = VCC_ARRAY1;
    vcc_array[2] = VCC_ARRAY2;
    vcc_array[3] = VCC_ARRAY3;
    vcc_array[4] = VCC_ARRAY4;
    //vcc_array[5] = 3600;

	vs[1] =	vs[2] = 500;														    

    waittime = 10;

    sprintf(moni_pin,"Vrsgmon");
    monitor_mode = VRSGMON_MON;

    stack0 = SetValue(IN_TESTSEL,param,AUTOTRIM_BGR08);
    stack1 = SetValue(IN_BDATA00,param,0);
    stack2 = SetValue(IN_LDATA0,param,waittime);
    stack3 = SetValue(IN_BDATA01,param,0);

    //  ********** Main Trim *********  //
      vs[0] = vcc_array[vcc_num];
      
      //if(vs[0] != VCC_ARRAY2){
        
	//  ************** Auto Trim **************  //
	printf("BgrTrim Vcc:%dmV - Target %dmV\n",vs[0],Target);
        ffpt_bak = Ffpt;
	Ffpt = 0;
        DeviceSpecificPowerUp();
        result = CpuModeMonitor_Vrsg(VRSGMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvrsg_bgr_at"),"BGR_Trim",&Target);

	if(result !=PASS){
	  Ffpt = ffpt_bak;
	  return(TRIM_FAIL);
	}
	
	SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param));
	SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param));

	//  ************** Extra Write **************  //
	    vs[0] = 3300;
        SetValue(IN_TESTSEL,param,1);
        SetValue(IN_BDATA00,param,1);
        SetValue(IN_BDATA01,param,vcc_num);
	
        DeviceSpecificPowerUp();
        result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("settrimingdata_code_at"),"BGR_RegWrite");

	if(result !=PASS){
	  Ffpt = ffpt_bak;
	  return(REG_FAIL);
	}

        Ffpt = ffpt_bak;

      /*
      }else{
        return(P_FAIL);
      }
      */


  SetValue(IN_TESTSEL,param,stack0);
  SetValue(IN_BDATA00,param,stack1);
  SetValue(IN_BDATA00,param,0);
  SetValue(IN_LDATA0,param,stack2);
  

  return(PASS);
}
 
int Trim_VddVddh(int mode, interface_t* param)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result,target,delta;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int SetReg,vs[3];
  char moni_pin[20];
  int first_flag,repeat_mode,inif_flag;
	vs[0] = 3300;
	vs[1] =	vs[2] = 500;														    

    waittime = 1;
	

    stack1 = SetValue(IN_BDATA00,param,0);
	stack2 = GetValue(IN_ETLR01,param);
	stack3 = GetValue(IN_ETLR02,param);
	stack4 = GetValue(IN_ETLR03,param);

	//  ************** Auto Trim **************  //
    
	SetReg = 0;
	first_flag = 1;
    for(i=0;i<=14;i++){
	  Ffpt = 0;
      
	  if(i<8) SetReg =  7-i;
	  else    SetReg = 22-i;

	  switch(mode){
		case 1: // Vdd
		  SetValue(IN_TESTSEL,param,MONITOR_VDD);
		  SetValue(IN_ETLR01,param,(GetValue(IN_ETLR01,param) & 0x87 | ((SetReg & 0x0F) << 3) ));
		  target = 790;
		  delta = VDD_DELTA;
		break;
		case 2: // Vddh
		  SetValue(IN_TESTSEL,param,MONITOR_VDDH);
		  SetValue(IN_ETLR02,param,(GetValue(IN_ETLR02,param) & 0x7F | ((SetReg & 0x01) << 7) ));
		  SetValue(IN_ETLR03,param,(GetValue(IN_ETLR03,param) & 0xF8 | ((SetReg & 0x0E) >> 1) ));
		  target = 1250;
		  delta = VDDH_DELTA;
		break;
      }
	  
	  repeat_mode = RAMBOOT_CHANGE | INIF_SET;
	  
	  if(first_flag==1){// Initial mode
	    Ffpt = 1;
	    first_flag = 0;
        repeat_mode = repeat_mode | INITIAL_SET;
	  }
      
      DeviceSpecificPowerUp();
      //result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
      result = CpuModeMonitor_Repeat(repeat_mode,VCLMON_MON, &vs[0], param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);

	  if(result !=PASS){
	    Ffpt = 1;
	    break;
	  }

	  printf("  ETLR01:0x%X, ETLR02:0x%X, ETLR03:0x%X, selvol=%dmV\n",GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),selvol);
      
      if( ((target - delta) <= selvol) && (selvol <= (target + delta))){
		printf("Vdd/Vddh Trimming ***PASS***\n");break;
      }else if(((target - delta*3) > selvol)){
		i++;
      }else if(((target + delta) < selvol)){
		printf("Vdd/Vddh Trimming ---FAIL---\n");break;
		result = FAIL;
      }  
	}
    Ffpt=1;
	BL_DATACLEAR(param);

    if(result != PASS){
	  SetValue(IN_ETLR01,param,stack2);
	  SetValue(IN_ETLR02,param,stack3);
	  SetValue(IN_ETLR03,param,stack4);
	}

  DeviceLevelsPowerDown();
  return(result);
}

int Iref_Trim_ALL(int mode, interface_t* param)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result,trim_result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int SetReg,vs[3];
  char filename[100],filepath[200];
  int Iref_moni[16][16];
	int etlr12_74,etlr12_30;
  int init_flag;
  int delta,min_delta;
  vs[0] = 3300;
	vs[1] =	vs[2] = 500;														    
  init_flag = 1;
  waittime = 10;
	
  for(i = 0; i <= 15; i++){
    for(j = 0; j<= 15; j++){
      Iref_moni[i][j] = 0;
    }
  }


  stack1 = GetValue(IN_ETLR12,param);

  SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);
  SetValue(IN_BDATA15,param,UA25);
    SetValue(IN_ETLR09,param,0xA0);

  trim_result = FAIL;

  for(etlr12_74 = 0; etlr12_74 <= 15; etlr12_74++){
    for(etlr12_30 = 0; etlr12_30 <= 15; etlr12_30 = etlr12_30 +15){
      if(init_flag == 1){
        Ffpt = 1;
      }else{
        Ffpt = 0;
      }

      SetValue(IN_ETLR12,param,((etlr12_74 << 4) | etlr12_30));

      DeviceSpecificPowerUp();

      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
      printf("Iref Monitor = %dnA\n",selvol);
      Iref_moni[etlr12_74][etlr12_30] = selvol;

      if(result != PASS){
        break;
      }

    }
    if(result != PASS){
      break;
    }

    delta = absolute(Iref_moni[etlr12_74][0] - Iref_moni[etlr12_74][15]);
    if(init_flag == 1){
      init_flag = 0;
      min_delta = delta;
    }else{
      if(min_delta > delta ){ //常に最小値を更新する。
        min_delta = delta;
      }else{ // 最小値でない場合はトリミング終了(但しTapは+1している)
        min_delta = delta;
        trim_result = PASS;
        
      }
    }

    if(trim_result == PASS){
      break;
    }
  }

  Ffpt = 1;
  DeviceLevelsPowerDown();

  
  // ----- File Name & Path ----- //
  sprintf(filename, "%s_W%d_X%dY%d_IrefTrimDebug",lotname,atoi(wafername),xy_location[0],xy_location[1]);
  sprintf(filepath, "%s%s%s.csv", glob_cdp, glob_datalog, filename);
  // ----- File Open ----- //
  if((Fptdata=fopen(filepath,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",filepath);
    return(FAIL);
  }
  fprintf(Fptdata,"IrefDebug\n");
  fprintf(Fptdata,"Trimming Result\n");
  fprintf(Fptdata,"ETLR12[7:4],%d\n",etlr12_74);
  fprintf(Fptdata,"ETLR09[7:0],0x%X,%d,\n",GetValue(IN_ETLR09,param),GetValue(IN_ETLR09,param)&0x7F);
  fprintf(Fptdata,"Iref(temp)[nA],0\n");
  fprintf(Fptdata," ,ETLR12_30 = 0,ETLR12_30 = 15,\n");
  for(i=0;i<=15;i++){
    fprintf(Fptdata,"ETLR12_74 = %d ,%d,%d,\n",i,Iref_moni[i][0],Iref_moni[i][15]);
    
  }
  CloseDataOutFile();  
  

  if((result != PASS) || (trim_result != PASS)){
    printf("--- Trimming Fail!!! ---\n");
    SetValue(IN_ETLR12,param,stack1);
    return(FAIL);
  }




  etlr12_74 = etlr12_74 -1; //次のTapに移動しているので戻しておく。
  SetValue(IN_ETLR12,param,((stack1 & 0x0F) | (etlr12_74 << 4)));
  printf("*** Trimming PASS ***  ETLR12[7:4] = 0x%X\n",etlr12_74);
  return(result);
}

int Iref_Temp_moni_ALL(int mode, interface_t* param)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,k,result,trim_result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int SetReg,vs[3];
  char filename[100],filepath[200];
  int Iref_moni[128][16][16],Iref_pass[128][16][16];
	int etlr12_74,etlr12_30,etlr09_60;
  int init_flag;
  int delta,min_delta;
  vs[0] = 3300;
	vs[1] =	vs[2] = 500;														    
  init_flag = 1;
  waittime = 10;
	
  for(k = 0; k <= 127; k++){
    for(i = 0; i <= 15; i++){
      for(j = 0; j<= 15; j++){
        Iref_moni[k][i][j] = 0;
      }
    }
  }


  stack1 = GetValue(IN_ETLR12,param);
  stack2 = GetValue(IN_ETLR09,param);
  
  

  for(etlr09_60 = 0; etlr09_60 <= 127; etlr09_60++){
    for(etlr12_74 = 0; etlr12_74 <= 15; etlr12_74++){
      for(etlr12_30 = 0; etlr12_30 <= 15; etlr12_30++){
        
        SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);
        SetValue(IN_BDATA15,param,UA25);
        
        if(init_flag == 1){
          Ffpt = 1;
          init_flag = 0;
        }else{
          Ffpt = 0;
        }

        SetValue(IN_ETLR09,param, ( (GetValue(IN_ETLR09,param) & 0x80) | etlr09_60 ));
        SetValue(IN_ETLR12,param,((etlr12_74 << 4) | etlr12_30));

        DeviceSpecificPowerUp();

        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
        if( selvol>=24000 ){
          SetValue(IN_BDATA15,param,UA250);
          result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
        }

        printf("etlr09_60 %2X, etlr12_74 %X, etlr12_30 %X,Iref Monitor = %dnA\n",etlr09_60,etlr12_74,etlr12_30,selvol);

        Iref_moni[etlr09_60][etlr12_74][etlr12_30] = selvol;
        Iref_pass[etlr09_60][etlr12_74][etlr12_30] = 0; //面倒なので、0＝Passとする。

        if(result != PASS){
          Iref_pass[etlr09_60][etlr12_74][etlr12_30] = 1;//面倒なので、1＝FAILとする。
        }

      }
    }
  }
  Ffpt = 1;
  DeviceLevelsPowerDown();

  
  // ----- File Name & Path ----- //
  sprintf(filename, "%s_W%d_X%dY%d_IrefmoniALL",lotname,atoi(wafername),xy_location[0],xy_location[1]);
  sprintf(filepath, "%s%s%s.csv", glob_cdp, glob_datalog, filename);
  // ----- File Open ----- //
  if((Fptdata=fopen(filepath,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",filepath);
    return(FAIL);
  }
  fprintf(Fptdata,"IrefDebug\n");
  fprintf(Fptdata,"ETLR09_60,ETLR12_74,ETLR12_30,Iref,Pass/Fail\n");
  for(etlr09_60 = 0; etlr09_60 <= 127; etlr09_60++){
    for(etlr12_74 = 0; etlr12_74 <= 15; etlr12_74++){
      for(etlr12_30 = 0; etlr12_30 <= 15; etlr12_30++){
        fprintf(Fptdata,"%d,%d,%d,%d,%d,\n",etlr09_60,etlr12_74,etlr12_30,Iref_moni[etlr09_60][etlr12_74][etlr12_30],Iref_pass[etlr09_60][etlr12_74][etlr12_30]);
      }
    }
  }

  CloseDataOutFile();  
  
  SetValue(IN_ETLR12,param,stack1);
  SetValue(IN_ETLR09,param,stack2);

  return(result);
}

int Iref_Trim_forRext(int mode, interface_t* param)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result,trim_result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int SetReg,vs[3];
  char filename[100],filepath[200];
  int Iref_moni[16][16],Iref_result;
	int etlr12_74,etlr12_30;
  int Target;
  int init_flag;
  int delta,min_delta;
  vs[0] = 3300;
	vs[1] =	vs[2] = 500;
  init_flag = 1;
  waittime = 10;
	
  Target = 7500; //7.5uA = 2.5uA設定 * 3倍 * 1uA

  for(i = 0; i <= 15; i++){
    for(j = 0; j<= 15; j++){
      Iref_moni[i][j] = 0;
    }
  }


  stack1 = GetValue(IN_ETLR12,param);

  SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);
  SetValue(IN_BDATA15,param,UA25);
  SetValue(IN_ETLR09,param,0xA0);
  
  trim_result = FAIL;

  for(etlr12_74 = 0; etlr12_74 <= 15; etlr12_74++){
    etlr12_30 = 15; //温特TapはMax固定
      if(init_flag == 1){
        init_flag = 0;
        Ffpt = 1;
      }else{
        Ffpt = 0;
      }

      SetValue(IN_ETLR12,param,((etlr12_74 << 4) | etlr12_30));

      DeviceSpecificPowerUp();

      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
      printf("Iref Monitor = %dnA\n",selvol);
      Iref_moni[etlr12_74][etlr12_30] = selvol;

    if(result != PASS){
      break;
    }
  }
    printf("Debug\n");
  // ------ Iref judge ------ //
  for(etlr12_74 = 15; etlr12_74 >= 0; etlr12_74-- ){
    printf("Result = %d nA\n",Iref_moni[etlr12_74][etlr12_30]);
    if(Iref_moni[etlr12_74][etlr12_30] >= Target){
    printf("PASS\n");
      trim_result = PASS;
      SetValue(IN_ETLR12,param,((stack1 & 0x0F) | (etlr12_74 << 4)));
      break;
    }


    if(etlr12_74 == 0){
    printf("FAIL\n");
      trim_result = FAIL;
      break;
    }
  }

  if(trim_result == PASS){
    // Iref Trimming
    SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_TEMP);
    SetValue(IN_BDATA15,param,UA25);
    Target = Target -150;
    DeviceSpecificPowerUp();
    CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_TEMP",&Target);
    // Iref monitor
    DeviceSpecificPowerUp();
    result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
    Iref_result = selvol;
  }
  

  Ffpt = 1;
  DeviceLevelsPowerDown();

  
  // ----- File Name & Path ----- //
  sprintf(filename, "%s_W%d_X%dY%d_IrefTrimDebug_forRext",lotname,atoi(wafername),xy_location[0],xy_location[1]);
  sprintf(filepath, "%s%s%s.csv", glob_cdp, glob_datalog, filename);
  // ----- File Open ----- //
  if((Fptdata=fopen(filepath,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",filepath);
    return(FAIL);
  }
  fprintf(Fptdata,"IrefDebug\n");
  fprintf(Fptdata,"Trimming Result\n");
  fprintf(Fptdata,"ETLR12[7:4],%d\n",etlr12_74);
  fprintf(Fptdata,"ETLR09[7:0],0x%X,%d,\n",GetValue(IN_ETLR09,param),GetValue(IN_ETLR09,param)&0x7F);
  fprintf(Fptdata,"Iref(temp)[nA],%d\n",Iref_result);
  fprintf(Fptdata,"monitor_Data,ETLR12_30 = 15,\n");
  for(i=0;i<=15;i++){
    fprintf(Fptdata,"ETLR12_74 = %d ,%d,\n",i,Iref_moni[i][etlr12_30]);
  }
  CloseDataOutFile();  
  

  if((result != PASS) || (trim_result != PASS)){
    printf("--- Trimming Fail!!! ---\n");
    SetValue(IN_ETLR12,param,stack1);
    return(FAIL);
  }

  SetValue(IN_ETLR12,param,((stack1 & 0x0F) | (etlr12_74 << 4)));
  printf("*** Trimming PASS ***  ETLR12[7:4] = 0x%X  ETLR09 = %X\n",etlr12_74,GetValue(IN_ETLR09,param));
  return(PASS);
}

int Shmoo_LPRead(int mode, interface_t* param)
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result,trim_result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7;
  char filename[100],filepath[200],modename[20],wgl_name[50];
  int loop_mode,loop,loop_max,loop_min,loop_delta,loop_change;
  int vs[3];
  vs[0] = 3300;
	vs[1] =	vs[2] = 500;
  waittime = 10;
	
  switch (mode)
  {
  case 0: //CHKA-DS-WS2
      SetValue(IN_LDATA0,param,0x3F7FE);
      SetValue(IN_LDATA1,param,0x405E3);
      SetValue(IN_LDATA2,param,0x40E50);
      SetValue(IN_LDATA3,param,0x41000);
      SetValue(IN_BDATA10,param,0);
      SetValue(IN_BDATA11,param,1);
      sprintf(wgl_name,"FLP0_CHKA_DSCAN1INCX_DS_LPREAD_WS2_BASE");
    break;
  case 1: //CHKA-XS-WS2
      SetValue(IN_LDATA0,param,0x3F7E7);
      SetValue(IN_LDATA1,param,0x405E3);
      SetValue(IN_LDATA2,param,0x60650);
      SetValue(IN_LDATA3,param,0x61000);
      SetValue(IN_BDATA10,param,0);
      SetValue(IN_BDATA11,param,1);
      sprintf(wgl_name,"FLP0_CHKA_MULTISCANI_XS_LPREAD_WS2_BASE");
    break;
  
  default:
    printf("\n  --- Mode select Error!!! ---\n");
    printf(  "  Mode = %d\n\n",mode);
    return(FAIL);
    break;
  }


	 Ffpt = 0;
    for(loop_mode = 1; loop_mode <= 4; loop_mode++){
      switch (loop_mode)
      {
        case 1:// Vdd Shmoo
          vs[0] = 3300;
          vs[1] = 580;
          vs[2] = 500;
          loop_max = 1200;
          loop_min = 600;
          loop_delta = 50;
          loop_change = 158;// 790mV = 500 * 1.58
          sprintf(modename,"VddShmoo_vddh_max");
        break;
        case 2:// Vddh Shmoo
          vs[0] = 3300;
          vs[1] = 500;
          vs[2] = 728;
          loop_max = 1500;
          loop_min = 1000;
          loop_delta = 50;
          loop_change = 250;// 1250mV = 500 * 2.5
          sprintf(modename,"VddhShmoo_vdd_max");
        break;
        case 3:// Vdd Shmoo
          vs[0] = 3300;
          vs[1] = 452;
          vs[2] = 500;
          loop_max = 1200;
          loop_min = 600;
          loop_delta = 50;
          loop_change = 158;// 790mV = 500 * 1.58
          sprintf(modename,"VddShmoo_vddh_min");
        break;
        case 4:// Vddh Shmoo
          vs[0] = 3300;
          vs[1] = 500;
          vs[2] = 456;
          loop_max = 1500;
          loop_min = 1000;
          loop_delta = 50;
          loop_change = 250;// 1250mV = 500 * 2.5
          sprintf(modename,"VddhShmoo_vdd_min");
        break;
      }
      sprintf(filename,"%s_W%d_X%dY%d_%s",lotname,atoi(wafername),xy_location[0],xy_location[1],modename);
      sprintf(filepath, "%s%s%s.csv", glob_cdp, glob_datalog, filename);
      if((Fptdata=fopen(filepath,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",filepath);
        return(FAIL);
      }
      fprintf(Fptdata,"Initial\n patten,%s\n vcc,%d,vddh,%d,vdd,%d\n",wgl_name,vs[0],vs[1],vs[2]);


      for(loop = loop_max + loop_delta; loop >= loop_min; loop = loop - loop_delta){
	    CloseDataOutFile();delay_timer(200);
        Fptdata=fopen(filepath,"at");
		      fprintf(Fptdata,"\n");
            switch(loop_mode){
              case 1:
              case 3:
            	  vs[2] = loop * 100 /loop_change;
      				break;
              case 2:
              case 4:
              	vs[1] = loop * 100 /loop_change;
			      	break;
            }
          if(loop != (loop_max + loop_delta)){
				    fprintf(Fptdata,"%d",loop); 
          }else{
				    fprintf(Fptdata,"%s,",modename);
          }
        for(tempo1=1;tempo1<=15;tempo1++){
          switch(tempo1){
              case  0 :SetValue(IN_EXTAL1,param,60*1000);break;
              case  1 :SetValue(IN_EXTAL1,param,50*1000);break;
              case  2 :SetValue(IN_EXTAL1,param,40*1000);break;
              case  3 :SetValue(IN_EXTAL1,param,35*1000);break;
              case  4 :SetValue(IN_EXTAL1,param,33*1000);break;
              case  5 :SetValue(IN_EXTAL1,param,30*1000);break;
              case  6 :SetValue(IN_EXTAL1,param,25*1000);break;
              case  7 :SetValue(IN_EXTAL1,param,23*1000);break;
              case  8 :SetValue(IN_EXTAL1,param,20*1000);break;
              case  9 :SetValue(IN_EXTAL1,param,17*1000);break;
              case 10 :SetValue(IN_EXTAL1,param,14*1000);break;
              case 11 :SetValue(IN_EXTAL1,param,12*1000);break;
              case 12 :SetValue(IN_EXTAL1,param,10*1000);break;
              case 13 :SetValue(IN_EXTAL1,param, 9*1000);break;
              case 14 :SetValue(IN_EXTAL1,param, 8*1000);break;
              case 15 :SetValue(IN_EXTAL1,param, 7*1000);break;
	        }

          if(loop == (loop_max + loop_delta)){
            fprintf(Fptdata,"%d,",GetValue(IN_EXTAL1,param)/1000);
          }else{
            printf("loop = %5d ,Vdd =%4dmV, Vddh =%4dmV T = %4dus : ",loop,vs[2],vs[1],GetValue(IN_EXTAL1,param)/1000);

            DeviceSpecificPowerUp();
            result = Minori_LPRead_loop(MNORMAL,&vs[0],waittime,param,wgl_name,"MINORI READ");
            SetValue(IN_BDATA11,param,0);
            if(result == PASS){
              printf("***PASS***\n");
              fprintf(Fptdata,",P");
	          }else if(result == FAIL){
	            printf("---FAIL---\n");
	            fprintf(Fptdata,",F");
	          }else{
	            printf("-- ERROR!!! --\n");
	            fprintf(Fptdata,",E");
            }//if end
          }//if end
        }//Freq end
      }//loop end
	  fprintf(Fptdata,"\n\n");

	CloseDataOutFile();delay_timer(200);

    }//loop_mode end
	  Ffpt = 1;
    DeviceSpecificPowerDown();
    DeviceLevelsPowerDown();
    return(PASS);
}

int Shmoo_LPRead_vs1MOSC(int mode, interface_t* param,int vs[NUM_POWERSUPPLY])
{
  int data_num,selvol,waittime,monitor_mode;
  int i,j,result,trim_result;
  int ffpt_bak,vcc_bak;
  int stack0,stack1,stack2,stack3,stack4;
  int tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7;
  char filename[100],filepath[200],modename[20],wgl_name[50];
  int loop_mode,loop,loop_max,loop_min,loop_delta,loop_change;
  int reg_set;
  waittime = 10;
	
  switch (mode)
  {
  case 0: //CHKA-DS-WS2
      SetValue(IN_LDATA0,param,0x3F7FE);
      SetValue(IN_LDATA1,param,0x405E3);
      SetValue(IN_LDATA2,param,0x40E50);
      SetValue(IN_LDATA3,param,0x41000);
      SetValue(IN_BDATA10,param,0);
      SetValue(IN_BDATA11,param,1);
      sprintf(wgl_name,"FLP0_CHKA_DSCAN1INCX_DS_LPREAD_WS2_BASE");
    break;
  case 1: //CHKA-XS-WS2
      SetValue(IN_LDATA0,param,0x3F7E7);
      SetValue(IN_LDATA1,param,0x405E3);
      SetValue(IN_LDATA2,param,0x60650);
      SetValue(IN_LDATA3,param,0x61000);
      SetValue(IN_BDATA10,param,0);
      SetValue(IN_BDATA11,param,1);
      sprintf(wgl_name,"FLP0_CHKA_MULTISCANI_XS_LPREAD_WS2_BASE");
    break;
  
  default:
    printf("\n  --- Mode select Error!!! ---\n");
    printf(  "  Mode = %d\n\n",mode);
    return(FAIL);
    break;
  }


	 Ffpt = 0;
    for(loop_mode = 1; loop_mode <= 1; loop_mode++){
      
      SetValue(IN_BDATA11,param,1); // Loop 初期化
      
      switch (loop_mode)
      {
        case 1:// 1MOSC Shmoo
          loop_max = 0xF;
          loop_min = 0x3;
          loop_delta = 1;
          sprintf(modename,"1MOSC_Shmoo");
        break;
      }
      sprintf(filename,"%s_W%d_X%dY%d_%s",lotname,atoi(wafername),xy_location[0],xy_location[1],modename);
      sprintf(filepath, "%s%s%s.csv", glob_cdp, glob_datalog, filename);
      if((Fptdata=fopen(filepath,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",filepath);
        return(FAIL);
      }
      fprintf(Fptdata,"Initial\n patten,%s\n vcc,%d,vddh,%d,vdd,%d\n",wgl_name,vs[0],vs[1],vs[2]);


      for(loop = loop_max + loop_delta; loop >= loop_min; loop = loop - loop_delta){
	    CloseDataOutFile();delay_timer(200);
        Fptdata=fopen(filepath,"at");
        SetValue(IN_BDATA11,param,1); // Loop 初期化
	  //WaitHitKey("\nDebug\n");
		      fprintf(Fptdata,"\n");
            switch(loop_mode){
              case 1:
                if(loop >= 0x8){
                  reg_set = loop - 0x8;
                }else{
                  reg_set = loop + 0x8;
                }
            	break;
            }
          if(loop != (loop_max + loop_delta)){
				    fprintf(Fptdata,"0x%2X",reg_set); 
            SetValue(IN_BDATA11,param,1); // Loop 初期化
            SetValue(IN_ETLR11,param,(GetValue(IN_ETLR11,param) & 0xF0 ) | reg_set);
            // ----- register Set ----- //
            DeviceSpecificPowerUp();
            SetValue(IN_TESTSEL,param,0);
            SetValue(IN_BDATA00,param,1);
            SetValue(IN_BDATA01,param,0);
            SetValue(IN_EXTAL1,param,31);
            CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER"); /*ファーム次第で変更する可能性あり*/
            SetValue(IN_TESTSEL,param,0);
            SetValue(IN_BDATA00,param,0);
            SetValue(IN_BDATA01,param,0);
            // ----- register Set-End ----- //
          }else{
				    fprintf(Fptdata,"%s,",modename);
          }
        for(tempo1=2;tempo1<=20;tempo1++){
          switch(tempo1){
              case  0 :SetValue(IN_EXTAL1,param,60*1000);break;
              case  1 :SetValue(IN_EXTAL1,param,50*1000);break;
              case  2 :SetValue(IN_EXTAL1,param,40*1000);break;
              case  3 :SetValue(IN_EXTAL1,param,35*1000);break;
              case  4 :SetValue(IN_EXTAL1,param,33*1000);break;
              case  5 :SetValue(IN_EXTAL1,param,30*1000);break;
              case  6 :SetValue(IN_EXTAL1,param,25*1000);break;
              case  7 :SetValue(IN_EXTAL1,param,23*1000);break;
              case  8 :SetValue(IN_EXTAL1,param,20*1000);break;
              case  9 :SetValue(IN_EXTAL1,param,17*1000);break;
              case 10 :SetValue(IN_EXTAL1,param,14*1000);break;
              case 11 :SetValue(IN_EXTAL1,param,12*1000);break;
              case 12 :SetValue(IN_EXTAL1,param,10*1000);break;
              case 13 :SetValue(IN_EXTAL1,param, 8*1000);break;
              case 14 :SetValue(IN_EXTAL1,param, 6*1000);break;
              case 15 :SetValue(IN_EXTAL1,param, 5*1000);break;
              case 16 :SetValue(IN_EXTAL1,param, 4*1000);break;
              case 17 :SetValue(IN_EXTAL1,param, 3*1000);break;
              case 18 :SetValue(IN_EXTAL1,param, 2*1000);break;
              case 19 :SetValue(IN_EXTAL1,param, 1*1000);break;
              case 20 :SetValue(IN_EXTAL1,param, 800);break;
	        }

          if(loop == (loop_max + loop_delta)){
            fprintf(Fptdata,"%d,",GetValue(IN_EXTAL1,param)/1000);
          }else{
            printf("loop = 0x%2X ,ETLR11 = 0x%2X, Vdd =%4dmV, Vddh =%4dmV T = %4dus : ",loop,GetValue(IN_ETLR11,param),vs[2],vs[1],GetValue(IN_EXTAL1,param)/1000);

            DeviceSpecificPowerUp();
            result = Minori_LPRead_loop(MNORMAL,&vs[0],waittime,param,wgl_name,"MINORI READ");
            SetValue(IN_BDATA11,param,0);
            if(result == PASS){
              printf("***PASS***\n");
              fprintf(Fptdata,",P");
	          }else if(result == FAIL){
	            printf("---FAIL---\n");
	            fprintf(Fptdata,",F");
	          }else{
	            printf("-- ERROR!!! --\n");
	            fprintf(Fptdata,",E");
            }//if end
          }//if end
        }//Freq end
      }//loop end
	  fprintf(Fptdata,"\n\n");

	CloseDataOutFile();delay_timer(200);

    }//loop_mode end
	  Ffpt = 1;
    DeviceSpecificPowerDown();
    DeviceLevelsPowerDown();
    return(PASS);
}


int Extra_Erase_Vth(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int etcr8;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  int info[32];                    /* wmat chip information              */
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = 0;

  stack0 = GetValue(IN_AREA,param);
  stack1 = GetValue(IN_TOPADDR0,param);
  stack2 = GetValue(IN_TESTSIZE0,param);
  stack3 = GetValue(IN_TOPADDR1,param);
  stack4 = GetValue(IN_TESTSIZE1,param);
  stack5 = GetValue(IN_TOPADDR2,param);
  stack6 = GetValue(IN_TESTSIZE2,param);
  stack7 = GetValue(IN_EXTAL1,param);
  stack8 = GetValue(IN_REG,param);

/**************************************************************************************/
/***   File Name Setting                                                            ***/
/**************************************************************************************/
      sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
      sprintf(test_name,"Extra3_VthRead");
      sprintf(item_name,"%s_%s",test_name,sample_name);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

      time( &time_list );// time_list += V4V5K_TIME_OFS;

/**************************************************************************************/
/***   Disturb & Distribution                                                       ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    SetValue(IN_AREA,param,3);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0x400);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(file_log,"<<Write-Inhibit>> ETLR25=0x%X,Vcc=%d,Distribution:0V~0.05VStep,Mat=%d(0x%X~0x%X)",GetValue(IN_ETLR25,param),vs[0],mat,GetValue(IN_TOPADDR0 ,param),GetValue(IN_TESTSIZE0 ,param)-1);
    fprintf(file_log,"\n"); 
    fclose( file_log );

    for ( i=1 ; i<=3 ; i++ ) {

      // --- Vth Initialize --- //
      SetValue(IN_ETCR08,param,0x00);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program");
      SetValue(IN_ETCR08,param,0x00);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");


    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

      loop_num = i-1;
      fprintf(file_log,"Erase_x%d",i);

      fclose( file_log );

        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_EXTAL1,param,31);    // 32MHz 
        SetValue(IN_BDATA00,param,1);    // PreWrite Non 
        for ( j=0 ; j<loop_num ; j++ ) {
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
        }
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0); 

      SetValue(IN_BDATA01,param,0);  // ALL Count
      SetValue(IN_BDATA02,param,99); // Log Headder
      SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
      SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
      SetValue(IN_EXTAL1,param,VTH_FREQ); 
      min_vol = -2000;
      tempo2 = 50;                                 /* VS-Step Default   */
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
      vth_mode = 1;                                /* VthRead1&2  */
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf); return(PASS);
      }
    } /* for i */
  SetValue(IN_ETCR08,param,etcr8);

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("Test-Time:%d[s]\n",time_list2-time_list); 
  printf("\n"); 

  /*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_TESTSEL,param,0);
  SetValue(IN_WRITESIZE,param,0);
  /*********************/

}

int Erase_VthvsIref(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int etcr8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = 0;

  stack0 = GetValue(IN_AREA,param);
  stack1 = GetValue(IN_TOPADDR0,param);
  stack2 = GetValue(IN_TESTSIZE0,param);
  stack3 = GetValue(IN_TOPADDR1,param);
  stack4 = GetValue(IN_TESTSIZE1,param);
  stack5 = GetValue(IN_TOPADDR2,param);
  stack6 = GetValue(IN_TESTSIZE2,param);
  stack7 = GetValue(IN_EXTAL1,param);
  stack8 = GetValue(IN_REG,param);

/**************************************************************************************/
/***   File Name Setting                                                            ***/
/**************************************************************************************/
      printf("Input Tempurature -40C(-40) / 25C(25) / 95C(95) ->");DigitInput(&tempo2);
      sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
      sprintf(test_name,"Iref_VthRead_%dC",tempo2);
      sprintf(item_name,"%s_%s",test_name,sample_name);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

      time( &time_list );// time_list += V4V5K_TIME_OFS;

/**************************************************************************************/
/***   Disturb & Distribution                                                       ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0x20000);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(file_log,"<<Iref vs Vthread>> ETLR25=0x%X,Vcc=%d,Distribution:0V~0.05VStep,Mat=%d(0x%X~0x%X)",GetValue(IN_ETLR25,param),vs[0],mat,GetValue(IN_TOPADDR0 ,param),GetValue(IN_TESTSIZE0 ,param)-1);
    fprintf(file_log,"\n"); 
    fclose( file_log );
	  SetValue(IN_ETLR09,param,( GetValue(IN_ETLR09,param) & 0x7F ) | 0x80); //温特Off
    for ( i=1 ; i<=5 ; i++ ) {
    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
      switch (i){
      case 1:
        fprintf(file_log,"Iref_5.0uA");
        SetValue(IN_ETLR25,param,( GetValue(IN_ETLR25,param) & 0xF0 ) | 0x00);
      break;
      case 2:
        fprintf(file_log,"Iref_3.5uA");
        SetValue(IN_ETLR25,param,( GetValue(IN_ETLR25,param) & 0xF0 ) | 0x0F);
      break;
      case 3:
        fprintf(file_log,"Iref_2.5uA");
        SetValue(IN_ETLR25,param,( GetValue(IN_ETLR25,param) & 0xF0 ) | 0x0D);
      break;
      case 4:
        fprintf(file_log,"Iref_2.0uA");
        SetValue(IN_ETLR25,param,( GetValue(IN_ETLR25,param) & 0xF0 ) | 0x0A);
      break;
      case 5:
        fprintf(file_log,"Iref_1.5uA");
        SetValue(IN_ETLR25,param,( GetValue(IN_ETLR25,param) & 0xF0 ) | 0x0B);
      break;
      }

      SetValue(IN_EXTAL1,param,31); 
      SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
      if( selvol>=24000 ){
        SetValue(IN_BDATA15,param,UA250);
        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
      }
        fprintf(file_log,",%d",selvol/3);
      
      SetValue(IN_BDATA15,param,0);
      fclose( file_log );

        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0); 

      SetValue(IN_BDATA01,param,0);  // ALL Count
      SetValue(IN_BDATA02,param,99); // Log Headder
      SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
      SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
      SetValue(IN_EXTAL1,param,VTH_FREQ); 
      min_vol = -2000;
      tempo2 = 50;                                 /* VS-Step Default   */
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
      vth_mode = 1;                                /* VthRead1&2  */
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf); return(PASS);
      }
    } /* for i */

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("Test-Time:%d[s]\n",time_list2-time_list); 
  printf("\n"); 

  /*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_TESTSEL,param,0);
  SetValue(IN_WRITESIZE,param,0);
  /*********************/

}

int Extra_VthRead(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param,int mode){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int etcr8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = 0;

  stack0 = GetValue(IN_AREA,param);
  stack1 = GetValue(IN_TOPADDR0,param);
  stack2 = GetValue(IN_TESTSIZE0,param);
  stack3 = GetValue(IN_TOPADDR1,param);
  stack4 = GetValue(IN_TESTSIZE1,param);
  stack5 = GetValue(IN_TOPADDR2,param);
  stack6 = GetValue(IN_TESTSIZE2,param);
  stack7 = GetValue(IN_EXTAL1,param);
  stack8 = GetValue(IN_REG,param);

/**************************************************************************************/
/***   File Name Setting                                                            ***/
/**************************************************************************************/
      printf("Input Retention Time [hr] (ex.100hr [100]) ->");DigitInput(&tempo2);
      sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
      sprintf(test_name,"%dhr",tempo2);
      sprintf(item_name,"%s_VthRead",sample_name);

      sprintf(file_name,"%s%s%s%s_%d.csv", glob_cdp, glob_bunpu, glob_Extralog, item_name, get_site_number());

      time( &time_list );// time_list += V4V5K_TIME_OFS;

/**************************************************************************************/
/***   Disturb & Distribution                                                       ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    SetValue(IN_AREA,param,3);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0x400);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    //File Hedder & File search
    if( NULL==(file_log = fopen( file_name, "rt" )) ){
      if(mode != 0){
        printf(" File Open Error!!! [%s] ",file_name);
        return(FAIL);
      }
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        fprintf(file_log,"<<Extra3 Vthread>> ETLR25=0x%X,Vcc=%d,Distribution:0V~0.05VStep,Mat=%d(0x%X~0x%X)",GetValue(IN_ETLR25,param),vs[0],mat,GetValue(IN_TOPADDR0 ,param),GetValue(IN_TESTSIZE0 ,param)-1);
        fprintf(file_log,"\n"); 
        fclose( file_log );
      }
    }

    // Retention Time Out
    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(file_log,"%s",test_name);
    fclose( file_log );

    //Mode:0 => Program-Erase_x2
    if(mode == 0){
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_EXTAL1,param,31);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      for(i=0;i<2;i++){
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        SetValue(IN_BDATA00,param,1);//PewWrite Skip
      }
    }


      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0); 
      SetValue(IN_BDATA01,param,0);  // ALL Count
      SetValue(IN_BDATA02,param,99); // Log Headder
      SetValue(IN_BDATA04,param,3);  // Log Output -> Bunpu/Extra
      SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
      SetValue(IN_EXTAL1,param,VTH_FREQ); 
      min_vol = -2000;
      tempo2 = 50;                                 /* VS-Step Default   */
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
      vth_mode = 1;                                /* VthRead1&2  */
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf); return(PASS);
      }

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("Test-Time:%d[s]\n",time_list2-time_list); 

  /*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_TESTSEL,param,0);
  SetValue(IN_WRITESIZE,param,0);
  /*********************/
  return(PASS);

}  

int VppTf_vs_Tap(int vs[NUM_POWERSUPPLY],interface_t* param,int mode,int Temprature){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int selvol,etlr15_20,trtf,etlr20_30,vpp;
  int result;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char trtf_name[30];
  FILE_T *file_log;

  cond1 = cond2 = cond3 = 0;

    SetValue(IN_REG,param,0x03);
    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0x100);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);



/**************************************************************************************/
/***   File Name Setting                                                            ***/
/**************************************************************************************/
      sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
      sprintf(item_name,"%s_TrTfMonitor_%dC",sample_name,Temprature);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

      time( &time_list );// time_list += V4V5K_TIME_OFS;


    //Monitor Log Out
    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(file_log,"Vpp-Tf vs Tap Result\n");
    fprintf(file_log,"Vcc = %d , Vddh = %d , Vdd = %d ,\n",vs[0],vs[1],vs[2]);
    fprintf(file_log,"Vpp,");
    fclose( file_log );


	Ffpt = 0;
    for(trtf = 0; trtf<=8; trtf++){
      for(vpp = 1; vpp<=5;vpp++){
	    printf("******* TrTf = %d , Vpp = %d ********\n",trtf,vpp);
        switch (vpp){
          case 1 : etlr20_30 = 0;break;
          case 2 : etlr20_30 = 4;break;
          case 3 : etlr20_30 = 9;break;
          case 4 : etlr20_30 = 12;break;
          case 5 : etlr20_30 = 15;break;
        }
        SetValue(IN_ETLR20,param, (GetValue(IN_ETLR20,param) & 0xF0 ) | etlr20_30 );
      
      if(trtf == 0 ){
        SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
        SetValue(IN_BDATA00,param,0);
        CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*5,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
        if( NULL==(file_log = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        }
        
        fprintf(file_log,"%d,,,",selvol);
        fclose( file_log );

      }else{


        switch (trtf){
          case 1 : etlr15_20 = 4; sprintf(trtf_name," 25,");break;
          case 2 : etlr15_20 = 5; sprintf(trtf_name," 50,");break;
          case 3 : etlr15_20 = 6; sprintf(trtf_name," 75,");break;
          case 4 : etlr15_20 = 7; sprintf(trtf_name,"100,");break;
          case 5 : etlr15_20 = 0; sprintf(trtf_name,"120,");break;
          case 6 : etlr15_20 = 1; sprintf(trtf_name,"150,");break;
          case 7 : etlr15_20 = 2; sprintf(trtf_name,"200,");break;
          case 8 : etlr15_20 = 3; sprintf(trtf_name,"300,");break;
        }
        SetValue(IN_ETLR15,param, (GetValue(IN_ETLR15,param) & 0xF8 ) | etlr15_20 );
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_ETCR11,param,0);

        CpuModeMonitor_VppTf(MNORMAL,&vs[0],param,W1SEC*3,CreatePatFileName("VppTf_Check"),"VppTf Monitor",&selvol);
		selvol = selvol * GetValue(IN_EXTAL1,param);
        if( NULL==(file_log = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        }
		if(vpp==1)fprintf(file_log,"%s",trtf_name);
        fprintf(file_log,"%d.%d,",selvol/1000,(selvol/100)%10);
        if((GetValue(OUT_ETCR16,param) & 0x40) == 0x40){
          fprintf(file_log,"FAIL,");
        }else{
          fprintf(file_log,"PASS,");
        }
		fclose( file_log );

        SetValue(IN_ETCR11,param,1);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("programall_code_at"),"Program ALL");
        if( NULL==(file_log = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        }

        if((GetValue(OUT_ETCR16,param) & 0x40) == 0x40){
          fprintf(file_log,"FAIL,");
        }else{
          fprintf(file_log,"PASS,");
        }
		fclose( file_log );
      }

      }
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }
      fprintf(file_log,"\n");
      fclose( file_log );
    }
  Ffpt =1;
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("\nTest-Time:%d[s]\n\n",time_list2-time_list); 

  return(PASS);

}

int MonitorALL_Tap(int vs[NUM_POWERSUPPLY],interface_t* param,int mode,int Temprature){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int selvol[260],etlr15_20,trtf,etlr20_30,vpp;
  int result;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[30];             /*                                    */
  char test_item[128];             /*                                    */
  char trtf_name[30];
  FILE_T *file_log;

  cond1 = cond2 = cond3 = 0;
/*
#define  MONITOR_VHH_W    1
#define  MONITOR_VHH_E    2
#define  MONITOR_VNOEMI   3
#define  MONITOR_VREG20   4
#define  MONITOR_VWI      5
#define  MONITOR_V33R     6
#define  MONITOR_VRSG     7
#define  MONITOR_VPP_W    8
#define  MONITOR_VPP_E    9
#define  MONITOR_VPP_PW   10
#define  MONITOR_VDEMG    11
#define  MONITOR_VREF10   12
#define  MONITOR_IREF     13
#define  MONITOR_IREF_T   14
*/


  switch(mode){
    case V_VHH_P:
      sprintf(test_name,"Vhh_P");
      SetValue(IN_TESTSEL,param,1);      //Vhh_Pのモニター
      SetValue(IN_BDATA00,param,17);      //ETLR17をインクリメントする
      SetValue(IN_BDATA01,param,0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x3F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VHH_E:
      sprintf(test_name,"Vhh_E");
      SetValue(IN_TESTSEL,param,2);      //Vhh_Eのモニター
      SetValue(IN_BDATA00,param,16);      //ETLR16をインクリメントする
      SetValue(IN_BDATA01,param,0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x3F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VNOEMI_PE:
      sprintf(test_name,"Vnoemi");
      SetValue(IN_TESTSEL,param,3);      //Vnoemiのモニター
      SetValue(IN_BDATA00,param,19);      //ETLR19をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xF0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VREG20:
      sprintf(test_name,"Vreg20");
      SetValue(IN_TESTSEL,param,4);      //Vreg20のモニター
      SetValue(IN_BDATA00,param,23);      //ETLR23をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xFC);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x03);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VWI:
      sprintf(test_name,"Vwi");
      SetValue(IN_TESTSEL,param,5);      //Vwiのモニター
      SetValue(IN_BDATA00,param,19);      //ETLR19をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0x0F);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,0x10);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_V33R:
      sprintf(test_name,"V33r");
      SetValue(IN_TESTSEL,param,6);      //V33rのモニター
      SetValue(IN_BDATA00,param,24);      //ETLR24をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xF8);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x07);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VRSG:
      sprintf(test_name,"Vrsg");
      SetValue(IN_TESTSEL,param,7);      //Vrsgのモニター
      SetValue(IN_BDATA00,param,18);      //ETLR18をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xF8);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x07);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VPP_P:
      sprintf(test_name,"Vpp_P");
      SetValue(IN_TESTSEL,param,8);      //Vpp_Pのモニター
      SetValue(IN_BDATA00,param,20);      //ETLR20をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xF0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
    break;
    case V_VPP_E:
      sprintf(test_name,"Vpp_E");
      SetValue(IN_TESTSEL,param,9);      //Vpp_Eのモニター
      SetValue(IN_BDATA00,param,20);      //ETLR20をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0x0F);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,0x10);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
    break;
    case V_VPP_PW:
      sprintf(test_name,"Vpp_PW");
      SetValue(IN_TESTSEL,param,10);      //Vpp_PWのモニター
      SetValue(IN_BDATA00,param,21);      //ETLR21をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0x0F);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,0x10);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
    break;
    case V_VDEMG:
      stack1 = SetValue(IN_ETLR20,param,0xFF);//VdemgはVppから生成のため最大Tapにしておく。
      sprintf(test_name,"Vdemg");
      SetValue(IN_TESTSEL,param,11);      //Vdemgのモニター
      SetValue(IN_BDATA00,param,21);      //ETLR21をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0xF0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
    break;
    case V_VREF10:
      sprintf(test_name,"Vref10");
      SetValue(IN_TESTSEL,param,12);      //Vref10のモニター
      SetValue(IN_BDATA00,param,11);      //ETLR11をインクリメントする
      SetValue(IN_BDATA01,param,GetValue(IN_ETLR00 + GetValue(IN_BDATA00,param),param) & 0x0F);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,0x10);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x0F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
    break;
    case I_IMONI_FLASH_NOTEMP:
      sprintf(test_name,"Iref");
      SetValue(IN_TESTSEL,param,13);      //Irefのモニター
      SetValue(IN_BDATA00,param,8);      //ETLR8をインクリメントする
      SetValue(IN_BDATA01,param,0x80);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x7F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0);    //0回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
      SetValue(IN_ETLR09,param,GetValue(IN_ETLR09,param)|0x80);       //レジスタ値の初期値
    break;
    case I_IMONI_FLASH_TEMP:
      sprintf(test_name,"IrefT");
      SetValue(IN_TESTSEL,param,14);      //IrefTのモニター
      SetValue(IN_BDATA00,param,9);      //ETLR9をインクリメントする
      SetValue(IN_BDATA01,param,0x00);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0x7F);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0);    //0回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);    //0回目でVSSMONに移行
      SetValue(IN_ETLR08,param,GetValue(IN_ETLR08,param)|0x80);       //レジスタ値の初期値
    break;
    case V_VHH_E_TEMP:
      sprintf(test_name,"VhhE_Temp");
      SetValue(IN_TESTSEL,param,2);       //Vhh_Eのモニター
      SetValue(IN_BDATA00,param,13);      //ETLR13をインクリメントする
      SetValue(IN_BDATA01,param,0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0xFF);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case V_VHH_P_TEMP:
      sprintf(test_name,"VhhP_Temp");
      SetValue(IN_TESTSEL,param,1);       //Vhh_Pのモニター
      SetValue(IN_BDATA00,param,14);      //ETLR14をインクリメントする
      SetValue(IN_BDATA01,param,0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0xFF);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0x100);    //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0x100);    //0x100回目でVSSMONに移行
    break;
    case I_IMONI_TEMP:
      sprintf(test_name,"Iref_Temp");
      SetValue(IN_TESTSEL,param,14);      //Irefのモニター
      SetValue(IN_BDATA00,param,12);      //ETLR12をインクリメントする
      SetValue(IN_BDATA01,param,0);       //レジスタ値の初期値
      SetValue(IN_BDATA03,param,1);       //レジスタ値のインクリメント量
      SetValue(IN_BDATA05,param,0xFF);    //レジスタ値のインクリメント回数
      SetValue(IN_LDATA0,param,0);        //0x100回目で電流モニターに移行
      SetValue(IN_LDATA1,param,0);        //0x100回目でVSSMONに移行
    break;
    
  }

  CpuModeMonitor_ALL(VCCMON_MON,&vs[0],param,W1SEC*10,CreatePatFileName("MonitorALL_Voltage"),"MonitorALL",&selvol[0]);

/**************************************************************************************/
/***   File Name Setting                                                            ***/
/**************************************************************************************/
  sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
  sprintf(item_name,"TapALL_%s_%dC_%s",test_name,Temprature,sample_name);
  sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

  time( &time_list );// time_list += V4V5K_TIME_OFS;

  //Monitor Log Out
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  fprintf(file_log,"%s Monitor vs Tap Result\n",test_name);
  fprintf(file_log,"Vcc = %d , Vddh = %d , Vdd = %d ,\n",vs[0],vs[1],vs[2]);
  fprintf(file_log,"ETLR%02d,%s\n",GetValue(IN_BDATA00,param),test_name);
  switch(mode){//レジスタの並びが順番でない物をCaseに作成する。
    case V_VRSG:
      for(j=0;j<=GetValue(IN_BDATA05,param);j++){
        switch (j){
          case 0: i = 1; break;
          case 1: i = 0; break;
          case 2: i = 3; break;
          case 3: i = 2; break;
          case 4: i = 5; break;
          case 5: i = 4; break;
          case 6: i = 7; break;
          case 7: i = 6; break;
        }
        fprintf(file_log,"0x%02X,%d\n",GetValue(IN_BDATA01,param) + (GetValue(IN_BDATA03,param) * i) ,selvol[i]);
      }
    break;
    case V_VREF10:
      for(j=0;j<=GetValue(IN_BDATA05,param);j++){
        if(j<=7) i = j + 8;
        else     i = j - 8;
        fprintf(file_log,"0x%02X,%d\n",GetValue(IN_BDATA01,param) + (GetValue(IN_BDATA03,param) * i) ,selvol[i]);
      }
    break;
    default:
      for(i=0;i<=GetValue(IN_BDATA05,param);i++){
        fprintf(file_log,"0x%02X,%d\n",GetValue(IN_BDATA01,param) + (GetValue(IN_BDATA03,param) * i) ,selvol[i]);
      }
    break;
  }
  fclose( file_log );

  if(mode == V_VDEMG)SetValue(IN_ETLR20,param,stack1);

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("\nTest-Time:%d[s]\n\n",time_list2-time_list);

  return(PASS);

}  

int SinraiMenu(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,k,j;
  int test_cond;
  int info[NUM_CHIPINFO];                    /* wmat chip information              */
  int result,target,Vresult;
  int first_flag,repeat_mode,inif_flag;
  char buff[200];
  char testname[20],item_name[20];

  tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = tempo6 = 0;
  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = 0;
  test_cond = 0;
  

  while( test_cond != EXIT){
    result = PASS;

    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Sample   | L# : %s  W# : %3d  S# : %05d\n",lotname,atoi(wafername),SampleNo);
    printf("| Info     |  X : %3d      Y : %3d\n",xy_location[0],xy_location[1]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|Shinraisei| 256K_Area RowSt(10) / 256K_Area All0/1(11) / MultiArea RowSt(12) / MultiArea All01(13) \n");
    printf("| Menu     | \n");
    printf("|          | \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| INIT     | Open&Short(1) / DeviceFunc[PASS](4) / DeviceFunc[FAIL](5)\n");
    printf("|          | Param Reset(6) / Tokunin_Init[FreeWord](8) / Tokunin_Init(9)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Other   | EXIT (e) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf(" Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();    

    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);

    switch(test_cond){
      case EXIT:
          return(PASS);
        break;
      case QUIT:
        DeviceSpecificPowerDown();
        if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
        PowerDown(1);
        break;
    /**************************************************************************/
    /* INIT(0~9)                                                              */
    /**************************************************************************/
    case 0:  /***  IF->DEVICEFUNC->SET I/F FROM Reg(T_MAT) INFO (0) ***/
      //if ( PASS == OpensShorts() ) {
        DeviceSpecificPowerUp();

        stack0 = SetValue(IN_REG,param,CHIPDATA);
        stack1 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"Register Read") ) {
          SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
          SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
          SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param));
          SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param));
          SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param));
          SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param));
          SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);
          tempo1 = GetValue(OUT_ETCR17,param);

          GetChipInfo( &info[0], param);

          if(tempo1 != 0x00){
            printf("\n --------- Warning!!! ---------\n");
            printf(  " --- Trimming Read Error!!! ---\n");
            printf(  " ------------------------------\n");
          }

          printf("************************************\n");
          printf("***   INIT PASS                  ***\n");
          printf("************************************\n");
          printf("\n");
        } else {
          printf("------------------------------------\n");
          printf("---   INIT FAIL                  ---\n");
          printf("------------------------------------\n");
          printf("\n");
        }      
      /*
      } else { 
        printf("------------------------------------\n");
        printf("---   PIN FAIL                  ---\n");
        printf("------------------------------------\n");
        printf("\n");
      }*/ 
      if(WSLot_CHECK() == FAIL) TempCheck(); // Extra5 Read
      
      break;
    case 1:   /*** OpensShorts ***/
      OpensShorts();
      DeviceSpecificPowerUp();
      break;
    case 4:   /*** DeviceFunc[PASS] ***/
      stack0 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS");
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 5:   /*** DeviceFunc[FAIL] ***/
      stack0 = SetValue(IN_TESTSEL,param,0x01);    /* 0x01 Fail Only */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG FAIL");
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 6:
      InitialValue(param);
      break;
    case 8:
      printf("Enter Free-Word ->\n");
      scanf("%s",&item_name);
      Tokunin_Init(param,item_name);
      break;
    case 9:
      Tokunin_Init(param,"");
      break;
    case 10:
		Ret_Distribution_256KB(&vs[0],W1SEC*waittime,param); //Row_st
      break;
    case 11:
		Ret_Distribution_All01(&vs[0],W1SEC*waittime,param);
      break;
    case 12:
		Ret_Distribution_multi_area_rowst(&vs[0],W1SEC*waittime,param); //Row_st
      break;
    case 13:
		Ret_Distribution_multi_area_all01(&vs[0],W1SEC*waittime,param); //Row_st
      break;


      default:
      break;


    }
  }
}


void CPcurrent_ALL(int mode, int vs[NUM_POWERSUPPLY],int waittime,interface_t* param,int temprature){
  int CpCurrent_Data[40],*CpCurrent_Path;
  int tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int result,etlr_num,etlr_set,etlr_mask,monitor_path;
  int voltage_start,voltage_delta,voltage_step,voltage_minus;
  int i,j,k;
  char file_name[200],mode_name[10];
  char item_name[128]; 
  char sample_name[128];
  FILE_T *file_log;

  printf("debug** mode = %d\n",mode);

  CpCurrent_Path = CpCurrent_Data;
  voltage_minus = -1;
  time( &time_list );// time_list += V4V5K_TIME_OFS;

  //Mode Initial
  switch(mode){
    case MONITOR_VPP_E:
      //Vssmon or Vccmon
      monitor_path = VSSMON_MON;
      //Max Voltage Set
      etlr_num  = 20;
      etlr_mask = 0x0F;
      etlr_set  = 0xF0;
      //Monitor Setting
      voltage_start = -1500;
      voltage_delta = -100;
      voltage_step = 18;
	  voltage_minus = 1;
      sprintf(mode_name,"VppE");
    break;
    case MONITOR_VPP_P:
      //Vssmon or Vccmon
      monitor_path = VSSMON_MON;
      //Max Voltage Set
      etlr_num  = 20;
      etlr_mask = 0xF0;
      etlr_set  = 0x0F;
      //Monitor Setting
      voltage_start = -2100;
      voltage_delta = -100;
      voltage_step = 18;
	  voltage_minus = 1;
      sprintf(mode_name,"VppW");
    break;
    case MONITOR_VPP_PW:
      //Vssmon or Vccmon
      monitor_path = VSSMON_MON;
      //Max Voltage Set
      etlr_num  = 21;
      etlr_mask = 0x0F;
      etlr_set  = 0xF0;
      //Monitor Setting
      voltage_start = -1000;
      voltage_delta = -100;
      voltage_step = 18;
	  voltage_minus = 1;
      sprintf(mode_name,"VppPW");
    break;
    case MONITOR_VHH_E:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 16;
      etlr_mask = 0xC0;
      etlr_set  = 0x3F;
      //Monitor Setting
      voltage_start = 5700;
      voltage_delta = 100;
      voltage_step = 30;
      sprintf(mode_name,"VhhE");
    break;
    case MONITOR_VHH_P:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 17;
      etlr_mask = 0xC0;
      etlr_set  = 0x3F;
      //Monitor Setting
      voltage_start = 6200;
      voltage_delta = 100;
      voltage_step = 36;
      sprintf(mode_name,"VhhW");
    break;
    case MONITOR_VRSG:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 18;
      etlr_mask = 0xF8;
      etlr_set  = 0x06;
      //Monitor Setting
      voltage_start = 2600;
      voltage_delta = 100;
      voltage_step = 9;
      sprintf(mode_name,"Vrsg");
    break;
    case MONITOR_VWI:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 19;
      etlr_mask = 0x0F;
      etlr_set  = 0xF0;
      //Monitor Setting
      voltage_start = 2000;
      voltage_delta = 100;
      voltage_step = 17;
      sprintf(mode_name,"Vwi");
    break;
    case MONITOR_VNOEMI:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 19;
      etlr_mask = 0xF0;
      etlr_set  = 0x0F;
      //Monitor Setting
      voltage_start = 2400;
      voltage_delta = 100;
      voltage_step = 17;
      sprintf(mode_name,"Vnoemi");
    break;
    case MONITOR_V33R:
      //Vssmon or Vccmon
      monitor_path = VCCMON_MON;
      //Max Voltage Set
      etlr_num  = 24;
      etlr_mask = 0xF8;
      etlr_set  = 0x07;
      //Monitor Setting
      voltage_start = 2900;
      voltage_delta = 100;
      voltage_step = 9;
      sprintf(mode_name,"V33r");
    break;
  }


  //Common Initial
  stack0 = SetValue(IN_TESTSEL, param, mode);
  stack1 = SetValue(IN_BDATA00, param, 0);
  stack2 = SetValue(IN_ETLR00+etlr_num, param, (GetValue(IN_ETLR00+etlr_num,param) & etlr_mask) |etlr_set);
  stack3 = SetValue(IN_LDATA0,param,voltage_start);
  stack4 = SetValue(IN_LDATA1,param,voltage_delta);
  stack5 = SetValue(IN_BDATA01,param,voltage_step);

  result = CpuMode_CPcurrentALL(monitor_path,&vs[0],param,waittime,CreatePatFileName("monitorvoltage_at"),"CP current ALL",CpCurrent_Path);

  //File Setting
  sprintf(sample_name,"%s_W%d_S%d",lotname,atoi(wafername),SampleNo);
  sprintf(item_name,"CPcurrentALL_%s_%dC_Vcc%d_Vddh%d_%s",mode_name,temprature,vs[0],vs[1],sample_name);
  sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

  //DataOut
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  fprintf(file_log,"%s CPcurrent vs TestVoltage Result\n",mode_name);
  fprintf(file_log,"Vcc = %d , Vddh = %d , Vdd = %d ,\n",vs[0],vs[1],vs[2]);
  fprintf(file_log,"Voltage,%s\n",mode_name);
  for(i=0;i<voltage_step;i++){
    fprintf(file_log,"%d,%d\n",voltage_start + (voltage_delta * i),CpCurrent_Data[i]*voltage_minus);
  }
  fclose( file_log );


  //Common Back
  SetValue(IN_TESTSEL, param, stack0);
  SetValue(IN_BDATA00, param, stack1);
  SetValue(IN_ETLR00+etlr_num, param, stack2);
  SetValue(IN_LDATA0,param,stack3);
  SetValue(IN_LDATA1,param,stack4);
  SetValue(IN_BDATA01,param,stack5);

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf("\nTest-Time:%d[s]\n\n",time_list2-time_list);

  return;
}


int ReadWindow_short(int mode, int vs[NUM_POWERSUPPLY],int waittime,interface_t* param,int temprature){
  int tempo0,tempo1,tempo2;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int vcc0[7],vcc1[7],vcc2[7],*vcc0_path,*vcc1_path,*vcc2_path;
  int IrefResult[17],*IrefResult_path;
  int CountData[7][16][30];
  int i,j,k;
  int result,addr,NoNum;
  int etlr09,etlr12,etlr25,etlr26,etlr27,etlr28,etlr29,etlr30,etlr32,etlr33,etlr34;
  char file_name[200],mode_name[10];
  char item_name[128]; 
  char sample_name[128];
  char time_buf[32];
  FILE_T *file_log;

  time( &time_list );// time_list += V4V5K_TIME_OFS;
  sprintf(time_buf,"%s",C_time((int *)&time_list));
  
  IrefResult_path = IrefResult;
  vcc0_path = vcc0;
  vcc1_path = vcc1;
  vcc2_path = vcc2;

    etlr09 = GetValue(IN_ETLR09,param);// ireftmpI
    etlr12 = GetValue(IN_ETLR12,param);// ireftmp
    etlr25 = GetValue(IN_ETLR25,param);// iref
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

	SetValue(IN_TESTSIZE0,param,0x19000);    /* 4kBx25=100kB */

  //Power Setting [0]:Typ Only
  vcc0[0] = vcc0[1] = vcc0[2] = 3300;
  vcc1[0] = 452; vcc1[1] = 500; vcc1[2] = 580;
  vcc2[0] = 443; vcc2[1] = 500; vcc2[2] = 728;
  //小領域のReadSize
  SetValue(IN_LDATA0,param,0x1000);
  //ETLR12 Tap Step
  SetValue(IN_BDATA00,param,8); //0x8-0xF
  //Power Step
  SetValue(IN_BDATA01,param,3);
  //ETLR12 Initial Number
  SetValue(IN_BDATA02,param,(GetValue(IN_ETLR12,param) & 0xF0) | 0x8); //0x8-0xF

  SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //

  printf("\n*** temparature? (ex:-40->40 /, 25-> 25/, 95-> 95) ->");DigitInput(&tempo2);
  temprature=tempo2;

	result = CpuMode_Monitor_PC(VSSMON_MON,param,waittime,CreatePatFileName("ReadWindow_zitan"),"ReadWindow-zitan",
                              vcc0_path,vcc1_path,vcc2_path,IrefResult_path);

  // i = Ireftmp Tap, j = Power Tap, k = ReadArea
  //BitCount : ECR -> CountData[PowerNum][AreaNum]
  addr = FBM_READTOP;
  for(i=0;i<GetValue(IN_BDATA00,param);i++){
    for(j=0;j<GetValue(IN_BDATA01,param);j++){
      for(k = 0;k < (GetValue(IN_TESTSIZE0,param)/GetValue(IN_LDATA0,param));k++){
        CountData[j][i][k] = ReadEcrMode8(addr,SIZE_L);
        addr+=4;
      }
    }
  }
  if(addr > (FBM_READTOP + 0x1000)){
    printf("Error!!! ReadCountData Over FBMSIZE 0x1000 < 0x%X\n",addr);
    return(FAIL);
  }

  /*******************************
   *  OLD File Make
   *******************************/
  for(i=0;i<GetValue(IN_BDATA00,param);i++){
    //File Setting
    sprintf(sample_name,"%s_W%d_S%dX%dY%d",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1]);
    sprintf(item_name,"ReadWindow_%s_IreftmpIdep_%dC_tmpI%02d",sample_name,tempo2,i);
    sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
  
    //FileOpen
    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    
    for(j=0;j<GetValue(IN_BDATA01,param);j++){
      //File hedder
      fprintf(file_log,"TOPADDR = %d\n",GetValue(IN_TOPADDR0,param));
      fprintf(file_log,"TESTSIZE = %d\n",GetValue(IN_TESTSIZE0,param));
      fprintf(file_log,"Vcc=%d,Vddh=%d,Vdd=%d,",vcc0[j],vcc1[j],vcc2[j]);
      fprintf(file_log,"Iref=%d,Ireftmp=%d,",IrefResult[0],IrefResult[i+1]);
      fprintf(file_log,"ETLR09=%02X,ETLR12=%02X,ETLR25=%02X,ETLR26=%02X,ETLR27=%02X,",
                        GetValue(IN_ETLR09,param),GetValue(IN_BDATA02,param)+i,GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param));
      fprintf(file_log,"ETLR28=%02X,ETLR29=%02X,ETLR30=%02X,ETLR32=%02X,ETLR33=%02X,ETLR34=%02X,",
                        GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param),GetValue(IN_ETLR30,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param));
      fprintf(file_log,"\n");
      fprintf(file_log,"No,1M,32M\n");

      //DataOut
      NoNum = 0;
      for(k = 0;k < (GetValue(IN_TESTSIZE0,param)/GetValue(IN_LDATA0,param));k++){
        fprintf(file_log,"%d,%d,%d\n",NoNum,0,CountData[j][i][k]);
        NoNum++;
      }
      fprintf(file_log,"\n");
    }
    fclose( file_log );
  }

  /*************************************
   *  TestSetting & Register File Make
   *************************************/
  delay_timer(get_site_number()*1000*500);//DUT毎に500ms Delayを追加(Fileの競合を防ぐため)
  //File Setting
  sprintf(item_name,"ReadWindow_TestInfoALL");
  sprintf(file_name,"%s%s%s.csv", glob_cdp, glob_datalog, item_name, get_site_number());
  
  //FileCheck
  if((file_log=fopen(file_name,"rt")) == NULL) {
    //------ headder Make Mode-----//
    if((file_log=fopen(file_name,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !\n",file_name);
      return(FAIL);
    }
    //Initial Set
    fprintf(file_log,"Lot,Wefer,S,X,Y,DUT,Time,ETLR12,ReadArea,");
    for(j=0;j<GetValue(IN_BDATA01,param);j++){            //Power
      fprintf(file_log,"Vcc[%d],Vddh[%d],Vdd[%d],",j,j,j);
    }
    for(i=0;i<=39;i++){                                   //ETLR
      fprintf(file_log,"ETLR%02d,",i);
    }
    for(i=0;i<=12;i++){                                   //ETCR
      fprintf(file_log,"ETCR%02d,",i);
    }
    for(i=0;i<=16;i++){                                   //BDATA
      fprintf(file_log,"IN_BDATA%02d,",i);
    }
    for(i=0;i<=5;i++){                                    //LDATA
      fprintf(file_log,"IN_LDATA%d,",i);
    }
    fprintf(file_log,"\n");
    fclose(file_log);
  }
  fclose(file_log);

  //FileOut
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  fprintf(file_log,"%s,%d,%d,%d,%d,%d,",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1],get_site_number());
  fprintf(file_log,"%-24.24s,",time_buf);
  fprintf(file_log,"0x%02X ~ 0x%02X,",GetValue(IN_BDATA02,param),GetValue(IN_BDATA02,param)+GetValue(IN_BDATA00,param));
  fprintf(file_log,"0x%05X ~ 0x%05X(0x%X Step),",GetValue(IN_TOPADDR0,param),GetValue(IN_TOPADDR0,param)+GetValue(IN_TESTSIZE0,param)-1,GetValue(IN_LDATA0,param));
  
  for(j=0;j<GetValue(IN_BDATA01,param);j++){            //Power
    fprintf(file_log,"%d,%d,%d,",vcc0[j],vcc1[j],vcc2[j]);
  }
  for(i=0;i<=39;i++){                                   //ETLR
    fprintf(file_log,"0x%02X,",GetValue(IN_ETLR00+i,param));
  }
  for(i=0;i<=12;i++){                                   //ETCR
    fprintf(file_log,"0x%02X,",GetValue(IN_ETCR00+i,param));
  }
  for(i=0;i<=16;i++){                                   //BDATA
    fprintf(file_log,"0x%02X,",GetValue(IN_BDATA00+i,param));
  }
  for(i=0;i<=5;i++){                                    //LDATA
    fprintf(file_log,"0x%02X,",GetValue(IN_LDATA0+i,param));
  }
  fprintf(file_log,"\n");
  fclose(file_log);

    SetValue(IN_ETLR09,param,etlr09);
    SetValue(IN_ETLR12,param,etlr12);
    SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);

    SetValue(IN_ETLR25,param,etlr25);// T1
    SetValue(IN_ETLR26,param,etlr26);// T1
    SetValue(IN_ETLR27,param,etlr27);// T2
    SetValue(IN_ETLR28,param,etlr28);// T2
    SetValue(IN_ETLR29,param,etlr29);// T2

  Firm_debugparam_Out(); //For Debug

  return(result);
}



int MINORI_Shmoo_Loop(int mode, int vs[NUM_POWERSUPPLY],int waittime,interface_t* param,int temprature, char* pat){
  int tempo0,tempo1,tempo2;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,j,k;
  int result,Pass_num;
  int Loop,vcc[NUM_POWERSUPPLY],Freq;
  char file_name[200],mode_name[10];
  char item_name[128]; 
  char sample_name[128];
  char clk_mode[20];
  FILE_T *file_log;
  int  FreqList[18] = {100,50,40,35,32,31,30,29,28,27,26,25,24,23,22,21,20};
  int  VddhList[10] = {1550,1500,1450,1400,1350,1300,1250,1200,1150,1100};
  int  VddList[10] = {1150,1100,1050,1000,950,900,850,790,720,650};
  int  VddList_max,VddhList_max,FreqList_max,Loop_max;
  FreqList_max = 16;
  VddhList_max = 9-2;
  VddList_max = 9-1;
  time( &time_list );// time_list += V4V5K_TIME_OFS;

  if(GetValue(IN_BDATA03,param)==1) sprintf(clk_mode,"Reverse-Phase");
  else sprintf(clk_mode,"In-Phase");

  //初回用Loop設定
  if(GetValue(IN_BDATA11,param) == 1){
    DeviceSpecificPowerUp();delay_timer(200);
    MinoriModeFunc_Repeat_F(MNORMAL,&vs[0],waittime,param,pat,"MINORI READ");
    SetValue(IN_BDATA11,param,0);
  }
  Ffpt = 0;
  //Vdd-Vddh Loop
  for(Loop = 0;Loop<=1;Loop++){
    vcc[0] = vs[0];vcc[1] = vs[1];vcc[2] = vs[2];
    switch (Loop){
      case 0:
        sprintf(mode_name,"VDD");
		Loop_max = VddList_max;
      break;
      case 1:
        sprintf(mode_name,"VDDH");
		Loop_max = VddhList_max;
      break;
    }
    //File Name Setting
    sprintf(sample_name,"%s_W%d_S%dX%dY%d",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1]);
    sprintf(item_name,"MINORI_SHMOO_%s_%dC_%s",mode_name,temprature,sample_name);
    sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
    //FileOpen
    if( NULL==(Fptdata = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(Fptdata," module,FLP0,patten,%s \n vcc,%d,vddh,%d,vdd,%d,CLK-Mode,%s\n",pat,vs[0],vs[1],vs[2],clk_mode);
    fprintf(Fptdata,"*** etlr25<7:0>=0x%X (iref)\n",GetValue(IN_ETLR25,param));
    fprintf(Fptdata,"*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
//    fprintf(Fptdata,"*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
    fprintf(Fptdata,"*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
//    fprintf(Fptdata,"*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
//    fprintf(Fptdata,"*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));
    fprintf(Fptdata,"*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
    fprintf(Fptdata,"*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
    fprintf(Fptdata,"*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));

    fprintf(Fptdata,"%s,",mode_name);
    //--周波数ヘッダー作成--//
    for(Freq = 0;Freq<=FreqList_max;Freq++){
      fprintf(Fptdata,"%d,",1000/FreqList[Freq]);
    }
    fprintf(Fptdata,"\n");

    //Vdd or Vddh Loop
    for(i=0;i<=Loop_max;i++){
      switch (Loop){
        case 0:
          vcc[2] = (VddList[i]*100)/158;
          fprintf(Fptdata,"%d,",VddList[i]);
        break;
        case 1:
          vcc[1] = VddhList[i]*10/25;
          fprintf(Fptdata,"%d,",VddhList[i]);
        break;
      }

      result = PASS;
      //Freq Loop
      for(Freq = 0;Freq <= FreqList_max;Freq++){
        SetValue(IN_EXTAL1,param,FreqList[Freq]);
		    if(Loop ==0)      printf("Vdd = %4dmV, Vddh = %4dmV T = %4dns : ",VddList[i],vcc[1]*25/10,GetValue(IN_EXTAL1,param));
		    else if(Loop ==1) printf("Vdd = %4dmV, Vddh = %4dmV T = %4dns : ",(vcc[2]*158)/100,VddhList[i],GetValue(IN_EXTAL1,param));
        Pass_num = 0;
        //MINORI Loop *10
        for(j=0;j<=9;j++){
          DeviceSpecificPowerUp();
          result = MinoriModeFunc_Repeat(MNORMAL,&vcc[0],waittime,param,pat,"MINORI READ");
          if(result == PASS)Pass_num++;
          else if(result == CFAIL)break;
        }
        //Result Set
        if(Pass_num == 10){
          printf("***PASS*** : %d\n",Pass_num);
          fprintf(Fptdata,"%d,",Pass_num);
        }else if(result == CFAIL){
          printf("---Error!!!--- : %d\n",Pass_num);
          fprintf(Fptdata,"E,");
        }else{
          printf("---FAIL--- : %d\n",Pass_num);
          fprintf(Fptdata,"%d,",Pass_num);
        }
      }
      fprintf(Fptdata,"\n");
    }
    fclose(Fptdata);

  }
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf(" *** Test Time = %d[s]***\n",time_list2-time_list);
  Ffpt = 1;
  return(PASS);

}

int RomBoot_Shmoo_Loop(int mode, int vs[NUM_POWERSUPPLY],interface_t* param,int temprature){
  int tempo0,tempo1,tempo2;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,j,k,n;
  int result,Pass_num,TimeOut;
  int Loop,vcc[NUM_POWERSUPPLY],Freq;
  char file_name[200],mode_name[10];
  char item_name[128]; 
  char sample_name[128];
  char BootOption1[40],BootOption2[40];
  char RegModeName[40];
  FILE_T *file_log;
  int  VddList_max,VddhList_max,FreqList_max,Loop_max;
  int in_ldata[16+2];
  int  FreqList[20] = {50,42,36,31,28,25,22,20};
  int  VddhList[10] = {1500,1450,1400,1350,1300,1250,1200};
  int  VddList[10] = {900,850,820,790,720};
  FreqList_max = 8-1;
  VddhList_max = 7-1;
  VddList_max = 5-1;
  time( &time_list );// time_list += V4V5K_TIME_OFS;

  for( n = 0; n<16; n++ ) in_ldata[ n ] = 0x55345678;
  SetValue( IN_EXTAL_PS, param,100*1000);
  vcc[0] = vs[0];vcc[1]=vcc[2]=0;
  result = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vcc[0], W1SEC*30, param, CreatePatFileName( "flash_boot_dijkstra_v004" ), "Program_BootCode", &in_ldata[0] );
  if(result != PASS) return(FAIL);

  //--- RomBoot Option設定 ---//
  n = 0;
  sprintf(BootOption1,"Normal-Mode");
  sprintf(BootOption2,"HOCO-ON");
  sprintf(RegModeName,"RegMode,%d",mode & 0x000000FF );
  switch(mode & 0x000000FF){
    case 0:
    break;
    case 1:
      in_ldata[n] = InLdataSet_Reg(1,26,0xFA);//ETLR26 -> 0xFA(Iref -> 2.0uA)
      n++;
    break;
  }
  if((mode & 0x00000100) != 0 ){//Boost Mode ON
    in_ldata[n] = 0x04000000; 
    n++;
    sprintf(BootOption1,"Boost-Mode");
  }
  if((mode & 0x00000200) == 0 ){//HOCO OFF
    in_ldata[n] = 0x05000000;
    n++;
    sprintf(BootOption2,"HOCO-OFF");
  }
  Ffpt = 0;
  //Vdd-Vddh Loop
  for(Loop = 0;Loop<=1;Loop++){
    vcc[0] = vs[0];vcc[1] = vs[1];vcc[2] = vs[2];
    switch (Loop){
      case 0:
        sprintf(mode_name,"VDD");
		Loop_max = VddList_max;
      break;
      case 1:
        sprintf(mode_name,"VDDH");
		Loop_max = VddhList_max;
      break;
    }
    //File Name Setting
    sprintf(sample_name,"%s_W%d_S%dX%dY%d",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1]);
    sprintf(item_name,"RomBoot_SHMOO_%s_%dC_%s",mode_name,temprature,sample_name);
    sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
    //FileOpen
    if( NULL==(Fptdata = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(Fptdata," module,FLP0,mode,%s,%s,%s,0x%X \n vcc,%d,vddh,%d,vdd,%d,\n",BootOption1,BootOption2,RegModeName,mode,vs[0],vs[1],vs[2]);

    fprintf(Fptdata,"%s,",mode_name);
    //--周波数ヘッダー作成--//
    for(Freq = 0;Freq<=FreqList_max;Freq++){
      fprintf(Fptdata,"%d,",1000/FreqList[Freq]);
    }
    fprintf(Fptdata,"\n");

    //Vdd or Vddh Loop
    for(i=0;i<=Loop_max;i++){
      switch (Loop){
        case 0:
          vcc[2] = (VddList[i]*100)/158;
          fprintf(Fptdata,"%d,",VddList[i]);
        break;
        case 1:
          vcc[1] = VddhList[i]*10/25;
          fprintf(Fptdata,"%d,",VddhList[i]);
        break;
      }

      result = PASS;
      //Freq Loop
      for(Freq = 0;Freq <= FreqList_max;Freq++){
        SetValue(IN_EXTAL1,param,FreqList[Freq]);
		    if(Loop ==0)      printf("Vdd = %4dmV, Vddh = %4dmV T = %4dns : ",VddList[i],vcc[1]*25/10,GetValue(IN_EXTAL1,param));
		    else if(Loop ==1) printf("Vdd = %4dmV, Vddh = %4dmV T = %4dns : ",(vcc[2]*158)/100,VddhList[i],GetValue(IN_EXTAL1,param));
        DeviceSpecificPowerUp();
        TimeOut = (W1SEC*(35 * GetValue(IN_EXTAL1,param)+1000))/1000;
        SetValue( IN_EXTAL_PS, param,GetValue(IN_EXTAL1,param)*1000);
        result = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], TimeOut, param, CreatePatFileName( "flash_boot_jmp4_v004" ), "jmp_RomBoot_Loop", &in_ldata[0] );
        //Result Set
        if(result == PASS){
          fprintf(Fptdata,"P,");
        }else if(result == FAIL){
          fprintf(Fptdata,"F,");
        }else{
          fprintf(Fptdata,"E,");
        }
      }
      fprintf(Fptdata,"\n");
    }
    fclose(Fptdata);

  }
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  printf(" *** Test Time = %d[s]***\n",time_list2-time_list);
  Ffpt = 1;
  return(PASS);

}


int RomBoot_Shmoo_Menu(int vs[NUM_POWERSUPPLY],interface_t* param){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,k,j,test_cond,temprature,mode;
  int while_flag,OPE_flag,HOCO_flag;
  char OPE_Name[15],HOCO_Name[15];
  int RegMode,RegModeMax,OPE_Max,HOCO_Max;
  //--- Initial ---//
  OPE_flag = 0;
  HOCO_flag = 1;
  OPE_Max = 1;
  HOCO_Max = 1;
  RegModeMax = 1;
  temprature = 25;

  while_flag = 1;
  while(while_flag){
    if(OPE_flag==0)  sprintf(OPE_Name,"Normal-Mode");
    else             sprintf(OPE_Name,"Boost-Mode");
    if(HOCO_flag==0) sprintf(HOCO_Name,"OFF");
    else             sprintf(HOCO_Name,"ON");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Sample   | L# : %s  W# : %3d  S# : %05d\n",lotname,atoi(wafername),SampleNo);
    printf("| Info     |  X : %3d      Y : %3d\n",xy_location[0],xy_location[1]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Register | Register No Change (0)\n");
    printf("| Mode&EXE | ETLR26 -> 0xFA[Iref -> 2.0uA](1)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|BootOption| OPE : %s(101)  HOCO : %s(102)\n",OPE_Name,HOCO_Name);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Power   | Vcc:%4dmV(vcc) Vddh:%4dmV(vddh) Vdd:%4dmV(vdd)\n", vs[0],vs[1],vs[2]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Other   | temprature : %dC(500) \n", temprature);
    printf("|          | ALL Exe(99) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("Please enter Test Condition -> ");DigitInput(&test_cond);
    switch(test_cond){
    case QUIT:
      DeviceSpecificPowerDown();
      if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
      PowerDown(1);
      break;
    case EXIT: return(PASS); break;
    //--- Test Condition Setting ---//
    case SET_CVCC:  printf("enter vcc(mV) -> ");  DigitInput(&vs[0]); break;
    case SET_CVDDH: printf("enter vddh(mV) -> "); DigitInput(&vs[1]); break;
    case SET_CVDD:  printf("enter vdd(mV) -> ");  DigitInput(&vs[2]); break;
    case 500 :      printf("enter temprature(C) -> ");  DigitInput(&temprature); break;
    case 101 :      printf("Select OPE : Normal-Mode(0) / Boost-Mode(1) -> ");  DigitInput(&OPE_flag); break;
    case 102 :      printf("Select HOCO : OFF(0) / ON(1) -> ");  DigitInput(&HOCO_flag); break;

    //--- Shmoo Exe ---//
    case 99:
      for(RegMode = 0;RegMode<=RegModeMax;RegMode++){
        for(OPE_flag = 0;OPE_flag<=OPE_Max;OPE_flag++){
          for(HOCO_flag = 0;HOCO_flag<=HOCO_Max;HOCO_flag++){
            mode = RegMode | (OPE_flag << 8) | (HOCO_flag << 9);
            RomBoot_Shmoo_Loop(mode,&vs[0],param,temprature);
          }
        }
      }
    break;

    case 0:
    case 1:
      mode = test_cond | (OPE_flag << 8) | (HOCO_flag << 9);
      RomBoot_Shmoo_Loop(mode,&vs[0],param,temprature);
    break;
    default:
    break;
    }
  }


}