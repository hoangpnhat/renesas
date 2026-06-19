/*******************************************************************************
*  FILENAME RC04EX_WT.c
*
  CONTENTS: This is the RC04EEX(SOTB) FLASH module function program
  for the 5000 test system
  COPYRIGHT 2018 Renesas Technology ALL RIGHTS RESERVED
  1'st created by Y.Aoki(SRM13)
 ******************************************************************************/
#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "RC04EX_apg.h"
#include "RC04EX_tst.h"
#include "RC04EX_fnc.h"
#include "COMPAT.h"
#include "RC04EX_WT.h"

#include "RC04EX.hpg"
#include "RC04EX_cyc.h"
#include "RC04EX_CGR.h"

#include "RC04EX_WT_integ.c"

#include "RC04EX_WT1.c"
#include "RC04EX_WT2.c"
#include "RC04EX_WT4.c"
#include "RC04EX_WT8.c"
#include "RC04EX_WT9.c"
#include "RC04EX_WT0.c"

#include "RC04EX_FT1.c"
#include "RC04EX_FT2.c"

/**************************************************************************************************/
/*      start test program function (DoTestProgram(0) from fshell.c)                              */
/**************************************************************************************************/
int hw_err_flag = 0;
int integm_flag = 0;

char logfile[256];

FLOW_COND_ST *exe_flow;
int exe_flow_size;

INTEGRATOR_PARAM_ST *integ_table;
int integ_table_size;

FLOW_COND_ST  next_test = { 0 };
FLOW_COND_ST  exe_test = {
  1234    ,                 //int  sno;
  1234    ,                 //int  tno;
  0x0000  ,                 //int  fk_on;
  0x0000  ,                 //int  fk_off;
  "FL_NONE" ,               //char* timming;
  3       ,                 //char exp;
  '!'     ,                 //char cat;
  "NO SET",                 //char *item;
  INIT_VCC,                 //int  vcc;mV
  INIT_VREF,                //int  vdd;mV
  INIT_VREFH,               //int  vddh;mV
  100000  ,                 //int  extal;ps
  1       ,                 //int  time_out;*1s
  1       ,                 //char pon;
  1       ,                 //char poff;
  ""      ,                 //char *mon_type;
  0       ,                 //int  limit_l;mV or nA
  0       ,                 //int  limit_h;mV or nA
  0       ,                 //int  supply;mV or nA
  ""      ,                 //char *pat_file;
  ""      ,                 //char *param_file;
  ""      ,                 //char *trans_file;
  ""      ,                 //char *item2;
};


