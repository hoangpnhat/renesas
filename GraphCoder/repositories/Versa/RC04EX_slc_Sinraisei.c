/******************************************************************************/
/*!
  @file RC04EXslc_Sinraisei.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Ret_Distribution(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */

  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
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
      } 
      */
/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("*** Test-Select [Ret:0    / Dis:1  / ReadDisturb:2] ->"); DigitInput(&cond0);
    if ( cond0==0 ) {
      printf("\n");
      printf("##############################\n");
      printf("### Retention Test         ###\n");
      printf("### Retention Test         ###\n");
      printf("### Retention Test         ###\n");
      printf("##############################\n");
      printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
	} else if ( cond0==1 ) {
      printf("\n");
      printf("==============================\n");
      printf("||| Disturb Test           |||\n");
      printf("||| Disturb Test           |||\n");
      printf("||| Disturb Test           |||\n");
      printf("==============================\n");
      printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
	} else if ( cond0==2 ) {
	  printf("\n");
	  printf("syoki ETLR00(90CTrim):0x%02X\n", GetValue(IN_ETLR00,param));
      printf("syoki ETLR01(90CTrim):0x%02X\n", GetValue(IN_ETLR01,param));
      SetValue(IN_ETLR00,param,Extra5_ETLR00[3]);//Set ETLR00 BGR25C Trim
      SetValue(IN_ETLR01,param,Extra5_ETLR01[3]);//Set ETLR01 BGR25C Trim
      printf("++++++++++++++++++++++++++++++\n");
      printf("+++ ReadDisturb Test       +++\n");
      printf("+++ ReadDisturb Test       +++\n");
      printf("+++ ReadDisturb Test       +++\n");
	  printf("Set25C ETLR00:0x%02X\n", GetValue(IN_ETLR00,param));//BGRset Reg00
	  printf("Set25C ETLR01:0x%02X\n", GetValue(IN_ETLR01,param));//BGRset Reg01
      printf("++++++++++++++++++++++++++++++\n");
      printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
	}

  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2/ Extra1:11 ] ->"); DigitInput(&mat);
  printf("*** Test Data-Area [Loss&Gain:0 / Loss:1 / Gain:2] ->"); DigitInput(&area);
  if ( cond0 == 1 ) {
  printf("*** Test Start-Add [0x0:0 / 0x20000:1 / 0x40000:2 / 0x60000:3] ->"); DigitInput(&st_addr);
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  printf("*** Test Temp   [-40:40 / 25C:25 / 60C:60 / 90C:90 ] ->"); DigitInput(&cond2);
  if ( cond2 == 25 ) sprintf(test_temp,"_25C");
  if ( cond2 == 40 ) sprintf(test_temp,"_-40C");
  if ( cond2 == 60 ) sprintf(test_temp,"_60C");
  if ( cond2 == 90 ) sprintf(test_temp,"_90C");

  printf("*** Vth-Read_I(ETLR25)  [Chip-Data:0 / 2.5uA:1 / 3.5uA:2 ] ->"); DigitInput(&cond4);
  if ( cond4 == 1 ) SetValue(IN_ETLR25,param,0xFD);   /* 2.5uA */
  if ( cond4 == 2 ) SetValue(IN_ETLR25,param,0xFF);   /* 3.5uA */
  sprintf(vthread_i,"_%X",GetValue(IN_ETLR25,param));

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) | (cond1==1) ) {
    vs[0] = 1600;
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ( cond0==0) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);   
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE"); /*ファーム次第で変更する可能性あり*/
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==1 ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ( cond0==0) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE"); /*ファーム次第で変更する可能性あり*/
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==2 ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ( cond0==0) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE"); /*ファーム次第で変更する可能性あり*/
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }else if( mat==11 ){
      SetValue(IN_AREA,param,1);
      SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      stack9 = SetValue(IN_BDATA00,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/
      SetValue(IN_BDATA00,param,1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/
      SetValue(IN_BDATA00,param,stack9);
    	 stack9 = SetValue(IN_TESTSEL,param,0);
      stack10 = SetValue(IN_BDATA00,param,1);
      stack11 = SetValue(IN_BDATA01,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER"); /*ファーム次第で変更する可能性あり*/
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER"); /*ファーム次第で変更する可能性あり*/
      SetValue(IN_TESTSEL,param,stack9);
      SetValue(IN_BDATA00,param,stack10);
      SetValue(IN_BDATA01,param,stack11); 
    }
  }

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    if ( mat==0 ) {
      SetValue(IN_AREA,param,0);
      if      ( cond0==0 || cond0==2) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);    
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0");
      if ((cond1 == 1)|(cond1 == 2)) {
        strcpy(test_name,"_010K");
        sprintf(test_item,"_%04dhr_FLP0",cond3);
        strcat(test_name,test_item);
      }
    } else if ( mat==1 ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( cond0==0 || cond0==2) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) strcpy(test_name,"_000K_before_FLI1");
      if ((cond1 == 1)|(cond1 == 2)) {
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_FLI1",cond3);
        strcat(test_name,test_item);
      }
    } else if ( mat==2 ) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( cond0==0 || cond0==2) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ( cond0==1) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2");
      if ((cond1 == 1)|(cond1 == 2)) {
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_FLI2",cond3);
        strcat(test_name,test_item);
      }
    }else if(mat = 11){
      SetValue(IN_AREA,param,1);
      if( cond0==0) {  /* Retention */
        SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
        SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);   
      }
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) strcpy(test_name,"_000K_before_EXT1");
      if ((cond1 == 1)|(cond1 == 2)) {
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_EXT1",cond3);
        strcat(test_name,test_item);
      }
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
//  SetValue(IN_EXTAL1,param,500);      /* Vth-Read f=2MHz   */
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count/4;
  /*********************/
	 if(mat <= 2){
  if ((area == 0)|(area == 1)) {
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,5);       /* WL-Even / BL-Even */
      SetValue(IN_BDATA02,param,1);
      vth_mode = 23;                                /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss;

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          2222   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        22  22  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            22  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **\n");
      printf("** AA    AA RR R    EE      AA    AA       22     **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      if ( cond0==0|| cond0==2) {
        printf("** WL:Odd / BL:Even                               **\n");
        printf("****************************************************\n");
        SetValue(IN_BDATA01,param,7);       /* WL-Odd / BL-Even  */
      } else if ( cond0==1) {
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
        SetValue(IN_BDATA01,param,6);       /* WL-Even / BL-Odd  */
      }
      SetValue(IN_BDATA02,param,2);
      vth_mode = 23;                                /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss;

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }

  if ((area == 0)|(area == 2)) {
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      if ( cond0==0|| cond0==2) {
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
        SetValue(IN_BDATA01,param,6);       /* WL-Even / BL-Odd  */
      } else if ( cond0==1) {
        printf("** WL:Odd / BL:Even                               **\n");
        printf("****************************************************\n");
        SetValue(IN_BDATA01,param,7);       /* WL-Odd / BL-Even  */
      }
      SetValue(IN_BDATA02,param,3);
      vth_mode = 1;                                /* VthRead2          */
      min_vol = search_gain;           

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA             44  **\n");
      printf("**   AAAA   RR   RR EE        AAAA           444  **\n");
      printf("**   AAAA   RR   RR EE        AAAA          4 44  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **\n");
      printf("** AA    AA RR R    EE      AA    AA      4444444 **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      printf("** WL:Odd / BL:Odd                                **\n");
      printf("****************************************************\n");
      SetValue(IN_BDATA01,param,8);       /* WL-Odd / BL-Odd   */
      SetValue(IN_BDATA02,param,4);
      vth_mode = 1;                                /* VthRead2          */
      min_vol = search_gain;           

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }
	 }if(mat ==11){
       printf("**************************************************************\n");
       printf("**                                                          **\n");
       printf("**  EEEEEEE XX    XX TTTTTTTT RRRRR      AA            11   **\n");
       printf("**  EE       XX  XX     TT    RR   RR   AAAA          111   **\n");
       printf("**  EE        XXXX      TT    RR   RR   AAAA           11   **\n");
       printf("**  EEEEEEE    XX       TT    RRRRR    AA  AA    ===   11   **\n");
       printf("**  EE        XXXX      TT    RRR      AAAAAA    ===   11   **\n");
       printf("**  EE       XX  XX     TT    RR R    AA    AA         11   **\n");
       printf("**  EEEEEEE XX    XX    TT    RR  RRR AA    AA        1111  **\n");
       printf("**                                                          **\n");
       printf("**************************************************************\n");
       SetValue(IN_BDATA01,param,0);
	 	    if(area == 1){       /* VthRead1          */
         min_vol = 0;
         vth_mode = 23;
         SetValue(IN_BDATA02,param,2);
	 	    }
	 	    if(area == 2){       /* VthRead2          */
         SetValue(IN_BDATA02,param,3);

	 	    	 min_vol = search_gain;
	 	    	 vth_mode = 1;
	 	    }
       printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count*4,vth_mode);
       result = VthReadDist(vth_mode, bit_count*4, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
       if(PASS != result){
         sprintf(outbuf," Make Vth Bunpu Error!!!\n");
         DataOut(Ffpt,outbuf);
       }
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
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

/******************************************************************************/
/*!
  Original File -> Sinraisei_3.c
*/
/******************************************************************************/
int Ret_Distribution_3(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  char pe_temp[128];             /*                                    */

  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
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
      } 
      */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);   /* Sample No */
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);   /* Koutei */
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);   /* Test Item */
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);   /* Test Temp */
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);   /* P/E  Temp */
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);   /* FLP0 P/E Cycle */
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);   /* FLI1 P/E Cycle */
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);   /* FLI2 P/E Cycle */

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |\n"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |\n"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); area=2; break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |\n"); sprintf(test_temp,"_125C"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); sprintf(test_temp,"_90C") ; break;
        case  60: printf("|  Test_Temp =  60C       |\n"); sprintf(test_temp,"_60C") ; break;
        case  25: printf("|  Test_Temp =  25C       |\n"); sprintf(test_temp,"_25C") ; break;
        case  40: printf("|  Test_Temp = -40C       |\n"); sprintf(test_temp,"_-40C"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); sprintf(test_temp,"_42C") ; break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); sprintf(pe_temp,"_125C") ; break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); sprintf(pe_temp,"_90C") ; break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); sprintf(pe_temp,"_60C") ; break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); sprintf(pe_temp,"_25C") ; break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); sprintf(pe_temp,"_-40C"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("\n");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) { 
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Ret-1st After Chip    !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  cond3 = 0; //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

/**************************************************************************************/
/***   Extra4(Test Information) ReWrite                                             ***/
/**************************************************************************************/
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA); //Ret1st Code Write
        SetValue(IN_BDATA02,param,cond0);
        SetValue(IN_BDATA03,param,cond2);
        SetValue(IN_BDATA04,param,cond5);
        SetValue(IN_BDATA05,param,cyc_flp0);
        SetValue(IN_BDATA06,param,cyc_fli1);
        SetValue(IN_BDATA07,param,cyc_fli2);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
        SetValue(IN_LDATA0 ,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
        SetValue(IN_BDATA04,param,0);
        SetValue(IN_BDATA05,param,0);
        SetValue(IN_BDATA06,param,0);
        SetValue(IN_BDATA07,param,0);
      }
  }

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) | (cond1==1) ) { //Before&1st
    vs[0] = 1600; //vcc=1.6V setting
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP); //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);   
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP); //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP); //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300; //vcc=3.3V
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

      // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLP0",cyc_flp0,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_010K");
        sprintf(test_item,"_%04dhr_FLP0",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI1",cyc_fli1,cond3);
      }

      /*
      if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_001K");
        sprintf(test_item,"_%04dhr_FLI1",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI2",cyc_fli2,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_FLI2",cond3);
        strcat(test_name,test_item);
      }
	  */
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count/2; //ALL -> Col or Row
  /*********************/
  if(mat <= 2){ //FLP0 & FLI1 & FLI2
  if ((area == 0)|(area == 1)) { //Retention [Loss]
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even */
      SetValue(IN_BDATA02,param,1); /*Log Headder*/
      vth_mode = 23;                      /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          2222   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        22  22  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            22  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **\n");
      printf("** AA    AA RR R    EE      AA    AA       22     **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,2);
      vth_mode = 23;                        /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }

  if ((area == 0)|(area == 2)) {  //Retention [Gain]
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even  */
      SetValue(IN_BDATA02,param,3); /*Log Headder*/
      vth_mode = 1;                         /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA             44  **\n");
      printf("**   AAAA   RR   RR EE        AAAA           444  **\n");
      printf("**   AAAA   RR   RR EE        AAAA          4 44  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **\n");
      printf("** AA    AA RR R    EE      AA    AA      4444444 **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      printf("** WL:Odd / BL:Odd                                **\n");
      printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,4);
      vth_mode = 1;                       /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
    }
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
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

/******************************************************************************/
/*!
  @file RC04EXslc_Sinraisei_3.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Ret_Distribution_256KB(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  char pe_temp[128];             /*                                    */

  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
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
      } 
      */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);   /* Sample No */
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);   /* Koutei */
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);   /* Test Item */
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);   /* Test Temp */
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);   /* P/E  Temp */
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);   /* FLP0 P/E Cycle */
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);   /* FLI1 P/E Cycle */
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);   /* FLI2 P/E Cycle */

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |\n"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |\n"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); area=2; break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |\n"); sprintf(test_temp,"_125C"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); sprintf(test_temp,"_90C") ; break;
        case  60: printf("|  Test_Temp =  60C       |\n"); sprintf(test_temp,"_60C") ; break;
        case  25: printf("|  Test_Temp =  25C       |\n"); sprintf(test_temp,"_25C") ; break;
        case  40: printf("|  Test_Temp = -40C       |\n"); sprintf(test_temp,"_-40C"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); sprintf(test_temp,"_42C") ; break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); sprintf(pe_temp,"_125C") ; break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); sprintf(pe_temp,"_90C") ; break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); sprintf(pe_temp,"_60C") ; break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); sprintf(pe_temp,"_25C") ; break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); sprintf(pe_temp,"_-40C"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("\n");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) { 
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Ret-1st After Chip    !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  cond3 = 0; //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

/**************************************************************************************/
/***   Extra4(Test Information) ReWrite                                             ***/
/**************************************************************************************/
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA); //Ret1st Code Write
        SetValue(IN_BDATA02,param,cond0);
        SetValue(IN_BDATA03,param,cond2);
        SetValue(IN_BDATA04,param,cond5);
        SetValue(IN_BDATA05,param,cyc_flp0);
        SetValue(IN_BDATA06,param,cyc_fli1);
        SetValue(IN_BDATA07,param,cyc_fli2);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
        SetValue(IN_LDATA0 ,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
        SetValue(IN_BDATA04,param,0);
        SetValue(IN_BDATA05,param,0);
        SetValue(IN_BDATA06,param,0);
        SetValue(IN_BDATA07,param,0);
      }
  }

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) | (cond1==1) ) { //Before&1st
    vs[0] = 1600; //vcc=1.6V setting
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP); //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);   
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP); //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP); //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300; //vcc=3.3V
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

      // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLP0",cyc_flp0,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_010K");
        sprintf(test_item,"_%04dhr_FLP0",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI1",cyc_fli1,cond3);
      }

      /*
      if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_001K");
        sprintf(test_item,"_%04dhr_FLI1",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI2",cyc_fli2,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_FLI2",cond3);
        strcat(test_name,test_item);
      }
	  */
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count/2; //ALL -> Col or Row
  /*********************/
  if(mat <= 2){ //FLP0 & FLI1 & FLI2
  if ((area == 0)|(area == 1)) { //Retention [Loss]
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even */
      SetValue(IN_BDATA02,param,1); /*Log Headder*/
      vth_mode = 23;                      /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          2222   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        22  22  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            22  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **\n");
      printf("** AA    AA RR R    EE      AA    AA       22     **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,2);
      vth_mode = 23;                        /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }

  if ((area == 0)|(area == 2)) {  //Retention [Gain]
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even  */
      SetValue(IN_BDATA02,param,3); /*Log Headder*/
      vth_mode = 1;                         /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA             44  **\n");
      printf("**   AAAA   RR   RR EE        AAAA           444  **\n");
      printf("**   AAAA   RR   RR EE        AAAA          4 44  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **\n");
      printf("** AA    AA RR R    EE      AA    AA      4444444 **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      printf("** WL:Odd / BL:Odd                                **\n");
      printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,4);
      vth_mode = 1;                       /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
    }
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
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

/******************************************************************************/
/*!
  @file RC04EXslc_Sinraisei_3.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Ret_Distribution_All01(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  char pe_temp[128];             /*                                    */

  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
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
      } 
      */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);   /* Sample No */
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);   /* Koutei */
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);   /* Test Item */
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);   /* Test Temp */
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);   /* P/E  Temp */
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);   /* FLP0 P/E Cycle */
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);   /* FLI1 P/E Cycle */
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);   /* FLI2 P/E Cycle */

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |\n"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |\n"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); area=2; break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |\n"); sprintf(test_temp,"_125C"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); sprintf(test_temp,"_90C") ; break;
        case  60: printf("|  Test_Temp =  60C       |\n"); sprintf(test_temp,"_60C") ; break;
        case  25: printf("|  Test_Temp =  25C       |\n"); sprintf(test_temp,"_25C") ; break;
        case  40: printf("|  Test_Temp = -40C       |\n"); sprintf(test_temp,"_-40C"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); sprintf(test_temp,"_42C") ; break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); sprintf(pe_temp,"_125C") ; break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); sprintf(pe_temp,"_90C") ; break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); sprintf(pe_temp,"_60C") ; break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); sprintf(pe_temp,"_25C") ; break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); sprintf(pe_temp,"_-40C"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("\n");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) { 
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Ret-1st After Chip    !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  cond3 = 0; //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

/**************************************************************************************/
/***   Extra4(Test Information) ReWrite                                             ***/
/**************************************************************************************/
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA); //Ret1st Code Write
        SetValue(IN_BDATA02,param,cond0);
        SetValue(IN_BDATA03,param,cond2);
        SetValue(IN_BDATA04,param,cond5);
        SetValue(IN_BDATA05,param,cyc_flp0);
        SetValue(IN_BDATA06,param,cyc_fli1);
        SetValue(IN_BDATA07,param,cyc_fli2);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
        SetValue(IN_LDATA0 ,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
        SetValue(IN_BDATA04,param,0);
        SetValue(IN_BDATA05,param,0);
        SetValue(IN_BDATA06,param,0);
        SetValue(IN_BDATA07,param,0);
      }
  }

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) | (cond1==1) ) { //Before&1st
    vs[0] = 1600; //vcc=1.6V setting
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP); //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);   
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP); //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP); //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300; //vcc=3.3V
    if ( mat==0 ) { //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

      // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLP0",cyc_flp0,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_010K");
        sprintf(test_item,"_%04dhr_FLP0",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==1 ) { //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);  
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI1",cyc_fli1,cond3);
      }

      /*
      if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_001K");
        sprintf(test_item,"_%04dhr_FLI1",cond3);
        strcat(test_name,test_item);
      }
	  */
    } else if ( mat==2 ) { //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2); //FLP0=10K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI2",cyc_fli2,cond3);
      }

	  /*
      if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K
      if ((cond1 == 1)|(cond1 == 2)) { //Before&1st
        strcpy(test_name,"_000K");
        sprintf(test_item,"_%04dhr_FLI2",cond3);
        strcat(test_name,test_item);
      }
	  */
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count; //ALL -> Col or Row
  /*********************/
  if(mat <= 2){ //FLP0 & FLI1 & FLI2
  if ((area == 0)|(area == 1)) { //Retention [Loss]
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,0);       /* All */
//      SetValue(IN_BDATA01,param,1);       /* WL-Even */
      SetValue(IN_BDATA02,param,1); /*Log Headder*/
      vth_mode = 23;                      /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

/*
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          2222   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        22  22  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            22  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **\n");
      printf("** AA    AA RR R    EE      AA    AA       22     **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       // WL-Odd //
      SetValue(IN_BDATA02,param,2);
      vth_mode = 23;                        // VthRead2&3(0~2.5V)  //
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
*/
  }

  if ((area == 0)|(area == 2)) {  //Retention [Gain]
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,0);       /* WL-Even  */
//      SetValue(IN_BDATA01,param,1);       /* WL-Even  */
      SetValue(IN_BDATA02,param,3); /*Log Headder*/
      vth_mode = 1;                         /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

