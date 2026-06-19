/******************************************************************************/
/*!
  @file RC04EX_Sample_Set.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Sample_Set(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_wvol_bef,result_vhhw_bef,result_vppw_bef,result_vhhe_bef,result_vppe_bef;
  int result_wvol_aft,result_vhhw_aft,result_vppw_aft,result_vhhe_aft,result_vppe_aft;
  int result_vrsg_aft,result_vnoemi_aft,result_vdemg_aft,result_vwi_aft;
  int error_flag;
  int info[32];                    /* wmat chip information              */
  int trimvalue[8];                /*                                    */
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = 0;

/**************************************************************************************/
/**************************************************************************************/
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
//      if ( PASS == OpensShorts() ) {
        DeviceSpecificPowerUp();

        SetValue(IN_REG,param,CHIPDATA);
        SetValue(IN_TESTSEL,param,0x00);
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) {
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
          printf("************************************\n");
          printf("***   INIT PASS                  ***\n");
          printf("************************************\n");
          printf("\n");
        } else {
          printf("------------------------------------\n");
          printf("---   INIT FAIL                  ---\n");
          printf("------------------------------------\n");
          printf("\n");
	  return(PASS);
        }      

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
    SetValue(IN_REG,param,3);

    SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param)&0xF8)|0x06); // Vrsg      3.4V //
    SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param)&0xC7)|0x30); // Vnoemi_rd 3.4V //
    SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0xF0)|0x0C); // Vnoemi_pe 3.6V //
    SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param)&0x0F)|0x00); // Vwi       2.0V //
    SetValue(IN_ETLR21,param,(GetValue(IN_ETLR21,param)&0xF0)|0x09); // Vdemg    -1.0V //

    SetValue(IN_ETLR26,param,0xCC); // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR27,param,0xFC); // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR28,param,0xF1); // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR29,param,0xF1); // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR30,param,0xF1); // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR32,param,0x31); // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
    SetValue(IN_ETLR33,param,0x11); // T4 saout DelayTime (min+1Tap) //
    SetValue(IN_ETLR34,param,0x11); // T6 T4-T5 DelayTime (min+1Tap) //
    printf("*** WS2 DelayTrim Condition is evaluating now. ");
    printf("*** WS2  = WS1 DelayTrim setting!!! ");

    printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
    printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
    printf("*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
    printf("*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
    printf("*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));
    printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
    printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
    printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B);
      info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B);
      info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B);
      info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10]= ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11]= ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B);
      info[12]= ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);
      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Sample-Set",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

/**************************************************************************************/
/***   Before Vol                                                                   ***/
/**************************************************************************************/
  fprintf(file_log,"\n"); 
//  SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
//  SetValue(IN_BDATA00,param,0);
//  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
//  result_vppe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw_bef = selvol;
//  SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
//  SetValue(IN_BDATA00,param,0);
//  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
//  result_vhhe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw_bef = selvol;

  result_wvol_bef = result_vhhw_bef - result_vppw_bef;

/**************************************************************************************/
/***   After Vol                                                                   ***/
/**************************************************************************************/

  /* Vhh -0.5V(-11Tap) */
  trimvalue[V_VHH_P]              = result_vhhw_bef - 500;
  printf("AutoTrim Vhh Write:%06d[mV]\n", trimvalue[V_VHH_P]);
  SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
  SetValue(IN_LDATA0,param,30*10000); /* 30ms(10000*100ns=1ms) */
  CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_Vhh_at"),"AUTOTRIM_VHH_W",&trimvalue[V_VHH_P]);
  SetValue(IN_LDATA0,param,0);

  /* Vpp -0.5V(-5Tap) */
  tempo = (GetValue(IN_ETLR20,param) & 0x0F) + 5;
  SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param) & 0xF0) | tempo);

  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw_aft = selvol;

  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw_aft = selvol;

  result_wvol_aft = result_vhhw_aft - result_vppw_aft;

  SetValue(IN_TESTSEL,param,MONITOR_VDEMG);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VDEMG",&selvol);
  result_vdemg_aft = selvol;

  SetValue(IN_TESTSEL,param,MONITOR_VRSG);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VRSG",&selvol);
  result_vrsg_aft = selvol;

  SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_PE",&selvol);
  result_vnoemi_aft = selvol;

  SetValue(IN_TESTSEL,param,MONITOR_VWI);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VWI",&selvol);
  result_vwi_aft = selvol;