TEST_RESULT_ST  test_result = {
  0       ,      // char first_cat
  0       ,      // int first_tno
  0       ,      // char fail_cat
  0       ,      // int fail_tno
  0       ,      // int judge_kind
  0       ,      // int monitor_1
  0       ,      // int monitor_2
  0       ,      // int test_time
  0       ,      // int boot_time
  0       ,      // int tap_shift;
  0,0,0,0 ,      // int ram_out[4];

};


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*  Description Mass Production Wafer Test DAXS BIST Function                 */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int WaferTestProgram( int prob_exec ){
  int vs[NUM_POWERSUPPLY];
  int process_n;
  int wt_test_time;
  int wt_flow_size, flow_cnt;
  int tempo, i;
  int pf_flag;
  int stack_pf_flag, mon_1st_fail_tno, mon_1st_fail_flag;
  char time_buf[32], time_buf2[32];
  FLOW_COND_ST  *wt_flow = 0;
  /*--------------------------------------------------------------------------*/
  /* variable initialize                                                      */
  /*--------------------------------------------------------------------------*/
  vs[0] = INIT_VCC;             /* vcc init */
  vs[1] = vs[2] = vs[3] = 0;    /* vcc init */
  fc_key = var2;       /* function key variable set */
  configration = RELEASEMODE;
  wt_test_time = 0;
  pf_flag = PASS;

  if( BIST_Start_Setup( prob_exec )!=PASS ){ DataOut( 1, "Log File Initialize Error !!\n" ); return( FAIL ); }
  if( DeviceSpecificInit()!=PASS ){ DataOut( Ffpt, "Device Initialize Error !!\n" ); return( FAIL ); }
  if( TesterTypeSelect( Tester_name ) ) return( FAIL );
  Ecrsetup8( ECR_OVERL );      /* SETUP ECR/BM */
  DeviceSpecificPowerUp();     /* PE vector vih1/vih2/vil1 mask drive time 100ns reconnect */


  wt_test_time = bentime();
  time( &time_list );
  sprintf( time_buf, "%s", C_time( (int *)&time_list ) );
  if( prob_exec&0xF0 ) sprintf( outbuf, "\n<<<<< START    FT%d TEST >>>>>\n", prob_exec&0x000F );
  else                 sprintf( outbuf, "\n<<<<< START PROBE%d TEST >>>>>\n", prob_exec&0x000F );
  DataOut( Ffpt, outbuf );
  sprintf( outbuf, "[ %.30s Test Chip -> X=%03d Y=%03d ] [start=%-24.24s]\n", devicename, xy_location[0], xy_location[1], time_buf ); DataOut( Ffpt, outbuf );
  if( fc_key ){ sprintf( outbuf, "???? Really Function Key not 0 ? fc_key(var2) = 0x%08X ????\n", fc_key ); DataOut( Ffpt, outbuf ); };
  DataOut( Ffpt, "Tno  ,Cat,Item                  ,Result    ,DC1   ,DC2   ,Boot Time ,Test Time  ,RAM(flag),Vcc  ,Vref ,VrefH,Extal  ,Force or Limit    ,Pattern                  ,Parameter                ,Trans,Output->," );
  for( i=0; i<BISTOUTPUTIF_END-BISTOUTPUTIF_TOP; i+=4 ){ sprintf( outbuf, "H'%04X  ,", i ); DataOut( Ffpt, outbuf ); }
  DataOut( Ffpt, "\n" );
  /*------------------------------------------------------------------------*/
  /* Test Flow Loop                                                         */
  /*------------------------------------------------------------------------*/
  for( flow_cnt = 0; flow_cnt<exe_flow_size; flow_cnt++ ){
    v_memcpy( (void*)( &exe_test ), (void*)( exe_flow + flow_cnt ), sizeof( FLOW_COND_ST ) ); /* Test Data Copy */
    if( PASS!=BIST_TestExe_Check( prob_exec ) ) continue;
    v_memcpy( (void*)( &next_test ), (void*)( exe_flow + flow_cnt + 1 ), sizeof( FLOW_COND_ST ) ); /* Next Test Data Copy */
    /*------------------------------------------------------------------------*/
    /* Flow Test Execute                                                      */
    /*------------------------------------------------------------------------*/
    pf_flag = BIST_Test_Func();
    if( pf_flag!=PASS ){
      if( pf_flag & 0x00000001 ){ printf("WT ERROR!!! Test Result Fail \n"); }
      if( pf_flag & 0x00000002 ){ printf("WT ERROR!!! Test Time Over \n"); }
      if( pf_flag & 0x00000004 ){ printf("WT ERROR!!! ECR ILLEGAL \n"); }
      if( pf_flag & 0x00000100 ){ printf("WT ERROR!!! Hardware Setting Error \n"); }
      if( pf_flag & 0x00001000 ){ printf("WT ERROR!!! DC Low Fail \n"); }
      if( pf_flag & 0x00002000 ){ printf("WT ERROR!!! DC Hi Fail \n"); }
      if( 0==test_result.first_cat ){
        test_result.first_cat = exe_test.cat;
        test_result.first_tno = exe_test.tno;
      }else{
        test_result.fail_cat = exe_test.cat;
        test_result.fail_tno = exe_test.tno;
      }
    }
    BIST_Integ_Store();

    if( CheckFK(FK_BREAK_KEY) ) tempo = CheckHitKey();
    else                        tempo = 0;

    if( ( PASS!=pf_flag && !CheckFK(FK_NOSTOP_FAIL) ) || tempo ) break;            /* fail stop break */
  } /* end for flow_cnt */

  if( pf_flag==PASS && test_result.first_cat==0 ) test_result.first_cat = '/';

  time( &time_list2 ); // time_list2 += V4V5K_TIME_OFS;
  sprintf( time_buf2, "%s", C_time( (int *)&time_list2 ) );
  wt_test_time = (bentime()-wt_test_time-init_bentime())/1000000;
  sprintf( outbuf, "\n END PROBE%d TEST  [end=%-24.24s]  test time=(%d s)\n", prob_exec, time_buf2, wt_test_time ); DataOut( Ffpt, outbuf );
  fclose( Fptdata );   /* debug log file close */
  fflush( stdout );

  DeviceLevelsPowerDown();
  return( PrintLogOutINTEG( prob_exec ) );

}  /* end WaferTestProgram */


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*  Description DAXS TEST Function                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int BIST_Test_Func( void ){
  int pf_flag;
  char *store_buff;
  char *boot_pat_vector;
  int trans_time, boot_time, boot_ferr, test_time, boot_exe;
  int cnt, i,j,bm_data,ecr_data,result;
  int pat_size, para_size, datasize,taddr;

  static interface_t interface[TABLE_END];

  test_result.monitor_1 = test_result.monitor_2 = 0x12345678; /* No Exe flag */
  hw_err_flag = pf_flag = boot_ferr = boot_exe = 0;

  test_time = bentime();
  sprintf( outbuf, "\n%5d,%c,%-22.22s", exe_test.tno, exe_test.cat, exe_test.item ); DataOut( Ffpt, outbuf );
  //** BM ECR Initialize **//
  for( cnt=RAM_TOP; cnt<=RAM_END; cnt=cnt+4 ){ WriteBmMode8( cnt, 0xFFFFFFFF, SIZE_L ); WriteEcrMode8( cnt, 0xFFFFFFFF, SIZE_L ); }

  if( 0==strcmp( exe_test.timming, "FL_OPST" ) ){
    pf_flag = BIST_OpensShorts( 7 ); goto RESULT_ST;
  }
  if( 0==strcmp( exe_test.timming, "FL_IRT" ) ){
    pf_flag = BIST_Istby( 0 ); goto RESULT_ST;
  }

  /*--------------------------------------------------------------------------*/
  /* RAM Write                                                                */
  /*--------------------------------------------------------------------------*/
  hw_err_flag |= DeviceLevelsPowerUp( daxs_trans.vcc, daxs_trans.vrefh, daxs_trans.vref, RNORMAL );
  hw_err_flag |= SetVohVolVtt( daxs_trans.voh, daxs_trans.vol, daxs_trans.vtt, daxs_datao_pin );
  set_strobe_mask( daxs_datao_pin );
  hw_err_flag |= SetTimming_NS( daxs_trans.write_rate );
  BIST_Moniter( 0x00 );
  if( PASS!=hw_err_flag ){ DataOut( Ffpt, "\nTrans Set Up Err\n" ); hw_err_flag |= 0x10000000; goto RESULT_ST; }

  /*---- bm Pattern set   ----*/
  if( 0!=strcmp( exe_test.pat_file, NULL ) ){
    strcpy( binpat, glob_cdp ); if( CheckFK(FK_WT_LOGIC) ) strcat( binpat, glob_binpat_logic ); else strcat( binpat, glob_binpat_wt );
    strcat( binpat, CreatePatFileName( exe_test.pat_file ) );
    pat_size = CheckPatLength( binpat ) - PAT_FILE_HEADER_SIZE;
    if( FAIL==comp_load_bm( binpat ) ){
      sprintf( outbuf, " !!! NOT OPEN FILE PATH -> %s (checksum:H'%X) !!!\n", binpat, pat_size ); DataOut( Ffpt,outbuf);
      hw_err_flag |= 0x00000010;
    }
  }
  if( 0==strcmp( exe_test.timming, "FL_LOGICTRM" ) ) goto RAM_TRANS;
  /*---- bm Parameter set ----*/
  if( 0!=strcmp( exe_test.param_file, NULL ) ){
    strcpy( binpat, glob_cdp ); if( CheckFK(FK_WT_LOGIC) ) strcat( binpat, glob_binpat_logic ); else strcat( binpat, glob_binpat_wt );
    strcat( binpat, CreatePatFileName( exe_test.param_file ) );
    para_size = CheckPatLength( binpat ) - PAT_FILE_HEADER_SIZE;
    if( FAIL==comp_load_bm( binpat ) ){
      sprintf( outbuf, " !!! NOT OPEN FILE PATH -> %s (checksum:H'%X) !!!\n", binpat, para_size ); DataOut( Ffpt,outbuf);
      hw_err_flag |= 0x00000020;
    }
  }

  /*---- bm Parameter set ----*/
  strcpy( binpat, glob_cdp ); if( CheckFK(FK_WT_LOGIC) ) strcat( binpat, glob_binpat_logic ); else strcat( binpat, glob_binpat_wt );
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
  strcat( binpat, "RC04EX_f_init_V000300.pat" );
#elif ( PRODUCT_NAME==PRODUCT_IO )
  strcat( binpat, "RC04EX_IO_f_init_V010000.pat" );
#endif

  para_size = CheckPatLength( binpat ) - PAT_FILE_HEADER_SIZE;
  if( FAIL==comp_load_bm( binpat ) ){
    sprintf( outbuf, " !!! NOT OPEN FILE PATH -> %s (checksum:H'%X) !!!\n", binpat, para_size ); DataOut( Ffpt,outbuf);
    hw_err_flag |= 0x00000040;
  }

  /*---- PAT or PARAM File Error ----*/
  if( 0==strcmp( exe_test.pat_file, NULL ) || 0==strcmp( exe_test.param_file, NULL ) ){
    DataOut( Ffpt, " !!! Null PAT or PARAM !!!\n" );
    hw_err_flag |= 0x00000080;
  }

  /*---- bm random data set ----*/
  datasize = BIST_RandomDataBM_Set();
  if( PASS!=hw_err_flag ){ DataOut( Ffpt, "\nBM Set Err\n" ); hw_err_flag |= 0x20000000; goto RESULT_ST; }

RAM_TRANS:
  /*---- DAXS Write ----*/
  if( ( NULL!=strstr( exe_test.pat_file, "WRK027") || NULL!=strstr( exe_test.pat_file, "WRK029") ) && 0!=strcmp( exe_test.timming, "FL_EXT" ) ){
    taddr = 0x29FF;
    RAMRunApg( 0, RAM_TOP, RAM_TOP+0x29FF );
    RAMRunApg( 0, RAM_TOP+0x3000, RAM_TOP+0x3FFF );
  }else{
    taddr = RAM_END;
    RAMRunApg( 0, RAM_TOP, RAM_END );
  }

  if( CheckFK(FK_VF_TRANS) ){
    /*---- DAXS Read ----*/
    if( CheckFK(FK_DISP_DEBUG) ) DataOut( Ffpt, "Read RAM \n" );

    SetTimming_NS( daxs_trans.read_rate );
    set_strobe_mask( daxs_datao_pin );
    SetVohVolVtt( daxs_trans.vcc/2, daxs_trans.vcc/2, 0, no_pin );

    RAMRunApg( 2, RAM_TOP, RAM_END );

    for( i=RAM_TOP; i<=taddr; i=i+4 ){ //BM<->ECR Compare
      bm_data = ReadBmMode8( i, SIZE_L );
      ecr_data= ReadEcrMode8( i, SIZE_L );
      if( bm_data!=ecr_data ){
        sprintf( outbuf, "\nBM <=> ECR Comp RAM Trans ERROR!!! Addr = %X, BM = %X, ECR = %X \n", i, bm_data, ecr_data ); DataOut( Ffpt, outbuf );
        pf_flag = RAMFAIL; goto RESULT_ST;
      }
    }
  }

  /*--------------------------------------------------------------------------*/
  /* DAXS BOOT Flash Test Execute                                             */
  /*--------------------------------------------------------------------------*/

  hw_err_flag |= DeviceLevelsPowerUp( exe_test.vcc, exe_test.vrefh, exe_test.vref, BIST_RELAY_Set() );
  hw_err_flag |= SetVohVolVtt( 2*exe_test.vcc/10, 2*exe_test.vcc/10, 5*exe_test.vcc/10 , bistpoll_pins );
  hw_err_flag |= set_strobe_mask( bistpoll_pins );

  hw_err_flag |= SetTimming_PS( exe_test.extal );
  hw_err_flag |= select_vector_mode( poll_pins, _PE_VECTOR_MODE );
  hw_err_flag |= reconnect_pin( poll_pins );

  boot_pat_vector = BIST_Apg_Set();
  if( PASS!=hw_err_flag ){ DataOut( Ffpt, "\nBoot Set Up Err\n" ); hw_err_flag |= 0x10000000; goto RESULT_ST; }
  hw_err_flag |= BIST_Supply_Set();
  BIST_Moniter( 0x10 );

  /*---- DAXS Boot ----*/
  if( CheckFK(FK_INPUT_SCOPE) ) WaitHitKey( "\nPlease Hit Any Key" );
  boot_time = bentime();
  run_apg( boot_pat_vector );  /* Boot */
  BIST_Moniter( 0x20 );

  boot_exe = 1;
  boot_ferr = check_pin_errors( bistpoll_pins, 4 );
  test_result.boot_time = ( bentime()-boot_time-init_bentime() )/1000;
  if( NULL!=strstr( exe_test.pat_file, "subosc_trim") ) comp_set_v3( 0 );

  /*--------------------------------------------------------------------------*/
  /* RAM Read                                                                 */
  /*--------------------------------------------------------------------------*/
  select_vector_mode(poll_pins,_PE_APG_MODE);
  reconnect_pin(poll_pins);
  SetTimming_NS( daxs_trans.read_rate );
  set_strobe_mask(daxs_datao_pin);  //Compare Setting command
  SetVohVolVtt( daxs_trans.voh, daxs_trans.vol, daxs_trans.vtt, daxs_datao_pin );//�I�[��R�⃍�W�J��Pin��H/L����̓d���ݒ�(DAXS�]���p)

  if( CheckFK( FK_DOUT_RD_RAM ) ){
    for( cnt=RAM_TOP; cnt<=RAM_END; cnt=cnt+4 ) WriteEcrMode8( cnt, 0xFFFFFFFF, SIZE_L );
    RAMRunApg( 2, RAM_TOP, RAM_END );
  }else{
    for( cnt=RAM_TOP+0x3D00; cnt<=RAM_TOP+0x3DFF; cnt=cnt+4 ) WriteEcrMode8( cnt, 0xFFFFFFFF, SIZE_L );
    RAMRunApg( 2, RAM_TOP+0x3D00, RAM_TOP+0x3DFF );
    if( 0==strcmp( exe_test.timming, "FL_INT" ) ){
      for( cnt=RAM_TOP+0x3600; cnt<=RAM_TOP+0x39FF; cnt=cnt+4 ) WriteEcrMode8( cnt, 0xFFFFFFFF, SIZE_L );
      RAMRunApg( 2, RAM_TOP+0x3600, RAM_TOP+0x39FF );
    }
    if( 0==strcmp( exe_test.timming, "FL_RAMDUMP" ) ){
      for( cnt=RAM_TOP+0x3600; cnt<=RAM_TOP+0x39FF; cnt=cnt+4 ) WriteEcrMode8( cnt, 0xFFFFFFFF, SIZE_L );
      RAMRunApg( 2, RAM_TOP+0x3600, RAM_TOP+0x39FF );
    }
  }
  if( NULL!=strstr( exe_test.pat_file, "read_dump") ){
    for( cnt=RAM_TOP+0x8000; cnt<=RAM_TOP+0xC000; cnt++ ) write_ecr( cnt, 0xFF );
    RAMRunApg( 2, RAM_TOP + 0x8000, RAM_TOP + 0xC000 + 0xFF );
  }

  /*--------------------------------------------------------------------------*/
  /* Result sequence                                                          */
  /*--------------------------------------------------------------------------*/
RESULT_ST:
  if( 1==exe_test.poff || hw_err_flag ){
    DeviceLevelsPowerDown();
  }

  if( boot_exe ){
    if( ReadEcrMode8(0x3D00,SIZE_L)==0xAAAAAAAA ){
      pf_flag |= 0x00000001;
    }else if( ReadEcrMode8(0x3D00,SIZE_L)==0x55555555 ){
    }else{
      pf_flag |= 0x00000004;
    }
    if( boot_ferr==0 )      pf_flag |= 0x00000002;
    if( exe_test.exp==1 )   pf_flag ^= 0x00000001;
  }
  if( PASS!=hw_err_flag ) pf_flag |= 0x00000100;

  if( 0x12345678!=test_result.monitor_1 && !( exe_test.limit_l==0 && exe_test.limit_h==0 ) ){
    if( 19200==exe_test.limit_l && 19200==exe_test.limit_h ){
      pf_flag |= BIST_DC_RAM_Judge();
    }else{
      if( test_result.monitor_1<exe_test.limit_l ) pf_flag |= 0x00001000;
      if( test_result.monitor_1>exe_test.limit_h ) pf_flag |= 0x00002000;
    }
  }

  test_result.test_time = ( bentime()-test_time-init_bentime() )/1000;

  if( pf_flag ) DataOut( Ffpt, ",!! FAIL !!" );
  else          DataOut( Ffpt, ",   PASS   " );
  if( 0x12345678!=test_result.monitor_1 ){
    //if( test_result.monitor_1<0 ) sprintf( outbuf, ",%5d", test_result.monitor_1 );
    //else                          sprintf( outbuf, ",%6d", test_result.monitor_1 );
    sprintf( outbuf, ",%6d", test_result.monitor_1 ); DataOut( Ffpt, outbuf );
  }else{
    DataOut( Ffpt, ",      " );
  }
  if( 0x12345678!=test_result.monitor_2 ){
    sprintf( outbuf, ",%6d", test_result.monitor_2 ); DataOut( Ffpt, outbuf );
  }else{
    DataOut( Ffpt, ",      " );
  }
  sprintf( outbuf, ", %6d ms,/ %6d ms", test_result.boot_time, test_result.test_time ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, ", %02X(%04X),%5d,%5d,%5d,%7d", ReadEcrMode8(0x3D00,SIZE_B), pf_flag, exe_test.vcc, exe_test.vref, exe_test.vrefh, exe_test.extal ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, ", %5d(%5d %5d), %s, %s, %s", exe_test.supply, exe_test.limit_l, exe_test.limit_h, exe_test.pat_file, exe_test.param_file, exe_test.trans_file ); DataOut( Ffpt, outbuf );

  if( CheckFK( FK_DOUT_RD_RAM ) ){
    if( 0==strcmp( exe_test.timming, "FL_LOGICTRM" ) ){
      DataOut( Ffpt, ", Addr=H'20000080-->" ); for( cnt=RAM_TOP + 0x80; cnt<RAM_TOP + 0x100; cnt+=4 ){ sprintf( outbuf, ",'%08X", ReadEcrMode8( cnt, SIZE_L ) ); DataOut( Ffpt, outbuf ); }
      if( CheckFK( FK_DOUT_DISP ) ){
        DataOut( Ffpt, "\n,,,,,,,,,,,,,,,,, Addr=H'20003D00-->" ); for( cnt=RAM_TOP + 0x3D00; cnt<RAM_TOP + 0x3E00; cnt+=4 ){ sprintf( outbuf, ",'%08X", ReadEcrMode8( cnt, SIZE_L ) ); DataOut( Ffpt, outbuf ); }
      }
    }else{
      DataOut( Ffpt, ", Addr=H'20003D00-->" ); for( cnt=RAM_TOP + 0x3D00; cnt<RAM_TOP + 0x3E00; cnt+=4 ){ sprintf( outbuf, ",'%08X", ReadEcrMode8( cnt, SIZE_L ) ); DataOut( Ffpt, outbuf ); }
      if( CheckFK( FK_DOUT_DISP ) ){
        DataOut( Ffpt, "\n,,,,,,,,,,,,,,,,, Addr=H'20000080-->" ); for( cnt=RAM_TOP + 0x80; cnt<RAM_TOP + 0x100; cnt+=4 ){ sprintf( outbuf, ",'%08X", ReadEcrMode8( cnt, SIZE_L ) ); DataOut( Ffpt, outbuf ); }
      }
    }
  }

  if( 0==strcmp( exe_test.timming, "FL_LOGICTRM" ) ) DispLogic( 0 );
  return( pf_flag );
}

