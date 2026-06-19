/******************************************************************************/
/*!
  @file RX200slc.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the V3300,4000,5000 test system
  @author S.Yamaoka                                                                          
  @date 2010.07.09 Rev.00
*/
/******************************************************************************/
#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "compat.h"
#include "RC04EX_apg.h"
#include "RC04EX_tst.h"
#include "RC04EX_fnc.h"

#include "RC04EX_slc_MonitorTrimingSelect.c"
#include "RC04EX_slc_MinoriSelect.c"
#include "RC04EX_slc_Sinraisei.c"
#include "RC04EX_slc_ReadWindow.c"
#include "RC04EX_slc_Disturb.c"
#include "RC04EX_slc_VthTrim.c"
#include "RC04EX_slc_WriteErase.c"
#include "RC04EX_slc_LogicSelect.c"
#include "RC04EX_slc_FCU_function.c"
#include "RC04EX_slc_Sample_Set.c"
#include "RC04EX_slc_Fken.c"
#include "RC04EX_slc_ReadShmooSelect.c"

/******************************************************************************/
/*      start test program function (DoTestProgram(0) from fshell.c)          */
/******************************************************************************/
int AnalisysTestProgram(interface_t* param,int test_cond)
{
  int waittime,datasize;
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7,tempo8;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int stack7,stack8,stack9,stack10,stack11,stack12,stack13;
  int ivsa_loop, ivsa_loop_max,start_adr,end_adr,adr;
  int max_vol,min_vol,result,vth_mode,bit_count,iparh,iparl,delta_ipar;
  int vs[NUM_POWERSUPPLY];
  int i,j,l;
  int trmr7,edge_flag;
  int cnt;
  int selvol,mode;
  int ind[25][2];
  int info[32];                    /* wmat chip information              */
  int vth_data[2][200];  
  int bdata00_bak,extal1_bak;
  int current_status;
  int addr_start[3],addr_size[3],area_set,houwa_flag;

  int vddh,loop_num,loop_max,loop_min,loop_delta,repeat_mode,first_flag;
  int ffpt_bak;
/***********************************/
  int k,m,loop,loop1,loop2,loop3,loop4;
  int old_testr10;
  int old_testr16_flp;
  int old_testr9_e2p;
  int old_speed;
  int old_pll;
  int old_pllon;
  int old_trmr7;
  int old_trmr11;
  int old_trmr13;
  int old_trmr14;
  int old_trmr17;
  int old_trmr18;
  int old_trmr19;
  int old_vcc;
  int old_vcc_mode;
  char datapat[256];
/***********************************/
  char time_buf[32];
  char* cp;

  signed char buf0;
  FILE_T *fpt;
  char wmatfbm[EXTRAS_SIZE];
  WmatStructure chipinfo;               /* Work-Mat(Extra5) Chip information */
  char pat_name[128];
  char item_name[128];
  char item_name2[128];
  char id_name[128];
  char random_pat_path[256];
  char file_name[256];
  char mat_value[60];
  char debug_ptnlist[10][50];
  FILE_T *file_point;

  vs[0] = INIT_VCC;
  vs[1] = vs[2] = 500;
  test = 1;   /* test number */
  Ffpt = INIT_FFPT; /* output log file & display flag(off) */
  waittime = INIT_WAIT;  /* Polling wait time (n x 10ms) */
  //fc_key = 0xFFFFFFFF;
  fc_key |= FK_DISP_DEBUG;
  
  memset(mat_value, 0, sizeof(mat_value));

  /***   e2p program mode�@�C���v   ***/
  glob_pmode_flp = 0;
  glob_pmode_e2p = 1;

  if(test_cond == 0)  InitialValue(param);
  test_cond = INIT_COND;

  if(TesterTypeSelect(Tester_name)) return(FAIL);

  time(&time_list); time_list += V4V5K_TIME_OFS;

  while( test_cond != QUIT){

    printf("+=====================================================================================================+\n");
    printf("| %s device analysis program %s\n",val_devtype,Tester_name);
    printf("| Copyright 2012 Renesas Electronics Corporation. All rights reserved\n");
    printf("| Program Name: %s.%d\n",program_name,0);
#ifdef V5KONLY
    printf("| V5000 site number:%d\n",get_site_number());
#endif
#ifdef V4KONLY
    printf("| V4000\n");
#endif
    printf("+=====================================================================================================+\n");
    printf("| MODE     | SELECT NUMBER\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Read     | CPU        :ALL 0or1(10) / Physical Checker-AorB(12) / Diagonal(14)\n");
    printf("|          |            :Row_Sripe-AorB(15) / Colum_Stripe-AorB(17) / dump(25) / dump_all(66)\n");
    printf("|          | VthRead    :Edge(23) / Count(24) / Distribution(26) / Distribution_Stripe(27)/\n");
    printf("|          | Read Limit :Normal Read(405) / FreqChange_CHK(412)\n");
    printf("|          | Other      :Pass-Code&SampleNo(789) / Iref vs VthRead(28)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|Program   | ALL0(50) / ALL1(51) / Phy-CHK_AorB(52) / Log-CHK_AorB(53)\n");
    printf("|(Write)   | Diagonal(54) / Row_Stripe(55) / Colum_Stripe(57)\n");
    printf("|          | U-MAT-ALL0(58) / EXTRA1~6-ALL0(59) / Triming Data(62)\n");
    printf("|          | Lot info(63) / Sample No.&Ret Cond(64) / Tcheck-Write ALL0(65) / Tcheck-Write ALL1(67) / QT_Ret Cond(68)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Erase    | ALL(30) / Without Prewrite(31) / EXTRA1~6-ALL1(32) \n");
    printf("|          | Tcheck-Erase(33) / Tcheck-Erase WithOut PreWrite(34) / Suspend:1msx5(35)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| P/E      | ALL0(5030) / ALL1(5031) / RowStripe0(5530) / RowStripe1(5531) / PE1kai&Vthserch(5051)  \n");
    printf("|          | PE_Nkai&Vthserch(5052) / renzokuERS&Vthserch(5053) / renzokuVthserch(5054) / renzokuERS&Vthserch(5053) \n");
    printf("|          | Extra3_E_Vth(5060) \n");
    printf("| Disturb  | ColumnStripeA(5730) / ColumnStripeB(5731) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Screening| Erase_0.4s(500) / WriteInhibit for 1stLot WS 100-200ms(510) / 50ms(511) \n");
    printf("|          | PreQT_Erroneous_WriteB(521) / \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Register | Read(60) / Write(61) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Menu     | Monitor&Triming(100)/PE-cycle&VthSearch(111)(121)/Minori-Menu(200)/Sinraisei(300) / QT_Sinraisei(303)\n");
    printf("|          | / EXTRA3_Retention(310)(311)\n");
    printf("|          | Read-Window(400)/Disturb(600)/Bit-Leak(700)/VthTrim(800)/VthTrim_Auto(801) / VthTrim_Eval(802) / VthTrim_deltaVth(803)\n");
    printf("|          | Write&Erase(900)/SeparateLoop(901)/LogicMenu(112)/RDSHM(113)/CurrentMenu(1000)/BGRMenu(1100)/Endurance_Debug(1200)\n");
    printf("|          | FCU_Function(1300)/Sample-Set(1400)/F-Ken(1500)/WT-ETLR-Read(1600)/RomBoot-Menu(1700)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| INIT     | Open&Short(1) / DeviceFunc[PASS](4) / DeviceFunc[FAIL](5)\n");
    printf("|          | Param Reset(6) / Tokunin_Init[FreeWord](8) / Tokunin_Init(9)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| exit(e) / quit(q) / system(sy) / save_Reg.(sreg) / load_Reg.(lreg) / logcap:%1d(log)\n",Ffpt);
    printf("+-----------------------------------------------------------------------------------------------------+\n");
    printf("Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();
    BL_DATACLEAR(param);

    /**************************************************************************/
    /* Commans Analisys                                                       */
    /**************************************************************************/
    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);
    switch(test_cond){
    case EXIT:
      printf("\nDo you want really break !!\n Yes or No = Yes=1/No=0 Enter -> ");
      DigitInput(&tempo);
      if(tempo){
        printf("\n !!! Are You Tired !!!\n !!! Good bye !!!\n");
        DeviceSpecificPowerDown();
        if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
        return(PASS);
      }
      break;
    case QUIT:
      DeviceSpecificPowerDown();
      if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
      PowerDown(1);
      break;
      
      
      
    /**************************************************************************/
    /* INIT(0~9)                                                              */
    /**************************************************************************/
    case 0:  /***  IF->DEVICEFUNC->SET I/F FROM Reg(T_MAT) INFO (0) ***/
      //if ( PASS == OpensShorts() ) {
        DeviceSpecificPowerUp();

        stack0 = SetValue(IN_REG,param,CHIPDATA);
        stack1 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"Register Read") ) {
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
          tempo1 = GetValue(OUT_ETCR17,param);

          GetChipInfo( &info[0], param);

          if(tempo1 != 0x00){
            printf("\n --------- Warning!!! ---------\n");
            printf(  " --- Trimming Read Error!!! ---\n");
            printf(  " ------------------------------\n");
          }

          printf("************************************\n");
          printf("***   INIT PASS                  ***\n");
          printf("************************************\n");
          printf("\n");
        } else {
          printf("------------------------------------\n");
          printf("---   INIT FAIL                  ---\n");
          printf("------------------------------------\n");
          printf("\n");
        }      
      /*
      } else { 
        printf("------------------------------------\n");
        printf("---   PIN FAIL                  ---\n");
        printf("------------------------------------\n");
        printf("\n");
      }*/ 
      if(WSLot_CHECK() == FAIL) TempCheck(); // Extra5 Read
      
      break;
    case 1:   /*** OpensShorts ***/
      OpensShorts();
      DeviceSpecificPowerUp();
      break;
    case 4:   /*** DeviceFunc[PASS] ***/
      stack0 = SetValue(IN_TESTSEL,param,0x00);    /* 0x00 pass Only */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS");
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 44:   /*** DeviceFunc[PASS] ***/
      stack0 = SetValue(IN_TESTSEL,param,0x44);    /* 0x00 pass Only */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS");
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 5:   /*** DeviceFunc[FAIL] ***/
      stack0 = SetValue(IN_TESTSEL,param,0x01);    /* 0x01 Fail Only */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG FAIL");
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 6:
      InitialValue(param);
      break;
    case 8:
      printf("Enter Free-Word ->\n");
      scanf("%s",&item_name);
      Tokunin_Init(param,item_name);
      break;
    case 9:
      Tokunin_Init(param,"");
      break;
    
    /**************************************************************************/
    /*                  READ                                                  */
    /**************************************************************************/
    case 10:  /*** CPU READ ***/
      printf("enter Read-Data ( ALL0:0 / ALL1:1 ) -> ");
      DigitInput(&tempo);
      stack0 = SetValue(IN_TESTSEL,param,tempo);
      stack1 = SetValue(IN_BDATA00,param,0x00);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");  /*�t�@�[������ŕύX����\������*/
      /*
      if( PASS!=tempo0 ) {
        printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );
      }
      */
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 12:  /*** READ Physical Checker ***/
      printf("PHYCHK mode ( CHK-A:0 / CHK-B:1 ) -> "); /*�t�@�[������ŕύX����\������*/
      DigitInput(&tempo); 
      stack0 = SetValue(IN_TESTSEL,param,tempo);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readchecker_code_at"),"CPU READ Checker");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 14:  /*** READ Diagonal ***/
      CpuModeFunc( MNORMAL, MSNORMAL, &vs[0], W1SEC*waittime, param, CreatePatFileName( "readdiagonal_code_at" ), "CPU READ Diagonal" );  /*�t�@�[������ŕύX����\������*/
      break;
    case 15: /*** READ Row-Stripe debug add***/
      printf( "Row Stripe mode ( Stripe-A:0 / Stripe-B:1 ) -> " );/*�t�@�[������ŕύX����\������*/
      DigitInput( &tempo );
      stack0 = SetValue( IN_TESTSEL, param, tempo );
      tempo0 = CpuModeFunc( MNORMAL, MSNORMAL, &vs[0], W1SEC*waittime, param, CreatePatFileName( "readstripe_code_at" ), "CPU READ rowStripe" );  /*�t�@�[������ŕύX����\������*/
      if( PASS!=tempo0 ) 
      printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
      SetValue( IN_TESTSEL, param, stack0 );
      break;
    case 17:  /*** READ Colum-Stripe debug add***/
      printf( "Colum Stripe mode ( Stripe-A:0 / Stripe-B:1 ) -> " );/*�t�@�[������ŕύX����\������*/
      DigitInput( &tempo );
      stack0 = SetValue( IN_TESTSEL, param, tempo+2 );
      tempo0 = CpuModeFunc( MNORMAL, MSNORMAL, &vs[0], W1SEC*waittime, param, CreatePatFileName( "readstripe_code_at" ), "CPU READ columStripe" );  /*�t�@�[������ŕύX����\������*/
      if( PASS!=tempo0 ) 
      printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
      SetValue( IN_TESTSEL, param, stack0 );
      break;
    case 21:  /*** READ LocalDiag debug add***/
       printf( "Local Diag mode ( Diag-A:0 / Diag-B:1 ) -> " );/*�t�@�[������ŕύX����\������*/
       DigitInput( &tempo ); 
       stack0 = SetValue(IN_TESTSEL, param, tempo );
       tempo0 = CpuModeFunc( MNORMAL, MSNORMAL, &vs[0], W1SEC*waittime, param, CreatePatFileName( "readdata_at" ), "CPU READ Diagonal" );  /*�t�@�[������ŕύX����\������*/
       if( PASS!=tempo0 ) {
       printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
       }
       SetValue( IN_TESTSEL, param, stack0 );
       break;
    case 19:  /*** READ Logical Checker debug add***/
       printf( "LOGCHK mode ( CHK-A:0 / CHK-B:1 ) -> " );/*�t�@�[������ŕύX����\������*/
       DigitInput( &tempo ); 
       stack0 = SetValue(IN_TESTSEL,param,tempo);
       tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdata_at"),"CPU READ_LOGCHECKER");  /*�t�@�[������ŕύX����\������*/
       if( PASS!=tempo0 ) {
       printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
       }
       SetValue(IN_TESTSEL,param,stack0);
       break;
    case 404:  /*** READ Triming Data debug add***/
       stack0 = SetValue(IN_REG,param,1);
       tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdata_at"),"CPU READ_TrimData");  /*�t�@�[������ŕύX����\������*/
       if( PASS!=tempo0 ) {
       printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
       }
       SetValue(IN_REG,param,stack0);
       break;
    case 405:  /*** READ limit window debug add***/
       printf( "enter No (mitei) -> " ); /*�t�@�[������ŕύX����*/
       DigitInput( &tempo ); 
       stack0 = SetValue(IN_TESTSEL,param,tempo);
       tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdata_at"),"Read Limit");  /*�t�@�[������ŕύX����\������*/
       if( PASS!=tempo0 ) {
       printf( "\nFail Address=0x%08X, Fail Data=0x%08X, Expect Data=0x%08X\n", GetValue( OUT_LDATA0, param ), GetValue( OUT_LDATA1, param ), GetValue( OUT_LDATA2, param ) );/*�t�@�[������ŕύX����\������*/
       }
       SetValue(IN_TESTSEL,param,stack0);
       break;
    case 412:
      printf("enter Read-Data ( CHKA:0 / CHKB:1 ) -> ");
      DigitInput(&tempo);

      stack0 = SetValue(IN_TESTSEL,param,tempo);
      stack1 = SetValue(IN_BDATA00,param,0x00);
      stack2 = GetValue(IN_EXTAL1,param);

	ffpt_bak = Ffpt;
      for(loop_num=1; loop_num<=2; loop_num++){
        switch(loop_num){
	  case 1:  // VDD
	    loop_max   = 1130;
	    loop_min   = 690;
	    loop_delta = 20;
	    sprintf(item_name,"VDD_mV"); 
	  break; 
          case 2:  // VDDH
	    loop_max   = 1430;
	    loop_min   = 1070;
	    loop_delta = 20;
	    sprintf(item_name,"VDDH_mV");
	  break;
        }
        //Initial Setting
	vs[1]=vs[2]=500;
	
	
	// FileName Setting
        sprintf(file_name,"CPU_SHOMM_CHK_%s",item_name);
        if(PASS != OpenDataOutFile(file_name)) delay_timer(2*1000*1000);
	
	// File Hedder
        fprintf(Fptdata," module,%s \n patten,CPU_CHK \n vcc,%d,vddh,%d,vdd,%d\n",DispMatName(mat_value,param),vs[0],vs[1],vs[2]);
	if(GetValue(IN_BDATA00,param) == 0){
	  fprintf(Fptdata," PLL OFF,");
	}else{
	  fprintf(Fptdata," PLL ON,");
	}
        fprintf(Fptdata,"PLL*,%d,",GetValue(IN_BDATA01,param));
	fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",item_name);
        CloseDataOutFile();delay_timer(200);
        
        for(loop=loop_max;loop>=loop_min;loop=loop-loop_delta){ //Loop
           OpenDataOutFile(file_name);

	   switch(loop_num){
	     case 1: //Vdd Setting
               vs[2]=loop*100/158;
	       fprintf(Fptdata,"%d",loop); 
	     break; 
             case 2: //Vddh Setting
               vs[1]=loop*10/25;
	       fprintf(Fptdata,"%d",loop); 
	     break;
	   }
	 
           for(tempo1=0;tempo1<=15;tempo1++){
               switch(tempo1){
	         case 0 :SetValue(IN_EXTAL1,param,1000);break;
	         case 1 :SetValue(IN_EXTAL1,param,100);break;
           case 2 :SetValue(IN_EXTAL1,param,50);break;
	         case 3 :SetValue(IN_EXTAL1,param,40);break;
	         case 4 :SetValue(IN_EXTAL1,param,33);break;
	         case 5 :SetValue(IN_EXTAL1,param,32);break;
	         case 6 :SetValue(IN_EXTAL1,param,31);break;
	         case 7 :SetValue(IN_EXTAL1,param,30);break;
	         case 8 :SetValue(IN_EXTAL1,param,29);break;
	         case 9 :SetValue(IN_EXTAL1,param,28);break;
	         case 10:SetValue(IN_EXTAL1,param,25);break;
	         case 11:SetValue(IN_EXTAL1,param,24);break;
	         case 12:SetValue(IN_EXTAL1,param,23);break;
	         case 13:SetValue(IN_EXTAL1,param,22);break;
	         case 14:SetValue(IN_EXTAL1,param,21);break;
	         case 15:SetValue(IN_EXTAL1,param,20);break;
	       }
	       printf("<<< loop = %4d ,Vdd =%4dmV, Vddh =%4dmV T = %4dns >>>\n",loop,vs[2],vs[1],GetValue(IN_EXTAL1,param));
               DeviceSpecificPowerUp();delay_timer(200);

               waittime = 4;
	       Ffpt = 0;
               result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readchecker_pll_at"),"CPU READ Checker");
      		   delay_timer(10*1000);
          if(result == PASS){
                 fprintf(Fptdata,",P");
	       }else if(result == FAIL){
	         fprintf(Fptdata,",F");
	       }else{
	         fprintf(Fptdata,",E");
		   }
	     
           }//speed Loop
	   fprintf(Fptdata,"\n");
	   CloseDataOutFile();delay_timer(200);
         }//vdd or reg Loop
      }//loop_num Loop
	  Ffpt = ffpt_bak;

      SetValue(IN_EXTAL1,param,stack2);
      SetValue(IN_TESTSEL,param,stack0);

      break;
      
/*    case 420:   �O���d���������CPURead
      printf("enter Read-Data ( ALL0:0 / ALL1:1 ) -> ");
      DigitInput(&tempo);
      SetValue(IN_TESTSEL,param,tempo);
      printf("enter voltage  -> "); 
      DigitInput(&tempo0);
      selvol = tempo0;
      if(selvol < 0){
        mode = VSSMON_APP;
        SetValue(IN_BDATA00,param,0x01);
      }else if(0 < selvol && selvol < 1250){
        mode = VSSMON_APP;
        SetValue(IN_BDATA00,param,0x02);
      }else if(selvol <= 1250){
        mode = VCCMON_APP;
        SetValue(IN_BDATA00,param,0x03);
      }else{
        ("Input Error\n");
        break;
      }
      CpuModeMonitor(mode, &vs[0], param,W1SEC*waittime,CreatePatFileName("readall_code_at"),"CPU READ ALL", &selvol);
      break;
*/
/******** Vth-Read ********/
    case 23: /*Vth Read Search*/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select Mode -> ");
      DigitInput(&tempo);
      printf("enter delta_vol(mV) -> ");
      DigitInput(&tempo0);
      result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);
      break;
	case 24: /* Vth Test*/
      stack0 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack1 = SetValue(IN_BDATA00,param,0);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);        // VthRead count mode
      SetValue(IN_BDATA00,param,stack1);
	  printf("LDATA = %d\n",GetValue(OUT_LDATA0,param));
      break; 
	case 25: /* Vth Test*/
      stack0 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack1 = SetValue(IN_BDATA00,param,0);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);        // VthRead count mode
      SetValue(IN_BDATA00,param,stack1);
      break;
      
    case 66:
      printf("  ========================================= \n");
      printf(" | VOLTAGE APPLICATION OFF => 0V           |\n");
      printf(" | VOLTAGE APPLICATION ON  => -2000~3300mV |\n");
      printf("  ========================================= \n");
      printf(" | Exit  (e)                               |\n");
      printf("  ========================================= \n");
      printf("Please enter The Voltage for VthRead -> ");
      DigitInput( &tempo );

      if(tempo == EXIT){
        break;
      }else if(tempo != 0){
        vth_mode = 0;
        bdata00_bak = 0;
        extal1_bak  = 0;
        extal1_bak = SetValue(IN_EXTAL1,param,VTH_FREQ);
        SetValue(IN_BDATA01,param,0); //BDATA01 Clear
        selvol = tempo;

        if(tempo >= -2000 && tempo < 0 ){
          mode = VSSMON_APP; //Connect PMU <=> VSSMON
          vth_mode = 1;
        }else if(tempo > 0 && tempo < 1250){
          mode = VSSMON_APP; //Connect PMU <=> VSSMON
          vth_mode = 2;
        }else if(tempo >= 1250 && tempo <= 3300){
          mode = VCCMON_APP; //Connect PMU <=> VCCMON
          vth_mode = 3;
        }else { printf("-- FAIL -- Invalid Voltage\n"); break;}

        // param Set
        bdata00_bak = SetValue(IN_BDATA00,param,vth_mode); // VthRead num select

      }else {
        selvol = 0;
        mode = VCCMON_APP;
        SetValue(IN_BDATA00,param,0);
      }

      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TESTSIZE0,param);
      stack2 = GetValue(IN_TESTSIZE1,param);
      stack3 = GetValue(IN_TESTSIZE2,param);
      stack4 = GetValue(IN_TOPADDR0,param);
      stack5 = GetValue(IN_TOPADDR1,param);
      stack6 = GetValue(IN_TOPADDR2,param);
