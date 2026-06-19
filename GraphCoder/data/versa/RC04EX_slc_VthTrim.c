// ****************************************************************************
// ! @file RC04EXslc_VthTrim.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Vth_Trim(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25;
  int error_flag;
  int info[32];
  // wmat chip information
  int trimvalue[8];

  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = 0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;

// ************************************************************************************
// ************************************************************************************
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);
      etlr_25 = GetValue(IN_ETLR25,param);

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
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
          printf("************************************");
          printf("***   INIT PASS                  ***");
          printf("************************************");
          printf("");
        } else {
          printf("------------------------------------");
          printf("---   INIT FAIL                  ---");
          printf("------------------------------------");
          printf("");
	  return(PASS);
        }
//      } else { 
//        printf("------------------------------------");
//        printf("---   PIN FAIL                  ---");
//        printf("------------------------------------");
//        printf("");
//	return(PASS);
//      } 

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
    //printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 / FLP0&FLI1:3 / FLP0&FLI1&FLI2:4] ->"); DigitInput(&mat);
  mat = 0;
  //IoFLI1/2͖
  printf("*** Test Data-Area [Loss&Gain:0 / Loss:1 / Gain:2] ->"); DigitInput(&area);

  // printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond0); if ( cond0 == 1 ) SetValue(IN_ETLR25,param,0xFD); // 2.5uA // if ( cond0 == 2 ) SetValue(IN_ETLR25,param,0xFF); // 3.5uA //
  SetValue(IN_ETLR25,param,0xFA);
  //Iref = 2.0uA
  

  printf("*** Vth_w Select   [0.8V:0 / 1.0V:1 / 1.2V:2 / 1.4V:3 ] ->"); DigitInput(&cond0);
  if ( cond0 == 0 ) SetValue(IN_LDATA0,param, 800);
  if ( cond0 == 1 ) SetValue(IN_LDATA0,param,1000);
  if ( cond0 == 2 ) SetValue(IN_LDATA0,param,1200);
  if ( cond0 == 3 ) SetValue(IN_LDATA0,param,1400);

  printf("*** Vth_e Select   [-0.9V:0 / -0.7V:1 / -0.5V:2 / -0.4V:3 / -0.3V:4 ] ->"); DigitInput(&cond0);
  if ( cond0 == 0 ) SetValue(IN_LDATA1,param,-900);
  if ( cond0 == 1 ) SetValue(IN_LDATA1,param,-700);
  if ( cond0 == 2 ) SetValue(IN_LDATA1,param,-500);
  if ( cond0 == 3 ) SetValue(IN_LDATA1,param,-400);
  if ( cond0 == 4 ) SetValue(IN_LDATA1,param,-300);
    //SetValue(IN_LDATA1,param,-700);     // Vth_e

  printf("*** Iref-Trim      [Yes:0 / No:1 ] ->"); DigitInput(&cond2);

  printf("*** Vhh-Temp       [ON:0 (W_-2.45mV&E_-3.62mV (@7.5V)) / OFF:1 ] ->"); DigitInput(&cond3);
  if ( cond3 == 0 ) {
    SetValue(IN_ETLR13,param,0xC8);
    // Vhh_e
    SetValue(IN_ETLR14,param,0x98);
    // Vhh_w
    SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param) & 0x3F) | 0xC0);
    // Temp_ON
  }
  if ( cond3 == 1 ) {
    SetValue(IN_ETLR15,param,0x3F);
    // Temp_OFF
  }

// printf("*** Delay fixed Trimming   [WS1:0 / WS2:1 ] ->"); DigitInput(&cond4); if ( cond4 == 0 ){ SetValue(IN_ETLR26,param,0xCC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR27,param,0xFC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR28,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR29,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR30,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR32,param,0x31); // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) // SetValue(IN_ETLR33,param,0x11); // T4 saout DelayTime (min+1Tap) // SetValue(IN_ETLR34,param,0x11); // T6 T4-T5 DelayTime (min+1Tap) // } if ( cond4 == 1 ){ SetValue(IN_ETLR26,param,0xCC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR27,param,0xFC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR28,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR29,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR30,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR32,param,0x31); // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) // SetValue(IN_ETLR33,param,0x11); // T4 saout DelayTime (min+1Tap) // SetValue(IN_ETLR34,param,0x11); // T6 T4-T5 DelayTime (min+1Tap) // printf("*** WS2 DelayTrim Condition is evaluating now. "); printf("*** WS2  = WS1 DelayTrim setting!!! "); } printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)",GetValue(IN_ETLR26,param)); printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)",GetValue(IN_ETLR27,param)); printf("*** etlr28<7:0>=0x%X (Discharge FLP0)",GetValue(IN_ETLR28,param)); printf("*** etlr29<7:0>=0x%X (Discharge FLI1)",GetValue(IN_ETLR29,param)); printf("*** etlr30<7:0>=0x%X (Discharge FLI2)",GetValue(IN_ETLR30,param)); printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)",GetValue(IN_ETLR32,param)); printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)",GetValue(IN_ETLR33,param)); printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)",GetValue(IN_ETLR34,param));
	
// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);
      // Extra5
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

      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);
      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_VthTrim",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

// ************************************************************************************
// **   Iref-Trim                                                                    **
// ************************************************************************************
  if (cond2 == 0) {
    SetValue(IN_REG,param,0x03);
    SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);

      SetValue(IN_BDATA15,param,UA25);
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
    result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    result_iref = selvol;
  }
  SetValue(IN_ETLR22,param,0xFB);
  //Pumpup(8MHz->16MHz)
  printf("ETLR22;%02X",GetValue(IN_ETLR22,param));