/*      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA             44  **\n");
      printf("**   AAAA   RR   RR EE        AAAA           444  **\n");
      printf("**   AAAA   RR   RR EE        AAAA          4 44  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **\n");
      printf("** AA    AA RR R    EE      AA    AA      4444444 **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      printf("** WL:Odd / BL:Odd                                **\n");
      printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       // WL-Odd //
      SetValue(IN_BDATA02,param,4);
      vth_mode = 1;                       // VthRead2          //
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
*/
    }
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
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

/******************************************************************************/
/*!
  @file RC04EXslc_Sinraisei_4kb.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Ret_Distribution_multi_area_rowst(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,k,m,p,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,cond6,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain,selsizew,snhyaku;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char test_item2[128];             /*                                    */
  int selsizec,selsizecinit,selsizemax,selmat,repeatcyc,matnum;             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  char pe_temp[128];             /*                                    */
  int matcyc[32];
	
  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
        DeviceSpecificPowerUp();

        SetValue(IN_REG,param,CHIPDATA);
        SetValue(IN_TESTSEL,param,0x00);
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) {
          SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
//          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
//          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
//          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
//          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
//          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
//          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
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
      } 
      */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);   /* Sample No */
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);   /* Koutei */
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);   /* Test Item */
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);   /* Test Temp */
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);   /* P/E  Temp */
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);   /* FLP0 P/E Cycle */
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);   /* FLI1 P/E Cycle */
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);   /* FLI2 P/E Cycle */

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |\n"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |\n"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); area=2; break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |\n"); sprintf(test_temp,"_125C"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); sprintf(test_temp,"_90C") ; break;
        case  60: printf("|  Test_Temp =  60C       |\n"); sprintf(test_temp,"_60C") ; break;
        case  25: printf("|  Test_Temp =  25C       |\n"); sprintf(test_temp,"_25C") ; break;
        case  40: printf("|  Test_Temp = -40C       |\n"); sprintf(test_temp,"_-40C"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); sprintf(test_temp,"_42C") ; break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); sprintf(pe_temp,"_125C") ; break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); sprintf(pe_temp,"_90C") ; break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); sprintf(pe_temp,"_60C") ; break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); sprintf(pe_temp,"_25C") ; break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); sprintf(pe_temp,"_-40C"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("\n");

/*
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
*/


    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}

    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}

    	
  printf("*** Common Start Address setting!! [ ex: 0x20000]    ->"); DigitInput(&selsizew);
