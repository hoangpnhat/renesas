/******************************************************************************/
/*!
  @file RC04EXslc_WriteErase.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int WriteErase_Tokusei(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10;
  int result,selvol,pulse;
  int i,j,k,l,m,n,cond0,cond1,cond2,cond3,cond4,mat,loop;
  int judg,target_vhh,pulse_reg,set_vcc;
  int etlr_09,etlr_15,etlr_16,etlr_17,etlr_25,etlr_36,etlr_37;
  int vth_mode,bit_count,min_vol,add_size;
  int info[32];                    /* wmat chip information              */
  int atreg[3]; //After Trimming Reg
  int result1[5][5],result2[5][5],result3[5][5]; //middle suso search result[target_vhh][pulse_reg]
  char file_name[128];             /*                                    */
  char file_name2[128];            /*                                    */
  char sample_name[128];           /*                                    */
  char test_temp[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = 0;
  

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
      stack9 = vs[0];
      etlr_09 = GetValue(IN_ETLR09,param);
      etlr_15 = GetValue(IN_ETLR15,param);
      etlr_16 = GetValue(IN_ETLR16,param);
      etlr_17 = GetValue(IN_ETLR17,param);
      etlr_25 = GetValue(IN_ETLR25,param);
      etlr_36 = GetValue(IN_ETLR36,param);
      etlr_37 = GetValue(IN_ETLR37,param);

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      //if ( PASS == OpensShorts() ) {
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
      /*        
      } else { 
        printf("------------------------------------\n");
        printf("---   PIN FAIL                  ---\n");
        printf("------------------------------------\n");
        printf("\n");
	return(PASS);
      }*/ 

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("+----+-----------------+\n");
  printf("| No |                 |\n");
  printf("+----+-----------------+\n");
  printf("|  0 | Write&Erase     |\n");
  printf("|  1 | Write           |\n");
  printf("|  2 | Erase           |\n");
  printf("|  3 | Erase_Houwa     |\n");
  printf("+----+-----------------+\n");
  printf("|  4 | Mulch_Write     |\n");
  printf("|  5 | Mulch_Erase     |\n");
  printf("| 45 | Mulch_W&E       |\n");
  printf("+----+-----------------+\n");
  printf("|  6 | Separate_Write  |\n");
  printf("|  7 | Separate_Erase  |\n");
  printf("| 67 | Separate_W&E    |\n");
  printf("+----+-----------------+\n");
  printf(" Select No (e:EXIT) -> ");	DigitInput(&cond0);

  if ( cond0 == EXIT ) return(PASS);

  if ( cond0 < 3 ) {
  printf("*** Vcc  Select [1.8V_Only:1 / Loop(1.5~3.7V):5] ->"); DigitInput(&cond3);
  }
  printf("*** Test MAT    [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);

  printf("*** Test Temp   [25C:25 / -40:40 / 95C:95 ] ->"); DigitInput(&cond2);
  if ( cond2 == 25 ) sprintf(test_temp,"_25C");
  if ( cond2 == 40 ) sprintf(test_temp,"_-40C");
  if ( cond2 == 95 ) sprintf(test_temp,"_95C");

//  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond1);
//  if ( cond1 == 1 ) SetValue(IN_ETLR25,param,0xFD);   /* 2.5uA */
//  if ( cond1 == 2 ) SetValue(IN_ETLR25,param,0xFF);   /* 3.5uA */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10] = ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11] = ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12] = ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      switch(cond0) {
        case 0:
        case 1:
        case 2:  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_WriteErase",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
                 break;
        case 3:  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Erase-Houwa_MAT%d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],mat);
                 break;
        case 4:
        case 5:
        case 45: sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Mulch",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
                 sprintf(file_name2,"%s%s", sample_name, test_temp);
                 break;
        case 6:
        case 7:
        case 67: sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Separate",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
                 sprintf(file_name2,"%s%s", sample_name, test_temp);
                 break;
        default: break;
      }

      sprintf(file_name,"%s%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, test_temp, get_site_number());

  time( &time_list );// time_list += V4V5K_TIME_OFS;

/**************************************************************************************/
/***   Write Tokusei                                                                ***/
/**************************************************************************************/
  if ((cond0 == 0) || (cond0 == 1)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  SetValue(IN_ETLR09,param, etlr_09 & 0x7F ); /* Iref  Temp ON  */

  /* Vhh_w Trimming *****************************************************/
  vs[0] = 3300;
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 6800;
	        SetValue(IN_ETLR17,param,0xCB); break;
        case 1: target_vhh = 7300;
	        SetValue(IN_ETLR17,param,0xD8); break;
        case 2: target_vhh = 7800;
	        SetValue(IN_ETLR17,param,0xE2); break;
        default: break;
        }
      atreg[target_vhh] = GetValue(IN_ETLR17,param); //Get VhhW(afterTrim)
    }
  
  for ( k=0 ; k<cond3 ; k++ ) {    /*** Vcc Loop ***/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  if ( cond3 == 1 ) set_vcc = 1800;
  else {
    switch (k) {
      case 0: set_vcc = 1500; break;
      case 1: set_vcc = 1800; break;
      case 2: set_vcc = 2600; break;
      case 3: set_vcc = 3300; break;
      case 4: set_vcc = 3700; break;
      default: break;
    }
  }
  fprintf(file_log,"<<Write>> Vcc=%d,",set_vcc);

    /* Vhh_w Trimming Data Set*****************************************************/
    for ( i=0 ; i<3 ; i++ ) {    /*** Voltage Loop ***/
      switch (i) {
        case 0: target_vhh = 6800; break;
        case 1: target_vhh = 7300; break;
        case 2: target_vhh = 7800; break;
        default: break;
      }
      SetValue(IN_ETLR17,param,atreg[target_vhh]);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    fprintf(file_log,"0,0,",selvol);

    SetValue(IN_ETCR11,param,0x04);//count stop

    SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,0);

    /* Erase Set ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_ETCR08,param,0x00);

    /* Write Test **************************************************/
    for ( j=0 ; j<4 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x4; break;  /*  1ms       */
        case 1: pulse_reg = 0x6; break;  /*  3ms(+2ms) */
        case 2: pulse_reg = 0x6; break;  /*  5ms(+2ms) */
        case 3: pulse_reg = 0x6; break;  /*  7ms(+2ms) */
        default: break;
      }

      SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param)&0xF0) | pulse_reg);

      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
      }

      vs[0] = set_vcc;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
      SetValue(IN_EXTAL1,param,stack10);
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
    }                         /* j (Puls_loop) */

    fprintf(file_log,",,");
    }                         /* i (Vhh_loop)  */

  fprintf(file_log,"\n"); 
  fclose( file_log );
  }                           /* k (Vcc_loop)  */

  SetValue(IN_ETLR09,param,etlr_09);
  SetValue(IN_ETLR15,param,etlr_15);
  SetValue(IN_ETLR17,param,etlr_17);
  SetValue(IN_ETLR36,param,etlr_36);
  }

