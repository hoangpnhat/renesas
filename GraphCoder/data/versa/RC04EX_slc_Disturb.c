// ****************************************************************************
// ! @file RC04EXslc_Disturb.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int Disturb(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int etcr8;
  int min_vol,result,vth_mode,bit_count;
  int i,j,l,mat,cond1,cond2,cond3,adr_mode,adr_st,adr_end;
  int loop_num;
  int info[32];
  // wmat chip information
  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

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

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
      // if ( PASS == OpensShorts() ) { DeviceSpecificPowerUp();  SetValue(IN_REG,param,CHIPDATA); SetValue(IN_TESTSEL,param,0x00); if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) { SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param)); SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param)); SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param)); SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param)); SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param)); SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param)); SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param)); SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param)); SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param)); SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param)); SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param)); SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param)); SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param)); SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param)); printf("************************************"); printf("***   INIT PASS                  ***"); printf("************************************"); printf(""); } else { printf("------------------------------------"); printf("---   INIT FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); } } else { printf("------------------------------------"); printf("---   PIN FAIL                  ---"); printf("------------------------------------"); printf(""); return(PASS); }

// ************************************************************************************
// **   Test Select                                                             **
// ************************************************************************************
  printf("");
  printf("******************************************");
  printf("*** Write-Inhibit TEST                 ***");
  printf("*** Write-Inhibit TEST                 ***");
  printf("*** Write-Inhibit TEST                 ***");
  printf("******************************************");
  printf("");

      printf("*** Test MAT    [FLP0:0  / FLI1:1 / FLI2:2 / e:Exit ] ->"); DigitInput(&mat);
      printf("*** Test MAT    [0x0~0xFFF :0 / 0x0~0x0FFFF:1 / 0x10000~0x1FFFF:2 / 0x0~0x0FFFF(PE-Non):3 / e:Exit ] ->"); DigitInput(&adr_mode);

      if ( mat == EXIT ) return(PASS);
      if ( adr_mode == EXIT ) return(PASS);

      printf("*** Test Temp   [-40:40 / 25C:25  / 90C:90 ] ->"); DigitInput(&cond2);

      switch (adr_mode){
        case 0:
          adr_st  = 0;
          adr_end = 0x1000;
        break;
        case 1:
          adr_st  = 0;
          adr_end = 0x10000;
        break;
        case 2:
          adr_st  = 0x10000;
          adr_end = 0x10000;
        break;
        case 3:
          adr_st  = 0x20000;
          adr_end = 0x10000;
        break;
      }

// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      SetValue(IN_REG,param,0x03);
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);
      // Extra5
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      // t@[ŕύX\
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10] = ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11] = ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12] = ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      // t@[ŕύX\
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);
      printf("***  S : %3d",info[20]);

      sprintf(sample_name,"Write-Inhibit_%c%c%c%c%c%c_W%02d_S%03d_MAT%d_Adr%d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],mat,adr_mode);

      if      (cond2 == 40) strcpy(test_name,"_-40C");
      else if (cond2 == 25) strcpy(test_name,"_25C");
      else if (cond2 == 90) strcpy(test_name,"_90C");
      else if ((cond1 == 1)|(cond1 == 2)) strcpy(test_name,"_Dis");
      else                  strcpy(test_name,"_000");

      strcpy(item_name,sample_name);
      strcat(item_name,test_name);
      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());

      time( &time_list );
      // time_list += V4V5K_TIME_OFS;

