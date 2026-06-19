// ****************************************************************************
// ! @file RC04EXslc_ReadWindow.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author T.Morioka @date 2012.04.16 Rev.00
// ****************************************************************************
int ReadWindow(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int stack9,stack10,stack11;
  int etlr09,etlr12,etlr16,etlr20,etlr25,etlr37;
  int min_vol,result,vth_mode,bit_count;
  int count_1,count_16,count_24,count_32;
  int i,j,k,l,mat,cond1,cond2,cond3;
  int on_test,vdd_sel,iref_sel,loop_num,selvol,loop_k,loop_kst,loop_ked,ireftmp_sel,ireftmp_onsel,iref_moni,iref_monitmp,evaltmp;
  int volcon,delayt1_sel,delayt2_sel,delayt3_sel,delayt4_sel,delayt6_sel,delayt5_sel,delaydef,freq01,freq32,freq01ns,freq32ns;
  int etlr26,etlr27,etlr28,etlr29,etlr30,etlr32,etlr33,etlr34;
  int tempo7,tempo8,tempo9,tempo10,tempo11,tempo12,tempo13,tempo14,tempo15,tempo16,tempo17,tempo18;
  int info[32];
  // wmat chip information
  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  FILE_T *file_count;

  cond1 = cond2 = cond3 = 0;
  count_1 = count_16 = count_24 = count_32 = 0;
  vdd_sel = iref_sel = delaydef = freq01 = freq32 = ireftmp_sel = ireftmp_onsel = 0;
  delayt4_sel = 0;
  delayt6_sel = 0;
  delayt5_sel = 0;
  delayt1_sel = 0;
  delayt2_sel = 0;
  delayt3_sel = 0;
  tempo7 = tempo8 = tempo9 = tempo10 = tempo11 = tempo12 = tempo13 = 0;
  tempo14 = tempo15 = tempo16 = tempo17 = 0;
  i = j = k = l = 0;
  
  stack0 = GetValue(IN_AREA,param);
  stack1 = GetValue(IN_TOPADDR0,param);
  stack2 = GetValue(IN_TESTSIZE0,param);
  stack3 = GetValue(IN_TOPADDR1,param);
  stack4 = GetValue(IN_TESTSIZE1,param);
  stack5 = GetValue(IN_TOPADDR2,param);
  stack6 = GetValue(IN_TESTSIZE2,param);
  stack7 = GetValue(IN_EXTAL1,param);
  stack8 = vs[0]; stack9 = vs[1]; stack10 = vs[2];
  stack11= GetValue(IN_REG,param);

// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
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
      } else {
        printf("------------------------------------");
        printf("---   PIN FAIL                  ---");
        printf("------------------------------------");
        printf("");
	return(PASS);
      }