/**************************************************************************************/
/***   Erase Tokusei                                                                ***/
/**************************************************************************************/
  if ((cond0 == 0) || (cond0 == 2)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  SetValue(IN_ETLR09,param, etlr_09 & 0x7F ); /* Iref  Temp ON  */

  /* syoki Erase *****************************************************/
  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
  }
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

  /* Vhh_e Trimming *****************************************************/
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 6500;
                SetValue(IN_ETLR16,param,0xD9); break;
        case 1: target_vhh = 7000;
                SetValue(IN_ETLR16,param,0xE5); break;
        case 2: target_vhh = 7500;
                SetValue(IN_ETLR16,param,0xEF); break;
        default: break;
        }
      atreg[m] = GetValue(IN_ETLR16,param); //Get VhhE(afterTrim)
    }

  for ( k=0 ; k<cond3 ; k++ ) {    /*** Vcc Loop ***/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  if ( cond3 == 1 ) set_vcc = 1800;
  else {
    switch (k) {
      case 0: set_vcc = 1500; break;
      case 1: set_vcc = 1800; break;
      case 2: set_vcc = 2600; break;
      case 3: set_vcc = 3300; break;
      case 4: set_vcc = 3700; break;
      default: break;
    }
  }
  fprintf(file_log,"<<Erase>> Vcc=%d,",set_vcc);

  /* Vhh_e Trimming Data Set*****************************************************/
    for ( i=0 ; i<3 ; i++ ) {    /*** Voltage Loop ***/
      switch (i) {
        case 0: target_vhh = 6500; break;
        case 1: target_vhh = 7000; break;
        case 2: target_vhh = 7500; break;
        default: break;
    }
    SetValue(IN_ETLR16,param,atreg[i]);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    fprintf(file_log,"0,0,",selvol);

    /* Write Set ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      SetValue(IN_ETCR08,param,0x00);

    /* Erase Test **************************************************/
    for ( j=0 ; j<5 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x3; break;  /*  1ms       */
        case 1: pulse_reg = 0x4; break;  /*  3ms(+2ms) */
        case 2: pulse_reg = 0x4; break;  /*  5ms(+2ms) */
        case 3: pulse_reg = 0x4; break;  /*  7ms(+2ms) */
        case 4: pulse_reg = 0x5; break;  /* 10ms(+3ms) */
        default: break;
      }
      SetValue(IN_ETLR37,param,(GetValue(IN_ETLR37,param)&0xF0) | pulse_reg);

      /* Erase *****************************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
      }

      vs[0] = set_vcc;
      if ( j==0 ) SetValue(IN_BDATA00,param,0);  /* Pre-Write ON  */
      else        SetValue(IN_BDATA00,param,1);  /* Pre-Write OFF */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_BDATA00,param,0);

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
      SetValue(IN_EXTAL1,param,stack10);
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
    }                         /* j (Puls_loop) */

  fprintf(file_log,",");
  }                           /* i (Vhh_loop) */

  fprintf(file_log,"\n"); 
  fclose( file_log );
  }                           /* k (Vcc_loop) */

  SetValue(IN_ETLR09,param,etlr_09);
  SetValue(IN_ETLR15,param,etlr_15);
  SetValue(IN_ETLR16,param,etlr_16);
  SetValue(IN_ETLR37,param,etlr_37);
  }

/**************************************************************************************/
/***   Erase-Houwa Tokusei                                                          ***/
/**************************************************************************************/
  if (cond0 == 3) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  //SetValue(IN_ETLR09,param, etlr_09 & 0x7F ); /* Iref  Temp ON */

 /* syoki Erase *****************************************************/
  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x100);
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x100);
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x100);
  }

  SetValue(IN_ETCR08,param,0x20);
  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
  SetValue(IN_ETCR08,param,0x00);

  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Erase-Houwa>> Vcc=%d,",set_vcc);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    fprintf(file_log,"0,0,",selvol);

      /* Write Set ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      SetValue(IN_ETCR08,param,0x00);

    /* Erase Test **************************************************/
    for ( j=0 ; j<7 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x3; loop = 0; break;  /*  1ms */
        case 1: pulse_reg = 0x4; loop = 0; break;  /*  3ms(+ 2ms)   */
        case 2: pulse_reg = 0x4; loop = 0; break;  /*  5ms(+ 2ms)   */
        case 3: pulse_reg = 0xF; loop = 0; break;  /* 10ms(+ 5ms)   */
        case 4: pulse_reg = 0xC; loop = 0; break;  /* 20ms(+10ms)   */
        case 5: pulse_reg = 0xC; loop = 3; break;  /* 50ms(+10ms*3) */
        case 6: pulse_reg = 0xC; loop = 5; break;  /*100ms(+10ms*5) */
        default: break;
      }
      SetValue(IN_ETLR37,param,(GetValue(IN_ETLR37,param)&0xF0) | pulse_reg);

      /* Erase *****************************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x80000);    /* 512KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x80000);    /* 512KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x80000);    /* 512KB */    
      }

      vs[0] = set_vcc;
      if( j==0 ) SetValue(IN_BDATA00,param,0);  /* Pre-Write ON  */
      else	 SetValue(IN_BDATA00,param,1);	/* Pre-Write OFF */
      for ( m = 0 ; m <= loop ; m++ ) {
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      }
      SetValue(IN_BDATA00,param,0);

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param); //1st suso search
      SetValue(IN_EXTAL1,param,stack10);
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
    }  /* j */
  fprintf(file_log,",");

  fprintf(file_log,"\n"); 
  fclose( file_log );

  SetValue(IN_ETLR09,param,etlr_09);
  SetValue(IN_ETLR37,param,etlr_37);
  }