/*----------------------------------------------------------------------------*/

/*  Description BM data set                                                   */
/*----------------------------------------------------------------------------*/
int BIST_RandomDataBM_Set( void ){
  int datasize, i, id_top, cnt, data1, data2;
  char time_buf[32],time_buf2[32];
  GetDateTime( &time_list, time_buf, 1 );

  datasize = 0;
  id_top = 0x3800;
  if( 0==strcmp( exe_test.timming, "FL_WRK" ) ){
    comp_write_bm( id_top + 0x00, 0x01 ); /* WT1_Rev */
    comp_write_bm( id_top + 0x01, 0x00 );
    comp_write_bm( id_top + 0x02, 0x01 ); /* WT2_Rev */
    comp_write_bm( id_top + 0x03, 0x00 );
    comp_write_bm( id_top + 0x04, 0x01 ); /* WT4_Rev */
    comp_write_bm( id_top + 0x05, 0x00 );
    comp_write_bm( id_top + 0x06, 0x01 ); /* WT8_Rev */
    comp_write_bm( id_top + 0x07, 0x00 );
    comp_write_bm( id_top + 0x08, lotname[ 0 ] );  /* Lot Name */
    comp_write_bm( id_top + 0x09, lotname[ 1 ] );
    comp_write_bm( id_top + 0x0A, lotname[ 2 ] );
    comp_write_bm( id_top + 0x0B, lotname[ 3 ] );
    comp_write_bm( id_top + 0x0C, lotname[ 4 ] );
    comp_write_bm( id_top + 0x0D, lotname[ 5 ] );
    comp_write_bm( id_top + 0x0E, lotname[ 6 ] );
    comp_write_bm( id_top + 0x0F, lotname[ 7 ] );
    comp_write_bm( id_top + 0x10, lotname[ 8 ] );
    comp_write_bm( id_top + 0x11, lotname[ 9 ] );
    comp_write_bm( id_top + 0x12, atoi( wafername ) );
    comp_write_bm( id_top + 0x13, tyear - 2000 );
    comp_write_bm( id_top + 0x14, xy_location[0]&0xFF );
    comp_write_bm( id_top + 0x15, (xy_location[0]>>8)&0xFF );
    comp_write_bm( id_top + 0x16, xy_location[1]&0xFF );
    comp_write_bm( id_top + 0x17, (xy_location[1]>>8)&0xFF );
    comp_write_bm( id_top + 0x18, tmon );
    comp_write_bm( id_top + 0x19, tday );
    comp_write_bm( id_top + 0x1A, 'M' );  /* ���_�R�[�h */
    comp_write_bm( id_top + 0x1B, 'u' );
    comp_write_bm( id_top + 0x1C, 0xFF ); /* C */
    comp_write_bm( id_top + 0x1D, 0xFF ); /* R */
    comp_write_bm( id_top + 0x1E, 0xFF ); /* C */
    comp_write_bm( id_top + 0x1F, 0xFF ); /* Code */
    datasize = 32;
    if( NULL!=strstr( exe_test.param_file, "WT1_P1997" ) ){
      for( cnt=0; cnt<integ_table_size; cnt++ ){
        if( ( integ_table+cnt )->t_no == 19900 ) data1 = ( integ_table+cnt )->value;  /* "T85", 19901, 19900,"E_Vhh_af_VthTri"  */
        if( ( integ_table+cnt )->t_no == 19950 ) data2 = ( integ_table+cnt )->value;  /* "T86", 19951, 19950,"P_Vhh_af_VthTri"  */
      }
      comp_write_bm( id_top + 0x20, data2&0x00FF ); /* P_Vhh */
      comp_write_bm( id_top + 0x21, (data2>>8)&0x00FF );
      comp_write_bm( id_top + 0x22, (data2>>16)&0x00FF );
      comp_write_bm( id_top + 0x23, (data2>>24)&0x00FF );
      comp_write_bm( id_top + 0x24, data1&0x00FF ); /* E_Vhh */
      comp_write_bm( id_top + 0x25, (data1>>8)&0x00FF );
      comp_write_bm( id_top + 0x26, (data1>>16)&0x00FF );
      comp_write_bm( id_top + 0x27, (data1>>24)&0x00FF );
      datasize += 8;
    }
  }else if( 0==strcmp( exe_test.timming, "FL_VTHTRM" ) ){
    comp_write_bm( id_top + 0x28, test_result.tap_shift );
    datasize = 1;
  }else if( 0==strcmp( exe_test.timming, "FL_EXT" ) ){
    strcpy( binpat, glob_cdp ); if( CheckFK(FK_WT_LOGIC) ) strcat( binpat, glob_binpat_logic ); else strcat( binpat, glob_binpat_wt );
    strcat( binpat, CreatePatFileName( exe_test.trans_file ) );
    datasize = CheckPatLength( binpat ) - PAT_FILE_HEADER_SIZE;
    if( FAIL==comp_load_bm( binpat ) ){
      sprintf( outbuf, " !!! NOT OPEN FILE PATH -> %s (checksum:H'%X) !!!\n", binpat, datasize ); DataOut( Ffpt,outbuf);
      hw_err_flag |= 0x00000020;
    }
  }else{
    datasize = 0;
  }
  return( datasize );
}

