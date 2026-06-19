/*******************************************************************************
*  FILENAME RC04EX_WT.c
*
  CONTENTS: This is the RC04EX(SOTB_IO) FLASH module function program
  for the 5000 test system
  COPYRIGHT 2019 Renesas Technology ALL RIGHTS RESERVED
  1'st created by T.Morioka(SRM13)
 ******************************************************************************/

int WT_WS_Flash(interface_t* param){
  int  waittime,datasize;
  int  tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int  stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int  stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int  vs[NUM_POWERSUPPLY];
  int  i,j,l,extra_num;
  int  selvol,mode,result,fail_flag,result_judge;
  int info[NUM_CHIPINFO];
  char status_name[25],test_name[20],mdl_name[10];
  char file_name[200],buff[300];
  char time_buf[32], time_buf2[32];
  char* cp;

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
  Ffpt = 0;
  /************************************
   * Initial Test *
   ************************************/
  //******* Open$Short Check *******//
  if(result == PASS){
    sprintf(test_name,"OpenShort");
    printf("\n============= %s Test =============\n",test_name);
    if ( OpensShorts() != PASS){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  DeviceSpecificPowerUp();

  if(result == PASS){
    sprintf(test_name,"DeviceFunc_P");
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*2,param,CreatePatFileName("devicefunction_at"),test_name);
    result_judge = GetValue(OUT_JUDGE1,param);
    if ((PASS != result) | (result_judge != PASS) ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  /************************************
   * Main Test *
   ************************************/
  DeviceSpecificPowerUp();

  //******* Mdl Loop *******//
  for(extra_num = 0; extra_num <= 6; extra_num++){
    // Addr Setting
    SetValue(IN_AREA,param,extra_num);
    switch ( extra_num ){
      case 0:
        SetValue(IN_TOPADDR0,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
        sprintf(mdl_name,"FLP");
      break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
        SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
        sprintf(mdl_name,"Extra");
      break;
      case 6:
        SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
        SetValue(IN_TESTSIZE0,param,EXTRAB_SIZE);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
        sprintf(mdl_name,"Extra");
      break;
      default:
        printf("extra_num Setting Error!!! C-code\n");
        return(FAIL);
      break;
    }

    //-------- Extra Erase --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-Erase",mdl_name,extra_num);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("erase_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }

    //-------- Extra Read ALL"1" --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-ReadALL1",mdl_name,extra_num);
      SetValue(IN_TESTSEL,param,1);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }

    //-------- Extra Write --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-Write",mdl_name,extra_num);
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("programall_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }

    //-------- Extra Read ALL"0" --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-ReadALL0",mdl_name,extra_num);
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }

    //-------- Extra Erase --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-Erase-2",mdl_name,extra_num);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("erase_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }

    //-------- Extra Read ALL"1" --------//
    if(result == PASS){
      sprintf(test_name,"%s%d-ReadALL1-2",mdl_name,extra_num);
      SetValue(IN_TESTSEL,param,1);
      SetValue(IN_BDATA00,param,0);
      printf("\n============= %s Test =============\n",test_name);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
      if (PASS != result ){
        sprintf(status_name,"%s_FAIL",test_name);
        printf("------------- %s FAIL -------------\n",test_name);
        result = FAIL;
      }else{
        printf("************* %s PASS *************\n",test_name);
      }
    }


    //CHK Check
  //-------- FLP0 CHK-A Write --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKA-Write",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);// CHKA Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 CHK-A Read --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKA-Read",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);// CHKA Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  
  //-------- FLP0 CHK-B Write --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKB-Write",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,1);// CHKB Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 ALL0 Read --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-ReadALL0-2",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  
  //-------- FLP0 Erase --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-Erase-3",mdl_name,extra_num);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("erase_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 Read ALL"1" --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-ReadALL1-3",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,1);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  }//Extra Loop

  /*********************************
   * FLP0 CHK Check
   *********************************/
  /*
  extra_num = 0;
  SetValue(IN_AREA,param,extra_num);
  SetValue(IN_TOPADDR0,param,FLP0_TOP);
  SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
  SetValue(IN_TOPADDR1,param,0);
  SetValue(IN_TESTSIZE1,param,0);
  SetValue(IN_TOPADDR2,param,0);
  SetValue(IN_TESTSIZE2,param,0);
  sprintf(mdl_name,"FLP");

  //-------- FLP0 CHK-A Write --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKA-Write",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);// CHKA Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 CHK-A Read --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKA-Read",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);// CHKA Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  
  //-------- FLP0 CHK-B Write --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-CHKB-Write",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,1);// CHKB Select
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*20,param,CreatePatFileName("programchecker_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 ALL0 Read --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-ReadALL0-2",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  
  //-------- FLP0 Erase --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-Erase-3",mdl_name,extra_num);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("erase_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //-------- FLP0 Read ALL"1" --------//
  if(result == PASS){
    sprintf(test_name,"%s%d-ReadALL1-3",mdl_name,extra_num);
    SetValue(IN_TESTSEL,param,1);
    SetValue(IN_BDATA00,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("readall_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }
  */

  //******* Trim Data Write *******//
  if(result == PASS){
    sprintf(test_name,"Trim-Write");
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,1);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("settrimingdata_code_at"),test_name);
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  //******* Trim Data Read *******//
  if(result == PASS){
    sprintf(test_name,"Trim-Read");
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA00,param,0);
    SetValue(IN_REG,param,0);
    printf("\n============= %s Test =============\n",test_name);
    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("devicefunction_at"),test_name);
    if(result == PASS){
        for(i=IN_ETLR00;i<=IN_ETLR39;i++){
            if(result == PASS){
                if( (GetValue(i,param)) != (GetValue((i-IN_ETLR00) + OUT_ETLR00,param)) ){
                    result = FAIL;
                }
            }
        }
    }
    if (PASS != result ){
      sprintf(status_name,"%s_FAIL",test_name);
      printf("------------- %s FAIL -------------\n",test_name);
      result = FAIL;
    }else{
      printf("************* %s PASS *************\n",test_name);
    }
  }

  
  /* get time stump */
  time( &time_list2 );// time_list += V4V5K_TIME_OFS;
  if(sprintf(time_buf2,"%s",C_time((int *)&time_list2)) != 0) {
    if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
  }

  printf("============================\n");
  printf("==== Test Time : %d[s] ====\n",time_list2 - time_list);
  printf("============================\n");
  printf("== Result : %s ==\n",status_name);


  return(result);
}