/**************************************************************************************/
/***   Mulch Write Tokusei                                                          ***/
/**************************************************************************************/
  if ((cond0 == 4) | (cond0 == 45)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);



  /* Write Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Mulch-Write>> Vcc=%d,Distribution:2.5V~0.1VStep,,Mat=%d(0x0~0x7FFFF)",set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<10 ; i++ ) {


    /* shoki Erase *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: SetValue(IN_ETCR06,param,0x60);                               add_size=0x04000 ; min_vol = 300; fprintf(file_log,"MG64 ");  break;   /* MG64Line */
      case 1: SetValue(IN_ETCR08,param,0x00); SetValue(IN_BDATA02,param,0); add_size=0x80000 ; min_vol = 500; fprintf(file_log,"256B ");  break;   /* 256B     */
      case 2: SetValue(IN_ETCR08,param,0x01); SetValue(IN_BDATA02,param,1); add_size=0x80000 ; min_vol = 500; fprintf(file_log,"4kB  ");  break;   /* 4kB      */
      case 3: SetValue(IN_ETCR08,param,0x02); SetValue(IN_BDATA02,param,2); add_size=0x80000 ; min_vol = 300; fprintf(file_log,"8kB  ");  break;   /* 8kB      */
      case 4: SetValue(IN_ETCR08,param,0x03); SetValue(IN_BDATA02,param,3); add_size=0x80000 ; min_vol = 300; fprintf(file_log,"16kB ");  break;   /* 6kB      */
      case 5: SetValue(IN_ETCR08,param,0x04); SetValue(IN_BDATA02,param,4); add_size=0x80000 ; min_vol = 300; fprintf(file_log,"32kB ");  break;   /* 32kB     */
      case 6: SetValue(IN_ETCR08,param,0x05); SetValue(IN_BDATA02,param,5); add_size=0x80000 ; min_vol =   0; fprintf(file_log,"64kB ");  break;   /* 64kB     */
      case 7: SetValue(IN_ETCR08,param,0x06); SetValue(IN_BDATA02,param,6); add_size=0x80000 ; min_vol =   0; fprintf(file_log,"128kB");  break;   /* 128kB    */
      case 8: SetValue(IN_ETCR08,param,0x07); SetValue(IN_BDATA02,param,7); add_size=0x80000 ; min_vol =   0; fprintf(file_log,"256kB");  break;   /* 256kB    */
      case 9: SetValue(IN_ETCR08,param,0x20);                               add_size=0x00100 ; min_vol =   0; fprintf(file_log,"512kB");  break;   /* 512kB    */
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,add_size);
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,add_size);
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,add_size);    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
  SetValue(IN_ETCR06,param,0x00);
  SetValue(IN_ETCR08,param,0x00);
  SetValue(IN_BDATA02,param,0x00);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x80000);
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x80000);
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x80000);    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 23;                 // VthRead2&3(0~2.5V)
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   Mulch Erase Tokusei                                                          ***/
/**************************************************************************************/
  if ((cond0 == 5) | (cond0 == 45)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Mulch-Erase>> Vcc=%d,Distribution:0V~0.1VStep,Mat=%d(0x0~0x7FFFF)",set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<8 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: SetValue(IN_ETCR08,param,0x00); SetValue(IN_BDATA02,param,0); add_size=0x80000 ; fprintf(file_log,"4kB  ");  break;   /* 4kB      */
      case 1: SetValue(IN_ETCR08,param,0x02); SetValue(IN_BDATA02,param,2); add_size=0x80000 ; fprintf(file_log,"8kB  ");  break;   /* 8kB      */
      case 2: SetValue(IN_ETCR08,param,0x03); SetValue(IN_BDATA02,param,3); add_size=0x80000 ; fprintf(file_log,"16kB ");  break;   /* 6kB      */
      case 3: SetValue(IN_ETCR08,param,0x04); SetValue(IN_BDATA02,param,4); add_size=0x80000 ; fprintf(file_log,"32kB ");  break;   /* 32kB     */
      case 4: SetValue(IN_ETCR08,param,0x05); SetValue(IN_BDATA02,param,5); add_size=0x80000 ; fprintf(file_log,"64kB ");  break;   /* 64kB     */
      case 5: SetValue(IN_ETCR08,param,0x06); SetValue(IN_BDATA02,param,6); add_size=0x80000 ; fprintf(file_log,"128kB");  break;   /* 128kB    */
      case 6: SetValue(IN_ETCR08,param,0x07); SetValue(IN_BDATA02,param,7); add_size=0x80000 ; fprintf(file_log,"256kB");  break;   /* 256kB    */
      case 7: SetValue(IN_ETCR08,param,0x20);                               add_size=0x00100 ; fprintf(file_log,"512kB");  break;   /* 512kB    */
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,add_size);
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,add_size);
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,add_size);    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
  SetValue(IN_ETCR06,param,0x00);
  SetValue(IN_ETCR08,param,0x00);
  SetValue(IN_BDATA02,param,0x00);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x80000);
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x80000);
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x80000);    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 1;                 // VthRead2&3(0~2.5V)
  min_vol = -2000; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   Separate Write Tokusei                                                       ***/
