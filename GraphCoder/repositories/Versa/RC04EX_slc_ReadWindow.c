/******************************************************************************/
/*!
  @file RC04EXslc_ReadWindow.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
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
  int info[32];                    /* wmat chip information              */
  char file_name[128];             /*                                    */
  char item_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_name[128];             /*                                    */
  char test_item[128];             /*                                    */
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

/**************************************************************************************/
/***   Initialization                                                               ***/
/**************************************************************************************/
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

/**************************************************************************************/
/***   Test Select                                                             ***/
/**************************************************************************************/
  printf("\n"); 
  printf("******************************************\n"); 
  printf("*** Read Window TEST                   ***\n");
  printf("*** Read Window TEST                   ***\n");
  printf("*** Read Window TEST                   ***\n");
  printf("******************************************\n"); 
  printf("\n"); 

      printf("*** Test Select [Middle_Vth-Set:0 / Distribution:1 / Distribution(Point):2 / Read-Count:3 / Read-Count for(T1T2):4 / Read-Count tmpIref:8 ]\n");
      printf("*** Test Select [Middle_Vth-Set@EXTRA:5 / Distribution@EXTRA:6 / Distribution(Point)@EXTRA:7 / Irefmonitor_tmpIref:9  Read-Count@EXTRA:10 ]\n");
      printf("*** Test Select [                       / Distribution@EXTRA:11 noGiji_TrimRead@irefRead / Exit:99 ->"); DigitInput(&cond1);
      if ( cond1 == 99 ) printf("*** Test exit!! ***");
      if ( cond1 == 3 || cond1 == 4 ) {
      printf("*** Test Temp   [-40:40 / 25C:25  / 95C:95 ] ->"); DigitInput(&cond2);
      }

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_REG,param,0x03);
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

/*
	info[0] = ReadEcrMode8(0x2A20,SIZE_B); info[1] = ReadEcrMode8(0x2A21,SIZE_B);
      info[2] = ReadEcrMode8(0x2A22,SIZE_B); info[3] = ReadEcrMode8(0x2A23,SIZE_B);
      info[4] = ReadEcrMode8(0x2A24,SIZE_B); info[5] = ReadEcrMode8(0x2A25,SIZE_B);
      info[10] = ReadEcrMode8(0x2A2A,SIZE_B);
      info[11] = ReadEcrMode8(0x2A2C,SIZE_B); info[12] = ReadEcrMode8(0x2A2E,SIZE_B);
*/
      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);
      info[20] = info[20] - 10000;
      sprintf(sample_name,"ReadWindow_%c%c%c%c%c%c_W%02d_S%03dX%dY%d",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[20],info[11],info[12]);

      if      (cond2 == 40) strcpy(test_name,"_-40C");
      else if (cond2 == 25) strcpy(test_name,"_25C");
      else if (cond2 == 95) strcpy(test_name,"_95C");
      else if ((cond1 == 1)|(cond1 == 2)) strcpy(test_name,"_Dis");
      else                  strcpy(test_name,"_000");

      strcpy(item_name,sample_name);
      strcat(item_name,test_name);

/**************************************************************************************/
/***   Middle_Vth-Set                                                         ***/
/**************************************************************************************/
  if ( cond1==0 ) {
    printf("\n");
    printf("**************************\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("**************************\n");
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
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
      SetValue(IN_ETCR08,param,0x00);

    etlr16 = GetValue(IN_ETLR16,param);
    etlr20 = GetValue(IN_ETLR20,param);
    etlr37 = GetValue(IN_ETLR37,param);

    printf("\n");
    printf("*** Ref ETLR16 =  H'%02X\n", GetValue(IN_ETLR16,param) );
    printf("*** Vhh_e(ETLR16) Tap Set Ex) 3(+3Tap) or -5(-5Tap) -> ");	DigitInput(&tempo1);
    SetValue(IN_ETLR16,param,(etlr16 + tempo1) | 0xC0);
//    SetValue(IN_ETLR16,param, tempo1); //Direct Input ex)0xFB
//    printf("*** Vpp_e(ETLR20) Tap Set -> ");	DigitInput(&tempo1);
//    etlr20 = (etlr20 & 0x0F) | (((etlr20 >> 4) + tempo1) << 4); 
//    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR20,param,((etlr20 & 0x0F) | 0x00 ));

    for ( i=0 ; i<25 ; i++ ) {
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x19000-(i*0x1000));     /* 4KB */

      if      ( count_32 > (32768/2) ) SetValue(IN_ETLR37,param,0xFF); /* Erase 5ms */
      else if ( count_32 > (32768/4) ) SetValue(IN_ETLR37,param,0xF4); /* Erase 2ms */
      else if ( count_32 > (32768/8) ) SetValue(IN_ETLR37,param,0xF3); /* Erase 1ms */
      else                             SetValue(IN_ETLR37,param,0xF1); /* Erase 0.6ms */ 

      SetValue(IN_EXTAL1,param,31);       /* 32MHz */
      SetValue(IN_BDATA00,param,1);		  /* Pre-Write Less */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/
      SetValue(IN_BDATA00,param,0);

      if ((i==0)|(i==3)|(i==6)|(i==9)|(i==12)|(i==15)|(i==18)|(i==21)) {
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
        SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);

        sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
        if( NULL==(file_count = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
          break;
        }
        if ( i == 0 ) fprintf(file_count,"No,32M,Tap=%d\n",tempo1);
    	  fprintf(file_count,"No_%d,%d\n",i,count_32);
    	  fclose( file_count);
	  }
	} /* for i */ 

    SetValue(IN_ETLR16,param,etlr16);
    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR37,param,etlr37);
  }
/**************************************************************************************/
/***   Distribution                                                                 ***/
/**************************************************************************************/
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
      SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
      if ( i==0 ) SetValue(IN_BDATA03,param,0);  /* Log header ON  */
      else        SetValue(IN_BDATA03,param,1);  /* Log header OFF */

      on_test=1;
      if ( cond1==2 ) {
        if ((i==0)|(i==12)|(i==24)) on_test=1;
        else on_test=0; 
      }
      if ( on_test==1 ) {
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);
        if      ( count_32 ==     0 ) min_vol = -500;
        else if ( count_32 == 32768 ) min_vol = -1800;
        else                          min_vol = -1000;

        SetValue(IN_EXTAL1,param,VTH_FREQ); 
        tempo2 = 100;                                 /* VS-Step Default   */
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
        vth_mode = 12;                                /* VthRead1&2  */
        result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
        if(PASS != result){
          sprintf(outbuf," Make Vth Bunpu Error!!!\n");
          DataOut(Ffpt,outbuf);
        }
      }
    } /* for i */
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_BDATA04,param,0);
  }