/*----------------------------------------------------------------------------*/
/*  Description APG load vlf & vecter set                                     */
/*----------------------------------------------------------------------------*/
char* BIST_Apg_Set( void ){
  int exvref, extal, timeout;

  exvref = 0;
  if( ( exe_test.vref != 0 ) && ( exe_test.vrefh != 0 ) ){
    exvref = exvref + 0x01;
    if( CheckFK(FK_DISP_DEBUG) ) DataOut( Ffpt, " Ext_Vref  ON \n" );
  }else{
    if( CheckFK(FK_DISP_DEBUG) ) DataOut( Ffpt, " Ext_Vref  OFF \n" );
  }

  extal = exe_test.extal / 1000;
  timeout = exe_test.time_out * 500;
  load_vlf_offset( _SUB_MODEENTRY_RAM, 1,  1000/2/extal );    /*   1us Wait */
  load_vlf_offset( _SUB_MODEENTRY_RAM, 2,300000/2/extal );    /* 300us Wait */
  load_vlf_offset( _SUB_MODEENTRY_RAM, 3, 70000/extal );      /*  70ms Wait */
  load_vlf_offset( _SUB_MODEENTRY_RAM, 4, exvref );  
  load_vlf_offset( _SUB_RAMBT_END, 2, 2000000/203/extal );    /*   2ms loop  0x1EC@20ns*/
  load_vlf_offset( _SUB_RAMBT_END, 0, timeout );              /* time out  Max 0xFFFF 131s */

  if( 0==strcmp( exe_test.mon_type, "VCL" ) || 0==strcmp( exe_test.timming, "FL_STBY" ) ){
    load_vlf_offset( _RAM_BOOT_CTRL_RESH, 4, 20 );  /* 200ms */
    return( _RAM_BOOT_CTRL_RESH );
  }
  if( 0==strcmp( exe_test.timming, "FL_MON" ) ){
    load_vlf_offset( _SUB_RAMBT_END_RES_H, 2, 2000000/203/extal );    /*   2ms loop  0x1EC@20ns*/
    load_vlf_offset( _SUB_RAMBT_END_RES_H, 0, timeout );              /* time out  Max 0xFFFF 131s */
    if( CheckFK(28) ) return( _RAM_BOOT_MONITOR_CTRL );
    else              return( _RAM_BOOT_CTRL_RESH_RELAY );
    //return( _RAM_BOOT_CTRL_RESH );
    //return( _RAM_BOOT_CTRL_RESH_RELAY );
  }
  return( _RAM_BOOT_CTRL ); /* normal function */
}

int BIST_Supply_Set( void ){
  if( NULL!=strstr( exe_test.pat_file, "subosc_trim") ){
    set_pmu_range( MA25 );
    comp_set_i3limit( 2000000 );  /* 2mA */
    comp_set_v3( exe_test.supply );
  }
  if( 0==strcmp( exe_test.timming, "FL_TRM" ) ){
    set_vlimit( 0 );
    comp_set_ilimit( 0 );
    pmu_delay( 10000 );         /* pmu delay 10ms */

    if( NULL!=strstr( exe_test.item, "Iref" ) ){
      set_pmu_range( UA25 );
      comp_set_ilimit( exe_test.supply );
    }else{
      set_pmu_range( UA250 );
      comp_set_ilimit( 250000 );// I-Limit = 250000nA(250uA)
      set_vlimit( exe_test.supply );
    }
    connect_pmu( v5_pin );      // Connect VNN <=> PMU
  }
  return( PASS );

}