/**************************************************************************************/
  if ((cond0 == 6) | (cond0 == 67)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Write>> Vcc=%d,Distribution:2.5V~0.1VStep,Mat=%d(0x0~0xFFF)",set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<5 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: loop=1 ; fprintf(file_log,"3ms_x1 ");   break;
      case 1: loop=3 ; fprintf(file_log,"1ms_x3 ");   break;
      case 2: loop=2 ; fprintf(file_log,"1ms+2ms");   break;
      case 3: loop=2 ; fprintf(file_log,"2ms+1ms");   break;
      case 4: loop=1 ; fprintf(file_log,"3ms_x1 ");   break;
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {

    if ( (i==0) || (i==4) ) {	 /* 3msx1 */
        SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0xF);
    }
    if ( i==1 ) {	 /* 1msx3 */
        SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
    }
    if ( i==2 ) {	 /* 1ms+2ms */
      if ( j==0 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
      if ( j==1 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x6);
    }
    if ( i==3 ) {	 /* 2ms+1ms */
      if ( j==0 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x6);
      if ( j==1 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
    }

    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
  }
  SetValue(IN_ETLR36,param,etlr_36);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 23;                 // VthRead2&3(0~2.5V)
  min_vol = 500; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   Separate Erase Tokusei                                                       ***/
/**************************************************************************************/
  if ((cond0 == 7) | (cond0 == 67)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Erase>> Vcc=%d,Distribution:0V~0.1VStep,Mat=%d(0x0~0xFFF)",set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<4 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: loop=1 ; fprintf(file_log,"5ms_x1 ");   break;
      case 1: loop=5 ; fprintf(file_log,"1ms_x5 ");   break;
      case 2: loop=2 ; fprintf(file_log,"1ms+4ms");   break;
      case 3: loop=2 ; fprintf(file_log,"4ms+1ms");   break;
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {

    if ( i==0 ) {	 /* 5msx1 */
        SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0xF);
    }
    if ( i==1 ) {	 /* 1msx5 */
        SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
    }
    if ( i==2 ) {	 /* 1ms+4ms */
      if ( j==0 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
      if ( j==1 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x6);
    }
    if ( i==3 ) {	 /* 4ms+1ms */
      if ( j==0 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x6);
      if ( j==1 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
    }

    if( j==0 ) SetValue(IN_BDATA00,param,0);
    else       SetValue(IN_BDATA00,param,1);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
  }
  SetValue(IN_BDATA00,param,0x00);
  SetValue(IN_ETLR37,param,etlr_37);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 1;                 // VthRead2&3(0~2.5V)
  min_vol = -2000; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   END                                                                          ***/
/**************************************************************************************/
  printf("\n"); 
  printf("**************************\n"); 
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("**************************\n"); 

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
  vs[0] = stack9;
  /*********************/

}  


int WriteErase_Tokusei_Kani(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10;
  int result,selvol,pulse;
  int i,j,k,l,m,n,cond0,cond1,cond2,cond3,cond4,mat,loop;
  int judg,target_vhh,pulse_reg,set_vcc;
  int etlr_16,etlr_17,etlr_25,etlr_36,etlr_37;
  int info[32];                    /* wmat chip information              */
  int atreg[3]; //After Trimming Reg
  int result1[5][5],result2[5][5],result3[5][5]; //middle suso search result[target_vhh][pulse_reg]
  char file_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_temp[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = 0;
  
  printf("Enter X-location->");DigitInput(&tempo0);
  printf("Enter Y-location->");DigitInput(&tempo1);
  switch(get_site_number()){
    case 0:
      xy_location[0] = tempo0;
      xy_location[1] = tempo1;
	break;
    case 1:
      xy_location[0] = tempo0+1;
      xy_location[1] = tempo1;
	break;
    case 2:
      xy_location[0] = tempo0;
      xy_location[1] = tempo1+1;
	break;
    case 3:
      xy_location[0] = tempo0+1;
      xy_location[1] = tempo1+1;
	break;
  }


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
      stack9 = vs[0];
      etlr_16 = GetValue(IN_ETLR16,param);
      etlr_17 = GetValue(IN_ETLR17,param);
      etlr_25 = GetValue(IN_ETLR25,param);
      etlr_36 = GetValue(IN_ETLR36,param);
      etlr_37 = GetValue(IN_ETLR37,param);

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      if ( PASS == OpensShorts() ) {
        DeviceSpecificPowerUp();
		/*
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
      } else { 
        printf("------------------------------------\n");
        printf("---   PIN FAIL                  ---\n");
        printf("------------------------------------\n");
        printf("\n");
	return(PASS);*/
      } 

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("*** Test Select [Write&Erase:0 / Write:1 / Erase:2 / Erase_Houwa:3] ->");
  //DigitInput(&cond0);
  cond0 = 3;  // Erase_Houwa
  printf("*** Vcc  Select [1.8V_Only:1 / Loop(1.6~3.6V):5] ->");
  //DigitInput(&cond3);
  cond3 = 1;  // Vcc Single
  printf("*** Test MAT    [FLP0:0 / FLI1:1 / FLI2:2 ] ->");
  //DigitInput(&mat);
  mat = 0;  // FLP0 Select

  printf("*** Test Temp   [25C:25 / -40:40 / 90C:90 ] ->");
  // DigitInput(&cond2);
  cond2 = 25; //25C only
  if ( cond2 == 25 ) sprintf(test_temp,"_25C");
  if ( cond2 == 40 ) sprintf(test_temp,"_-40C");
  if ( cond2 == 90 ) sprintf(test_temp,"_90C");

  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->");
  //DigitInput(&cond1);
  cond1 = 1; // Iref 2.5uA Only
  if ( cond1 == 1 ) SetValue(IN_ETLR25,param,0xFD);   /* 2.5uA */
  if ( cond1 == 2 ) SetValue(IN_ETLR25,param,0xFF);   /* 3.5uA */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      /*
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10] = ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11] = ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12] = ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      SetValue(IN_AREA,param,4);	 // Extra4 //
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);
      */

      //sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_WriteErase",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%s_W%02d_X%d_Y%d_WriteErase",lotname,atoi(wafername),xy_location[0],xy_location[1]);
  
      sprintf(file_name,"%s%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, test_temp, get_site_number());

/**************************************************************************************/
/***   Write Tokusei                                                                ***/
/**************************************************************************************/
  if ((cond0 == 0) || (cond0 == 1)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Vhh_w Trimming *****************************************************/
  vs[0] = 3300;
  if(cond2 != 40){ //Other -40C
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 7000; break;
        case 1: target_vhh = 7500; break;
        case 2: target_vhh = 8000; break;
        default: break;
        }
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_rev723base_Vhh_at"),"AUTOTRIM_VHH_W",&target_vhh);
      atreg[m] = GetValue(IN_ETLR17,param); //Get VhhW(afterTrim)
      /* debug */
      //printf("atreg[%d]=%X\n",target_vhh,atreg[target_vhh]);
      //WaitHitKey( "\nPlease Hit Any Key\n" );
      /* debug */
      }
    }
  else{ //-40C
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 7000;
                SetValue(IN_ETLR17,param,0xD0); break;
        case 1: target_vhh = 7500;
                SetValue(IN_ETLR17,param,0xDC); break;
        case 2: target_vhh = 8000;
                SetValue(IN_ETLR17,param,0xE7); break;
        default: break;
        }
      atreg[m] = GetValue(IN_ETLR17,param); //Get VhhW(afterTrim)
      /* debug */
      //printf("atreg[%d]=%X\n",target_vhh,atreg[target_vhh]);
      //WaitHitKey( "\nPlease Hit Any Key\n" );
      /* debug */
      }
    }
  
  for ( k=0 ; k<cond3 ; k++ ) {    /*** Vcc Loop ***/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  if ( cond3 == 1 ) set_vcc = 1800;
  else {
    switch (k) {
      case 0: set_vcc = 1600; break;
      case 1: set_vcc = 1800; break;
      case 2: set_vcc = 2600; break;
      case 3: set_vcc = 3300; break;
      case 4: set_vcc = 3600; break;
      default: break;
    }
  }
  fprintf(file_log,"<<Write>> Vcc=%d,",set_vcc);

/* Vhh_w Trimming Data Set*****************************************************/
  for ( i=0 ; i<3 ; i++ ) {    /*** Voltage Loop ***/
    switch (i) {
      case 0: target_vhh = 7000; break;
      case 1: target_vhh = 7500; break;
      case 2: target_vhh = 8000; break;
      default: break;
    }
  SetValue(IN_ETLR17,param,atreg[i]);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    fprintf(file_log,"0,0,",selvol);
    
    SetValue(IN_ETLR22,param,0xFB);//Pumpup(8MHz->16MHz)
    //SetValue(IN_ETCR02,param,0x80);//Vreg20=Vcc
    SetValue(IN_ETCR11,param,0x04);//count stop

    SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,0);

    /* Write Test **************************************************/
    for ( j=0 ; j<5 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x4; break;  /*  1ms */
        case 1: pulse_reg = 0xF; break;  /*  3ms */
        case 2: pulse_reg = 0xA; break;  /*  5ms */
        case 3: pulse_reg = 0xC; break;  /*  7ms */
        case 4: pulse_reg = 0xE; break;  /* 10ms */
        default: break;
      }
    if(pulse_reg != 0xE){
      SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param)&0xF0) | pulse_reg);

      /* Simple writing ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_ETCR08,param,0x00);

      /* Write *****************************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
      }

      vs[0] = set_vcc;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
      SetValue(IN_EXTAL1,param,stack10);
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
      }
      else{//Write pulse 10ms
        fprintf(file_log,",");//dummy space
        }
    }  /* j */
  fprintf(file_log,",");
  }    /* i */
  //TrimReg Output
  for(n=7000; n<=8000; n+=500){
    fprintf(file_log,"Target=%d,",n);
    fprintf(file_log,"ETLR17,0x%X,",atreg[n]);
    fprintf(file_log,",");
    }
  fprintf(file_log,"\n"); 
  fclose( file_log );
  }    /* k */

  SetValue(IN_ETLR17,param,etlr_17);
  SetValue(IN_ETLR36,param,etlr_36);
  }