// ************************************************************************************
// **   Loss Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    //fprintf(file_log,"<<Loss>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple Erase ********************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

        // Program ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result <= (GetValue(IN_LDATA0,param) -800)) judg =999;
    else if (result <= (GetValue(IN_LDATA0,param) -760)) judg = 20;
    else if (result <= (GetValue(IN_LDATA0,param) -720)) judg = 19;
    else if (result <= (GetValue(IN_LDATA0,param) -680)) judg = 18;
    else if (result <= (GetValue(IN_LDATA0,param) -640)) judg = 17;
    else if (result <= (GetValue(IN_LDATA0,param) -600)) judg = 16;
    else if (result <= (GetValue(IN_LDATA0,param) -560)) judg = 15;
    else if (result <= (GetValue(IN_LDATA0,param) -520)) judg = 14;
    else if (result <= (GetValue(IN_LDATA0,param) -480)) judg = 13;
    else if (result <= (GetValue(IN_LDATA0,param) -440)) judg = 12;
    else if (result <= (GetValue(IN_LDATA0,param) -400)) judg = 11;
    else if (result <= (GetValue(IN_LDATA0,param) -360)) judg = 10;
    else if (result <= (GetValue(IN_LDATA0,param) -320)) judg =  9;
    else if (result <= (GetValue(IN_LDATA0,param) -280)) judg =  8;
    else if (result <= (GetValue(IN_LDATA0,param) -240)) judg =  7;
    else if (result <= (GetValue(IN_LDATA0,param) -200)) judg =  6;
    else if (result <= (GetValue(IN_LDATA0,param) -160)) judg =  5;
    else if (result <= (GetValue(IN_LDATA0,param) -120)) judg =  4;
    else if (result <= (GetValue(IN_LDATA0,param) - 80)) judg =  3;
    else if (result <= (GetValue(IN_LDATA0,param) - 40)) judg =  2;
    else if (result <= (GetValue(IN_LDATA0,param) -  0)) judg =  1;
    else if (result >= (GetValue(IN_LDATA0,param) +800)) judg =999;
    else if (result >= (GetValue(IN_LDATA0,param) +760)) judg =-19;
    else if (result >= (GetValue(IN_LDATA0,param) +720)) judg =-18;
    else if (result >= (GetValue(IN_LDATA0,param) +680)) judg =-17;
    else if (result >= (GetValue(IN_LDATA0,param) +640)) judg =-16;
    else if (result >= (GetValue(IN_LDATA0,param) +600)) judg =-15;
    else if (result >= (GetValue(IN_LDATA0,param) +560)) judg =-14;
    else if (result >= (GetValue(IN_LDATA0,param) +520)) judg =-13;
    else if (result >= (GetValue(IN_LDATA0,param) +480)) judg =-12;
    else if (result >= (GetValue(IN_LDATA0,param) +440)) judg =-11;
    else if (result >= (GetValue(IN_LDATA0,param) +400)) judg =-10;
    else if (result >= (GetValue(IN_LDATA0,param) +360)) judg = -9;
    else if (result >= (GetValue(IN_LDATA0,param) +320)) judg = -8;
    else if (result >= (GetValue(IN_LDATA0,param) +280)) judg = -7;
    else if (result >= (GetValue(IN_LDATA0,param) +240)) judg = -6;
    else if (result >= (GetValue(IN_LDATA0,param) +200)) judg = -5;
    else if (result >= (GetValue(IN_LDATA0,param) +160)) judg = -4;
    else if (result >= (GetValue(IN_LDATA0,param) +120)) judg = -3;
    else if (result >= (GetValue(IN_LDATA0,param) + 80)) judg = -2;
    else if (result >= (GetValue(IN_LDATA0,param) + 60)) judg = -1;

    fprintf(file_log,"No%d,%d,%d,ETLR17=0x%X,",i,result,judg,GetValue(IN_ETLR17,param));
    result_loss = judg;
    loop_loss   = i;
    vth_loss    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass

    SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + judg) | 0xC0);
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Gain Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 2)) {
    //fprintf(file_log,"<<Gain>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple writing *******************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    SetValue(IN_ETCR08,param,0x00);

        // Erase ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result >= (GetValue(IN_LDATA1,param) +760)) judg =999;
    else if (result >= (GetValue(IN_LDATA1,param) +720)) judg = 20;
    else if (result >= (GetValue(IN_LDATA1,param) +680)) judg = 19;
    else if (result >= (GetValue(IN_LDATA1,param) +640)) judg = 18;
    else if (result >= (GetValue(IN_LDATA1,param) +600)) judg = 17;
    else if (result >= (GetValue(IN_LDATA1,param) +560)) judg = 16;
    else if (result >= (GetValue(IN_LDATA1,param) +520)) judg = 15;
    else if (result >= (GetValue(IN_LDATA1,param) +480)) judg = 14;
    else if (result >= (GetValue(IN_LDATA1,param) +440)) judg = 13;
    else if (result >= (GetValue(IN_LDATA1,param) +400)) judg = 12;
    else if (result >= (GetValue(IN_LDATA1,param) +360)) judg = 11;
    else if (result >= (GetValue(IN_LDATA1,param) +320)) judg = 10;
    else if (result >= (GetValue(IN_LDATA1,param) +280)) judg =  9;
    else if (result >= (GetValue(IN_LDATA1,param) +240)) judg =  8;
    else if (result >= (GetValue(IN_LDATA1,param) +200)) judg =  7;
    else if (result >= (GetValue(IN_LDATA1,param) +160)) judg =  6;
    else if (result >= (GetValue(IN_LDATA1,param) +120)) judg =  5;
    else if (result >= (GetValue(IN_LDATA1,param) + 80)) judg =  4;
    else if (result >= (GetValue(IN_LDATA1,param) + 60)) judg =  3;
    else if (result >= (GetValue(IN_LDATA1,param) + 40)) judg =  2;
    else if (result >= (GetValue(IN_LDATA1,param) + 20)) judg =  1;
    else if (result >= (GetValue(IN_LDATA1,param) +  0)) judg =  1;
    else if (result <= (GetValue(IN_LDATA1,param) -840)) judg =999;
    else if (result <= (GetValue(IN_LDATA1,param) -800)) judg =-20;
    else if (result <= (GetValue(IN_LDATA1,param) -760)) judg =-19;
    else if (result <= (GetValue(IN_LDATA1,param) -720)) judg =-18;
    else if (result <= (GetValue(IN_LDATA1,param) -680)) judg =-17;
    else if (result <= (GetValue(IN_LDATA1,param) -640)) judg =-16;
    else if (result <= (GetValue(IN_LDATA1,param) -600)) judg =-15;
    else if (result <= (GetValue(IN_LDATA1,param) -560)) judg =-14;
    else if (result <= (GetValue(IN_LDATA1,param) -520)) judg =-13;
    else if (result <= (GetValue(IN_LDATA1,param) -480)) judg =-12;
    else if (result <= (GetValue(IN_LDATA1,param) -440)) judg =-11;
    else if (result <= (GetValue(IN_LDATA1,param) -400)) judg =-10;
    else if (result <= (GetValue(IN_LDATA1,param) -360)) judg = -9;
    else if (result <= (GetValue(IN_LDATA1,param) -320)) judg = -8;
    else if (result <= (GetValue(IN_LDATA1,param) -280)) judg = -7;
    else if (result <= (GetValue(IN_LDATA1,param) -240)) judg = -6;
    else if (result <= (GetValue(IN_LDATA1,param) -200)) judg = -5;
    else if (result <= (GetValue(IN_LDATA1,param) -160)) judg = -4;
    else if (result <= (GetValue(IN_LDATA1,param) -120)) judg = -3;
    else if (result <= (GetValue(IN_LDATA1,param) - 80)) judg = -2;

    fprintf(file_log,"No%d,%d,%d,ETLR16=0x%X,",i,result,judg,GetValue(IN_ETLR16,param));
    result_gain = judg;
    loop_gain   = i;
    vth_gain    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass
   	if      ( error_flag ==1 ) break;
   	// Vhh_e Tap  Limit

    SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + judg) | 0xC0);

    if(GetValue(IN_ETLR16,param) > 0xFF){
      printf("!! Tap Limit !!");
      SetValue(IN_ETLR16,param,0xFF | 0xC0);
     	error_flag = 1;
    }
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Log Out                                                                      **
// ************************************************************************************
  fprintf(file_log,"");
  SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
  result_vppe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
  result_vhhe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw = selvol;
  fprintf(file_log,"Vhh_w/Vpp_w = ,%d,%d",result_vhhw,result_vppw);
  fprintf(file_log,"Vhh_e/Vpp_e = ,%d,%d",result_vhhe,result_vppe);
  fprintf(file_log,"Iref = ,%d[nA]",result_iref);
  fprintf(file_log,"");
  fprintf(file_log,"ETLR0,ETLR1,ETLR2,ETLR3,ETLR4,ETLR5,ETLR6,ETLR7,ETLR8,ETLR9,");
  fprintf(file_log,"ETLR10,ETLR11,ETLR12,ETLR13,ETLR14,ETLR15,ETLR16,ETLR17,ETLR18,ETLR19,");
  fprintf(file_log,"ETLR20,ETLR21,ETLR22,ETLR23,ETLR24,ETLR25,ETLR26,ETLR27,ETLR28,ETLR29,");
  fprintf(file_log,"ETLR30,ETLR31,ETLR32,ETLR33,ETLR34,ETLR35,ETLR36,ETLR37,ETLR38,ETLR39");
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),GetValue(IN_ETLR04,param), GetValue(IN_ETLR05,param),GetValue(IN_ETLR06,param),GetValue(IN_ETLR07,param),GetValue(IN_ETLR08,param),GetValue(IN_ETLR09,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR10,param),GetValue(IN_ETLR11,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR13,param),GetValue(IN_ETLR14,param), GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),GetValue(IN_ETLR17,param),GetValue(IN_ETLR18,param),GetValue(IN_ETLR19,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR20,param),GetValue(IN_ETLR21,param),GetValue(IN_ETLR22,param),GetValue(IN_ETLR23,param),GetValue(IN_ETLR24,param), GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", GetValue(IN_ETLR30,param),GetValue(IN_ETLR31,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param), GetValue(IN_ETLR35,param),GetValue(IN_ETLR36,param),GetValue(IN_ETLR37,param),GetValue(IN_ETLR38,param),GetValue(IN_ETLR39,param));

    // *****************
  fclose( file_log );

// ************************************************************************************
// **   Result                                                                       **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    if ( result_loss == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("**************************");
    } else if ( result_loss == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_loss);
      printf("vth_w    = %d",vth_loss);
      printf("");
  }

  if ((area == 0) || (area == 2)) {
    if ( result_gain == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("**************************");
    } else if ( result_gain == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("==========================");
    } else if ( 0 < result_gain && result_gain < 21) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_gain);
      printf("vth_w    = %d",vth_gain);
      printf("");
  }

  if ( cond2==0 ) {
      printf("");
    if (( result_iref>(7500-300) ) && ( result_iref<(7500+300) )) {
      printf("**************************");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("**************************");
    } else {
      printf("==========================");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("==========================");
    }
      printf("Iref = %d[nA] (7200~7800uA)",result_iref);
      printf("");
  }

// ************************************************************************************
// **   Register ReWrite                                                             **
// ************************************************************************************
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); DigitInput(&cond1);

  if ( cond1 == 0 ) {
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    SetValue(IN_BDATA01,param,0);
    
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
  }

    // Common Clear
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
//  SetValue(IN_ETLR25,param,etlr_25);
    // *******************

}

int Vth_Trim_Auto(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25;
  int error_flag;
  int info[32];
  // wmat chip information
  int trimvalue[8];

  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = 0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;


// ************************************************************************************
// ************************************************************************************
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);
      etlr_25 = GetValue(IN_ETLR25,param);

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
//      if ( PASS == OpensShorts() ) {
        DeviceSpecificPowerUp();

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  // SetValue(IN_REG,param,CHIPDATA); SetValue(IN_TESTSEL,param,0x00); if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) { SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param)); SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param)); SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param)); SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param)); SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param)); SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param)); SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param)); SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param)); SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param)); SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param)); SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param)); SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param)); SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param)); SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param)); printf("************************************"); printf("***   INIT PASS                  ***"); printf("************************************"); printf(""); } else { printf("------------------------------------"); printf("---   INIT FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }  //      } else {  //        printf("------------------------------------");  //        printf("---   PIN FAIL                  ---");  //        printf("------------------------------------");  //        printf("");  //	return(PASS);  //      }      printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 / FLP0&FLI1:3 / FLP0&FLI1&FLI2:4] ->");  //DigitInput(&mat); mat = 0; printf("*** Test Data-Area [Loss&Gain:0 / Loss:1 / Gain:2] ->");  //DigitInput(&area); area = 0;  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->");  //DigitInput(&cond0); cond0 = 1; /* printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond0); if ( cond0 == 1 ) SetValue(IN_ETLR25,param,0xFD); // 2.5uA // if ( cond0 == 2 ) SetValue(IN_ETLR25,param,0xFF); // 3.5uA //
  SetValue(IN_ETLR25,param,0xFA);
  //Iref = 2.0uA

  printf("*** Vth_w Select   [0.8V:0 / 1.0V:1 / 1.2V:2 / 1.4V:3 ] ->");
    //DigitInput(&cond0);
  cond0 = 1;
  if ( cond0 == 0 ) SetValue(IN_LDATA0,param, 800);
  if ( cond0 == 1 ) SetValue(IN_LDATA0,param,1000);
  if ( cond0 == 2 ) SetValue(IN_LDATA0,param,1200);
  if ( cond0 == 3 ) SetValue(IN_LDATA0,param,1400);

  printf("*** Vth_e Select   [-0.9V:0 / -0.7V:1 / -0.5V:2 / -0.3V:3 ] ->");
    //DigitInput(&cond0);
  cond0 = 1;
  if ( cond0 == 0 ) SetValue(IN_LDATA1,param,-900);
  if ( cond0 == 1 ) SetValue(IN_LDATA1,param,-700);
  if ( cond0 == 2 ) SetValue(IN_LDATA1,param,-500);
  if ( cond0 == 3 ) SetValue(IN_LDATA1,param,-300);
    //SetValue(IN_LDATA1,param,-700);     // Vth_e

  printf("*** Iref-Trim      [Yes:0 / No:1 ] ->");
    //DigitInput(&cond2);
  cond2 = 0;

  printf("*** Vhh-Temp       [ON:0 (W_-2.45mV&E_-3.62mV (@7.5V)) / OFF:1 ] ->");
    //DigitInput(&cond3);
  cond3 = 0;

  if ( cond3 == 0 ) {
    SetValue(IN_ETLR13,param,0xC8);
    // Vhh_e
    SetValue(IN_ETLR14,param,0x98);
    // Vhh_w
    SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param) & 0x3F) | 0xC0);
    // Temp_ON
  }
  if ( cond3 == 1 ) {
    SetValue(IN_ETLR15,param,0x3F);
    // Temp_OFF
  }
	
  printf("*** Delay fixed Trimming   [WS1:0 / WS2:1 / No-Set:2] ->");
    //DigitInput(&cond4);
	cond4 = 2;

  if ( cond4 == 0 ){
    // SetValue(IN_ETLR26,param,0xCC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR27,param,0xFC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR28,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR29,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR30,param,0xF1); // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR32,param,0x31);
    // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
    SetValue(IN_ETLR33,param,0x11);
    // T4 saout DelayTime (min+1Tap) //
    SetValue(IN_ETLR34,param,0x11);
    // T6 T4-T5 DelayTime (min+1Tap) //
	}
	if ( cond4 == 1 ){
    // SetValue(IN_ETLR26,param,0xCC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR27,param,0xFC); // T1 Precharge DelayTime (min+4Tap) // SetValue(IN_ETLR28,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR29,param,0xF1); // T2 Discharge DelayTime (min+1Tap) // SetValue(IN_ETLR30,param,0xF1); // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR32,param,0x31);
    // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
    SetValue(IN_ETLR33,param,0x11);
    // T4 saout DelayTime (min+1Tap) //
    SetValue(IN_ETLR34,param,0x11);
    // T6 T4-T5 DelayTime (min+1Tap) //
    printf("*** WS2 DelayTrim Condition is evaluating now. ");
    printf("*** WS2  = WS1 DelayTrim setting!!! ");
	}
    printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)",GetValue(IN_ETLR26,param));
    printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)",GetValue(IN_ETLR27,param));
    printf("*** etlr28<7:0>=0x%X (Discharge FLP0)",GetValue(IN_ETLR28,param));
    printf("*** etlr29<7:0>=0x%X (Discharge FLI1)",GetValue(IN_ETLR29,param));
    printf("*** etlr30<7:0>=0x%X (Discharge FLI2)",GetValue(IN_ETLR30,param));
    printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)",GetValue(IN_ETLR32,param));
    printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)",GetValue(IN_ETLR33,param));
    printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)",GetValue(IN_ETLR34,param));