/**************************************************************************************/
/***   Count                                                                        ***/
/**************************************************************************************/
  if ( cond1==3 ) {
    printf("\n");
    printf("*** ireftemp On?   [0:OFF / 1:ON ] ->"); DigitInput(&ireftmp_sel);
  	etlr09 = GetValue(IN_ETLR09,param);// ireftemp
  	etlr12 = GetValue(IN_ETLR12,param);// ireftemp
  	
    etlr26 = GetValue(IN_ETLR26,param);
    etlr27 = GetValue(IN_ETLR27,param);
    etlr28 = GetValue(IN_ETLR28,param);
    etlr29 = GetValue(IN_ETLR29,param);
    etlr30 = GetValue(IN_ETLR30,param);
    etlr32 = GetValue(IN_ETLR32,param);
    etlr33 = GetValue(IN_ETLR33,param);
    etlr34 = GetValue(IN_ETLR34,param);

    if ( ireftmp_sel == 1 ) {
    printf("\n *** etlr12<3:0>= 0x00  target=2.57nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x01  target=3.4nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x02  target=4.23nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x03  target=5.06nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x04  target=5.88nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x05  target=6.71nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x06  target=7.54nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x07  target=8.37nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x08  target=9.2nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x09  target=10.03nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0A  target=10.86nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0B  target=11.69nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0C  target=12.52nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0D  target=13.35nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0E  target=14.18nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0F  target=15.01nA/C at 2.5uA***");
    printf("\n *** ireftemp target select!! etlr12<3:0> ex) 0x08, etlr12<7:4> is masked. -> ");DigitInput(&tempo16);

  	etlr09 = GetValue(IN_ETLR09,param);// ireftemp
  	etlr12 = GetValue(IN_ETLR12,param);// ireftemp
    SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //
    SetValue(IN_ETLR12,param,(etlr12 & 0xF0) | tempo16);  // ireftemp //
  	tempo17 = GetValue(IN_ETLR09,param);// ireftemp
    printf("\n ********* Before  etlr09<7:0>= 0x%02X *********",etlr09);
    printf("\n ********* After   etlr09<7:0>= 0x%02X *********",tempo17);
    printf("\n ********* Before  etlr12<7:0>= 0x%02X *********",etlr12);
    printf("\n ********* Setting etlr12<7:0>= 0x%02X *********",GetValue(IN_ETLR12,param));
    }

  	printf("\n");
    printf("*** Vdd/vddh Select ?   [0:OFF / 1:ON ] ->"); DigitInput(&vdd_sel);
    if ( vdd_sel == 0 ) {
    printf("*** Iref Select ?       [0:OFF / 1:ON ] ->"); DigitInput(&iref_sel);
    }
  	printf("*** Freq condition[1] Input!! (ex: 1MHz-> 1 @User /  0.4MHz-> 400 @TrimRead) ->");DigitInput(&freq01);
  	printf("*** Freq condition[2] Input!! (ex:32MHz->32 @User /, 8MHz  -> 8   @TrimRead) ->");DigitInput(&freq32);
    if ( freq01 >= 100 ) freq01*1000;
    if ( freq32 >= 100 ) freq32*1000;
    freq01ns = 1*1000/freq01;
    freq32ns = 1*1000/freq32;


  if ( vdd_sel != 1 ) {
    printf("*** Min vddh=1.13V  vdd=0.70V -> 0 \n");
    printf("*** Typ vddh=1.25V  vdd=0.79V -> 1 \n");
    printf("*** Max vddh=1.45V  vdd=1.15V -> 2 \n");

  	printf("*** Voltage Condition select? ->");DigitInput(&volcon);
        switch (volcon) {
        	case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
            case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
        	case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
        }
    }
  	printf("*** DelayTime chip condition select? (Measure for:vsVddVddh & vsIref & vsT3) [0:yes / 1:NO ] ->");DigitInput(&delaydef);
  	if ( vdd_sel == 0 && iref_sel == 0 ) {
    printf("*** 2nsDelay T4(saout) Select(etlr33<7:4>:FLI / etlr33<3:0>:FLP) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt4_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 ) {
    printf("*** 2nsDelay T6(T4-T5) Select(etlr34<7:4>:FLI / etlr34<3:0>:FLP) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt6_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 ) {
    printf("*** 2nsDelay T5(saenb2) Select(etlr32<7:4>) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt5_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 ) {
    printf("*** 2nsDelay T3(saenb) Select(etlr32<3:0>) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt3_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 ) {
    printf("*** 8nsDelay T1(PrechargeTime) Select(etlr26<7:0>:FLI1,FLP0 / etlr27<3:0>:FLI2) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt1_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 && delayt1_sel == 0 ) {
    printf("*** 10nsDelay T2(DischargeTime) Select(etlr28~etlr30<4:0>:FLP0,FLI1,FLI2 ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt2_sel);
    }

   	if ( delaydef != 0 ) {
    printf("*** T4 setting!! ETLR33 Select ?  ex) 0x11,0x33 7bit input ->"); DigitInput(&tempo7);
    SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay Read*/
    printf("*** T6 setting!! ETLR34 Select ?  ex) 0x11,0x44 7bit input ->"); DigitInput(&tempo8);
    SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay Read*/
    printf("*** T5 setting!! ETLR32 Select ?  ex) 0x10,0x23 <7:4> 4bit input ->"); DigitInput(&tempo9);
    SetValue(IN_ETLR32,param,(etlr32 & 0xF0) | tempo9);  /* 2ns Delay Read*/
    printf("*** T3 setting!! ETLR32 Select ?  ex) 0x01,0x23 <3:0> 4bit input ->"); DigitInput(&tempo10);
    SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | tempo10);  /* 2ns Delay Read*/
    printf("*** T1 8nsDelay setting!! ETLR26<7:0>=0x%02 / ETLR27<7:0>=0x%02\n  ex) 0x10 <7:4> 4bit input ->",GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param)); DigitInput(&tempo11);
    SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,(etlr27 & 0xF0) | (tempo11 & 0x0F));  /* 8ns PreCharge Delay maxTap */
    printf("*** T2 10nsDelay setting!! ETLR28<7:0>=0x%02 / ETLR29<7:0>=0x%02 / ETLR30<7:0>=0x%02\n  ex) 0x10 <4:0> 5bit input ->",GetValue(IN_ETLR28,param),GetValue(IN_ETLR29,param),GetValue(IN_ETLR29,param)); DigitInput(&tempo12);
    SetValue(IN_ETLR28,param,(etlr28 & 0xE0) | (tempo12 & 0x1F)),SetValue(IN_ETLR29,param,(etlr29 & 0xE0) | (tempo12 & 0x1F)),SetValue(IN_ETLR30,param,(etlr30 & 0xE0) | (tempo12 & 0x1F));//for k 10ns PrechargeTime 2.94ns //
	}

    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);
    SetValue(IN_REG,param,0x03);
    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

    if      ( vdd_sel  == 1 )  loop_num = 3;
    else if ( iref_sel == 1 )  loop_num = 5;
    else if ( delayt4_sel == 1 )  loop_num = 16;
    else if ( delayt6_sel == 1 )  loop_num = 16;
    else if ( delayt5_sel == 1 )  loop_num = 16;
    else if ( delayt3_sel == 1 )  loop_num = 16;
    else if ( delayt1_sel == 1 )  loop_num = 16;
    else if ( delayt2_sel == 1 )  loop_num = 8;
//    else if ( delayt2_sel == 1 )  loop_num = 32;
    else    loop_num = 1;

    for ( j=0 ; j<loop_num ; j++ ) {
      if ( vdd_sel == 1 ) {
        printf("*** vdd_sel is [%d]!!!!!!!!!\n", vdd_sel );
        printf("*** loop_num is [%d]@@@@@@@@@\n", loop_num );
        printf("*** @@@@@@@@@ switch j is [%d][%d][%d][%d][%d][%d][%d][%d][%d]\n", j );

        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    // Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) //
          case 1:  vs[1] = 500; vs[2] = 500; break;	    // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) //
          case 2:  vs[1] = 580; vs[2] = 728; break;	    // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) //
		  default: break;
		}
	  }

      if ( iref_sel == 1 ) {
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
          case 1:  SetValue(IN_ETLR25,param,0xFB); break;  /* Min Iref=1.5uA */
          case 2:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
          case 3:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
          case 4:  SetValue(IN_ETLR25,param,0xFC); break;  /* Min Iref=3.0uA */
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;  /* Min Iref=3.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;  /* Min Iref=5.0uA */
		  default: break;
		}
	  }

      if ( delayt4_sel == 1 ) {
//          SetValue(IN_ETLR33,param,0xFF); break;  /* 2ns Delay */
          SetValue(IN_ETLR34,param,0xFF); /* 2ns Delay */
          SetValue(IN_ETLR32,param,0xFF); /* 2ns Delay */
        switch (j) {
          case 0:  SetValue(IN_ETLR33,param,0x00); break;  /* 2ns Delay 0.82ns */
          case 1:  SetValue(IN_ETLR33,param,0x11); break;  /* 2ns Delay 0.95ns */
          case 2:  SetValue(IN_ETLR33,param,0x22); break;  /* 2ns Delay 1.11ns */
          case 3:  SetValue(IN_ETLR33,param,0x33); break;  /* 2ns Delay 1.24ns target */
          case 4:  SetValue(IN_ETLR33,param,0x44); break;  /* 2ns Delay 1.31ns */
          case 5:  SetValue(IN_ETLR33,param,0x55); break;  /* 2ns Delay 1.44ns */
          case 6:  SetValue(IN_ETLR33,param,0x66); break;  /* 2ns Delay 1.60ns */
          case 7:  SetValue(IN_ETLR33,param,0x77); break;  /* 2ns Delay 1.73ns */
          case 8:  SetValue(IN_ETLR33,param,0x88); break;  /* 2ns Delay 1.89ns */
          case 9:  SetValue(IN_ETLR33,param,0x99); break;  /* 2ns Delay 2.02ns */
          case 10:  SetValue(IN_ETLR33,param,0xAA); break;  /* 2ns Delay 2.18ns */
          case 11:  SetValue(IN_ETLR33,param,0xBB); break;  /* 2ns Delay 2.31ns */
          case 12:  SetValue(IN_ETLR33,param,0xCC); break;  /* 2ns Delay 2.38ns */
          case 13:  SetValue(IN_ETLR33,param,0xDD); break;  /* 2ns Delay 2.51ns */
          case 14:  SetValue(IN_ETLR33,param,0xEE); break;  /* 2ns Delay 2.67ns */
          case 15:  SetValue(IN_ETLR33,param,0xFF); break;  /* 2ns Delay 2.80ns Trimming Read*/
		  default: break;
		}
	  }
      if ( delayt6_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0x11 7bit input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
//          SetValue(IN_ETLR34,param,0xFF);  /* 2ns Delay */
          SetValue(IN_ETLR32,param,0xFF);  /* 2ns Delay */
      		}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR34,param,0x00); break;  /* 2ns Delay 0.82ns */
          case 1:  SetValue(IN_ETLR34,param,0x11); break;  /* 2ns Delay 0.95ns */
          case 2:  SetValue(IN_ETLR34,param,0x22); break;  /* 2ns Delay 1.11ns */
          case 3:  SetValue(IN_ETLR34,param,0x33); break;  /* 2ns Delay 1.24ns */
          case 4:  SetValue(IN_ETLR34,param,0x44); break;  /* 2ns Delay 1.31ns target */
          case 5:  SetValue(IN_ETLR34,param,0x55); break;  /* 2ns Delay 1.44ns */
          case 6:  SetValue(IN_ETLR34,param,0x66); break;  /* 2ns Delay 1.60ns */
          case 7:  SetValue(IN_ETLR34,param,0x77); break;  /* 2ns Delay 1.73ns */
          case 8:  SetValue(IN_ETLR34,param,0x88); break;  /* 2ns Delay 1.89ns */
          case 9:  SetValue(IN_ETLR34,param,0x99); break;  /* 2ns Delay 2.02ns */
          case 10:  SetValue(IN_ETLR34,param,0xAA); break;  /* 2ns Delay 2.18ns */
          case 11:  SetValue(IN_ETLR34,param,0xBB); break;  /* 2ns Delay 2.31ns */
          case 12:  SetValue(IN_ETLR34,param,0xCC); break;  /* 2ns Delay 2.38ns */
          case 13:  SetValue(IN_ETLR34,param,0xDD); break;  /* 2ns Delay 2.51ns */
          case 14:  SetValue(IN_ETLR34,param,0xEE); break;  /* 2ns Delay 2.67ns */
          case 15:  SetValue(IN_ETLR34,param,0xFF); break;  /* 2ns Delay 2.80ns Trimming Read*/
		  default: break;
		}
	  }
      else if ( delayt5_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0x11 7bit input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0x11 7bit input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
//          SetValue(IN_ETLR32,param,0xFF);  /* 2ns Delay */
      		}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x00); break;  // 2ns Delay 0.82ns //
          case 1:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x10); break;  // 2ns Delay 0.95ns //
          case 2:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x20); break;  // 2ns Delay 1.11ns target //
          case 3:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x30); break;  // 2ns Delay 1.24ns //
          case 4:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x40); break;  // 2ns Delay 1.31ns //
          case 5:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x50); break;  // 2ns Delay 1.44ns //
          case 6:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x60); break;  // 2ns Delay 1.60ns //
          case 7:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x70); break;  // 2ns Delay 1.73ns //
          case 8:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x80); break;  // 2ns Delay 1.89ns //
          case 9:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0x90); break;  // 2ns Delay 2.02ns //
          case 10:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xA0); break;  // 2ns Delay 2.18ns //
          case 11:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xB0); break;  // 2ns Delay 2.31ns //
          case 12:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xC0); break;  // 2ns Delay 2.38ns //
          case 13:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xD0); break;  // 2ns Delay 2.51ns //
          case 14:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xE0); break;  // 2ns Delay 2.67ns //
          case 15:  SetValue(IN_ETLR32,param,(etlr32 & 0x0F) | 0xF0); break;  // 2ns Delay 2.80ns Trimming Read//
		  default: break;
		}
	  }
      else if ( delayt3_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0x11 7bit input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay Read*/
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0x11 7bit input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay Read*/
          printf("*** T5 setting!! ETLR32 Select ?  ex) 0x10 <7:4> 4bit input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,(tempo9 & 0xF0));  /* 2ns Delay Read*/
          SetValue(IN_ETLR26,param,0x77),SetValue(IN_ETLR27,param,0x07);  /* 8ns PreCharge Delay maxTap */
          printf("*** T1 8nsDelay is MaxTap setting!! etlr26<7:0>=0x%02 / etlr27<7:0>=0x%02\n",GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param));
      		}
          }
      	if ( delaydef != 0 ) {
        switch (j) {
          case 0:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x00); break;  /* 2ns Delay 0.82ns */
          case 1:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x01); break;  /* 2ns Delay 0.95ns */
          case 2:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x02); break;  /* 2ns Delay 1.11ns target */
          case 3:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x03); break;  /* 2ns Delay 1.24ns */
          case 4:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x04); break;  /* 2ns Delay 1.31ns */
          case 5:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x05); break;  /* 2ns Delay 1.44ns */
          case 6:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x06); break;  /* 2ns Delay 1.60ns */
          case 7:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x07); break;  /* 2ns Delay 1.73ns */
          case 8:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x08); break;  /* 2ns Delay 1.89ns */
          case 9:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x09); break;  /* 2ns Delay 2.02ns */
          case 10:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0A); break;  /* 2ns Delay 2.18ns */
          case 11:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0B); break;  /* 2ns Delay 2.31ns */
          case 12:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0C); break;  /* 2ns Delay 2.38ns */
          case 13:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0D); break;  /* 2ns Delay 2.51ns */
          case 14:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0E); break;  /* 2ns Delay 2.67ns */
          case 15:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0F); break;  /* 2ns Delay 2.80ns Trimming Read*/
		  default: break;
			}
      	}
      	if ( delaydef == 0 ) {
      		tempo9 = GetValue(IN_ETLR32,param);
        switch (j) {
          case 0:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x00); break;  /* 2ns Delay 0.82ns */
          case 1:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x01); break;  /* 2ns Delay 0.95ns */
          case 2:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x02); break;  /* 2ns Delay 1.11ns target */
          case 3:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x03); break;  /* 2ns Delay 1.24ns */
          case 4:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x04); break;  /* 2ns Delay 1.31ns */
          case 5:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x05); break;  /* 2ns Delay 1.44ns */
          case 6:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x06); break;  /* 2ns Delay 1.60ns */
          case 7:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x07); break;  /* 2ns Delay 1.73ns */
          case 8:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x08); break;  /* 2ns Delay 1.89ns */
          case 9:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x09); break;  /* 2ns Delay 2.02ns */
          case 10:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0A); break;  /* 2ns Delay 2.18ns */
          case 11:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0B); break;  /* 2ns Delay 2.31ns */
          case 12:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0C); break;  /* 2ns Delay 2.38ns */
          case 13:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0D); break;  /* 2ns Delay 2.51ns */
          case 14:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0E); break;  /* 2ns Delay 2.67ns */
          case 15:  SetValue(IN_ETLR32,param,(tempo9 & 0xF0) | 0x0F); break;  /* 2ns Delay 2.80ns Trimming Read*/
		  default: break;
			}
      	}
	  }
      else if ( delayt1_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
      		}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR26,param,0x88),SetValue(IN_ETLR27,param,0x88 & 0x0F); break;  /* 8ns PrechargeTime 5.23ns */
          case 1:  SetValue(IN_ETLR26,param,0x99),SetValue(IN_ETLR27,param,0x99 & 0x0F); break;  /* 8ns PrechargeTime 5.62ns */
          case 2:  SetValue(IN_ETLR26,param,0xAA),SetValue(IN_ETLR27,param,0xAA & 0x0F); break;  /* 8ns PrechargeTime 6.04ns */
          case 3:  SetValue(IN_ETLR26,param,0xBB),SetValue(IN_ETLR27,param,0xBB & 0x0F); break;  /* 8ns PrechargeTime 6.44ns */
          case 4:  SetValue(IN_ETLR26,param,0xCC),SetValue(IN_ETLR27,param,0xCC & 0x0F); break;  /* 8ns PrechargeTime 6.74ns */
          case 5:  SetValue(IN_ETLR26,param,0xDD),SetValue(IN_ETLR27,param,0xDD & 0x0F); break;  /* 8ns PrechargeTime 7.14ns */
          case 6:  SetValue(IN_ETLR26,param,0xEE),SetValue(IN_ETLR27,param,0xEE & 0x0F); break;  /* 8ns PrechargeTime 7.54ns target(sf)*/
          case 7:  SetValue(IN_ETLR26,param,0xFF),SetValue(IN_ETLR27,param,0xFF & 0x0F); break;  /* 8ns PrechargeTime 7.93ns */
          case 8:  SetValue(IN_ETLR26,param,0x00),SetValue(IN_ETLR27,param,0x00 & 0x0F); break;  /* 8ns PrechargeTime 8.21ns target(tt,ff,ss) */
          case 9:  SetValue(IN_ETLR26,param,0x11),SetValue(IN_ETLR27,param,0x11 & 0x0F); break;  /* 8ns PrechargeTime 8.60ns target(fs)*/
          case 10:  SetValue(IN_ETLR26,param,0x22),SetValue(IN_ETLR27,param,0x22 & 0x0F); break;  /* 8ns PrechargeTime 9.03ns */
          case 11:  SetValue(IN_ETLR26,param,0x33),SetValue(IN_ETLR27,param,0x33 & 0x0F); break;  /* 8ns PrechargeTime 9.42ns */
          case 12:  SetValue(IN_ETLR26,param,0x44),SetValue(IN_ETLR27,param,0x44 & 0x0F); break;  /* 8ns PrechargeTime 9.71ns */
          case 13:  SetValue(IN_ETLR26,param,0x55),SetValue(IN_ETLR27,param,0x55 & 0x0F); break;  /* 8ns PrechargeTime 10.10ns */
          case 14:  SetValue(IN_ETLR26,param,0x66),SetValue(IN_ETLR27,param,0x66 & 0x0F); break;  /* 8ns PrechargeTime 10.51ns */
          case 15:  SetValue(IN_ETLR26,param,0x77),SetValue(IN_ETLR27,param,0x77 & 0x0F); break;  /* 8ns PrechargeTime 10.90ns Trimming Read*/
		  default: break;
		}
	  }
      else if ( delayt2_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo11);
          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Delay */
      		}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR28,param,0x10 & 0x1F),SetValue(IN_ETLR29,param,0x10 & 0x1F),SetValue(IN_ETLR30,param,0x10 & 0x1F); break;  // 10ns PrechargeTime 2.94ns //
          case 1:  SetValue(IN_ETLR28,param,0x11 & 0x1F),SetValue(IN_ETLR29,param,0x11 & 0x1F),SetValue(IN_ETLR30,param,0x11 & 0x1F); break;  // 10ns PrechargeTime 3.57ns //
          case 2:  SetValue(IN_ETLR28,param,0x12 & 0x1F),SetValue(IN_ETLR29,param,0x12 & 0x1F),SetValue(IN_ETLR30,param,0x12 & 0x1F); break;  // 10ns PrechargeTime 4.10ns //
          case 3:  SetValue(IN_ETLR28,param,0x13 & 0x1F),SetValue(IN_ETLR29,param,0x13 & 0x1F),SetValue(IN_ETLR30,param,0x13 & 0x1F); break;  // 10ns PrechargeTime 4.71ns //
          case 4:  SetValue(IN_ETLR28,param,0x14 & 0x1F),SetValue(IN_ETLR29,param,0x14 & 0x1F),SetValue(IN_ETLR30,param,0x14 & 0x1F); break;  // 10ns PrechargeTime 5.22ns //
          case 5:  SetValue(IN_ETLR28,param,0x15 & 0x1F),SetValue(IN_ETLR29,param,0x15 & 0x1F),SetValue(IN_ETLR30,param,0x15 & 0x1F); break;  // 10ns PrechargeTime 5.85ns target(sf)//
          case 6:  SetValue(IN_ETLR28,param,0x16 & 0x1F),SetValue(IN_ETLR29,param,0x16 & 0x1F),SetValue(IN_ETLR30,param,0x16 & 0x1F); break;  // 10ns PrechargeTime 6.36ns target(tt,ff,ss)//
          case 7:  SetValue(IN_ETLR28,param,0x17 & 0x1F),SetValue(IN_ETLR29,param,0x17 & 0x1F),SetValue(IN_ETLR30,param,0x17 & 0x1F); break;  // 10ns PrechargeTime 6.97ns target(fs)//