/**************************************************************************************/
/***   Erase Tokusei                                                                ***/
/**************************************************************************************/
  if ((cond0 == 0) || (cond0 == 2)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

 /* syoki Erase *****************************************************/
  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
  }
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

  /* Vhh_e Trimming *****************************************************/
  if(cond2 != 40){ //Other -40C
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 6500; break;
        case 1: target_vhh = 6800; break;
        case 2: target_vhh = 7100; break;
        default: break;
        }
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_E);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_rev723base_Vhh_at"),"AUTOTRIM_VHH_E",&target_vhh);
      atreg[m] = GetValue(IN_ETLR16,param); //Get VhhE(afterTrim)
      }
    }
  else{ //-40C
    for(m=0; m<3; m++){
      switch(m){
        case 0: target_vhh = 6500;
                SetValue(IN_ETLR16,param,0xD9); break;
        case 1: target_vhh = 6800;
                SetValue(IN_ETLR16,param,0xE0); break;
        case 2: target_vhh = 7100;
                SetValue(IN_ETLR16,param,0xE7); break;
        default: break;
        }
      atreg[m] = GetValue(IN_ETLR16,param); //Get VhhE(afterTrim)
      }
    }

  for ( k=0 ; k<cond3 ; k++ ) {    /*** Vcc Loop ***/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  if ( cond3 == 1 ) set_vcc = 1800;
  else {
    switch (k) {
      case 0: set_vcc = 1600; break;
      case 1: set_vcc = 1800; break;
      case 2: set_vcc = 2600; break;
      case 3: set_vcc = 3300; break;
      case 4: set_vcc = 3600; break;
      default: break;
    }
  }
  fprintf(file_log,"<<Erase>> Vcc=%d,",set_vcc);

  /* Vhh_e Trimming Data Set*****************************************************/
  for ( i=0 ; i<3 ; i++ ) {    /*** Voltage Loop ***/
    switch (i) {
      case 0: target_vhh = 6500; break;
      case 1: target_vhh = 6800; break;
      case 2: target_vhh = 7100; break;
      default: break;
      }
    SetValue(IN_ETLR16,param,atreg[i]);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    fprintf(file_log,"0,0,",selvol);

    /* Erase Test **************************************************/
    for ( j=0 ; j<5 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x3; break;  /*  1ms */
        case 1: pulse_reg = 0x5; break;  /*  3ms */
        case 2: pulse_reg = 0xF; break;  /*  5ms */
        case 3: pulse_reg = 0xA; break;  /*  7ms */
        case 4: pulse_reg = 0xC; break;  /* 10ms */
        default: break;
      }
    if(pulse_reg != 0x3){
      SetValue(IN_ETLR37,param,(GetValue(IN_ETLR37,param)&0xF0) | pulse_reg);

      /* Simple writing ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      SetValue(IN_ETCR08,param,0x00);

      /* Erase *****************************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
      }

      vs[0] = set_vcc;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result1[i][j] = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param); //1st suso search
      result2[i][j] = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param); //2nd suso search
      result3[i][j] = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param); //3rd suso search
      SetValue(IN_EXTAL1,param,stack10);
      result = (result1[i][j]+result2[i][j]+result3[i][j])/3;  //average suso search
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
    }
    else{//Erase pulse 1ms
      fprintf(file_log,",");//dummy space
      }
    }  /* j */
  fprintf(file_log,",");
  }    /* i */
  //TrimReg, middle suso search Output
  for(n=6500; n<=7100; n+=300){
    fprintf(file_log,"Target=%d,",n);
    fprintf(file_log,"ETLR16 ,0x%X,",atreg[n]);
    for(l=0; l<4; l++){
      switch(l){
        case 0: pulse_reg = 0x5; pulse=3; break;  /*  3ms */
        case 1: pulse_reg = 0xF; pulse=5; break;  /*  5ms */
        case 2: pulse_reg = 0xA; pulse=7; break;  /*  7ms */
        case 3: pulse_reg = 0xC; pulse=10; break;  /* 10ms */
        default: break;
        }
      fprintf(file_log,"Erase pulse=%dms,",pulse); 
      fprintf(file_log,"Suso,%d,%d,%d,",result1[(n-6500)/300][l],result2[(n-6500)/300][l],result3[(n-6500)/300][l]);
      }
     fprintf(file_log,",");
    }
  fprintf(file_log,"\n"); 
  fclose( file_log );
  }    /* k */

  SetValue(IN_ETLR16,param,etlr_16);
  SetValue(IN_ETLR37,param,etlr_37);
  }

