/******************************************************************************/
/*!
  @file RC04EXslc_Fken.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int F_Ken(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9;
  int result,selvol,pulse,search_loss,search_gain,etlr_flag;
  int i,j,k,l,m,n,cond0,cond1,cond2,cond3,cond4,mat,loop;
  int info[32],etlr[39];           /* wmat chip information              */
  char file_name[128];             /*                                    */
  char sample_name[128];           /*                                    */
  char test_temp[128];             /*                                    */
  FILE_T *file_log;

  cond1 = cond2 = cond3 = cond4 = 0;
  result = 0;
 
  time( &time_list );// time_list += V4V5K_TIME_OFS;

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
      stack9 = vs[0];

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
/***   Register Check                                                               ***/
/**************************************************************************************/
  SetValue(IN_TESTSEL,param,0);  // VthRead count mode
  SetValue(IN_BDATA00,param,0);
  SetValue(IN_AREA,param,5);	 /* Extra5 */
  SetValue(IN_TOPADDR0,param,0);
  SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
  
  for ( i=8 ; i<40 ; i++ ) {
    etlr[i] = ReadEcrMode8(FBM_READTOP+(0x0050+i),SIZE_B);
  }

  etlr_flag=0;
  if ( etlr[8]  != GetValue(OUT_ETLR08,param) ) etlr_flag = 0x1;  
  if ( etlr[9]  != GetValue(OUT_ETLR09,param) ) etlr_flag = etlr_flag + 0x2;  
  if ( etlr[10] != GetValue(OUT_ETLR10,param) ) etlr_flag = etlr_flag + 0x4;  
  if ( etlr[11] != GetValue(OUT_ETLR11,param) ) etlr_flag = etlr_flag + 0x8;  
  if ( etlr[12] != GetValue(OUT_ETLR12,param) ) etlr_flag = etlr_flag + 0x10;  
  if ( etlr[13] != GetValue(OUT_ETLR13,param) ) etlr_flag = etlr_flag + 0x20;  
  if ( etlr[14] != GetValue(OUT_ETLR14,param) ) etlr_flag = etlr_flag + 0x40;  
  if ( etlr[15] != GetValue(OUT_ETLR15,param) ) etlr_flag = etlr_flag + 0x80;  
  if ( etlr[16] != GetValue(OUT_ETLR16,param) ) etlr_flag = etlr_flag + 0x100;  
  if ( etlr[17] != GetValue(OUT_ETLR17,param) ) etlr_flag = etlr_flag + 0x200;  
//  if ( etlr[18] != GetValue(OUT_ETLR18,param) ) etlr_flag = etlr_flag + 0x400;  
//  if ( etlr[19] != GetValue(OUT_ETLR19,param) ) etlr_flag = etlr_flag + 0x800;  
  if ( etlr[20] != GetValue(OUT_ETLR20,param) ) etlr_flag = etlr_flag + 0x1000;  
//  if ( etlr[21] != GetValue(OUT_ETLR21,param) ) etlr_flag = etlr_flag + 0x2000;  
  if ( etlr[22] != GetValue(OUT_ETLR22,param) ) etlr_flag = etlr_flag + 0x4000;  
  if ( etlr[23] != GetValue(OUT_ETLR23,param) ) etlr_flag = etlr_flag + 0x8000;  
  if ( etlr[24] != GetValue(OUT_ETLR24,param) ) etlr_flag = etlr_flag + 0x10000;  
  if ( etlr[25] != GetValue(OUT_ETLR25,param) ) etlr_flag = etlr_flag + 0x20000;  
  if ( etlr[26] != GetValue(OUT_ETLR26,param) ) etlr_flag = etlr_flag + 0x40000;  
  if ( etlr[27] != GetValue(OUT_ETLR27,param) ) etlr_flag = etlr_flag + 0x80000;  
  if ( etlr[28] != GetValue(OUT_ETLR28,param) ) etlr_flag = etlr_flag + 0x100000;  
  if ( etlr[29] != GetValue(OUT_ETLR29,param) ) etlr_flag = etlr_flag + 0x200000;  
  if ( etlr[30] != GetValue(OUT_ETLR30,param) ) etlr_flag = etlr_flag + 0x400000;  
  if ( etlr[31] != GetValue(OUT_ETLR31,param) ) etlr_flag = etlr_flag + 0x800000;  
  if ( etlr[32] != GetValue(OUT_ETLR32,param) ) etlr_flag = etlr_flag + 0x1000000;  
  if ( etlr[33] != GetValue(OUT_ETLR33,param) ) etlr_flag = etlr_flag + 0x2000000;  
  if ( etlr[34] != GetValue(OUT_ETLR34,param) ) etlr_flag = etlr_flag + 0x4000000;  
  if ( etlr[35] != GetValue(OUT_ETLR35,param) ) etlr_flag = etlr_flag + 0x8000000;  
  if ( etlr[36] != GetValue(OUT_ETLR36,param) ) etlr_flag = etlr_flag + 0x1000000;  
  if ( etlr[37] != GetValue(OUT_ETLR37,param) ) etlr_flag = etlr_flag + 0x2000000;  
  if ( etlr[38] != GetValue(OUT_ETLR38,param) ) etlr_flag = etlr_flag + 0x4000000;  
  if ( etlr[39] != GetValue(OUT_ETLR39,param) ) etlr_flag = etlr_flag + 0x8000000;  

  if ( etlr_flag != 0 ){
    printf("\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!!  Register Fail  !!!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("Fail code:%X",etlr_flag);
    printf("\n");
    printf("\n");
  }
  if ( etlr_flag != 0 ) return(PASS);

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
//  SetValue(IN_TESTSEL,param,0);  // VthRead count mode
//  SetValue(IN_BDATA00,param,0);
//  SetValue(IN_AREA,param,5);	 /* Extra5 */
//  SetValue(IN_TOPADDR0,param,0);
//  SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
//  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
  info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B); info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
  info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B); info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
  info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B); info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
  info[10] = ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
  info[11] = ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B); info[12] = ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

  SetValue(IN_AREA,param,4);	 /* Extra4 */
  SetValue(IN_TOPADDR0,param,0);
  SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
  info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

  printf("\n");
  printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
  printf("*** W# : %3d\n",info[10]);
  printf("***  X : %3d\n",info[11]);
  printf("***  Y : %3d\n",info[12]);
  printf("***  S : %3d\n",info[20]);

  sprintf(sample_name,"%c%c%c%c%c%c_W%02d_F-Ken",info[0],info[1],info[2],info[3],info[4],info[5],info[10]);
  sprintf(file_name,"%s%s%s_%d.csv", glob_cdp, glob_datalog, sample_name, get_site_number());

  if( NULL==(file_log = fopen( file_name, "at" )) ){
    printf( "OpenDataOutFile Fptdata Open Err[%s]", file_name );
  }

  fprintf(file_log,"%d,%d,%d,",info[11],info[12],info[20]);  /* X,Y,Sample */