/*----------------------------------------------------------------------------*/
/*  Description moniter test                                                  */
/*----------------------------------------------------------------------------*/
int BIST_Moniter( int mode ){
  if( 0==strcmp( exe_test.mon_type, "" ) || 0==strcmp( exe_test.timming, "FL_TRM" ) ) return( PASS );

  switch( mode&0xF0 ){
  case 0x00:   /* trans before */
    if( 0==strcmp( exe_test.timming, "FL_STBY" ) ){
      comp_set_ilimit( 0 );        /* PMU 0A */
      set_vlimit( 0 );             /* PMU 0V */
      set_pmu_range( MA25 );       /* range NA2500, UA25, UA250, UA2500, MA25, MA250 MA1000 */
      pmu_delay( 1000 );           /* PMU Conect delay(us) 1ms */
      exe_test.limit_h = next_test.limit_h;
      comp_set_ilimit( 20000000 ); /* PMU +-xxmA clamp  20mA */
      comp_set_iparh( 20000000 );  /* PMU meas Hi current judgh 20mA  */
      pmu_delay( 1000 );           /* PMU Conect delay(us) 1ms */
      set_vlimit( daxs_trans.vcc );/* PMU trans */
      connect_pmu( v1_pin );       /* PMU vs1 connect    */
    }else{
      comp_set_ilimit( 0 );        /* PMU 0A */
      set_vlimit( 0 );             /* PMU 0V */
      set_pmu_range( UA250 );      /* range NA2500, UA25, UA250, UA2500, MA25, MA250 MA1000 */
      pmu_delay( 1000 );           /* PMU Conect delay(us) 1ms */
    }
    break;
  case 0x10:   /* boot before */
    if( 0==strcmp( exe_test.timming, "FL_STBY" ) ){
      set_vlimit( exe_test.vcc );  /* PMU Boot Vcc */
      delay_timer( 10000 );        /* wait 10ms */
    }else{
      comp_set_ilimit( 250000 );   /* PMU+ -xxmA clamp  250uA */
      connect_pmu( v5_pin );       /* PMU connect */
    }
    break;
  case 0x20:   /* boot after */
    if( 0==strcmp( exe_test.mon_type, "VCL" ) ){
      connect_pmu( vcl1_pin );
      SITE_MASK( 1, test_result.monitor_1 = read_adc_ave( ADC_VMEAS, 0, 10 ) );
      reconnect_pin( vcl1_pin );
      connect_pmu( vcl2_pin );
      SITE_MASK( 1, test_result.monitor_2 = read_adc_ave( ADC_VMEAS, 0, 10 ) );
      reconnect_pin( vcl2_pin );
      connect_pmu( daxs_datai_pin );
      if( CheckFK(FK_DISP_DEBUG) ){
        sprintf( outbuf, "\nVCL1 = %d[mV] ", test_result.monitor_1 ); DataOut( Ffpt, outbuf );
        sprintf( outbuf, " VCL2 = %d[mV]", test_result.monitor_1 ); DataOut( Ffpt, outbuf );
        sprintf( outbuf, " ATBM1(DATAI) = %d[mV]\n", read_adc_ave( ADC_VMEAS, 0, 10 ) ); DataOut( Ffpt, outbuf );
      }
      reconnect_pin( daxs_datai_pin );
    }else if( 0==strcmp( exe_test.timming, "FL_STBY" ) ){
      SITE_MASK( 1, test_result.monitor_1 = read_adc_ave( ADC_IMEAS, 0, 100 ) );
      reconnect_pin( v1_pin );
    }else if( exe_test.supply ){
      //connect_pmu(v5_pin);     // Connect VNN <=> PMU
      set_vlimit( exe_test.supply );
      SITE_MASK (1, test_result.monitor_1 = read_adc_ave( ADC_IMEAS, 0, 100 ) );
    }else{
      //connect_pmu( v5_pin );      /* PMU connect */
      SITE_MASK( 1, test_result.monitor_1 = read_adc_ave( ADC_VMEAS, 0, 10 ) );
    }
    set_vlimit( 0 );
    comp_set_ilimit( 0 );
    reconnect_pin( v5_pin );
    run_apg( _RAM_BOOT_CTRL_RESL );
    break;
  default:
    break;
  }

  return( PASS );
}


/*----------------------------------------------------------------------------*/
/*  Description RELAY set                                                     */
/*----------------------------------------------------------------------------*/
int BIST_RELAY_Set( void ){
  int relay;
  relay = RNORMAL;

  if( 0==strcmp( exe_test.timming, "FL_MON" ) || 0==strcmp( exe_test.timming, "FL_TRM" ) ){
    if( 0==strcmp( exe_test.mon_type, "Vccmon" ) ) relay = RVCCMON;
    if( 0==strcmp( exe_test.mon_type, "Vssmon" ) ) relay = RVSSMON;
  }else{
    relay = RNORMAL;
  }

  if( CheckFK(FK_DISP_DEBUG) ){ sprintf( outbuf, " BIST_RELAY_Set relay=0x%04X\n", relay ); DataOut( Ffpt, outbuf ); }
  return( relay );
}

/*----------------------------------------------------------------------------*/
/*  Description Easy Istby Test                                               */
/*----------------------------------------------------------------------------*/
int BIST_Istby( int mode ){
  int status, hw_flag;
  status = hw_flag = 0;
  if( 0==mode || 1==mode || 2==mode ){
    DeviceLevelsPowerDown();
    hw_flag |= SetTimming_NS( 500 );
    hw_flag |= DeviceLevelsPowerUp( exe_test.vcc, exe_test.vrefh, exe_test.vref, RNORMAL );
    pmu_delay( 50000 );                   /* PMU delay(us) 50ms */
    set_vlimit( 0 );                      /* PMU output voltage reset */
    set_pmu_range( MA250 );               /* range NA2500, UA25, UA250, UA2500, MA25, MA250 MA1000 */
    comp_set_ilimit( 200000000 );         /* PMU +-200mA clamp  */
    comp_set_iparh( exe_test.limit_h );   /* PMU meas Hi current judgh */
    connect_pmu( v1_pin );                /* PMU vs1 connect    */
    hw_flag |= set_vlimit( exe_test.vcc );/* PMU output voltage */
    delay_timer( 10000 );                /* 10ms */
    if( 0==mode ) run_apg( _ISTBY_RESET );
//    if( 1==mode ) run_apg( _ISTBY_TM30 );
//    if( 2==mode ) run_apg( _ISTBY_TM27 );
    delay_timer( 100000 );                /* wait(us) 100ms */
    /* mask 0x01=-ilimit clamp/0x02=+ilimit clamp/0x04=iparl/0x08=iparh/0x10=voltage < CVPAR/0x20=voltage > CVPAR */
    SITE_MASK( 1, status = read_pmu_status( 0x0C ) );                        /* judgh iparl/iparh     */
    SITE_MASK( 1, test_result.monitor_1 = read_adc_ave( ADC_IMEAS, 0, 100 ) );  /* 100 time meas average  */
    set_vlimit( 0 );
    disconnect_pin( v1_pin );           /* vs1 pmu reject  */
    if( CheckFK(FK_DISP_DEBUG) ){ sprintf( outbuf, "-- Istby  %d nA --\n", test_result.monitor_1 ); DataOut( Ffpt, outbuf ); }
    DeviceLevelsPowerDown();
  }

  if( status ) return( FAIL );
  else         return( PASS );
}


/*----------------------------------------------------------------------------*/
/*  Description Open/Short Test                                               */
/*----------------------------------------------------------------------------*/
int BIST_OpensShorts( int mode ){
  int status_op,status_st,voltage,pf_flag;
  int cnt;

  pf_flag = FAIL;
  DeviceLevelsPowerDown();
  if( mode&0x01 ){                        /* VCC, PVCC short */
    set_pmu_range( MA25 );         /* range NA2500, UA25, UA250, UA2500, MA25, MA250 MA1000 */
    pmu_delay( 10000 );            /* PMU delay(us) 10ms */
    comp_set_ilimit( 20000000 );   /* PMU force 20mA clamp  */
    set_vlimit( 0 );               /* PMU output voltage reset */
    comp_set_v1( 0 );              /* vs1 reset          */
    comp_set_cvpar( INIT_VCC );    /* PMU 3.0v clamp     */
    connect_pmu( v1_pin );         /* PMU vs1 connect    */
    set_vlimit( INIT_VCC );        /* PMU output voltage */
#ifdef V5KONLY
    SITE_MASK( 1, status_st = read_pmu_status( 0x23 ) );        /* judgh Iclamp CVPAR over */
    SITE_MASK( 1, voltage = read_adc_ave( ADC_VMEAS, 0, 10 ) ); /* 10 time meas  */
#else
    status_st = read_pmu_status( 0x23 );            /* judge Iclamp CVPAR over */
    voltage = read_adc_ave( ADC_VMEAS, 0, 10 );     /* 10 time meas  */
#endif
    disconnect_pin( v1_pin );             /* vs1 pmu reject  */
    if( INIT_VCC*2/3 > voltage ){
      if( CheckFK(FK_DISP_DEBUG) ){ sprintf( outbuf, "-- VCC FAIL %d mV --\n", voltage ); DataOut( Ffpt, outbuf ); }
/* caution!! *///      goto BIST_OS_END;
    }
    if( CheckFK(FK_DISP_DEBUG) ){ sprintf( outbuf, "-- VCC %d mV --\n", voltage ); DataOut( Ffpt, outbuf ); }
  }

  run_apg( _OPENS_SHORTS );                 /* addr, data, op_pins nr=1 DR Hi     */
  reconnect_pin( os_pins );                 /* os_pins connect                    */
  pmu_delay( 100 );                         /* pmu wait=100usec                   */
  set_pmu_range( UA25 );                    /* max=25uA                           */
  set_vlimit( -1500 );                      /* set pmu input voltage = -1.5V      */
  comp_set_ilimit( 10000 );                 /* I-Limit = 10000nA(10uA)            */
  comp_set_cvpar ( -1000 );                 /* -1v limit                          */

  if( mode&0x02 ){
    status_op = seq_pmu_test_vlog( os_pins, 0x10 );         /* os_pins pmu voltage log all  mask Sense voltage < CVPAR */
    if( status_op ){
      comp_set_cvpar( 0 );
      if( CheckFK(FK_DISP_DEBUG) ) disp_pinvlog( 0x03, 0x03, 0x01, os_pins, "-- open --" );
      if( CheckFK(FK_DISP_DEBUG) ) DataOut( Ffpt,  "-- FAIL(open) --\n" );
      goto BIST_OS_END;
    }
  }

  if( mode&0x04 ){
    comp_set_cvpar( 10 );  /* 10mv limit */
    status_st = seq_pmu_test_vlog( os_pins, 0x20 );           /* os_pins pmu voltage log all mask Sense voltage > CVPAR */
    if( CheckFK(FK_DISP_DEBUG) ) for(cnt=0; cnt<64; cnt++){ sprintf( outbuf, "%d,", os_pins[cnt*4+3] ); DataOut( Ffpt, outbuf ); }
    if( status_st ){
      if( CheckFK(FK_DISP_DEBUG) ) disp_pinvlog( 0x03, 0x03, 0x01, os_pins, "-- short --" );
      if( CheckFK(FK_DISP_DEBUG) ) DataOut( Ffpt,  "-- FAIL(short) --\n" );
      goto BIST_OS_END;
    }
  }
  pf_flag = PASS;
BIST_OS_END:
  set_vlimit( 0 );      /* reset pmu input voltage = 0V */
  comp_set_cvpar( 0 );  /* reset 0v limit */
  run_apg( _OPENS_SHORTS_END ); /* addr, data, op_pins nc after nr=0 DR Lo */
  DeviceSpecificPowerDown();
  DeviceSpecificPowerUp();     /* PE vector vih1/vih2/vil1 mask drive time 100ns reconnect */

  if( CheckFK(FK_DISP_DEBUG) && PASS==pf_flag ) DataOut( Ffpt, "** BIST_OpensShorts PASS **\n" );
  return( pf_flag );
}