/**************************************************************************************/
/***   Erase-Houwa Tokusei                                                                ***/
/**************************************************************************************/
  if (cond0 == 3) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

 /* syoki Erase *****************************************************/
  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
  }
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Erase-Houwa>> Vcc=%d,",set_vcc);

    /* Voltage Monitor *****************************************************/
    vs[0] = set_vcc;
    SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
    SetValue(IN_BDATA00,param,0);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
    fprintf(file_log,"%d,",selvol);

    fprintf(file_log,"0,0,",selvol);

      /* Simple writing ********************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x100);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x100);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x100);    
      }

      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      vs[0] = 3300;
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      SetValue(IN_ETCR08,param,0x00);

    /* Erase Test **************************************************/
    for ( j=0 ; j<5 ; j++ ) {    /*** Pulse Loop ***/
      switch (j) {
        case 0: pulse_reg = 0x3; loop = 0; break;  /*  1ms */
        case 1: pulse_reg = 0x4; loop = 0; break;  /*  3ms(+ 2ms)   */
        case 2: pulse_reg = 0x4; loop = 0; break;  /*  5ms(+ 2ms)   */
        case 3: pulse_reg = 0xF; loop = 0; break;  /* 10ms(+ 5ms)   */
        case 4: pulse_reg = 0xC; loop = 0; break;  /* 20ms(+10ms)   */
        case 5: pulse_reg = 0xC; loop = 3; break;  /* 50ms(+10ms*3) */
        case 6: pulse_reg = 0xC; loop = 5; break;  /*100ms(+10ms*5) */
        default: break;
      }
//    if(pulse_reg != 0x3){
      SetValue(IN_ETLR37,param,(GetValue(IN_ETLR37,param)&0xF0) | pulse_reg);


      /* Erase *****************************************************/
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,0x1000);    /* 4KB */
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,0x1000);    /* 4KB */
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,0x1000);    /* 4KB */    
      }

      vs[0] = set_vcc;
      if( j==0 ) SetValue(IN_BDATA00,param,0);  /* Pre-Write ON  */
      else 		 SetValue(IN_BDATA00,param,1);	/* Pre-Write-OFF */
      for ( m = 0 ; m <= loop ; m++ ) {
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      }
	  SetValue(IN_BDATA00,param,0);

      /* Vth-Search & Log-Output ************************************************/
      vs[0] = 3300;
      stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param); //1st suso search
      SetValue(IN_EXTAL1,param,stack10);
      printf(" ----result---- %dmV\n",result);

      fprintf(file_log,"%d,",result);
//    }
//    else{//Erase pulse 1ms
//      fprintf(file_log,",");//dummy space
//      }
    }  /* j */
  fprintf(file_log,",");

  fprintf(file_log,"\n"); 
  fclose( file_log );

  SetValue(IN_ETLR16,param,etlr_16);
  SetValue(IN_ETLR37,param,etlr_37);
  }

/**************************************************************************************/
/***   END                                                                          ***/
/**************************************************************************************/
  printf("\n"); 
  printf("**************************\n"); 
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("**************************\n"); 

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
  vs[0] = stack9;
  /*********************/

}