//  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); selsizemax=0xFFFF;//DigitInput(&selsizemax);
  printf("*** AreaNo setting!! [ ex: 1] SampleName 200X X->input   ->"); DigitInput(&snhyaku); //SampleName 200X X->input
  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); DigitInput(&selsizemax);
	selsizec=selsizemax;
	
//  printf("*** Test Area setting!!            [ ex: 0xFFF (4KB)]->");  //DigitInput(&selsizec);
  printf("*** Test Area xxKB??               [ ex: 4KB->4, 64KB->64     ]->"); DigitInput(&matnum);
  printf("*** Repeat cyc number!! max128     [ ex: 5cyc:5     ]->\n"); repeatcyc=0;//DigitInput(&repeatcyc);
  printf("*** Test Mat  setting!!            [ FLP0:0, FLI1:1, FLI2:2, FLP0&FLI1:3, FLI1&FLI2:4, FLP0&FLI2:5, FLP0&FLI1&FLI2:6 ->"); DigitInput(&selmat);
      printf("\n");
      printf("*** Start Address :H'%X \n",selsizew);
      printf("*** Test  MaxArea :H'%X \n",selsizemax);
      printf("*** Test  Area    :H'%X \n",selsizec);
      printf("*** Test  Area(KB):%dKB \n",matnum);
      printf("*** Repeat cyc    :%dcyc\n",repeatcyc);
      if(selmat==0) printf("*** Test  Mat     :FLP0\n");
      else if(selmat==1) printf("*** Test  Mat     :FLI1\n");
      else if(selmat==2) printf("*** Test  Mat     :FLI2\n");
      else if(selmat==3) printf("*** Test  Mat     :FLP0&FLI1\n");
      else if(selmat==4) printf("*** Test  Mat     :FLI1&FLI2\n");
      else if(selmat==5) printf("*** Test  Mat     :FLP0&FLI2\n");
      else if(selmat==6) printf("*** Test  Mat     :FLP0&FLI1&FLI2  %d\n",selmat);
      else printf("*** Test  Mat     :Not setting!!\n");
      printf("*** Setting Address Check!! ***\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
    	
       printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) { 
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Ret-1st After Chip    !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  cond3 = 0; //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

/**************************************************************************************/
/***   Extra4(Test Information) ReWrite                                             ***/
/**************************************************************************************/
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA); //Ret1st Code Write
        SetValue(IN_BDATA02,param,cond0);
        SetValue(IN_BDATA03,param,cond2);
        SetValue(IN_BDATA04,param,cond5);
        SetValue(IN_BDATA05,param,cyc_flp0);
        SetValue(IN_BDATA06,param,cyc_fli1);
        SetValue(IN_BDATA07,param,cyc_fli2);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
        SetValue(IN_LDATA0 ,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
        SetValue(IN_BDATA04,param,0);
        SetValue(IN_BDATA05,param,0);
        SetValue(IN_BDATA06,param,0);
        SetValue(IN_BDATA07,param,0);
      }
  }

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) || (cond1==1) ) { //Before&1st
    vs[0] = 1600; //vcc=1.6V setting
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);

    m=0;
  	for( m=0; m<3; m++ ){    
  if(m==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) { //FLP0
 //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
    //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,selsizew); //FLP0
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1); //xxKB
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);   
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }//if m=0 

  if(m==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) { //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,selsizew); //FLI1
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1); //xxKB
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }//if m=1 

  if(m==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) { //FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,selsizew); //FLI2
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1); //xxKB
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
   }//if m=2 
  }//for 
 }//Test Data Write 

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300; //vcc=3.3V

	selsizecinit = selsizec;
  for( k=0; k<=repeatcyc; k++ ){    

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%02d%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],k,info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d%01d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],snhyaku);

  	
  	p=0;