// ************************************************************************************
// **   Test Select                                                             **
// ************************************************************************************
  printf("");
  printf("******************************************");
  printf("*** Read Window TEST                   ***");
  printf("*** Read Window TEST                   ***");
  printf("*** Read Window TEST                   ***");
  printf("******************************************");
  printf("");

      printf("*** Test Select [Middle_Vth-Set:0 / Distribution:1 / Distribution(Point):2 / Read-Count:3 / Read-Count for(T1T2):4 / Read-Count tmpIref:8 ]");
      printf("*** Test Select [Middle_Vth-Set@EXTRA:5 / Distribution@EXTRA:6 / Distribution(Point)@EXTRA:7 / Irefmonitor_tmpIref:9  Read-Count@EXTRA:10 ]");
      printf("*** Test Select [                       / Distribution@EXTRA:11 noGiji_TrimRead@irefRead / Exit:99 ->"); DigitInput(&cond1);
      if ( cond1 == 99 ) printf("*** Test exit!! ***");
      if ( cond1 == 3 || cond1 == 4 ) {
      printf("*** Test Temp   [-40:40 / 25C:25  / 95C:95 ] ->"); DigitInput(&cond2);
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

// info[0] = ReadEcrMode8(0x2A20,SIZE_B); info[1] = ReadEcrMode8(0x2A21,SIZE_B); info[2] = ReadEcrMode8(0x2A22,SIZE_B); info[3] = ReadEcrMode8(0x2A23,SIZE_B); info[4] = ReadEcrMode8(0x2A24,SIZE_B); info[5] = ReadEcrMode8(0x2A25,SIZE_B); info[10] = ReadEcrMode8(0x2A2A,SIZE_B); info[11] = ReadEcrMode8(0x2A2C,SIZE_B); info[12] = ReadEcrMode8(0x2A2E,SIZE_B);
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
      info[20] = info[20] - 10000;
      sprintf(sample_name,"ReadWindow_%c%c%c%c%c%c_W%02d_S%03dX%dY%d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],info[11],info[12]);

      if      (cond2 == 40) strcpy(test_name,"_-40C");
      else if (cond2 == 25) strcpy(test_name,"_25C");
      else if (cond2 == 95) strcpy(test_name,"_95C");
      else if ((cond1 == 1)|(cond1 == 2)) strcpy(test_name,"_Dis");
      else                  strcpy(test_name,"_000");

      strcpy(item_name,sample_name);
      strcat(item_name,test_name);

// ************************************************************************************
// **   Middle_Vth-Set                                                         **
// ************************************************************************************
  if ( cond1==0 ) {
    printf("");
    printf("**************************");
    printf("*** Middle_Vth-Set OK? ***");
    printf("*** Middle_Vth-Set OK? ***");
    printf("*** Middle_Vth-Set OK? ***");
    printf("**************************");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_REG,param,0x03);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      // t@[ŕύX\
      SetValue(IN_ETCR08,param,0x00);

    etlr16 = GetValue(IN_ETLR16,param);
    etlr20 = GetValue(IN_ETLR20,param);
    etlr37 = GetValue(IN_ETLR37,param);

    printf("");
    printf("*** Ref ETLR16 =  H'%02X", GetValue(IN_ETLR16,param) );
    printf("*** Vhh_e(ETLR16) Tap Set Ex) 3(+3Tap) or -5(-5Tap) -> ");	DigitInput(&tempo1);
    SetValue(IN_ETLR16,param,(etlr16 + tempo1) | 0xC0);
//    SetValue(IN_ETLR16,param, tempo1); //Direct Input ex)0xFB
//    printf("*** Vpp_e(ETLR20) Tap Set -> ");	DigitInput(&tempo1);
//    etlr20 = (etlr20 & 0x0F) | (((etlr20 >> 4) + tempo1) << 4); 
//    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR20,param,((etlr20 & 0x0F) | 0x00 ));

    for ( i=0 ; i<25 ; i++ ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x19000-(i*0x1000));
      // 4KB

      if      ( count_32 > (32768/2) ) SetValue(IN_ETLR37,param,0xFF);
      // Erase 5ms
      else if ( count_32 > (32768/4) ) SetValue(IN_ETLR37,param,0xF4);
      // Erase 2ms
      else if ( count_32 > (32768/8) ) SetValue(IN_ETLR37,param,0xF3);
      // Erase 1ms
      else                             SetValue(IN_ETLR37,param,0xF1);
      // Erase 0.6ms

      SetValue(IN_EXTAL1,param,31);
      // 32MHz
      SetValue(IN_BDATA00,param,1);
      // Pre-Write Less
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\
      SetValue(IN_BDATA00,param,0);

      if ((i==0)|(i==3)|(i==6)|(i==9)|(i==12)|(i==15)|(i==18)|(i==21)) {
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_EXTAL1,param,31);
        // 32MHz
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
        SetValue(IN_TESTSIZE0,param,0x1000);
        // 4KB
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");
        // t@[ŕύX\
    	count_32 = GetValue(OUT_LDATA0,param);

        sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
        if( NULL==(file_count = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
          break;
        }
        if ( i == 0 ) fprintf(file_count,"No,32M,Tap=%d",tempo1);
    	  fprintf(file_count,"No_%d,%d",i,count_32);
    	  fclose( file_count);
	  }
	}
	// for i

    SetValue(IN_ETLR16,param,etlr16);
    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR37,param,etlr37);
  }