//      SetValue(IN_TESTSIZE0,param,FBM_READSIZE2);
      tempo3=0;
      tempo0 = 0;
      start_adr = stack4;
      end_adr = start_adr + stack1;
      loop_max = stack1 / FBM_READSIZE2;
      if(stack1 % FBM_READSIZE2 != 0 ) loop_max++;
        first_flag = 1;
        for(i=1;i<=loop_max;i++){
          current_status = i * 100 / loop_max;
          printf("  ************************************************** \n");
          printf("  | VthMode:%d                                      | \n",GetValue(IN_BDATA00,param));
          printf("  | current_status:%d [%%] start_adr:0x%X end_adr:0x%X | \n",current_status,start_adr + (i-1)*FBM_READSIZE2,end_adr);
          printf("  ************************************************** \n");

  //        SetValue(IN_TOPADDR0,param,FBM_READSIZE2*(i-1) + stack4);
          if(first_flag==1){ /* Initial mode */
            first_flag = 0;
            repeat_mode = INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
	        }else{
//            repeat_mode = INIF_SET | OUTIF_READ | RAMBOOT_CHANGE;
            repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
          }
          Ffpt = 0;
//          result = CpuModeMonitor_Repeat(repeat_mode,mode,&vs[0],param,W1SEC*waittime,CreatePatFileName("readdump_code_at"),"CPU READ VthRead",&selvol);
          result = CpuModeMonitor_Repeat(repeat_mode,mode,&vs[0],param,W1SEC*waittime,CreatePatFileName("readdump_ram1_ALL"),"CPU READ VthRead",&selvol);
          Ffpt = 1;
          /*
          printf(" --- Debug OUT ---\n");
          for(j=0;j<=5;j++){
            printf(" L%d : 0x%X ,",j,GetValue(OUT_LDATA0+j,param));
          }printf("\n");
          for(j=0;j<=5;j++){
            printf(" B%d : 0x%X ,",j,GetValue(OUT_BDATA00+j,param));
          }printf("\n");
          printf(" --- Debug IN ---\n");
          for(j=0;j<=5;j++){
            printf(" L%d : 0x%X ,",j,GetValue(IN_LDATA0+j,param));
          }printf("\n");
          for(j=0;j<=5;j++){
            printf(" B%d : 0x%X ,",j,GetValue(IN_BDATA00+j,param));
          }printf("\n");
		  */
          if(result!=PASS)break;

          result = copy_bmecr_offset( ecr1Memory, FBM_READTOP2, FBM_READSIZE2, ecr1Memory, 0x13000+(i-1)*FBM_READSIZE2 );
          tempo3++;
          if(result!=PASS)break;

        }
        sprintf(file_name,"All_FBM_%d.bin",get_site_number());
        result = FbmDataOut_BIST(0x13000, (stack1 + stack2 + stack3), file_name);
        if(result!=PASS){
          printf("  FBM Error!!! Loop Break!\n");
          break;
        }
      SetValue(IN_TESTSIZE0,param,stack1);
      SetValue(IN_TESTSIZE1,param,stack2);
      SetValue(IN_TESTSIZE2,param,stack3);
      SetValue(IN_TOPADDR0,param,stack4);
      SetValue(IN_TOPADDR1,param,stack5);
      SetValue(IN_TOPADDR2,param,stack6);
	  DeviceLevelsPowerDown(); //�e�d��OFF�R�}���h

      if(tempo != 0){
        // param Return //
        SetValue(IN_BDATA00,param,bdata00_bak);
        SetValue(IN_EXTAL1,param,extal1_bak);
      }

      break;
    case 26:
    case 27:
      tempo1 = SetValue(IN_EXTAL1,param,VTH_FREQ);
      tempo2 = 50;  /* VS-Step Default */
      tempo3 = 1;
      if ( test_cond == 26 ) tempo4 = 0;
      if ( test_cond == 27 ) tempo4 = 1;
      vth_mode = 0;
      SetValue(IN_BDATA01,param,0);
      while(tempo != EXIT){
      while(1){
        printf("\n");
        if ( test_cond == 26 ) {
        printf("  =============================== \n");
        printf(" |  ###  ALL Pattern  ###        |\n");
        printf(" |  ###  ALL Pattern  ###        |\n");
        printf(" |  ###  ALL Pattern  ###        |\n");
        } else if ( test_cond == 27 ) {
        printf("  =============================== \n");
        printf(" | ***  Stripe Pattern  ***      |\n");
        printf(" | ***  Stripe Pattern  ***      |\n");
        printf(" | ***  Stripe Pattern  ***      |\n");
        printf("  ------------------------------- \n");
        switch (tempo3) {
          case  1:  printf(" | Mode: WL-Even                 | \n"); break;
          case  2:  printf(" | Mode: WL-Odd                  | \n"); break;
          case  3:  printf(" | Mode: BL-Even                 | \n"); break;
          case  4:  printf(" | Mode: BL-Odd                  | \n"); break;
          case  5:  printf(" | Mode: WL-Even / BL-Even       | \n"); break;
          case  6:  printf(" | Mode: WL-Even / BL-Odd        | \n"); break;
          case  7:  printf(" | Mode: WL-Odd  / BL-Even       | \n"); break;
          case  8:  printf(" | Mode: WL-Odd  / BL-Odd        | \n"); break;
          default:  break;
        }
        }
        printf("  =============================== \n");
        printf(" | VthRead1  (-2.00V~   0V) => 1 |\n");
        printf(" | VthRead2  (    0V~1.25V) => 2 |\n");
        printf(" | VthRead3  ( 1.25V~3.30V) => 3 |\n");
        printf("  ------------------------------- \n");
        printf(" | VthRead1&2(-2.00V~1.25V) =>12 |\n");
        printf(" | VthRead2&3(    0V~2.50V) =>23 |\n");
        printf("  =============================== \n");
        printf(" | Freq   = %2dMHz    (s)         |\n",1000/GetValue(IN_EXTAL1,param));
        printf(" | V-Step = %3dmv    (vs)        |\n",tempo2);
        printf(" | Read-Mode Set     (88)        |\n");
        if(tempo4 == 0){
          printf(" | Edge Search Mode : ON (vt)    |\n");
        }else if(tempo4 == 1){
          printf(" | Edge Search Mode : OFF (vt)   |\n");
        }
        if(tempo2 == 5000){
          printf(" | Dump Mode : ON                |\n");
        }else{
          printf(" | Dump Mode : OFF               |\n");
        }
        printf("  ================================= \n");
        printf(" | Exit  (e)                     |\n");				 
        printf("  ================================= \n");
        printf("  VthRead Select => "); DigitInput( &tempo );
        vth_mode = tempo;

        if(tempo == EXIT){
          break;
        }else if(vth_mode == 1){
          max_vol =  0;         //max = 0V
          min_vol = -2000;      //min = -2V
          printf("  1.vth_mode = %d\n",vth_mode);
          break;
        }else if(vth_mode == 2){
          max_vol = 1250;   //max = 1.25V
          min_vol = 0;      //min = 0V
          printf("  2.vth_mode = %d\n",vth_mode);
          break;
        }else if(vth_mode == 3){
          max_vol = vs[0];   //max = Vcc
          min_vol = 1250;    //min = 1.25V
          printf("  3.vth_mode = %d\n",vth_mode);
          break;
        }else if(vth_mode == 12){
          max_vol = 1250;   //max = 2.5V
          min_vol = -2000;      //min = 0V
          printf("  12.vth_mode = 1&2\n");
          break;
        }else if(vth_mode == 23){
          max_vol = 2500;   //max = 2.5V
          min_vol = 0;      //min = 0V
          printf("  23.vth_mode = 2&3\n");
          break;
        }else if(tempo == SET_SPEED){
	      printf(" Change Freq (ns)=> ");DigitInput( &tempo );
	      SetValue(IN_EXTAL1,param,tempo);
        }else if(tempo == SET_VSTEP){
	      printf(" Vth-Read Step (mV) / DumpON (5000) => ");DigitInput( &tempo2 );
        }else if(tempo == SET_VSTEP){
	      printf(" Vth-Read Step (mV) / DumpON (5000) => ");DigitInput( &tempo2 );
        }else if(tempo == 88){
          printf("\n");
          printf(" ****************************** \n");
          printf(" | No | Mode                  | \n");
          printf(" +----+-----------------------+ \n");
          printf(" |  1 | WL-Even               | \n");
          printf(" |  2 | WL-Odd                | \n");
          printf(" +----+-----------------------+ \n");
          printf(" |  3 | BL-Even               | \n");			  
          printf(" |  4 | BL-Odd                | \n");
          printf(" +----+-----------------------+ \n");
          printf(" |  5 | WL-Even / BL-Even     | \n");
          printf(" |  6 | WL-Even / BL-Odd      | \n");
          printf(" +----+-----------------------+ \n");
          printf(" |  7 | WL-Odd  / BL-Even     | \n");
          printf(" |  8 | WL-Odd  / BL-Odd      | \n");
          printf(" ****************************** \n");
	      printf(" Read Mode Set => ");DigitInput( &tempo3 );
        }else if(tempo == SET_EDGE){
          printf("Edge Seach Mode ON:0 /OFF:1 \n");
          printf("Enter Mode =>");DigitInput( &tempo4 );
        }  
      }
      if ( tempo != EXIT ) {
      if ( test_cond == 27 ) SetValue(IN_BDATA01,param,tempo3);
      printf("  vth_mode = %d\n",vth_mode);
      if(tempo4 == 1 || vth_mode == 3){// Vth�T�[�`����
        printf("  Start min Vol (mV) => "); DigitInput( &tempo );
        min_vol = tempo;
      }else{ // Vth�T�[�`�L��
        if(vth_mode == 1 || vth_mode == 12) mode = 1;       //Vth�T�[�`�pMode�ݒ�
        else if(vth_mode == 2 || vth_mode == 23) mode = 3;  //Vth�T�[�`�pMode�ݒ�
        min_vol = VthEdgeSearch(mode,100,&vs[0],W1SEC*waittime, param);  //Vth�T�[�`���s
	//DeviceSpecificPowerUp();  //VthDist�pPowerUp
      }
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern

      if ( test_cond == 27 ) {
       if ( GetValue(IN_BDATA01,param) < 5 ) bit_count = bit_count/2 ;
       else                                  bit_count = bit_count/4 ;
	  }

      strcpy(item_name,"VthRead_ALL");
      printf("FileName = %s, bit_count = %d, vth_mode = %d\n",item_name,bit_count,vth_mode);
      result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name); //Vth���z����

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }
	  }
	}
    SetValue(IN_BDATA01,param,0);
    SetValue(IN_EXTAL1,param,tempo1);
    tempo = 0;
    break;
    case 28:
      Erase_VthvsIref( &vs[0], waittime, param);
    break;

    case 789: /* W-MAT Info */
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
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

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);
      printf("\n");
      printf("*** WT1 Pass Code : %2X\n",info[13]);
      printf("*** WT2 Pass Code : %2X\n",info[14]);
      printf("*** WT3 Pass Code : %2X\n",info[15]);
      printf("*** WT4 Pass Code : %2X\n",info[16]);
      printf("*** WT5 Pass Code : %2X\n",info[17]);
      printf("*** WT6 Pass Code : %2X\n",info[18]);
      printf("*** WT8 Pass Code : %2X\n",info[19]);
      printf("\n");

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3); 
      SetValue(IN_BDATA00,param,stack4);
      break;
    


    /**************************************************************************/
    /*                  WRITE                                                 */
    /**************************************************************************/
    case 50:  /*** All0 ***/
    case 51:  /*** All1 ***/
      if ( test_cond == 50 ) stack0 = SetValue(IN_TESTSEL,param,0);
      if ( test_cond == 51 ) stack0 = SetValue(IN_TESTSEL,param,1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      break;

    case 52: /*** Physical CHECKER ***/
      printf("PHYCHK mode ( CHK-A:0 / CHK-B:1 ) -> "); /*�t�@�[������ŕύX����\������*/
      DigitInput(&tempo0);
      stack0 = SetValue(IN_TESTSEL,param,tempo0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W30SEC,param,CreatePatFileName("programchecker_code_at"),"Program CHECKER");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0); 
      break;
    case 53:  /*** Logical CHECKER debug add***/
      printf("+----+---------+\n");
      printf("| No | Mode    |\n");
      printf("+----+---------+\n");
      printf("|  1 | 1Byte-A |\n");
      printf("|  2 | 1Byte-B |\n");
      printf("|  3 | 2Byte-A |\n");
      printf("|  4 | 2Byte-B |\n");
      printf("|  5 | 4Byte-A |\n");
      printf("|  6 | 4Byte-B |\n");
      printf("+----+---------+\n");
      printf(" enter No ->");
      DigitInput(&tempo0);
      stack0 = SetValue(IN_TESTSEL,param,tempo0);
      stack1 = SetValue(IN_WRITESIZE,param,glob_pmode_flp);  /*�t�@�[������ŕύX����\������*/
      switch (tempo0) { /*�t�@�[������ŕύX����\������*/
        case 1: tempo = BM_LCHKA1; break;
        case 2: tempo = BM_LCHKB1; break;
        case 3: tempo = BM_LCHKA2; break;
        case 4: tempo = BM_LCHKB2; break;
        case 5: tempo = BM_LCHKA4; break;
        case 6: tempo = BM_LCHKB4; break;
        default:printf(" !!! Input Error !!!\n");break;
      }
      SetValue(IN_TESTSEL,param,tempo);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programchecker_code_at"),"Program CHECKER");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0); SetValue(IN_WRITESIZE,param,stack1);
      break;
    case 54: /*** GLOBAL-DIAG ***/
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W30SEC,param,CreatePatFileName("programdiagonal_code_at"),"Program Global Diagonal");  /*�t�@�[������ŕύX����\������*/
      break;
    case 55: /*** Row STRIPE-A/B debug add***/
      printf("Row Stripe mode ( Stripe-A:0, Stripe-B:1 ) -> "); /* Stripe-A:Even_WL_Only-Write / Stripe-B:Odd_WL_Only-Write */
      DigitInput(&tempo0);
      stack0 = SetValue(IN_TESTSEL,param,tempo0);
      stack1 = SetValue(IN_WRITESIZE,param,0);                  /* 256Byte �����݈ȊO��Ή�*/
      stack2 = GetValue(IN_TOPADDR0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TOPADDR2,param);
      stack5 = GetValue(IN_TESTSIZE0,param);
      stack6 = GetValue(IN_TESTSIZE1,param);
      stack7 = GetValue(IN_TESTSIZE2,param);
      
      if ( tempo0 == 1 ) {                                      /* Stripe-B �͊J�n�Ԓn��0x100�i�߂� */
        if ( GetValue(IN_TESTSIZE0,param) != 0 ) {
          SetValue(IN_TOPADDR0 ,param,GetValue(IN_TOPADDR0,param) +0x100);
          SetValue(IN_TESTSIZE0,param,GetValue(IN_TESTSIZE0,param)-0x100);
        }
        if ( GetValue(IN_TESTSIZE1,param) != 0 ) {
          SetValue(IN_TOPADDR1 ,param,GetValue(IN_TOPADDR1,param) +0x100);
          SetValue(IN_TESTSIZE1,param,GetValue(IN_TESTSIZE1,param)-0x100);
        }
        if ( GetValue(IN_TESTSIZE2,param) != 0 ) {
          SetValue(IN_TOPADDR2 ,param,GetValue(IN_TOPADDR2,param) +0x100);
          SetValue(IN_TESTSIZE2,param,GetValue(IN_TESTSIZE2,param)-0x100);
        }
      }
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programstripe_code_at"),"Program Row-STRIPE"); /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0); 
      SetValue(IN_WRITESIZE,param,stack1);
      SetValue(IN_TOPADDR0,param,stack2);
      SetValue(IN_TOPADDR1,param,stack3);
      SetValue(IN_TOPADDR2,param,stack4);
      SetValue(IN_TESTSIZE0,param,stack5);
      SetValue(IN_TESTSIZE1,param,stack6);
      SetValue(IN_TESTSIZE2,param,stack7);
    break;
    case 57: /*** Colum STRIPE-A/B debug add***/
      printf("Column Stripe mode ( Stripe-A:0, Stripe-B:1 ) -> "); /* Stripe-A:Even_BL_Write / Stripe-B:Odd_WL_Write */
      DigitInput(&tempo0);
      stack0 = SetValue(IN_TESTSEL,param,tempo0);
      stack1 = SetValue(IN_WRITESIZE,param,glob_pmode_flp);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programcolstripe_code_at"),"Program Column-STRIPE"); /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0); 
      SetValue(IN_WRITESIZE,param,stack1);
      break;
    case 58: /*** U-MAT-ALL0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      stack1 = SetValue(IN_ETCR08,param,0x20);
	  stack2 = SetValue(IN_TOPADDR0,param,0);
      stack3 = SetValue(IN_TOPADDR1,param,0);
      stack4 = SetValue(IN_TOPADDR2,param,0);
	  stack5 = GetValue(IN_TESTSIZE0,param);
      stack6 = GetValue(IN_TESTSIZE1,param);
      stack7 = GetValue(IN_TESTSIZE2,param);
      stack8  = SetValue(IN_REG,param,GetValue(IN_REG,param)|0x03);
      if ( GetValue(IN_TESTSIZE0,param) != 0 ) SetValue(IN_TESTSIZE0,param,0x100);
      if ( GetValue(IN_TESTSIZE1,param) != 0 ) SetValue(IN_TESTSIZE1,param,0x100);
      if ( GetValue(IN_TESTSIZE2,param) != 0 ) SetValue(IN_TESTSIZE2,param,0x100);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_ETCR08,param,stack1);
	  SetValue(IN_TOPADDR0,param,stack2);
      SetValue(IN_TOPADDR1,param,stack3);
      SetValue(IN_TOPADDR2,param,stack4);
	  SetValue(IN_TESTSIZE0,param,stack5);
      SetValue(IN_TESTSIZE1,param,stack6);
      SetValue(IN_TESTSIZE2,param,stack7);
      SetValue(IN_REG,param,stack8);
      break;
    case 59:
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_TESTSEL,param,0);
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      for(tempo0=1;tempo0<=6;tempo0++){
        SetValue(IN_AREA,param,tempo0);
        switch(tempo0){
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
            break;
          case 6:
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,EXTRAB_SIZE);
            break;
        }
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Write"); /*�t�@�[������ŕύX����\������*/
      }
      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      break;

      case 65:  /*** All0 ***/
      case 67:  /*** All1 ***/

      printf("====  Tcheck Select  ====\n");
      printf("| E/W    mode (1)       |\n");
      printf("| Vnoemi mode (2)       |\n");
      printf("| Vwi    mode (3)       |\n");
      printf("| Vhh    mode (4)       |\n");
      printf("| Vpp    mode (5)       |\n");
      printf("=========================\n");
      printf(" Select Tcheck Mode ->");
      DigitInput(&tempo1);
      switch(tempo1){
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          SetValue(IN_ETCR11,param,(GetValue(IN_ETCR11,param)&0x1F) | (tempo1 << 5));
        break;
      }
      if(test_cond == 65){
        stack0 = SetValue(IN_TESTSEL,param,0);
      }else{
        stack0 = SetValue(IN_TESTSEL,param,1);
      }
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL (Tcheck)");
      SetValue(IN_TESTSEL,param,stack0);
      if((GetValue(OUT_ETCR16,param) & 0x40) == 0x40){
        printf("\n\n  --- TimeOut Check ERROR!!! --- \n");
        printf(    "      ETCR16 = 0x%X\n\n\n",GetValue(OUT_ETCR16,param));
      }
      break;


    /**************************************************************************/
    /*                  ERASE                                                 */
    /**************************************************************************/
    case 30: 
    case 31:
      if( 31==test_cond ) stack0 = SetValue(IN_BDATA00,param,1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/
      if( 31==test_cond ) SetValue(IN_BDATA00,param,stack0);
      break;

    case 33: 
    case 34:

      printf("====  Tcheck Select  ====\n");
      printf("| E/W    mode (1)       |\n");
      printf("| Vnoemi mode (2)       |\n");
      printf("| Vwi    mode (3)       |\n");
      printf("| Vhh    mode (4)       |\n");
      printf("| Vpp    mode (5)       |\n");
      printf("=========================\n");
      printf(" Select Tcheck Mode ->");
      DigitInput(&tempo1);
      switch(tempo1){
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          SetValue(IN_ETCR11,param,(GetValue(IN_ETCR11,param)&0x1F) | (tempo1 << 5));
        break;
      }
      
      if( 34==test_cond ) stack0 = SetValue(IN_BDATA00,param,1);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase(Tcheck)");
      if( 34==test_cond ) SetValue(IN_BDATA00,param,stack0);
      if((GetValue(OUT_ETCR16,param) & 0x40) == 0x40){
        printf("\n\n  --- TimeOut Check ERROR!!! --- \n");
        printf(    "      ETCR16 = 0x%X\n\n\n",GetValue(OUT_ETCR16,param));
      }

      break;
    case 35:
      stack0 = SetValue(IN_BDATA00,param,0); //PreWrite ON 
      stack1 = SetValue(IN_ETLR37,param,( GetValue(IN_ETLR37,param) & 0xF0 ) | 0x03);//Erase Pulse 1ms
      
      for(i=0;i<5;i++){
        result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        SetValue(IN_BDATA00,param,1); //PreWrite OFF
        if(result != PASS)break;
      }

      SetValue(IN_ETLR37,param,stack1);
      SetValue(IN_BDATA00,param,stack0);
    break;



    case 32:
      SetValue(IN_BDATA00,param,0);
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      for(tempo0=1;tempo0<=6;tempo0++){
        SetValue(IN_AREA,param,tempo0);
        switch(tempo0){
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
            break;
          case 6:
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
            break;
        }
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/
      }
      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      break;
   
    /**************************************************************************/
    /*                  PE Endurance                                          */
    /**************************************************************************/
    case 5030: /* ALL0 */
    case 5031: /* ALL1 */
    case 5530: /* Row Stripe(0data) */
    case 5531: /* Row Stripe(1data) */
    case 5730: /* Column Stripe(Even) */
    case 5731: /* Column Stripe(Odd) */
      printf(" Enter PE Cycle number (ex.1000) -> ");
      DigitInput(&tempo);
      if( tempo<=0 ) break;
      SetValue(IN_LDATA0,param,tempo);
      printf(" Enter PE Wait time(sec) (ex.60) -> ");
      DigitInput(&tempo);
      if( tempo<=0 ) break;
      SetValue(IN_LDATA1,param,tempo);
      printf(" Select Module (0:FLP0, 1:FLI1, 2:FLI2, 3:FLI0&1, 99:User ALL) -> ");
      DigitInput(&tempo0);
      printf(" Enter or Skip Address setting (0:Skip(=All area), 1:Enter) -> ");
      DigitInput(&tempo);
      if(tempo==1){
        if(tempo0!=99){
          printf(" Start Address (ex. 0x1000) -> ");
          DigitInput(&tempo1);
          printf(" End Address (max=0x7FFFF) -> ");
          DigitInput(&tempo2);
        }else{
          printf(" Start Address(FLP0) -> ");
          DigitInput(&tempo1);
          printf(" End Address(FLP0) -> ");
          DigitInput(&tempo2);
          printf(" Start Address(FLI1) -> ");
          DigitInput(&tempo3);
          printf(" End Address(FLI1) -> ");
          DigitInput(&tempo4);
          printf(" Start Address(FLI2) -> ");
          DigitInput(&tempo5);
          printf(" End Address(FLI2) -> ");
          DigitInput(&tempo6);
        }
      }else{
        tempo1=tempo3=tempo5=0x00000000;       /* TOPADDR */
        tempo2=tempo4=tempo6=0x0007FFFF;       /* ENDADDR */
      }

      SetValue(IN_AREA,param,0);
      if(tempo0==0){
        SetValue(IN_TOPADDR0,param,tempo1);             /* FLP0_TOP  */
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1)); /* FLP0_SIZE */
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else if(tempo0==1){
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,tempo1);             /* FLI1_TOP  */
        SetValue(IN_TESTSIZE1,param,(tempo2-tempo1+1)); /* FLP1_SIZE */
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else if(tempo0==2){
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,tempo1);             /* FLI2_TOP  */
        SetValue(IN_TESTSIZE2,param,(tempo2-tempo1+1)); /* FLP2_SIZE */
      }else if(tempo0==3){
        SetValue(IN_TOPADDR0,param,tempo1);             /* FLI0_TOP  */
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1)); /* FLP0_SIZE */
        SetValue(IN_TOPADDR1,param,tempo1);             /* FLI1_TOP  */
        SetValue(IN_TESTSIZE1,param,(tempo2-tempo1+1)); /* FLP1_SIZE */
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else{
        SetValue(IN_TOPADDR0,param,tempo1);
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1));
        SetValue(IN_TOPADDR1,param,tempo3);
        SetValue(IN_TESTSIZE1,param,(tempo4-tempo3+1));
        SetValue(IN_TOPADDR2,param,tempo5);
        SetValue(IN_TESTSIZE2,param,(tempo6-tempo5+1));
      }

      //CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("pecycle_code_at"),"PE Cycle");
      if( 5030==test_cond ){
        SetValue(IN_TESTSEL,param,0); /* ALL0 */
        SetValue(IN_BDATA01,param,0); /* ALL0 paterrn ���[�N���i�[�p */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("pecycle_code_all_at"), "PE Cycle(ALL0)" );
      }else if( 5031==test_cond ){
        SetValue(IN_TESTSEL,param,1); /* ALL1 */
        SetValue(IN_BDATA01,param,1); /* ALL1 paterrn ���[�N���i�[�p */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("pecycle_code_all_at"), "PE Cycle(ALL1)" );
      }else if( 5530==test_cond ){
        SetValue(IN_TESTSEL,param,0); /* Row Stripe(0data) */
        SetValue(IN_BDATA01,param,2); /* Row Stripe(0data) ���[�N���i�[�p */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("pecycle_code_stripe_at"), "PE Cycle(Row Stripe 0)" );
      }else if( 5531==test_cond ){
        SetValue(IN_TESTSEL,param,1); /* Row Stripe(1data) */
        SetValue(IN_BDATA01,param,3); /* Row Stripe(1data) ���[�N���i�[�p */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("pecycle_code_stripe_at"), "PE Cycle(Row Stripe 1)" );
      }else if( 5730==test_cond ){
        SetValue(IN_TESTSEL,param,0); /* Column Stripe(Even) */
        SetValue(IN_BDATA01,param,4); /* Column Stripe(Even) ���[�N���i�[�p */
        SetValue(IN_ETCR07,param,0x01); /* SG/MG non-select */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("disturb_colstripe_code_at"), "Disturb(Column Stripe Even)" );
        SetValue(IN_ETCR07,param,0x00); /* SG/MG non-select */
      }else if( 5731==test_cond ){
        SetValue(IN_TESTSEL,param,1); /* Column Stripe(Odd) */
        SetValue(IN_BDATA01,param,5); /* Column Stripe(Odd) ���[�N���i�[�p */
        SetValue(IN_ETCR07,param,0x01); /* SG/MG non-select */
        CpuModeFunc_PE( 24, MNORMAL, MSNORMAL, &vs[0], W10MIN, param, CreatePatFileName("disturb_colstripe_code_at"), "Disturb(Column Stripe Odd)" );
        SetValue(IN_ETCR07,param,0x00); /* SG/MG non-select */
      }
      break;

    	
    /**************************************************************************/
    /*         Erase & vthSearch & Write & Search  case 5051                  */
    /**************************************************************************/
    case 5051: 
  	  vs[0] = 3300;
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
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

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);
      printf("\n");
      printf("*** WT1 Pass Code : %2X\n",info[13]);
      printf("*** WT2 Pass Code : %2X\n",info[14]);
      printf("*** WT3 Pass Code : %2X\n",info[15]);
      printf("*** WT4 Pass Code : %2X\n",info[16]);
      printf("*** WT5 Pass Code : %2X\n",info[17]);
      printf("*** WT6 Pass Code : %2X\n",info[18]);
      printf("*** WT8 Pass Code : %2X\n",info[19]);
      printf("\n");

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3); 
      SetValue(IN_BDATA00,param,stack4);

      SetValue(IN_AREA,param,0);//FLP0
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);

    vs[0] = 1600;
    /*** All0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);

    /* Erase */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 2;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);

	  sprintf(file_name,"%s%s%c%c%c%c%c%c_w%3d_PE_VthEdge_%d.csv",glob_cdp, glob_datalog,info[0],info[1],info[2],info[3],info[4],info[5],info[10],get_site_number());
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
	  fprintf(file_point,"Erase VthSearch,FLP0+FLI1+FLI2,");
	  fprintf(file_point,"%dmV,",result);

    vs[0] = 1600;
    /*** All0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 3;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

	  fprintf(file_point,"Write VthSearch,FLP0+FLI1+FLI2,");
	  fprintf(file_point,"%dmV,X%3d,Y%3d,S#%3d\n",result,info[11],info[12],info[20]);
      fclose( file_point);
    break;

    /**************************************************************************/
    /*         Erase & vthSearch & Write & Search  case 5052 PE=10K           */
    /**************************************************************************/
    case 5052: 
  	  vs[0] = 3300;
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
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

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);
      printf("\n");
      printf("*** WT1 Pass Code : %2X\n",info[13]);
      printf("*** WT2 Pass Code : %2X\n",info[14]);
      printf("*** WT3 Pass Code : %2X\n",info[15]);
      printf("*** WT4 Pass Code : %2X\n",info[16]);
      printf("*** WT5 Pass Code : %2X\n",info[17]);
      printf("*** WT6 Pass Code : %2X\n",info[18]);
      printf("*** WT8 Pass Code : %2X\n",info[19]);
      printf("\n");

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3); 
      SetValue(IN_BDATA00,param,stack4);

      SetValue(IN_AREA,param,0);//FLP0
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      printf(" P/E cyc count? ex)=1000kai->1000  ->?");
      DigitInput(&tempo7);

    vs[0] = 1600;
    /*** All0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);


      if (tempo != 99){
      for ( j=0 ; j<=tempo7  ; j++ ) {
      /* Erase */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 2;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);

	  sprintf(file_name,"%s%s%c%c%c%c%c%c_w%3d_PE_VthEdge_%d.csv",glob_cdp, glob_datalog,info[0],info[1],info[2],info[3],info[4],info[5],info[10],get_site_number());
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
	  fprintf(file_point,"PEcount=,%d, Erase VthSearch,FLP0+FLI1+FLI2,",j);
	  fprintf(file_point,"%dmV,",result);

    vs[0] = 1600;
    /*** All0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 3;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

	  fprintf(file_point,"Write VthSearch,FLP0+FLI1+FLI2,");
	  fprintf(file_point,"%dmV,X%3d,Y%3d,S#%3d\n",result,info[11],info[12],info[20]);
      fclose( file_point);
        }//for
      }//if
      if (tempo == 99) printf(" PEcyc & Search Eval exit!! \n");
    break;

    /**************************************************************************/
    /*         Erase & vthSearch & Erase & Search  case 5053 ERS-ERS           */
    /**************************************************************************/
    case 5053: 
  	  vs[0] = 3300;
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
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

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);
      printf("\n");
      printf("*** WT1 Pass Code : %2X\n",info[13]);
      printf("*** WT2 Pass Code : %2X\n",info[14]);
      printf("*** WT3 Pass Code : %2X\n",info[15]);
      printf("*** WT4 Pass Code : %2X\n",info[16]);
      printf("*** WT5 Pass Code : %2X\n",info[17]);
      printf("*** WT6 Pass Code : %2X\n",info[18]);
      printf("*** WT8 Pass Code : %2X\n",info[19]);
      printf("\n");

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3); 
      SetValue(IN_BDATA00,param,stack4);

      SetValue(IN_AREA,param,0);//FLP0
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      printf(" P/E cyc count? ex)=1000kai->1000  ->?");
      DigitInput(&tempo7);

    vs[0] = 1600;
    /*** All0 ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);

      if (tempo != 99){
      for ( j=0 ; j<=tempo7  ; j++ ) {
      /* Erase */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 2;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);

	  sprintf(file_name,"%s%s%c%c%c%c%c%c_w%3d_ERSERS_VthEdge_%d.csv",glob_cdp, glob_datalog,info[0],info[1],info[2],info[3],info[4],info[5],info[10],get_site_number());
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
	  fprintf(file_point,"PEcount=,%d, Erase VthSearch,FLP0+FLI1+FLI2,",j);
	  fprintf(file_point,"%dmV,",result);

    vs[0] = 1600;
      /* Erase */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/

    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
       tempo = 2;
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

	  fprintf(file_point,"Erase VthSearch,FLP0+FLI1+FLI2,");
	  fprintf(file_point,"%dmV,X%3d,Y%3d,S#%3d\n",result,info[11],info[12],info[20]);
      fclose( file_point);
        }//for
      }//if
      if (tempo == 99) printf(" PEcyc & Search Eval exit!! \n");
    break;


    /**************************************************************************/
    /*         vthSearch cyc  case 5054                                       */
    /**************************************************************************/
    case 5054: 
  	  vs[0] = 3300;
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
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

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);
      printf("\n");
      printf("*** WT1 Pass Code : %2X\n",info[13]);
      printf("*** WT2 Pass Code : %2X\n",info[14]);
      printf("*** WT3 Pass Code : %2X\n",info[15]);
      printf("*** WT4 Pass Code : %2X\n",info[16]);
      printf("*** WT5 Pass Code : %2X\n",info[17]);
      printf("*** WT6 Pass Code : %2X\n",info[18]);
      printf("*** WT8 Pass Code : %2X\n",info[19]);
      printf("\n");

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3); 
      SetValue(IN_BDATA00,param,stack4);

      SetValue(IN_AREA,param,0);//FLP0
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      printf(" P/E cyc count? ex)=1000kai->1000  ->?");
      DigitInput(&tempo7);
      printf(" ERS serch? PGM search? ERS:0, PGM:1  ->?");
      DigitInput(&tempo8);

      if (tempo != 99){
      for ( j=0 ; j<=tempo7  ; j++ ) {
    vs[0] = 3300;
/******** Vth-Read ********/
      printf("  =============================== \n");
      printf(" | Low_Side  Down           => 1 |\n");
      printf(" | Low_Side  up             => 2 |\n");
      printf(" | High_Side Down           => 3 |\n");
      printf(" | High_Side up             => 4 |\n");
      printf("  =============================== \n");
      printf("Select MOde -> ");
      if (tempo8 == 0){
       tempo = 2;
      }
      else if (tempo8 == 1){
       tempo = 3;
      }
    	printf("enter delta_vol(mV) -> ");
       tempo0 = 20;
    	result = VthEdgeSearch(tempo,tempo0,&vs[0],W1SEC*waittime, param);
      printf(" ----result---- %dmV\n",result);

      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("*** S# : %3d\n",info[20]);

	  sprintf(file_name,"%s%s%c%c%c%c%c%c_w%3d_Search_VthEdge_%d.csv",glob_cdp, glob_datalog,info[0],info[1],info[2],info[3],info[4],info[5],info[10],get_site_number());
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
	  fprintf(file_point,"Searchcount=,%d, VthSearch,FLP0+FLI1+FLI2,",j);
	  fprintf(file_point,"%dmV,\n",result);
      fclose( file_point);
        }//for
      }//if
      if (tempo == 99) printf(" PEcyc & Search Eval exit!! \n");
    break;
	case 5060:
	  Extra_Erase_Vth( &vs[0], waittime, param);
    break;

    /**************************************************************************/
    /*         Screening test of Write Inhibit for WS1                        */
    /**************************************************************************/
    case 500:  /*** Erase 0.4s ***/
      stack0  = SetValue(IN_BDATA00,param,1);                       /* Pre-Write ress */
      stack1  = SetValue(IN_REG,param,GetValue(IN_REG,param)|0x03);
      stack2  = SetValue(IN_ETLR37,param,0xFD );                    /* Puls=20ms      */
      stack3  = GetValue(IN_ETCR08,param);
      stack4  = SetValue(IN_AREA,param,0);                          /* FLP0/FLI1/FLI2 */
      stack5  = GetValue(IN_TOPADDR0,param);
      stack6  = GetValue(IN_TESTSIZE0,param);
      stack7  = GetValue(IN_TOPADDR1,param);
      stack8  = GetValue(IN_TESTSIZE1,param);
      stack9  = GetValue(IN_TOPADDR2,param);
      stack10 = GetValue(IN_TESTSIZE2,param);
      SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | 0x20); /* User Select */
 
      for ( j=0 ; j<3  ; j++ ) {
        SetValue(IN_TESTSIZE0,param,0x0); SetValue(IN_TESTSIZE1,param,0x0); SetValue(IN_TESTSIZE2,param,0x0);
        if ( j==0 ) { SetValue(IN_TOPADDR0,param,0); SetValue(IN_TESTSIZE0,param,0x15000); }  /* FLP0 4KBx20kai */   
        if ( j==1 ) { SetValue(IN_TOPADDR1,param,0); SetValue(IN_TESTSIZE1,param,0x15000); }  /* FLI1 4KBx20kai */   
        if ( j==2 ) { SetValue(IN_TOPADDR2,param,0); SetValue(IN_TESTSIZE2,param,0x15000); }  /* FLI2 4KBx20kai */   
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); 
      }
      SetValue(IN_BDATA00  ,param,stack0); SetValue(IN_REG      ,param,stack1); SetValue(IN_ETLR37   ,param,stack2);
      SetValue(IN_ETCR08   ,param,stack3); SetValue(IN_AREA     ,param,stack4);

      for ( j=0 ; j<3  ; j++ ) {
        SetValue(IN_TESTSIZE0,param,0x0); SetValue(IN_TESTSIZE1,param,0x0); SetValue(IN_TESTSIZE2,param,0x0);
        if ( j==0 ) { SetValue(IN_TOPADDR0,param,0); SetValue(IN_TESTSIZE0,param,FLP0_SIZE); }  /* FLP0           */   
        if ( j==1 ) { SetValue(IN_TOPADDR1,param,0); SetValue(IN_TESTSIZE1,param,FLI1_SIZE); }  /* FLI1           */   
        if ( j==2 ) { SetValue(IN_TOPADDR2,param,0); SetValue(IN_TESTSIZE2,param,FLI2_SIZE); }  /* FLI2           */   
        SetValue(IN_TESTSEL,param,0);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
        SetValue(IN_TESTSEL,param,0); /* ALL0 */
        tempo = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");  /*�t�@�[������ŕύX����\������*/
        if ( tempo != PASS ) break;
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); 
        SetValue(IN_TESTSEL,param,1); /* ALL1 */
        tempo = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");  /*�t�@�[������ŕύX����\������*/
        if ( tempo != PASS ) break;
      }
      if ( tempo == PASS ) { 
        printf("\n");
        printf(" ***************************************** \n");
        printf(" ***************************************** \n");
        printf("                                           \n");
        printf(" PPPPPP        A        SSSSSS    SSSSSS   \n");
        printf(" PPPPPPP      AAA      SSS  SSS  SSS  SSS  \n");
        printf(" PP    PP    AA AA     SS    SS  SS    SS  \n");
        printf(" PP    PP    AA AA     SSS       SSS       \n");
        printf(" PPPPPPP    AA   AA     SSSS      SSSS     \n");
        printf(" PPPPPP     AA   AA       SSSS      SSSS   \n");
        printf(" PP        AAAAAAAAA        SSS       SSS  \n");
        printf(" PP        AAAAAAAAA   SS    SS  SS    SS  \n");
        printf(" PP       AA       AA  SSS  SSS  SSS  SSS  \n");
        printf(" PP       AA       AA   SSSSSS    SSSSSS   \n");
        printf("                                           \n");
        printf(" ***************************************** \n");
        printf(" ***************************************** \n");
        printf("\n");
      } else {
        printf("\n");
        printf(" FFFFFFFF      A       IIIIII  LL        \n");
        printf(" FFFFFFFF     AAA      IIIIII  LL        \n");
        printf(" FF          AA AA       II    LL        \n");
        printf(" FF          AA AA       II    LL        \n");
        printf(" FFFFFFF    AA   AA      II    LL        \n");
        printf(" FFFFFFF    AA   AA      II    LL        \n");
        printf(" FF        AAAAAAAAA     II    LL        \n");
        printf(" FF        AAAAAAAAA     II    LL        \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  \n");
        printf(" --------------------------------------- \n");
        if ( j == 0 ) printf("   FLP0  \n");
        if ( j == 1 ) printf("   FLI1  \n");
        if ( j == 2 ) printf("   FLI2  \n");
        printf(" --------------------------------------- \n");
        printf("\n");
      }      

      SetValue(IN_TOPADDR0 ,param,stack5); SetValue(IN_TESTSIZE0,param,stack6);
      SetValue(IN_TOPADDR1 ,param,stack7); SetValue(IN_TESTSIZE1,param,stack8);
      SetValue(IN_TOPADDR2 ,param,stack9); SetValue(IN_TESTSIZE2,param,stack10);
      SetValue(IN_TESTSEL,param,0);
      break;

    case 510:  /*** Vhh=7.5V 200ms(Cat1)/150ms(Cat2)/100ms(Cat3) ***/
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x03);
      stack0 = SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0x00);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_TESTSEL,param,1);
      tempo1 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( (PASS!=tempo0)||(PASS!=tempo1) ) {
        printf("\n");
        printf("tempo0(Program) = %d\n",tempo0);
        printf("tempo1(Erase)   = %d\n",tempo1);
        printf("  IIIIII  NN     NN  IIIIII  IIIIII  FFFFFFFF     A       IIIIII  LL       \n");
        printf("  IIIIII  NNN    NN  IIIIII  IIIIII  FFFFFFFF    AAA      IIIIII  LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("  IIIIII  NN    NNN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("  IIIIII  NN     NN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("\n");
        break;
      }

      SetValue(IN_TESTSEL,param,1);
//      stack2 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x10); /* Vhh=7.0V */
//      stack3 = SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param)&0xF0)|0x0C); /* ref current 3uA */
      stack2 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x1C); /* Vhh=7.5V */
      stack3 = SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param)&0xF0)|0x0E); /* ref current 4uA */
      stack4 = SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param)&0xF0)|0x0E); /* 10ms */
      /*** Program ALL1 100ms ***/
      stack1 = SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20); /* User Sector, 0x40=User+E2-6, 0x80=User+E1-6 */
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /*** Read ALL1 @4uA ***/
      SetValue(IN_ETCR08,param,stack1);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( PASS!=tempo0 ) {
        printf("\n");
        printf("tempo0 = %d\n",tempo0);
        printf(" FFFFFFFF      A       IIIIII  LL        ==   CCCCCC        A      IIIIII   999999  \n");
        printf(" FFFFFFFF     AAA      IIIIII  LL        ==  CCC  CCC      AAA     IIIIII  999  999 \n");
        printf(" FF          AA AA       II    LL        ==  CC    CC     AA AA      II    99    99 \n");
        printf(" FF          AA AA       II    LL        ==  CC           AA AA      II    999  999 \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II     9999999 \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II          99 \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC         AAAAAAAAA    II          99 \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC    CC   AAAAAAAAA    II          99 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==  CCC  CCC  AA       AA   II         999 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==   CCCCCC   AA       AA   II        999  \n");
        printf("\n");
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR25,param,stack3);
        SetValue(IN_ETLR36,param,stack4);
        break;
      }

      /*** Program ALL1 150ms ***/
      SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20); /* User Sector, 0x40=User+E2-6, 0x80=User+E1-6 */
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 150ms) */
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 150ms) */
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 150ms) */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /*** Read ALL1 @4uA ***/
      SetValue(IN_ETCR08,param,stack1);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( PASS!=tempo0 ) {
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        printf("\n");
        printf(" PPPPPP        A        SSSSSS    SSSSSS   ==   CCCCCC        A      IIIIII   333333  \n");
        printf(" PPPPPPP      AAA      SSS  SSS  SSS  SSS  ==  CCC  CCC      AAA     IIIIII  33    33 \n");
        printf(" PP    PP    AA AA     SS    SS  SS    SS  ==  CC    CC     AA AA      II          33 \n");
        printf(" PP    PP    AA AA     SSS       SSS       ==  CC           AA AA      II         33  \n");
        printf(" PPPPPPP    AA   AA     SSSS      SSSS     ==  CC          AA   AA     II      3333   \n");
        printf(" PPPPPP     AA   AA       SSSS      SSSS   ==  CC          AA   AA     II      3333   \n");
        printf(" PP        AAAAAAAAA        SSS       SSS  ==  CC         AAAAAAAAA    II         33  \n");
        printf(" PP        AAAAAAAAA   SS    SS  SS    SS  ==  CC    CC   AAAAAAAAA    II          33 \n");
        printf(" PP       AA       AA  SSS  SSS  SSS  SSS  ==  CCC  CCC  AA       AA   II    33    33 \n");
        printf(" PP       AA       AA   SSSSSS    SSSSSS   ==   CCCCCC   AA       AA   II     333333  \n");
        printf("\n");
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR25,param,stack3);
        SetValue(IN_ETLR36,param,stack4);
        break;
      }

      /*** Program ALL1 200ms ***/
      SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20); /* User Sector, 0x40=User+E2-6, 0x80=User+E1-6 */
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 200ms) */
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 200ms) */
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x500);   /* address loop count = 5 -> stress time = 50ms(Total 200ms) */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /*** Read ALL1 @4uA ***/
      SetValue(IN_ETCR08,param,stack1);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( PASS!=tempo0 ) {
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        printf("\n");
        printf(" PPPPPP        A        SSSSSS    SSSSSS   ==   CCCCCC        A      IIIIII   222222  \n");
        printf(" PPPPPPP      AAA      SSS  SSS  SSS  SSS  ==  CCC  CCC      AAA     IIIIII  22    22 \n");
        printf(" PP    PP    AA AA     SS    SS  SS    SS  ==  CC    CC     AA AA      II          22 \n");
        printf(" PP    PP    AA AA     SSS       SSS       ==  CC           AA AA      II         22  \n");
        printf(" PPPPPPP    AA   AA     SSSS      SSSS     ==  CC          AA   AA     II        22   \n");
        printf(" PPPPPP     AA   AA       SSSS      SSSS   ==  CC          AA   AA     II       22    \n");
        printf(" PP        AAAAAAAAA        SSS       SSS  ==  CC         AAAAAAAAA    II      22     \n");
        printf(" PP        AAAAAAAAA   SS    SS  SS    SS  ==  CC    CC   AAAAAAAAA    II     22      \n");
        printf(" PP       AA       AA  SSS  SSS  SSS  SSS  ==  CCC  CCC  AA       AA   II    22       \n");
        printf(" PP       AA       AA   SSSSSS    SSSSSS   ==   CCCCCC   AA       AA   II    22222222 \n");
        printf("\n");
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR25,param,stack3);
        SetValue(IN_ETLR36,param,stack4);
        break;
      }else{
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        printf("\n");
        printf(" PPPPPP        A        SSSSSS    SSSSSS   ==   CCCCCC        A      IIIIII   111   \n");
        printf(" PPPPPPP      AAA      SSS  SSS  SSS  SSS  ==  CCC  CCC      AAA     IIIIII  1111   \n");
        printf(" PP    PP    AA AA     SS    SS  SS    SS  ==  CC    CC     AA AA      II      11   \n");
        printf(" PP    PP    AA AA     SSS       SSS       ==  CC           AA AA      II      11   \n");
        printf(" PPPPPPP    AA   AA     SSSS      SSSS     ==  CC          AA   AA     II      11   \n");
        printf(" PPPPPP     AA   AA       SSSS      SSSS   ==  CC          AA   AA     II      11   \n");
        printf(" PP        AAAAAAAAA        SSS       SSS  ==  CC         AAAAAAAAA    II      11   \n");
        printf(" PP        AAAAAAAAA   SS    SS  SS    SS  ==  CC    CC   AAAAAAAAA    II      11   \n");
        printf(" PP       AA       AA  SSS  SSS  SSS  SSS  ==  CCC  CCC  AA       AA   II    111111 \n");
        printf(" PP       AA       AA   SSSSSS    SSSSSS   ==   CCCCCC   AA       AA   II    111111 \n");
        printf("\n");
      }
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_ETLR17,param,stack2);
      SetValue(IN_ETLR25,param,stack3);
      SetValue(IN_ETLR36,param,stack4);
      break;

    case 511:  /*** Vhh=7.5V 50ms&Vhh=7.0V 100ms(Cat4) ***/
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x03);
      stack0 = SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0x00);
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_TESTSEL,param,1);
      tempo1 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( (PASS!=tempo0)||(PASS!=tempo1) ) {
        printf("\n");
        printf("tempo0(Program) = %d\n",tempo0);
        printf("tempo1(Erase)   = %d\n",tempo1);
        printf("  IIIIII  NN     NN  IIIIII  IIIIII  FFFFFFFF     A       IIIIII  LL       \n");
        printf("  IIIIII  NNN    NN  IIIIII  IIIIII  FFFFFFFF    AAA      IIIIII  LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("  IIIIII  NN    NNN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("  IIIIII  NN     NN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("\n");
        break;
      }

      SetValue(IN_TESTSEL,param,1);
//      stack2 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x10); /* Vhh=7.0V */
//      stack3 = SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param)&0xF0)|0x0C); /* ref current 3uA */
      stack2 = SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param)&0xC0)|0x1C); /* Vhh=7.5V */
      stack3 = SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param)&0xF0)|0x0E); /* ref current 4uA */
      stack4 = SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param)&0xF0)|0x0E); /* 10ms */
      /*** Program ALL1 50ms ***/
      stack1 = SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20); /* User Sector, 0x40=User+E2-6, 0x80=User+E1-6 */
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0x500);   /* address loop count = 5 -> stress time = 50ms */
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0x500);   /* address loop count = 5 -> stress time = 50ms */
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0x500);   /* address loop count = 5 -> stress time = 50ms */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /*** Read ALL1 @4uA ***/
      SetValue(IN_ETCR08,param,stack1);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( PASS!=tempo0 ) {
        printf("\n");
        printf("tempo0 = %d\n",tempo0);
        printf(" FFFFFFFF      A       IIIIII  LL        ==   CCCCCC        A      IIIIII   999999  \n");
        printf(" FFFFFFFF     AAA      IIIIII  LL        ==  CCC  CCC      AAA     IIIIII  999  999 \n");
        printf(" FF          AA AA       II    LL        ==  CC    CC     AA AA      II    99    99 \n");
        printf(" FF          AA AA       II    LL        ==  CC           AA AA      II    999  999 \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II     9999999 \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II          99 \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC         AAAAAAAAA    II          99 \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC    CC   AAAAAAAAA    II          99 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==  CCC  CCC  AA       AA   II         999 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==   CCCCCC   AA       AA   II        999  \n");
        printf("\n");
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR25,param,stack3);
        SetValue(IN_ETLR36,param,stack4);
        break;
      }

      SetValue(IN_ETLR17,param,(stack2&0xC0)|0x10); /* Vhh=7.0V */
      SetValue(IN_ETLR25,param,stack3);
      SetValue(IN_ETLR36,param,stack4);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      SetValue(IN_TESTSEL,param,1);
      tempo1 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( (PASS!=tempo0)||(PASS!=tempo1) ) {
        printf("\n");
        printf("tempo0(Program) = %d\n",tempo0);
        printf("tempo1(Erase)   = %d\n",tempo1);
        printf("  IIIIII  NN     NN  IIIIII  IIIIII  FFFFFFFF     A       IIIIII  LL       \n");
        printf("  IIIIII  NNN    NN  IIIIII  IIIIII  FFFFFFFF    AAA      IIIIII  LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN N   NN    II      II    FF         AA AA       II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN  N  NN    II      II    FFFFFFF   AA   AA      II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("    II    NN   N NN    II      II    FF       AAAAAAAAA     II    LL       \n");
        printf("  IIIIII  NN    NNN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("  IIIIII  NN     NN  IIIIII    II    FF      AA       AA  IIIIII  LLLLLLLL \n");
        printf("\n");
        break;
      }

      SetValue(IN_TESTSEL,param,1);
      SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param)&0xF0)|0x0E); /* ref current 4uA */
      SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param)&0xF0)|0x0E); /* 10ms */
      /*** Program ALL1 100ms@Vhh=7.0V ***/
      SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param)&0x1F)|0x20); /* User Sector, 0x40=User+E2-6, 0x80=User+E1-6 */
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,0xA00);   /* address loop count = 10 -> stress time = 100ms */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      /*** Read ALL1 @4uA ***/
      SetValue(IN_ETCR08,param,stack1);
      SetValue(IN_TOPADDR0,param,FLP0_TOP);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_TOPADDR1,param,FLI1_TOP);
      SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      SetValue(IN_TOPADDR2,param,FLI2_TOP);
      SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      tempo0 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readall_code_at"),"CPU READ ALL");
      if( PASS!=tempo0 ) {
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        printf("\n");
        printf(" FFFFFFFF      A       IIIIII  LL        ==   CCCCCC        A      IIIIII  55555555 \n");
        printf(" FFFFFFFF     AAA      IIIIII  LL        ==  CCC  CCC      AAA     IIIIII  55555555 \n");
        printf(" FF          AA AA       II    LL        ==  CC    CC     AA AA      II    55       \n");
        printf(" FF          AA AA       II    LL        ==  CC           AA AA      II    55       \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II    555555   \n");
        printf(" FFFFFFF    AA   AA      II    LL        ==  CC          AA   AA     II     555555  \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC         AAAAAAAAA    II          55 \n");
        printf(" FF        AAAAAAAAA     II    LL        ==  CC    CC   AAAAAAAAA    II    55    55 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==  CCC  CCC  AA       AA   II    555  555 \n");
        printf(" FF       AA       AA  IIIIII  LLLLLLLL  ==   CCCCCC   AA       AA   II     555555  \n");
        printf("\n");
        SetValue(IN_TESTSEL,param,stack0);
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR25,param,stack3);
        SetValue(IN_ETLR36,param,stack4);
        break;
      }else{
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        printf("\n");
        printf(" PPPPPP        A        SSSSSS    SSSSSS   ==   CCCCCC        A      IIIIII      444  \n");
        printf(" PPPPPPP      AAA      SSS  SSS  SSS  SSS  ==  CCC  CCC      AAA     IIIIII     4444  \n");
        printf(" PP    PP    AA AA     SS    SS  SS    SS  ==  CC    CC     AA AA      II      44 44  \n");
        printf(" PP    PP    AA AA     SSS       SSS       ==  CC           AA AA      II     44  44  \n");
        printf(" PPPPPPP    AA   AA     SSSS      SSSS     ==  CC          AA   AA     II    44   44  \n");
        printf(" PPPPPP     AA   AA       SSSS      SSSS   ==  CC          AA   AA     II    44444444 \n");
        printf(" PP        AAAAAAAAA        SSS       SSS  ==  CC         AAAAAAAAA    II    44444444 \n");
        printf(" PP        AAAAAAAAA   SS    SS  SS    SS  ==  CC    CC   AAAAAAAAA    II         44  \n");
        printf(" PP       AA       AA  SSS  SSS  SSS  SSS  ==  CCC  CCC  AA       AA   II         44  \n");
        printf(" PP       AA       AA   SSSSSS    SSSSSS   ==   CCCCCC   AA       AA   II         44  \n");
        printf("\n");
      }
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_ETLR17,param,stack2);
      SetValue(IN_ETLR25,param,stack3);
      SetValue(IN_ETLR36,param,stack4);
      break;

    case 521:
      vs[0] = 3300;
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = SetValue(IN_TESTSEL,param,0);
      stack4 = SetValue(IN_BDATA00,param,0);

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      if ( info[20] >= 10000 ) info[20] = info[20] - 10000;  /* 2nd-Cut */

	     SetValue(IN_AREA,param,0);
	     SetValue(IN_TOPADDR0,param,FLP0_TOP);
	     SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
	     SetValue(IN_TOPADDR1,param,0);
	     SetValue(IN_TESTSIZE1,param,0);
	     SetValue(IN_TOPADDR2,param,0);
	     SetValue(IN_TESTSIZE2,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

      stack9  = SetValue(IN_ETLR17,param,0xE7);  /*Vhh_w=8.0V*/
      stack10 = SetValue(IN_ETLR19,param,(GetValue(IN_ETLR19,param) & 0xF0)); // Vnoemi_PE => 2.4V
      SetValue(IN_TESTSEL,param,1); /*Chk-B*/
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W30SEC,param,CreatePatFileName("programchecker_code_at"),"Program CHECKER");  /*�t�@�[������ŕύX����\������*/

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W30SEC,param,CreatePatFileName("programchecker_code_at"),"Program CHECKER");  /*�t�@�[������ŕύX����\������*/

      SetValue(IN_EXTAL1,param,VTH_FREQ);
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
      bit_count = bit_count/2;       /* Chk pattern  */
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,3);         /* Log Headder       */
      vth_mode = 1;                         /* VthRead2          */
      min_vol = -2000;           

      sprintf(item_name,"VthRead_beforePE_S%03d",info[20]);
      result = VthReadDist(vth_mode, bit_count, min_vol, 50, &vs[0],W1SEC*waittime, param, item_name);

      if(PASS != result){
        sprintf(outbuf," Make Vth Bunpu Error!!!\n");
        DataOut(Ffpt,outbuf);
      }

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3);
      SetValue(IN_BDATA00,param,stack4);
      SetValue(IN_TESTSEL,param,0); 
      SetValue(IN_ETLR17,param,stack9);
      SetValue(IN_ETLR25,param,stack10);
      break;

    /**************************************************************************/
    /*                  Register                                              */
    /**************************************************************************/
    case 60:  /*** Read Register***/
      printf( "enter No (mitei) ->" ); /*�t�@�[������ŕύX����\������*/
      DigitInput( &tempo );
      stack0 = SetValue(IN_TESTSEL,param,tempo);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdata_at"),"CPU READ_REGISTER"); /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      break;
    case 61: /*** Write Register***/
      printf("enter No (mitei) ->"); /*�t�@�[������ŕύX����\������*/
      DigitInput(&tempo0);
      stack0 = SetValue(IN_TESTSEL,param,tempo0);
      stack1 = SetValue(IN_WRITESIZE,param,glob_pmode_flp); /*�t�@�[������ŕύX����\������*/
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programdata_at"),"Program Write REGISTER"); /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_WRITESIZE,param,stack1);
      break;
    case 62: /*** Write Triming Data (Register) ***/
      stack0 = SetValue(IN_TESTSEL,param,0);
      stack1 = SetValue(IN_BDATA00,param,1);
      stack2 = SetValue(IN_BDATA01,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER"); /*�t�@�[������ŕύX����\������*/
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_BDATA00,param,stack1);
      SetValue(IN_BDATA01,param,stack2);      
      break;  
    case 63: /*** Lot,Waf No.(Extra5) ***/
      //printf(" Program Lot/Waf/XY info? (1:Yes / 0:No) -> ");
      //DigitInput(&tempo0);
      //if( tempo0==1 ){
        printf(" Enter Lot Name -> ");
        StringInput(lotname);
        SetValue(IN_BDATA01,param,(int)lotname[0]);
        SetValue(IN_BDATA02,param,(int)lotname[1]);
        SetValue(IN_BDATA03,param,(int)lotname[2]);
        SetValue(IN_BDATA04,param,(int)lotname[3]);
        SetValue(IN_BDATA05,param,(int)lotname[4]);
        SetValue(IN_BDATA06,param,(int)lotname[5]);
        SetValue(IN_BDATA07,param,(int)lotname[6]);
        SetValue(IN_BDATA08,param,(int)lotname[7]);
        SetValue(IN_BDATA09,param,(int)lotname[8]);
        SetValue(IN_BDATA10,param,(int)lotname[9]);
        if( 0==(int)lotname[6] ){
          SetValue(IN_BDATA07,param,' ');
          SetValue(IN_BDATA08,param,' ');
          SetValue(IN_BDATA09,param,' ');
          SetValue(IN_BDATA10,param,' ');
        }
        printf(" Enter Wafer No. -> ");
        DigitInput(&tempo);
        if( tempo<=0 ) break;
        SetValue(IN_BDATA11,param,tempo);
        printf(" Enter X-coodinate -> ");
        DigitInput(&tempo);
        SetValue(IN_BDATA12,param,tempo);
        printf(" Enter Y-coodinate -> ");
        DigitInput(&tempo);
        SetValue(IN_BDATA13,param,tempo);
      //}
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e5lotinfo_at"),"Program Lot/Waf/XY No");
      break;  
    case 64: /*** Program Sample No.(Extra4) ***/
      stack0 = SetValue(IN_AREA,param,tempo);
      SetValue(IN_AREA,param,4); /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      SetValue(IN_LDATA0 ,param,ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L));
      SetValue(IN_BDATA01,param,ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B));
      SetValue(IN_BDATA02,param,ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B));
      SetValue(IN_BDATA03,param,ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B));
      SetValue(IN_BDATA04,param,ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B));
      SetValue(IN_BDATA05,param,ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B));
      SetValue(IN_BDATA06,param,ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B));
      SetValue(IN_BDATA07,param,ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B));

      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      if ( GetValue(IN_LDATA0 ,param) >= 10000 ) {  
        printf("|  S# : %03d               |\n",GetValue(IN_LDATA0 ,param)-10000);
      } else {
        printf("|  S# : %03d               |\n",GetValue(IN_LDATA0 ,param));
	  }
      printf("+-------------------------+\n");

      switch(GetValue(IN_BDATA01,param)) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(GetValue(IN_BDATA02,param)) {
        case 0x00: printf("|  Retention [Loss]       |\n"); break;
        case 0x01: printf("|  Retention [Gain]       |\n"); break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(GetValue(IN_BDATA03,param)) {
        case 90: printf("|  Test_Temp =  90C       |\n"); break;
        case 60: printf("|  Test_Temp =  60C       |\n"); break;
        case 25: printf("|  Test_Temp =  25C       |\n"); break;
        case 40: printf("|  Test_Temp = -40C       |\n"); break;
        default: printf("|  Test_Temp ???          |\n"); break;
      }

      switch(GetValue(IN_BDATA04,param)) {
        case 90: printf("|  P/E_Temp  =  90C       |\n"); break;
        case 60: printf("|  P/E_Temp  =  60C       |\n"); break;
        case 25: printf("|  P/E_Temp  =  25C       |\n"); break;
        case 40: printf("|  P/E_Temp  = -40C       |\n"); break;
        default: printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(GetValue(IN_BDATA05,param)) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(GetValue(IN_BDATA06,param)) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(GetValue(IN_BDATA07,param)) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*******************************************\n");
      printf("  0:Sample-No & Test-Condition\n");
      printf("  1:Sample-No ONLY\n");
      printf("  2:Test-Condition ONLY\n");
      printf("  3:Test-Flag 0xAA (Ret-1st After)\n");
      printf(" 99:Exit\n");
      printf("*******************************************\n");
      printf(" Select No -> ");
      DigitInput(&tempo);
      if ( tempo == 99 ) break;

      if((tempo==0) || (tempo==1)) {
        printf(" Enter Sample No. (skip case=>0) -> ");
        DigitInput(&tempo0);
        if( tempo0<=0 ) break;
        printf(" Enter CutSelect (1st-Cut:1 / 2nd-Cut:2 ) -> ");
        //DigitInput(&tempo1);
        tempo1 = 2;
        if ( tempo1==2 ) SetValue(IN_LDATA0,param,tempo0+10000);
        else             SetValue(IN_LDATA0,param,tempo0);
      }

      if(tempo==3) {
        SetValue(IN_BDATA01,param,0xAA); /* ID of After PE */
	  }

      if((tempo==0) || (tempo==2)) {
        SetValue(IN_BDATA01,param,0x55); /* ID of before PE */
        printf("+=======================+\n");
        printf("| No | Test Item        |\n");
        printf("+-----------------------+\n");
        printf("|  1 | Retention_Loss   |\n");
        printf("|  2 | Retention_Gain   |\n");
        printf("+-----------------------+\n");
        printf("|  3 | Erroneous Erase  |\n");
        printf("|  4 | Erroneous Write  |\n");
        printf("+-----------------------+\n");
        printf("|  5 | Endurance        |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA02,param,0x00); break;
	  case  2: SetValue(IN_BDATA02,param,0x01); break;
	  case  3: SetValue(IN_BDATA02,param,0x10); break;
	  case  4: SetValue(IN_BDATA02,param,0x20); break;
	  case  5: SetValue(IN_BDATA02,param,0x30); break;
          default: SetValue(IN_BDATA02,param,0x99); break;
        }
	
        printf("+=======================+\n");
        printf("| No | Test Nodule      |\n");
        printf("+-----------------------+\n");
        printf("|  0 | FLP0 + FLI1      |\n");
        printf("| 10 | FLP0             |\n");
        printf("| 11 | FLI1             |\n");
        printf("| 12 | FLI2             |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo1);

        printf("+=======================+\n");
        printf("| No | P/E Temp         |\n");
        printf("+-----------------------+\n");
        printf("|  1 |  90C             |\n");
        printf("|  2 |  60C             |\n");
        printf("|  3 |  25C             |\n");
        printf("|  4 | -40C             |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA03,param,90); break;
	  case  2: SetValue(IN_BDATA03,param,60); break;
	  case  3: SetValue(IN_BDATA03,param,25); break;
	  case  4: SetValue(IN_BDATA03,param,40); break;
          default: SetValue(IN_BDATA03,param,0x99); break;
        }

        printf("+=======================+\n");
        printf("| No | Test Temp        |\n");
        printf("+-----------------------+\n");
        printf("|  1 |  90C             |\n");
        printf("|  2 |  60C             |\n");
        printf("|  3 |  25C             |\n");
        printf("|  4 | -40C             |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA04,param,90); break;
	  case  2: SetValue(IN_BDATA04,param,60); break;
	  case  3: SetValue(IN_BDATA04,param,25); break;
	  case  4: SetValue(IN_BDATA04,param,40); break;
          default: SetValue(IN_BDATA04,param,0x99); break;
        }

        printf("+=======================+\n");
        printf("| No | P/E Cycle        |\n");
        printf("+-----------------------+\n");
        printf("|  1 |  10K             |\n");
        printf("|  2 |   1K             |\n");
        printf("|  3 |   0              |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: tempo2 =   10 ; break;
	  case  2: tempo2 =    1 ; break;
	  case  3: tempo2 =    0 ; break;
          default: tempo2 = 0x99 ; break;
        }

        if ( tempo1 ==  0 ) {    /* FLP0+FLI1 */
          SetValue(IN_BDATA05,param,tempo2);
          SetValue(IN_BDATA06,param,0);
          SetValue(IN_BDATA07,param,0x99);
        }
        if ( tempo1 == 10 ) {    /* FLP0 */
          SetValue(IN_BDATA05,param,tempo2);
          SetValue(IN_BDATA06,param,0x99);
          SetValue(IN_BDATA07,param,0x99);
        }
        if ( tempo1 == 11 ) {    /* FLI1 */
          SetValue(IN_BDATA05,param,0x99);
          SetValue(IN_BDATA06,param,tempo2);
          SetValue(IN_BDATA07,param,0x99);
        }
        if ( tempo1 == 12 ) {    /* FLI2 */
          SetValue(IN_BDATA05,param,0x99);
          SetValue(IN_BDATA06,param,0x99);
          SetValue(IN_BDATA07,param,tempo2);
        }
      }

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
      SetValue(IN_LDATA0 ,param,0);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);
      SetValue(IN_BDATA06,param,0);
      SetValue(IN_BDATA07,param,0);
      SetValue(IN_AREA,param,stack0); 
      break;  
    case 68: /*** QT Retention CodeSet(Extra4) ***/
      stack0 = SetValue(IN_AREA,param,tempo);
      SetValue(IN_AREA,param,4); /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      SetValue(IN_LDATA0 ,param,ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L));
      SetValue(IN_BDATA01,param,ReadEcrMode8(FBM_READTOP+0x0030,SIZE_B));
      SetValue(IN_BDATA02,param,ReadEcrMode8(FBM_READTOP+0x0031,SIZE_B));
      SetValue(IN_BDATA03,param,ReadEcrMode8(FBM_READTOP+0x0032,SIZE_B));
      SetValue(IN_BDATA04,param,ReadEcrMode8(FBM_READTOP+0x0033,SIZE_B));
      SetValue(IN_BDATA05,param,ReadEcrMode8(FBM_READTOP+0x0034,SIZE_B));
      SetValue(IN_BDATA06,param,ReadEcrMode8(FBM_READTOP+0x0035,SIZE_B));
      SetValue(IN_BDATA07,param,ReadEcrMode8(FBM_READTOP+0x0036,SIZE_B));

      printf("+=========================+\n");
      printf("|  Test Information       |\n");
      printf("+-------------------------+\n");
      if ( GetValue(IN_LDATA0 ,param) >= 10000 ) {  
        printf("|  S# : %03d               |\n",GetValue(IN_LDATA0 ,param)-10000);
      } else {
        printf("|  S# : %03d               |\n",GetValue(IN_LDATA0 ,param));
	  }
      printf("+-------------------------+\n");

      switch(GetValue(IN_BDATA01,param)) {
        case 0x55: printf("|  Flag : 0x55 (Before)   |\n"); break;
        case 0xAA: printf("|  Flag : 0xAA (After)    |\n"); break;
        default:   printf("|  Flag ???               |\n"); break;
      }

      switch(GetValue(IN_BDATA02,param)) {
        case 0x00: printf("|  Retention [Loss]       |\n"); break;
        case 0x01: printf("|  Retention [Gain]       |\n"); break;
        case 0x10: printf("|  Erroneous Erase [Loss] |\n"); break;
        case 0x20: printf("|  Erroneous Write [Gain] |\n"); break;
        case 0x30: printf("|  Endurance              |\n"); break;
        default:   printf("|  Test ???               |\n"); break;
      }

      switch(GetValue(IN_BDATA03,param)) {
        case 125: printf("|  Test_Temp = 125C       |\n"); break;
        case  90: printf("|  Test_Temp =  90C       |\n"); break;
        case  60: printf("|  Test_Temp =  60C       |\n"); break;
        case  25: printf("|  Test_Temp =  25C       |\n"); break;
        case  40: printf("|  Test_Temp = -40C       |\n"); break;
        case  42: printf("|  Test_Temp =  42C       |\n"); break;
        default:  printf("|  Test_Temp ???          |\n"); break;
      }

      switch(GetValue(IN_BDATA04,param)) {
        case 125: printf("|  P/E_Temp  = 125C       |\n"); break;
        case  90: printf("|  P/E_Temp  =  90C       |\n"); break;
        case  60: printf("|  P/E_Temp  =  60C       |\n"); break;
        case  25: printf("|  P/E_Temp  =  25C       |\n"); break;
        case  40: printf("|  P/E_Temp  = -40C       |\n"); break;
        default:  printf("|  P/E_Temp  ???          |\n"); break;
      }

      switch(GetValue(IN_BDATA05,param)) {
        case  0: printf("|  FLP0:  0               |\n"); break;
        case  1: printf("|  FLP0:  1K              |\n"); break;
        case 10: printf("|  FLP0: 10K              |\n"); break;
        default: printf("|  FLP0: ---              |\n"); break;
      }
      switch(GetValue(IN_BDATA06,param)) {
        case  0: printf("|  FLI1:  0               |\n"); break;
        case  1: printf("|  FLI1:  1K              |\n"); break;
        case 10: printf("|  FLI1: 10K              |\n"); break;
        default: printf("|  FLI1: ---              |\n"); break;
      }
      switch(GetValue(IN_BDATA07,param)) {
        case  0: printf("|  FLI2:  0               |\n"); break;
        case  1: printf("|  FLI2:  1K              |\n"); break;
        case 10: printf("|  FLI2: 10K              |\n"); break;
        default: printf("|  FLI2: ---              |\n"); break;
      }
      printf("+=========================+\n");

      printf("\n");
      printf("*******************************************\n");
//      printf("  0:Sample-No & Test-Condition\n");
//      printf("  1:Sample-No ONLY\n");
      printf("  2:Test-Condition ONLY\n");
      printf("  3:Test-Flag 0xAA (Ret-1st After)\n");
      printf("  4:Test-Flag 0x55 (Before  After)\n");
      printf(" 99:Exit\n");
      printf("*******************************************\n");
      printf(" Select No -> ");
      DigitInput(&tempo);
      if ( tempo == 99 ) break;

//      if((tempo==0) || (tempo==1)) {
//        printf(" Enter Sample No. (skip case=>0) -> ");
//        DigitInput(&tempo0);
//        if( tempo0<=0 ) break;
//        printf(" Enter CutSelect (1st-Cut:1 / 2nd-Cut:2 ) -> ");
//        DigitInput(&tempo1);
//        if ( tempo1==2 ) SetValue(IN_LDATA0,param,tempo0+10000);
//        else             SetValue(IN_LDATA0,param,tempo0);
//      }

      if(tempo==3) {
        SetValue(IN_BDATA01,param,0xAA); /* ID of After PE */
	  }

      if(tempo==4) {
        SetValue(IN_BDATA01,param,0x55); /* ID of Before PE */
	  }

      if((tempo==0) || (tempo==2)) {
        SetValue(IN_BDATA01,param,0x55); /* ID of before PE */
        printf("+=======================+\n");
        printf("| No | Test Item        |\n");
        printf("+-----------------------+\n");
        printf("|  1 | Retention_Loss   |\n");
        printf("|  2 | Retention_Gain   |\n");
        printf("+-----------------------+\n");
//        printf("|  3 | Erroneous Erase  |\n");
//        printf("|  4 | Erroneous Write  |\n");
//        printf("+-----------------------+\n");
        printf("|  5 | Endurance        |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA02,param,0x00); break;
	  case  2: SetValue(IN_BDATA02,param,0x01); break;
//	  case  3: SetValue(IN_BDATA02,param,0x10); break;
//	  case  4: SetValue(IN_BDATA02,param,0x20); break;
	  case  5: SetValue(IN_BDATA02,param,0x30); break;
          default: SetValue(IN_BDATA02,param,0x99); break;
        }
	
        printf("+===================================+\n");
        printf("| No | Test Nodule                  |\n");
        printf("+-----------------------------------+\n");
        printf("|  0 | FLP0(10k)+FLI1(1k)+FLI2(0k)  |\n");
        printf("|  1 | FLP0(0k)+FLI1(10k)+FLI2(0k)  |\n");
        printf("| 10 | FLP0                         |\n");
        printf("| 11 | FLI1                         |\n");
        printf("| 12 | FLI2                         |\n");
      	printf("| 13 | FLP0(10k)+FLI1(10k)+FLI2(10k)|\n");
        printf("+-----------------------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo1);

        printf("+=======================+\n");
        printf("| No | Test Temp        |\n");
        printf("+-----------------------+\n");
        printf("|  1 | 125C             |\n");
        printf("|  2 |  90C             |\n");
        printf("|  3 |  60C             |\n");
        printf("|  4 |  25C             |\n");
        printf("|  5 | -40C             |\n");
      	printf("| 10 | +42C             |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA03,param,125); break;
	  case  2: SetValue(IN_BDATA03,param, 90); break;
	  case  3: SetValue(IN_BDATA03,param, 60); break;
	  case  4: SetValue(IN_BDATA03,param, 25); break;
	  case  5: SetValue(IN_BDATA03,param, 40); break;
	  case 10: SetValue(IN_BDATA03,param, 42); break;
          default: SetValue(IN_BDATA03,param,0x99); break;
        }

        printf("+=======================+\n");
        printf("| No | P/E Temp         |\n");
        printf("+-----------------------+\n");
        printf("|  1 | 125C             |\n");
        printf("|  2 |  90C             |\n");
        printf("|  3 |  60C             |\n");
        printf("|  4 |  25C             |\n");
        printf("|  5 | -40C             |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: SetValue(IN_BDATA04,param,125); break;
	  case  2: SetValue(IN_BDATA04,param, 90); break;
	  case  3: SetValue(IN_BDATA04,param, 60); break;
	  case  4: SetValue(IN_BDATA04,param, 25); break;
	  case  5: SetValue(IN_BDATA04,param, 40); break;
          default: SetValue(IN_BDATA04,param,0x99); break;
        }

     if ( tempo1 !=  0 ) {    /* !=FLP0+FLI1 */
        printf("+=======================+\n");
        printf("| No | P/E Cycle        |\n");
        printf("+-----------------------+\n");
        printf("|  1 |  10K             |\n");
        printf("|  2 |   1K             |\n");
        printf("|  3 |   0              |\n");
        printf("+-----------------------+\n");
        printf(" Select No -> "); DigitInput(&tempo0);
        switch(tempo0) {
	  case  1: tempo2 =   10 ; break;
	  case  2: tempo2 =    1 ; break;
	  case  3: tempo2 =    0 ; break;
          default: tempo2 = 0x99 ; break;
         }
        }

        if ( tempo1 ==  0 ) {    /* FLP0+FLI1 */
          SetValue(IN_BDATA05,param,10);
          SetValue(IN_BDATA06,param,1);
          SetValue(IN_BDATA07,param,0);
        }
        if ( tempo1 ==  1 ) {    /* FLP0+FLI1 */
          SetValue(IN_BDATA05,param,0);
          SetValue(IN_BDATA06,param,10);
          SetValue(IN_BDATA07,param,0);
        }

//      	if ( tempo1 ==  0 ) {    /* FLP0+FLI1 */
//          SetValue(IN_BDATA05,param,tempo2);
//          SetValue(IN_BDATA06,param,0);
//          SetValue(IN_BDATA07,param,0x99);
//        }
        if ( tempo1 == 10 ) {    /* FLP0 */
          SetValue(IN_BDATA05,param,tempo2);
          SetValue(IN_BDATA06,param,0x99);
          SetValue(IN_BDATA07,param,0x99);
        }
        if ( tempo1 == 11 ) {    /* FLI1 */
          SetValue(IN_BDATA05,param,0x99);
          SetValue(IN_BDATA06,param,tempo2);
          SetValue(IN_BDATA07,param,0x99);
        }
        if ( tempo1 == 12 ) {    /* FLI2 */
          SetValue(IN_BDATA05,param,0x99);
          SetValue(IN_BDATA06,param,0x99);
          SetValue(IN_BDATA07,param,tempo2);
        }
        if ( tempo1 ==  13 ) {    /* FLP0+FLI1 */
          SetValue(IN_BDATA05,param,10);
          SetValue(IN_BDATA06,param,10);
          SetValue(IN_BDATA07,param,10);
        }
      }

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("program_e4sampleno_at"),"Program Sample No");
      SetValue(IN_LDATA0 ,param,0);
      SetValue(IN_BDATA01,param,0);
      SetValue(IN_BDATA02,param,0);
      SetValue(IN_BDATA03,param,0);
      SetValue(IN_BDATA04,param,0);
      SetValue(IN_BDATA05,param,0);
      SetValue(IN_BDATA06,param,0);
      SetValue(IN_BDATA07,param,0);
      SetValue(IN_AREA,param,stack0); 
      break;
      case 69:
        stack1 = SetValue(IN_ETLR36,param,GetValue(IN_ETLR36,param) & 0xF0 | 0x01);//Write 0.6ms
        stack2 = SetValue(IN_ETLR17,param,GetValue(IN_ETLR17,param) & 0xC0 | 0x02);//Write Vhh 6.5V
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W30SEC,param,CreatePatFileName("programdiagonal_code_at"),"Program Global Diagonal");
        SetValue(IN_ETLR17,param,stack2);
        SetValue(IN_ETLR36,param,stack1);
      break;
    /**************************************************************************/
    /*                  OTHER                                                 */
    /**************************************************************************/
    case 112:  /* Logic Pattern Menu */
      LogicSelect(&vs[0],W1SEC*1,param);
      break;
    case 113:  /* Logic Pattern Menu */
      ReadShmooSelect(&vs[0],W1SEC*1,param);
      break;
    case 100:  /*** Monitir&Triming Menu ***/
      MonitorTrimingSelect(&vs[0],W1SEC*1,param);
      break;
    case 111:
      printf("enter PE-Cycle->");DigitInput(&tempo1);
	  stack0 = SetValue(IN_EXTAL1,param,30);
	  stack1 = SetValue(IN_TESTSEL,param,0);// ALL0 Write Set
	  stack2 = SetValue(IN_BDATA00,param,1);// PreWrite Set
	  //DataOut File Setting
	  sprintf(file_name,"%s%sPECycleXVthEdge_%d.csv", glob_cdp, glob_datalog, get_site_number());
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
	  fprintf(file_point,"PE-Cycle,Erase-VthEdge,Write-VthEdge\n");
	  fclose( file_point);
	  delay_timer(1000);

      //PE-Loop Start
      for(i=1;i<=tempo1;i++){
	  //DataOut File Setting
      if( NULL==(file_point = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
		//Edge-Search Flag Set
        switch(i){
		  case 1:
		  case 5:
		  case 10:
		  case 30:
		  case 50:
		  case 80:
		  case 100:
		  case 300:
		  case 500:
		  case 800:
		  case 1000:
		  case 3000:
		  case 5000:
		  case 7000:
		  case 8000:
		  case 9000:
		  case 10000:
		    edge_flag = 1;
		    break;
		  default:
		    edge_flag = 0;
		    break;
        }

        //Erase Start
		SetValue(IN_EXTAL1,param,30);
      	result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");

		//Error-Set
        if(result!=PASS){
		  switch(ReadEcrMode8(0x3D00,SIZE_B)){
		    case 0x78: sprintf(item_name,"TimeOver");break;
		    case 0xAA: sprintf(item_name,"0x3D00=AA-Error");break;
		    default  : sprintf(item_name,"OtherError");break;
                  }
		  fprintf(file_point,"%d,E-%s\n",i,item_name);
        }

		//E-Serch
		if(edge_flag ==1){
		  tempo2=VthReadSearch(1, &vs[0], param,waittime,20);
		}

        //Write Start
		SetValue(IN_EXTAL1,param,30);
      	result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Write");

		//Error-Set
        if(result!=PASS){
		  switch(ReadEcrMode8(0x3D00,SIZE_B)){
		    case 0x78: sprintf(item_name,"TimeOver");break;
		    case 0xAA: sprintf(item_name,"0x3D00=AA-Error");break;
		    default  : sprintf(item_name,"OtherError");break;
          }
		  fprintf(file_point,"%d,P-%s\n",i,item_name);
        }

		//P-Serch & DataOut
		if(edge_flag ==1){
		  tempo3=VthReadSearch(0, &vs[0], param,waittime,20);
		  fprintf(file_point,"%d,%dmV,%dmV\n",i,tempo2,tempo3);
		}
	  fclose( file_point);
      }//Loop end 
	  SetValue(IN_EXTAL1,param,30);
	  SetValue(IN_TESTSEL,param,stack1);
	  SetValue(IN_BDATA00,param,stack2);

    break;          

    case 121:
    stack1 = GetValue(IN_ETLR16,param);
    stack2 = GetValue(IN_ETLR17,param);
    for ( j=0 ; j<9 ; j++) {

      cnt=200;
      tempo0=0;
      tempo1=0x2000+(0x2000*j);
      tempo2=tempo1+0xFF;
      sprintf(id_name,"%d",j);
      strcpy(item_name ,"Endurance_Vth_w_No");
      strcat(item_name ,id_name);
      strcpy(item_name2,"Endurance_Vth_e_No");
      strcat(item_name2,id_name);

      switch(j) {
        case 0: SetValue(IN_ETLR16,param,(stack1 -8 ) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,stack2);                /* vhh_w */
                break;
        case 1: SetValue(IN_ETLR16,param,(stack1 -16) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,stack2);                /* vhh_w */
                break;
        case 2: SetValue(IN_ETLR16,param,(stack1 -24) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,stack2);                /* vhh_w */
                break;
        case 3: SetValue(IN_ETLR16,param,(stack1 -24) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -21) | 0xC0);   /* vhh_w */
                break;
        case 4: SetValue(IN_ETLR16,param,stack1);                /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -7 ) | 0xC0);   /* vhh_w */
                break;
        case 5: SetValue(IN_ETLR16,param,stack1);                /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -14) | 0xC0);   /* vhh_w */
                break;
        case 6: SetValue(IN_ETLR16,param,stack1);                /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -21) | 0xC0);   /* vhh_w */
                break;
        case 7: SetValue(IN_ETLR16,param,(stack1 -16) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -14) | 0xC0);   /* vhh_w */
                break;
        case 8: SetValue(IN_ETLR16,param,(stack1 -8 ) | 0xC0);   /* vhh_e */
                SetValue(IN_ETLR17,param,(stack2 -7 ) | 0xC0);   /* vhh_w */
                break;
      } 
    
      /*
      printf(" Enter PE Cycle number [10k x n] (ex.100=1Mkai) -> ");
      DigitInput(&cnt);
      printf(" Select Module (0:FLP0, 1:FLI1, 2:FLI2, 3:FLI0&1, 99:User ALL) -> ");
      DigitInput(&tempo0);
      printf(" Enter or Skip Address setting (0:Skip(=All area), 1:Enter) -> ");
      DigitInput(&tempo);
      if(tempo==1){
        if(tempo0!=99){
          printf(" Start Address (ex. 0x1000) -> ");
          DigitInput(&tempo1);
          printf(" End Address (max=0x7FFFF) -> ");
          DigitInput(&tempo2);
        }else{
          printf(" Start Address(FLP0) -> ");
          DigitInput(&tempo1);
          printf(" End Address(FLP0) -> ");
          DigitInput(&tempo2);
          printf(" Start Address(FLI1) -> ");
          DigitInput(&tempo3);
          printf(" End Address(FLI1) -> ");
          DigitInput(&tempo4);
          printf(" Start Address(FLI2) -> ");
          DigitInput(&tempo5);
          printf(" End Address(FLI2) -> ");
          DigitInput(&tempo6);
        }
      }else{
        tempo1=tempo3=tempo5=0x00000000;
        tempo2=tempo4=tempo6=0x0007FFFF;
      }
      */

      SetValue(IN_AREA,param,0);
      if(tempo0==0){
        SetValue(IN_TOPADDR0,param,tempo1);             /* FLP0_TOP  */
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1)); /* FLP0_SIZE */
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else if(tempo0==1){
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,tempo1);             /* FLI1_TOP  */
        SetValue(IN_TESTSIZE1,param,(tempo2-tempo1+1)); /* FLP1_SIZE */
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else if(tempo0==2){
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,tempo1);             /* FLI2_TOP  */
        SetValue(IN_TESTSIZE2,param,(tempo2-tempo1+1)); /* FLP2_SIZE */
      }else if(tempo0==3){
        SetValue(IN_TOPADDR0,param,tempo1);             /* FLI0_TOP  */
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1)); /* FLP0_SIZE */
        SetValue(IN_TOPADDR1,param,tempo1);             /* FLI1_TOP  */
        SetValue(IN_TESTSIZE1,param,(tempo2-tempo1+1)); /* FLP1_SIZE */
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }else{
        SetValue(IN_TOPADDR0,param,tempo1);
        SetValue(IN_TESTSIZE0,param,(tempo2-tempo1+1));
        SetValue(IN_TOPADDR1,param,tempo3);
        SetValue(IN_TESTSIZE1,param,(tempo4-tempo3+1));
        SetValue(IN_TOPADDR2,param,tempo5);
        SetValue(IN_TESTSIZE2,param,(tempo6-tempo5+1));
      }

      //PE-Loop Start
      for(i=0 ; i<=cnt ; i++){

      if ( i != 0 ) {
        /* PE Function */
        SetValue(IN_TESTSEL,param,0); /* ALL0 */
        SetValue(IN_BDATA01,param,0); /* ALL0 paterrn ���[�N���i�[�p */
        SetValue(IN_LDATA0,param,10000);  /* 10K P/E */
        SetValue(IN_LDATA1,param,1);      /* PE Wait time(sec) */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*180,param,CreatePatFileName("pecycle_code_all_at"),"PE Cycle");
      }

        /* Vth-Read Funtion */
        SetValue(IN_TESTSEL,param,0); /* ALL0 */
        SetValue(IN_BDATA01,param,0); /* ALL0 paterrn ���[�N���i�[�p */
        SetValue(IN_LDATA0,param,1);  /* P/E */
        SetValue(IN_LDATA1,param,1);  /* PE Wait time(sec) */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*180,param,CreatePatFileName("pecycle_code_all_at"),"PE Cycle");

        SetValue(IN_TESTSEL,param,0);
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
        stack3 = SetValue(IN_EXTAL1,param,VTH_FREQ);
        SetValue(IN_BDATA01,param,0);
        result = VthReadDist(23, 2048, 0, 50, &vs[0],W1SEC*waittime, param, item_name);
        SetValue(IN_EXTAL1,param,stack3);
        if(PASS != result){
          sprintf(outbuf," Make Vth Bunpu Error!!!\n");
          DataOut(Ffpt,outbuf);
        }

        SetValue(IN_BDATA00,param,0);  /* Pre-Write ON */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
        stack3 = SetValue(IN_EXTAL1,param,VTH_FREQ);
        SetValue(IN_BDATA01,param,0);
        result = VthReadDist(12, 2048, -2000, 50, &vs[0],W1SEC*waittime, param, item_name2);
        SetValue(IN_EXTAL1,param,stack3);
        if(PASS != result){
          sprintf(outbuf," Make Vth Bunpu Error!!!\n");
          DataOut(Ffpt,outbuf);
        }

      } /* i */
    }   /* j */
    break;

    case 200:
      MinoriSelect(vs, waittime, param);
    break;

    case 300:
      Ret_Distribution(&vs[0],W1SEC*waittime,param);
    break;
    case 303:
      Ret_Distribution_3(&vs[0],W1SEC*waittime,param);
    break;
    case 310:
    case 311:
      if(PASS==Extra_VthRead(&vs[0],W1SEC*waittime,param,test_cond-310)){
        printf("\n");
        printf("**************\n");
        printf("**   PASS   **\n");
        printf("**************\n");
      }else{
        printf("\n");
        printf("--------------\n");
        printf("--   FAIL   --\n");
        printf("--------------\n");
      }
    break;
    case 371:
      SinraiMenu(&vs[0],waittime,param);
    break;
    case 400:
      ReadWindow(&vs[0],W1SEC*waittime,param);
    break;

    case 600:
      Disturb(&vs[0],W1SEC*waittime,param);
    break;

    case 700:  /*** Bit Leak ***/
      stack3 = SetValue(IN_EXTAL1,param,1000);

      printf("+----+-----------+\n");
      printf("| No |ETCR07[4:3]|\n");
      printf("+----+-----------+\n");
      printf("|  0 |     00    |\n");
      printf("|  1 |     01    |\n");
      printf("|  2 |     10    |\n");
      printf("|  3 |     11    |\n");
      printf("+----+-----------+\n");
      printf("Select Mode -> ");
      DigitInput(&tempo);
      stack1 = SetValue(IN_BDATA01,param,tempo);
      
      printf("Chane Iref (default:0.5uA) ?  No[0] / Yes[1] -> ");
      DigitInput(&tempo);
      if ( tempo == 0 ) stack2 = SetValue(IN_BDATA02,param,0);  /* 0.5uA */
      else {
        printf("+----+------+------+\n");
        printf("|    | Iref |ETLR25|\n");
        printf("| No | [uA] |[3:0] |\n");
        printf("+----+------+------+\n");
        printf("|  0 |  0.5 |  0x9 |\n");
        printf("|  1 |  1.0 |  0x8 |\n");
        printf("|  2 |  1.5 |  0xB |\n");
        printf("|  3 |  2.0 |  0xA |\n");
        printf("+----+------+------+\n");
        printf("|  4 |  2.5 |  0xD |\n");
        printf("|  5 |  3.0 |  0xC |\n");
        printf("|  6 |  3.5 |  0xF |<= Default\n");
        printf("|  7 |  4.0 |  0xE |\n");
        printf("+----+------+------+\n");
        printf("|  8 |  4.5 |  0x1 |\n");
        printf("|  9 |  5.0 |  0x0 |\n");
        printf("| 10 |  5.5 |  0x3 |\n");
        printf("| 11 |  6.0 |  0x2 |\n");
        printf("+----+------+------+\n");
        printf("| 12 |  6.5 |  0x5 |\n");
        printf("| 13 |  7.0 |  0x4 |\n");
        printf("| 14 |  7.5 |  0x7 |\n");
        printf("| 15 |  8.0 |  0x6 |\n");
        printf("+----+------+------+\n");
        printf("Select Iref No -> ");
        DigitInput(&tempo);
        switch  ( tempo ) {
          case  0: tempo0 = 0x9; break;
          case  1: tempo0 = 0x8; break;
          case  2: tempo0 = 0xB; break;
          case  3: tempo0 = 0xA; break;
          case  4: tempo0 = 0xD; break;
          case  5: tempo0 = 0xC; break;
          case  6: tempo0 = 0xF; break;
          case  7: tempo0 = 0xE; break;
          case  8: tempo0 = 0x1; break;
          case  9: tempo0 = 0x0; break;
          case 10: tempo0 = 0x3; break;
          case 11: tempo0 = 0x2; break;
          case 12: tempo0 = 0x5; break;
          case 13: tempo0 = 0x4; break;
          case 14: tempo0 = 0x7; break;
          case 15: tempo0 = 0x6; break;
          default: break;
        }
      stack2 = SetValue(IN_BDATA02,param,tempo0);
      }
      stack0 = SetValue(IN_TESTSEL,param,0);  /* ALL0 Read */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("bitlineleak_at"),"Bit Line Leak");
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_BDATA01,param,stack1);
      SetValue(IN_BDATA02,param,stack2);
      SetValue(IN_EXTAL1,param,stack3);
      break;

    case 800:
      Vth_Trim(&vs[0],W1SEC*waittime,param);
    break;
    case 801:
      Vth_Trim_Auto(&vs[0],W1SEC*waittime,param);
    break;
    case 802:
      Vth_Trim_Eval(&vs[0],W1SEC*waittime,param);
    break;
    case 803:
      Vth_Trim_deltaVth(&vs[0],W1SEC*waittime,param);
    break;

    case 900:
      WriteErase_Tokusei(&vs[0],W1SEC*waittime,param);
    break;
    case 901:
      printf("Input Tempurature ->");DigitInput(&tempo2);
      stack0 = GetValue(IN_ETLR15,param);
      stack1 = GetValue(IN_ETLR16,param);

      for(i=1;i<=3;i++){
        for(j=1;j<=2;j++){
          switch(j){
            case 1: sprintf(item_name,"trtf100"); break;
            case 2: sprintf(item_name,"trtf300"); SetValue(IN_ETLR15,param,(GetValue(IN_ETLR15,param) & 0xF8 ) | 0x3 ); break;
          }
          switch(i){
            case 1: sprintf(item_name2,"Vhh0"); break;
            case 2: sprintf(item_name2,"Vhh4"); SetValue(IN_ETLR16,param,GetValue(IN_ETLR16,param) - 4); break;
            case 3: sprintf(item_name2,"Vhh8"); SetValue(IN_ETLR16,param,GetValue(IN_ETLR16,param) - 8); break;
          }

          sprintf(file_name,"%s_%s",item_name,item_name2);
          WriteErase_Tokusei_Separate(&vs[0],W1SEC*waittime,param,file_name,tempo2);
          SetValue(IN_ETLR15,param,stack0);
          SetValue(IN_ETLR16,param,stack1);
        }
      }
    break;
    case 902:
      WriteErase_Tokusei_Kani(&vs[0],W1SEC*waittime,param);
    break;

    case 1000:
      CurrentMenu(&vs[0],waittime,param);
    break;
    case 1100:
      BGRMenu(&vs[0],waittime,param);
    break;
    case 1300:
      FCUMenu(&vs[0],waittime,param);
    break;
    case 1400:
      Sample_Set(&vs[0],W1SEC*waittime,param);
    break;
    case 1500:
      F_Ken(&vs[0],W1SEC*waittime,param);
    break;
    case 1700:
      RomBoot_Shmoo_Menu(&vs[0],param);
    break;
    case 1600:
      stack0 = SetValue(IN_AREA,param,5);
      stack1 = SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
      stack2 = SetValue(IN_TOPADDR1,param,0);
      stack3 = SetValue(IN_TOPADDR2,param,0);
      stack4 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      stack5 = SetValue(IN_TESTSIZE1,param,0);
      stack6 = SetValue(IN_TESTSIZE2,param,0);
      SetValue(IN_TESTSEL,param,0);SetValue(IN_BDATA00,param,0);SetValue(IN_BDATA01,param,0);

      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      
      for(i=0;i<=39;i++){
        tempo1 = ReadEcrMode8(FBM_READTOP + 0x50 + i,SIZE_B);
        printf(" ETLR%02d : 0x%02X(IN_ETLR) : 0x%02X(EXTRA5)\n",i,GetValue(IN_ETLR00+i,param),tempo1);
        SetValue(IN_ETLR00+i,param,tempo1);
      }
      
      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TOPADDR1,param,stack2);
      SetValue(IN_TOPADDR2,param,stack3);
      SetValue(IN_TESTSIZE0,param,stack4);
      SetValue(IN_TESTSIZE1,param,stack5);
      SetValue(IN_TESTSIZE2,param,stack6);
    break;
    /**************************************************************************/
    /*                  OTHER                                                 */
    /**************************************************************************/
    
    case 1200:
      waittime = 20;
      area_set      = GetValue(IN_AREA     ,param);
      addr_start[0] = GetValue(IN_TOPADDR0 ,param);
      addr_size[0]  = GetValue(IN_TESTSIZE0,param);
      addr_start[1] = GetValue(IN_TOPADDR1 ,param);
      addr_size[1]  = GetValue(IN_TESTSIZE1,param);
      addr_start[2] = GetValue(IN_TOPADDR2 ,param);
      addr_size[2]  = GetValue(IN_TESTSIZE2,param);
      
      printf("Select PE-Times ->"); DigitInput(&loop_max);
      