//  	p=2;
//  	for( p=0; p<3; p++ ){    
  	matcyc[k]=k;
  	printf("******k=%d********************************************\n",k);
    printf("******k=%d********************************************\n",k);
    printf("******matcyc[k]=%d********************************************\n",matcyc[k]);
    printf("******ppppppppp=%d********************************************\n",p);
//  if(p==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) { //FLP0
 //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1); //xxKB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0_%03dKB_rowst_mat%03d",cyc_flp0,matnum,matcyc[k]); //FLP0=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLP0_%03dKB_rowst_mat%03d",cyc_flp0,cond3,matnum,matcyc[k]); //FLP0=10K xxKB-mat[1]
      }
    }
//  }//if p=0 

  	
//  if(p==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) { //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1); //xxKB
      printf("***CHECK111111*************************\n");
      printf("***CHECK111111*************************\n");
      printf("***CHECK111111*************************\n");
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);  
      printf("***CHECK22222*************************\n");
      printf("***CHECK22222*************************\n");
      printf("***CHECK22222*************************\n");
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1_%03dKB_rowst_mat%03d",cyc_fli1,matnum,matcyc[k]); //FLI1=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI1_%03dKB_rowst_mat%03d",cyc_fli1,cond3,matnum,matcyc[k]); //FLI1=10K xxKB-mat[1]
      }
    }