//          case 8:  SetValue(IN_ETLR28,param,0x18 & 0x1F),SetValue(IN_ETLR29,param,0x18 & 0x1F),SetValue(IN_ETLR30,param,0x18 & 0x1F); break;  // 10ns PrechargeTime 7.41ns //
//          case 9:  SetValue(IN_ETLR28,param,0x19 & 0x1F),SetValue(IN_ETLR29,param,0x19 & 0x1F),SetValue(IN_ETLR30,param,0x19 & 0x1F); break;  // 10ns PrechargeTime 8.04ns //
//          case 10:  SetValue(IN_ETLR28,param,0x1A & 0x1F),SetValue(IN_ETLR29,param,0x1A & 0x1F),SetValue(IN_ETLR30,param,0x1A & 0x1F); break;  // 10ns PrechargeTime 8.57ns //
//          case 11:  SetValue(IN_ETLR28,param,0x1B & 0x1F),SetValue(IN_ETLR29,param,0x1B & 0x1F),SetValue(IN_ETLR30,param,0x1B & 0x1F); break;  // 10ns PrechargeTime 9.18ns //
//          case 12:  SetValue(IN_ETLR28,param,0x1C & 0x1F),SetValue(IN_ETLR29,param,0x1C & 0x1F),SetValue(IN_ETLR30,param,0x1C & 0x1F); break;  // 10ns PrechargeTime 9.70ns //
//          case 13:  SetValue(IN_ETLR28,param,0x1D & 0x1F),SetValue(IN_ETLR29,param,0x1D & 0x1F),SetValue(IN_ETLR30,param,0x1D & 0x1F); break;  // 10ns PrechargeTime 10.33ns //
//          case 14:  SetValue(IN_ETLR28,param,0x1E & 0x1F),SetValue(IN_ETLR29,param,0x1E & 0x1F),SetValue(IN_ETLR30,param,0x1E & 0x1F); break;  // 10ns PrechargeTime 10.84ns //
//          case 15:  SetValue(IN_ETLR28,param,0x1F & 0x1F),SetValue(IN_ETLR29,param,0x1F & 0x1F),SetValue(IN_ETLR30,param,0x1F & 0x1F); break;  // 10ns PrechargeTime 11.44ns //
//          case 16:  SetValue(IN_ETLR28,param,0x00 & 0x1F),SetValue(IN_ETLR29,param,0x00 & 0x1F),SetValue(IN_ETLR30,param,0x00 & 0x1F); break;  // 10ns PrechargeTime 11.86ns //
//          case 17:  SetValue(IN_ETLR28,param,0x01 & 0x1F),SetValue(IN_ETLR29,param,0x01 & 0x1F),SetValue(IN_ETLR30,param,0x01 & 0x1F); break;  // 10ns PrechargeTime 12.51ns //
//          case 18:  SetValue(IN_ETLR28,param,0x02 & 0x1F),SetValue(IN_ETLR29,param,0x02 & 0x1F),SetValue(IN_ETLR30,param,0x02 & 0x1F); break;  // 10ns PrechargeTime 13.04ns //
//          case 19:  SetValue(IN_ETLR28,param,0x03 & 0x1F),SetValue(IN_ETLR29,param,0x03 & 0x1F),SetValue(IN_ETLR30,param,0x03 & 0x1F); break;  // 10ns PrechargeTime 13.65ns //
//          case 20:  SetValue(IN_ETLR28,param,0x04 & 0x1F),SetValue(IN_ETLR29,param,0x04 & 0x1F),SetValue(IN_ETLR30,param,0x04 & 0x1F); break;  // 10ns PrechargeTime 14.17ns //
//          case 21:  SetValue(IN_ETLR28,param,0x05 & 0x1F),SetValue(IN_ETLR29,param,0x05 & 0x1F),SetValue(IN_ETLR30,param,0x05 & 0x1F); break;  // 10ns PrechargeTime 14.81ns //
//          case 22:  SetValue(IN_ETLR28,param,0x06 & 0x1F),SetValue(IN_ETLR29,param,0x06 & 0x1F),SetValue(IN_ETLR30,param,0x06 & 0x1F); break;  // 10ns PrechargeTime 15.32ns //
//          case 23:  SetValue(IN_ETLR28,param,0x07 & 0x1F),SetValue(IN_ETLR29,param,0x07 & 0x1F),SetValue(IN_ETLR30,param,0x07 & 0x1F); break;  // 10ns PrechargeTime 15.93ns //
//          case 24:  SetValue(IN_ETLR28,param,0x08 & 0x1F),SetValue(IN_ETLR29,param,0x08 & 0x1F),SetValue(IN_ETLR30,param,0x08 & 0x1F); break;  // 10ns PrechargeTime 16.41ns //
//          case 25:  SetValue(IN_ETLR28,param,0x09 & 0x1F),SetValue(IN_ETLR29,param,0x09 & 0x1F),SetValue(IN_ETLR30,param,0x09 & 0x1F); break;  // 10ns PrechargeTime 17.04ns //
//          case 26:  SetValue(IN_ETLR28,param,0x0A & 0x1F),SetValue(IN_ETLR29,param,0x0A & 0x1F),SetValue(IN_ETLR30,param,0x0A & 0x1F); break;  // 10ns PrechargeTime 17.57ns //
//          case 27:  SetValue(IN_ETLR28,param,0x0B & 0x1F),SetValue(IN_ETLR29,param,0x0B & 0x1F),SetValue(IN_ETLR30,param,0x0B & 0x1F); break;  // 10ns PrechargeTime 18.18ns //
//          case 28:  SetValue(IN_ETLR28,param,0x0C & 0x1F),SetValue(IN_ETLR29,param,0x0C & 0x1F),SetValue(IN_ETLR30,param,0x0C & 0x1F); break;  // 10ns PrechargeTime 18.70ns //
//          case 29:  SetValue(IN_ETLR28,param,0x0D & 0x1F),SetValue(IN_ETLR29,param,0x0D & 0x1F),SetValue(IN_ETLR30,param,0x0D & 0x1F); break;  // 10ns PrechargeTime 19.33ns //
//          case 30:  SetValue(IN_ETLR28,param,0x0E & 0x1F),SetValue(IN_ETLR29,param,0x0E & 0x1F),SetValue(IN_ETLR30,param,0x0E & 0x1F); break;  // 10ns PrechargeTime 19.84ns //
//          case 31:  SetValue(IN_ETLR28,param,0x0F & 0x1F),SetValue(IN_ETLR29,param,0x0F & 0x1F),SetValue(IN_ETLR30,param,0x0F & 0x1F); break;  // 10ns PrechargeTime 20.44ns //
		  default: break;
		}
	  }

      else if (vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 && delayt1_sel == 0 && delayt2_sel == 0) {
        vs[1] = vs[2] = 500;
		SetValue(IN_ETLR25,param,0xFF);
		SetValue(IN_ETLR33,param,0xFF);
		SetValue(IN_ETLR34,param,0xFF);
		SetValue(IN_ETLR32,param,0xFF);
	  }

    printf("MONITOR imoni(Flash) notemp\n");
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    
    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);


    for ( i=0 ; i<25 ; i++ ) {
      printf("***** i== %d *****\n",i); 
      printf("***** i== %d *****\n",i); 
      printf("***** i== %d *****\n",i); 
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000); /* 4KB  */
//      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
//        count_1 = GetValue(OUT_LDATA0,param);
       count_1 = 0;
      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_32 = GetValue(OUT_LDATA0,param);

    	
    	
    	
      if      ( vdd_sel  == 1 ) sprintf(file_name,"%s%s%s_Vdd_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( iref_sel == 1 ) sprintf(file_name,"%s%s%s_Iref_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt4_sel == 1 ) sprintf(file_name,"%s%s%s_delayt4_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt6_sel == 1 ) sprintf(file_name,"%s%s%s_delayt6_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt5_sel == 1 ) sprintf(file_name,"%s%s%s_delayt5_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt3_sel == 1 ) sprintf(file_name,"%s%s%s_delayt3_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt1_sel == 1 ) sprintf(file_name,"%s%s%s_delayt1_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt2_sel == 1 ) sprintf(file_name,"%s%s%s_delayt2_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else                      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      if( NULL==(file_count = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }

      if ( i == 0 ) {
        printf("*** TOPADDR0 is %d!!!!!!!!!\n",GetValue(IN_TOPADDR0,param));
        printf("*** TESTSIZE0 is %d!!!!!!!!!\n",GetValue(IN_TESTSIZE0,param));
        fprintf(file_count,"TOPADDR0 =%d\n",GetValue(IN_TOPADDR0 ,param));
        fprintf(file_count,"TESTSIZE0=%d\n",GetValue(IN_TESTSIZE0,param));/* 4KB  */
        fprintf(file_count,"Vcc=%d,vddh=%d,vdd=%d,Iref=%d,ETLR09=%2X,ETLR12=%2X,ETLR25=%2X,ETLR26=%2X,ETLR27=%2X,ETLR28=%2X,ETLR29=%2X,ETLR30=%2X,ETLR32=%2X,ETLR33=%2X,ETLR34=%2X\n"
	                   ,vs[0],vs[1],vs[2],selvol,GetValue(IN_ETLR09,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param)
			   ,GetValue(IN_ETLR29,param),GetValue(IN_ETLR30,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param)); 
       if ( freq01 >= 100 ) fprintf(file_count,"No,%dK,%dM\n",freq01,freq32);
       if ( freq32 >= 100 ) fprintf(file_count,"No,%dM,%dK\n",freq01,freq32);
       if ( freq01 >= 100 && freq32 >= 100 ) fprintf(file_count,"No,%dK,%dK\n",freq01,freq32);
        fprintf(file_count,"No,%dM,%dM\n",freq01,freq32);
      }
	  fprintf(file_count,"No_%d,%d,%d\n",i,count_1,count_32);
      if ( i == 24 ) fprintf(file_count,"\n");
	  fclose( file_count);

	} /* for i */ 
	} /* for j */ 
    SetValue(IN_ETLR09,param,etlr09);
    SetValue(IN_ETLR12,param,etlr12);
    SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);
  }