/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      if(result != PASS) break;
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

      sprintf(file_name,"%c%c%c%c%c%c_W%02d_S%03d_EnduranceLog",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20]);
      
      SetValue(IN_AREA,param,area_set);
      SetValue(IN_TOPADDR0,param,addr_start[0]);
      SetValue(IN_TESTSIZE0,param,addr_size[0]);
      SetValue(IN_TOPADDR1,param,addr_start[1]);
      SetValue(IN_TESTSIZE1,param,addr_size[1]);
      SetValue(IN_TOPADDR2,param,addr_start[1]);
      SetValue(IN_TESTSIZE2,param,addr_size[1]);

      
      
      //sprintf(file_name,"EnduranceLog");
      
      for(loop_num = 0; loop_num <= loop_max; loop_num++){
        houwa_flag = 0;

	OpenDataOutFile(file_name);
	
	switch(loop_num){
	  case 0:
	  case 10:
	  case 100:
	  case 500:
	  case 1000:
	  case 3000:
	  case 5000:
	  case 9999:
	    houwa_flag = 1;
	  break;
	  default:
	  break;
	}
	
	if(houwa_flag == 2){
	  SetValue(IN_TESTSEL,param,0);
	  SetValue(IN_BDATA00,param,0);
	  SetValue(IN_BDATA01,param,0);
	  SetValue(IN_BDATA02,param,0);
	  SetValue(IN_BDATA03,param,0);
	  SetValue(IN_BDATA04,param,0);
	  SetValue(IN_BDATA05,param,0);
	  SetValue(IN_BDATA06,param,0);
	  SetValue(IN_LDATA0 ,param,0);
	  SetValue(IN_LDATA1 ,param,0);
	  SetValue(IN_LDATA2 ,param,0);
	  SetValue(IN_LDATA3 ,param,0);
    stack1 = SetValue(IN_EXTAL1,param,VTH_FREQ);

    mode = 1; // - Mode

	  min_vol = VthEdgeSearch(mode,100,&vs[0],W1SEC*waittime, param);

          bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern

          strcpy(item_name,"VthRead_ALL");
          result = VthReadDist(1, bit_count, min_vol, 50 /* Step Vol*/, &vs[0],W1SEC*waittime, param, item_name); //Vth���z����
          
	  SetValue(IN_EXTAL1,param,stack1);
	  
	  
	  
	  SetValue(IN_TESTSEL,param,0);
	  SetValue(IN_BDATA00,param,0);
	  SetValue(IN_BDATA01,param,0);
	  SetValue(IN_BDATA02,param,0);
	  SetValue(IN_BDATA03,param,0);
	  SetValue(IN_BDATA04,param,0);
	  SetValue(IN_BDATA05,param,0);
	  SetValue(IN_BDATA06,param,0);
	  SetValue(IN_LDATA0 ,param,0);
	  SetValue(IN_LDATA1 ,param,0);
	  SetValue(IN_LDATA2 ,param,0);
	  SetValue(IN_LDATA3 ,param,0);
	}


	if(houwa_flag == 1){ //Houwa Kakunin
	  SetValue(IN_TESTSEL,param,0);
	  SetValue(IN_BDATA00,param,0);
	  SetValue(IN_BDATA01,param,0);
	  SetValue(IN_BDATA02,param,0);
	  SetValue(IN_BDATA03,param,0);
	  SetValue(IN_BDATA04,param,0);
	  SetValue(IN_BDATA05,param,0);
	  SetValue(IN_BDATA06,param,0);
	  SetValue(IN_LDATA0 ,param,0);
	  SetValue(IN_LDATA1 ,param,0);
	  SetValue(IN_LDATA2 ,param,0);
	  SetValue(IN_LDATA3 ,param,0);

          DeviceSpecificPowerUp();
	  WriteErase_Tokusei_Kani(&vs[0],W1SEC*waittime,param);
	  
	  OpenDataOutFile(file_name);delay_timer(200);
          fprintf(Fptdata,"Loop_times,%d,VthBunpu,Erase_Houwa,\n",loop_num);
	  CloseDataOutFile();delay_timer(200);
	  
	  loop_num = loop_num + 1;
	  SetValue(IN_TESTSEL,param,0);
	  SetValue(IN_BDATA00,param,0);
	  SetValue(IN_BDATA01,param,0);
	  SetValue(IN_BDATA02,param,0);
	  SetValue(IN_BDATA03,param,0);
	  SetValue(IN_BDATA04,param,0);
	  SetValue(IN_BDATA05,param,0);
	  SetValue(IN_BDATA06,param,0);
	  SetValue(IN_LDATA0 ,param,0);
	  SetValue(IN_LDATA1 ,param,0);
	  SetValue(IN_LDATA2 ,param,0);
	  SetValue(IN_LDATA3 ,param,0);
	}
	OpenDataOutFile(file_name);delay_timer(200);
	
	fprintf(Fptdata,"Loop_times,%d,",loop_num);
	
	// --- Write ---//
	Ffpt = 0;
	DeviceSpecificPowerUp();
	result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
        printf("===== Loop = %5d  =====\n",loop_num);
	
	if(result != PASS){
	  printf("\n----- Function Fail !!! -----\n\n");
          fprintf(Fptdata,"ALL0-Write Fail,");
        }else{
          fprintf(Fptdata,"ALL0-Write Pass,");
	}
	
	// --- Erase ---//
	DeviceSpecificPowerUp();
	result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
	if(result != PASS){
	  printf("\n----- Function Fail !!! -----\n\n");
          fprintf(Fptdata,"ALL1-Erase Fail,");
        }else{
          fprintf(Fptdata,"ALL1-Erase Pass,");
	}
        fprintf(Fptdata,"\n");delay_timer(200);
	
        CloseDataOutFile();delay_timer(200);
	Ffpt = 1;
      }
    
    break;
    
    case 995:
	  //ChangeAddress(FLP0,0,0xFFF,param);
    for(k = 1; k<= 2; k++){
      switch(k){//Edge Select
        case 1:
          tempo3 = 1;
          sprintf(item_name2,"Erase");
          ChangeAddress(FLI1,0,0xFFF,param);
        break;
        case 2:
          tempo3 = 4;
          sprintf(item_name2,"Write");
          ChangeAddress(FLI2,0,0xFFF,param);
        break;
      }



    tempo1 = 0;
    for(j = 1; j <= 2; j++){
      switch(j){
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:cnt = 0;break;//PE10��
        case 11:cnt = 10;break;//PE20��
        case 12:cnt = 10;break;//PE30��
        case 13:cnt = 70;break;//PE100��
        case 14:cnt = 200;break;//PE300��
        case 15:cnt = 200;break;//PE500��
      }

      for(i=1;i<=(cnt - 1);i++){
        Ffpt = 0;
        tempo1++;
        if(k==1){
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/
        }else if(k==2){
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/
        }
      }

      Ffpt = 1;
      vs[0]=3300;
      
      //File Name Set
      tempo1++;
      sprintf(item_name,"%s_W%d_X%dY%d_Retention_%s_%d",lotname,atoi(wafername),xy_location[0],xy_location[1],item_name2,tempo1,tempo3);
      sprintf( file_name, "%s%s%s.csv", glob_cdp, glob_datalog, item_name );

      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }
      
      if(k==1){

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/

      /* get time stump */
      time( &time_list );// time_list += V4V5K_TIME_OFS;
      if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
        if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
      }
      fprintf(Fptdata,"%-24.24s,WRITE,,,%d,\n",time_buf,cnt);
      }

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*�t�@�[������ŕύX����\������*/
      /* get time stump */
      time( &time_list );// time_list += V4V5K_TIME_OFS;
      if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
        if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
      }
      fprintf(Fptdata,"%-24.24s,ERASE,,%d,\n",time_buf,cnt);

      if(k==2){

      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*�t�@�[������ŕύX����\������*/

      /* get time stump */
      time( &time_list );// time_list += V4V5K_TIME_OFS;
      if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
        if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
      }
      fprintf(Fptdata,"%-24.24s,WRITE,,,%d,\n",time_buf,cnt);
      }
      fclose(Fptdata);

      for(i=0;i<=3;i++){
        if((Fptdata=fopen(file_name,"at")) == NULL) {
          printf(" -- FAIL -- not open file:%s !\n",file_name);
          break;
        }

        result = VthEdgeSearch(tempo3,20,&vs[0],W1SEC*waittime, param);
        /* get time stump */
        time( &time_list );// time_list += V4V5K_TIME_OFS;
        if(sprintf(time_buf,"%s",C_time((int *)&time_list)) != 0) {
          if((cp=strstr(time_buf,"\n")) != NULL) *cp = NUL;
        }
        fprintf(Fptdata,"%-24.24s,Vth,%d,%d,\n",time_buf,result,cnt);
        fclose(Fptdata);

      }
    }
    }
    break;

    case 997:
    SetValue(IN_BDATA00,param,1);SetValue(IN_LDATA0,param,1000);

    for(j=0;j<=1;j++){
    for(k=0;k<=/*0xFF*/30;k++){
      
      sprintf(file_name,"FailCountResult_%s_W%d_S%dX%dY%d_MAT%d",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1],j);
      OpenDataOutFile(file_name);delay_timer(200);
      fprintf(Fptdata,"WL,");
      for(i=0;i<=0xFF;i++){
      for(tempo0=0;tempo0<8;tempo0++){
        fprintf(Fptdata,"Addr%03d-Bit%d,",i,tempo0);
      }
      }
      fprintf(Fptdata,"\n");
      CloseDataOutFile();delay_timer(200);

      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      SetValue(IN_BDATA00,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      
      SetValue(IN_TOPADDR0,param,j*0x10000+k*0x100);
      SetValue(IN_TESTSIZE0,param,0x100);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");

      SetValue(IN_BDATA00,param,1);SetValue(IN_LDATA0,param,1000);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("RomAccess_FailCount"),"RomAccess_FailCount");

      OpenDataOutFile(file_name);delay_timer(200);
      fprintf(Fptdata,"%d,",k);
      for(i=0;i<2048;i++){
        fprintf(Fptdata,"%d,",ReadEcrMode8(0x1000+i*4,SIZE_L));
      }
      fprintf(Fptdata,"\n");
      CloseDataOutFile();delay_timer(200);
    }}
    
    break;
    case 998:
    SetValue(IN_BDATA00,param,1);SetValue(IN_LDATA0,param,1000);
    for(j=0;j<=10;j++){
    CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("RomAccess_Screening"),"RomAccess_Screening");
    sprintf(file_name,"DiagReadResult_%s_W%d_S%dX%dY%d_%d",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1],j);
    OpenDataOutFile(file_name);delay_timer(200);
    fprintf(Fptdata,"i,FailNum\n",i,ReadEcrMode8(FBM_READTOP+i*4,SIZE_L));
    for(i=0;i<1024;i++){
      fprintf(Fptdata,"%d,%d\n",i,ReadEcrMode8(FBM_READTOP+i*4,SIZE_L));
    }
    CloseDataOutFile();delay_timer(200);
    }
    
    break;


    case 999:
      sprintf(debug_ptnlist[0],"TestOnly");
      sprintf(debug_ptnlist[1],"FACI_test");
      sprintf(debug_ptnlist[2],"readdiagonal_code_at");
      sprintf(debug_ptnlist[3],"pecycle_code_stripe_at");
      sprintf(debug_ptnlist[4],"FACI_RamdomData_PE");
      sprintf(debug_ptnlist[5],"MonitorALL_Vhh_tem");
      sprintf(debug_ptnlist[6],"Vthseach_zitan");
      sprintf(debug_ptnlist[7],"Vthbunpu_zitan");
      sprintf(debug_ptnlist[8],"RomAccess_Screening");
      sprintf(debug_ptnlist[9],"");
    
      tempo1 = 1;
      tempo2 = 0;
      while(tempo1){
        printf("---------------------------------\n");
        printf(" Pattern : %s (0)\n",debug_ptnlist[tempo2]);
        printf("---------------------------------\n");
        printf(" TestSel : 0x%X (1)\n",GetValue(IN_TESTSEL,param));
        for(i=0;i<=5;i++){
        printf(" BDATA%02d : 0x%X (%d)\n",i,GetValue(IN_BDATA00 + i,param),10+i);
        }
        for(i=0;i<=3;i++){
        printf(" LDATA%d : 0x%X (%d)\n",i,GetValue(IN_LDATA0 + i,param),20+i);
        }
        printf("---------------------------------\n");
        printf(" Pattern Start (100) / Exit(99)\n");
        DigitInput(&tempo3);

        if(tempo3 == 99){
          tempo1 = 0;
        }

			  if(tempo3 == 100){
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName(debug_ptnlist[tempo2]),"Firm Debug");
          Firm_debugparam_Out();
          printf("Time = %d us\n",GetValue(IN_EXTAL1,param)*GetValue(OUT_LDATA0,param)/1000);
          printf("Time = %d us\n",GetValue(IN_EXTAL1,param)*GetValue(OUT_LDATA1,param)/1000);
          printf("Time = %d us\n",GetValue(IN_EXTAL1,param)*GetValue(OUT_LDATA2,param)/1000);
          printf("Time = %d us\n",GetValue(IN_EXTAL1,param)*GetValue(OUT_LDATA3,param)/1000);
        }

        if(tempo3 == 0){
          for(i=0;i<10;i++){
            printf(" %s.pat (%d)\n",debug_ptnlist[i],i);
          }
          printf("pat_name Select -> ");
          DigitInput(&tempo2);
        }
        
        if(tempo3 == 1){
          printf("Input TestSel -> ");DigitInput(&tempo4);
          SetValue(IN_TESTSEL,param,tempo4);
        }
        
        if((tempo3 >= 10) && (tempo3 < 20)){
          printf("Input BDATA%02d -> ",tempo3-10);DigitInput(&tempo4);
          SetValue(IN_BDATA00+tempo3-10,param,tempo4);
        }
        
        if((tempo3 >= 20) && (tempo3 < 30)){
          printf("Input LDATA%d -> ",tempo3-20);DigitInput(&tempo4);
          SetValue(IN_LDATA0+tempo3-20,param,tempo4);
        }
      }

    break;
    default:
    }/*switch*/
  }/*while*/

  if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
  return(PASS);
}

