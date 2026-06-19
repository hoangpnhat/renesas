/******************************************************************************/
/*!
  @file RC04EXslc_VthTrim.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int WT_Vth_Trim(interface_t* param)
{
  FILE_T *fpt;
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25;
  int error_flag,fail_flag,waittime;
  int info[32];                    /* wmat chip information              */
  int trimvalue[8];                /*                                    */
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  int  vs[NUM_POWERSUPPLY];
  FILE_T *file_log;
  waittime = INIT_WAIT;
  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = fail_flag =0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;
  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  Ffpt = INIT_FFPT;
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
      etlr_25 = GetValue(IN_ETLR25,param);
      InitialValue(param);
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
  mat=4;
  area=0;
  SetValue(IN_ETLR25,param,0xFD);   /* 2.5uA */
  SetValue(IN_LDATA0,param,1000);
  SetValue(IN_LDATA1,param,-700);
  cond2=0;
  SetValue(IN_ETLR13,param,0xC8);   /* Vhh_e    */
  SetValue(IN_ETLR14,param,0x98);   /* Vhh_w    */
  SetValue(IN_ETLR15,param,0xFF);   /* Temp_ON  */
  SetValue(IN_ETLR26,param,0xCC);  // T1 Precharge DelayTime (min+4Tap) //
  SetValue(IN_ETLR27,param,0xFC);  // T1 Precharge DelayTime (min+4Tap) //
  SetValue(IN_ETLR28,param,0xF1);  // T2 Discharge DelayTime (min+1Tap) //
  SetValue(IN_ETLR29,param,0xF1);  // T2 Discharge DelayTime (min+1Tap) //
  SetValue(IN_ETLR30,param,0xF1);  // T2 Discharge DelayTime (min+1Tap) //
  SetValue(IN_ETLR32,param,0x31);  // T5 saenb2 (min+3Tap) , T3 saenb DelayTime (min+1Tap) //
  SetValue(IN_ETLR33,param,0x11);  // T4 saout DelayTime (min+1Tap) //
  SetValue(IN_ETLR34,param,0x11);  // T6 T4-T5 DelayTime (min+1Tap) //
  printf("*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
  printf("*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
  printf("*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
  printf("*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
  printf("*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));
  printf("*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
  printf("*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
  printf("*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));
  tempo6 = (GetValue(IN_ETLR20,param)& 0xF0);
  SetValue(IN_ETLR20,param,(tempo6| 0x09)); /*Vpp_E=-3.0V*/

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

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_VthTrim",info[0],info[1],info[2],info[3],info[4],info[5],info[10]);

      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());
      if( NULL==(file_log = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
      }

      fprintf(file_log,"X/Y,%3d,%3d",info[11],info[12]);
	     fprintf(file_log,"\n");
/**************************************************************************************/
/***   Iref-Trim                                                                    ***/
/**************************************************************************************/
  SetValue(IN_REG,param,0x03);
  SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
  SetValue(IN_BDATA15,param,UA25);
  CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);
  SetValue(IN_BDATA15,param,UA25);
  SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
  result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
  result_iref = selvol;
  SetValue(IN_ETLR22,param,0xFB);//Pumpup(8MHz->16MHz)
  printf("ETLR22;%02X\n",GetValue(IN_ETLR22,param));

/**************************************************************************************/
/***   Loss Vth-Trim                                                                ***/
/**************************************************************************************/
  vs[0] = 3300;
  if ((area == 0) || (area == 1)) {
  fprintf(file_log,"<<Loss>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);

/* Simple Erase *********************************************/
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    SetValue(IN_TOPADDR1,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    SetValue(IN_TOPADDR2,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
    SetValue(IN_ETCR08,param,0x00);

    /* Program *****************************************************/
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    SetValue(IN_TOPADDR1,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    SetValue(IN_TOPADDR2,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

    /* Vth-Search ************************************************/
    result = VthEdgeSearch(3,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV\n",result);

    /* Vth Judgment **********************************************/
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

    if      ( judg == 999 ) break;  /* Fail */
    else if ( judg ==   0 ) break;  /* Pass */

    SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + judg) | 0xC0);
  }
  fprintf(file_log,"\n"); 
  }

/**************************************************************************************/
/***   Gain Vth-Trim                                                                ***/
/**************************************************************************************/
    vs[0] = 3300;
	if ((area == 0) || (area == 2)) {
  fprintf(file_log,"<<Gain>> ETLR25=0x%X,",GetValue(IN_ETLR25,param));
  SetValue(IN_REG,param,0x03);
  for ( i=0 ; i<8 ; i++ ) {
    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    /* Simple writing ********************************************/
    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    SetValue(IN_TOPADDR1,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    SetValue(IN_TOPADDR2,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    SetValue(IN_ETCR08,param,0x20);
    SetValue(IN_TESTSEL,param,0);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
    SetValue(IN_ETCR08,param,0x00);

    /* Erase *****************************************************/

    SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
    SetValue(IN_TOPADDR1,param,FLI1_TOP);
    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
    SetValue(IN_TOPADDR2,param,FLI2_TOP);
    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

    /* Vth-Search ************************************************/
    vs[0] = 3300;
    result = VthEdgeSearch(2,20,&vs[0],W1SEC*waittime, param);
    printf(" ----result---- %dmV\n",result);

    /* Vth Judgment **********************************************/
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

    if      ( judg == 999 ) break;  /* Fail */
    else if ( judg ==   0 ) break;  /* Pass */
   	if      ( error_flag ==1 ) break;  /* Vhh_e Tap  Limit */

    SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + judg) | 0xC0);

    if(GetValue(IN_ETLR16,param) > 0xFF){
      printf("!! Tap Limit !!\n");
      SetValue(IN_ETLR16,param,0xFF | 0xC0);
     	error_flag = 1;
    }
  }
  fprintf(file_log,"\n"); 
  }

/**************************************************************************************/
/***   Log Out                                                                      ***/
/**************************************************************************************/
  fprintf(file_log,"\n"); 
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
  fprintf(file_log,"Vhh_w/Vpp_w = ,%d,%d\n",result_vhhw,result_vppw); 
  fprintf(file_log,"Vhh_e/Vpp_e = ,%d,%d\n",result_vhhe,result_vppe); 
  fprintf(file_log,"Iref = ,%d[nA]\n",result_iref); 
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

  fprintf(file_log,"\n");

/**************************************************************************************/
/***   Result                                                                       ***/
/**************************************************************************************/
  if ((area == 0) || (area == 1)) {
    if ( result_loss == 0 ) {
      printf("\n"); 
      printf("**************************\n"); 
      printf("*** <<Loss>> Pass      ***\n");
      printf("*** <<Loss>> Pass      ***\n");
      printf("*** <<Loss>> Pass      ***\n");
      printf("**************************\n"); 
    } else if ( result_loss == 999 ) {
      printf("\n"); 
      printf("==========================\n"); 
      printf("=== <<Loss>> Fail      ===\n");
      printf("=== <<Loss>> Fail      ===\n");
      printf("=== <<Loss>> Fail      ===\n");
      printf("==========================\n");
    	 fail_flag =1;
    } else {
      printf("\n"); 
      printf("##########################\n"); 
      printf("### <<Loss>> Loop Over ###\n");
      printf("### <<Loss>> Loop Over ###\n");
      printf("### <<Loss>> Loop Over ###\n");
      printf("##########################\n");
     	fail_flag =1;
    }  
      printf("loop_num = %d\n",loop_loss); 
      printf("vth_w    = %d\n",vth_loss); 
      printf("\n"); 
  }

  if ((area == 0) || (area == 2)) {
    if ( result_gain == 0 ) {
      printf("\n"); 
      printf("**************************\n"); 
      printf("*** <<Gain>> Pass      ***\n");
      printf("*** <<Gain>> Pass      ***\n");
      printf("*** <<Gain>> Pass      ***\n");
      printf("**************************\n"); 
    } else if ( result_gain == 999 ) {
      printf("\n"); 
      printf("==========================\n"); 
      printf("=== <<Gain>> Fail      ===\n");
      printf("=== <<Gain>> Fail      ===\n");
      printf("=== <<Gain>> Fail      ===\n");
      printf("==========================\n");
    	 fail_flag =1;
    } else if ( 0 < result_gain && result_gain < 21) {
      printf("\n"); 
      printf("==========================\n"); 
      printf("=== <<Gain>> Over Tap  ===\n");
      printf("=== <<Gain>> Over Tap  ===\n");
      printf("=== <<Gain>> Over Tap  ===\n");
      printf("==========================\n");
    	 fail_flag =1;
    } else {
      printf("\n"); 
      printf("##########################\n"); 
      printf("### <<Gain>> Loop Over ###\n");
      printf("### <<Gain>> Loop Over ###\n");
      printf("### <<Gain>> Loop Over ###\n");
      printf("##########################\n");
     	fail_flag =1;
    }  
      printf("loop_num = %d\n",loop_gain); 
      printf("vth_w    = %d\n",vth_gain); 
      printf("\n"); 
  }

  if ( cond2==0 ) {
      printf("\n"); 
    if (( result_iref>(7500-300) ) && ( result_iref<(7500+300) )) {
      printf("**************************\n"); 
      printf("*** <<Iref>> Pass      ***\n");
      printf("*** <<Iref>> Pass      ***\n");
      printf("*** <<Iref>> Pass      ***\n");
      printf("**************************\n"); 
    } else {
      printf("==========================\n"); 
      printf("=== <<Iref>> Fail      ===\n");
      printf("=== <<Iref>> Fail      ===\n");
      printf("=== <<Iref>> Fail      ===\n");
      printf("==========================\n");
     	fail_flag =1;
    } 
      printf("Iref = %d[nA] (7200~7800uA)\n",result_iref); 
      printf("\n"); 
  }

	if(fail_flag == 0) fprintf(file_log,"VthTrim : PASS");
	if(fail_flag == 1) fprintf(file_log,"VthTrim : FAIL");
    fprintf(file_log,"\n\n");
	/*******************/ 
  fclose( file_log );
/**************************************************************************************/
/***   Register ReWrite                                                             ***/
/**************************************************************************************/
  printf("*** Register Re-Write  [OK:0 / NG:1 ] ->"); cond1=0;

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
//  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