//  }//if p=1 

  	
//  if(p==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) { //FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1); //4KB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2_%03dKB_rowst_mat%03d",cyc_fli2,matnum,matcyc[k]); //FLI2=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI2_%03dKB_rowst_mat%03d",cyc_fli2,cond3,matnum,matcyc[k]); //FLI2=10K xxKB-mat[1]
      }
    }
//  }//if p=2 

  	
    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count/2; //ALL -> Col or Row
  /*********************/
  if(selmat <= 6){ //FLP0 & FLI1 & FLI2
  if ((area == 0)||(area == 1)) { //Retention [Loss]
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even */
      SetValue(IN_BDATA02,param,1); /*Log Headder*/
      vth_mode = 23;                      /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          2222   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        22  22  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            22  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **\n");
      printf("** AA    AA RR R    EE      AA    AA       22     **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,2);
      vth_mode = 23;                        /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }

  if ((area == 0)||(area == 2)) {  //Retention [Gain]
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,1);       /* WL-Even  */
      SetValue(IN_BDATA02,param,3); /*Log Headder*/
      vth_mode = 1;                         /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA             44  **\n");
      printf("**   AAAA   RR   RR EE        AAAA           444  **\n");
      printf("**   AAAA   RR   RR EE        AAAA          4 44  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **\n");
      printf("** AA    AA RR R    EE      AA    AA      4444444 **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
      printf("** WL:Odd / BL:Odd                                **\n");
      printf("****************************************************\n");
      SetValue(IN_BDATA01,param,2);       /* WL-Odd */
      SetValue(IN_BDATA02,param,4);
      vth_mode = 1;                       /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu .csv Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
    }
  }
// }//for p
  	selsizew = selsizew + selsizec + 0x01;
//  	selsizew = selsizec+0x01;
//    selsizec = selsizecinit;
}//for k repeatcyc

	
	/*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

/******************************************************************************/
/*!
  @file RC04EXslc_Sinraisei_4kb.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int Ret_Distribution_multi_area_all01(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,k,m,p,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,cond6,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain,selsizew,snhyaku;
  int etlr_25;
  int info[32];                    /* wmat chip information              */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
  char test_item2[128];             /*                                    */
  int selsizec,selsizecinit,selsizemax,selmat,repeatcyc,matnum;             /*                                    */
  char vthread_i[128];             /*                                    */
  char test_temp[128];             /*                                    */
  char pe_temp[128];             /*                                    */
  int matcyc[32];
	
  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
      /*
      if ( PASS == OpensShorts() ) {
      */
        DeviceSpecificPowerUp();

        SetValue(IN_REG,param,CHIPDATA);
        SetValue(IN_TESTSEL,param,0x00);
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) {
          SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
//          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
//          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
//          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
//          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
//          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
//          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
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
      } 
      */

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);   /* Sample No */
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);   /* Koutei */
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);   /* Test Item */
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);   /* Test Temp */
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);   /* P/E  Temp */
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);   /* FLP0 P/E Cycle */
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);   /* FLI1 P/E Cycle */
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);   /* FLI2 P/E Cycle */

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

      printf("\n");
      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |\n"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |\n"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); area=2; break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |\n"); sprintf(test_temp,"_125C"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); sprintf(test_temp,"_90C") ; break;
        case  60: printf("|  Test_Temp =  60C       |\n"); sprintf(test_temp,"_60C") ; break;
        case  25: printf("|  Test_Temp =  25C       |\n"); sprintf(test_temp,"_25C") ; break;
        case  40: printf("|  Test_Temp = -40C       |\n"); sprintf(test_temp,"_-40C"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); sprintf(test_temp,"_42C") ; break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); sprintf(pe_temp,"_125C") ; break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); sprintf(pe_temp,"_90C") ; break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); sprintf(pe_temp,"_60C") ; break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); sprintf(pe_temp,"_25C") ; break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); sprintf(pe_temp,"_-40C"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

/**************************************************************************************/
/***   Condition Select                                                             ***/
/**************************************************************************************/
  printf("\n");

/*
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
*/


    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}

    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Select Mat NG         !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}

    	
  printf("*** Common Start Address setting!! [ ex: 0x20000]    ->"); DigitInput(&selsizew);