/*----------------------------------------------------------------------------*/
/*  Description PrintLogOutINTEG set                                          */
/*----------------------------------------------------------------------------*/
int PrintLogOutINTEG( int prob_exec ){
  int  i, cnt, tempo;
  int  ofs, unit;
  char px_str;
  char time_buf[32],time_buf2[32];
  char* cp;
  FILE_T *fpt;

  if( prob_exec&0xF0 ){
    sprintf( logfile, "%s%s%s_L%s_W%s_FT%d_INTEG_%d.csv", glob_cdp, glob_probe, DEVICENAME, lotname, wafername, prob_exec&0x0F, get_site_number() );
  }else{
    sprintf( logfile, "%s%s%s_L%s_W%s_WT%d_INTEG_%d.csv", glob_cdp, glob_probe, DEVICENAME, lotname, wafername, prob_exec&0x0F, get_site_number() );
  }

  /* logfile open */
  if( NULL==( fpt = fopen( logfile, "at" ) ) ){
    ErrorPrint( logfile ); return( FAIL );
  }

  /* get time */
  GetDateTime( &time_list, time_buf, 1 );
  time( &time_list2 );
  GetDateTime( &time_list2, time_buf2, 1 );


  /* print data */
  fprintf( fpt,"\n%d,%d,%d,%d,", xy_location[0], xy_location[1], shot_number, get_site_number() ); /* X,Y,INDEX,DUT, */
  if( '/'==test_result.first_cat ) fprintf( fpt,"GO," );      /* G/N, */
  else                             fprintf( fpt,"NG," );
  fprintf( fpt,"%d,%c,", test_result.first_cat, test_result.first_cat );        /* C1,C1_MARK, */
  fprintf( fpt,"%d,%c,%d,%d,", test_result.fail_cat, test_result.fail_cat, test_result.first_tno, test_result.fail_tno );  /* C2,C2_MARK,FT,ET */
  fprintf( fpt, "%s,%s,", time_buf, time_buf2 ); /* TSTART,TEND */

  for( cnt = 0; cnt<integ_table_size; cnt++ ){
    if( ( integ_table + cnt )->address == 0 ){
      if( ( integ_table + cnt )->exe_flag == ON ){  /* DC */
        if     ( 0==strcmp( ( integ_table + cnt )->unit, "V" ) )  unit = 1000;
        else if( 0==strcmp( ( integ_table + cnt )->unit, "mA" ) ) unit = 1000000;
        else if( 0==strcmp( ( integ_table + cnt )->unit, "uA" ) ) unit = 1000;
        else if( 0==strcmp( ( integ_table + cnt )->unit, "MHz" ) ) unit = 1000000;
        else if( 0==strcmp( ( integ_table + cnt )->unit, "KHz" ) ) unit = 1000;
        else    unit = 1;
        switch( unit ){
          case 1000: fprintf( fpt,"%d.%03d,", ( integ_table + cnt )->value/1000, ( integ_table + cnt )->value%1000 ); break;
          case 1000000: fprintf( fpt,"%d.%04d,", ( integ_table + cnt )->value/1000000, ( ( integ_table + cnt )->value/100 ) % 10000 ); break;
          default: fprintf( fpt,"%d,", ( integ_table + cnt )->value ); break;
        }
      }else{
        fprintf( fpt,"NON," );
      }
    }else{
      if( ( integ_table + cnt )->exe_flag == ON ){
        if( 0==strcmp( ( integ_table + cnt )->mode, "HEX" ) ){
          fprintf( fpt,"0x%X,", ( integ_table + cnt )->value );
        }else if( 0==strcmp( ( integ_table + cnt )->mode, "DEC" ) ){
          switch( ( integ_table + cnt )->calculation ){
          case 11: case 21: case 23: case 41: case 81: case 83: //H_osc
            tempo = 1000 * ( 65535 - ( integ_table + cnt )->value ) * 4 / 270; fprintf( fpt,"%d.%03d,", tempo/1000, tempo%1000 );
            break;
          case 12: case 22: case 24: case 42: case 82: case 84: //L_osc
            tempo = 1000 * ( 65535 - ( integ_table + cnt )->value ) / 100; fprintf( fpt,"%d.%03d,", tempo/1000, tempo%1000 );
            break;
          default:
            fprintf( fpt,"%d,", ( integ_table + cnt )->value );
            break;
          }
        }else{
        };
      }else{
        fprintf( fpt,"NON," );
      }
    }
  }

  if( fclose( fpt ) ) exit( -10 );
  if( '/'==test_result.first_cat ) return( PASS );
  else                             return( test_result.first_cat );

}