// ************************************************************************************
// **   Disturb & Distribution                                                       **
// ************************************************************************************
    SetValue(IN_REG,param,0x03);
    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR0 ,param,0);
    SetValue(IN_TESTSIZE0,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);

        // shoki Write&Erase ****************************************************
    if ( mat==0 ) {
      SetValue(IN_TOPADDR0 ,param,adr_st);
      SetValue(IN_TESTSIZE0,param,adr_end);
    } else if ( mat==1 ) {
      SetValue(IN_TOPADDR1 ,param,adr_st);
      SetValue(IN_TESTSIZE1,param,adr_end);
    } else if ( mat==2 ) {
      SetValue(IN_TOPADDR2 ,param,adr_st);
      SetValue(IN_TESTSIZE2,param,adr_end);
    }
    
    if(adr_mode!=3){
      SetValue(IN_ETCR08,param,0x20);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program");
      SetValue(IN_ETCR08,param,0x00);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Program");
    }
    
    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }
    fprintf(file_log,"<<Write-Inhibit>> ETLR25=0x%X,Vcc=%d,Distribution:0V~0.05VStep,Mat=%d(0x%X~0x%X)",GetValue(IN_ETLR25,param),vs[0],mat,adr_st,adr_end-1);
    fprintf(file_log,"");
    fclose( file_log );

    for ( i=0 ; i<10 ; i++ ) {

    if( NULL==(file_log = fopen( file_name, "at" )) ){
      printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
    }

      switch (i) {
        case 0: loop_num =  0; fprintf(file_log,"Erase"); break;
        // Init
        case 2: loop_num =  2; fprintf(file_log,"x3   "); break;
        // x3
        case 4: loop_num =  5; fprintf(file_log,"x10  "); break;
        // x10
        case 6: loop_num = 12; fprintf(file_log,"x32  "); break;
        // x32
        case 8: loop_num = 23; fprintf(file_log,"x70  "); break;
        // x70
        default: break;
      }

      fclose( file_log );
	      
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,adr_st);
        SetValue(IN_TESTSIZE0,param,adr_end);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,adr_st);
        SetValue(IN_TESTSIZE1,param,adr_end);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,adr_st);
        SetValue(IN_TESTSIZE2,param,adr_end);
      }

//      if ( mat==0 ) {
//        SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
//        SetValue(IN_TESTSIZE0,param,0x100);
//      } else if ( mat==1 ) {
//        SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
//        SetValue(IN_TESTSIZE1,param,0x100);
//      } else if ( mat==2 ) {
//        SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
//        SetValue(IN_TESTSIZE2,param,0x100);
//      }

//       SetValue(IN_ETLR36,param,0xFE);    // 10ms
//       SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20);    // User Sector

      if ( i!=0 ) {
      // Write-Inhibit Stress Input

        SetValue(IN_TESTSEL,param,1);
        // ALL1
        SetValue(IN_EXTAL1,param,31);
        // 32MHz
        for ( j=0 ; j<loop_num ; j++ ) {
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
        // t@[ŕύX\
        }
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_ETCR08,param,etcr8);
      }

            // Distribution
      SetValue(IN_ETCR08,param,etcr8);
      if ( mat==0 ) {
        SetValue(IN_TOPADDR0 ,param,adr_st);
        SetValue(IN_TESTSIZE0,param,adr_end);
      } else if ( mat==1 ) {
        SetValue(IN_TOPADDR1 ,param,adr_st);
        SetValue(IN_TESTSIZE1,param,adr_end);
      } else if ( mat==2 ) {
        SetValue(IN_TOPADDR2 ,param,adr_st);
        SetValue(IN_TESTSIZE2,param,adr_end);
      }

      if(adr_mode != 3){
        SetValue(IN_BDATA01,param,0);
        // ALL Count
      }else{
        SetValue(IN_BDATA01,param,1);
        // WL-even 
      }
      SetValue(IN_BDATA02,param,99);
      // Log Headder
      SetValue(IN_BDATA04,param,1);
      // Log Output -> DATALOG
      SetValue(IN_BDATA05,param,1);
      // Vth-Level-Log:OFF
      SetValue(IN_EXTAL1,param,VTH_FREQ);
      min_vol = -2000;
      tempo2 = 50;
      // VS-Step Default
      if(adr_mode ==3){
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 / 2;
        // /2 = RowStripe
      }else{
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
        // *1=ALL Pattern
      }
      vth_mode = 1;
      // VthRead1&2
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!");
        DataOut(Ffpt,outbuf); return(PASS);
      }
    }
    // for i
  SetValue(IN_ETCR08,param,etcr8);

  time( &time_list2 );
  // time_list += V4V5K_TIME_OFS;
  printf("Test-Time:%d[s]",time_list2-time_list);
  printf("");

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
  SetValue(IN_TESTSEL,param,0);
  SetValue(IN_WRITESIZE,param,0);
    // *******************

}

