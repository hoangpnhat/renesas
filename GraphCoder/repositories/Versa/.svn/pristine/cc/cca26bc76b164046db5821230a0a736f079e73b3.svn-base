/*******************************************************************************
*  FILENAME RC04EX_WT.c
*
  CONTENTS: This is the RC04EX(SOTB) FLASH module function program
  for the 5000 test system
  COPYRIGHT 2018 Renesas Technology ALL RIGHTS RESERVED
  1'st created by T.Morioka(SRM13)
 ******************************************************************************/



int BGR_WT(interface_t* param , int wt_num)
{
  int  waittime,datasize;
  int  tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int  stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int  stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int  vs[NUM_POWERSUPPLY],Vresult[BGR_ARRAY_VCC];
  int  i,j,l;
  int  selvol,mode,result,fail_flag;
  int info[NUM_CHIPINFO];
  char status_name[25],test_name[20];
  char file_name[200],buff[300];
  char time_buf[32], time_buf2[32];
  char* cp;

  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  fail_flag =0;
  
  result = PASS;
  sprintf(status_name,"ALL_PASS");
  InitialValue(param);
  
  
  for(i=0;i<=(BGR_ARRAY_VCC - 1);i++){
    Vresult[i] = 0;
  }
  
  /* get time stump */
  time( &time_list );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * 1st Chip & Before Data Close *
   ************************************/

  //File Name Set
  sprintf(file_name,"%s_W%d_BGRWT%d_Rev1p1",lotname,atoi(wafername),wt_num);
  
  //sprintf(file_name,"TEST_L%s_BGRWT%d_Rev0p0",lot_name,wt_num);
  sprintf( buff, "%s%s%s.csv", glob_cdp, glob_datalog, file_name );

  if((Fptdata=fopen(buff,"rt")) == NULL) {
    //------ headder -----//
    if((Fptdata=fopen(buff,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !\n",buff);
      return(FAIL);
    }
    //Initial Set
    fprintf(Fptdata,"X,Y,PASS/FAIL,StartTime,EndTime,");
    //Monitor data Out
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY0);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY1);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY2);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY3);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY4);
    // ETLR Data Out
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"ETLR%2d,",(i - IN_ETLR00));
    }
    
    if(fclose(Fptdata)) exit(-10);
  }else{
    //--------- File Close ---------//
    if(fclose(Fptdata)) exit(-10);
  }
  
  /************************************
   * Initial Test *
   ************************************/

  //******* Open$Short Check *******//
  if(result == PASS){
    sprintf(test_name,"OpenShort");
    if ( OpensShorts() != PASS){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  DeviceSpecificPowerUp();
  //******* WT(Rext) Check & レジスタ取り込み *******//
  if(result == PASS){
    sprintf(test_name,"ExtraRead");
    
    stack0 = SetValue(IN_REG,param,CHIPDATA);
    stack1 = SetValue(IN_AREA,param,5);	 /* Extra5 */
    stack2 = SetValue(IN_TOPADDR0,param,0);
    stack3 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);

    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("readdump_code_at"),"Chip-Info Get");  /*ファーム次第で変更する可能性あり*/
		
    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }

  // ********* Chip Info ********* // 
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

  //  ---------- return Value ----------  //
  SetValue(IN_REG,param,stack0);
  SetValue(IN_AREA,param,stack1);	 // Extra5 //
  SetValue(IN_TOPADDR0,param,stack2);
  SetValue(IN_TESTSIZE0,param,stack3);

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
  SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);


  //******* Rext PassCode Check *******//
  if(result == PASS){
    sprintf(test_name,"PassCode");
    if ( info[19] != 0x40){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }

  /************************************
   * Main Test *
   ************************************/
  DeviceSpecificPowerUp();

  //******* Vrsg Monitor *******//
  if(result == PASS){
    sprintf(test_name,"VrsgMoni");
    if (PASS != VrsgMonitor_Repeat(param,&Vresult[0])){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  
  //******* Vrsg Check *******//
  if(result == PASS){
    sprintf(test_name,"VrsgCheck");
    for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
      if(Vresult[i] == FAIL) fail_flag = 1;
    }
    if (fail_flag == 1){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  
  /* get time stump */
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf2,"%s",C_time((int *)&time_list2)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * Result & File Out *
   ************************************/

  //******* File Out *******//
  /* log file open */
  if((Fptdata=fopen(buff,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",buff);
    return(FAIL);
  }

  // Initial Set
  fprintf(Fptdata,"\n%d,%d,%s,%-24.24s,%-24.24s,",xy_location[0],xy_location[1],status_name,time_buf,time_buf2);

  // Monitor data Out
  for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
    fprintf(Fptdata,"%d,",Vresult[i]);
  }

  // ETLR Data Out
  for(i=IN_ETLR00;i<=IN_ETLR39;i++){
    fprintf(Fptdata,"%2X,",GetValue(i,param));
  }

  // Data Close
  if(fclose(Fptdata)) exit(-10);

  return(result);
}



int BGR_WT_HT(interface_t* param)
{
  int  waittime,datasize,wt_num;
  int  tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int  stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int  stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int  vs[NUM_POWERSUPPLY],Vresult[BGR_ARRAY_VCC],etlr00_bak[BGR_ARRAY_VCC],etlr01_bak[BGR_ARRAY_VCC];
  int  i,j,l;
  int  selvol,mode,result,fail_flag;
  int  info[NUM_CHIPINFO];
  char status_name[25],test_name[20];
  char file_name[200],buff[300];
  char time_buf[32], time_buf2[32];
  char* cp;

  wt_num = 2;
  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  fail_flag =0;
  
  result = PASS;
  sprintf(status_name,"ALL_PASS");
  InitialValue(param);
  
  
  for(i=0;i<=(BGR_ARRAY_VCC - 1);i++){
    Vresult[i] = 0;
  }
  
  /* get time stump */
  time( &time_list );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * 1st Chip & Before Data Close *
   ************************************/

  //File Name Set
  sprintf(file_name,"%s_W%d_BGRWT%d_Rev1p1",lotname,atoi(wafername),wt_num);
  
  //sprintf(file_name,"TEST_L%s_BGRWT%d_Rev0p0",lot_name,wt_num);
  sprintf( buff, "%s%s%s.csv", glob_cdp, glob_datalog, file_name );

  if((Fptdata=fopen(buff,"rt")) == NULL) {
    //------ headder -----//
    if((Fptdata=fopen(buff,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !\n",buff);
      return(FAIL);
    }
    //Initial Set
    fprintf(Fptdata,"X,Y,PASS/FAIL,StartTime,EndTime,");
    //Monitor data Out
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY0);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY1);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY2);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY3);
    fprintf(Fptdata,"Vrsg_Vcc%d,",VCC_ARRAY4);
    
    // ETLR Data Out
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"ETLR%2d,",(i - IN_ETLR00));
    }
    
    // BGR Trim Result Out
    fprintf(Fptdata,"BGR_ETLR00_Vcc%d,BGR_ETLR01_Vcc%d,",VCC_ARRAY0,VCC_ARRAY0);
    fprintf(Fptdata,"BGR_ETLR00_Vcc%d,BGR_ETLR01_Vcc%d,",VCC_ARRAY1,VCC_ARRAY1);
    fprintf(Fptdata,"BGR_ETLR00_Vcc%d,BGR_ETLR01_Vcc%d,",VCC_ARRAY2,VCC_ARRAY2);
    fprintf(Fptdata,"BGR_ETLR00_Vcc%d,BGR_ETLR01_Vcc%d,",VCC_ARRAY3,VCC_ARRAY3);
    fprintf(Fptdata,"BGR_ETLR00_Vcc%d,BGR_ETLR01_Vcc%d,",VCC_ARRAY4,VCC_ARRAY4);
    
    if(fclose(Fptdata)) exit(-10);
  }else{
    //--------- File Close ---------//
    if(fclose(Fptdata)) exit(-10);
  }
  
  /************************************
   * Initial Test *
   ************************************/

  //******* Open$Short Check *******//
  if(result == PASS){
    sprintf(test_name,"OpenShort");
    if ( OpensShorts() != PASS){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  DeviceSpecificPowerUp();
  //******* WT(Rext) Check & レジスタ取り込み *******//
  if(result == PASS){
    sprintf(test_name,"ExtraRead");
    
    stack0 = SetValue(IN_REG,param,CHIPDATA);
    stack1 = SetValue(IN_AREA,param,5);	 /* Extra5 */
    stack2 = SetValue(IN_TOPADDR0,param,0);
    stack3 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);

    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("readdump_code_at"),"Chip-Info Get");  /*ファーム次第で変更する可能性あり*/
		
    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }

  // ********* Chip Info ********* // 
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

  //  ---------- return Value ----------  //
  SetValue(IN_REG,param,stack0);
  SetValue(IN_AREA,param,stack1);	 // Extra5 //
  SetValue(IN_TOPADDR0,param,stack2);
  SetValue(IN_TESTSIZE0,param,stack3);

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
  SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);

  etlr00_bak[2] = GetValue(OUT_ETLR00,param);
  etlr01_bak[2] = GetValue(OUT_ETLR01,param);

  //******* Rext PassCode Check *******//
  if(result == PASS){
    sprintf(test_name,"PassCode");
    if ( info[19] != 0x40){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  /************************************
   * Main Test *
   ************************************/
  DeviceSpecificPowerUp();

  //******* Vrsg Monitor Vcc2800mV *******//
  if(result == PASS){
    sprintf(test_name,"VrsgMoni");
    if (PASS != VrsgMonitor_Single(2,param,&tempo1)){  //
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  Vresult[2] = tempo1;


  //******* Vrsg Trim Vcc1300~3600mV *******//
  if(result == PASS){
  
    for(i=0;i<=(BGR_ARRAY_VCC - 1);i++){
      if(result == PASS){
        if(i != 2){
        
          result = BgrTrim_RegWrite(0, i, param, Vresult[2]);
          etlr00_bak[i] = GetValue(OUT_ETLR00,param);
          etlr01_bak[i] = GetValue(OUT_ETLR01,param);
	
          if(result == TRIM_FAIL){
            sprintf(status_name,"%s_FAIL","BGRTrim");
          }else if(result == REG_FAIL){
            sprintf(status_name,"%s_FAIL","REGWR");
          }else{
	  
	        result = VrsgMonitor_Single(i,param,&tempo1);
	        Vresult[i] = tempo1;
            if(result != PASS) sprintf(status_name,"%s_FAIL","VrsgMoni");
          }
        }
    

    
        if(result == PASS){
          printf("\n*********** BGR Trim and Moni PASS ***********\n");
        }else{
          printf("\n----------- %s FAIL -----------\n",test_name);
        }
  	  }
  	}
  }

  DeviceSpecificPowerUp();
  //******* EXTRA1 return *******//
  if(result == PASS){
    sprintf(test_name,"ExtraWrite");
    
    stack0 = SetValue(IN_TESTSEL,param,1);
    stack1 = SetValue(IN_BDATA00,param,1);
    stack2 = SetValue(IN_BDATA01,param,2);
    
    SetValue(IN_ETLR00,param,etlr00_bak[2]);
    SetValue(IN_ETLR01,param,etlr01_bak[2]);
	vs[0]=3300;

    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");  /*ファーム次第で変更する可能性あり*/
    
    SetValue(IN_TESTSEL,param,stack0);
    SetValue(IN_BDATA00,param,stack1);
    SetValue(IN_BDATA01,param,stack2);      
    
    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }

  //******* Vrsg Check *******//
  if(result == PASS){
    sprintf(test_name,"VrsgCheck");
    for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
      if(Vresult[i] == FAIL) fail_flag = 1;
    }
    if (fail_flag == 1){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  
  /* get time stump */
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf2,"%s",C_time((int *)&time_list2)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * Result & File Out *
   ************************************/

  //******* File Out *******//
  /* log file open */
  if((Fptdata=fopen(buff,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",buff);
    return(FAIL);
  }

  // Initial Set
  fprintf(Fptdata,"\n%d,%d,%s,%-24.24s,%-24.24s,",xy_location[0],xy_location[1],status_name,time_buf,time_buf2);

  // Monitor data Out
  for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
    fprintf(Fptdata,"%d,",Vresult[i]);
  }

  // ETLR Data Out
  for(i=IN_ETLR00;i<=IN_ETLR39;i++){
    fprintf(Fptdata,"%2X,",GetValue(i,param));
  }
  
  for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
    fprintf(Fptdata,"%X,%X,",etlr00_bak[i],etlr01_bak[i]);
  }

  // Data Close
  if(fclose(Fptdata)) exit(-10);

  return(result);
}

int WT_Def(interface_t* param)
{
  int  waittime,datasize,wt_num;
  int  tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int  stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int  stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int  vs[NUM_POWERSUPPLY],Vresult[BGR_ARRAY_VCC],etlr00_bak[BGR_ARRAY_VCC],etlr01_bak[BGR_ARRAY_VCC];
  int  i,j,l;
  int  selvol,mode,result,fail_flag;
  int  info[NUM_CHIPINFO];
  char status_name[25],test_name[20];
  char file_name[200],buff[300];
  char time_buf[32], time_buf2[32];
  char* cp;

  wt_num = 2;
  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  fail_flag =0;
  
  result = PASS;
  sprintf(status_name,"ALL_PASS");
  InitialValue(param);
  
  /* get time stump */
  time( &time_list );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * 1st Chip & Before Data Close *
   ************************************/

  //File Name Set
  sprintf(file_name,"%s_W%d_Test",lotname,atoi(wafername),wt_num);
  
  sprintf( buff, "%s%s%s.csv", glob_cdp, glob_datalog, file_name );

  if((Fptdata=fopen(buff,"rt")) == NULL) {
    //------ headder -----//
    if((Fptdata=fopen(buff,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !\n",buff);
      return(FAIL);
    }
    //Initial Set
    fprintf(Fptdata,"X,Y,PASS/FAIL,StartTime,EndTime,");
    
    // ETLR Data Out
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"ETLR%2d,",(i - IN_ETLR00));
    }
    
    if(fclose(Fptdata)) exit(-10);
  }else{
    //--------- File Close ---------//
    if(fclose(Fptdata)) exit(-10);
  }
  
  /************************************
   * Initial Test *
   ************************************/

  //******* Open$Short Check *******//
  if(result == PASS){
    sprintf(test_name,"OpenShort");
    if ( OpensShorts() != PASS){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  DeviceSpecificPowerUp();
  //******* WT(Rext) Check & レジスタ取り込み *******//
  if(result == PASS){
    sprintf(test_name,"ExtraRead");
    
    stack0 = SetValue(IN_REG,param,CHIPDATA);
    stack1 = SetValue(IN_AREA,param,5);	 // Extra5 
    stack2 = SetValue(IN_TOPADDR0,param,0);
    stack3 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);

    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("readdump_code_at"),"Chip-Info Get");
		
    if (PASS != result){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }

  // ********* Chip Info ********* // 
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

  //  ---------- return Value ----------  //
  SetValue(IN_REG,param,stack0);
  SetValue(IN_AREA,param,stack1);	 // Extra5 //
  SetValue(IN_TOPADDR0,param,stack2);
  SetValue(IN_TESTSIZE0,param,stack3);

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
  SetValue(IN_REG,param,stack0); SetValue(IN_TESTSEL,param,stack1);

  etlr00_bak[2] = GetValue(OUT_ETLR00,param);
  etlr01_bak[2] = GetValue(OUT_ETLR01,param);

  //******* Rext PassCode Check *******//
  /* 要らない？
  if(result == PASS){
    sprintf(test_name,"PassCode");
    if ( info[19] != 0x40){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("\n----------- %s FAIL -----------\n",test_name);
      result = FAIL;
    }else{
      printf("\n*********** %s PASS ***********\n",test_name);
    }
  }
  */
  /************************************
   * Main Test *
   ************************************/
  DeviceSpecificPowerUp();


  
  /* get time stump */
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf2,"%s",C_time((int *)&time_list2)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  /************************************
   * Result & File Out *
   ************************************/

  //******* File Out *******//
  /* log file open */
  if((Fptdata=fopen(buff,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !\n",buff);
    return(FAIL);
  }

  // Initial Set
  fprintf(Fptdata,"\n%d,%d,%s,%-24.24s,%-24.24s,",xy_location[0],xy_location[1],status_name,time_buf,time_buf2);

  // ETLR Data Out
  for(i=IN_ETLR00;i<=IN_ETLR39;i++){
    fprintf(Fptdata,"%2X,",GetValue(i,param));
  }
  
  // Data Close
  if(fclose(Fptdata)) exit(-10);

  return(result);
}