/**************************************************************************************/
/***   Count For T1(Precharge),T2(Discharge)                                                                        ***/
/**************************************************************************************/
  if ( cond1==4 ) {
    printf("\n");
    printf("*** Vdd/vddh Select ?   [0:OFF / 1:ON ] ->"); DigitInput(&vdd_sel);
    if ( vdd_sel == 0 ) {
    printf("*** Iref Select ?       [0:OFF / 1:ON ] ->"); DigitInput(&iref_sel);
    }
  	printf("*** Freq condition[1] Input!! (ex: 1MHz-> 1 @User /  0.4MHz-> 400 @TrimRead) ->");DigitInput(&freq01);
  	printf("*** Freq condition[2] Input!! (ex:32MHz->32 @User /, 8MHz  -> 8   @TrimRead) ->");DigitInput(&freq32);
    if ( freq01 >= 100 ) freq01*1000;
    if ( freq32 >= 100 ) freq32*1000;
    freq01ns = 1*1000/freq01;
    freq32ns = 1*1000/freq32;

    if ( vdd_sel != 1 ) {
    printf("*** Min vddh=1.13V  vdd=0.70V -> 0 \n");
    printf("*** Typ vddh=1.25V  vdd=0.79V -> 1 \n");
    printf("*** Max vddh=1.45V  vdd=1.15V -> 2 \n");
    printf("*** Voltage Condition select? ->");DigitInput(&volcon);
        switch (volcon) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
    }

    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 ) {
    printf("*** 8nsDelay T1(PrechargeTime) Select(etlr26<7:0>:FLI1,FLP0 / etlr27<3:0>:FLI2) ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt1_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 && delayt1_sel == 0 ) {
    printf("*** 10nsDelay T2(DischargeTime) Select(etlr28~etlr30<4:0>:FLP0,FLI1,FLI2 ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt2_sel);
    }

    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);
    SetValue(IN_REG,param,0x03);
    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

    if      ( vdd_sel  == 1 )  loop_num = 3;
    else if ( iref_sel == 1 )  loop_num = 5;
    else if ( delayt1_sel == 1 ) {
    		loop_num = 16;
    printf("*** T2 10nsDelay Tap Start? [ex) min   Tap->0, min+1 Tap->1] ->"); DigitInput(&loop_kst);
    printf("*** T2 10nsDelay Tap End?   [ex) min+1 Tap->1, min+3 Tap->3] ->"); DigitInput(&loop_ked);
    		loop_k = loop_ked;
            }
    else if ( delayt2_sel == 1 )  loop_num = 16;
    else    loop_num = 1;

    for ( k=loop_kst ; k<=loop_k ; k++ ) {//for k 
            switch (k) {//for k 
          case 0:  SetValue(IN_ETLR28,param,0x10 & 0x1F),SetValue(IN_ETLR29,param,0x10 & 0x1F),SetValue(IN_ETLR30,param,0x10 & 0x1F); break;//for k 10ns PrechargeTime 2.94ns //
          case 1:  SetValue(IN_ETLR28,param,0x11 & 0x1F),SetValue(IN_ETLR29,param,0x11 & 0x1F),SetValue(IN_ETLR30,param,0x11 & 0x1F); break;//for k 10ns PrechargeTime 3.57ns //
          case 2:  SetValue(IN_ETLR28,param,0x12 & 0x1F),SetValue(IN_ETLR29,param,0x12 & 0x1F),SetValue(IN_ETLR30,param,0x12 & 0x1F); break;//for k 10ns PrechargeTime 4.10ns //
          case 3:  SetValue(IN_ETLR28,param,0x13 & 0x1F),SetValue(IN_ETLR29,param,0x13 & 0x1F),SetValue(IN_ETLR30,param,0x13 & 0x1F); break;//for k 10ns PrechargeTime 4.71ns //
          case 4:  SetValue(IN_ETLR28,param,0x14 & 0x1F),SetValue(IN_ETLR29,param,0x14 & 0x1F),SetValue(IN_ETLR30,param,0x14 & 0x1F); break;//for k 10ns PrechargeTime 5.22ns //
          case 5:  SetValue(IN_ETLR28,param,0x15 & 0x1F),SetValue(IN_ETLR29,param,0x15 & 0x1F),SetValue(IN_ETLR30,param,0x15 & 0x1F); break;//for k 10ns PrechargeTime 5.85ns target(sf)//
          case 6:  SetValue(IN_ETLR28,param,0x16 & 0x1F),SetValue(IN_ETLR29,param,0x16 & 0x1F),SetValue(IN_ETLR30,param,0x16 & 0x1F); break;//for k 10ns PrechargeTime 6.36ns target(tt,ff,ss)//
          case 7:  SetValue(IN_ETLR28,param,0x17 & 0x1F),SetValue(IN_ETLR29,param,0x17 & 0x1F),SetValue(IN_ETLR30,param,0x17 & 0x1F); break;//for k 10ns PrechargeTime 6.97ns target(fs)//
          case 8:  SetValue(IN_ETLR28,param,0x18 & 0x1F),SetValue(IN_ETLR29,param,0x18 & 0x1F),SetValue(IN_ETLR30,param,0x18 & 0x1F); break;//for k 10ns PrechargeTime 7.41ns //
          case 9:  SetValue(IN_ETLR28,param,0x19 & 0x1F),SetValue(IN_ETLR29,param,0x19 & 0x1F),SetValue(IN_ETLR30,param,0x19 & 0x1F); break;//for k 10ns PrechargeTime 8.04ns //
          case 10:  SetValue(IN_ETLR28,param,0x1A & 0x1F),SetValue(IN_ETLR29,param,0x1A & 0x1F),SetValue(IN_ETLR30,param,0x1A & 0x1F); break;//for k 10ns PrechargeTime 8.57ns //
          case 11:  SetValue(IN_ETLR28,param,0x1B & 0x1F),SetValue(IN_ETLR29,param,0x1B & 0x1F),SetValue(IN_ETLR30,param,0x1B & 0x1F); break;//for k 10ns PrechargeTime 9.18ns //
          case 12:  SetValue(IN_ETLR28,param,0x1C & 0x1F),SetValue(IN_ETLR29,param,0x1C & 0x1F),SetValue(IN_ETLR30,param,0x1C & 0x1F); break;//for k 10ns PrechargeTime 9.70ns //
          case 13:  SetValue(IN_ETLR28,param,0x1D & 0x1F),SetValue(IN_ETLR29,param,0x1D & 0x1F),SetValue(IN_ETLR30,param,0x1D & 0x1F); break;//for k 10ns PrechargeTime 10.33ns //
          case 14:  SetValue(IN_ETLR28,param,0x1E & 0x1F),SetValue(IN_ETLR29,param,0x1E & 0x1F),SetValue(IN_ETLR30,param,0x1E & 0x1F); break;//for k 10ns PrechargeTime 10.84ns //
          case 15:  SetValue(IN_ETLR28,param,0x1F & 0x1F),SetValue(IN_ETLR29,param,0x1F & 0x1F),SetValue(IN_ETLR30,param,0x1F & 0x1F); break;//for k 10ns PrechargeTime 11.44ns //
          case 16:  SetValue(IN_ETLR28,param,0x00 & 0x1F),SetValue(IN_ETLR29,param,0x00 & 0x1F),SetValue(IN_ETLR30,param,0x00 & 0x1F); break;//for k 10ns PrechargeTime 11.86ns //
          case 17:  SetValue(IN_ETLR28,param,0x01 & 0x1F),SetValue(IN_ETLR29,param,0x01 & 0x1F),SetValue(IN_ETLR30,param,0x01 & 0x1F); break;//for k 10ns PrechargeTime 12.51ns //
          case 18:  SetValue(IN_ETLR28,param,0x02 & 0x1F),SetValue(IN_ETLR29,param,0x02 & 0x1F),SetValue(IN_ETLR30,param,0x02 & 0x1F); break;//for k 10ns PrechargeTime 13.04ns //
          case 19:  SetValue(IN_ETLR28,param,0x03 & 0x1F),SetValue(IN_ETLR29,param,0x03 & 0x1F),SetValue(IN_ETLR30,param,0x03 & 0x1F); break;//for k 10ns PrechargeTime 13.65ns //
          case 20:  SetValue(IN_ETLR28,param,0x04 & 0x1F),SetValue(IN_ETLR29,param,0x04 & 0x1F),SetValue(IN_ETLR30,param,0x04 & 0x1F); break;//for k 10ns PrechargeTime 14.17ns //
          case 21:  SetValue(IN_ETLR28,param,0x05 & 0x1F),SetValue(IN_ETLR29,param,0x05 & 0x1F),SetValue(IN_ETLR30,param,0x05 & 0x1F); break;//for k 10ns PrechargeTime 14.81ns //
          case 22:  SetValue(IN_ETLR28,param,0x06 & 0x1F),SetValue(IN_ETLR29,param,0x06 & 0x1F),SetValue(IN_ETLR30,param,0x06 & 0x1F); break;//for k 10ns PrechargeTime 15.32ns //
          case 23:  SetValue(IN_ETLR28,param,0x07 & 0x1F),SetValue(IN_ETLR29,param,0x07 & 0x1F),SetValue(IN_ETLR30,param,0x07 & 0x1F); break;//for k 10ns PrechargeTime 15.93ns //
          case 24:  SetValue(IN_ETLR28,param,0x08 & 0x1F),SetValue(IN_ETLR29,param,0x08 & 0x1F),SetValue(IN_ETLR30,param,0x08 & 0x1F); break;//for k 10ns PrechargeTime 16.41ns //
          case 25:  SetValue(IN_ETLR28,param,0x09 & 0x1F),SetValue(IN_ETLR29,param,0x09 & 0x1F),SetValue(IN_ETLR30,param,0x09 & 0x1F); break;//for k 10ns PrechargeTime 17.04ns //
          case 26:  SetValue(IN_ETLR28,param,0x0A & 0x1F),SetValue(IN_ETLR29,param,0x0A & 0x1F),SetValue(IN_ETLR30,param,0x0A & 0x1F); break;//for k 10ns PrechargeTime 17.57ns //
          case 27:  SetValue(IN_ETLR28,param,0x0B & 0x1F),SetValue(IN_ETLR29,param,0x0B & 0x1F),SetValue(IN_ETLR30,param,0x0B & 0x1F); break;//for k 10ns PrechargeTime 18.18ns //
          case 28:  SetValue(IN_ETLR28,param,0x0C & 0x1F),SetValue(IN_ETLR29,param,0x0C & 0x1F),SetValue(IN_ETLR30,param,0x0C & 0x1F); break;//for k 10ns PrechargeTime 18.70ns //
          case 29:  SetValue(IN_ETLR28,param,0x0D & 0x1F),SetValue(IN_ETLR29,param,0x0D & 0x1F),SetValue(IN_ETLR30,param,0x0D & 0x1F); break;//for k 10ns PrechargeTime 19.33ns //
          case 30:  SetValue(IN_ETLR28,param,0x0E & 0x1F),SetValue(IN_ETLR29,param,0x0E & 0x1F),SetValue(IN_ETLR30,param,0x0E & 0x1F); break;//for k 10ns PrechargeTime 19.84ns //
          case 31:  SetValue(IN_ETLR28,param,0x0F & 0x1F),SetValue(IN_ETLR29,param,0x0F & 0x1F),SetValue(IN_ETLR30,param,0x0F & 0x1F); break;//for k 10ns PrechargeTime 20.44ns //
		  default: break;//for k
		}//for k
    for ( j=0 ; j<loop_num ; j++ ) {
      if ( vdd_sel == 1 ) {
        printf("*** vdd_sel is [%d]!!!!!!!!!\n", vdd_sel );
        printf("*** loop_num is [%d]@@@@@@@@@\n", loop_num );
        printf("*** @@@@@@@@@ switch j is [%d][%d][%d][%d][%d][%d][%d][%d][%d]\n", j );

        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
	  }
      if ( iref_sel == 1 ) {
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
          case 1:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */

//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;  /* Min Iref=3.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;  /* Min Iref=5.0uA */
		  default: break;
		}
	  }
      else if ( delayt1_sel == 1 ) {
      if ( j == 0 && k == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?   ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T2 discharge setting!! ETLR28<7:0>,ETLR29<7:0>,ETLR30<7:0> Select ?  ex) 0xF0 input ->"); DigitInput(&tempo13);
          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);  /* 10ns Discharge time*/
          	}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR26,param,0x88),SetValue(IN_ETLR27,param,0xF8 & 0x0F); break;  /* 8ns PrechargeTime 1.88ns */
          case 1:  SetValue(IN_ETLR26,param,0x99),SetValue(IN_ETLR27,param,0xF9 & 0x0F); break;  /* 8ns PrechargeTime 2.21ns */
          case 2:  SetValue(IN_ETLR26,param,0xAA),SetValue(IN_ETLR27,param,0xFA & 0x0F); break;  /* 8ns PrechargeTime 2.55ns */
          case 3:  SetValue(IN_ETLR26,param,0xBB),SetValue(IN_ETLR27,param,0xFB & 0x0F); break;  /* 8ns PrechargeTime 2.87ns */
          case 4:  SetValue(IN_ETLR26,param,0xCC),SetValue(IN_ETLR27,param,0xFC & 0x0F); break;  /* 8ns PrechargeTime 3.09ns */
          case 5:  SetValue(IN_ETLR26,param,0xDD),SetValue(IN_ETLR27,param,0xFD & 0x0F); break;  /* 8ns PrechargeTime 3.42ns */
          case 6:  SetValue(IN_ETLR26,param,0xEE),SetValue(IN_ETLR27,param,0xFE & 0x0F); break;  /* 8ns PrechargeTime 3.73ns */
          case 7:  SetValue(IN_ETLR26,param,0xFF),SetValue(IN_ETLR27,param,0xFF & 0x0F); break;  /* 8ns PrechargeTime 4.05ns target(sf,ss,tt)*/
          case 8:  SetValue(IN_ETLR26,param,0x00),SetValue(IN_ETLR27,param,0xF0 & 0x0F); break;  /* 8ns PrechargeTime 4.21ns target(ff)*/
          case 9:  SetValue(IN_ETLR26,param,0x11),SetValue(IN_ETLR27,param,0xF1 & 0x0F); break;  /* 8ns PrechargeTime 4.53ns target(fs)*/
          case 10:  SetValue(IN_ETLR26,param,0x22),SetValue(IN_ETLR27,param,0xF2 & 0x0F); break;  /* 8ns PrechargeTime 4.87ns */
          case 11:  SetValue(IN_ETLR26,param,0x33),SetValue(IN_ETLR27,param,0xF3 & 0x0F); break;  /* 8ns PrechargeTime 5.19ns */
          case 12:  SetValue(IN_ETLR26,param,0x44),SetValue(IN_ETLR27,param,0xF4 & 0x0F); break;  /* 8ns PrechargeTime 5.40ns */
          case 13:  SetValue(IN_ETLR26,param,0x55),SetValue(IN_ETLR27,param,0xF5 & 0x0F); break;  /* 8ns PrechargeTime 5.72ns */
          case 14:  SetValue(IN_ETLR26,param,0x66),SetValue(IN_ETLR27,param,0xF6 & 0x0F); break;  /* 8ns PrechargeTime 6.04ns */
          case 15:  SetValue(IN_ETLR26,param,0x77),SetValue(IN_ETLR27,param,0xF7 & 0x0F); break;  /* 8ns PrechargeTime 6.36ns Trimming Read*/
		  default: break;
		}
	  }
      else if ( delayt2_sel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo11);
          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Precharge time*/
          	}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR28,param,0x10 & 0x1F),SetValue(IN_ETLR29,param,0x10 & 0x1F),SetValue(IN_ETLR30,param,0x10 & 0x1F); break;  // 10ns PrechargeTime 2.36ns //
          case 1:  SetValue(IN_ETLR28,param,0x11 & 0x1F),SetValue(IN_ETLR29,param,0x11 & 0x1F),SetValue(IN_ETLR30,param,0x11 & 0x1F); break;  // 10ns PrechargeTime 2.87ns //
          case 2:  SetValue(IN_ETLR28,param,0x12 & 0x1F),SetValue(IN_ETLR29,param,0x12 & 0x1F),SetValue(IN_ETLR30,param,0x12 & 0x1F); break;  // 10ns PrechargeTime 3.28ns target(sf)//
          case 3:  SetValue(IN_ETLR28,param,0x13 & 0x1F),SetValue(IN_ETLR29,param,0x13 & 0x1F),SetValue(IN_ETLR30,param,0x13 & 0x1F); break;  // 10ns PrechargeTime 3.77ns target(tt,ff,ss)//
          case 4:  SetValue(IN_ETLR28,param,0x14 & 0x1F),SetValue(IN_ETLR29,param,0x14 & 0x1F),SetValue(IN_ETLR30,param,0x14 & 0x1F); break;  // 10ns PrechargeTime 4.13ns target(fs)//
          case 5:  SetValue(IN_ETLR28,param,0x15 & 0x1F),SetValue(IN_ETLR29,param,0x15 & 0x1F),SetValue(IN_ETLR30,param,0x15 & 0x1F); break;  // 10ns PrechargeTime 4.64ns //
          case 6:  SetValue(IN_ETLR28,param,0x16 & 0x1F),SetValue(IN_ETLR29,param,0x16 & 0x1F),SetValue(IN_ETLR30,param,0x16 & 0x1F); break;  // 10ns PrechargeTime 5.04ns //
          case 7:  SetValue(IN_ETLR28,param,0x17 & 0x1F),SetValue(IN_ETLR29,param,0x17 & 0x1F),SetValue(IN_ETLR30,param,0x17 & 0x1F); break;  // 10ns PrechargeTime 5.52ns //
          case 8:  SetValue(IN_ETLR28,param,0x18 & 0x1F),SetValue(IN_ETLR29,param,0x18 & 0x1F),SetValue(IN_ETLR30,param,0x18 & 0x1F); break;  // 10ns PrechargeTime 5.82ns //
          case 9:  SetValue(IN_ETLR28,param,0x19 & 0x1F),SetValue(IN_ETLR29,param,0x19 & 0x1F),SetValue(IN_ETLR30,param,0x19 & 0x1F); break;  // 10ns PrechargeTime 6.33ns //
          case 10:  SetValue(IN_ETLR28,param,0x1A & 0x1F),SetValue(IN_ETLR29,param,0x1A & 0x1F),SetValue(IN_ETLR30,param,0x1A & 0x1F); break;  // 10ns PrechargeTime 6.75ns //
          case 11:  SetValue(IN_ETLR28,param,0x1B & 0x1F),SetValue(IN_ETLR29,param,0x1B & 0x1F),SetValue(IN_ETLR30,param,0x1B & 0x1F); break;  // 10ns PrechargeTime 7.23ns //
          case 12:  SetValue(IN_ETLR28,param,0x1C & 0x1F),SetValue(IN_ETLR29,param,0x1C & 0x1F),SetValue(IN_ETLR30,param,0x1C & 0x1F); break;  // 10ns PrechargeTime 7.61ns //
          case 13:  SetValue(IN_ETLR28,param,0x1D & 0x1F),SetValue(IN_ETLR29,param,0x1D & 0x1F),SetValue(IN_ETLR30,param,0x1D & 0x1F); break;  // 10ns PrechargeTime 8.11ns //
          case 14:  SetValue(IN_ETLR28,param,0x1E & 0x1F),SetValue(IN_ETLR29,param,0x1E & 0x1F),SetValue(IN_ETLR30,param,0x1E & 0x1F); break;  // 10ns PrechargeTime 8.51ns //
          case 15:  SetValue(IN_ETLR28,param,0x1F & 0x1F),SetValue(IN_ETLR29,param,0x1F & 0x1F),SetValue(IN_ETLR30,param,0x1F & 0x1F); break;  // 10ns PrechargeTime 8.99ns //
          case 16:  SetValue(IN_ETLR28,param,0x00 & 0x1F),SetValue(IN_ETLR29,param,0x00 & 0x1F),SetValue(IN_ETLR30,param,0x00 & 0x1F); break;  // 10ns PrechargeTime 9.22ns //
          case 17:  SetValue(IN_ETLR28,param,0x01 & 0x1F),SetValue(IN_ETLR29,param,0x01 & 0x1F),SetValue(IN_ETLR30,param,0x01 & 0x1F); break;  // 10ns PrechargeTime 9.74ns //
          case 18:  SetValue(IN_ETLR28,param,0x02 & 0x1F),SetValue(IN_ETLR29,param,0x02 & 0x1F),SetValue(IN_ETLR30,param,0x02 & 0x1F); break;  // 10ns PrechargeTime 10.15ns //
          case 19:  SetValue(IN_ETLR28,param,0x03 & 0x1F),SetValue(IN_ETLR29,param,0x03 & 0x1F),SetValue(IN_ETLR30,param,0x03 & 0x1F); break;  // 10ns PrechargeTime 10.64ns //
          case 20:  SetValue(IN_ETLR28,param,0x04 & 0x1F),SetValue(IN_ETLR29,param,0x04 & 0x1F),SetValue(IN_ETLR30,param,0x04 & 0x1F); break;  // 10ns PrechargeTime 11.02ns //
          case 21:  SetValue(IN_ETLR28,param,0x05 & 0x1F),SetValue(IN_ETLR29,param,0x05 & 0x1F),SetValue(IN_ETLR30,param,0x05 & 0x1F); break;  // 10ns PrechargeTime 11.53ns //
          case 22:  SetValue(IN_ETLR28,param,0x06 & 0x1F),SetValue(IN_ETLR29,param,0x06 & 0x1F),SetValue(IN_ETLR30,param,0x06 & 0x1F); break;  // 10ns PrechargeTime 11.93ns //
          case 23:  SetValue(IN_ETLR28,param,0x07 & 0x1F),SetValue(IN_ETLR29,param,0x07 & 0x1F),SetValue(IN_ETLR30,param,0x07 & 0x1F); break;  // 10ns PrechargeTime 12.41ns //
          case 24:  SetValue(IN_ETLR28,param,0x08 & 0x1F),SetValue(IN_ETLR29,param,0x08 & 0x1F),SetValue(IN_ETLR30,param,0x08 & 0x1F); break;  // 10ns PrechargeTime 12.76ns //
          case 25:  SetValue(IN_ETLR28,param,0x09 & 0x1F),SetValue(IN_ETLR29,param,0x09 & 0x1F),SetValue(IN_ETLR30,param,0x09 & 0x1F); break;  // 10ns PrechargeTime 13.26ns //
          case 26:  SetValue(IN_ETLR28,param,0x0A & 0x1F),SetValue(IN_ETLR29,param,0x0A & 0x1F),SetValue(IN_ETLR30,param,0x0A & 0x1F); break;  // 10ns PrechargeTime 13.67ns //
          case 27:  SetValue(IN_ETLR28,param,0x0B & 0x1F),SetValue(IN_ETLR29,param,0x0B & 0x1F),SetValue(IN_ETLR30,param,0x0B & 0x1F); break;  // 10ns PrechargeTime 14.16ns //
          case 28:  SetValue(IN_ETLR28,param,0x0C & 0x1F),SetValue(IN_ETLR29,param,0x0C & 0x1F),SetValue(IN_ETLR30,param,0x0C & 0x1F); break;  // 10ns PrechargeTime 14.54ns //
          case 29:  SetValue(IN_ETLR28,param,0x0D & 0x1F),SetValue(IN_ETLR29,param,0x0D & 0x1F),SetValue(IN_ETLR30,param,0x0D & 0x1F); break;  // 10ns PrechargeTime 15.04ns //
          case 30:  SetValue(IN_ETLR28,param,0x0E & 0x1F),SetValue(IN_ETLR29,param,0x0E & 0x1F),SetValue(IN_ETLR30,param,0x0E & 0x1F); break;  // 10ns PrechargeTime 15.43ns //
          case 31:  SetValue(IN_ETLR28,param,0x0F & 0x1F),SetValue(IN_ETLR29,param,0x0F & 0x1F),SetValue(IN_ETLR30,param,0x0F & 0x1F); break;  // 10ns PrechargeTime 15.92ns //
		  default: break;
		}
	  }

      else if (vdd_sel == 0 && iref_sel == 0 && delayt1_sel == 0 && delayt2_sel == 0) {
 printf("***** DEBUG15 *****\n"); 
        vs[1] = vs[2] = 500;
		SetValue(IN_ETLR25,param,0xFF);
		SetValue(IN_ETLR33,param,0xFF);
		SetValue(IN_ETLR34,param,0xFF);
		SetValue(IN_ETLR32,param,0xFF);
	  }

    printf("MONITOR imoni(Flash) notemp\n");
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    

    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);


    for ( i=0 ; i<25 ; i++ ) {
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000); /* 4KB  */
//      SetValue(IN_EXTAL1,param,1000);        /* 1MHz */
//      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
//        count_1 = GetValue(OUT_LDATA0,param);
		  count_1 = 0;