// ************************************************************************************
// **   Add Register Chenge Kairo Tokunin                                            **
// ************************************************************************************
// SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param) & 0xF8) | 0x06); // Vrsg => 3.4V SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param) & 0xC7) | 0x30); // Vnoemi_R => 3.4V SetValue(IN_ETLR20,param,(GetValue(IN_ETLR20,param) & 0xF0) | 0x09); // Vpp_w => -3.0V SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param) & 0xF0) | 0x0C); // Vnoemi_PE => 3.6V SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param) & 0x0F) | 0x00); // Vwi => 2.0V SetValue(IN_ETLR21,param,(GetValue(IN_ETLR21,param) & 0xF0) | 0x09); // Vdemg =>-1.0V
// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);
      // Extra5
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
// CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL"); info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B); info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B); info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B); info[10]= ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B); info[11]= ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12]= ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);  SetValue(IN_AREA,param,4); // Extra4 // SetValue(IN_TOPADDR0,param,0); SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE); CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL"); info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L); if ( info[20] >= 10000 ) info[20] = info[20] - 10000; // 2nd-Cut //  printf(""); printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]); printf("*** W# : %3d",info[10]); printf("***  X : %3d",info[11]); printf("***  Y : %3d",info[12]); printf("***  S : %3d",info[20]);
//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_VthTrim",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%s_W%02d_X%d_Y%d_VthTrim",lotname,atoi(wafername),xy_location[0],xy_location[1]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

// ************************************************************************************
// **   Iref-Trim                                                                    **
// ************************************************************************************
  if (cond2 == 0) {
    SetValue(IN_REG,param,0x03);
    SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);

      SetValue(IN_BDATA15,param,UA25);
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
    result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    result_iref = selvol;
  }
//  SetValue(IN_ETLR22,param,0xFB);//Pumpup(8MHz->16MHz)
//  printf("ETLR22;%02X",GetValue(IN_ETLR22,param));

// ************************************************************************************
// **   Loss Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    //fprintf(file_log,"<<Loss>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple Erase ********************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

        // Program ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result <= (GetValue(IN_LDATA0,param) -800)) judg =999;
    else if (result <= (GetValue(IN_LDATA0,param) -760)) judg = 20;
    else if (result <= (GetValue(IN_LDATA0,param) -720)) judg = 19;
    else if (result <= (GetValue(IN_LDATA0,param) -680)) judg = 18;
    else if (result <= (GetValue(IN_LDATA0,param) -640)) judg = 17;
    else if (result <= (GetValue(IN_LDATA0,param) -600)) judg = 16;
    else if (result <= (GetValue(IN_LDATA0,param) -560)) judg = 15;
    else if (result <= (GetValue(IN_LDATA0,param) -520)) judg = 14;
    else if (result <= (GetValue(IN_LDATA0,param) -480)) judg = 13;
    else if (result <= (GetValue(IN_LDATA0,param) -440)) judg = 12;
    else if (result <= (GetValue(IN_LDATA0,param) -400)) judg = 11;
    else if (result <= (GetValue(IN_LDATA0,param) -360)) judg = 10;
    else if (result <= (GetValue(IN_LDATA0,param) -320)) judg =  9;
    else if (result <= (GetValue(IN_LDATA0,param) -280)) judg =  8;
    else if (result <= (GetValue(IN_LDATA0,param) -240)) judg =  7;
    else if (result <= (GetValue(IN_LDATA0,param) -200)) judg =  6;
    else if (result <= (GetValue(IN_LDATA0,param) -160)) judg =  5;
    else if (result <= (GetValue(IN_LDATA0,param) -120)) judg =  4;
    else if (result <= (GetValue(IN_LDATA0,param) - 80)) judg =  3;
    else if (result <= (GetValue(IN_LDATA0,param) - 40)) judg =  2;
    else if (result <= (GetValue(IN_LDATA0,param) -  0)) judg =  1;
    else if (result >= (GetValue(IN_LDATA0,param) +800)) judg =999;
    else if (result >= (GetValue(IN_LDATA0,param) +760)) judg =-19;
    else if (result >= (GetValue(IN_LDATA0,param) +720)) judg =-18;
    else if (result >= (GetValue(IN_LDATA0,param) +680)) judg =-17;
    else if (result >= (GetValue(IN_LDATA0,param) +640)) judg =-16;
    else if (result >= (GetValue(IN_LDATA0,param) +600)) judg =-15;
    else if (result >= (GetValue(IN_LDATA0,param) +560)) judg =-14;
    else if (result >= (GetValue(IN_LDATA0,param) +520)) judg =-13;
    else if (result >= (GetValue(IN_LDATA0,param) +480)) judg =-12;
    else if (result >= (GetValue(IN_LDATA0,param) +440)) judg =-11;
    else if (result >= (GetValue(IN_LDATA0,param) +400)) judg =-10;
    else if (result >= (GetValue(IN_LDATA0,param) +360)) judg = -9;
    else if (result >= (GetValue(IN_LDATA0,param) +320)) judg = -8;
    else if (result >= (GetValue(IN_LDATA0,param) +280)) judg = -7;
    else if (result >= (GetValue(IN_LDATA0,param) +240)) judg = -6;
    else if (result >= (GetValue(IN_LDATA0,param) +200)) judg = -5;
    else if (result >= (GetValue(IN_LDATA0,param) +160)) judg = -4;
    else if (result >= (GetValue(IN_LDATA0,param) +120)) judg = -3;
    else if (result >= (GetValue(IN_LDATA0,param) + 80)) judg = -2;
    else if (result >= (GetValue(IN_LDATA0,param) + 60)) judg = -1;

    fprintf(file_log,"No%d,%d,%d,ETLR17=0x%X,",i,result,judg,GetValue(IN_ETLR17,param));
    result_loss = judg;
    loop_loss   = i;
    vth_loss    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass

    SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + judg) | 0xC0);
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Gain Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 2)) {
    //fprintf(file_log,"<<Gain>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple writing *******************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    SetValue(IN_ETCR08,param,0x00);

        // Erase ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result >= (GetValue(IN_LDATA1,param) +760)) judg =999;
    else if (result >= (GetValue(IN_LDATA1,param) +720)) judg = 20;
    else if (result >= (GetValue(IN_LDATA1,param) +680)) judg = 19;
    else if (result >= (GetValue(IN_LDATA1,param) +640)) judg = 18;
    else if (result >= (GetValue(IN_LDATA1,param) +600)) judg = 17;
    else if (result >= (GetValue(IN_LDATA1,param) +560)) judg = 16;
    else if (result >= (GetValue(IN_LDATA1,param) +520)) judg = 15;
    else if (result >= (GetValue(IN_LDATA1,param) +480)) judg = 14;
    else if (result >= (GetValue(IN_LDATA1,param) +440)) judg = 13;
    else if (result >= (GetValue(IN_LDATA1,param) +400)) judg = 12;
    else if (result >= (GetValue(IN_LDATA1,param) +360)) judg = 11;
    else if (result >= (GetValue(IN_LDATA1,param) +320)) judg = 10;
    else if (result >= (GetValue(IN_LDATA1,param) +280)) judg =  9;
    else if (result >= (GetValue(IN_LDATA1,param) +240)) judg =  8;
    else if (result >= (GetValue(IN_LDATA1,param) +200)) judg =  7;
    else if (result >= (GetValue(IN_LDATA1,param) +160)) judg =  6;
    else if (result >= (GetValue(IN_LDATA1,param) +120)) judg =  5;
    else if (result >= (GetValue(IN_LDATA1,param) + 80)) judg =  4;
    else if (result >= (GetValue(IN_LDATA1,param) + 60)) judg =  3;
    else if (result >= (GetValue(IN_LDATA1,param) + 40)) judg =  2;
    else if (result >= (GetValue(IN_LDATA1,param) + 20)) judg =  1;
    else if (result >= (GetValue(IN_LDATA1,param) +  0)) judg =  1;
    else if (result <= (GetValue(IN_LDATA1,param) -840)) judg =999;
    else if (result <= (GetValue(IN_LDATA1,param) -800)) judg =-20;
    else if (result <= (GetValue(IN_LDATA1,param) -760)) judg =-19;
    else if (result <= (GetValue(IN_LDATA1,param) -720)) judg =-18;
    else if (result <= (GetValue(IN_LDATA1,param) -680)) judg =-17;
    else if (result <= (GetValue(IN_LDATA1,param) -640)) judg =-16;
    else if (result <= (GetValue(IN_LDATA1,param) -600)) judg =-15;
    else if (result <= (GetValue(IN_LDATA1,param) -560)) judg =-14;
    else if (result <= (GetValue(IN_LDATA1,param) -520)) judg =-13;
    else if (result <= (GetValue(IN_LDATA1,param) -480)) judg =-12;
    else if (result <= (GetValue(IN_LDATA1,param) -440)) judg =-11;
    else if (result <= (GetValue(IN_LDATA1,param) -400)) judg =-10;
    else if (result <= (GetValue(IN_LDATA1,param) -360)) judg = -9;
    else if (result <= (GetValue(IN_LDATA1,param) -320)) judg = -8;
    else if (result <= (GetValue(IN_LDATA1,param) -280)) judg = -7;
    else if (result <= (GetValue(IN_LDATA1,param) -240)) judg = -6;
    else if (result <= (GetValue(IN_LDATA1,param) -200)) judg = -5;
    else if (result <= (GetValue(IN_LDATA1,param) -160)) judg = -4;
    else if (result <= (GetValue(IN_LDATA1,param) -120)) judg = -3;
    else if (result <= (GetValue(IN_LDATA1,param) - 80)) judg = -2;

    fprintf(file_log,"No%d,%d,%d,ETLR16=0x%X,",i,result,judg,GetValue(IN_ETLR16,param));
    result_gain = judg;
    loop_gain   = i;
    vth_gain    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass
   	if      ( error_flag ==1 ) break;
   	// Vhh_e Tap  Limit

    SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + judg) | 0xC0);

    if(GetValue(IN_ETLR16,param) > 0xFF){
      printf("!! Tap Limit !!");
      SetValue(IN_ETLR16,param,0xFF | 0xC0);
     	error_flag = 1;
    }
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Log Out                                                                      **
// ************************************************************************************
  fprintf(file_log,"");
  SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
  result_vppe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
  result_vhhe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw = selvol;
  fprintf(file_log,"Vhh_w/Vpp_w = ,%d,%d",result_vhhw,result_vppw);
  fprintf(file_log,"Vhh_e/Vpp_e = ,%d,%d",result_vhhe,result_vppe);
  fprintf(file_log,"Iref = ,%d[nA]",result_iref);
  fprintf(file_log,"");
  fprintf(file_log,"ETLR0,ETLR1,ETLR2,ETLR3,ETLR4,ETLR5,ETLR6,ETLR7,ETLR8,ETLR9,");
  fprintf(file_log,"ETLR10,ETLR11,ETLR12,ETLR13,ETLR14,ETLR15,ETLR16,ETLR17,ETLR18,ETLR19,");
  fprintf(file_log,"ETLR20,ETLR21,ETLR22,ETLR23,ETLR24,ETLR25,ETLR26,ETLR27,ETLR28,ETLR29,");
  fprintf(file_log,"ETLR30,ETLR31,ETLR32,ETLR33,ETLR34,ETLR35,ETLR36,ETLR37,ETLR38,ETLR39");
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),GetValue(IN_ETLR04,param), GetValue(IN_ETLR05,param),GetValue(IN_ETLR06,param),GetValue(IN_ETLR07,param),GetValue(IN_ETLR08,param),GetValue(IN_ETLR09,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR10,param),GetValue(IN_ETLR11,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR13,param),GetValue(IN_ETLR14,param), GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),GetValue(IN_ETLR17,param),GetValue(IN_ETLR18,param),GetValue(IN_ETLR19,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR20,param),GetValue(IN_ETLR21,param),GetValue(IN_ETLR22,param),GetValue(IN_ETLR23,param),GetValue(IN_ETLR24,param), GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", GetValue(IN_ETLR30,param),GetValue(IN_ETLR31,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param), GetValue(IN_ETLR35,param),GetValue(IN_ETLR36,param),GetValue(IN_ETLR37,param),GetValue(IN_ETLR38,param),GetValue(IN_ETLR39,param));

    // *****************
  fclose( file_log );