/**************************************************************************************/
/***   Log Out                                                                      ***/
/**************************************************************************************/

  fprintf(file_log,"Before_Vhh_w/Vpp_w = ,%d,%d\n",result_vhhw_bef,result_vppw_bef); 
  fprintf(file_log,"After_Vhh_w/Vpp_w = ,%d,%d\n" ,result_vhhw_aft,result_vppw_aft); 
  fprintf(file_log,"After_Vnoemi = ,%d\n" ,result_vnoemi_aft); 
  fprintf(file_log,"After_Vrsg   = ,%d\n" ,result_vrsg_aft); 
  fprintf(file_log,"After_Vdemg  = ,%d\n" ,result_vdemg_aft); 
  fprintf(file_log,"After_Vwi    = ,%d\n" ,result_vwi_aft); 
  fprintf(file_log,"\n"); 
  fprintf(file_log,"ETLR0,ETLR1,ETLR2,ETLR3,ETLR4,ETLR5,ETLR6,ETLR7,ETLR8,ETLR9,");
  fprintf(file_log,"ETLR10,ETLR11,ETLR12,ETLR13,ETLR14,ETLR15,ETLR16,ETLR17,ETLR18,ETLR19,");
  fprintf(file_log,"ETLR20,ETLR21,ETLR22,ETLR23,ETLR24,ETLR25,ETLR26,ETLR27,ETLR28,ETLR29,");
  fprintf(file_log,"ETLR30,ETLR31,ETLR32,ETLR33,ETLR34,ETLR35,ETLR36,ETLR37,ETLR38,ETLR39\n");
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,",
                    GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),GetValue(IN_ETLR04,param),
                    GetValue(IN_ETLR05,param),GetValue(IN_ETLR06,param),GetValue(IN_ETLR07,param),GetValue(IN_ETLR08,param),GetValue(IN_ETLR09,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,",
                    GetValue(IN_ETLR10,param),GetValue(IN_ETLR11,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR13,param),GetValue(IN_ETLR14,param),
                    GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),GetValue(IN_ETLR17,param),GetValue(IN_ETLR18,param),GetValue(IN_ETLR19,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,",
                    GetValue(IN_ETLR20,param),GetValue(IN_ETLR21,param),GetValue(IN_ETLR22,param),GetValue(IN_ETLR23,param),GetValue(IN_ETLR24,param),
                    GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X\n",
                    GetValue(IN_ETLR30,param),GetValue(IN_ETLR31,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param),
                    GetValue(IN_ETLR35,param),GetValue(IN_ETLR36,param),GetValue(IN_ETLR37,param),GetValue(IN_ETLR38,param),GetValue(IN_ETLR39,param));

  /*******************/ 
  fclose( file_log );

/**************************************************************************************/
/***   Result                                                                       ***/
/**************************************************************************************/
  printf("\n"); 
  if (( result_wvol_aft>(result_wvol_bef-50) ) && ( result_wvol_aft<(result_wvol_bef+50) )) printf("*** Write-Voltage Pass ***\n");
  else                                                                                      printf("=== Write-Voltage Fail ===\n"); 
  if (( result_vnoemi_aft>(3600-80)) && ( result_vnoemi_aft<(3600+80) ))                    printf("*** Vnoemi        Pass ***\n");
  else                                                                                      printf("=== Vnoemi        Fail ===\n");
  if (( result_vrsg_aft>(3400-80)) && ( result_vrsg_aft<(3400+80) ))                        printf("*** Vrsg          Pass ***\n");
  else                                                                                      printf("=== Vesg          Fail ===\n");
  if (( result_vwi_aft>(2000-80)) && ( result_vwi_aft<(2000+80) ))                          printf("*** Vwi           Pass ***\n");
  else                                                                                      printf("=== Vwi           Fail ===\n");
  if (( result_vdemg_aft>(-1000-80)) && ( result_vdemg_aft<(-1000+80) ))                    printf("*** Vdemg         Pass ***\n");
  else                                                                                      printf("=== Vdemg         Fail ===\n");

  printf("Write-Voltage(Before) = %d[mV]   (%d/%d)\n",result_wvol_bef,result_vhhw_bef,result_vppw_bef); 
  printf("Write-Voltage(After ) = %d[mV]   (%d/%d)\n",result_wvol_aft,result_vhhw_aft,result_vppw_aft); 
  printf("Vnoemi       (After ) = %d[mV]   Target: 3600mV\n",result_vnoemi_aft); 
  printf("Vrsgi        (After ) = %d[mV]   Target: 3400mV\n",result_vrsg_aft); 
  printf("Vwi          (After ) = %d[mV]   Target: 2000mV\n",result_vwi_aft); 
  printf("Vdemg        (After ) = %d[mV]   Target:-1000mV\n",result_vdemg_aft); 
  printf("\n"); 

/**************************************************************************************/
/***   Register ReWrite                                                             ***/
/**************************************************************************************/
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); DigitInput(&cond1);

  if ( cond1 == 0 ) {
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    SetValue(IN_BDATA01,param,0);      
    
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
  }

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
  /*********************/

}