//  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); selsizemax=0xFFFF;//DigitInput(&selsizemax);
  printf("*** AreaNo setting!! [ ex:SampleName 200X X->input]   ->"); DigitInput(&snhyaku); //SampleName 200X X->input
  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); DigitInput(&selsizemax);
	selsizec=selsizemax;
	
//  printf("*** Test Area setting!!            [ ex: 0xFFF (4KB)]->");  //DigitInput(&selsizec);
  printf("*** Test Area xxKB??               [ ex: 4KB->4, 64KB->64     ]->"); DigitInput(&matnum);
  printf("*** Repeat cyc number!! max128     [ ex: 5cyc:5     ]->\n"); repeatcyc=0;//DigitInput(&repeatcyc);
  printf("*** Test Mat  setting!!            [ FLP0:0, FLI1:1, FLI2:2, FLP0&FLI1:3, FLI1&FLI2:4, FLP0&FLI2:5, FLP0&FLI1&FLI2:6 ->"); DigitInput(&selmat);
      printf("\n");
      printf("*** Start Address :H'%X \n",selsizew);
      printf("*** Test  MaxArea :H'%X \n",selsizemax);
      printf("*** Test  Area    :H'%X \n",selsizec);
      printf("*** Test  Area(KB):%dKB \n",matnum);
      printf("*** Repeat cyc    :%dcyc\n",repeatcyc);
      if(selmat==0) printf("*** Test  Mat     :FLP0\n");
      else if(selmat==1) printf("*** Test  Mat     :FLI1\n");
      else if(selmat==2) printf("*** Test  Mat     :FLI2\n");
      else if(selmat==3) printf("*** Test  Mat     :FLP0&FLI1\n");
      else if(selmat==4) printf("*** Test  Mat     :FLI1&FLI2\n");
      else if(selmat==5) printf("*** Test  Mat     :FLP0&FLI2\n");
      else if(selmat==6) printf("*** Test  Mat     :FLP0&FLI1&FLI2  %d\n",selmat);
      else printf("*** Test  Mat     :Not setting!!\n");
      printf("*** Setting Address Check!! ***\n");
      printf("************************\n");
      printf("***  Test Run OK?   ****\n");
      printf("************************\n");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
    	
       printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) { 
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!!  Ret-1st After Chip    !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return(PASS);
	}
  }
  cond3 = 0; //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