//      SetValue(IN_EXTAL1,param,31);        /* 32MHz */
      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_32 = GetValue(OUT_LDATA0,param);

      if      ( vdd_sel  == 1 ) sprintf(file_name,"%s%s%s_Vdd_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( iref_sel == 1 ) sprintf(file_name,"%s%s%s_Iref_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( delayt1_sel == 1 ) sprintf(file_name,"%s%s%s_delayt1_%d_Dis0x%d.csv", glob_cdp, glob_datalog, item_name, get_site_number(), k);
      else if ( delayt2_sel == 1 ) sprintf(file_name,"%s%s%s_delayt2_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else                      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      if( NULL==(file_count = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }

      if ( i == 0 ) {
        printf("*** TOPADDR0 is %d!!!!!!!!!\n",GetValue(IN_TOPADDR0,param));
        printf("*** TESTSIZE0 is %d!!!!!!!!!\n",GetValue(IN_TESTSIZE0,param));
        fprintf(file_count,"TOPADDR0 =%d\n",GetValue(IN_TOPADDR0 ,param));
        fprintf(file_count,"TESTSIZE0=%d\n",GetValue(IN_TESTSIZE0,param));/* 4KB  */
        fprintf(file_count,"Vcc=%d,vddh=%d,vdd=%d,Iref=%d,ETLR09=%2X,ETLR12=%2X,ETLR25=%2X,ETLR26=%2X,ETLR27=%2X,ETLR28=%2X,ETLR29=%2X,ETLR30=%2X,ETLR32=%2X,ETLR33=%2X,ETLR34=%2X\n"
	                   ,vs[0],vs[1],vs[2],selvol,GetValue(IN_ETLR09,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param)
			   ,GetValue(IN_ETLR29,param),GetValue(IN_ETLR30,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param)); 
       if ( freq01 >= 100 ) fprintf(file_count,"No,%dK,%dM\n",freq01,freq32);
       if ( freq32 >= 100 ) fprintf(file_count,"No,%dM,%dK\n",freq01,freq32);
       if ( freq01 >= 100 && freq32 >= 100 ) fprintf(file_count,"No,%dK,%dK\n",freq01,freq32);
        fprintf(file_count,"No,%dM,%dM\n",freq01,freq32);
      }
	  fprintf(file_count,"No_%d,%d,%d\n",i,count_1,count_32);
      if ( i == 24 ) fprintf(file_count,"\n");
	  fclose( file_count);

	} /* for i */ 
	} /* for j */ 
	} /* for k */
     SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);
  }
/**************************************************************************************/
/***   Middle_Vth-Set extra                                                         ***/
/**************************************************************************************/
  if ( cond1==5 ) {
    printf("\n");
    printf("**************************\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("*** Middle_Vth-Set OK? ***\n");
    printf("*** Extra6 is used!!   ***\n");
    printf("*** Extra6 is used!!   ***\n");
    printf("*** Extra6 is used!!   ***\n");
    printf("**************************\n");
    printf("*** OK=0 / NO=Other-No ->"); DigitInput(&tempo);
    if ( tempo != 0 ) return(PASS);
	    SetValue(IN_AREA,param,6); //Extra6
	    SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
	    SetValue(IN_TESTSIZE0,param,0x4000);/* 16KB */
	    SetValue(IN_TOPADDR1,param,0);
	    SetValue(IN_TESTSIZE1,param,0);
	    SetValue(IN_TOPADDR2,param,0);
	    SetValue(IN_TESTSIZE2,param,0);

//      SetValue(IN_AREA,param,0);
//      SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
//      SetValue(IN_TESTSIZE0,param,0x100);
//      SetValue(IN_TOPADDR1 ,param,0);
//      SetValue(IN_TESTSIZE1,param,0);
//      SetValue(IN_TOPADDR2 ,param,0);
//      SetValue(IN_TESTSIZE2,param,0);
//      SetValue(IN_ETCR08,param,0x20);
      SetValue(IN_REG,param,0x03);
      SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");  /*ファーム次第で変更する可能性あり*/
//      SetValue(IN_ETCR08,param,0x00);

    etlr16 = GetValue(IN_ETLR16,param);
    etlr20 = GetValue(IN_ETLR20,param);
    etlr37 = GetValue(IN_ETLR37,param);
    SetValue(IN_ETCR06,param,0x80);

  	printf("\n");
    printf("*** Ref ETLR16 =  H'%02X\n", GetValue(IN_ETLR16,param) );
    printf("*** Vhh_e(ETLR16) Tap Set Ex) 3(+3Tap) or -5(-5Tap) -> ");	DigitInput(&tempo1);
    printf("*** -Tap x Xbai -> ");	DigitInput(&tempo2);
    SetValue(IN_ETLR16,param,(etlr16 + tempo1) | 0xC0);
//    SetValue(IN_ETLR16,param, tempo1); //Direct Input ex)0xFB
//    printf("*** Vpp_e(ETLR20) Tap Set -> ");	DigitInput(&tempo1);
//    etlr20 = (etlr20 & 0x0F) | (((etlr20 >> 4) + tempo1) << 4); 
//    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR20,param,((etlr20 & 0x0F) | 0x00 ));

    for ( i=0 ; i<4 ; i++ ) {
        for ( j=i ; j<4 ; j++) {
      SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(j*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000);     /* 4KB */

      if      ( count_32 > (32768/4) ) SetValue(IN_ETLR37,param,0xF5),SetValue(IN_ETLR16,param,(etlr16 + tempo1 +tempo2*4) | 0xC0); /* Erase 0.6ms */
      else if ( count_32 > (32768/8) ) SetValue(IN_ETLR37,param,0xF4),SetValue(IN_ETLR16,param,(etlr16 + tempo1 +tempo2*3) | 0xC0); /* Erase 0.6ms */
      else if ( count_32 > (32768/16) ) SetValue(IN_ETLR37,param,0xF3),SetValue(IN_ETLR16,param,(etlr16 + tempo1 +tempo2*2) | 0xC0); /* Erase 0.6ms */
      else if ( count_32 > 1 )          SetValue(IN_ETLR37,param,0xF2),SetValue(IN_ETLR16,param,(etlr16 + tempo1 +tempo2*1) | 0xC0); /* Erase 0.6ms */ 
      else                             SetValue(IN_ETLR37,param,0xF5); /* Erase 3ms */ 
      if      ( count_32 > (32768/4) ) printf("***1 3ms x4 -> ");/* Erase 3ms */
      else if ( count_32 > (32768/8) ) printf("***2 2ms x3 -> ");/* Erase 2ms */
      else if ( count_32 > (32768/16) )printf("***3 1ms x2 -> ");/* Erase 1ms */
      else if ( count_32 > 1 )         printf("***4 0.8ms x1 -> ");/* Erase 0.8ms */ 
      else                             printf("***5 3ms -> ");/* Erase 3ms */ 

      SetValue(IN_EXTAL1,param,31);       /* 32MHz */
      SetValue(IN_BDATA00,param,1);		  /* Pre-Write Less */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase"); /*ファーム次第で変更する可能性あり*/
      SetValue(IN_BDATA00,param,0);
        }
        
      if ((i==0)|(i==1)|(i==2)|(i==3)) {
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(i*0x1000));
        SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);

        sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
        if( NULL==(file_count = fopen( file_name, "at" )) ){
          printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
          break;
        }
        if ( i == 0 ) fprintf(file_count,"No,32M,Tap=%d\n",tempo1);
    	  fprintf(file_count,"No_%d,%d\n",i,count_32);
    	  fclose( file_count);
	  }
	} /* for i */ 

    SetValue(IN_ETLR16,param,etlr16);
    SetValue(IN_ETLR20,param,etlr20);
    SetValue(IN_ETLR37,param,etlr37);
  }