int WriteErase_Tokusei_Separate(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param,char* Add_name,int temprature)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10;
  int result,selvol,pulse;
  int i,j,k,l,m,n,cond0,cond1,cond2,cond3,cond4,mat,loop;
  int judg,target_vhh,pulse_reg,set_vcc,pe_cond,pe_time_sum,pe_time,PreWrite_Skip;
  int etlr_09,etlr_15,etlr_16,etlr_17,etlr_25,etlr_36,etlr_37;
  int vth_mode,bit_count,min_vol,add_size;
  int info[32];                    /* wmat chip information              */
  int atreg[3]; //After Trimming Reg
  int result1[5][5],result2[5][5],result3[5][5]; //middle suso search result[target_vhh][pulse_reg]
  char file_name[128];             /*                                    */
  char file_name2[128];            /*                                    */
  char sample_name[128];           /*                                    */
  char test_temp[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = pe_cond = 0;
  

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
      stack9 = vs[0];
      etlr_09 = GetValue(IN_ETLR09,param);
      etlr_15 = GetValue(IN_ETLR15,param);
      etlr_16 = GetValue(IN_ETLR16,param);
      etlr_17 = GetValue(IN_ETLR17,param);
      etlr_25 = GetValue(IN_ETLR25,param);
      etlr_36 = GetValue(IN_ETLR36,param);
      etlr_37 = GetValue(IN_ETLR37,param);

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
/*
      if ( PASS == OpensShorts() ) {
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
      } else { 
        printf("------------------------------------\n");
        printf("---   PIN FAIL                  ---\n");
        printf("------------------------------------\n");
        printf("\n");
	return(PASS);
      } 
      */

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("+----+-----------------+\n");
  printf("| No |                 |\n");
  printf("+----+-----------------+\n");
  printf("|  0 | Write&Erase     |\n");
  printf("|  1 | Write           |\n");
  printf("|  2 | Erase           |\n");
  printf("|  3 | Erase_Houwa     |\n");
  printf("+----+-----------------+\n");
  printf("|  4 | Mulch_Write     |\n");
  printf("|  5 | Mulch_Erase     |\n");
  printf("| 45 | Mulch_W&E       |\n");
  printf("+----+-----------------+\n");
  printf("|  6 | Separate_Write  |\n");
  printf("|  7 | Separate_Erase  |\n");
  printf("| 67 | Separate_W&E    |\n");
  printf("+----+-----------------+\n");
  printf(" Select No (e:EXIT) -> ");	//DigitInput(&cond0);
  cond0 = 67;
  if ( cond0 == EXIT ) return(PASS);

  if ( cond0 < 3 ) {
  printf("*** Vcc  Select [1.8V_Only:1 / Loop(1.5~3.7V):5] ->"); DigitInput(&cond3);
  }
  printf("*** Test MAT    [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); //DigitInput(&mat);
  mat = 2;

  printf("*** Test Temp   [25C:25 / -40:40 / 95C:95 ] ->"); //DigitInput(&cond2);
  cond2 = temprature;
  sprintf(test_temp,"_%dC",temprature);

//  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond1);
//  if ( cond1 == 1 ) SetValue(IN_ETLR25,param,0xFD);   /* 2.5uA */
//  if ( cond1 == 2 ) SetValue(IN_ETLR25,param,0xFF);   /* 3.5uA */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10] = ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11] = ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12] = ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      switch(cond0) {
        case 0:
        case 1:
        case 2:  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_WriteErase",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
                 break;
        case 3:  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Erase-Houwa_MAT%d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],mat);
                 break;
        case 4:
        case 5:
        case 45: sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Mulch",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
                 sprintf(file_name2,"%s%s", sample_name, test_temp);
                 break;
        case 6:
        case 7:
        case 67: sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Separate_%s",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],Add_name);
                 sprintf(file_name2,"%s%s", sample_name, test_temp);
                 break;
        case 8:  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d_Erasekatei_%s",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],Add_name);
                 sprintf(file_name2,"%s%s", sample_name, test_temp);
                 break;
        default: break;
      }

      sprintf(file_name,"%s%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, test_temp, get_site_number());

  time( &time_list );// time_list += V4V5K_TIME_OFS;


/**************************************************************************************/
/***   Separate Write Tokusei                                                       ***/
/**************************************************************************************/
  if ((cond0 == 6) | (cond0 == 67)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Write>> ETLR15=0x%X,ETLR16=0x%X,Vcc=%d,Distribution:2.5V~0.1VStep,Mat=%d(0x0~0xFFF)",GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<5 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: loop=1 ; fprintf(file_log,"3ms_x1 ");   break;
      case 1: loop=3 ; fprintf(file_log,"1ms_x3 ");   break;
      case 2: loop=2 ; fprintf(file_log,"1ms+2ms");   break;
      case 3: loop=2 ; fprintf(file_log,"2ms+1ms");   break;
      case 4: loop=1 ; fprintf(file_log,"3ms_x1 ");   break;
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {

    if ( (i==0) || (i==4) ) {	 /* 3msx1 */
        SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0xF);
    }
    if ( i==1 ) {	 /* 1msx3 */
        SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
    }
    if ( i==2 ) {	 /* 1ms+2ms */
      if ( j==0 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
      if ( j==1 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x6);
    }
    if ( i==3 ) {	 /* 2ms+1ms */
      if ( j==0 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x6);
      if ( j==1 ) SetValue(IN_ETLR36,param,(etlr_36&0xF0)|0x4);
    }

    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
  }
  SetValue(IN_ETLR36,param,etlr_36);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 23;                 // VthRead2&3(0~2.5V)
  min_vol = 500; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   Separate Erase Tokusei                                                       ***/
/**************************************************************************************/
  if ((cond0 == 7) | (cond0 == 67)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Erase>>,ETLR15=0x%X,ETLR16=0x%X,Vcc=%d,Distribution:0V~0.1VStep,Mat=%d(0x0~0xFFF)",GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<5 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
    }

    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    vs[0] = 3300;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program");
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0: loop=1 ; fprintf(file_log,"5ms_x1 ");   break;
      case 1: loop=5 ; fprintf(file_log,"1ms_x5 ");   break;
      case 2: loop=2 ; fprintf(file_log,"1ms+4ms");   break;
      case 3: loop=2 ; fprintf(file_log,"4ms+1ms");   break;
      case 4: loop=1 ; fprintf(file_log,"5ms_x1 ");   break;
      default: break;
    }

  fclose( file_log );

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {

    if ( (i==0) || (i==4)) {	 /* 5msx1 */
        SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0xF);
    }
    if ( i==1 ) {	 /* 1msx5 */
        SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
    }
    if ( i==2 ) {	 /* 1ms+4ms */
      if ( j==0 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
      if ( j==1 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x6);
    }
    if ( i==3 ) {	 /* 4ms+1ms */
      if ( j==0 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x6);
      if ( j==1 ) SetValue(IN_ETLR37,param,(etlr_37&0xF0)|0x3);
    }

    if( j==0 ) SetValue(IN_BDATA00,param,0);
    else       SetValue(IN_BDATA00,param,1);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
  }
  SetValue(IN_BDATA00,param,0x00);
  SetValue(IN_ETLR37,param,etlr_37);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  if ( mat==0 ) {
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
  } else if ( mat==1 ) {
    SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
  } else if ( mat==2 ) {
    SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
  }

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 1;                 // VthRead2&3(0~2.5V)
  min_vol = -2000; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   Erase  Houwa kakunin                                                         ***/
/**************************************************************************************/
  if ((cond0 == 8)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Erase>>,ETLR15=0x%X,ETLR16=0x%X,Vcc=%d,Distribution:0V~0.1VStep,Mat=%d(0x0~0xFFF)",GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<=12 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
    }

    SetValue(IN_TESTSEL,param,0);
    vs[0] = 1600;
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0 : fprintf(file_log,"0.6ms"); SetValue(IN_ETLR37,param,0xF1);  break;
      case 1 : fprintf(file_log,"0.8ms"); SetValue(IN_ETLR37,param,0xF2);  break;
      case 2 : fprintf(file_log,"1.0ms"); SetValue(IN_ETLR37,param,0xF3);  break;
      case 3 : fprintf(file_log,"2.0ms"); SetValue(IN_ETLR37,param,0xF4);  break;
      case 4 : fprintf(file_log,"3.0ms"); SetValue(IN_ETLR37,param,0xF5);  break;
      case 5 : fprintf(file_log,"4.0ms"); SetValue(IN_ETLR37,param,0xF6);  break;
      case 6 : fprintf(file_log,"4.5ms"); SetValue(IN_ETLR37,param,0xF7);  break;
      case 7 : fprintf(file_log,"5.0ms"); SetValue(IN_ETLR37,param,0xFF);  break;
      case 8 : fprintf(file_log,"5.5ms"); SetValue(IN_ETLR37,param,0xF8);  break;
      case 9 : fprintf(file_log,"6.0ms"); SetValue(IN_ETLR37,param,0xF9);  break;
      case 10: fprintf(file_log,"7.0ms"); SetValue(IN_ETLR37,param,0xFA);  break;
      case 11: fprintf(file_log,"8.0ms"); SetValue(IN_ETLR37,param,0xFB);  break;
      case 12: fprintf(file_log,"10ms");  SetValue(IN_ETLR37,param,0xFC);  break;
      default: break;
    }
    loop=1;

  fclose( file_log );

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {
    SetValue(IN_BDATA00,param,0);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
  }
  SetValue(IN_BDATA00,param,0x00);
  SetValue(IN_ETLR37,param,etlr_37);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 12;                 // VthRead1&2(1.25~2.5V)
  min_vol = -2000; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }



/**************************************************************************************/
/***   Erase  katei kakunin                                                         ***/
/**************************************************************************************/
  if ((cond0 == 9)) {

  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);
  SetValue(IN_TOPADDR0 ,param,0);
  SetValue(IN_TESTSIZE0,param,0);
  SetValue(IN_TOPADDR1 ,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2 ,param,0);
  SetValue(IN_TESTSIZE2,param,0);

  /* Erase Test ********************************************/
  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }
  
  set_vcc = 3300;
  fprintf(file_log,"<<Separate-Erase>>,ETLR15=0x%X,ETLR16=0x%X,Vcc=%d,Distribution:0V~0.1VStep,Mat=%d(0x0~0xFFF)",GetValue(IN_ETLR15,param),GetValue(IN_ETLR16,param),set_vcc,mat);

  fprintf(file_log,"\n"); 
  fclose( file_log );

  for ( i=0 ; i<=12 ; i++ ) {

    /* shoki Write *****************************************************/
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x1000);    /* 4kB */
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x1000);    /* 4kB */
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x1000);    /* 4kB */    
    }

    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,0);
    vs[0] = 1600;
    if(pe_cond == 0){
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    }
    SetValue(IN_ETCR08,param,0x00);

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

    switch(i) {
      case 0  : pe_time = 5;break;

      case 1  : pe_time = 4;break;
      case 2  : pe_time = 1;break;

      case 3  : pe_time = 1;break;
      case 4  : pe_time = 4;break;

      case 5  : pe_time = 2;break;
      case 6  : pe_time = 3;break;

      case 7  : pe_time = 3;break;
      case 8  : pe_time = 2;break;

      case 9  : pe_time = 1;break;
      case 10 : pe_time = 1;break;
      case 11 : pe_time = 3;break;

      case 12 : pe_time = 3;break;
      case 13 : pe_time = 1;break;
      case 14 : pe_time = 1;break;

      case 15 : pe_time = 1;break;
      case 16 : pe_time = 3;break;
      case 17 : pe_time = 1;break;

      case 18 : pe_time = 1;break;
      case 19 : pe_time = 1;break;
      case 20 : pe_time = 1;break;
      case 21 : pe_time = 1;break;
      case 22 : pe_time = 1;break;
      default : break;
    }
    loop=1;

    pe_time_sum = pe_time_sum + pe_time;
    fprintf(file_log,"%dms",pe_time_sum);
    SetValue(IN_BDATA00,param,PreWrite_Skip);

    switch (pe_time){
      case 1:SetValue(IN_ETLR37,param,0xF3);break;
      case 2:SetValue(IN_ETLR37,param,0xF4);break;
      case 3:SetValue(IN_ETLR37,param,0xF5);break;
      case 4:SetValue(IN_ETLR37,param,0xF6);break;
      case 5:SetValue(IN_ETLR37,param,0xFF);break;
    }

    if(pe_time_sum == 5){
      pe_cond = 0;        //Next PE ON
      PreWrite_Skip = 0;  //Next Erase PreWrite ON
      pe_time_sum = 0;    //PE Total time Reset
    }else{
      pe_cond = 1;        //Next PE Off
      PreWrite_Skip = 1;  //Next Erase PreWrite Off
    }


  fclose( file_log );

  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;

  for ( j=0 ; j<loop ; j++ ) {
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase ALL");
  }
  SetValue(IN_BDATA00,param,0x00);
  SetValue(IN_ETLR37,param,etlr_37);

  /* Log-Output ************************************************/
  vs[0] = 3300;

  SetValue(IN_BDATA01,param,0);  // ALL Count
  SetValue(IN_BDATA02,param,99); // Log Headder
  SetValue(IN_BDATA04,param,1);  // Log Output -> DATALOG
  SetValue(IN_BDATA05,param,1);  // Vth-Level-Log:OFF
  vth_mode = 12;                 // VthRead1&2(1.25~2.5V)
  min_vol = -2000; 
  stack10 = SetValue(IN_EXTAL1,param,VTH_FREQ);

//  min_vol = VthEdgeSearch(3,100,&vs[0],W1SEC, param);  // Loss Search
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  printf( "### Distribution ###" );
  printf("bit_count = %d, vth_mode = %d, min_vol = %d\n",bit_count,vth_mode,min_vol);
  result = VthReadDist(vth_mode, bit_count, min_vol, 100, &vs[0],W1SEC, param, file_name2);
  SetValue(IN_EXTAL1,param,stack10);

  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA04,param,0);
  SetValue(IN_BDATA05,param,0);

  } /* for i */
  }

/**************************************************************************************/
/***   END                                                                          ***/
/**************************************************************************************/
  printf("\n"); 
  printf("**************************\n"); 
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("*** TEST END           ***\n");
  printf("**************************\n"); 

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
  vs[0] = stack9;
  /*********************/

}  