/**************************************************************************************/
/***   Extra4(Test Information) ReWrite                                             ***/
/**************************************************************************************/
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA); //Ret1st Code Write
        SetValue(IN_BDATA02,param,cond0);
        SetValue(IN_BDATA03,param,cond2);
        SetValue(IN_BDATA04,param,cond5);
        SetValue(IN_BDATA05,param,cyc_flp0);
        SetValue(IN_BDATA06,param,cyc_fli1);
        SetValue(IN_BDATA07,param,cyc_fli2);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
        SetValue(IN_LDATA0 ,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
        SetValue(IN_BDATA04,param,0);
        SetValue(IN_BDATA05,param,0);
        SetValue(IN_BDATA06,param,0);
        SetValue(IN_BDATA07,param,0);
      }
  }

/**************************************************************************************/
/***   Test Data Write                                                          ***/
/**************************************************************************************/
  if ( (cond1==0) || (cond1==1) ) { //Before&1st
    vs[0] = 1600; //vcc=1.6V setting
    printf("\n");
    printf("******************************\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** !!! Write Start OK?    ***\n");
    printf("*** set vcc %dmV         ***\n",vs[0]);
    printf("******************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);

    m=0;
  	for( m=0; m<3; m++ ){    
  if(m==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) { //FLP0
 //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
    //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,selsizew); //FLP0
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1); //xxKB
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);   
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }//if m=0 

  if(m==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) { //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,selsizew); //FLI1
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1); //xxKB
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);  
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
  }//if m=1 

  if(m==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) { //FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,selsizew); //FLI2
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1); //xxKB
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/

      if      ((cond0==0x00) || (cond0==0x01)) {  /* Retention */
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);    
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*ファーム次第で変更する可能性あり*/
    }
   }//if m=2 
  }//for 
 }//Test Data Write 