// ************************************************************************************
// **   Distribution                                                                 **
// ************************************************************************************
  if (( cond1==1 ) | ( cond1==2 )) {
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR1 ,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2 ,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_REG,param,0x03);
      SetValue(IN_BDATA04,param,1);

    for ( i=0 ; i<25 ; i++ ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000);
      // 4KB
      if ( i==0 ) SetValue(IN_BDATA03,param,0);
      // Log header ON
      else        SetValue(IN_BDATA03,param,1);
      // Log header OFF

      on_test=1;
      if ( cond1==2 ) {
        if ((i==0)|(i==12)|(i==24)) on_test=1;
        else on_test=0;
      }
      if ( on_test==1 ) {
        SetValue(IN_EXTAL1,param,31);
        // 32MHz
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");
        // t@[ŕύX\
    	count_32 = GetValue(OUT_LDATA0,param);
        if      ( count_32 ==     0 ) min_vol = -500;
        else if ( count_32 == 32768 ) min_vol = -1800;
        else                          min_vol = -1000;

        SetValue(IN_EXTAL1,param,VTH_FREQ);
        tempo2 = 100;
        // VS-Step Default
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
        /        // VthRead1&2
    // for i
// ************************************************************************************
// **   Count                                                                        **
// ************************************************************************************
        	// Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
            // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
        	// Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
    // 2ns Delay Read
    // 2ns Delay Read
    // 2ns Delay Read
    // 2ns Delay Read
    // 8ns PreCharge Delay maxTap
          // Min Iref=1.0uA
          // Min Iref=1.5uA
          // Min Iref=2.0uA
          // Min Iref=2.5uA
          // Min Iref=3.0uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;    // Min Iref=3.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;    // Min Iref=5.0uA
//          SetValue(IN_ETLR33,param,0xFF); break;    // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay 0.82ns
          // 2ns Delay 0.95ns
          // 2ns Delay 1.11ns
          // 2ns Delay 1.24ns target
          // 2ns Delay 1.31ns
          // 2ns Delay 1.44ns
          // 2ns Delay 1.60ns
          // 2ns Delay 1.73ns
          // 2ns Delay 1.89ns
          // 2ns Delay 2.02ns
          // 2ns Delay 2.18ns
          // 2ns Delay 2.31ns
          // 2ns Delay 2.38ns
          // 2ns Delay 2.51ns
          // 2ns Delay 2.67ns
          // 2ns Delay 2.80ns Trimming Read
          // 2ns Delay
//          SetValue(IN_ETLR34,param,0xFF);    // 2ns Delay
          // 2ns Delay
          // 2ns Delay 0.82ns
          // 2ns Delay 0.95ns
          // 2ns Delay 1.11ns
          // 2ns Delay 1.24ns
          // 2ns Delay 1.31ns target
          // 2ns Delay 1.44ns
          // 2ns Delay 1.60ns
          // 2ns Delay 1.73ns
          // 2ns Delay 1.89ns
          // 2ns Delay 2.02ns
          // 2ns Delay 2.18ns
          // 2ns Delay 2.31ns
          // 2ns Delay 2.38ns
          // 2ns Delay 2.51ns
          // 2ns Delay 2.67ns
          // 2ns Delay 2.80ns Trimming Read
          // 2ns Delay
          // 2ns Delay
//          SetValue(IN_ETLR32,param,0xFF);    // 2ns Delay
          // 2ns Delay Read
          // 2ns Delay Read
          // 2ns Delay Read
          // 8ns PreCharge Delay maxTap
          // 2ns Delay 0.82ns
          // 2ns Delay 0.95ns
          // 2ns Delay 1.11ns target
          // 2ns Delay 1.24ns
          // 2ns Delay 1.31ns
          // 2ns Delay 1.44ns
          // 2ns Delay 1.60ns
          // 2ns Delay 1.73ns
          // 2ns Delay 1.89ns
          // 2ns Delay 2.02ns
          // 2ns Delay 2.18ns
          // 2ns Delay 2.31ns
          // 2ns Delay 2.38ns
          // 2ns Delay 2.51ns
          // 2ns Delay 2.67ns
          // 2ns Delay 2.80ns Trimming Read
          // 2ns Delay 0.82ns
          // 2ns Delay 0.95ns
          // 2ns Delay 1.11ns target
          // 2ns Delay 1.24ns
          // 2ns Delay 1.31ns
          // 2ns Delay 1.44ns
          // 2ns Delay 1.60ns
          // 2ns Delay 1.73ns
          // 2ns Delay 1.89ns
          // 2ns Delay 2.02ns
          // 2ns Delay 2.18ns
          // 2ns Delay 2.31ns
          // 2ns Delay 2.38ns
          // 2ns Delay 2.51ns
          // 2ns Delay 2.67ns
          // 2ns Delay 2.80ns Trimming Read
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns PrechargeTime 5.23ns
          // 8ns PrechargeTime 5.62ns
          // 8ns PrechargeTime 6.04ns
          // 8ns PrechargeTime 6.44ns
          // 8ns PrechargeTime 6.74ns
          // 8ns PrechargeTime 7.14ns
          // 8ns PrechargeTime 7.54ns target(sf)
          // 8ns PrechargeTime 7.93ns
          // 8ns PrechargeTime 8.21ns target(tt,ff,ss)
          // 8ns PrechargeTime 8.60ns target(fs)
          // 8ns PrechargeTime 9.03ns
          // 8ns PrechargeTime 9.42ns
          // 8ns PrechargeTime 9.71ns
          // 8ns PrechargeTime 10.10ns
          // 8ns PrechargeTime 10.51ns
          // 8ns PrechargeTime 10.90ns Trimming Read
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns Delay
      // 4KB
//      SetValue(IN_EXTAL1,param,freq01ns);    // 1MHz
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");    // t@[ŕύX\
      // 32MHz
      // t@[ŕύX\
        // 4KB
	// for i
	// for j
// ************************************************************************************
// **   Count For T1(Precharge),T2(Discharge)                                                                        **
// ************************************************************************************
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min Iref=2.0uA
          // Min Iref=2.5uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;    // Min Iref=3.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;    // Min Iref=5.0uA
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 10ns Discharge time
          // 8ns PrechargeTime 1.88ns
          // 8ns PrechargeTime 2.21ns
          // 8ns PrechargeTime 2.55ns
          // 8ns PrechargeTime 2.87ns
          // 8ns PrechargeTime 3.09ns
          // 8ns PrechargeTime 3.42ns
          // 8ns PrechargeTime 3.73ns
          // 8ns PrechargeTime 4.05ns target(sf,ss,tt)
          // 8ns PrechargeTime 4.21ns target(ff)
          // 8ns PrechargeTime 4.53ns target(fs)
          // 8ns PrechargeTime 4.87ns
          // 8ns PrechargeTime 5.19ns
          // 8ns PrechargeTime 5.40ns
          // 8ns PrechargeTime 5.72ns
          // 8ns PrechargeTime 6.04ns
          // 8ns PrechargeTime 6.36ns Trimming Read
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns Precharge time
      // 4KB
//      SetValue(IN_EXTAL1,param,1000); // 1MHz
//      SetValue(IN_EXTAL1,param,freq01ns);    // 1MHz
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");    // t@[ŕύX\
//      SetValue(IN_EXTAL1,param,31);   // 32MHz
      // 32MHz
      // t@[ŕύX\
        // 4KB
	// for i
	// for j
	// for k
// ************************************************************************************
// **   Middle_Vth-Set extra                                                         **
// ************************************************************************************
	    // 16KB
      // t@[ŕύX\
      // 4KB
      // Erase 0.6ms
      // Erase 0.6ms
      // Erase 0.6ms
      // Erase 0.6ms
      // Erase 3ms
      // Erase 3ms
      // Erase 1ms
      // Erase 3ms
      // Pre-Write Less
      // t@[ŕύX\
        // 32MHz
        // 4KB
        // t@[ŕύX\
	// for i
// ************************************************************************************
// **   Distribution@Extra                                                                 **
// ************************************************************************************
      // 4KB
      // Log header ON
      // Log header OFF
        // 32MHz
        // t@[ŕύX\
        // VS-Step Default
        // VthRead1&2
    // for i
// ************************************************************************************
// **   Count For tempiref                                                           **
// ************************************************************************************
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min Iref=2.0uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFB); break;    // Min Iref=1.5uA
//          case 2:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 3:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xFC); break;    // Min Iref=3.0uA
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns Precharge time
          // 10ns Discharge time
          // Min Iref=2.0uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFB); break;    // Min Iref=1.5uA