/**************************************************************************************/
/***   Distribution@Extra                                                                 ***/
/**************************************************************************************/
  if (( cond1==6 ) | ( cond1==7 )) {
    SetValue(IN_AREA,param,6); //Extra6
    SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
    SetValue(IN_TESTSIZE0,param,EXTRAB_SIZE);
    SetValue(IN_TOPADDR1,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    SetValue(IN_ETCR06,param,0x80);

      SetValue(IN_REG,param,0x03);
      SetValue(IN_BDATA04,param,1);

    for ( i=0 ; i<4 ; i++ ) {
      SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
      if ( i==0 ) SetValue(IN_BDATA03,param,0);  /* Log header ON  */
      else        SetValue(IN_BDATA03,param,1);  /* Log header OFF */

      on_test=1;
      if ( cond1==7 ) {
        if ((i==0)|(i==4)) on_test=1;
        else on_test=0; 
      }
      if ( on_test==1 ) {
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);
        if      ( count_32 ==     0 ) min_vol = -500;
        else if ( count_32 == 32768 ) min_vol = -1800;
        else                          min_vol = -1000;

        SetValue(IN_EXTAL1,param,VTH_FREQ); 
        tempo2 = 100;                                 /* VS-Step Default   */
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
        vth_mode = 12;                                /* VthRead1&2  */
        result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
        if(PASS != result){
          sprintf(outbuf," Make Vth Bunpu Error!!!\n");
          DataOut(Ffpt,outbuf);
        }
      }
    } /* for i */
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_BDATA04,param,0);
  }
/**************************************************************************************/
/***   Count For tempiref                                                           ***/
/**************************************************************************************/
  if ( cond1==8 ) {
    printf("\n");
    printf("*** Vdd/vddh Select ?   [0:OFF / 1:ON ] ->"); DigitInput(&vdd_sel);
    if ( vdd_sel == 0 ) {
    printf("*** Iref Select ?       [0:OFF / 1:ON ] ->"); DigitInput(&iref_sel);
    }
  	printf("*** Freq condition[1] Input!! (ex: 1MHz-> 1 @User /  0.4MHz-> 400 @TrimRead) ->");DigitInput(&freq01);
  	printf("*** Freq condition[2] Input!! (ex:32MHz->32 @User /, 8MHz  -> 8   @TrimRead) ->");DigitInput(&freq32);
    if ( freq01 >= 100 ) freq01*1000;
    if ( freq32 >= 100 ) freq32*1000;
    freq01ns = 1*1000/freq01;
    freq32ns = 1*1000/freq32;

  	printf("\n*** temparature? (ex:-40->40 /, 25-> 25) ->");DigitInput(&tempo18);


    if ( vdd_sel != 1 ) {
    printf("*** Min vddh=1.13V  vdd=0.70V -> 0 \n");
    printf("*** Typ vddh=1.25V  vdd=0.79V -> 1 \n");
    printf("*** Max vddh=1.45V  vdd=1.15V -> 2 \n");
    printf("*** Voltage Condition select? ->");DigitInput(&volcon);
        switch (volcon) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
    }

    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 ) {
    printf("*** iref & ireftemp Select? (etlr25<3:0> & etlr12<7:0>) ?  [0:OFF / 1:ON ] ->"); DigitInput(&ireftmp_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 && ireftmp_sel == 0 ) {
    printf("*** ireftemp only Select? (etlr12<7:0>)  [0:OFF / 1:ON ] ->"); DigitInput(&ireftmp_onsel);
//    printf("*** 10nsDelay T2(DischargeTime) Select(etlr28~etlr30<4:0>:FLP0,FLI1,FLI2 ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt2_sel);
    }

    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);
    SetValue(IN_REG,param,0x03);
    	
    etlr09 = GetValue(IN_ETLR09,param);// ireftmpI
    etlr12 = GetValue(IN_ETLR12,param);// ireftmp
    etlr25 = GetValue(IN_ETLR25,param);// iref
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

    if      ( vdd_sel  == 1 )  loop_num = 3;
    else if ( iref_sel == 1 )  loop_num = 5;
    else if ( ireftmp_sel == 1 ) {
//    		loop_num = 5;
    		loop_num = 3;
    printf("\n *** etlr12<3:0>= 0x00(0Tap)  target=2.57nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x01(1)  target=3.4nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x02(2)  target=4.23nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x03(3)  target=5.06nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x04(4)  target=5.88nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x05(5)  target=6.71nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x06(6)  target=7.54nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x07(7)  target=8.37nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x08(8)  target=9.2nA/C at 2.5uA [Target@3.5uA!!]***");
    printf("\n *** etlr12<3:0>= 0x09(9)  target=10.03nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0A(10)  target=10.86nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0B(11)  target=11.69nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0C(12)  target=12.52nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0D(13)  target=13.35nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0E(14)  target=14.18nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0F(15Tap)  target=15.01nA/C at 2.5uA***");

    SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //
  	tempo17 = GetValue(IN_ETLR09,param);// ireftemp
    printf("\n ********* Before  etlr09<7:0>= 0x%02X *********",etlr09);
    printf("\n ********* After   etlr09<7:0>= 0x%02X *********",tempo17);
    printf("\n ********* Before  etlr12<7:0>= 0x%02X *********",etlr12);
    printf("\n ********* Setting etlr12<7:0>= 0x%02X *********",GetValue(IN_ETLR12,param));

    printf("*** iref start setting!! [ex) 0x02  Tap->2,  0x05 Tap->5] ->"); DigitInput(&loop_kst);
    printf("*** iref stop  setting!! [ex) 0x0A  Tap->10, 0x0F Tap->15] ->"); DigitInput(&loop_ked);
    		loop_k = loop_ked;
            }
    else if ( ireftmp_onsel == 1 ) {
///////              loop_num = 3;
              loop_num = 15;
              SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //
              tempo17 = GetValue(IN_ETLR09,param);// ireftemp
              }
    else    loop_num = 1;

    for ( k=loop_kst ; k<=loop_k ; k++ ) {//for k 
            switch (k) {//for k 
          case 0:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x00); break;//target=2.57nA/C at 2.5uA //
          case 1:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x01); break;//target=3.4nA/C at 2.5uA //
          case 2:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x02); break;//target=4.23nA/C at 2.5uA //
          case 3:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x03); break;//target=5.06nA/C at 2.5uA //
          case 4:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x04); break;//target=5.88nA/C at 2.5uA //
          case 5:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x05); break;//target=6.71nA/C at 2.5uA //
          case 6:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x06); break;//target=7.54nA/C at 2.5uA //
          case 7:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x07); break;//target=8.37nA/C at 2.5uA //
          case 8:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x08); break;//target=9.2nA/C at 2.5uA [Target@3.5uA!!] //
          case 9:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x09); break;//target=10.03nA/C at 2.5uA //
          case 10:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0A); break;//target=10.86nA/C at 2.5uA //
          case 11:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0B); break;//target=11.69nA/C at 2.5uA //
          case 12:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0C); break;//target=12.52nA/C at 2.5uA //
          case 13:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0D); break;//target=13.35nA/C at 2.5uA //
          case 14:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0E); break;//target=14.18nA/C at 2.5uA //
          case 15:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0F); break;//target=15.01nA/C at 2.5uA //
		  default: break;//for k
		}//for k
    for ( j=0 ; j<loop_num ; j++ ) {
      if ( vdd_sel == 1 ) {
        printf("*** vdd_sel is [%d]!!!!!!!!!\n", vdd_sel );
        printf("*** loop_num is [%d]@@@@@@@@@\n", loop_num );
        printf("*** @@@@@@@@@ switch j is [%d][%d][%d][%d][%d][%d][%d][%d][%d]\n", j );

        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
	  }
      if ( iref_sel == 1 ) {
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFB); break;  /* Min Iref=1.5uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xFC); break;  /* Min Iref=3.0uA */

		  default: break;
		}
	  }
      else if ( ireftmp_sel == 1 ) {
      if ( j == 0 && k == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?   ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo11);
          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Precharge time*/
          printf("*** T2 discharge setting!! ETLR28<7:0>,ETLR29<7:0>,ETLR30<7:0> Select ?  ex) 0xF0 input ->"); DigitInput(&tempo13);
          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);  /* 10ns Discharge time*/
          	}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFB); break;  /* Min Iref=1.5uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xFC); break;  /* Min Iref=3.0uA */
		  default: break;
		}
        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
	  }
      else if ( ireftmp_onsel == 1 ) {
      if ( j == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?  ex) 0xEF input ->"); DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo11);
          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Precharge time*/
          printf("*** T2 discharge setting!! ETLR28<7:0>,ETLR29<7:0>,ETLR30<7:0> Select ?  ex) 0xF0 input ->"); DigitInput(&tempo13);
          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);  /* 10ns Discharge time*/
          	}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x00); break;//target=2.57nA/C at 2.5uA //
          case 1:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x01); break;//target=3.4nA/C at 2.5uA //
          case 2:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x02); break;//target=4.23nA/C at 2.5uA //
          case 3:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x03); break;//target=5.06nA/C at 2.5uA //
          case 4:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x04); break;//target=5.88nA/C at 2.5uA //
          case 5:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x05); break;//target=6.71nA/C at 2.5uA //
          case 6:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x06); break;//target=7.54nA/C at 2.5uA //
          case 7:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x07); break;//target=8.37nA/C at 2.5uA //
          case 8:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x08); break;//target=9.2nA/C at 2.5uA [Target@3.5uA!!] //
          case 9:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x09); break;//target=10.03nA/C at 2.5uA //
          case 10:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0A); break;//target=10.86nA/C at 2.5uA //
          case 11:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0B); break;//target=11.69nA/C at 2.5uA //
          case 12:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0C); break;//target=12.52nA/C at 2.5uA //
          case 13:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0D); break;//target=13.35nA/C at 2.5uA //
          case 14:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0E); break;//target=14.18nA/C at 2.5uA //
          case 15:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0F); break;//target=15.01nA/C at 2.5uA //
		  default: break;
		}
	  }

      else if (vdd_sel == 0 && iref_sel == 0 && ireftmp_sel == 0 && ireftmp_onsel == 0) {
 printf("***** DEBUG15 *****\n"); 
        vs[1] = vs[2] = 500;
		SetValue(IN_ETLR25,param,0xFF);
		SetValue(IN_ETLR33,param,0xFF);
		SetValue(IN_ETLR34,param,0xFF);
		SetValue(IN_ETLR32,param,0xFF);
	  }

    printf("MONITOR imoni(Flash) notemp\n");
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    

    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);
    iref_moni = selvol;

    printf("MONITOR imoni(Flash) tempiref\n");
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    

    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);
    iref_monitmp = selvol;


    for ( i=0 ; i<25 ; i++ ) {
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000); /* 4KB  */
//      SetValue(IN_EXTAL1,param,1000);        /* 1MHz */
//      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
//        count_1 = GetValue(OUT_LDATA0,param);
		  count_1 = 0;
//      SetValue(IN_EXTAL1,param,31);        /* 32MHz */
      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_32 = GetValue(OUT_LDATA0,param);

      if      ( vdd_sel  == 1 ) sprintf(file_name,"%s%s%s_Vdd_%dC_%d.csv", glob_cdp, glob_datalog, item_name,tempo18, get_site_number());
      else if ( iref_sel == 1 ) sprintf(file_name,"%s%s%s_Iref_%dC_%d.csv", glob_cdp, glob_datalog, item_name,tempo18, get_site_number());
      else if ( ireftmp_sel == 1 ) sprintf(file_name,"%s%s%s_ireftmpIdep_%dC_tmpI0x%d_%d.csv", glob_cdp, glob_datalog, item_name,tempo18, k, get_site_number());
      else if ( ireftmp_onsel == 1 ) sprintf(file_name,"%s%s%s_ireftmp_%dC_%d.csv", glob_cdp, glob_datalog, item_name,tempo18, get_site_number());
      else                      sprintf(file_name,"%s%s%s_%dC_%d.csv", glob_cdp, glob_datalog, item_name,tempo18, get_site_number());
      if( NULL==(file_count = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }

      if ( i == 0 ) {
        printf("*** TOPADDR0 is %d!!!!!!!!!\n",GetValue(IN_TOPADDR0,param));
        printf("*** TESTSIZE0 is %d!!!!!!!!!\n",GetValue(IN_TESTSIZE0,param));
        fprintf(file_count,"TOPADDR0 =%d\n",GetValue(IN_TOPADDR0 ,param));
        fprintf(file_count,"TESTSIZE0=%d\n",GetValue(IN_TESTSIZE0,param));/* 4KB  */
        fprintf(file_count,"Vcc=%d,vddh=%d,vdd=%d,Iref=%d,Ireftmp=%d,ETLR09=%2X,ETLR12=%2X,ETLR25=%2X,ETLR26=%2X,ETLR27=%2X,ETLR28=%2X,ETLR29=%2X,ETLR30=%2X,ETLR32=%2X,ETLR33=%2X,ETLR34=%2X\n"
	                   ,vs[0],vs[1],vs[2],iref_moni,iref_monitmp,GetValue(IN_ETLR09,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param)
			   ,GetValue(IN_ETLR29,param),GetValue(IN_ETLR30,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param)); 
       if ( freq01 >= 100 ) fprintf(file_count,"No,%dK,%dM\n",freq01,freq32);
       if ( freq32 >= 100 ) fprintf(file_count,"No,%dM,%dK\n",freq01,freq32);
       if ( freq01 >= 100 && freq32 >= 100 ) fprintf(file_count,"No,%dK,%dK\n",freq01,freq32);
        fprintf(file_count,"No,%dM,%dM\n",freq01,freq32);
      }
	  fprintf(file_count,"No_%d,%d,%d\n",i,count_1,count_32);
      if ( i == 24 ) fprintf(file_count,"\n");
	  fclose( file_count);

	} /* for i */ 
	} /* for j */ 
	} /* for k */
     SetValue(IN_ETLR09,param,etlr09);
     SetValue(IN_ETLR12,param,etlr12);
     SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);
  }
