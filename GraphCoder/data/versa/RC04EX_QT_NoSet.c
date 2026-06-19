 // ****************************************************************************** FILENAME RC04EX_WT.c  CONTENTS: This is the RC04EX(SOTB) FLASH module function program for the 5000 test system COPYRIGHT 2018 Renesas Technology ALL RIGHTS RESERVED 1'st created by T.Morioka(SRM13) *****************************************************************************



int BGR_QT_NoSet(interface_t* param , int sample_no)
{
  int  waittime,datasize;
  int  tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int  stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int  stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int  vs[NUM_POWERSUPPLY],Vresult[BGR_ARRAY_VCC];
  int  i,j,l;
  int  selvol,mode,result,fail_flag,sample_no_read,NameLen;
  int info[NUM_CHIPINFO];
  char status_name[25],test_name[20];
  char file_name[200],buff[300];
  char time_buf[32], time_buf2[32];
  char* cp;
  char TesterName[10];

  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  fail_flag =0;

  waittime = 3;

  Ffpt = 0;
  
  result = PASS;
  sprintf(status_name,"ALL_PASS");
  InitialValue(param);
  
  SetValue(IN_REG,param,0);
  //Use Chip Data

  
    // ----- Get tester ID ----- //
  NameLen = 10;
  get_system_option("__szTesterID",TesterName,&NameLen);

  
    // get time stump
  time( &time_list );
  // time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) { if((cp=strstr(time_buf,"")) != NULL) *cp = NUL;
  }

   // *********************************** 1st Chip & Before Data Close * ***********************************

    //File Name Set
  sprintf(file_name,"QT_SampleNo_SettingLog_%s_DUT%d",TesterName,get_site_number()+1);
  
    //sprintf(file_name,"TEST_L%s_BGRWT%d_Rev0p0",lot_name,wt_num);
  sprintf( buff, "%s%s%s.csv", glob_cdp, glob_datalog, file_name );

  if((Fptdata=fopen(buff,"rt")) == NULL) {
        //------ headder -----//
    if((Fptdata=fopen(buff,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !",buff);
      return(FAIL);
    }
        //Initial Set
    fprintf(Fptdata,"Lot Name,W No,X,Y,sample No,PASS/FAIL,StartTime,EndTime,");
        // ETLR Data Out
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"ETLR%2d,",(i - IN_ETLR00));
    }
    
    if(fclose(Fptdata)) exit(-10);
  }else{
        //--------- File Close ---------//
    if(fclose(Fptdata)) exit(-10);
  }
  
   // *********************************** Initial Test * ***********************************

    /                                     // ****** Open$Short Check ******/

  if(result == PASS){
    sprintf(test_name,"OpenShort");
    if ( OpensShorts() != PASS){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("----------- %s FAIL -----------",test_name);
      result = FAIL;
    }else{
      printf("*********** %s PASS ***********",test_name);
    }
  }
  DeviceSpecificPowerUp();
    /                                     // ****** WT(Rext) Check & WX^荞 ******/

  if(result == PASS){
    sprintf(test_name,"ExtraRead");
    result = GetChipInfo(&info[0],param);
    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("----------- %s FAIL -----------",test_name);
      result = FAIL;
    }else{
      printf("*********** %s PASS ***********",test_name);
    }
  }

    //----- Register Setting For EXTRA1 -----//
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

   // *********************************** Main Test * ***********************************
  DeviceSpecificPowerUp();
    /                                     // ****** Register Change & Set ******/

  if(result == PASS){
    sprintf(test_name,"EXT1Set");
        //--- Register Change ---//
    SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param) & 0xF8) | 0x06);
    // Vrsg => 3.4V
    SetValue(IN_ETLR18,param,(GetValue(IN_ETLR18,param) & 0xC7) | 0x30);
    // Vnoemi_R => 3.4V
    SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param) & 0x0F) | 0x00);
    // Vwi => 2.0V
    SetValue(IN_ETLR21,param,(GetValue(IN_ETLR21,param) & 0xF0) | 0x09);
    // Vdemg =>-1.0V

    
        //--- Register Setting ---//
      SetValue(IN_REG,param,3);
      //Use Versa Data
      stack0 = SetValue(IN_TESTSEL,param,0);
      stack1 = SetValue(IN_BDATA00,param,1);
      stack2 = SetValue(IN_BDATA01,param,0);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
      // t@[ŕύX\
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_BDATA00,param,stack1);
      SetValue(IN_BDATA01,param,stack2);

    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("----------- %s FAIL -----------",test_name);
      result = FAIL;
    }else{
      printf("*********** %s PASS ***********",test_name);
    }
  }
  
  SetValue(IN_REG,param,0);
  //Use Chip Data
  
    /                                     // ****** sample No Set ******/

  if(result == PASS){
    sprintf(test_name,"SampleNoSet");
      SetValue(IN_LDATA0 ,param,sample_no+10000);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);
      SetValue(IN_BDATA06,param,0);
      SetValue(IN_BDATA07,param,0);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
      SetValue(IN_LDATA0 ,param,0);

    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("----------- %s FAIL -----------",test_name);
    }else{
      printf("*********** %s PASS ***********",test_name);
    }
  }
  
  
    /                                     // ****** sample No Read ******/

  if(result == PASS){
    sprintf(test_name,"SampleNoCheck");
      SetValue(IN_REG,param,0);
      //Use Chip Data
      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      sample_no_read = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L) - 10000;
            //----- Register Setting For EXTRA1 -----//
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

    if ((PASS != result) | (sample_no !=sample_no_read)){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("----------- %s FAIL -----------",test_name);
    }else{
      printf("*********** %s PASS ***********",test_name);
    }
  }
  
    // get time stump
  time( &time_list2 );
  // time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf2,"%s",C_time((int *)&time_list2)) != 0) { if((cp=strstr(time_buf,"")) != NULL) *cp = NUL;
  }

   // *********************************** Result & File Out * ***********************************

    /                                     // ****** File Out ******/

    // log file open
  if((Fptdata=fopen(buff,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !",buff);
    return(FAIL);
  }

    // Initial Set
  fprintf(Fptdata,"%s,%d,%d,%d,%d,%s,%-24.24s,%-24.24s,",lotname,atoi(wafername),xy_location[0],xy_location[1],sample_no_read,status_name,time_buf,time_buf2);

    // ETLR Data Out
  for(i=IN_ETLR00;i<=IN_ETLR39;i++){
    fprintf(Fptdata,"%2X,",GetValue(i,param));
  }

    // Data Close
  if(fclose(Fptdata)) exit(-10);

  return(result);
}