//          case 2:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 3:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xFC); break;    // Min Iref=3.0uA
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns Precharge time
          // 10ns Discharge time
      // 4KB
//      SetValue(IN_EXTAL1,param,1000); // 1MHz
//      SetValue(IN_EXTAL1,param,freq01ns);    // 1MHz
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");    // t@[ŕύX\
//      SetValue(IN_EXTAL1,param,31);   // 32MHz
      // 32MHz
      // t@[ŕύX\
        // 4KB
	// for i
	// for j
	// for k
// ************************************************************************************
// **   Count For tempiref monitor                                                           **
// ************************************************************************************
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
          // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
          // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
          // Min Iref=2.0uA
          // Min Iref=2.5uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;    // Min Iref=3.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;    // Min Iref=5.0uA
          // 2ns Delay
          // 2ns Delay
          // 2ns Delay
          // 8ns Precharge time
          // 10ns Discharge time
          // Min Iref=2.0uA
          // Min Iref=2.5uA
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;    // Min Iref=1.0uA
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;    // Min Iref=2.0uA
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;    // Min Iref=2.5uA
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;    // Min Iref=3.5uA
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;    // Min Iref=5.0uA
//          SetValue(IN_ETLR33,param,tempo7);    // 2ns Delay
//          SetValue(IN_ETLR34,param,tempo8);    // 2ns Delay
//          SetValue(IN_ETLR32,param,tempo9);    // 2ns Delay
//          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);    // 8ns Precharge time
//          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);    // 10ns Discharge time
      // 4KB