/**************************************************************************************/
/***   Vth-Read Function                                                            ***/
/**************************************************************************************/
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300; //vcc=3.3V

	selsizecinit = selsizec;
  for( k=0; k<=repeatcyc; k++ ){    

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%02d%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],k,info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d%01d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],snhyaku);

  	
//  	p=0;
//  	p=2;
//  	for( p=0; p<3; p++ ){    
  	matcyc[k]=k;
  	printf("******k=%d********************************************\n",k);
    printf("******k=%d********************************************\n",k);
    printf("******matcyc[k]=%d********************************************\n",matcyc[k]);
    printf("******ppppppppp=%d********************************************\n",p);
//  if(p==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) { //FLP0
 //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1); //xxKB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);  
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0_%03dKB_all01_mat%03d",cyc_flp0,matnum,matcyc[k]); //FLP0=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLP0_%03dKB_all01_mat%03d",cyc_flp0,cond3,matnum,matcyc[k]); //FLP0=10K xxKB-mat[1]
      }
    }
//  }//if p=0 

  	
//  if(p==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) { //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1); //xxKB
      printf("***CHECK111111*************************\n");
      printf("***CHECK111111*************************\n");
      printf("***CHECK111111*************************\n");
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);  
      printf("***CHECK22222*************************\n");
      printf("***CHECK22222*************************\n");
      printf("***CHECK22222*************************\n");
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1_%03dKB_all01_mat%03d",cyc_fli1,matnum,matcyc[k]); //FLI1=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI1_%03dKB_all01_mat%03d",cyc_fli1,cond3,matnum,matcyc[k]); //FLI1=10K xxKB-mat[1]
      }
    }
//  }//if p=1 

  	
//  if(p==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) { //FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************\n");
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      printf("****** selmat=%d**************************\n",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {  /* Retention or RDT*/
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1); //4KB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {  /* Disturb   */
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2_%03dKB_all01_mat%03d",cyc_fli2,matnum,matcyc[k]); //FLI2=10K xxKB-mat[i]
      if ((cond1 == 1)||(cond1 == 2)) { //Before&1st
        sprintf(test_name,"_%03dK_%04dhr_FLI2_%03dKB_all01_mat%03d",cyc_fli2,cond3,matnum,matcyc[k]); //FLI2=10K xxKB-mat[1]
      }
    }
//  }//if p=2 

  	
    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
  /*  Common Setting   */
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;                        /* VS-Step Default   */
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
  bit_count = bit_count; //Col or Row -> ALL
  /*********************/
  if(selmat <= 6){ //FLP0 & FLI1 & FLI2
  if ((area == 0)||(area == 1)) { //Retention [Loss]
      printf("**************************************************\n");
      printf("**                                              **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          11   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        111   **\n");
      printf("**   AAAA   RR   RR EE        AAAA         11   **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **\n");
      printf("** AA    AA RR R    EE      AA    AA       11   **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **\n");
      printf("**                                              **\n");
      printf("**************************************************\n");
      printf("** WL:Even / BL:Even                            **\n");
      printf("**************************************************\n");
      SetValue(IN_BDATA01,param,0);       /* All */
      SetValue(IN_BDATA02,param,1); /*Log Headder*/
      vth_mode = 23;                      /* VthRead2&3(0~2.5V)  */
      min_vol = search_loss; //min_vol=0.5V @Before & 1st

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
  }

  if ((area == 0)||(area == 2)) {  //Retention [Gain]
      printf("****************************************************\n");
      printf("**                                                **\n");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **\n");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **\n");
      printf("**   AAAA   RR   RR EE        AAAA            33  **\n");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **\n");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **\n");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **\n");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **\n");
      printf("**                                                **\n");
      printf("****************************************************\n");
        printf("** WL:Even / BL:Odd                               **\n");
        printf("****************************************************\n");
      SetValue(IN_BDATA01,param,0);       /* All  */
      SetValue(IN_BDATA02,param,3); /*Log Headder*/
      vth_mode = 1;                         /* VthRead2          */
      min_vol = search_gain; //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

    }
  }
// }//for p
  	selsizew = selsizew + selsizec + 0x01;
}//for k repeatcyc

	
	/*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
  /*********************/

}  