// ************************************************************************************
// **   Result                                                                       **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    if ( result_loss == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("**************************");
    } else if ( result_loss == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_loss);
      printf("vth_w    = %d",vth_loss);
      printf("");
  }

  if ((area == 0) || (area == 2)) {
    if ( result_gain == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("**************************");
    } else if ( result_gain == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("==========================");
    } else if ( 0 < result_gain && result_gain < 21) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_gain);
      printf("vth_w    = %d",vth_gain);
      printf("");
  }

  if ( cond2==0 ) {
      printf("");
    if (( result_iref>(7500-300) ) && ( result_iref<(7500+300) )) {
      printf("**************************");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("**************************");
    } else {
      printf("==========================");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("==========================");
    }
      printf("Iref = %d[nA] (7200~7800uA)",result_iref);
      printf("");
  }

// ************************************************************************************
// **   Register ReWrite                                                             **
// ************************************************************************************
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); DigitInput(&cond1);

  if ( cond1 == 0 ) {
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    SetValue(IN_BDATA01,param,0);
    
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
  }

    // Common Clear
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
//  SetValue(IN_ETLR25,param,etlr_25);
    // *******************

}

// ************************************************************************************
// **   Vth_Trim_Eval                                                             **
// **   Vth_Trim_Eval                                                             **
// **   Vth_Trim_Eval                                                             **
// ************************************************************************************

int Vth_Trim_Eval(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond6;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25,selsizew;
  int error_flag;
  int info[32];
  // wmat chip information
  int trimvalue[8];

  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = 0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;

// ************************************************************************************
// ************************************************************************************
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);
      etlr_25 = GetValue(IN_ETLR25,param);

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
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
          printf("************************************");
          printf("***   INIT PASS                  ***");
          printf("************************************");
          printf("");
        } else {
          printf("------------------------------------");
          printf("---   INIT FAIL                  ---");
          printf("------------------------------------");
          printf("");
	  return(PASS);
        }
//      } else { 
//        printf("------------------------------------");
//        printf("---   PIN FAIL                  ---");
//        printf("------------------------------------");
//        printf("");
//	return(PASS);
//      } 

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************

    //printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 / FLP0&FLI1:3 / FLP0&FLI1&FLI2:4] ->"); DigitInput(&mat);
  mat = 0;
  //IoFLI1/2͖
  printf("*** Test Data-Area [Loss&Gain:0 / Loss:1 / Gain:2] ->");
  //DigitInput(&area);
  area=2;

  // printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond0); if ( cond0 == 1 ) SetValue(IN_ETLR25,param,0xFD); // 2.5uA // if ( cond0 == 2 ) SetValue(IN_ETLR25,param,0xFF); // 3.5uA //
  SetValue(IN_ETLR25,param,0xFA);
  //Iref = 2.0uA

  printf("*** Vth_w Select   [0.8V:0 / 1.0V:1 / 1.2V:2 / 1.4V:3 ] ->");
  //DigitInput(&cond0);
  cond0=1;
  if ( cond0 == 0 ) SetValue(IN_LDATA0,param, 800);
  if ( cond0 == 1 ) SetValue(IN_LDATA0,param,1000);
  if ( cond0 == 2 ) SetValue(IN_LDATA0,param,1200);
  if ( cond0 == 3 ) SetValue(IN_LDATA0,param,1400);

  printf("*** Test Address setting!!    [64KB-0(0x00000-0x0FFFF):0] ");
  printf("*** Test Address setting!!    [64KB-1(0x20000-0x2FFFF):1] ");
  printf("*** Test Address setting!!    [64KB-2(0x40000-0x4FFFF):2] ");
  printf("*** Test Address setting!!    [64KB-3(0x60000-0x6FFFF):3] ->"); DigitInput(&cond6);
  switch(cond6) {
	  case  0: selsizew = 0x00000; break;
	  case  1: selsizew = 0x20000; break;
	  case  2: selsizew = 0x40000; break;
	  case  3: selsizew = 0x60000; break;
          default: selsizew = 0x00000; break;
  }

  printf("*** Vth_e Select   [-0.9V:0 / -0.7V:1 / -0.5V:2 / -0.3V:3 ] ");
  printf("*** Vth_e Select   [-0.4V:4 / -0.5V:5 / -0.6V:6 / -0.8V:8 / -1.0V:10 / -1.2V:12 ] ->");
  DigitInput(&cond0);
  if ( cond0 == 0 ) SetValue(IN_LDATA1,param,-900);
  if ( cond0 == 1 ) SetValue(IN_LDATA1,param,-700);
  if ( cond0 == 2 ) SetValue(IN_LDATA1,param,-500);
  if ( cond0 == 3 ) SetValue(IN_LDATA1,param,-300);
  if ( cond0 == 4 ) SetValue(IN_LDATA1,param,-400);
  if ( cond0 == 5 ) SetValue(IN_LDATA1,param,-500);
  if ( cond0 == 6 ) SetValue(IN_LDATA1,param,-600);
  if ( cond0 == 8 ) SetValue(IN_LDATA1,param,-800);
  if ( cond0 == 10 ) SetValue(IN_LDATA1,param,-1000);
  if ( cond0 == 12 ) SetValue(IN_LDATA1,param,-1200);
    //SetValue(IN_LDATA1,param,-700);     // Vth_e

  printf("*** Iref-Trim      [Yes:0 / No:1 ] ->");
  //DigitInput(&cond2);
  cond2=0;

  printf("*** Vhh-Temp       [ON:0 (W_-2.45mV&E_-3.62mV (@7.5V)) / OFF:1 ] ->");
  //DigitInput(&cond3);
  cond3=0;
  if ( cond3 == 0 ) {
    SetValue(IN_ETLR13,param,0xC8);
    // Vhh_e
    SetValue(IN_ETLR14,param,0x98);
    // Vhh_w
    SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param) & 0x3F) | 0xC0);
    // Temp_ON
  }
  if ( cond3 == 1 ) {
    SetValue(IN_ETLR15,param,0x3F);
    // Temp_OFF
  }
	
  printf("*** Delay fixed Trimming   [WS1:0 / WS2:1 ] ->");
  //DigitInput(&cond4);
  cond4=1;
	if ( cond4 == 0 ){
    SetValue(IN_ETLR26,param,0xCC);
    // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR27,param,0xFC);
    // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR28,param,0xF1);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR29,param,0xF1);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR30,param,0xF1);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR32,param,0x31);
    // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
    SetValue(IN_ETLR33,param,0x11);
    // T4 saout DelayTime (min+1Tap) //
    SetValue(IN_ETLR34,param,0x11);
    // T6 T4-T5 DelayTime (min+1Tap) //
	}
	if ( cond4 == 1 ){
///// 8nsDelay 10nsDelay Const regset ///
    SetValue(IN_ETLR26,param,0x33);
    // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR27,param,0xF3);
    // T1 Precharge DelayTime (min+4Tap) //
    SetValue(IN_ETLR28,param,0xFB);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR29,param,0xFB);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR30,param,0xFB);
    // T2 Discharge DelayTime (min+1Tap) //
    SetValue(IN_ETLR32,param,0x3E);
    // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
    SetValue(IN_ETLR33,param,0x11);
    // T4 saout DelayTime (min+1Tap) //
    SetValue(IN_ETLR34,param,0x11);
    // T6 T4-T5 DelayTime (min+1Tap) //
    printf("*** WS2 DelayTrim Condition is evaluating now. ");
    printf("*** WS2  = WS1 DelayTrim setting!!! ");
	}
    printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)",GetValue(IN_ETLR26,param));
    printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)",GetValue(IN_ETLR27,param));
    printf("*** etlr28<7:0>=0x%X (Discharge FLP0)",GetValue(IN_ETLR28,param));
    printf("*** etlr29<7:0>=0x%X (Discharge FLI1)",GetValue(IN_ETLR29,param));
    printf("*** etlr30<7:0>=0x%X (Discharge FLI2)",GetValue(IN_ETLR30,param));
    printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)",GetValue(IN_ETLR32,param));
    printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)",GetValue(IN_ETLR33,param));
    printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)",GetValue(IN_ETLR34,param));

// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);
      // Extra5
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

      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);
      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_VthTrim",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

// ************************************************************************************
// **   Iref-Trim                                                                    **
// ************************************************************************************
  if (cond2 == 0) {
    SetValue(IN_REG,param,0x03);
    SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);

      SetValue(IN_BDATA15,param,UA25);
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
    result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    result_iref = selvol;
  }
  SetValue(IN_ETLR22,param,0xFB);
  //Pumpup(8MHz->16MHz)
  printf("ETLR22;%02X",GetValue(IN_ETLR22,param));

// ************************************************************************************
// **   Loss Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    //fprintf(file_log,"<<Loss>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple Erase ********************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE/8);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE/8);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE/8);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

//    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
//    SetValue(IN_ETCR08,param,0x00);

        // Program ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE/8);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE/8);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE/8);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result <= (GetValue(IN_LDATA0,param) -800)) judg =999;
    else if (result <= (GetValue(IN_LDATA0,param) -760)) judg = 20;
    else if (result <= (GetValue(IN_LDATA0,param) -720)) judg = 19;
    else if (result <= (GetValue(IN_LDATA0,param) -680)) judg = 18;
    else if (result <= (GetValue(IN_LDATA0,param) -640)) judg = 17;
    else if (result <= (GetValue(IN_LDATA0,param) -600)) judg = 16;
    else if (result <= (GetValue(IN_LDATA0,param) -560)) judg = 15;
    else if (result <= (GetValue(IN_LDATA0,param) -520)) judg = 14;
    else if (result <= (GetValue(IN_LDATA0,param) -480)) judg = 13;
    else if (result <= (GetValue(IN_LDATA0,param) -440)) judg = 12;
    else if (result <= (GetValue(IN_LDATA0,param) -400)) judg = 11;
    else if (result <= (GetValue(IN_LDATA0,param) -360)) judg = 10;
    else if (result <= (GetValue(IN_LDATA0,param) -320)) judg =  9;
    else if (result <= (GetValue(IN_LDATA0,param) -280)) judg =  8;
    else if (result <= (GetValue(IN_LDATA0,param) -240)) judg =  7;
    else if (result <= (GetValue(IN_LDATA0,param) -200)) judg =  6;
    else if (result <= (GetValue(IN_LDATA0,param) -160)) judg =  5;
    else if (result <= (GetValue(IN_LDATA0,param) -120)) judg =  4;
    else if (result <= (GetValue(IN_LDATA0,param) - 80)) judg =  3;
    else if (result <= (GetValue(IN_LDATA0,param) - 40)) judg =  2;
    else if (result <= (GetValue(IN_LDATA0,param) -  0)) judg =  1;
    else if (result >= (GetValue(IN_LDATA0,param) +800)) judg =999;
    else if (result >= (GetValue(IN_LDATA0,param) +760)) judg =-19;
    else if (result >= (GetValue(IN_LDATA0,param) +720)) judg =-18;
    else if (result >= (GetValue(IN_LDATA0,param) +680)) judg =-17;
    else if (result >= (GetValue(IN_LDATA0,param) +640)) judg =-16;
    else if (result >= (GetValue(IN_LDATA0,param) +600)) judg =-15;
    else if (result >= (GetValue(IN_LDATA0,param) +560)) judg =-14;
    else if (result >= (GetValue(IN_LDATA0,param) +520)) judg =-13;
    else if (result >= (GetValue(IN_LDATA0,param) +480)) judg =-12;
    else if (result >= (GetValue(IN_LDATA0,param) +440)) judg =-11;
    else if (result >= (GetValue(IN_LDATA0,param) +400)) judg =-10;
    else if (result >= (GetValue(IN_LDATA0,param) +360)) judg = -9;
    else if (result >= (GetValue(IN_LDATA0,param) +320)) judg = -8;
    else if (result >= (GetValue(IN_LDATA0,param) +280)) judg = -7;
    else if (result >= (GetValue(IN_LDATA0,param) +240)) judg = -6;
    else if (result >= (GetValue(IN_LDATA0,param) +200)) judg = -5;
    else if (result >= (GetValue(IN_LDATA0,param) +160)) judg = -4;
    else if (result >= (GetValue(IN_LDATA0,param) +120)) judg = -3;
    else if (result >= (GetValue(IN_LDATA0,param) + 80)) judg = -2;
    else if (result >= (GetValue(IN_LDATA0,param) + 60)) judg = -1;

    fprintf(file_log,"No%d,%d,%d,ETLR17=0x%X,",i,result,judg,GetValue(IN_ETLR17,param));
    result_loss = judg;
    loop_loss   = i;
    vth_loss    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass

    SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + judg) | 0xC0);
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Gain Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 2)) {
    //fprintf(file_log,"<<Gain>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple writing *******************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE/8);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE/8);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE/8);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

//    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
//    SetValue(IN_ETCR08,param,0x00);

        // Erase ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE/8);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE/8);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE/8);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result >= (GetValue(IN_LDATA1,param) +760)) judg =999;
    else if (result >= (GetValue(IN_LDATA1,param) +720)) judg = 20;
    else if (result >= (GetValue(IN_LDATA1,param) +680)) judg = 19;
    else if (result >= (GetValue(IN_LDATA1,param) +640)) judg = 18;
    else if (result >= (GetValue(IN_LDATA1,param) +600)) judg = 17;
    else if (result >= (GetValue(IN_LDATA1,param) +560)) judg = 16;
    else if (result >= (GetValue(IN_LDATA1,param) +520)) judg = 15;
    else if (result >= (GetValue(IN_LDATA1,param) +480)) judg = 14;
    else if (result >= (GetValue(IN_LDATA1,param) +440)) judg = 13;
    else if (result >= (GetValue(IN_LDATA1,param) +400)) judg = 12;
    else if (result >= (GetValue(IN_LDATA1,param) +360)) judg = 11;
    else if (result >= (GetValue(IN_LDATA1,param) +320)) judg = 10;
    else if (result >= (GetValue(IN_LDATA1,param) +280)) judg =  9;
    else if (result >= (GetValue(IN_LDATA1,param) +240)) judg =  8;
    else if (result >= (GetValue(IN_LDATA1,param) +200)) judg =  7;
    else if (result >= (GetValue(IN_LDATA1,param) +160)) judg =  6;
    else if (result >= (GetValue(IN_LDATA1,param) +120)) judg =  5;
    else if (result >= (GetValue(IN_LDATA1,param) + 80)) judg =  4;
    else if (result >= (GetValue(IN_LDATA1,param) + 60)) judg =  3;
    else if (result >= (GetValue(IN_LDATA1,param) + 40)) judg =  2;
    else if (result >= (GetValue(IN_LDATA1,param) + 20)) judg =  1;
    else if (result >= (GetValue(IN_LDATA1,param) +  0)) judg =  1;
//    else if (result <= (GetValue(IN_LDATA1,param) -1320)) judg =999;
//    else if (result <= (GetValue(IN_LDATA1,param) -1280)) judg =-32;
//    else if (result <= (GetValue(IN_LDATA1,param) -1240)) judg =-31;
//    else if (result <= (GetValue(IN_LDATA1,param) -1200)) judg =-30;
//    else if (result <= (GetValue(IN_LDATA1,param) -1160)) judg =-29;
//    else if (result <= (GetValue(IN_LDATA1,param) -1120)) judg =-28;
//    else if (result <= (GetValue(IN_LDATA1,param) -1080)) judg =-27;
//    else if (result <= (GetValue(IN_LDATA1,param) -1040)) judg =-26;
//    else if (result <= (GetValue(IN_LDATA1,param) -1000)) judg =-25;
//    else if (result <= (GetValue(IN_LDATA1,param) -960)) judg =-24;
//    else if (result <= (GetValue(IN_LDATA1,param) -920)) judg =-23;
//    else if (result <= (GetValue(IN_LDATA1,param) -880)) judg =-22;
//    else if (result <= (GetValue(IN_LDATA1,param) -840)) judg =-21;
//    else if (result <= (GetValue(IN_LDATA1,param) -800)) judg =-20;
//    else if (result <= (GetValue(IN_LDATA1,param) -760)) judg =-19;
//    else if (result <= (GetValue(IN_LDATA1,param) -720)) judg =-18;
//    else if (result <= (GetValue(IN_LDATA1,param) -680)) judg =-17;
//    else if (result <= (GetValue(IN_LDATA1,param) -640)) judg =-16;
//    else if (result <= (GetValue(IN_LDATA1,param) -600)) judg =-15;
//    else if (result <= (GetValue(IN_LDATA1,param) -560)) judg =-14;
//    else if (result <= (GetValue(IN_LDATA1,param) -520)) judg =-13;
//    else if (result <= (GetValue(IN_LDATA1,param) -480)) judg =-12;
//    else if (result <= (GetValue(IN_LDATA1,param) -440)) judg =-11;
//    else if (result <= (GetValue(IN_LDATA1,param) -400)) judg =-10;
//    else if (result <= (GetValue(IN_LDATA1,param) -360)) judg = -9;
//    else if (result <= (GetValue(IN_LDATA1,param) -320)) judg = -8;
//    else if (result <= (GetValue(IN_LDATA1,param) -280)) judg = -7;
//    else if (result <= (GetValue(IN_LDATA1,param) -240)) judg = -6;
//    else if (result <= (GetValue(IN_LDATA1,param) -200)) judg = -5;
//    else if (result <= (GetValue(IN_LDATA1,param) -160)) judg = -4;
//    else if (result <= (GetValue(IN_LDATA1,param) -120)) judg = -3;
//    else if (result <= (GetValue(IN_LDATA1,param) - 80)) judg = -2;


    else if (result <= (GetValue(IN_LDATA1,param) -840)) judg =999;
    else if (result <= (GetValue(IN_LDATA1,param) -800)) judg =-32;
    else if (result <= (GetValue(IN_LDATA1,param) -780)) judg =-31;
    else if (result <= (GetValue(IN_LDATA1,param) -760)) judg =-30;
    else if (result <= (GetValue(IN_LDATA1,param) -740)) judg =-29;
    else if (result <= (GetValue(IN_LDATA1,param) -720)) judg =-28;
    else if (result <= (GetValue(IN_LDATA1,param) -700)) judg =-27;
    else if (result <= (GetValue(IN_LDATA1,param) -680)) judg =-26;
    else if (result <= (GetValue(IN_LDATA1,param) -660)) judg =-25;
    else if (result <= (GetValue(IN_LDATA1,param) -640)) judg =-24;
    else if (result <= (GetValue(IN_LDATA1,param) -620)) judg =-23;
    else if (result <= (GetValue(IN_LDATA1,param) -600)) judg =-22;
    else if (result <= (GetValue(IN_LDATA1,param) -580)) judg =-21;
    else if (result <= (GetValue(IN_LDATA1,param) -560)) judg =-20;
    else if (result <= (GetValue(IN_LDATA1,param) -540)) judg =-19;
    else if (result <= (GetValue(IN_LDATA1,param) -520)) judg =-18;
    else if (result <= (GetValue(IN_LDATA1,param) -500)) judg =-17;
    else if (result <= (GetValue(IN_LDATA1,param) -480)) judg =-16;
    else if (result <= (GetValue(IN_LDATA1,param) -460)) judg =-15;
    else if (result <= (GetValue(IN_LDATA1,param) -440)) judg =-14;
    else if (result <= (GetValue(IN_LDATA1,param) -420)) judg =-13;
    else if (result <= (GetValue(IN_LDATA1,param) -400)) judg =-12;
    else if (result <= (GetValue(IN_LDATA1,param) -380)) judg =-11;
    else if (result <= (GetValue(IN_LDATA1,param) -360)) judg =-10;
    else if (result <= (GetValue(IN_LDATA1,param) -340)) judg = -9;
    else if (result <= (GetValue(IN_LDATA1,param) -320)) judg = -8;
    else if (result <= (GetValue(IN_LDATA1,param) -280)) judg = -7;
    else if (result <= (GetValue(IN_LDATA1,param) -240)) judg = -6;
    else if (result <= (GetValue(IN_LDATA1,param) -200)) judg = -5;
    else if (result <= (GetValue(IN_LDATA1,param) -160)) judg = -4;
    else if (result <= (GetValue(IN_LDATA1,param) -120)) judg = -3;
    else if (result <= (GetValue(IN_LDATA1,param) - 80)) judg = -2;

    fprintf(file_log,"No%d,%d,%d,ETLR16=0x%X,",i,result,judg,GetValue(IN_ETLR16,param));
    result_gain = judg;
    loop_gain   = i;
    vth_gain    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass
   	if      ( error_flag ==1 ) break;
   	// Vhh_e Tap  Limit

    SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + judg) | 0xC0);

    if(GetValue(IN_ETLR16,param) > 0xFF){
      printf("!! Tap Limit !!");
      SetValue(IN_ETLR16,param,0xFF | 0xC0);
     	error_flag = 1;
    }
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Log Out                                                                      **
// ************************************************************************************
  fprintf(file_log,"");
  SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
  result_vppe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
  result_vhhe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw = selvol;
  fprintf(file_log,"Vhh_w/Vpp_w = ,%d,%d",result_vhhw,result_vppw);
  fprintf(file_log,"Vhh_e/Vpp_e = ,%d,%d",result_vhhe,result_vppe);
  fprintf(file_log,"Iref = ,%d[nA]",result_iref);
  fprintf(file_log,"");
  fprintf(file_log,"ETLR0,ETLR1,ETLR2,ETLR3,ETLR4,ETLR5,ETLR6,ETLR7,ETLR8,ETLR9,");
  fprintf(file_log,"ETLR10,ETLR11,ETLR12,ETLR13,ETLR14,ETLR15,ETLR16,ETLR17,ETLR18,ETLR19,");
  fprintf(file_log,"ETLR20,ETLR21,ETLR22,ETLR23,ETLR24,ETLR25,ETLR26,ETLR27,ETLR28,ETLR29,");
  fprintf(file_log,"ETLR30,ETLR31,ETLR32,ETLR33,ETLR34,ETLR35,ETLR36,ETLR37,ETLR38,ETLR39");
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),GetValue(IN_ETLR04,param), GetValue(IN_ETLR05,param),GetValue(IN_ETLR06,param),GetValue(IN_ETLR07,param),GetValue(IN_ETLR08,param),GetValue(IN_ETLR09,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR10,param),GetValue(IN_ETLR11,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR13,param),GetValue(IN_ETLR14,param), GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),GetValue(IN_ETLR17,param),GetValue(IN_ETLR18,param),GetValue(IN_ETLR19,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR20,param),GetValue(IN_ETLR21,param),GetValue(IN_ETLR22,param),GetValue(IN_ETLR23,param),GetValue(IN_ETLR24,param), GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", GetValue(IN_ETLR30,param),GetValue(IN_ETLR31,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param), GetValue(IN_ETLR35,param),GetValue(IN_ETLR36,param),GetValue(IN_ETLR37,param),GetValue(IN_ETLR38,param),GetValue(IN_ETLR39,param));

    // *****************
  fclose( file_log );

