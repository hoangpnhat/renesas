// ****************************************************************************
// ! @file RC04EXslc_Sinraisei.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Ret_Distribution(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];


  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }
// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("*** Test-Select [Ret:0    / Dis:1  / ReadDisturb:2] ->"); DigitInput(&cond0);
    if ( cond0==0 ) {
      printf("");
      printf("##############################");
      printf("### Retention Test         ###");
      printf("### Retention Test         ###");
      printf("### Retention Test         ###");
      printf("##############################");
      printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
	} else if ( cond0==1 ) {
      printf("");
      printf("==============================");
      printf("||| Disturb Test           |||");
      printf("||| Disturb Test           |||");
      printf("||| Disturb Test           |||");
      printf("==============================");
      printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
	} else if ( cond0==2 ) {
	  printf("");
	  printf("syoki ETLR00(90CTrim):0x%02X", GetValue(IN_ETLR00,param));
      printf("syoki ETLR01(90CTrim):0x%02X", GetValue(IN_ETLR01,param));
      SetValue(IN_ETLR00,param,Extra5_ETLR00[3]);
      //Set ETLR00 BGR25C Trim
      SetValue(IN_ETLR01,param,Extra5_ETLR01[3]);
      //Set ETLR01 BGR25C Trim
      printf("++++++++++++++++++++++++++++++");
      printf("+++ ReadDisturb Test       +++");
      printf("+++ ReadDisturb Test       +++");
      printf("+++ ReadDisturb Test       +++");
	  printf("Set25C ETLR00:0x%02X", GetValue(IN_ETLR00,param));
	  //BGRset Reg00 
      printf("++++++++++++++++++++++++++++++");
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
  if ( cond4 == 1 ) SetValue(IN_ETLR25,param,0xFD);
  // 2.5uA
  if ( cond4 == 2 ) SetValue(IN_ETLR25,param,0xFF);
  // 3.5uA
  sprintf(vthread_i,"_%X",GetValue(IN_ETLR25,param));

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20] = ReadEcrMode8(FBM_READTOP+0x20,SIZE_L);

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) | (cond1==1) ) {
    vs[0] = 1600;
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ( cond0==0) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE");
      // t@[ŕύX\
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ( cond0==0) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE");
      // t@[ŕύX\
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ( cond0==0) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP+(0x20000*st_addr));
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      ( cond0==0) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE");
      // t@[ŕύX\
      else if ( cond0==1) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }else if( mat==11 ){
      SetValue(IN_AREA,param,1);
      SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      stack9 = SetValue(IN_BDATA00,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\
      SetValue(IN_BDATA00,param,1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\
      SetValue(IN_BDATA00,param,stack9);
    	 stack9 = SetValue(IN_TESTSEL,param,0);
      stack10 = SetValue(IN_BDATA00,param,1);
      stack11 = SetValue(IN_BDATA01,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
      // t@[ŕύX\
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
      // t@[ŕύX\
      SetValue(IN_TESTSEL,param,stack9);
      SetValue(IN_BDATA00,param,stack10);
      SetValue(IN_BDATA01,param,stack11);
    }
  }

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    if ( mat==0 ) {
      SetValue(IN_AREA,param,0);
      if      ( cond0==0 || cond0==2) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
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
      if      ( cond0==0 || cond0==2) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
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
      if      ( cond0==0 || cond0==2) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ( cond0==1) {
      // Disturb
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
      if( cond0==0) {
      // Retention
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
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
//  SetValue(IN_EXTAL1,param,500);      // Vth-Read f=2MHz
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // WL-Even / BL-Even
      // VthRead2&3(0~2.5V)
        // WL-Odd / BL-Even
        // WL-Even / BL-Odd
      // VthRead2&3(0~2.5V)
        // WL-Even / BL-Odd
        // WL-Odd / BL-Even
      // VthRead2
      // WL-Odd / BL-Odd
      // VthRead2
	 	    // VthRead1
	 	    // VthRead2
  // Common Clear
  // *******************
// ****************************************************************************
// 1=ALL Pattern bit_count = bit_count/4;  if(mat <= 2){ if ((area == 0)|(area == 1)) { printf("**************************************************"); printf("**                                              **"); printf("**    AA    RRRRR   EEEEEEE    AA          11   **"); printf("**   AAAA   RR   RR EE        AAAA        111   **"); printf("**   AAAA   RR   RR EE        AAAA         11   **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **"); printf("** AA    AA RR R    EE      AA    AA       11   **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **"); printf("**                                              **"); printf("**************************************************"); printf("** WL:Even / BL:Even                            **"); printf("**************************************************"); SetValue(IN_BDATA01,param,5); SetValue(IN_BDATA02,param,1); vth_mode = 23; min_vol = search_loss;  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          2222   **"); printf("**   AAAA   RR   RR EE        AAAA        22  22  **"); printf("**   AAAA   RR   RR EE        AAAA            22  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **"); printf("** AA    AA RR R    EE      AA    AA       22     **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **"); printf("**                                                **"); printf("****************************************************"); if ( cond0==0|| cond0==2) { printf("** WL:Odd / BL:Even                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,7); } else if ( cond0==1) { printf("** WL:Even / BL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,6); } SetValue(IN_BDATA02,param,2); vth_mode = 23; min_vol = search_loss;  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } }  if ((area == 0)|(area == 2)) { printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          3333   **"); printf("**   AAAA   RR   RR EE        AAAA        33  33  **"); printf("**   AAAA   RR   RR EE        AAAA            33  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **"); printf("** AA    AA RR R    EE      AA    AA      33  33  **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **"); printf("**                                                **"); printf("****************************************************"); if ( cond0==0|| cond0==2) { printf("** WL:Even / BL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,6); } else if ( cond0==1) { printf("** WL:Odd / BL:Even                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,7); } SetValue(IN_BDATA02,param,3); vth_mode = 1; min_vol = search_gain;  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA             44  **"); printf("**   AAAA   RR   RR EE        AAAA           444  **"); printf("**   AAAA   RR   RR EE        AAAA          4 44  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **"); printf("** AA    AA RR R    EE      AA    AA      4444444 **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd / BL:Odd                                **"); printf("****************************************************"); SetValue(IN_BDATA01,param,8); SetValue(IN_BDATA02,param,4); vth_mode = 1; min_vol = search_gain;  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } } }if(mat ==11){ printf("**************************************************************"); printf("**                                                          **"); printf("**  EEEEEEE XX    XX TTTTTTTT RRRRR      AA            11   **"); printf("**  EE       XX  XX     TT    RR   RR   AAAA          111   **"); printf("**  EE        XXXX      TT    RR   RR   AAAA           11   **"); printf("**  EEEEEEE    XX       TT    RRRRR    AA  AA    ===   11   **"); printf("**  EE        XXXX      TT    RRR      AAAAAA    ===   11   **"); printf("**  EE       XX  XX     TT    RR R    AA    AA         11   **"); printf("**  EEEEEEE XX    XX    TT    RR  RRR AA    AA        1111  **"); printf("**                                                          **"); printf("**************************************************************"); SetValue(IN_BDATA01,param,0); if(area == 1){ min_vol = 0; vth_mode = 23; SetValue(IN_BDATA02,param,2); } if(area == 2){ SetValue(IN_BDATA02,param,3);  min_vol = search_gain; vth_mode = 1; } printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count*4,vth_mode); result = VthReadDist(vth_mode, bit_count*4, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name); if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } }  SetValue(IN_AREA,param,stack0); SetValue(IN_TOPADDR0 ,param,stack1); SetValue(IN_TESTSIZE0,param,stack2); SetValue(IN_TOPADDR1 ,param,stack3); SetValue(IN_TESTSIZE1,param,stack4); SetValue(IN_TOPADDR2 ,param,stack5); SetValue(IN_TESTSIZE2,param,stack6); SetValue(IN_EXTAL1,param,stack7); SetValue(IN_BDATA01,param,0); SetValue(IN_BDATA02,param,0); SetValue(IN_BDATA03,param,0); SetValue(IN_REG,param,stack8); SetValue(IN_ETLR25,param,etlr_25);   }   /*! Original File -> Sinraisei_3.c
// ****************************************************************************
int Ret_Distribution_3(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  char pe_temp[128];


  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);
      // Koutei
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);
      // Test Item
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);
      // Test Temp
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);
      // P/E  Temp
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);
      // FLP0 P/E Cycle
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);
      // FLI1 P/E Cycle
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);
      // FLI2 P/E Cycle

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("+=========================+");
      printf("|  Test Information       |");
      printf("+-------------------------+");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |"); break;
        default:   printf("|  Flag ???               |"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |"); area=2; break;
        case 0x30: printf("|  Endurance              |"); break;
        default:   printf("|  Test ???               |"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |"); s
        case  90: printf("|  Test_Temp =  90C       |"); s
        case  60: printf("|  Test_Temp =  60C       |"); s
        case  25: printf("|  Test_Temp =  25C       |"); s
        case  40: printf("|  Test_Temp = -40C       |"); s
        case  42: printf("|  Test_Temp =  42C       |"); s
        default:  printf("|  Test_Temp ???          |"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |"); s
        case  90: printf("|  P/E_Temp  =  90C       |"); s
        case  60: printf("|  P/E_Temp  =  60C       |"); s
        case  25: printf("|  P/E_Temp  =  25C       |"); s
        case  40: printf("|  P/E_Temp  = -40C       |"); s
        default:  printf("|  P/E_Temp  ???          |"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |"); break;
        case  1: printf("|  FLP0:  1K              |"); break;
        case 10: printf("|  FLP0: 10K              |"); break;
        default: printf("|  FLP0: ---              |"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |"); break;
        case  1: printf("|  FLI1:  1K              |"); break;
        case 10: printf("|  FLI1: 10K              |"); break;
        default: printf("|  FLI1: ---              |"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |"); break;
        case  1: printf("|  FLI2:  1K              |"); break;
        case 10: printf("|  FLI2: 10K              |"); break;
        default: printf("|  FLI2: ---              |"); break;
      }
      printf("+=========================+");

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Ret-1st After Chip    !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  cond3 = 0;
  //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

// ************************************************************************************
// **   Extra4(Test Information) ReWrite                                             **
// ************************************************************************************
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA);
        //Ret1st Code Write
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

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) | (cond1==1) ) {
  //Before&1st
    vs[0] = 1600;
    //vcc=1.6V setting
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    //vcc=3.3V
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

            // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_010K"); sprintf(test_item,"_%04dhr_FLP0",cond3); strcat(test_name,test_item); }
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_001K"); sprintf(test_item,"_%04dhr_FLI1",cond3); strcat(test_name,test_item); }
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_000K"); sprintf(test_item,"_%04dhr_FLI2",cond3); strcat(test_name,test_item); }
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // WL-Even
      // Log Headder
      // VthRead2&3(0~2.5V)
      // WL-Odd
      // VthRead2&3(0~2.5V)
      // WL-Even
      // Log Headder
      // VthRead2
      // WL-Odd
      // VthRead2
  // Common Clear
  // *******************
// ****************************************************************************
// 1=ALL Pattern bit_count = bit_count/2; //ALL -> Col or Row  if(mat <= 2){ //FLP0 & FLI1 & FLI2 if ((area == 0)|(area == 1)) { //Retention [Loss] printf("**************************************************"); printf("**                                              **"); printf("**    AA    RRRRR   EEEEEEE    AA          11   **"); printf("**   AAAA   RR   RR EE        AAAA        111   **"); printf("**   AAAA   RR   RR EE        AAAA         11   **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **"); printf("** AA    AA RR R    EE      AA    AA       11   **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **"); printf("**                                              **"); printf("**************************************************"); printf("** WL:Even / BL:Even                            **"); printf("**************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,1); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          2222   **"); printf("**   AAAA   RR   RR EE        AAAA        22  22  **"); printf("**   AAAA   RR   RR EE        AAAA            22  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **"); printf("** AA    AA RR R    EE      AA    AA       22     **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,2); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } }  if ((area == 0)|(area == 2)) { //Retention [Gain] printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          3333   **"); printf("**   AAAA   RR   RR EE        AAAA        33  33  **"); printf("**   AAAA   RR   RR EE        AAAA            33  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **"); printf("** AA    AA RR R    EE      AA    AA      33  33  **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Even / BL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,3); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA             44  **"); printf("**   AAAA   RR   RR EE        AAAA           444  **"); printf("**   AAAA   RR   RR EE        AAAA          4 44  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **"); printf("** AA    AA RR R    EE      AA    AA      4444444 **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd / BL:Odd                                **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,4); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } } }  SetValue(IN_AREA,param,stack0); SetValue(IN_TOPADDR0 ,param,stack1); SetValue(IN_TESTSIZE0,param,stack2); SetValue(IN_TOPADDR1 ,param,stack3); SetValue(IN_TESTSIZE1,param,stack4); SetValue(IN_TOPADDR2 ,param,stack5); SetValue(IN_TESTSIZE2,param,stack6); SetValue(IN_EXTAL1,param,stack7); SetValue(IN_BDATA01,param,0); SetValue(IN_BDATA02,param,0); SetValue(IN_BDATA03,param,0); SetValue(IN_REG,param,stack8); SetValue(IN_ETLR25,param,etlr_25);   }   /*! @file RC04EXslc_Sinraisei_3.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Ret_Distribution_256KB(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  char pe_temp[128];


  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);
      // Koutei
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);
      // Test Item
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);
      // Test Temp
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);
      // P/E  Temp
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);
      // FLP0 P/E Cycle
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);
      // FLI1 P/E Cycle
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);
      // FLI2 P/E Cycle

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("+=========================+");
      printf("|  Test Information       |");
      printf("+-------------------------+");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |"); break;
        default:   printf("|  Flag ???               |"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |"); area=2; break;
        case 0x30: printf("|  Endurance              |"); break;
        default:   printf("|  Test ???               |"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |"); s
        case  90: printf("|  Test_Temp =  90C       |"); s
        case  60: printf("|  Test_Temp =  60C       |"); s
        case  25: printf("|  Test_Temp =  25C       |"); s
        case  40: printf("|  Test_Temp = -40C       |"); s
        case  42: printf("|  Test_Temp =  42C       |"); s
        default:  printf("|  Test_Temp ???          |"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |"); s
        case  90: printf("|  P/E_Temp  =  90C       |"); s
        case  60: printf("|  P/E_Temp  =  60C       |"); s
        case  25: printf("|  P/E_Temp  =  25C       |"); s
        case  40: printf("|  P/E_Temp  = -40C       |"); s
        default:  printf("|  P/E_Temp  ???          |"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |"); break;
        case  1: printf("|  FLP0:  1K              |"); break;
        case 10: printf("|  FLP0: 10K              |"); break;
        default: printf("|  FLP0: ---              |"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |"); break;
        case  1: printf("|  FLI1:  1K              |"); break;
        case 10: printf("|  FLI1: 10K              |"); break;
        default: printf("|  FLI1: ---              |"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |"); break;
        case  1: printf("|  FLI2:  1K              |"); break;
        case 10: printf("|  FLI2: 10K              |"); break;
        default: printf("|  FLI2: ---              |"); break;
      }
      printf("+=========================+");

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Ret-1st After Chip    !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  cond3 = 0;
  //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

// ************************************************************************************
// **   Extra4(Test Information) ReWrite                                             **
// ************************************************************************************
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA);
        //Ret1st Code Write
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

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) | (cond1==1) ) {
  //Before&1st
    vs[0] = 1600;
    //vcc=1.6V setting
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    //vcc=3.3V
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

            // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_010K"); sprintf(test_item,"_%04dhr_FLP0",cond3); strcat(test_name,test_item); }
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_001K"); sprintf(test_item,"_%04dhr_FLI1",cond3); strcat(test_name,test_item); }
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_000K"); sprintf(test_item,"_%04dhr_FLI2",cond3); strcat(test_name,test_item); }
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // WL-Even
      // Log Headder
      // VthRead2&3(0~2.5V)
      // WL-Odd
      // VthRead2&3(0~2.5V)
      // WL-Even
      // Log Headder
      // VthRead2
      // WL-Odd
      // VthRead2
  // Common Clear
  // *******************
// ****************************************************************************
// 1=ALL Pattern bit_count = bit_count/2; //ALL -> Col or Row  if(mat <= 2){ //FLP0 & FLI1 & FLI2 if ((area == 0)|(area == 1)) { //Retention [Loss] printf("**************************************************"); printf("**                                              **"); printf("**    AA    RRRRR   EEEEEEE    AA          11   **"); printf("**   AAAA   RR   RR EE        AAAA        111   **"); printf("**   AAAA   RR   RR EE        AAAA         11   **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **"); printf("** AA    AA RR R    EE      AA    AA       11   **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **"); printf("**                                              **"); printf("**************************************************"); printf("** WL:Even / BL:Even                            **"); printf("**************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,1); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          2222   **"); printf("**   AAAA   RR   RR EE        AAAA        22  22  **"); printf("**   AAAA   RR   RR EE        AAAA            22  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **"); printf("** AA    AA RR R    EE      AA    AA       22     **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,2); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } }  if ((area == 0)|(area == 2)) { //Retention [Gain] printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          3333   **"); printf("**   AAAA   RR   RR EE        AAAA        33  33  **"); printf("**   AAAA   RR   RR EE        AAAA            33  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **"); printf("** AA    AA RR R    EE      AA    AA      33  33  **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Even / BL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,3); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA             44  **"); printf("**   AAAA   RR   RR EE        AAAA           444  **"); printf("**   AAAA   RR   RR EE        AAAA          4 44  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **"); printf("** AA    AA RR R    EE      AA    AA      4444444 **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd / BL:Odd                                **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,4); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } } }  SetValue(IN_AREA,param,stack0); SetValue(IN_TOPADDR0 ,param,stack1); SetValue(IN_TESTSIZE0,param,stack2); SetValue(IN_TOPADDR1 ,param,stack3); SetValue(IN_TESTSIZE1,param,stack4); SetValue(IN_TOPADDR2 ,param,stack5); SetValue(IN_TESTSIZE2,param,stack6); SetValue(IN_EXTAL1,param,stack7); SetValue(IN_BDATA01,param,0); SetValue(IN_BDATA02,param,0); SetValue(IN_BDATA03,param,0); SetValue(IN_REG,param,stack8); SetValue(IN_ETLR25,param,etlr_25);   }   /*! @file RC04EXslc_Sinraisei_3.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Ret_Distribution_All01(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  char pe_temp[128];


  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);
      // Koutei
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);
      // Test Item
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);
      // Test Temp
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);
      // P/E  Temp
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);
      // FLP0 P/E Cycle
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);
      // FLI1 P/E Cycle
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);
      // FLI2 P/E Cycle

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("+=========================+");
      printf("|  Test Information       |");
      printf("+-------------------------+");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |"); break;
        default:   printf("|  Flag ???               |"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |"); area=2; break;
        case 0x30: printf("|  Endurance              |"); break;
        default:   printf("|  Test ???               |"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |"); s
        case  90: printf("|  Test_Temp =  90C       |"); s
        case  60: printf("|  Test_Temp =  60C       |"); s
        case  25: printf("|  Test_Temp =  25C       |"); s
        case  40: printf("|  Test_Temp = -40C       |"); s
        case  42: printf("|  Test_Temp =  42C       |"); s
        default:  printf("|  Test_Temp ???          |"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |"); s
        case  90: printf("|  P/E_Temp  =  90C       |"); s
        case  60: printf("|  P/E_Temp  =  60C       |"); s
        case  25: printf("|  P/E_Temp  =  25C       |"); s
        case  40: printf("|  P/E_Temp  = -40C       |"); s
        default:  printf("|  P/E_Temp  ???          |"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |"); break;
        case  1: printf("|  FLP0:  1K              |"); break;
        case 10: printf("|  FLP0: 10K              |"); break;
        default: printf("|  FLP0: ---              |"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |"); break;
        case  1: printf("|  FLI1:  1K              |"); break;
        case 10: printf("|  FLI1: 10K              |"); break;
        default: printf("|  FLI1: ---              |"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |"); break;
        case  1: printf("|  FLI2:  1K              |"); break;
        case 10: printf("|  FLI2: 10K              |"); break;
        default: printf("|  FLI2: ---              |"); break;
      }
      printf("+=========================+");

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("");
  printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat);
  if ( mat == 0 ) {
    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 1 ) {
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  if ( mat == 2 ) {
    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
//  printf("*** Test Select    [Before:0 / 1st:1 ] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Ret-1st After Chip    !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  cond3 = 0;
  //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

// ************************************************************************************
// **   Extra4(Test Information) ReWrite                                             **
// ************************************************************************************
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA);
        //Ret1st Code Write
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

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) | (cond1==1) ) {
  //Before&1st
    vs[0] = 1600;
    //vcc=1.6V setting
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      //FLP0
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      //FLI1
      SetValue(IN_TESTSIZE1,param,0x100);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      //FLI2
      SetValue(IN_TESTSIZE2,param,0x100);
      SetValue(IN_ETCR08,param,0x20);
      //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);
      //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    //vcc=3.3V
    if ( mat==0 ) {
    //FLP0
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

            // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0",cyc_flp0);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_010K_before_FLP0"); //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_010K"); sprintf(test_item,"_%04dhr_FLP0",cond3); strcat(test_name,test_item); }
    } else if ( mat==1 ) {
    //FLI1
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);

	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1",cyc_fli1);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_001K_before_FLI1"); //FLI1=1K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_001K"); sprintf(test_item,"_%04dhr_FLI1",cond3); strcat(test_name,test_item); }
    } else if ( mat==2 ) {
    //FLI2
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
	  	  // morioka Add
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2",cyc_fli2);
      //FLP0=10K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st s
      }

	  // if (cond1 == 0) strcpy(test_name,"_000K_before_FLI2"); //FLI2=0K if ((cond1 == 1)|(cond1 == 2)) { //Before&1st strcpy(test_name,"_000K"); sprintf(test_item,"_%04dhr_FLI2",cond3); strcat(test_name,test_item); }
    }

    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // All
//      SetValue(IN_BDATA01,param,1);   // WL-Even
      // Log Headder
      // VthRead2&3(0~2.5V)
// 1=ALL Pattern bit_count = bit_count; //ALL -> Col or Row  if(mat <= 2){ //FLP0 & FLI1 & FLI2 if ((area == 0)|(area == 1)) { //Retention [Loss] printf("**************************************************"); printf("**                                              **"); printf("**    AA    RRRRR   EEEEEEE    AA          11   **"); printf("**   AAAA   RR   RR EE        AAAA        111   **"); printf("**   AAAA   RR   RR EE        AAAA         11   **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **"); printf("** AA    AA RR R    EE      AA    AA       11   **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **"); printf("**                                              **"); printf("**************************************************"); printf("** WL:Even / BL:Even                            **"); printf("**************************************************"); SetValue(IN_BDATA01,param,0);  SetValue(IN_BDATA02,param,1); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  /* printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          2222   **"); printf("**   AAAA   RR   RR EE        AAAA        22  22  **"); printf("**   AAAA   RR   RR EE        AAAA            22  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **"); printf("** AA    AA RR R    EE      AA    AA       22     **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); // WL-Odd // SetValue(IN_BDATA02,param,2); vth_mode = 23; // VthRead2&3(0~2.5V)  // min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }
  }

  if ((area == 0)|(area == 2)) {
  //Retention [Gain]
      printf("****************************************************");
      printf("**                                                **");
      printf("**    AA    RRRRR   EEEEEEE    AA          3333   **");
      printf("**   AAAA   RR   RR EE        AAAA        33  33  **");
      printf("**   AAAA   RR   RR EE        AAAA            33  **");
      printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **");
      printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **");
      printf("** AA    AA RR R    EE      AA    AA      33  33  **");
      printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **");
      printf("**                                                **");
      printf("****************************************************");
        printf("** WL:Even / BL:Odd                               **");
        printf("****************************************************");
      SetValue(IN_BDATA01,param,0);
      // WL-Even
//      SetValue(IN_BDATA01,param,1);   // WL-Even
      SetValue(IN_BDATA02,param,3);
      // Log Headder
      vth_mode = 1;
      // VthRead2
      min_vol = search_gain;
      //min_vol=-2.0V

      printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      
      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!");
        DataOut(Ffpt,outbuf);
      }

// printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA             44  **"); printf("**   AAAA   RR   RR EE        AAAA           444  **"); printf("**   AAAA   RR   RR EE        AAAA          4 44  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **"); printf("** AA    AA RR R    EE      AA    AA      4444444 **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd / BL:Odd                                **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); // WL-Odd // SetValue(IN_BDATA02,param,4); vth_mode = 1; // VthRead2          // min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }
    }
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
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_BDATA02,param,0);
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_REG,param,stack8);
  SetValue(IN_ETLR25,param,etlr_25);
    // *******************

}

// ****************************************************************************
// ! @file RC04EXslc_Sinraisei_4kb.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Ret_Distribution_multi_area_rowst(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,k,m,p,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,cond6,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain,selsizew,snhyaku;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char test_item2[128];

  int selsizec,selsizecinit,selsizemax,selmat,repeatcyc,matnum;

  char vthread_i[128];

  char test_temp[128];

  char pe_temp[128];

  int matcyc[32];
	
  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);
      // Koutei
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);
      // Test Item
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);
      // Test Temp
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);
      // P/E  Temp
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);
      // FLP0 P/E Cycle
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);
      // FLI1 P/E Cycle
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);
      // FLI2 P/E Cycle

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("+=========================+");
      printf("|  Test Information       |");
      printf("+-------------------------+");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |"); break;
        default:   printf("|  Flag ???               |"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |"); area=2; break;
        case 0x30: printf("|  Endurance              |"); break;
        default:   printf("|  Test ???               |"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |"); s
        case  90: printf("|  Test_Temp =  90C       |"); s
        case  60: printf("|  Test_Temp =  60C       |"); s
        case  25: printf("|  Test_Temp =  25C       |"); s
        case  40: printf("|  Test_Temp = -40C       |"); s
        case  42: printf("|  Test_Temp =  42C       |"); s
        default:  printf("|  Test_Temp ???          |"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |"); s
        case  90: printf("|  P/E_Temp  =  90C       |"); s
        case  60: printf("|  P/E_Temp  =  60C       |"); s
        case  25: printf("|  P/E_Temp  =  25C       |"); s
        case  40: printf("|  P/E_Temp  = -40C       |"); s
        default:  printf("|  P/E_Temp  ???          |"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |"); break;
        case  1: printf("|  FLP0:  1K              |"); break;
        case 10: printf("|  FLP0: 10K              |"); break;
        default: printf("|  FLP0: ---              |"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |"); break;
        case  1: printf("|  FLI1:  1K              |"); break;
        case 10: printf("|  FLI1: 10K              |"); break;
        default: printf("|  FLI1: ---              |"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |"); break;
        case  1: printf("|  FLI2:  1K              |"); break;
        case 10: printf("|  FLI2: 10K              |"); break;
        default: printf("|  FLI2: ---              |"); break;
      }
      printf("+=========================+");

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("");

// printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat); if ( mat == 0 ) { if ( cyc_flp0 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } } if ( mat == 1 ) { if ( cyc_fli1 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } } if ( mat == 2 ) { if ( cyc_fli2 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } }


    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}

    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}

    	
  printf("*** Common Start Address setting!! [ ex: 0x20000]    ->"); DigitInput(&selsizew);
//  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); selsizemax=0xFFFF;//DigitInput(&selsizemax);
  printf("*** AreaNo setting!! [ ex: 1] SampleName 200X X->input   ->"); DigitInput(&snhyaku);
  //SampleName 200X X->input 
	selsizec=selsizemax;
	
//  printf("*** Test Area setting!!            [ ex: 0xFFF (4KB)]->");  //DigitInput(&selsizec);
  printf("*** Test Area xxKB??               [ ex: 4KB->4, 64KB->64     ]->"); DigitInput(&matnum);
  printf("*** Repeat cyc number!! max128     [ ex: 5cyc:5     ]->"); repeatcyc=0;
  //DigitInput(&repeatcyc);
  printf("*** Test Mat  setting!!            [ FLP0:0, FLI1:1, FLI2:2, FLP0&FLI1:3, FLI1&FLI2:4, FLP0&FLI2:5, FLP0&FLI1&FLI2:6 ->"); DigitInput(&selmat);
      printf("");
      printf("*** Start Address :H'%X ",selsizew);
      printf("*** Test  MaxArea :H'%X ",selsizemax);
      printf("*** Test  Area    :H'%X ",selsizec);
      printf("*** Test  Area(KB):%dKB ",matnum);
      printf("*** Repeat cyc    :%dcyc",repeatcyc);
      if(selmat==0) printf("*** Test  Mat     :FLP0");
      else if(selmat==1) printf("*** Test  Mat     :FLI1");
      else if(selmat==2) printf("*** Test  Mat     :FLI2");
      else if(selmat==3) printf("*** Test  Mat     :FLP0&FLI1");
      else if(selmat==4) printf("*** Test  Mat     :FLI1&FLI2");
      else if(selmat==5) printf("*** Test  Mat     :FLP0&FLI2");
      else if(selmat==6) printf("*** Test  Mat     :FLP0&FLI1&FLI2  %d",selmat);
      else printf("*** Test  Mat     :Not setting!!");
      printf("*** Setting Address Check!! ***");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
    	
       printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Ret-1st After Chip    !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  cond3 = 0;
  //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

// ************************************************************************************
// **   Extra4(Test Information) ReWrite                                             **
// ************************************************************************************
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA);
        //Ret1st Code Write
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

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) || (cond1==1) ) {
  //Before&1st
    vs[0] = 1600;
    //vcc=1.6V setting
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);

    m=0;
  	for( m=0; m<3; m++ ){
  if(m==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) {
    //FLP0
  //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
        //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,selsizew);
      //FLP0
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      //xxKB
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }
  //if m=0 

  if(m==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) {
    //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,selsizew);
      //FLI1
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      //xxKB
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }
  //if m=1 

  if(m==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) {
  	//FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,selsizew);
      //FLI2
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      //xxKB
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
   }
   //if m=2 
  }
  //for 
 }
 //Test Data Write 

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    //vcc=3.3V

	selsizecinit = selsizec;
  for( k=0; k<=repeatcyc; k++ ){

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%02d%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],k,info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d%01d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],snhyaku);

  	
  	p=0;