extern int DispLogic( int mode ){
  int cnt, freq, size, work, d_ram_top;
  int greg66, greg76, greg77, etlr07_04;
  char time_buf2[32], dump_filename[256];
  etlr07_04 = 0;

  if( NULL!=strstr( exe_test.pat_file, "oco_monitor") ){
    d_ram_top = RAM_TOP + 0x3600;
    greg66 = ReadEcrMode8( RAM_TOP + 0xA0, SIZE_L ); greg76 = ReadEcrMode8( RAM_TOP + 0xA4, SIZE_L ); greg77 = ReadEcrMode8( RAM_TOP + 0xA8, SIZE_L ); etlr07_04 = ReadEcrMode8( RAM_TOP + 0xAC, SIZE_L );
    sprintf( outbuf, "\nETLR01_00=%04X,   SubOSC Trim=%02X,   BGRADJT=%02X,   BGRTRMV=%02X\n", ReadEcrMode8( d_ram_top + 0x48, SIZE_W ), ReadEcrMode8( d_ram_top + 0x3C, SIZE_B ), ReadEcrMode8( d_ram_top + 0x40, SIZE_B ), ReadEcrMode8( d_ram_top + 0x44, SIZE_B ) ); DataOut( Ffpt, outbuf );
    DataOut( Ffpt, "\n OCO meas \n" );
    DataOut( Ffpt,   "Name           ,Trim     ,count        ,Freq[Hz]\n" );
    sprintf( outbuf, "ILOCO 16KHz    ,%4X     ,%08X     ,%9d\n", ReadEcrMode8( d_ram_top + 0x28, SIZE_L ), ReadEcrMode8( d_ram_top + 0x14, SIZE_L ), ReadEcrMode8( d_ram_top + 0x00, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "LOCO 32.768KHz ,%4X     ,%08X     ,%9d\n", ReadEcrMode8( d_ram_top + 0x2C, SIZE_L ), ReadEcrMode8( d_ram_top + 0x18, SIZE_L ), ReadEcrMode8( d_ram_top + 0x04, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "MOCO 2MHz      ,%4X     ,%08X     ,%9d\n", ReadEcrMode8( d_ram_top + 0x30, SIZE_L ), ReadEcrMode8( d_ram_top + 0x1C, SIZE_L ), ReadEcrMode8( d_ram_top + 0x08, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "HOCO 48MHz     ,%8X ,%08X     ,%9d\n",     ReadEcrMode8( d_ram_top + 0x34, SIZE_L ), ReadEcrMode8( d_ram_top + 0x20, SIZE_L ), ReadEcrMode8( d_ram_top + 0x0C, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "HOCO 64MHz     ,%8X ,%08X     ,%9d\n",     ReadEcrMode8( d_ram_top + 0x38, SIZE_L ), ReadEcrMode8( d_ram_top + 0x24, SIZE_L ), ReadEcrMode8( d_ram_top + 0x10, SIZE_L ) ); DataOut( Ffpt, outbuf );
#if ( PRODUCT_NAME==PRODUCT_IO )
    sprintf( outbuf, "HVOCO 2MHz     ,%4X     ,%08X     ,%9d\n", ReadEcrMode8( d_ram_top + 0x54, SIZE_L ), ReadEcrMode8( d_ram_top + 0x50, SIZE_L ), ReadEcrMode8( d_ram_top + 0x4C, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "WT1 KT32 48MHz ,%8X ,%08X     ,%9d\n",     ReadEcrMode8( d_ram_top + 0x60, SIZE_L ), ReadEcrMode8( d_ram_top + 0x5C, SIZE_L ), ReadEcrMode8( d_ram_top + 0x58, SIZE_L ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "WT1 KT32 64MHz ,%8X ,%08X     ,%9d\n",     ReadEcrMode8( d_ram_top + 0x6C, SIZE_L ), ReadEcrMode8( d_ram_top + 0x68, SIZE_L ), ReadEcrMode8( d_ram_top + 0x64, SIZE_L ) ); DataOut( Ffpt, outbuf );
#endif
    DeviceLevelsPowerDown();
  }else if( NULL!=strstr( exe_test.pat_file, "monitor_faci") ){
    DataOut( Ffpt,   "\nFACI   ,before    ,after" );
    sprintf( outbuf, "\nFPCKAR 0x407FE08C ,    H'%04X,    H'%04X", ReadEcrMode8( RAM_TOP + 0x88, SIZE_W ), ReadEcrMode8( RAM_TOP + 0x8C, SIZE_W ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "\nFPCKAR 0x407FE0E4 ,    H'%04X,    H'%04X", ReadEcrMode8( RAM_TOP + 0x98, SIZE_W ), ReadEcrMode8( RAM_TOP + 0x9C, SIZE_W ) ); DataOut( Ffpt, outbuf );
    sprintf( outbuf, "\nGREG098 ,H'%08X,H'%08X", ReadEcrMode8( RAM_TOP + 0x90, SIZE_L ), ReadEcrMode8( RAM_TOP + 0x94, SIZE_L ) ); DataOut( Ffpt, outbuf );
    for( cnt=0; cnt<128; cnt++ ){
      sprintf( outbuf, "\nGREG%03d ,H'%08X,H'%08X", cnt, ReadEcrMode8( RAM_TOP + 0x2000 + cnt*4, SIZE_L ), ReadEcrMode8( RAM_TOP + 0x2200 + cnt*4, SIZE_L ) ); DataOut( Ffpt, outbuf );
    }
    DataOut( Ffpt, "\n" );
  }else if( NULL!=strstr( exe_test.pat_file, "read_dump") ){
    time( &time_list2 );
    GetDateTime( &time_list2, time_buf2, 1 );
    work = ReadEcrMode8( RAM_TOP + 0x50, SIZE_L ) & 0x0F;
    sprintf( dump_filename, "%s_L%s_W%s_x%03d_y%03d_Info%d_%s_%02d%02d%02d_%d.bin", devicename, lotname, wafername, xy_location[0], xy_location[1], work, &info_extra[work][0], thour, tmin, tsec, get_site_number() );
    work = ReadEcrMode8( RAM_TOP + 0x58, SIZE_L ) -  ReadEcrMode8( RAM_TOP + 0x54, SIZE_L );
    FbmDataOut_BIST( RAM_TOP + 0x8000, work, dump_filename );
  }else if( NULL!=strstr( exe_test.pat_file, "bgr_monitor") ){
    if( CheckFK(FK_DISP_DEBUG) ){ sprintf( outbuf, "\nVCL1 = %5d [mV],  VCL2 = %5d [mV],  DATAI = %5d [mV]\n", test_result.monitor_1, test_result.monitor_2, 123 ); DataOut( Ffpt, outbuf ); }
  }else if( NULL!=strstr( exe_test.pat_file, "bgr2_trim") ||  NULL!=strstr( exe_test.pat_file, "bgr3_trim") ){
    if( CheckFK(FK_DISP_DEBUG) ){
      DataOut( Ffpt, "\nAddress  ,Data\n" );
      for( cnt=RAM_TOP + 0x80; cnt<RAM_TOP + 0x100; cnt+=4 ){
        sprintf( outbuf, "%08X ,%08X\n", cnt, ReadEcrMode8( cnt, SIZE_L ) ); DataOut( Ffpt, outbuf );
      }
    }
  }else{
  }

 return( 0 );

}

extern int BIST_Start_Setup( int prob_exec ){
  static int   first_flag = 0;
  char  time_buf[32];
  time_t  first_time;
  FILE_T  *fpt;
  int cnt;

  if( first_flag==0 ){ time( &first_time ); first_flag = 1; }
  GetDateTime( &first_time, time_buf, 0 );


  /* flow & integ info */
  switch( prob_exec&0x00FF ){
  case BWT_FL0&0x00FF: exe_flow = wt0_flow; exe_flow_size = sizeof(wt0_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT0; integ_table_size = sizeof(integ_WT0)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BWT_FL1&0x00FF: exe_flow = wt1_flow; exe_flow_size = sizeof(wt1_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT1; integ_table_size = sizeof(integ_WT1)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BWT_FL2&0x00FF: exe_flow = wt2_flow; exe_flow_size = sizeof(wt2_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT2; integ_table_size = sizeof(integ_WT2)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BWT_FL4&0x00FF: exe_flow = wt4_flow; exe_flow_size = sizeof(wt4_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT4; integ_table_size = sizeof(integ_WT4)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BWT_FL8&0x00FF: exe_flow = wt8_flow; exe_flow_size = sizeof(wt8_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT8; integ_table_size = sizeof(integ_WT8)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BWT_FL9&0x00FF: exe_flow = wt9_flow; exe_flow_size = sizeof(wt9_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT9; integ_table_size = sizeof(integ_WT9)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BFT_FL1&0x00FF: exe_flow = ft1_flow; exe_flow_size = sizeof(ft1_flow)/sizeof(FLOW_COND_ST); integ_table = integ_FT1; integ_table_size = sizeof(integ_FT1)/sizeof(INTEGRATOR_PARAM_ST); break;
  case BFT_FL2&0x00FF: exe_flow = ft2_flow; exe_flow_size = sizeof(ft2_flow)/sizeof(FLOW_COND_ST); integ_table = integ_FT2; integ_table_size = sizeof(integ_FT2)/sizeof(INTEGRATOR_PARAM_ST); break;
  default:             exe_flow = wt1_flow; exe_flow_size = sizeof(wt1_flow)/sizeof(FLOW_COND_ST); integ_table = integ_WT1; integ_table_size = sizeof(integ_WT1)/sizeof(INTEGRATOR_PARAM_ST); break;
  }

  /* Result Integ Init */
  for( cnt=0; cnt<integ_table_size; cnt++ ){
    ( integ_table + cnt )->value = 0;
    ( integ_table + cnt )->exe_flag = 0;
  }
  test_result.first_cat = test_result.fail_cat = 0;
  test_result.first_tno = test_result.fail_tno = test_result.judge_kind = test_result.monitor_1 = test_result.monitor_2 = test_result.test_time =  test_result.boot_time = 0;
  test_result.ram_out[0] = test_result.ram_out[1] = test_result.ram_out[2] = test_result.ram_out[3] = 0;

  /* debug log file */
  if( CheckFK(FK_DLOG_DEBUG) ){
    Ffpt = 2;
    if( prob_exec&0xF0 ){
      sprintf( logfile, "%s%sBIST_FT%d_RESULTR_%04d%02d%02d_%d.csv", glob_cdp, glob_probe, prob_exec&0x0F, tyear, tmon, tday, get_site_number() );
    }else{
      sprintf( logfile, "%s%sBIST_WT%d_RESULTR_%04d%02d%02d_%d.csv", glob_cdp, glob_probe, prob_exec&0x0F, tyear, tmon, tday, get_site_number() );
    }
    if( NULL==( Fptdata = fopen( logfile, "at" ) ) ){ ErrorPrint( logfile ); return( FAIL ); }
  }else{
    Ffpt = 1;
  }

  /* integ log file */
  if( prob_exec&0xF0 ){
    sprintf( logfile, "%s%s%s_L%s_W%s_FT%d_INTEG_%d.csv", glob_cdp, glob_probe, DEVICENAME, lotname, wafername, prob_exec&0x0F, get_site_number() );
  }else{
    sprintf( logfile, "%s%s%s_L%s_W%s_WT%d_INTEG_%d.csv", glob_cdp, glob_probe, DEVICENAME, lotname, wafername, prob_exec&0x0F, get_site_number() );
  }
  if( ( fpt = fopen( logfile, "rt" ) )==NULL ){     /* 1st Chip ? */
    if( NULL==( fpt = fopen( logfile, "at" ) ) ){ ErrorPrint( logfile ); return( FAIL ); }
    /*---- integ header ------------------------------------------------------------------*/
    fprintf( fpt, "#VERSION,0x2000,QCDATA\n" );     /* CSV�t�@�C���o�[�W������            */
    fprintf( fpt, "#FILE_TYPE,Normal\n" );          /* CSV�t�@�C���^�C�v                  */
    fprintf( fpt, "#FILE_SUM,1\n" );                /* QC�f�[�^�t�@�C����                 */
    fprintf( fpt, "#FILE_NO,1\n" );                 /* �擪�L�[���[�h                     */
    fprintf( fpt, "#FILE_HEAD\n" );                 /* �t�@�C���w�b�_                     */
    fprintf( fpt, "##ANALOG_DATA,2.02\n" );         /* �R�����g                           */
    fprintf( fpt, "QC_VERSION,0x5020\n" );          /* BIN�t�H�[�}�b�g�o�[�W����          */
    fprintf( fpt, "F_TYPE,WAFER\n" );               /* �t�@�C�����                       */
    fprintf( fpt, "#COMMON_HEAD\n" );               /* ���ʃw�b�_                         */
    fprintf( fpt, "TESTER_NAME,VERSA\n" );          /* �e�X�^����                         */
    fprintf( fpt, "TCP_NAME,VERSA\n" );             /* TCP��                              */
    fprintf( fpt, "ST_NO,1\n" );                    /* �X�e�[�V����No                     */
    fprintf( fpt, "FACTORY,MUSASHI\n" );            /* �H�ꖼ                             */
    fprintf( fpt, "CHIP_NAME,%s\n", DEVICENAME );   /* �i�햼                             */
    fprintf( fpt, "KEY_NO,%s\n", lotname );         /* KEY No                             */
    fprintf( fpt, "LOT_NO,%s\n", lotname );         /* LOT No                             */
    fprintf( fpt, "PROCESS_CODE" );                 /* �H���R�[�h                         */
    if( prob_exec&0xF0 ) fprintf( fpt, " FT" );
    fprintf( fpt, ",%d\n", prob_exec&0x0F );
    fprintf( fpt, "TESTPRO_NAME,%s\n",PROGRAMNAME );/* �e�X�g�v���O������                 */
    fprintf( fpt, "ROM_NO,\n" );                    /* ROM No                             */
    fprintf( fpt, "PROBE_CARD, OD=35\n" );          /* �v���[�u�J�[�h                     */
    fprintf( fpt, "OPERATOR_NAME,SUGAWARA\n" );     /* Operation��                        */
    fprintf( fpt, "HOSTNAME,\n" );                  /* �v���C���e�O���[�^��             */
    fprintf( fpt, "TIME,%d/%d/%d %02d:%02d:%02d\n", tyear, tmon, tday, thour, tmin, tsec );
    fprintf( fpt, "LOTSTART_TIME,%d/%d/%d %02d:%02d:%02d\n", tyear, tmon, tday, thour, tmin, tsec );
    fprintf( fpt, "START_TIME,%d/%d/%d %02d:%02d:%02d\n", tyear, tmon, tday, thour, tmin, tsec );
    fprintf( fpt, "END_TIME,%d/%d/%d %02d:%02d:%02d\n", tyear, tmon, tday, thour, tmin, tsec );
    fprintf( fpt, "PASS_CHIP_NUM,dummy\nFAIL_CHIP_NUM,dummy\n" );
    fprintf( fpt, "WAFER_NUM,%s\n", wafername );
    fprintf( fpt, "SLOT,WAFERID\n01,%s%s", lotname, wafername );
    fprintf( fpt, "MODEL_VERSION,FILE,0x5010\nTHEORETICAL_NUM,FILE,3568\nCATE_MAX,FILE,255\nCATE1_MAX,FILE,127\nCATE2_MAX,FILE,255\n" ); /**/
    fprintf( fpt, "FACOMMENT,FILE,%s\nTESTNUM,FILE,190\n", baselog );
    fprintf( fpt, "TSNO,TEST-ID,COMMENT,MODE,LOLIMIT,HILIMIT\n");
    for( cnt=0; cnt<integ_table_size; cnt++ ){
      fprintf( fpt, "%s,%4d,%s,%s,%s,%s\n", ( integ_table + cnt )->t_no,
                                            ( integ_table + cnt )->id,
                                            ( integ_table + cnt )->name,
                                            ( integ_table + cnt )->mode,
                                            ( integ_table + cnt )->lolimit,
                                            ( integ_table + cnt )->hilimit );
    }
    fprintf( fpt, "TRANS_FA,FILE,OFF,,,\nTRANS_YMS,FILE,OFF,,,\n" );         
    fprintf( fpt, "WAFER_ANGLE,FILE,180,,,\n" );                             
    fprintf( fpt, "BASE_X,FILE,100,,,\nBASE_Y,FILE,100,,,\n" );              
    fprintf( fpt, "REFERENCE_X,FILE,0\nREFERENCE_Y,FILE,0\n" );              
    fprintf( fpt, "CHIPSIZE_X,FILE,5090\nCHIPSIZE_Y,FILE,5090\n" );          
    fprintf( fpt, "WAFER_SIZE,FILE,12\nMULTI,FILE,64\n" );                   
    fprintf( fpt, "RELATE_X,RELATE_Y,\n" );                                  
    fprintf( fpt, "0,0,\n0,-1,\n0,-2,\n0,-3,\n0,-4,\n0,-5,\n0,-6,\n0,-7,\n");
    fprintf( fpt, "1,0,\n1,-1,\n1,-2,\n1,-3,\n1,-4,\n1,-5,\n1,-6,\n1,-7,\n");
    fprintf( fpt, "2,0,\n2,-2,\n2,-2,\n2,-3,\n2,-4,\n2,-5,\n2,-6,\n2,-7,\n");
    fprintf( fpt, "3,0,\n3,-3,\n3,-3,\n3,-3,\n3,-4,\n3,-5,\n3,-6,\n3,-7,\n");
    fprintf( fpt, "WT_FT,FILE,WT\n#OWN_HEAD\n" );                            
    fprintf( fpt, "X_MIN,7\nY_MIN,8\nCHIPNUM,1124\n" );                      
    fprintf( fpt, "X,Y,INDEX,DUT,G/N,C1,C1_MARK,C2,C2_MARK,FT,ET,TSTART,TEND" );
    for( cnt=0; cnt<integ_table_size; cnt++ ){
      fprintf( fpt, ",%s", ( integ_table + cnt )->t_no );
    } 
  }
  if( fclose( fpt ) ) exit(-10);
  return( PASS );

}


extern int BIST_TestExe_Check( int prob_exec ){
  if( 0==strcmp( exe_test.timming, "" ) ) return( FAIL );
  if( 0!=exe_test.fk_on && 0==( exe_test.fk_on&fc_key ) ) return( FAIL );
  if( 0!=exe_test.fk_off && ( exe_test.fk_off&fc_key ) ) return( FAIL );
  return( PASS );

}

extern int BIST_Integ_Store( void ){
  int i, cnt;
  for( cnt = 0; cnt<integ_table_size; cnt++ ){
    if( ( integ_table + cnt )->testno != exe_test.tno ) continue;
    ( integ_table + cnt )->exe_flag = ON;
    if( ( integ_table + cnt )->address == 0 ){
      if( ( integ_table + cnt )->calculation==0x02 ) ( integ_table + cnt )->value = test_result.monitor_2;
      else                                           ( integ_table + cnt )->value = test_result.monitor_1;
    }else{
      ( integ_table + cnt )->value = ReadEcrMode8( ( integ_table + cnt )->address, ( integ_table + cnt )->size );
    }
  }

}

extern int BIST_DC_RAM_Judge( void ){
  int i, cnt, p_limit, e_limit, l_limit;
  e_limit = p_limit = 0;
  for( cnt = 0; cnt<integ_table_size; cnt++ ){
    if( ( integ_table + cnt )->id != 19201 ) e_limit = ( integ_table + cnt )->value;
    if( ( integ_table + cnt )->id != 19202 ) p_limit = ( integ_table + cnt )->value;
  }
  if( exe_test.tno==19300 ){
    l_limit = ( p_limit*10000 - ( 211 * p_limit ) ) / 10000;
    if( test_result.monitor_1<l_limit ) return( 0x00001000 );
  }
  if( exe_test.tno==19600 ){
    l_limit = ( e_limit*10000 - ( 317 * e_limit ) ) / 10000;
    if( test_result.monitor_1<l_limit ) return( 0x00001000 );
  }
  return( PASS );

}