// ************************************************************************************
// **   Result                                                                       **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    if ( result_loss == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("**************************");
    } else if ( result_loss == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_loss);
      printf("vth_w    = %d",vth_loss);
      printf("");
  }

  if ((area == 0) || (area == 2)) {
    if ( result_gain == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("**************************");
    } else if ( result_gain == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("==========================");
    } else if ( 0 < result_gain && result_gain < 21) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_gain);
      printf("vth_w    = %d",vth_gain);
      printf("");
  }

  if ( cond2==0 ) {
      printf("");
    if (( result_iref>(7500-300) ) && ( result_iref<(7500+300) )) {
      printf("**************************");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("**************************");
    } else {
      printf("==========================");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("==========================");
    }
      printf("Iref = %d[nA] (7200~7800uA)",result_iref);
      printf("");
  }

// ************************************************************************************
// **   Register ReWrite                                                             **
// ************************************************************************************
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); DigitInput(&cond1);

  if ( cond1 == 0 ) {
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    SetValue(IN_BDATA01,param,0);
    
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
  }

    // Common Clear
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
//  SetValue(IN_ETLR25,param,etlr_25);
    // *******************

}



// ************************************************************************************
// **   Vth_Trim_deltaVth                                                             **
// **   Vth_Trim_deltaVth                                                             **
// **   Vth_Trim_deltaVth                                                             **
// ************************************************************************************

int Vth_Trim_deltaVth(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,cond6,deltacond0,deltacond1;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain,result1st;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25,selsizew;
  int error_flag;
  int info[32];
  // wmat chip information
  int trimvalue[8];

  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = 0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;

// ************************************************************************************
// ************************************************************************************
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);
      etlr_25 = GetValue(IN_ETLR25,param);

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
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
          printf("************************************");
          printf("***   INIT PASS                  ***");
          printf("************************************");
          printf("");
        } else {
          printf("------------------------------------");
          printf("---   INIT FAIL                  ---");
          printf("------------------------------------");
          printf("");
	  return(PASS);
        }
//      } else { 
//        printf("------------------------------------");
//        printf("---   PIN FAIL                  ---");
//        printf("------------------------------------");
//        printf("");
//	return(PASS);
//      } 

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 / FLP0&FLI1:3 / FLP0&FLI1&FLI2:4] ->"); DigitInput(&mat);
  printf("*** mat=%d ***",mat);
  printf("*** Test Data-Area [Loss&Gain:0 / Loss:1 / Gain:2] ->");
  //DigitInput(&area);
  area=2;
  //Gain
  printf("*** area=%d ***",area);

  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->");
  //DigitInput(&cond0);
  cond0=1;
  //2.5uA
  printf("*** cond0=%d ***",cond0);
  // printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond0); if ( cond0 == 1 ) SetValue(IN_ETLR25,param,0xFD); // 2.5uA // if ( cond0 == 2 ) SetValue(IN_ETLR25,param,0xFF); // 3.5uA //
  SetValue(IN_ETLR25,param,0xFA);
  //Iref = 2.0uA

  printf("*** Vth_w Select   [0.8V:0 / 1.0V:1 / 1.2V:2 / 1.4V:3 ] ->");
  //DigitInput(&cond0);
  cond0=1;
  //1.0V
  printf("*** cond0=%d ***",cond0);
  if ( cond0 == 0 ) SetValue(IN_LDATA0,param, 800);
  if ( cond0 == 1 ) SetValue(IN_LDATA0,param,1000);
  if ( cond0 == 2 ) SetValue(IN_LDATA0,param,1200);
  if ( cond0 == 3 ) SetValue(IN_LDATA0,param,1400);