//  	p=2;
//  	for( p=0; p<3; p++ ){    
  	matcyc[k]=k;
  	printf("******k=%d********************************************",k);
    printf("******k=%d********************************************",k);
    printf("******matcyc[k]=%d********************************************",matcyc[k]);
    printf("******ppppppppp=%d********************************************",p);
//  if(p==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) {
    //FLP0
  //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);
        //xxKB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/4);
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0_%03dKB_rowst_mat%03d",cyc_flp0,matnum,matcyc[k]);
      //FLP0=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=0 

  	
//  if(p==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) {
    //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);
        //xxKB
      printf("***CHECK111111*************************");
      printf("***CHECK111111*************************");
      printf("***CHECK111111*************************");
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/4);
      printf("***CHECK22222*************************");
      printf("***CHECK22222*************************");
      printf("***CHECK22222*************************");
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1_%03dKB_rowst_mat%03d",cyc_fli1,matnum,matcyc[k]);
      //FLI1=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=1 

  	
//  if(p==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) {
  	//FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);
        //4KB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/4);
      }
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2_%03dKB_rowst_mat%03d",cyc_fli2,matnum,matcyc[k]);
      //FLI2=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=2 

  	
    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // WL-Even
      // Log Headder
      // VthRead2&3(0~2.5V)
      // WL-Odd
      // VthRead2&3(0~2.5V)
      // WL-Even
      // Log Headder
      // VthRead2
      // WL-Odd
      // VthRead2
	// Common Clear
  // *******************