/**************************************************************************************/
/***   Condition Set                                                                ***/
/**************************************************************************************/
  SetValue(IN_REG,param,0x03);
  SetValue(IN_AREA,param,0);

/**************************************************************************************/
/***   Shoki Erase                                                                  ***/
/**************************************************************************************/
  SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
  SetValue(IN_TESTSIZE0,param,0x100);
  SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
  SetValue(IN_TESTSIZE1,param,0x100);
  SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
  SetValue(IN_TESTSIZE2,param,0x100);    

  SetValue(IN_ETCR08,param,0x20);
  SetValue(IN_TESTSEL,param,0);
  vs[0] = 3300;
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("erase_code_at"),"Erase");
  SetValue(IN_ETCR08,param,0x00);

/**************************************************************************************/
/***   Write Test                                                                   ***/
/**************************************************************************************/
  SetValue(IN_TOPADDR0 ,param,FLP0_TOP);
  SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
  SetValue(IN_TOPADDR1 ,param,FLI1_TOP);
  SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
  SetValue(IN_TOPADDR2 ,param,FLI2_TOP);
  SetValue(IN_TESTSIZE2,param,FLI2_SIZE);    

  vs[0] = 1600;
  SetValue(IN_TESTSEL,param,0);
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*25,param,CreatePatFileName("programall_code_at"),"Program ALL");

/**************************************************************************************/
/***   Search Loss                                                                  ***/
/**************************************************************************************/
  vs[0] = 3300;
  search_loss = VthEdgeSearch(3,20,&vs[0],W1SEC*2, param);
  fprintf(file_log,"%d,",search_loss);

/**************************************************************************************/
/***   Erase Test                                                                   ***/
/**************************************************************************************/
  vs[0] = 1600;
  SetValue(IN_TESTSEL,param,0);
  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*3,param,CreatePatFileName("erase_code_at"),"Erase");

/**************************************************************************************/
/***   Search Gain                                                                  ***/
/**************************************************************************************/
  vs[0] = 3300;
  search_gain = VthEdgeSearch(2,20,&vs[0],W1SEC*2, param);
  fprintf(file_log,"%d,",search_gain);

/**************************************************************************************/
/***   Register Log Out                                                             ***/
/**************************************************************************************/
  fprintf(file_log,"%X,",GetValue(OUT_ETLR08,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR09,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR10,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR11,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR12,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR13,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR14,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR15,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR16,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR17,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR18,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR19,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR20,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR21,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR22,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR23,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR24,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR25,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR26,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR27,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR28,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR29,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR30,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR31,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR32,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR33,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR34,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR35,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR36,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR37,param));
  fprintf(file_log,"%X,",GetValue(OUT_ETLR38,param)); fprintf(file_log,"%X,",GetValue(OUT_ETLR39,param));
  fprintf(file_log,"\n");

/**************************************************************************************/
/***   END                                                                          ***/
/**************************************************************************************/
  fclose( file_log );

  time( &time_list2 );// time_list += V4V5K_TIME_OFS;

  printf("\n"); 
  printf("**************************\n"); 
  printf("*** TEST END           ***\n");
  printf("***                    ***\n");

  printf("*** Loss:%4d[mV]      ***",search_loss);
  if ((search_loss>1100)|(search_loss<800))  printf("  <= Loss Data Check!!!\n");
  else printf("\n");

  printf("*** Gain:%4d[mV]      ***",search_gain);
  if ((search_gain>-650)|(search_gain<-900)) printf("  <= Gain Data Check!!!\n");
  else printf("\n");

  printf("**************************\n"); 

  printf("Test-Time:%d[s]\n",time_list2-time_list); 
  printf("\n"); 

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
  vs[0] = stack9;
  /*********************/

}  