// printf("*** Test Address setting!!    [64KB-0(0x00000-0x0FFFF):0] "); printf("*** Test Address setting!!    [64KB-1(0x20000-0x2FFFF):1] "); printf("*** Test Address setting!!    [64KB-2(0x40000-0x4FFFF):2] "); printf("*** Test Address setting!!    [64KB-3(0x60000-0x6FFFF):3] ->"); DigitInput(&cond6); switch(cond6) { case  0: selsizew = 0x00000; break; case  1: selsizew = 0x20000; break; case  2: selsizew = 0x40000; break; case  3: selsizew = 0x60000; break; default: selsizew = 0x00000; break; }

// printf("*** Vth_e Select   [-0.9V:0 / -0.7V:1 / -0.5V:2 / -0.3V:3 ] "); printf("*** Vth_e Select   [-0.4V:4 / -0.5V:5 / -0.6V:6 / -0.8V:8 / -1.0V:10 / -1.2V:12 ] ->"); DigitInput(&cond0); if ( cond0 == 0 ) SetValue(IN_LDATA1,param,-900); if ( cond0 == 1 ) SetValue(IN_LDATA1,param,-700); if ( cond0 == 2 ) SetValue(IN_LDATA1,param,-500); if ( cond0 == 3 ) SetValue(IN_LDATA1,param,-300); if ( cond0 == 4 ) SetValue(IN_LDATA1,param,-400); if ( cond0 == 5 ) SetValue(IN_LDATA1,param,-500); if ( cond0 == 6 ) SetValue(IN_LDATA1,param,-600); if ( cond0 == 8 ) SetValue(IN_LDATA1,param,-800); if ( cond0 == 10 ) SetValue(IN_LDATA1,param,-1000); if ( cond0 == 12 ) SetValue(IN_LDATA1,param,-1200);  //SetValue(IN_LDATA1,param,-700);  // Vth_e //
	
  printf("*** Iref-Trim      [Yes:0 / No:1 ] ->");
  //DigitInput(&cond2);
  cond2=1;
  //No
  printf("*** cond2=%d ***",cond2);

	// printf("*** Vhh-Temp       [ON:0 (W_-2.45mV&E_-3.62mV (@7.5V)) / OFF:1 ] ->"); //DigitInput(&cond3); cond3=1; // printf("*** cond3=%d ***",cond3); if ( cond3 == 0 ) { SetValue(IN_ETLR13,param,0xC8); // Vhh_e SetValue(IN_ETLR14,param,0x98); // Vhh_w SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param) & 0x3F) | 0xC0); // Temp_ON } if ( cond3 == 1 ) {  //    SetValue(IN_ETLR15,param,0x3F);   // Temp_OFF }
  printf("*** Delay fixed Trimming   [WS1:0 / WS2:1 ] ->");
  //DigitInput(&cond4);
  cond4=1;
  //WS2
  // printf("*** cond4=%d ***",cond4); if ( cond4 == 0 ){ SetValue(IN_ETLR32,param,0x3E); // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (max-1Tap) // SetValue(IN_ETLR33,param,0x11); // T4 saout DelayTime (min+1Tap) // SetValue(IN_ETLR34,param,0x11); // T6 T4-T5 DelayTime (min+1Tap) // } if ( cond4 == 1 ){ SetValue(IN_ETLR32,param,0x3E); // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (max-1Tap) // SetValue(IN_ETLR33,param,0x11); // T4 saout DelayTime (min+1Tap) // SetValue(IN_ETLR34,param,0x11); // T6 T4-T5 DelayTime (min+1Tap) // printf("*** WS2 DelayTrim Condition is evaluating now. "); printf("*** WS2  = WS1 DelayTrim setting!!! "); }
    printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)",GetValue(IN_ETLR26,param));
    printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)",GetValue(IN_ETLR27,param));
    printf("*** etlr28<7:0>=0x%X (Discharge FLP0)",GetValue(IN_ETLR28,param));
    printf("*** etlr29<7:0>=0x%X (Discharge FLI1)",GetValue(IN_ETLR29,param));
    printf("*** etlr30<7:0>=0x%X (Discharge FLI2)",GetValue(IN_ETLR30,param));
    printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)",GetValue(IN_ETLR32,param));
    printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)",GetValue(IN_ETLR33,param));
    printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)",GetValue(IN_ETLR34,param));

  printf("*** trtf Setting 100us -> 300us  [trtf=100us:0 / trtf=300us:1 ] ->");
  //DigitInput(&cond5);
  cond5=1;
  //trtf=300us
  printf("*** cond5=%d ***",cond5);
	if ( cond5 == 0 ){
    printf("*** No setting!! (Def:trtf=100us)");
	}
	if ( cond5 == 1 ){
    SetValue(IN_ETLR15,param,0xFB);
    // trtf=300us //
    printf("*** etlr15<7:0>=0x%X (trtf setting)",GetValue(IN_ETLR15,param));
	}

// ************************************************************************************
// **   Chip-Infomation Read Lot# W# XY S#                                           **
// ************************************************************************************
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);
      // Extra5
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

      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);
      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_VthTrim",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

// ************************************************************************************
// **   Iref-Trim                                                                    **
// ************************************************************************************
  if (cond2 == 0) {
    SetValue(IN_REG,param,0x03);
    tempo = SetValue(IN_ETLR25,param,0xFA);
    //Auto TrimmingIref2.0uA
    SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);

      SetValue(IN_BDATA15,param,UA25);
      SetValue(IN_ETLR25,param,tempo);
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
    result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    result_iref = selvol;
  }
  SetValue(IN_ETLR22,param,0xFB);
  //Pumpup(8MHz->16MHz) vppe_clk
  printf("ETLR22;%02X",GetValue(IN_ETLR22,param));