// ****************************************************************************
// 1=ALL Pattern bit_count = bit_count/2; //ALL -> Col or Row  if(selmat <= 6){ //FLP0 & FLI1 & FLI2 if ((area == 0)||(area == 1)) { //Retention [Loss] printf("**************************************************"); printf("**                                              **"); printf("**    AA    RRRRR   EEEEEEE    AA          11   **"); printf("**   AAAA   RR   RR EE        AAAA        111   **"); printf("**   AAAA   RR   RR EE        AAAA         11   **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   11   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===   11   **"); printf("** AA    AA RR R    EE      AA    AA       11   **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      1111  **"); printf("**                                              **"); printf("**************************************************"); printf("** WL:Even / BL:Even                            **"); printf("**************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,1); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          2222   **"); printf("**   AAAA   RR   RR EE        AAAA        22  22  **"); printf("**   AAAA   RR   RR EE        AAAA            22  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===     22   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===    22    **"); printf("** AA    AA RR R    EE      AA    AA       22     **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA      222222  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,2); vth_mode = 23; min_vol = search_loss; //min_vol=0.5V @Before & 1st  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); } }  if ((area == 0)||(area == 2)) { //Retention [Gain] printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA          3333   **"); printf("**   AAAA   RR   RR EE        AAAA        33  33  **"); printf("**   AAAA   RR   RR EE        AAAA            33  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===    333   **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===      33  **"); printf("** AA    AA RR R    EE      AA    AA      33  33  **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA       3333   **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Even / BL:Odd                               **"); printf("****************************************************"); SetValue(IN_BDATA01,param,1); SetValue(IN_BDATA02,param,3); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu Error!!!"); DataOut(Ffpt,outbuf); }  printf("****************************************************"); printf("**                                                **"); printf("**    AA    RRRRR   EEEEEEE    AA             44  **"); printf("**   AAAA   RR   RR EE        AAAA           444  **"); printf("**   AAAA   RR   RR EE        AAAA          4 44  **"); printf("**  AA  AA  RRRRR   EEEEEEE  AA  AA  ===   4  44  **"); printf("**  AAAAAA  RRR     EE       AAAAAA  ===  4   44  **"); printf("** AA    AA RR R    EE      AA    AA      4444444 **"); printf("** AA    AA RR  RRR EEEEEEE AA    AA          44  **"); printf("**                                                **"); printf("****************************************************"); printf("** WL:Odd / BL:Odd                                **"); printf("****************************************************"); SetValue(IN_BDATA01,param,2); SetValue(IN_BDATA02,param,4); vth_mode = 1; min_vol = search_gain; //min_vol=-2.0V  printf("FileName = %s, bit_count = %d, vth_mode = %d",item_name,bit_count,vth_mode); result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);  if(PASS != result){ sprintf(outbuf," Make Vth Bunpu .csv Error!!!"); DataOut(Ffpt,outbuf); } } }  // }//for p selsizew = selsizew + selsizec + 0x01;  //  	selsizew = selsizec+0x01;  //    selsizec = selsizecinit; } //for k repeatcyc    SetValue(IN_AREA,param,stack0); SetValue(IN_TOPADDR0 ,param,stack1); SetValue(IN_TESTSIZE0,param,stack2); SetValue(IN_TOPADDR1 ,param,stack3); SetValue(IN_TESTSIZE1,param,stack4); SetValue(IN_TOPADDR2 ,param,stack5); SetValue(IN_TESTSIZE2,param,stack6); SetValue(IN_EXTAL1,param,stack7); SetValue(IN_BDATA01,param,0); SetValue(IN_BDATA02,param,0); SetValue(IN_BDATA03,param,0); SetValue(IN_REG,param,stack8); SetValue(IN_ETLR25,param,etlr_25);   }   /*! @file RC04EXslc_Sinraisei_4kb.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Ret_Distribution_multi_area_all01(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,k,m,p,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4,cond5,cond6,test_flag,cyc_flp0,cyc_fli1,cyc_fli2;
  int search_loss,search_gain,selsizew,snhyaku;
  int etlr_25;
  int info[32];
  // wmat chip information
  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char test_item2[128];

  int selsizec,selsizecinit,selsizemax,selmat,repeatcyc,matnum;

  char vthread_i[128];

  char test_temp[128];

  char pe_temp[128];

  int matcyc[32];
	
  cond1 = cond2 = cond3 = cond4 = 0;
  search_loss =   500;
  search_gain = -2000;

  SetValue(IN_BDATA03,param,0);

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
      // if ( PASS == OpensShorts() ) {
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
      // } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

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
      // t@[ŕύX\
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
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No
      test_flag = ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B);
      // Koutei
      cond0     = ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B);
      // Test Item
      cond2     = ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B);
      // Test Temp
      cond5     = ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B);
      // P/E  Temp
      cyc_flp0  = ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B);
      // FLP0 P/E Cycle
      cyc_fli1  = ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B);
      // FLI1 P/E Cycle
      cyc_fli2  = ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B);
      // FLI2 P/E Cycle

      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;
      // 2nd-Cut

      printf("");
      printf("+=========================+");
      printf("|  Test Information       |");
      printf("+-------------------------+");
      switch(test_flag) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |"); break;
        default:   printf("|  Flag ???               |"); break;
      }

      switch(cond0) {
        case 0x00: printf("|  Retention [Loss]       |"); area=1; break;
        case 0x01: printf("|  Retention [Gain]       |"); area=2; break;
        case 0x10: printf("|  Erroneous Erase [Loss] |"); area=1; break;
        case 0x20: printf("|  Erroneous Write [Gain] |"); area=2; break;
        case 0x30: printf("|  Endurance              |"); break;
        default:   printf("|  Test ???               |"); break;
      }

      switch(cond2) {
        case 125: printf("|  Test_Temp = 125C       |"); s
        case  90: printf("|  Test_Temp =  90C       |"); s
        case  60: printf("|  Test_Temp =  60C       |"); s
        case  25: printf("|  Test_Temp =  25C       |"); s
        case  40: printf("|  Test_Temp = -40C       |"); s
        case  42: printf("|  Test_Temp =  42C       |"); s
        default:  printf("|  Test_Temp ???          |"); break;
      }

      switch(cond5) {
        case 125: printf("|  P/E_Temp  = 125C       |"); s
        case  90: printf("|  P/E_Temp  =  90C       |"); s
        case  60: printf("|  P/E_Temp  =  60C       |"); s
        case  25: printf("|  P/E_Temp  =  25C       |"); s
        case  40: printf("|  P/E_Temp  = -40C       |"); s
        default:  printf("|  P/E_Temp  ???          |"); break;
      }

      switch(cyc_flp0) {
        case  0: printf("|  FLP0:  0               |"); break;
        case  1: printf("|  FLP0:  1K              |"); break;
        case 10: printf("|  FLP0: 10K              |"); break;
        default: printf("|  FLP0: ---              |"); break;
      }
      switch(cyc_fli1) {
        case  0: printf("|  FLI1:  0               |"); break;
        case  1: printf("|  FLI1:  1K              |"); break;
        case 10: printf("|  FLI1: 10K              |"); break;
        default: printf("|  FLI1: ---              |"); break;
      }
      switch(cyc_fli2) {
        case  0: printf("|  FLI2:  0               |"); break;
        case  1: printf("|  FLI2:  1K              |"); break;
        case 10: printf("|  FLI2: 10K              |"); break;
        default: printf("|  FLI2: ---              |"); break;
      }
      printf("+=========================+");

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);

      printf("");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);