//      SetValue(IN_EXTAL1,param,1000); // 1MHz
      // 1MHz
      // t@[ŕύX\
//      SetValue(IN_EXTAL1,param,31);   // 32MHz
      // 32MHz
      // t@[ŕύX\
//////        fprintf(file_count,"TESTSIZE0=%d",GetValue(IN_TESTSIZE0,param));    // 4KB f

//fori	}                                // for i
	// for j
	// for k
// ************************************************************************************
// **   Count@Extra6                                                                        **
// ************************************************************************************
        	// Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2])
            // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2])
        	// Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2])
    // 16KB
        // 32MHz
        // 4KB
//hashi        SetValue(IN_TESTSIZE0,param,0x4000);    // 16KB
      // 1MHz
        // t@[ŕύX\
      // 32MHz
        // t@[ŕύX\
//        SetValue(IN_TESTSIZE0,param,0x1000);    // 4KB
//      SetValue(IN_EXTAL1,param,freq01ns);    // 1MHz
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");    // t@[ŕύX\
//      SetValue(IN_EXTAL1,param,freq32ns);    // 32MHz
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");    // t@[ŕύX\
        // 4KB
	// for i
	// for j
// ************************************************************************************
// **   Distribution@Extra iref=2.5uA VthRead NoGiji_TrimRead                                                                 **
// ************************************************************************************
      // 4KB
      // Log header ON
      // Log header OFF
        // 32MHz
        // t@[ŕύX\
        // VS-Step Default
        // VthRead1&2
    // for i
  // Common Clear
  // *******************