// ************************************************************************************
// **   Loss Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    //fprintf(file_log,"<<Loss>> ETLR25=0x%X,",GetValue(IN_ETLR25,param)); //0xFD=2.5uA, 0xFA=2.0uA SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple Erase ********************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
    //FLP0&FLI1&FLI2
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    //Multi Block select User -> On
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);
    //Multi Block select User -> Off

        // Program ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV",result);

        // Vth Judgment *********************************************
    judg = 0;
    if      (result <= (GetValue(IN_LDATA0,param) -800)) judg =999;
    else if (result <= (GetValue(IN_LDATA0,param) -760)) judg = 20;
    else if (result <= (GetValue(IN_LDATA0,param) -720)) judg = 19;
    else if (result <= (GetValue(IN_LDATA0,param) -680)) judg = 18;
    else if (result <= (GetValue(IN_LDATA0,param) -640)) judg = 17;
    else if (result <= (GetValue(IN_LDATA0,param) -600)) judg = 16;
    else if (result <= (GetValue(IN_LDATA0,param) -560)) judg = 15;
    else if (result <= (GetValue(IN_LDATA0,param) -520)) judg = 14;
    else if (result <= (GetValue(IN_LDATA0,param) -480)) judg = 13;
    else if (result <= (GetValue(IN_LDATA0,param) -440)) judg = 12;
    else if (result <= (GetValue(IN_LDATA0,param) -400)) judg = 11;
    else if (result <= (GetValue(IN_LDATA0,param) -360)) judg = 10;
    else if (result <= (GetValue(IN_LDATA0,param) -320)) judg =  9;
    else if (result <= (GetValue(IN_LDATA0,param) -280)) judg =  8;
    else if (result <= (GetValue(IN_LDATA0,param) -240)) judg =  7;
    else if (result <= (GetValue(IN_LDATA0,param) -200)) judg =  6;
    else if (result <= (GetValue(IN_LDATA0,param) -160)) judg =  5;
    else if (result <= (GetValue(IN_LDATA0,param) -120)) judg =  4;
    else if (result <= (GetValue(IN_LDATA0,param) - 80)) judg =  3;
    else if (result <= (GetValue(IN_LDATA0,param) - 40)) judg =  2;
    else if (result <= (GetValue(IN_LDATA0,param) -  0)) judg =  1;
    else if (result >= (GetValue(IN_LDATA0,param) +800)) judg =999;
    else if (result >= (GetValue(IN_LDATA0,param) +760)) judg =-19;
    else if (result >= (GetValue(IN_LDATA0,param) +720)) judg =-18;
    else if (result >= (GetValue(IN_LDATA0,param) +680)) judg =-17;
    else if (result >= (GetValue(IN_LDATA0,param) +640)) judg =-16;
    else if (result >= (GetValue(IN_LDATA0,param) +600)) judg =-15;
    else if (result >= (GetValue(IN_LDATA0,param) +560)) judg =-14;
    else if (result >= (GetValue(IN_LDATA0,param) +520)) judg =-13;
    else if (result >= (GetValue(IN_LDATA0,param) +480)) judg =-12;
    else if (result >= (GetValue(IN_LDATA0,param) +440)) judg =-11;
    else if (result >= (GetValue(IN_LDATA0,param) +400)) judg =-10;
    else if (result >= (GetValue(IN_LDATA0,param) +360)) judg = -9;
    else if (result >= (GetValue(IN_LDATA0,param) +320)) judg = -8;
    else if (result >= (GetValue(IN_LDATA0,param) +280)) judg = -7;
    else if (result >= (GetValue(IN_LDATA0,param) +240)) judg = -6;
    else if (result >= (GetValue(IN_LDATA0,param) +200)) judg = -5;
    else if (result >= (GetValue(IN_LDATA0,param) +160)) judg = -4;
    else if (result >= (GetValue(IN_LDATA0,param) +120)) judg = -3;
    else if (result >= (GetValue(IN_LDATA0,param) + 80)) judg = -2;
    else if (result >= (GetValue(IN_LDATA0,param) + 60)) judg = -1;

    fprintf(file_log,"No%d,%d,%d,ETLR17=0x%X,",i,result,judg,GetValue(IN_ETLR17,param));
    result_loss = judg;
    loop_loss   = i;
    vth_loss    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass

    SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + judg) | 0xC0);
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Gain Vth-Trim                                                                **
// ************************************************************************************
  if ((area == 0) || (area == 2)) {
    //fprintf(file_log,"<<Gain>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

        // Simple writing *******************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
    //FLP0&FLI1&FLI2
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    //Multi Block select User -> On
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    SetValue(IN_ETCR08,param,0x00);
    //Multi Block select User -> Off

        // Erase ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    } else if ( mat==3 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    } else if ( mat==4 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    }

    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

        // Vth-Search ***********************************************
    vs[0] = 3300;
    result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
    //20mV step
    printf(" ----result---- %dmV",result);
  	if(i==0) {
  		result1st = result;
    printf(" ====result 1st = %dmV",result1st);
    printf("*** WT Vth_e=-0.7V Target!! ");
    printf("*** Vth_e deltaVth Select   [deltaVth +0.0V:0 / +0.1V:1 / +0.2V:2 / +0.3V:3 ] ");
    printf("*** Vth_e deltaVth Select   [deltaVth +0.4V:4 / +0.5V:5 / +0.6V:6 / +0.7V:7 ] ->");
    DigitInput(&deltacond0);
      switch(deltacond0) {
	    case  0: deltacond1 = 0; break;
	    case  1: deltacond1 = 100; break;
	    case  2: deltacond1 = 200; break;
	    case  3: deltacond1 = 300; break;
	    case  4: deltacond1 = 400; break;
	    case  5: deltacond1 = 500; break;
	    case  6: deltacond1 = 600; break;
	    case  7: deltacond1 = 700; break;
            default: deltacond1 = 0; break;
      }
      //switch
  	}
  	//if(i==0)

        // Vth Judgment *********************************************
    judg = 0;
    if      ((result-result1st-deltacond1) >= (  2000)) judg =999;
    else if ((result-result1st-deltacond1) <= ( -1155)) judg =-33;
    else if ((result-result1st-deltacond1) <= ( -1120)) judg =-32;
    else if ((result-result1st-deltacond1) <= ( -1085)) judg =-31;
    else if ((result-result1st-deltacond1) <= ( -1050)) judg =-30;
    else if ((result-result1st-deltacond1) <= ( -1015)) judg =-29;
    else if ((result-result1st-deltacond1) <= ( -980)) judg =-28;
    else if ((result-result1st-deltacond1) <= ( -945)) judg =-27;
    else if ((result-result1st-deltacond1) <= ( -910)) judg =-26;
    else if ((result-result1st-deltacond1) <= ( -875)) judg =-25;
    else if ((result-result1st-deltacond1) <= ( -840)) judg =-24;
    else if ((result-result1st-deltacond1) <= ( -805)) judg =-23;
    else if ((result-result1st-deltacond1) <= ( -770)) judg =-22;
    else if ((result-result1st-deltacond1) <= ( -735)) judg =-21;
    else if ((result-result1st-deltacond1) <= ( -700)) judg =-20;
    else if ((result-result1st-deltacond1) <= ( -665)) judg =-19;
    else if ((result-result1st-deltacond1) <= ( -630)) judg =-18;
    else if ((result-result1st-deltacond1) <= ( -595)) judg =-17;
    else if ((result-result1st-deltacond1) <= ( -560)) judg =-16;
    else if ((result-result1st-deltacond1) <= ( -525)) judg =-15;
    else if ((result-result1st-deltacond1) <= ( -490)) judg =-14;
    else if ((result-result1st-deltacond1) <= ( -455)) judg =-13;
    else if ((result-result1st-deltacond1) <= ( -420)) judg =-12;
    else if ((result-result1st-deltacond1) <= ( -385)) judg =-11;
    else if ((result-result1st-deltacond1) <= ( -350)) judg =-10;
    else if ((result-result1st-deltacond1) <= ( -315)) judg = -9;
    else if ((result-result1st-deltacond1) <= ( -280)) judg = -8;
    else if ((result-result1st-deltacond1) <= ( -245)) judg = -7;
    else if ((result-result1st-deltacond1) <= ( -210)) judg = -6;
    else if ((result-result1st-deltacond1) <= ( -175)) judg = -5;
    else if ((result-result1st-deltacond1) <= ( -140)) judg = -4;
    else if ((result-result1st-deltacond1) <= ( -105)) judg = -3;
    else if ((result-result1st-deltacond1) <= ( - 70)) judg = -2;
    else if ((result-result1st-deltacond1) <= ( - 35)) judg = -1;
    //vhhe50mVstep/1tap 50mVx0.7=vth35mV/1tap -50mV<Target<0
    else if ((result-result1st-deltacond1) >= (    0)) judg =  0;
    else if ((result-result1st-deltacond1) >= (   35)) judg =  1;
    else if ((result-result1st-deltacond1) >= (   70)) judg =  2;
    else if ((result-result1st-deltacond1) >= (  105)) judg =  3;
    else if ((result-result1st-deltacond1) >= (  140)) judg =  4;
    else if ((result-result1st-deltacond1) >= (  175)) judg =  5;
    else if ((result-result1st-deltacond1) >= (  210)) judg =  6;
    else if ((result-result1st-deltacond1) >= (  245)) judg =  7;
    else if ((result-result1st-deltacond1) >= (  280)) judg =  8;
    else if ((result-result1st-deltacond1) >= (  315)) judg =  9;
    else if ((result-result1st-deltacond1) >= (  350)) judg = 10;
  	tempo7=result-result1st;
  	printf(" ==== Calc (result-result1st) = %dmV",tempo7);
  	printf(" ==== Setting deltacond1      = %dmV",deltacond1);

    fprintf(file_log,"No%d,%d,%d,ETLR16=0x%X,",i,result,judg,GetValue(IN_ETLR16,param));
    result_gain = judg;
    loop_gain   = i;
    vth_gain    = result;

    if      ( judg == 999 ) break;
    // Fail
    else if ( judg ==   0 ) break;
    // Pass
   	if      ( error_flag ==1 ) break;
   	// Vhh_e Tap  Limit

    SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + judg) | 0xC0);

    if(GetValue(IN_ETLR16,param) > 0xFF){
      printf("!! Tap Limit !!");
      SetValue(IN_ETLR16,param,0xFF | 0xC0);
     	error_flag = 1;
    }
  }
  fprintf(file_log,"");
  }

// ************************************************************************************
// **   Log Out                                                                      **
// ************************************************************************************
  fprintf(file_log,"");
  SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
  result_vppe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
  result_vppw = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
  result_vhhe = selvol;
  SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
  SetValue(IN_BDATA00,param,0);
  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
  result_vhhw = selvol;
  fprintf(file_log,"Vhh_w/Vpp_w = ,%d,%d",result_vhhw,result_vppw);
  fprintf(file_log,"Vhh_e/Vpp_e = ,%d,%d",result_vhhe,result_vppe);
  fprintf(file_log,"Iref = ,%d[nA]",result_iref);
  fprintf(file_log,"");
  fprintf(file_log,"ETLR0,ETLR1,ETLR2,ETLR3,ETLR4,ETLR5,ETLR6,ETLR7,ETLR8,ETLR9,");
  fprintf(file_log,"ETLR10,ETLR11,ETLR12,ETLR13,ETLR14,ETLR15,ETLR16,ETLR17,ETLR18,ETLR19,");
  fprintf(file_log,"ETLR20,ETLR21,ETLR22,ETLR23,ETLR24,ETLR25,ETLR26,ETLR27,ETLR28,ETLR29,");
  fprintf(file_log,"ETLR30,ETLR31,ETLR32,ETLR33,ETLR34,ETLR35,ETLR36,ETLR37,ETLR38,ETLR39");
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR00,param),GetValue(IN_ETLR01,param),GetValue(IN_ETLR02,param),GetValue(IN_ETLR03,param),GetValue(IN_ETLR04,param), GetValue(IN_ETLR05,param),GetValue(IN_ETLR06,param),GetValue(IN_ETLR07,param),GetValue(IN_ETLR08,param),GetValue(IN_ETLR09,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR10,param),GetValue(IN_ETLR11,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR13,param),GetValue(IN_ETLR14,param), GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),GetValue(IN_ETLR17,param),GetValue(IN_ETLR18,param),GetValue(IN_ETLR19,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,", GetValue(IN_ETLR20,param),GetValue(IN_ETLR21,param),GetValue(IN_ETLR22,param),GetValue(IN_ETLR23,param),GetValue(IN_ETLR24,param), GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param));
  fprintf(file_log,"0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", GetValue(IN_ETLR30,param),GetValue(IN_ETLR31,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param), GetValue(IN_ETLR35,param),GetValue(IN_ETLR36,param),GetValue(IN_ETLR37,param),GetValue(IN_ETLR38,param),GetValue(IN_ETLR39,param));

    // *****************
  fclose( file_log );

// ************************************************************************************
// **   Result                                                                       **
// ************************************************************************************
  if ((area == 0) || (area == 1)) {
    if ( result_loss == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("*** <<Loss>> Pass      ***");
      printf("**************************");
    } else if ( result_loss == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("=== <<Loss>> Fail      ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("### <<Loss>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_loss);
      printf("vth_w    = %d",vth_loss);
      printf("");
  }

  if ((area == 0) || (area == 2)) {
    if ( result_gain == 0 ) {
      printf("");
      printf("**************************");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("*** <<Gain>> Pass      ***");
      printf("**************************");
    } else if ( result_gain == 999 ) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("=== <<Gain>> Fail      ===");
      printf("==========================");
    } else if ( 0 < result_gain && result_gain < 21) {
      printf("");
      printf("==========================");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("=== <<Gain>> Over Tap  ===");
      printf("==========================");
    } else {
      printf("");
      printf("##########################");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("### <<Gain>> Loop Over ###");
      printf("##########################");
    }
      printf("loop_num = %d",loop_gain);
      printf("vth_w    = %d",vth_gain);
      printf("");
  }

  if ( cond2==0 ) {
      printf("");
    if (( result_iref>(7500-300) ) && ( result_iref<(7500+300) )) {
      printf("**************************");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("*** <<Iref>> Pass      ***");
      printf("**************************");
    } else {
      printf("==========================");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("=== <<Iref>> Fail      ===");
      printf("==========================");
    }
      printf("Iref = %d[nA] (7200~7800uA)",result_iref);
      printf("");
  }

// ************************************************************************************
// **   Register ReWrite                                                             **
// ************************************************************************************
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); DigitInput(&cond1);

  if ( cond1 == 0 ) {
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    SetValue(IN_BDATA01,param,0);
    
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
  }

    // Common Clear
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_REG,param,stack8);
//  SetValue(IN_ETLR25,param,etlr_25);
    // *******************

}