// ************************************************************************************
// **   Condition Select                                                             **
// ************************************************************************************
  printf("");

// printf("*** Test MAT       [FLP0:0 / FLI1:1 / FLI2:2 ] ->"); DigitInput(&mat); if ( mat == 0 ) { if ( cyc_flp0 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } } if ( mat == 1 ) { if ( cyc_fli1 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } } if ( mat == 2 ) { if ( cyc_fli2 == 0x99 ) { printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); printf("!!!  Select Mat NG         !!!"); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); return(PASS); } }


    if ( cyc_flp0 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
    if ( cyc_fli1 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}

    if ( cyc_fli2 == 0x99 ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Select Mat NG         !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}

    	
  printf("*** Common Start Address setting!! [ ex: 0x20000]    ->"); DigitInput(&selsizew);
//  printf("*** Test MaxArea setting!!         [ ex: 0x1FFFF (128KB)]->"); selsizemax=0xFFFF;//DigitInput(&selsizemax);
  printf("*** AreaNo setting!! [ ex:SampleName 200X X->input]   ->"); DigitInput(&snhyaku);
  //SampleName 200X X->input 
	selsizec=selsizemax;
	
//  printf("*** Test Area setting!!            [ ex: 0xFFF (4KB)]->");  //DigitInput(&selsizec);
  printf("*** Test Area xxKB??               [ ex: 4KB->4, 64KB->64     ]->"); DigitInput(&matnum);
  printf("*** Repeat cyc number!! max128     [ ex: 5cyc:5     ]->"); repeatcyc=0;
  //DigitInput(&repeatcyc);
  printf("*** Test Mat  setting!!            [ FLP0:0, FLI1:1, FLI2:2, FLP0&FLI1:3, FLI1&FLI2:4, FLP0&FLI2:5, FLP0&FLI1&FLI2:6 ->"); DigitInput(&selmat);
      printf("");
      printf("*** Start Address :H'%X ",selsizew);
      printf("*** Test  MaxArea :H'%X ",selsizemax);
      printf("*** Test  Area    :H'%X ",selsizec);
      printf("*** Test  Area(KB):%dKB ",matnum);
      printf("*** Repeat cyc    :%dcyc",repeatcyc);
      if(selmat==0) printf("*** Test  Mat     :FLP0");
      else if(selmat==1) printf("*** Test  Mat     :FLI1");
      else if(selmat==2) printf("*** Test  Mat     :FLI2");
      else if(selmat==3) printf("*** Test  Mat     :FLP0&FLI1");
      else if(selmat==4) printf("*** Test  Mat     :FLI1&FLI2");
      else if(selmat==5) printf("*** Test  Mat     :FLP0&FLI2");
      else if(selmat==6) printf("*** Test  Mat     :FLP0&FLI1&FLI2  %d",selmat);
      else printf("*** Test  Mat     :Not setting!!");
      printf("*** Setting Address Check!! ***");
      printf("************************");
      printf("***  Test Run OK?   ****");
      printf("************************");
      printf(" OK=0 / NO=Other-No ->"); DigitInput(&tempo);
      if ( tempo != 0 ) return(PASS);
    	
       printf("*** Test Select    [Before:0 / 1st:1 / Read-Out:2] ->"); DigitInput(&cond1);
  if ((cond1 == 0) || ( cond1 == 1)) {
    if ( test_flag==0xAA ) {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      printf("!!!  Ret-1st After Chip    !!!");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return(PASS);
	}
  }
  cond3 = 0;
  //Before 0hr,1st 0hr setting
  if ( cond1 == 2 ) {
  search_loss =   100;
  printf("*** Test Time    (hr-Time input)     ->"); DigitInput(&cond3);
  }
  st_addr = 0;

// ************************************************************************************
// **   Extra4(Test Information) ReWrite                                             **
// ************************************************************************************
  if ( cond1 == 1 ) {
//      printf("*** Extra4(Test Information) ReWrite? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      printf("*** Extra4(Test Information) ReWrite & Ret_1st Code'AA' Write? [Yes:0 / No:1] ->"); DigitInput(&tempo);
      
      if ( tempo == 0 ) {
        SetValue(IN_LDATA0 ,param,info[20]);
        SetValue(IN_BDATA01,param,0xAA);
        //Ret1st Code Write
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

// ************************************************************************************
// **   Test Data Write                                                          **
// ************************************************************************************
  if ( (cond1==0) || (cond1==1) ) {
  //Before&1st
    vs[0] = 1600;
    //vcc=1.6V setting
    printf("");
    printf("******************************");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** !!! Write Start OK?    ***");
    printf("*** set vcc %dmV         ***",vs[0]);
    printf("******************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
    SetValue(IN_REG,param,0x03);

    m=0;
  	for( m=0; m<3; m++ ){
  if(m==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) {
    //FLP0
  //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
        //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,selsizew);
      //FLP0
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      //xxKB
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR0 ,param,selsizew);
      SetValue(IN_TESTSIZE0,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }
  //if m=0 

  if(m==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) {
    //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,selsizew);
      //FLI1
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      //xxKB
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR1 ,param,selsizew);
      SetValue(IN_TESTSIZE1,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
  }
  //if m=1 

  if(m==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) {
  	//FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,selsizew);
      //FLI2
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      //xxKB
//      SetValue(IN_ETCR08,param,0x20); //Multi Block select User -> On
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
//      SetValue(IN_ETCR08,param,0x00); //Multi Block select User -> Off

      SetValue(IN_TOPADDR2 ,param,selsizew);
      SetValue(IN_TESTSIZE2,param,selsizemax+0x1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

      if      ((cond0==0x00) || (cond0==0x01)) {
      // Retention
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      SetValue(IN_TESTSEL,param,0);
      if      (cond0==0x00) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      else if ((cond0==0x10) || (cond0==0x20)) CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE");
      // t@[ŕύX\
    }
   }
   //if m=2 
  }
  //for 
 }
 //Test Data Write 

// ************************************************************************************
// **   Vth-Read Function                                                            **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    vs[0] = 3300;
    //vcc=3.3V

	selsizecinit = selsizec;
  for( k=0; k<=repeatcyc; k++ ){

//      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%02d%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],k,info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_S%03d%01d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],snhyaku);

  	
//  	p=0;
//  	p=2;
//  	for( p=0; p<3; p++ ){    
  	matcyc[k]=k;
  	printf("******k=%d********************************************",k);
    printf("******k=%d********************************************",k);
    printf("******matcyc[k]=%d********************************************",matcyc[k]);
    printf("******ppppppppp=%d********************************************",p);
//  if(p==0){
    if( (selmat==0) || (selmat==3) || (selmat==5) || (selmat==6) ) {
    //FLP0
  //FLP0//FLP0//FLP0//FLP0//FLP0//FLP0//FLP0
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("******FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0FLP0********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR0 ,param,selsizew);
        SetValue(IN_TESTSIZE0,param,selsizec+0x1);
        //xxKB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
        SetValue(IN_TESTSIZE0,param,FLP0_SIZE/2);
      }
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLP0_%03dKB_all01_mat%03d",cyc_flp0,matnum,matcyc[k]);
      //FLP0=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=0 

  	
//  if(p==1){
    if( (selmat==1) || (selmat==3) || (selmat==4) || (selmat==6) ) {
    //FLI1
//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1//FLI1
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("******FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1FLI1********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR1 ,param,selsizew);
        SetValue(IN_TESTSIZE1,param,selsizec+0x1);
        //xxKB
      printf("***CHECK111111*************************");
      printf("***CHECK111111*************************");
      printf("***CHECK111111*************************");
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
        SetValue(IN_TESTSIZE1,param,FLI1_SIZE/2);
      printf("***CHECK22222*************************");
      printf("***CHECK22222*************************");
      printf("***CHECK22222*************************");
      }
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI1_%03dKB_all01_mat%03d",cyc_fli1,matnum,matcyc[k]);
      //FLI1=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=1 

  	
//  if(p==2){
  	if( (selmat==2) || (selmat==4) || (selmat==5) || (selmat==6) ) {
  	//FLI2
//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2//FLI2  	
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("******FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2FLI2********************************************");
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      printf("****** selmat=%d**************************",selmat);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      if      ( (cond0==0x00) || (cond0==0x01) || (cond0==2)) {
      // Retention or RDT
        SetValue(IN_TOPADDR2 ,param,selsizew);
        SetValue(IN_TESTSIZE2,param,selsizec+0x1);
        //4KB
      }
      else if ((cond0==0x10) || (cond0==0x20)) {
      // Disturb
        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
        SetValue(IN_TESTSIZE2,param,FLI2_SIZE/2);
      }
      if (cond1 == 0) sprintf(test_name,"_%03dK_before_FLI2_%03dKB_all01_mat%03d",cyc_fli2,matnum,matcyc[k]);
      //FLI2=10K xxKB-mat[i] if ((cond1 == 1)||(cond1 == 2)) { //Before&1st s
      }
    }
//  }//if p=2 

  	
    strcpy(item_name,sample_name);
    strcat(item_name,test_temp);
    strcat(item_name,test_name);
//    strcat(item_name,vthread_i);
 
    // Common Setting
  SetValue(IN_EXTAL1,param,VTH_FREQ);
  tempo2 = 50;
  // VS-Step Default
  bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
  /  // *******************
      // All
      // Log Headder
      // VthRead2&3(0~2.5V)
      // All
      // Log Headder
      // VthRead2
	// Common Clear
  // *******************