/////////////////////////////////////////////////////////
/**************************************************************************************/
/***   Count For tempiref monitor                                                           ***/
/**************************************************************************************/
  if ( cond1==9 ) {
    printf("\n");
    printf("*** temparature??  [-40C :40 / 25C:25 / 95C:95 ] ->"); DigitInput(&evaltmp);
    printf("*** Vdd/vddh Select ?   [0:OFF / 1:ON ] ->"); DigitInput(&vdd_sel);
    if ( vdd_sel == 0 ) {
    printf("*** Iref Select ?       [0:OFF / 1:ON ] ->"); DigitInput(&iref_sel);
    }
  	printf("*** Freq condition[1] Input!! (ex: 1MHz-> 1 @User /  0.4MHz-> 400 @TrimRead) ->\n");//DigitInput(&freq01);
  	printf("*** Freq condition[2] Input!! (ex:32MHz->32 @User /, 8MHz  -> 8   @TrimRead) ->\n");//DigitInput(&freq32);
    freq01 = 1;
    freq32 = 32;
    if ( freq01 >= 100 ) freq01*1000;
    if ( freq32 >= 100 ) freq32*1000;
    freq01ns = 1*1000/freq01;
    freq32ns = 1*1000/freq32;

    if ( vdd_sel != 1 ) {
    printf("*** Min vddh=1.13V  vdd=0.70V -> 0 \n");
    printf("*** Typ vddh=1.25V  vdd=0.79V -> 1 \n");
    printf("*** Max vddh=1.45V  vdd=1.15V -> 2 \n");
    printf("*** Voltage Condition select? ->");DigitInput(&volcon);
        switch (volcon) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
    }

    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 ) {
    printf("*** iref & ireftemp Select? (etlr25<3:0> & etlr12<7:0>) ?  [0:OFF / 1:ON ] ->"); DigitInput(&ireftmp_sel);
    }
    if ( vdd_sel == 0 && iref_sel == 0 && delayt4_sel == 0 && delayt6_sel == 0 && delayt5_sel == 0 && delayt3_sel == 0 && ireftmp_sel == 0 ) {
    printf("*** ireftemp only Select? (etlr12<7:0>)  [0:OFF / 1:ON ] ->"); DigitInput(&ireftmp_onsel);
    printf("*** 10nsDelay T2(DischargeTime) Select(etlr28~etlr30<4:0>:FLP0,FLI1,FLI2 ?   [0:OFF / 1:ON ] ->"); DigitInput(&delayt2_sel);
    }

    SetValue(IN_AREA,param,0);
    SetValue(IN_TOPADDR1 ,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2 ,param,0);
    SetValue(IN_TESTSIZE2,param,0);
    SetValue(IN_REG,param,0x03);
    	
    etlr09 = GetValue(IN_ETLR09,param);// ireftmpI
    etlr12 = GetValue(IN_ETLR12,param);// ireftmp
    etlr25 = GetValue(IN_ETLR25,param);// iref
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

    if      ( vdd_sel  == 1 )  loop_num = 3;
    else if ( iref_sel == 1 )  loop_num = 5;
    else if ( ireftmp_sel == 1 ) {
//    		loop_num = 5;
    		loop_num = 2;
    printf("\n *** etlr12<3:0>= 0x00(0Tap)  target=2.57nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x01(1)  target=3.4nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x02(2)  target=4.23nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x03(3)  target=5.06nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x04(4)  target=5.88nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x05(5)  target=6.71nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x06(6)  target=7.54nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x07(7)  target=8.37nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x08(8)  target=9.2nA/C at 2.5uA [Target@3.5uA!!]***");
    printf("\n *** etlr12<3:0>= 0x09(9)  target=10.03nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0A(10)  target=10.86nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0B(11)  target=11.69nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0C(12)  target=12.52nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0D(13)  target=13.35nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0E(14)  target=14.18nA/C at 2.5uA***");
    printf("\n *** etlr12<3:0>= 0x0F(15Tap)  target=15.01nA/C at 2.5uA***");

    SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //
  	tempo17 = GetValue(IN_ETLR09,param);// ireftemp
    printf("\n ********* Before  etlr09<7:0>= 0x%02X *********",etlr09);
    printf("\n ********* After   etlr09<7:0>= 0x%02X *********",tempo17);
    printf("\n ********* Before  etlr12<7:0>= 0x%02X *********",etlr12);
    printf("\n ********* Setting etlr12<7:0>= 0x%02X *********",GetValue(IN_ETLR12,param));

    printf("*** iref start setting!! [ex) 0x02  Tap->2,  0x05 Tap->5] ->"); DigitInput(&loop_kst);
    printf("*** iref stop  setting!! [ex) 0x0A  Tap->10, 0x0F Tap->15] ->"); DigitInput(&loop_ked);
    		loop_k = loop_ked;
            }
    else if ( ireftmp_onsel == 1 ) {
///////              loop_num = 3;
              loop_num = 15;
              SetValue(IN_ETLR09,param,(etlr09 & 0x7F));  // ireftemp On etlr09<7>=0 //
              tempo17 = GetValue(IN_ETLR09,param);// ireftemp
              }
    else    loop_num = 1;

    for ( k=loop_kst ; k<=loop_k ; k++ ) {//for k 
            switch (k) {//for k 
          case 0:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x00); break;//target=2.57nA/C at 2.5uA //
          case 1:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x01); break;//target=3.4nA/C at 2.5uA //
          case 2:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x02); break;//target=4.23nA/C at 2.5uA //
          case 3:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x03); break;//target=5.06nA/C at 2.5uA //
          case 4:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x04); break;//target=5.88nA/C at 2.5uA //
          case 5:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x05); break;//target=6.71nA/C at 2.5uA //
          case 6:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x06); break;//target=7.54nA/C at 2.5uA //
          case 7:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x07); break;//target=8.37nA/C at 2.5uA //
          case 8:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x08); break;//target=9.2nA/C at 2.5uA [Target@3.5uA!!] //
          case 9:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x09); break;//target=10.03nA/C at 2.5uA //
          case 10:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0A); break;//target=10.86nA/C at 2.5uA //
          case 11:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0B); break;//target=11.69nA/C at 2.5uA //
          case 12:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0C); break;//target=12.52nA/C at 2.5uA //
          case 13:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0D); break;//target=13.35nA/C at 2.5uA //
          case 14:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0E); break;//target=14.18nA/C at 2.5uA //
          case 15:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0F); break;//target=15.01nA/C at 2.5uA //
		  default: break;//for k
		}//for k
    for ( j=0 ; j<loop_num ; j++ ) {
      if ( vdd_sel == 1 ) {
        printf("*** vdd_sel is [%d]!!!!!!!!!\n", vdd_sel );
        printf("*** loop_num is [%d]@@@@@@@@@\n", loop_num );
        printf("*** @@@@@@@@@ switch j is [%d][%d][%d][%d][%d][%d][%d][%d][%d]\n", j );

        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
          case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
          case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
		}
	  }
      if ( iref_sel == 1 ) {
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
          case 1:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;  /* Min Iref=3.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;  /* Min Iref=5.0uA */
		  default: break;
		}
	  }
      else if ( ireftmp_sel == 1 ) {
      if ( j == 0 && k == 0 ) {
      	if ( delaydef != 0 ) {
          printf("*** T4 setting!! ETLR33 Select ?   ex) 0xEF input ->"); tempo7=0x11;//DigitInput(&tempo7);
          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); tempo8=0x11;//DigitInput(&tempo8);
          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); tempo9=0x11;//DigitInput(&tempo9);
          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); tempo11=0x22;//DigitInput(&tempo11);
          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Precharge time*/
          printf("*** T2 discharge setting!! ETLR28<7:0>,ETLR29<7:0>,ETLR30<7:0> Select ?  ex) 0xF0 input ->");tempo13=0xF1;// DigitInput(&tempo13);
          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);  /* 10ns Discharge time*/
          	}
          }
        switch (j) {
          case 0:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
          case 1:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 0:  SetValue(IN_ETLR25,param,0xF8); break;  /* Min Iref=1.0uA */
//          case 1:  SetValue(IN_ETLR25,param,0xFA); break;  /* Min Iref=2.0uA */
//          case 2:  SetValue(IN_ETLR25,param,0xFD); break;  /* Min Iref=2.5uA */
//          case 3:  SetValue(IN_ETLR25,param,0xFF); break;  /* Min Iref=3.5uA */
//          case 4:  SetValue(IN_ETLR25,param,0xF0); break;  /* Min Iref=5.0uA */
		  default: break;
		}
	  }
//      else if ( ireftmp_onsel == 1 ) {
//      if ( j == 0 ) {
//      	if ( delaydef != 0 ) {
//          printf("*** T4 setting!! ETLR33 Select ?  ex) 0xEF input ->"); DigitInput(&tempo7);
//          SetValue(IN_ETLR33,param,tempo7);  /* 2ns Delay */
//          printf("*** T6 setting!! ETLR34 Select ?  ex) 0xEF input ->"); DigitInput(&tempo8);
//          SetValue(IN_ETLR34,param,tempo8);  /* 2ns Delay */
//          printf("*** T5/T3 setting!! ETLR32<7:4>/ETLR32<3:0> Select ?  ex) 0xEF input ->"); DigitInput(&tempo9);
//          SetValue(IN_ETLR32,param,tempo9);  /* 2ns Delay */
//          printf("*** T1 setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo11);
//          SetValue(IN_ETLR26,param,tempo11),SetValue(IN_ETLR27,param,tempo11 & 0x0F);  /* 8ns Precharge time*/
//          printf("*** T2 discharge setting!! ETLR28<7:0>,ETLR29<7:0>,ETLR30<7:0> Select ?  ex) 0xF0 input ->"); DigitInput(&tempo13);
//          SetValue(IN_ETLR28,param,tempo13),SetValue(IN_ETLR29,param,tempo13),SetValue(IN_ETLR30,param,tempo13);  /* 10ns Discharge time*/
//          	}
//          }
//        switch (j) {
//          case 0:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x00); break;//target=2.57nA/C at 2.5uA //
//          case 1:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x01); break;//target=3.4nA/C at 2.5uA //
//          case 2:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x02); break;//target=4.23nA/C at 2.5uA //
//          case 3:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x03); break;//target=5.06nA/C at 2.5uA //
//          case 4:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x04); break;//target=5.88nA/C at 2.5uA //
//          case 5:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x05); break;//target=6.71nA/C at 2.5uA //
//          case 6:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x06); break;//target=7.54nA/C at 2.5uA //
//          case 7:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x07); break;//target=8.37nA/C at 2.5uA //
//          case 8:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x08); break;//target=9.2nA/C at 2.5uA [Target@3.5uA!!] //
//          case 9:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x09); break;//target=10.03nA/C at 2.5uA //
//          case 10:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0A); break;//target=10.86nA/C at 2.5uA //
//          case 11:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0B); break;//target=11.69nA/C at 2.5uA //
//          case 12:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0C); break;//target=12.52nA/C at 2.5uA //
//          case 13:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0D); break;//target=13.35nA/C at 2.5uA //
//          case 14:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0E); break;//target=14.18nA/C at 2.5uA //
//          case 15:  SetValue(IN_ETLR12,param,etlr12 & 0xF0 | 0x0F); break;//target=15.01nA/C at 2.5uA //
//		  default: break;
//		}
//	  }

      else if (vdd_sel == 0 && iref_sel == 0 && ireftmp_sel == 0 && ireftmp_onsel == 0) {
 printf("***** DEBUG15 *****\n"); 
        vs[1] = vs[2] = 500;
		SetValue(IN_ETLR25,param,0xFF);
		SetValue(IN_ETLR33,param,0xFF);
		SetValue(IN_ETLR34,param,0xFF);
		SetValue(IN_ETLR32,param,0xFF);
	  }

    printf("MONITOR imoni(Flash) notemp\n");
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    

    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);
    iref_moni = selvol;

    printf("MONITOR imoni(Flash) tempiref\n");
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    

    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);
    iref_monitmp = selvol;


//fori    for ( i=0 ; i<25 ; i++ ) {
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
    printf("***** i== %d *****\n",i); 
      SetValue(IN_TESTSEL,param,0);
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_TOPADDR0 ,param,FLP0_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000); /* 4KB  */
//      SetValue(IN_EXTAL1,param,1000);        /* 1MHz */
      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_1 = GetValue(OUT_LDATA0,param);