/******************************************************************************/
/*                                                                            */
/* int AnalyzeCommonCommand(int command, interface_t *param,int *waittime_p)  */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
int AnalyzeCommonCommand(int command, interface_t *param,int *waittime_p,int *vcc[NUM_POWERSUPPLY])
{
  int tempo,tempo0,tempo1,tempo2,tempo3;
  char reg_filename[256]; /* Register Saveing FileName */

  memset(reg_filename, 0, sizeof(reg_filename));

  switch(command){
    case SET_BGR:
      TempCheck();
      if(Extra5_TempCheck != EXTRA5_TEMP_ON){
        SetValue(IN_ETLR00,param,Extra5_ETLR00[2]);
        SetValue(IN_ETLR01,param,( (Extra5_ETLR01[2] & 0x07) | (GetValue(IN_ETLR01,param) & 0xF8 ) ));
      }
      break;
    case SET_CVCC:
      printf("enter vcc(mV) -> ");
      DigitInput(&tempo);
      vcc[0] = tempo;
      printf("\tset vcc %dmV\n",vcc[0]);
      break;
    case SET_CVDD:  //Add set vref 2018/0307
      printf("enter vdd(mV) -> ");
      DigitInput(&tempo);
      vcc[2] = tempo;
      printf("\tset VDD %dmV\n",vcc[2]);
      break;
    case SET_CVDDH: //Add set vrefh 2018/0307
      printf("enter vddh(mV) -> ");
      DigitInput(&tempo);
      vcc[1] = tempo;
      printf("\tset VDDH %dmV\n",vcc[1]);
      break;
    case SET_CVMAX:
      printf("vddh=1.45V max-> 580(mV) setting!! \n");
      printf("vdd =1.15V max-> 728(mV) setting!! \n");
      printf("Are you setting? [yes:1 ,no:0]\n");
      DigitInput(&tempo);
      if(tempo == 1) vcc[1]=580, vcc[2]=728; //vddh vdd setting//
      else if (tempo == 0) printf("Not setting!!!\n");
      break;
     case SET_CVMIN:
      printf("vddh=1.13V min -> 452(mV) setting!! \n");
      printf("vdd =0.72V min -> 456(mV) setting!! \n");
      printf("Are you setting? [yes:1 ,no:0]\n");
      DigitInput(&tempo);
      if(tempo == 1) vcc[1]=452, vcc[2]=456; //vddh vdd setting//
      else if (tempo == 0) printf("Not setting!!!\n");
      break;
     case SET_CVTYP:
      printf("vddh=1.25V min -> 500(mV) setting!! \n");
      printf("vdd =0.79V min -> 500(mV) setting!! \n");
      printf("Are you setting? [yes:1 ,no:0]\n");
      DigitInput(&tempo);
      if(tempo == 1) vcc[1]=500, vcc[2]=500; //vddh vdd setting//
      else if (tempo == 0) printf("Not setting!!!\n");
      break;
    case SET_WAIT:                        /**** Set WaitTime ****/
      printf("enter WaitTime (sec) ->");DigitInput(&tempo);
	  *waittime_p = tempo;
      printf("WaitTime %ds\n",&waittime_p);
      break;
    case SET_LOG:
      SelectFfpt( param, &vcc[0], 0 );
      break;
    case SET_MDL:                         /**** Set Mat ****/
      SelectAddress(param,command);
      break;
    case SET_SPEED:                       /**** Set SPEED ****/
      SpeedSelect(param,command);
      break;
    case SET_PLL:                       /**** Set PLL ****/
      SpeedSelect(param,command);
      break;
    case SET_REG:
      SetReg(param);
      break;
    case SET_INDATA:                      /**** Set IN_BDATA/IN_LDATA ****/
      SetInBDataLData(param);
      break;
	case SET_LPREAD:
	  printf("Select LowPowerRead ON(1)/OFF(0) -> ");
      DigitInput(&tempo);
      SetValue(IN_LOWPOWER,param,tempo);
      break;
	case SET_CLKFCG:
	  printf("Select clkf Gating{FlashSM} ON(1)/OFF(0) -> ");
      DigitInput(&tempo);
      SetValue(IN_BDATA14,param,tempo);
      break;
    case SET_ADR:                         /**** Set Address ****/
      printf("==================================\n");
      printf(" 256B Select [0x0000~0x000FF] (  1)\n");
      printf("  4KB Select [0x0000~0x00FFF] (  4)\n");
      printf("  8KB Select [0x0000~0x01FFF] (  8)\n");
      printf(" 16KB Select [0x0000~0x03FFF] ( 16)\n");
      printf(" 32KB Select [0x0000~0x07FFF] ( 32)\n");
      printf(" 64KB Select [0x0000~0x0FFFF] ( 64)\n");
      printf("128KB Select [0x0000~0x1FFFF] (128)\n");
      printf("256KB Select [0x0000~0x3FFFF] (256)\n");
      printf("==================================\n");
      printf(" Manual                      (99)\n");
      printf("==================================\n");
      printf("-> "); DigitInput(&tempo);
      if ( tempo == 99) SelectAddress(param,command);
      else {
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        if ( tempo == 1 ) {
          if ( GetValue(IN_TESTSIZE0,param) != 0 ) SetValue(IN_TESTSIZE0,param,0x100);
          if ( GetValue(IN_TESTSIZE1,param) != 0 ) SetValue(IN_TESTSIZE1,param,0x100);
          if ( GetValue(IN_TESTSIZE2,param) != 0 ) SetValue(IN_TESTSIZE2,param,0x100);
        } else {
          if ( GetValue(IN_TESTSIZE0,param) != 0 ) SetValue(IN_TESTSIZE0,param,(0x1000 * (tempo/4)));
          if ( GetValue(IN_TESTSIZE1,param) != 0 ) SetValue(IN_TESTSIZE1,param,(0x1000 * (tempo/4)));
          if ( GetValue(IN_TESTSIZE2,param) != 0 ) SetValue(IN_TESTSIZE2,param,(0x1000 * (tempo/4)));
        }
      }
      break;
    case SAVE_REG:                        /**** Save Register ****/
      printf("enter File Name(Save Register) -> ");
      gets(reg_filename);
      SaveRegFile(reg_filename,param);
      break;
    case LOAD_REG:                        /**** Load Register ****/
      printf("enter File Name(Load Register) -> ");
      gets(reg_filename);
      LoadRegFile(reg_filename,param);
      break;

    case SET_WSIZE:
      printf("enter program size(8(8)/128(128)/256(0) -> ");
      DigitInput(&tempo);
      SetValue(IN_WRITESIZE,param,tempo);
      break;

    case SET_MON:
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x02);
      printf("===================\n");
      printf(" Vhh       (0)\n");
      printf(" Vpp       (1)\n");
      printf(" Vwi       (2)\n");
      printf(" Vdemg     (3)\n");
      printf(" Vreg20    (4)\n");
      printf(" Vnoemi    (5)\n");
      printf(" Vhh&vpp   (9)\n");
      printf("-------------------\n");
      printf(" Mat-OFF   (55)\n");
      printf("-------------------\n");
      printf(" ALL-Clear (88)\n");
      printf("===================\n");
      printf("enter temp set number->");DigitInput(&tempo0);
      
      switch(tempo0){
      case 0:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x01); // VHH monitor mode
          printf("Vhh monitor mode Set\n");
	  break;
      case 1:
          SetValue(IN_ETCR02,param,(GetValue(IN_ETCR02,param)&0xC0)|0x22); // VPP monitor mode
          printf("Vpp monitor mode Set\n");
	  break;
      case 2:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x05); // VWI monitor mode
          printf("Vwi monitor mode Set\n");
	  break;
      case 3:
	      SetValue(IN_ETCR02,param,(GetValue(IN_ETCR02,param)&0xC0)|0x23); // Vdemg monitor mode
          printf("Vdemg monitor mode Set\n");
	  break;
      case 4:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x03); // Vreg20 monitor mode
          printf("Vreg20 monitor mode Set\n");
      case 5:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x02); // Vnoemi monitor mode
          printf("Vnoemi monitor mode Set\n");
	  break;
      case 9:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x01); // VHH monitor mode
          SetValue(IN_ETCR02,param,(GetValue(IN_ETCR02,param)&0xC0)|0x22); // VPP monitor mode
          printf("Vhh&Vpp monitor mode Set\n");
	  break;
      case 55:
	      SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param)&0xF7)|0x08); // MatOFF mode
          printf("Mat-OFF Set\n");
	  break;
      case 88:
	      SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x00); // VCCMON monitor mode
	      SetValue(IN_ETCR02,param,(GetValue(IN_ETCR02,param)&0xC0)|0x00); // VSSMON monitor mode
	      SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param)&0xF7)|0x00); // MatOFF mode
          printf("ALL Clear \n");
	  break;
        default:
	    printf("input number ERROR!!! ->%d\n",tempo0);
	    break;
      }
      break;

    case SET_PULS:
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x01);
      printf("+----+----------+------+\n");
      printf("|    |   Write  |ETLR36|\n");
      printf("| No |   Plus   |[3:0] |\n");
      printf("+----+----------+------+\n");
      printf("|  0 | NoChange |  0x%1X |\n",GetValue(IN_ETLR36,param) & 0x0F );
      printf("+----+----------+------+\n");
      printf("|  1 |   3000u  |  0xF |<= Default\n");
      printf("|  2 |  10000u  |  0xE |\n");
      printf("|  3 |   8000u  |  0xD |\n");
      printf("|  4 |   7000u  |  0xC |\n");
      printf("+----+----------+------+\n");
      printf("|  5 |   6000u  |  0xB |\n");
      printf("|  6 |   5000u  |  0xA |\n");
      printf("|  7 |   4000u  |  0x9 |\n");
      printf("|  8 |   3500u  |  0x8 |\n");
      printf("+----+----------+------+\n");
      printf("|  9 |   2500u  |  0x7 |\n");
      printf("| 10 |   2000u  |  0x6 |\n");
      printf("| 11 |   1500u  |  0x5 |\n");
      printf("| 12 |   1000u  |  0x4 |\n");
      printf("+----+----------+------+\n");
      printf("| 13 |    800u  |  0x3 |\n");
      printf("| 14 |    700u  |  0x2 |\n");
      printf("| 15 |    600u  |  0x1 |\n");
      printf("| 16 |     50u  |  0x0 |\n");
      printf("+----+----------+------+\n");
      printf("Select No -> ");
      DigitInput(&tempo);
      if ( tempo != 0 ) {
        SetValue(IN_ETLR36,param,(GetValue(IN_ETLR36,param) & 0xF0 ) | (0xF - (tempo-1)));
      }

      printf("+----+----------+------+\n");
      printf("|    |   Erase  |ETLR37|\n");
      printf("| No |   Plus   |[3:0] |\n");
      printf("+----+----------+------+\n");
      printf("|  0 | NoChange |  0x%1X |\n",GetValue(IN_ETLR37,param) & 0x0F );
      printf("+----+----------+------+\n");
      printf("|  1 |   5000u  |  0xF |<= Default\n");
      printf("|  2 |  50000u  |  0xE |\n");
      printf("|  3 |  20000u  |  0xD |\n");
      printf("|  4 |  10000u  |  0xC |\n");
      printf("+----+----------+------+\n");
      printf("|  5 |   8000u  |  0xB |\n");
      printf("|  6 |   7000u  |  0xA |\n");
      printf("|  7 |   6000u  |  0x9 |\n");
      printf("|  8 |   5500u  |  0x8 |\n");
      printf("+----+----------+------+\n");
      printf("|  9 |   4500u  |  0x7 |\n");
      printf("| 10 |   4000u  |  0x6 |\n");
      printf("| 11 |   3000u  |  0x5 |\n");
      printf("| 12 |   2000u  |  0x4 |\n");
      printf("+----+----------+------+\n");
      printf("| 13 |   1000u  |  0x3 |\n");
      printf("| 14 |    800u  |  0x2 |\n");
      printf("| 15 |    600u  |  0x1 |\n");
      printf("| 16 |     50u  |  0x0 |\n");
      printf("+----+----------+------+\n");
      printf("Select No -> ");
      DigitInput(&tempo);
      if ( tempo != 0 ) {
        SetValue(IN_ETLR37,param,(GetValue(IN_ETLR37,param) & 0xF0 ) | (0xF - (tempo-1)));
      }
      break;
    case SET_IREF:
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x01);
      printf("+----+------+------+\n");
      printf("|    | Iref |ETLR25|\n");
      printf("| No | [uA] |[3:0] |\n");
      printf("+----+------+------+\n");
      printf("|  0 |  0.5 |  0x9 |\n");
      printf("|  1 |  1.0 |  0x8 |\n");
      printf("|  2 |  1.5 |  0xB |\n");
      printf("|  3 |  2.0 |  0xA |\n");
      printf("+----+------+------+\n");
      printf("|  4 |  2.5 |  0xD |\n");
      printf("|  5 |  3.0 |  0xC |\n");
      printf("|  6 |  3.5 |  0xF |<= Default\n");
      printf("|  7 |  4.0 |  0xE |\n");
      printf("+----+------+------+\n");
      printf("|  8 |  4.5 |  0x1 |\n");
      printf("|  9 |  5.0 |  0x0 |\n");
      printf("| 10 |  5.5 |  0x3 |\n");
      printf("| 11 |  6.0 |  0x2 |\n");
      printf("+----+------+------+\n");
      printf("| 12 |  6.5 |  0x5 |\n");
      printf("| 13 |  7.0 |  0x4 |\n");
      printf("| 14 |  7.5 |  0x7 |\n");
      printf("| 15 |  8.0 |  0x6 |\n");
      printf("+----+------+------+\n");
      printf("Select No -> ");
      DigitInput(&tempo);
      switch  ( tempo ) {
        case  0: tempo0 = 0x9; break;
        case  1: tempo0 = 0x8; break;
        case  2: tempo0 = 0xB; break;
        case  3: tempo0 = 0xA; break;
        case  4: tempo0 = 0xD; break;
        case  5: tempo0 = 0xC; break;
        case  6: tempo0 = 0xF; break;
        case  7: tempo0 = 0xE; break;
        case  8: tempo0 = 0x1; break;
        case  9: tempo0 = 0x0; break;
        case 10: tempo0 = 0x3; break;
        case 11: tempo0 = 0x2; break;
        case 12: tempo0 = 0x5; break;
        case 13: tempo0 = 0x4; break;
        case 14: tempo0 = 0x7; break;
        case 15: tempo0 = 0x6; break;
        default: break;
      }
        SetValue(IN_ETLR25,param,(GetValue(IN_ETLR25,param) & 0xF0 ) | tempo0);
      break;
    case SET_MUL:
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x02);
      printf("+----+---------------+----------------+\n");
      printf("| No | Mode          |Register        |\n");
      printf("+----+---------------+----------------+\n");
      printf("|  0 | User          |ETCR08[7:5]=001 |\n");
      printf("|  1 | User+Extra2-6 |ETCR08[7:5]=010 |\n");
      printf("|  2 | User+Extra1-6 |ETCR08[7:5]=100 |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 10 |   4KB         |ETCR08[2:0]=001 |\n");
      printf("| 11 |   8KB         |ETCR08[2:0]=010 |\n");
      printf("| 12 |  16KB         |ETCR08[2:0]=011 |\n");
      printf("| 13 |  32KB         |ETCR08[2:0]=100 |\n");
      printf("| 14 |  64KB         |ETCR08[2:0]=101 |\n");
      printf("| 15 | 128KB         |ETCR08[2:0]=110 |\n");
      printf("| 16 | 256KB         |ETCR08[2:0]=111 |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 20 | X Odd         |ETCR08[4:3]=01  |\n");
      printf("| 21 | X Even        |ETCR08[4:3]=10  |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 30 | Z all select  |ETCR07[5]=1     |\n"); //Add 2018/0704
      printf("| 31 | SG All Select |ETCR07[2]=1     |\n"); //~
      printf("| 32 | MG All Select |ETCR07[1]=1     |\n");
      printf("| 33 | SG/MG unselect|ETCR07[0]=1     |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 40 | YR Odd        |ETCR07[4:3]=10  |\n");
      printf("| 41 | YR Even       |ETCR07[4:3]=01  |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 50 | 2MAT Select   |ETCR06[5:4]=01  |\n");
      printf("| 51 | 4MAT Select   |ETCR06[5:4]=10  |\n");
      printf("+----+---------------+----------------+\n");
      printf("| 60 | MG64 Select   |ETCR06[6]=1     |\n"); //~
      printf("| 61 | MAT unselect  |ETCR06[3]=1     |\n"); //Add 2018/0704
      printf("+----+---------------+----------------+\n");
      printf("| 99 | ALL Clear     |                |\n");
      printf("+----+---------------+----------------+\n");
      printf("Select No -> ");
      DigitInput(&tempo);
      switch  ( tempo ) {
        case  0: tempo0 = 0x20; SetValue(IN_BDATA03,param,1); break;
        case  1: tempo0 = 0x40; SetValue(IN_BDATA03,param,2); break;
        case  2: tempo0 = 0x80; SetValue(IN_BDATA03,param,4); break;
        case 10: tempo0 = 0x01; SetValue(IN_BDATA02,param,1); break;
        case 11: tempo0 = 0x02; SetValue(IN_BDATA02,param,2); break;
        case 12: tempo0 = 0x03; SetValue(IN_BDATA02,param,3); break;
        case 13: tempo0 = 0x04; SetValue(IN_BDATA02,param,4); break;
        case 14: tempo0 = 0x05; SetValue(IN_BDATA02,param,5); break;
        case 15: tempo0 = 0x06; SetValue(IN_BDATA02,param,6); break;
        case 16: tempo0 = 0x07; SetValue(IN_BDATA02,param,7); break;
        case 20: 
        case 21: if(tempo==20){tempo0 = 0x08; }//xodd
                 else if(tempo == 21){tempo0 = 0x10;} //xeven
                 printf("    Select Area\n");
                 printf("    +----+---------------+----------------+\n"); 
                 printf("    |  0 | User          |ETCR08[7:5]=001 |\n");
                 printf("    |  1 | User+Extra2-6 |ETCR08[7:5]=010 |\n");
                 printf("    |  2 | User+Extra1-6 |ETCR08[7:5]=100 |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 10 |   4KB         |ETCR08[2:0]=001 |\n");
                 printf("    | 11 |   8KB         |ETCR08[2:0]=010 |\n");
                 printf("    | 12 |  16KB         |ETCR08[2:0]=011 |\n");
                 printf("    | 13 |  32KB         |ETCR08[2:0]=100 |\n");
                 printf("    | 14 |  64KB         |ETCR08[2:0]=101 |\n");
                 printf("    | 15 | 128KB         |ETCR08[2:0]=110 |\n");
                 printf("    | 16 | 256KB         |ETCR08[2:0]=111 |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 50 | 2MAT Select   |ETCR06[5:4]=01  |\n");
                 printf("    | 51 | 4MAT Select   |ETCR06[5:4]=10  |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 99 | All           |                |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    Select No -> ");
                 DigitInput(&tempo1);
                 switch  ( tempo1 ) {
                   case  0: tempo2 = 0x20; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case  1: tempo2 = 0x40; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case  2: tempo2 = 0x80; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case 10:
                   case 11:
                   case 12:
                   case 13:
                   case 14:
                   case 15:
                   case 16:
                          if(tempo1 == 10){tempo2 = 0x01; SetValue(IN_BDATA02,param,1);}
                     else if(tempo1 == 11){tempo2 = 0x02; SetValue(IN_BDATA02,param,2);}
                     else if(tempo1 == 12){tempo2 = 0x03; SetValue(IN_BDATA02,param,3);}
                     else if(tempo1 == 13){tempo2 = 0x04; SetValue(IN_BDATA02,param,4);}
                     else if(tempo1 == 14){tempo2 = 0x05; SetValue(IN_BDATA02,param,5);}
                     else if(tempo1 == 15){tempo2 = 0x06; SetValue(IN_BDATA02,param,6);}
                     else if(tempo1 == 16){tempo2 = 0x07; SetValue(IN_BDATA02,param,7);}
                     SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2);
                     break;
                   case 50: tempo2 = 0x10; SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xCF ) | tempo2); break;
                   case 51: tempo2 = 0x20; SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xCF ) | tempo2); break;
                   default: break;
                   }
                 break;
        case 30: tempo0 = 0x20; break; //Z all select
        case 31: tempo0 = 0x04; break; //SG All Select
        case 32: tempo0 = 0x02; break; //MG All Select
        case 33: tempo0 = 0x01; break; //SG/MG unselect
        case 40: tempo0 = 0x10; break; //YR Odd
        case 41: tempo0 = 0x08; break; //YR Even
        case 50: tempo0 = 0x10; break; //2MAT Select
        case 51: tempo0 = 0x20; break; //4MAT Select
        case 60: tempo0 = 0x40;
                 printf("    Select Area\n");
                 printf("    +----+---------------+----------------+\n"); 
                 printf("    |  0 | User          |ETCR08[7:5]=001 |\n");
                 printf("    |  1 | User+Extra2-6 |ETCR08[7:5]=010 |\n");
                 printf("    |  2 | User+Extra1-6 |ETCR08[7:5]=100 |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 10 |   4KB         |ETCR08[2:0]=001 |\n");
                 printf("    | 11 |   8KB         |ETCR08[2:0]=010 |\n");
                 printf("    | 12 |  16KB         |ETCR08[2:0]=011 |\n");
                 printf("    | 13 |  32KB         |ETCR08[2:0]=100 |\n");
                 printf("    | 14 |  64KB         |ETCR08[2:0]=101 |\n");
                 printf("    | 15 | 128KB         |ETCR08[2:0]=110 |\n");
                 printf("    | 16 | 256KB         |ETCR08[2:0]=111 |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 50 | 2MAT Select   |ETCR06[5:4]=01  |\n");
                 printf("    | 51 | 4MAT Select   |ETCR06[5:4]=10  |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    | 99 | All           |                |\n");
                 printf("    +----+---------------+----------------+\n");
                 printf("    Select No -> ");
                 DigitInput(&tempo1);
                 switch  ( tempo1 ) {
                   case  0: tempo2 = 0x20; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case  1: tempo2 = 0x40; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case  2: tempo2 = 0x80; SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2); break;
                   case 10:
                   case 11:
                   case 12:
                   case 13:
                   case 14:
                   case 15:
                   case 16:
                          if(tempo1 == 10){tempo2 = 0x01; SetValue(IN_BDATA02,param,1);}
                     else if(tempo1 == 11){tempo2 = 0x02; SetValue(IN_BDATA02,param,2);}
                     else if(tempo1 == 12){tempo2 = 0x03; SetValue(IN_BDATA02,param,3);}
                     else if(tempo1 == 13){tempo2 = 0x04; SetValue(IN_BDATA02,param,4);}
                     else if(tempo1 == 14){tempo2 = 0x05; SetValue(IN_BDATA02,param,5);}
                     else if(tempo1 == 15){tempo2 = 0x06; SetValue(IN_BDATA02,param,6);}
                     else if(tempo1 == 16){tempo2 = 0x07; SetValue(IN_BDATA02,param,7);}
                     SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo2);
                     break;
                   case 50: tempo2 = 0x10; SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xCF ) | tempo2); break;
                   case 51: tempo2 = 0x20; SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xCF ) | tempo2); break;
                   default: break;
                   }
                 break; //MG64 Select
        case 61: tempo0 = 0x08; break; //MAT unSelect
        case 99: SetValue(IN_ETCR08,param,0x00);
                 SetValue(IN_ETCR07,param,0x00); //Add 2018/0704
                 SetValue(IN_ETCR06,param,0x00); //Add 2018/0704
                 SetValue(IN_BDATA02,param,0);
                 SetValue(IN_BDATA03,param,0);
		 break;
        default: break;
      }
      if      ( tempo<10 ) SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0x1F ) | tempo0);
      else if ( tempo<20 ) SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0xF8 ) | tempo0);
      else if ( tempo<30 ) SetValue(IN_ETCR08,param,(GetValue(IN_ETCR08,param) & 0xE7 ) | tempo0);
      else if ( tempo<31 ) SetValue(IN_ETCR07,param,(GetValue(IN_ETCR07,param) & 0xDF ) | tempo0); //Z all select
      else if ( tempo<32 ) SetValue(IN_ETCR07,param,(GetValue(IN_ETCR07,param) & 0xFB ) | tempo0); //SG All Select
      else if ( tempo<33 ) SetValue(IN_ETCR07,param,(GetValue(IN_ETCR07,param) & 0xFD ) | tempo0); //MG All Selec
      else if ( tempo<34 ) SetValue(IN_ETCR07,param,(GetValue(IN_ETCR07,param) & 0xFE ) | tempo0); //SG/MG unselect
      else if ( tempo<50 ) SetValue(IN_ETCR07,param,(GetValue(IN_ETCR07,param) & 0xE7 ) | tempo0); //YR Odd/Even
      else if ( tempo<60 ) SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xCF ) | tempo0); //2/4MAT Select
      else if ( tempo<61 ) SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xBF ) | tempo0); //MG64 Select
      else if ( tempo<62 ) SetValue(IN_ETCR06,param,(GetValue(IN_ETCR06,param) & 0xF7 ) | tempo0); //MAT unSelect

      //print Reg value
      printf("\n");
      printf("BDATA2:0x%02X\n",GetValue(IN_BDATA02,param));
      printf("BDATA3:0x%02X\n",GetValue(IN_BDATA03,param));
      printf("ETCR06:0x%02X\n",GetValue(IN_ETCR06,param));
      printf("ETCR07:0x%02X\n",GetValue(IN_ETCR07,param));
      printf("ETCR08:0x%02X\n",GetValue(IN_ETCR08,param));
      printf("IN_REG:0x%02X, IN_REG|0x02:0x%02X\n",GetValue(IN_REG,param), GetValue(IN_REG,param)|0x02);
      //print Reg value end
      break;

    case SET_TAP:                   /**** set TAP ****/
      SetValue(IN_REG,param,GetValue(IN_REG,param)|0x01);

      while(1){
      printf("+----+-------+-------------------+\n");
      printf("| No | Item  |                   |\n");
      printf("+----+-------+-------------------+\n");
      printf("|  0 | vhh_e | ETLR16[5:0]:0x%02X  |\n",GetValue(IN_ETLR16,param) & 0x3F);
      printf("|  1 | vhh_w | ETLR17[5:0]:0x%02X  |\n",GetValue(IN_ETLR17,param) & 0x3F);
      printf("+----+-------+-------------------+\n");
      printf("|  2 | vpp_e | ETLR20[7:4]:0x%02X  |\n",GetValue(IN_ETLR20,param) >>4   );
      printf("|  3 | vpp_w | ETLR20[3:0]:0x%02X  |\n",GetValue(IN_ETLR20,param) & 0x0F);
      printf("+----+-------+-------------------+\n");
      printf("|  e | Exit                      |\n",GetValue(IN_ETLR13,param));
      printf("+----+---------------------------+\n");
      printf("Select No  -> "); DigitInput(&tempo0);
      if(EXIT==tempo0) break;
      printf("Change Tap -> "); DigitInput(&tempo1);

      switch (tempo0) {
        case 0:
          SetValue(IN_ETLR16,param,(GetValue(IN_ETLR16,param) + tempo1) | 0xC0);
          break;
        case 1:
          SetValue(IN_ETLR17,param,(GetValue(IN_ETLR17,param) + tempo1) | 0xC0);
          break;
        case 2:
          tempo2 = GetValue(IN_ETLR20,param);
          tempo3 = ((tempo2>>4) + tempo1) <<4;
          SetValue(IN_ETLR20,param,(tempo2&0x0F)|tempo3);
          break;
        case 3:
          tempo2 = GetValue(IN_ETLR20,param);
          tempo3 = ((tempo2&0x0F) + tempo1);
          SetValue(IN_ETLR20,param,(tempo2&0xF0)|tempo3);
          break;
        default: break;
      }
    }
    break;

  }
  return(PASS);
}