//      SetValue(IN_EXTAL1,param,31);        /* 32MHz */
      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_32 = GetValue(OUT_LDATA0,param);

      if      ( vdd_sel  == 1 ) sprintf(file_name,"%s%s%s_Vdd_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else if ( iref_sel == 1 ) sprintf(file_name,"%s%s%s_Iref_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
//      else if ( ireftmp_sel == 1 ) sprintf(file_name,"%s%s%s_ireftmpIdep_%d_tmpI0x%d.csv", glob_cdp, glob_datalog, item_name, get_site_number(), k);
      else if ( ireftmp_sel == 1 ) sprintf(file_name,"%s%s%s_ireftmpIdep_%d_%dC_tmpI0xXX.csv", glob_cdp, glob_datalog, item_name,evaltmp, get_site_number());
      else if ( ireftmp_onsel == 1 ) sprintf(file_name,"%s%s%s_ireftmp_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else                      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      if( NULL==(file_count = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }

      if ( i == 0 ) {
        printf("*** TOPADDR0 is %d!!!!!!!!!\n",GetValue(IN_TOPADDR0,param));
        printf("*** TESTSIZE0 is %d!!!!!!!!!\n",GetValue(IN_TESTSIZE0,param));
//////        fprintf(file_count,"TOPADDR0 =%d\n",GetValue(IN_TOPADDR0 ,param));
//////        fprintf(file_count,"TESTSIZE0=%d\n",GetValue(IN_TESTSIZE0,param));/* 4KB  */
        fprintf(file_count,"Vcc=%d,vddh=%d,vdd=%d,Iref=%d,Ireftmp=%d,ETLR09=%2X,ETLR12=%2X,ETLR25=%2X,ETLR26=%2X,ETLR27=%2X,ETLR28=%2X,ETLR29=%2X,ETLR30=%2X,ETLR32=%2X,ETLR33=%2X,ETLR34=%2X\n"
	                   ,vs[0],vs[1],vs[2],iref_moni,iref_monitmp,GetValue(IN_ETLR09,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR27,param),GetValue(IN_ETLR28,param)
			   ,GetValue(IN_ETLR29,param),GetValue(IN_ETLR30,param),GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param)); 
//////       if ( freq01 >= 100 ) fprintf(file_count,"No,%dK,%dM\n",freq01,freq32);
//////       if ( freq32 >= 100 ) fprintf(file_count,"No,%dM,%dK\n",freq01,freq32);
//////       if ( freq01 >= 100 && freq32 >= 100 ) fprintf(file_count,"No,%dK,%dK\n",freq01,freq32);
//////        fprintf(file_count,"No,%dM,%dM\n",freq01,freq32);
      }
//////	  fprintf(file_count,"No_%d,%d,%d\n",i,count_1,count_32);
      if ( i == 24 ) fprintf(file_count,"\n");
	  fclose( file_count);

//fori	} /* for i */ 
	} /* for j */ 
	} /* for k */
     SetValue(IN_ETLR09,param,etlr09);
     SetValue(IN_ETLR12,param,etlr12);
     SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);
  }



/**************************************************************************************/
/***   Count@Extra6                                                                        ***/
/**************************************************************************************/
  if ( cond1==10 ) {
    printf("\n");

    etlr16 = GetValue(IN_ETLR16,param);
    etlr20 = GetValue(IN_ETLR20,param);
    etlr37 = GetValue(IN_ETLR37,param);
//    SetValue(IN_ETCR05,param,0x40);
    SetValue(IN_ETCR05,param,0x00);
    SetValue(IN_ETCR06,param,0x80);
//    SetValue(IN_ETCR05,param,0x40);
//    SetValue(IN_ETCR06,param,0x00);
    
  	etlr09 = GetValue(IN_ETLR09,param);// ireftemp
  	etlr12 = GetValue(IN_ETLR12,param);// ireftemp
    printf("\n ********* Before  etlr12<7:0>= 0x%02X *********",etlr12);
    printf("\n ********* Setting etlr12<7:0>= 0x%02X *********",GetValue(IN_ETLR12,param));

  	printf("\n");
    printf("*** Vdd/vddh Select ?   [0:OFF / 1:ON ] ->");vdd_sel=1;// DigitInput(&vdd_sel);

  	printf("*** Freq condition[1] Input!! (ex: 0.5MHz-> 500 @TrimRead) ->");freq01=500;//DigitInput(&freq01);
  	printf("*** Freq condition[2] Input!! (ex: 4MHz  -> 4   @TrimRead) ->");freq32=4;//DigitInput(&freq32);
    if ( freq01 >= 100 ) freq01ns = 1*1000000/freq01;
    if ( freq32 >= 100 ) freq32ns = 1*1000000/freq32;
    if ( freq01 < 100  ) freq01ns = 1*1000/freq01;
    if ( freq32 < 100  ) freq32ns = 1*1000/freq32;


  if ( vdd_sel != 1 ) {
    printf("*** Min vddh=1.13V  vdd=0.70V -> 0 \n");
    printf("*** Typ vddh=1.25V  vdd=0.79V -> 1 \n");
    printf("*** Max vddh=1.45V  vdd=1.15V -> 2 \n");

  	printf("*** Voltage Condition select? ->");DigitInput(&volcon);
        switch (volcon) {
        	case 0:  vs[1] = 452; vs[2] = 443; break;	    /* Min vddh=1.13V(vs[1])  vdd=0.70V(vs[2]) */
            case 1:  vs[1] = 500; vs[2] = 500; break;	    /* Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) */
        	case 2:  vs[1] = 580; vs[2] = 728; break;	    /* Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) */
		  default: break;
        }
    }

    SetValue(IN_AREA,param,6); //Extra6
    SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
    SetValue(IN_TESTSIZE0,param,0x4000);/* 16KB */
    SetValue(IN_TOPADDR1,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    SetValue(IN_REG,param,0x03);
    SetValue(IN_TESTSEL,param,0);

    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

    if      ( vdd_sel  == 1 )  loop_num = 3;
    else    loop_num = 1;

    for ( j=0 ; j<loop_num ; j++ ) {
      printf("***** jj== %d *****\n",j); 
      printf("***** jj== %d *****\n",j); 
      printf("***** jj== %d *****\n",j); 
      printf("***** loop_num== %d *****\n",loop_num);
      printf("***** loop_num== %d *****\n",loop_num); 
      printf("***** loop_num== %d *****\n",loop_num); 
      if ( vdd_sel == 1 ) {
        printf("*** vdd_sel is [%d]!!!!!!!!!\n", vdd_sel );
        printf("*** loop_num is [%d]@@@@@@@@@\n", loop_num );
        printf("*** @@@@@@@@@ switch j is [%d][%d][%d][%d][%d][%d][%d][%d][%d]\n", j );

        switch (j) {
          case 0:  vs[1] = 452; vs[2] = 456; break;	    // Min vddh=1.13V(vs[1])  vdd=0.72V(vs[2]) //
          case 1:  vs[1] = 500; vs[2] = 500; break;	    // Typ vddh=1.25V(vs[1])  vdd=0.79V(vs[2]) //
          case 2:  vs[1] = 580; vs[2] = 728; break;	    // Max vddh=1.45V(vs[1])  vdd=1.15V(vs[2]) //
		  default: break;
		}
	  }

      else if (vdd_sel == 0 ) {
        vs[1] = vs[2] = 500;
	  }

    printf("MONITOR imoni(Flash) notemp\n");
//    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);    
    SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);    
    SetValue(IN_BDATA15,param,UA25);
    CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    if( selvol>=24000 ){
      SetValue(IN_BDATA15,param,UA250);
      CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
    }
    SetValue(IN_TESTSEL,param,0);
    SetValue(IN_BDATA15,param,0);


    for ( i=0 ; i<25 ; i++ ) {
      printf("***** i== %d *****\n",i); 
      printf("***** i== %d *****\n",i); 
      printf("***** i== %d *****\n",i); 

      count_1 = 0;
      count_32 = 0;

    if ( i == 0 || i == 1 || i == 2 || i == 3 ) { //i=1~3
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(i*0x1000));
        SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
//hashi        SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP);
//hashi        SetValue(IN_TESTSIZE0,param,0x4000);               /* 16KB */
      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
        count_1 = GetValue(OUT_LDATA0,param);

      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);

//        SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(i*0x1000));
//        SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
//      SetValue(IN_EXTAL1,param,freq01ns);        /* 1MHz */
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
//        count_1 = GetValue(OUT_LDATA0,param);
//      SetValue(IN_EXTAL1,param,freq32ns);        /* 32MHz */
//      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
//        count_32 = GetValue(OUT_LDATA0,param);
    }//i=1~3

      if      ( vdd_sel  == 1 ) sprintf(file_name,"%s%s%s_Vdd_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      else                      sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, item_name, get_site_number());
      if( NULL==(file_count = fopen( file_name, "at" )) ){
        printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
        break;
      }
    SetValue(IN_ETCR05,param,0x40);
    SetValue(IN_ETCR06,param,0x80);

      if ( i == 0 ) {
        printf("*** TOPADDR0 is %d!!!!!!!!!\n",GetValue(IN_TOPADDR0,param));
        printf("*** TESTSIZE0 is %d!!!!!!!!!\n",GetValue(IN_TESTSIZE0,param));
        fprintf(file_count,"TOPADDR0 =%d\n",GetValue(IN_TOPADDR0 ,param));
        fprintf(file_count,"TESTSIZE0=%d\n",GetValue(IN_TESTSIZE0,param));/* 4KB  */
        fprintf(file_count,"Vcc=%d,vddh=%d,vdd=%d,Iref=%d,ETCR05=%2X,ETCR06=%2X,ETLR09=%2X,ETLR12=%2X,ETLR25=%2X,ETLR26=%2X,ETLR28=%2X,ETLR32=%2X,ETLR33=%2X,ETLR34=%2X\n"
	                   ,vs[0],vs[1],vs[2],selvol,GetValue(IN_ETCR05,param),GetValue(IN_ETCR06,param),GetValue(IN_ETLR09,param),GetValue(IN_ETLR12,param),GetValue(IN_ETLR25,param),GetValue(IN_ETLR26,param),GetValue(IN_ETLR28,param)
			   ,GetValue(IN_ETLR32,param),GetValue(IN_ETLR33,param),GetValue(IN_ETLR34,param)); 
       if ( freq01 >= 100 && freq32 < 100 ) fprintf(file_count,"No,%dK,%dM\n",freq01,freq32);
       if ( freq01 <  100 && freq32 >= 100  ) fprintf(file_count,"No,%dM,%dK\n",freq01,freq32);
       if ( freq01 >= 100 && freq32 >= 100 ) fprintf(file_count,"No,%dK,%dK\n",freq01,freq32);
       if ( freq01 < 100 && freq32 < 100 )  fprintf(file_count,"No,%dM,%dM\n",freq01,freq32);
      }
	  fprintf(file_count,"No_%d,%d,%d\n",i,count_1,count_32);
      if ( i == 24 ) fprintf(file_count,"\n");
	  fclose( file_count);

	} /* for i */ 
	} /* for j */ 
    SetValue(IN_ETLR09,param,etlr09);
    SetValue(IN_ETLR12,param,etlr12);
    SetValue(IN_ETLR25,param,etlr25);
    SetValue(IN_ETLR32,param,etlr32);
    SetValue(IN_ETLR33,param,etlr33);
    SetValue(IN_ETLR34,param,etlr34);
  }


/**************************************************************************************/
/***   Distribution@Extra iref=2.5uA VthRead NoGiji_TrimRead                                                                 ***/
/**************************************************************************************/
  if ( cond1==11 ) {
    SetValue(IN_AREA,param,6); //Extra6
    SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
    SetValue(IN_TESTSIZE0,param,EXTRAB_SIZE);
    SetValue(IN_TOPADDR1,param,0);
    SetValue(IN_TESTSIZE1,param,0);
    SetValue(IN_TOPADDR2,param,0);
    SetValue(IN_TESTSIZE2,param,0);

    SetValue(IN_ETCR05,param,0x40);
    SetValue(IN_ETCR06,param,0x00);

      SetValue(IN_REG,param,0x03);
      SetValue(IN_BDATA04,param,1);

    for ( i=0 ; i<4 ; i++ ) {
      SetValue(IN_TOPADDR0 ,param,EXTRAB_TOP+(i*0x1000));
      SetValue(IN_TESTSIZE0,param,0x1000);               /* 4KB */
      if ( i==0 ) SetValue(IN_BDATA03,param,0);  /* Log header ON  */
      else        SetValue(IN_BDATA03,param,1);  /* Log header OFF */

      on_test=1;
      if ( on_test==1 ) {
        SetValue(IN_EXTAL1,param,31);       /* 32MHz */
        CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readcount_code_at"),"CPU Vth-READ ALL");  /*ファーム次第で変更する可能性あり*/
    	count_32 = GetValue(OUT_LDATA0,param);
        if      ( count_32 ==     0 ) min_vol = -500;
        else if ( count_32 == 32768 ) min_vol = -1800;
        else                          min_vol = -1000;

        SetValue(IN_EXTAL1,param,VTH_FREQ); 
        tempo2 = 100;                                 /* VS-Step Default   */
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
        vth_mode = 12;                                /* VthRead1&2  */
        result = VthReadDist(vth_mode, bit_count, min_vol, tempo2, &vs[0],W1SEC*waittime, param, item_name);
        if(PASS != result){
          sprintf(outbuf," Make Vth Bunpu Error!!!\n");
          DataOut(Ffpt,outbuf);
        }
      }
    } /* for i */
  SetValue(IN_BDATA03,param,0);
  SetValue(IN_BDATA04,param,0);
  }

/////////////////////////////////////////////////////////
  /*  Common Clear   */
  SetValue(IN_AREA,param,stack0);
  SetValue(IN_TOPADDR0 ,param,stack1);
  SetValue(IN_TESTSIZE0,param,stack2);
  SetValue(IN_TOPADDR1 ,param,stack3);
  SetValue(IN_TESTSIZE1,param,stack4);
  SetValue(IN_TOPADDR2 ,param,stack5);
  SetValue(IN_TESTSIZE2,param,stack6);
  SetValue(IN_EXTAL1,param,stack7);
  vs[0] = stack8;
  vs[1] = stack9;
  vs[2] = stack10;
  SetValue(IN_REG,param,stack11);
  SetValue(IN_BDATA01,param,0);
  SetValue(IN_TESTSEL,param,0);
  /*********************/

}  

