 // ****************************************************************************** FILENAME COMPAT.c  CONTENTS: This file is used for compatibility for V3k to V5k test program.  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.  1998/08/19  rev.1.0  Created by Ken-K *****************************************************************************
// NG V3300 local func Variable init value
// NG V3300 goto label
// NG V3300 file name 8 char over
// NG V3300 string 256 over ?  program column 256 over ?
// NG V3300 memcpy
// NG V3300 entry_point = int
// NG V3300 pin list 32 pin max f723xAGR.h
// NG V3300 #ifdef  before // comment , space
// NG V3300/V5000 ? read_lbreg()
// NG V3300 scope lot of number of lines
// NG V5000/V4000/V3000 struct size & pointer & buffer Address access C object rule miss match
// ?? V3300 #elif defined(V4KONLY) warning
// NG V3300 struct element int[] array initializers error
// NG V4400 struct element int[3] array element 0,1,2 initializers can't [3]
// NG V3300 const prototype
// NG V3300 pointer initial NULL ??
// ?? V5000 file buffer big&fclose Write V4000 file buffer small no fclose write
// ?? V5000 no folder system error stop
// NG V4000 time_t
// NG V4400/V5000 array element int vs[3] use 0,1,2 can't use [3]
// ?? V4000 big endian , V5000 little endian
// ?? V4000 bit field order right , V5000 order left
//PPS          V3300                               V4000               V5000 PPS12         V5000 PPS12E
//VS1          -10(-12)-10(+12)v   1A,500mA,50mA   0-8v        500mA   0-8v       200mA    0-12v      400mA
//VS2          0-15v               1A              0-15v       500mA   0-8v       200mA    0-12v      400mA
//VS3          -10(-12)-10(+12)v   1A,500mA,50mA   0-8v        500mA   0-8v       200mA    0-12v      400mA
//VS4          0-15v               1A              0-15v       500mA   0-8v       200mA    0-12v      400mA
//VS5                                              -13-2v      25mA    0-13v      100mA    0-13v      400mA
//VS6                                                                  0-13v      100mA    0-13v      400mA
//VS7                                                                  0-13v      100mA    0-13v      400mA
//VS8                                                                  0-13v      100mA    0-13v      400mA
//VS9                                                                  -10-5v     +-16mA   -10-12v    400mA
//VS10                                                                 -10-5v     +-16mA   -10-12v    400mA
//VS11                                                                 -10-5v     +-16mA   -10-12v    400mA
//VS12                                                                 -10-5v     +-16mA   -10-12v    400mA
//PMU          -12+12V ???       25mA  ???       -10v+15v  500mA   -12v+13v 50mA     -22v+22v 50mA
//PE other
//APG Vect     256K                                32K
//Cyc Max      102.4us                             204.8us
//Cyc Min      50ns                                10ns
//Edge         100ps                               19.5ps
//Vil          -16.75v                           -16v
//Vih          -0.757v                           -0.756.25v
//Min(Vih-Vil) 0.25v                               0.25v(?0.025mV?)
//Vhh          015v                              212v
//Vol          -0.57v                            -16v
//Voh          -0.57v                            -0.756.25v
//      V5000PPS12e / V5000PPS12 / V4000  Renesas_V5000_ZIFconnector_wiringRev2.0.xls
// VS1  0-12        / 0-8        / 0-8
// VS2  0-13        / 0-13       / 0-15
// VS3  0-12        / 0-8        / 0-8
// VS4  0-13        / 0-13       / 0-15
// VS5  -10-12      / -10-5      / -13-2
// VS6  -10-12      / -10-5      / NC
// PMU  -22-22      / -22-22     / -10-15
// VIH  -0.25-6.5                / -0.75-6.25
// VIL  -0.5-6.25                / -1-6
// VHH  1.5-13                   / 0-13
// VOH  -0.5-6.5                 / -0.75-6.25
// VOL  -0.5-6.5                 / -1-6

#ifndef _COMPAT_C_
#define _COMPAT_C_

#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "RC04EX.hpg"
#include "RC04EX_CGR.h"
#include "RC04EX_cyc.h"
#include "compat.h"


// ****************************************************************************
// BM/Ecr Function
// ****************************************************************************
void comp_set_bmecr_8bit( void ){
#ifdef V4K5KTESTER
  set_bmecr_data_width( bm1Memory,  8 );
  set_bmecr_data_width( ecr1Memory, 8 );
#else
  set_bmecr_8bit();
  // ECR/BM 8bit mode set
#endif
}


void comp_set_bmecr_16bit( void ){
#ifdef V4K5KTESTER
  set_bmecr_data_width( bm1Memory,  16 );
  set_bmecr_data_width( ecr1Memory, 16 );
#else
  set_bmecr_16bit();
  // ECR/BM 8bit mode set
#endif
}


int comp_set_bm_address_mask( int mode, int sel, int jam ){
  int status;
#ifdef V4K5KTESTER
  status = set_bmecr_address_mask( bm1Memory, sel, jam );
#else
  status=0;
  if( _32MB_BM_installed() ){
    status = set_bm_address_mask( mode, sel, jam );
  }
#endif
  return( status );
}


int comp_set_ecr_address_mask( int mode, int sel, int jam ){
  int status;
#ifdef V4K5KTESTER
  status = set_bmecr_address_mask( ecr1Memory, sel, jam );
#else
  status=0;
  if( _32MB_BM_installed() ){
    status = set_ecr_address_mask( mode, sel, jam );
  }
#endif
  return (status);
}


void comp_set_ecr_write_mask( int mask ){
#ifdef V4K5KTESTER
  set_bmecr_write_mask( ecr1Memory, mask );
#else
  set_ecr_write_mask( mask );
#endif
}


void comp_set_ecr_postdecode_mask( int mask ){
#ifdef V4K5KTESTER
  setup_analysis_group( 0, mask, 0xffff );
#else
  set_ecr_postdecode_mask( mask );
#endif
}


int comp_set_ecr_mode( int mode ){
  int status;
  status = PASS;
#ifdef V4K5KTESTER
  if      ( mode==ECR_DISAB ){
    printf("bmecr setting inside Ecrsetup8");
    status = set_bmecr_mode( ecr1Memory, ECR_MODE_DISABLE );
  }else if( mode==ECR_OVERW ){
    status = set_bmecr_mode( ecr1Memory, ECR_MODE_OVERWRITE );
  }else if( mode==ECR_OVERL ){
    status = set_bmecr_mode( ecr1Memory, ECR_MODE_OVERLAY );
  }
#else
  status = set_ecr_mode( mode );
#endif
  return( status );
}


int comp_load_bm( char *filename ){
  int status;
#ifdef V4K5KTESTER
  status = load_bmecr(bm1Memory, filename);
#else
  status = load_bm(filename);
  // LEGACY COMMAND
#endif
  return (status);
}

int comp_load_ecr( char *filename ){
  int status;
#ifdef V4K5KTESTER
  status = load_bmecr(ecr1Memory, filename);
#else
  status = load_ecr(filename);
  // LEGACY COMMAND
#endif
  return (status);
}

int comp_write_ecr_to_file( char *filename, int mode, int address, int length){
  int status;
#ifdef V4K5KTESTER
  status = save_bmecr( ecr1Memory, filename, SAVE_ZIP_NONE|SAVE_NOHEADER, address, length );
#else
  status = write_ecr_to_file( filename, mode, address, length );
#endif
    //printf("FBM File Out -> %s",filename);
  return( status );
}


int comp_write_buf_to_file( char *filename, int mode, int address, int length ){
  int status;
  status=0;
#ifdef V4K5KTESTER
  status = save_bmecr( bm1Memory, filename, SAVE_ZIP_NONE|SAVE_NOHEADER, address, length );
#else
  status = write_buf_to_file( filename, mode, address, length );
#endif
  return( status );
}


void comp_copy_ecr_to_buf( int start_addr, int end_addr ){
  int data_size;
#if defined(V5KONLY)
  data_size = end_addr - start_addr + 1;
  copy_ecr_to_buf (start_addr, data_size);
#else
#endif
}


int comp_write_bm( int address, int data ){
  int status;
  status = 0;
#ifdef V4K5KTESTER
  status = write_bmecr( bm1Memory, address, data );
#else
  write_bm( address, data );
#endif
  return( status );
}


int comp_read_bm( int address ){
  int status;
  status = 0;
#ifdef V4K5KTESTER
  status = read_bmecr( bm1Memory, address );
#else
  read_bm( address );
#endif
  return( status );
}


void comp_fill_bm( int data ){
  char temp_buf[255];
#ifdef V4K5KTESTER
  fill_bmecr( bm1Memory, data );
#else
  sprintf( temp_buf, "fbm 0x%X", data );
  system( temp_buf );
#endif
}


void comp_fill_ecr( int data ){
  char temp_buf[255];
#ifdef V4K5KTESTER
  fill_bmecr( ecr1Memory, data );
#else
  sprintf( temp_buf, "fecr %X", data );
  system( temp_buf );
#endif
}


// ****************************************************************************
// PMU function
// ****************************************************************************
int comp_set_ilimit( int value){
  int status;
#ifdef V4K5KTESTER
  if( value >= 50 mA ) status = set_ilimit( 50 mA );
  else                 status = set_ilimit( value );
#else
  status = set_ilimit( value );
#endif
  return( status );
}


int comp_set_iparh( int value ){
  int status;
#ifdef V4K5KTESTER
  status = set_iparh( value );
  // select 256K_APG
#else
  if( slot_info[_PPS_SLOT].board_type==_PPS_PMU_BTYP ){
     status = set_ppiparh( value );
     // select 64K_APG
  }else{
     status = set_iparh( value );
     // select 256K_APG
  }
#endif
  return( status );
}


int comp_set_cvpar( int value ){
  int status;
#ifdef V4K5KTESTER
  status = set_cvpar( value );
  // select 256K_APG
#else
  if( slot_info[_PPS_SLOT].board_type==_PPS_PMU_BTYP ){
    status = set_ppvpar( value );
    // select 64K_APG
  }else{
    status = set_cvpar( value );
    // select 256K_APG
  }
#endif
  return( status );
}


// ****************************************************************************
// PPS function
// ****************************************************************************
// SITE_MASK�̗p�r�s��


int comp_set_v1( int value ){
  int status;
#ifdef V5KONLY
  SITE_MASK( 1, status = set_pps_resource( PPS_VCC_V, value ) );
#else
  status = set_v1( value );
#endif
  return( status );
}


int comp_set_v2( int value ){
  int status;
#ifdef V5KONLY
  status = PASS;
  // caution!!
#else
  status = PASS;
  // caution!!
#endif
  return( status );
}


int comp_set_v3( int value ){
  int status;
#ifdef V5KONLY
  SITE_MASK( 1, status = set_pps_resource( PPS_VPP_V, value ) );
#else
  status = set_v2( value );
#endif
  return( status );
}


int comp_set_v4( int value ){
  int status;
#ifdef USE_PPS1
  return 0;
  // * for PPS1 BOARD *
#endif
#ifdef V5KONLY
  status = PASS;
#else
  status = PASS;
#endif
  return( status );
}

int comp_set_v5( int value ){
  int status;
#ifdef V5KONLY
  status = PASS;
  SITE_MASK (1, status = set_pps_resource (PPS_VNN_V, value));
  // caution!!
#else
  status = set_v5( value );
  // caution!!
#endif
  return( status );
}

int comp_set_i1limit( int value ){
  int status;
#ifdef USE_PPS1
  return 0;
  // * for PPS1 BOARD *
#endif
#ifdef V5KONLY
  if( value > 200 mA ){
    SITE_MASK( 1, status = set_pps_resource( PPS_VCC_ICLAMP, PPS_VCC_CLAMP_400mA ) );
  }else{
    SITE_MASK( 1, status = set_pps_resource( PPS_VCC_ICLAMP, PPS_VCC_CLAMP_200mA ) );
  }
#else
  status = set_i1limit( value );
#endif
  return( status );
}


int comp_set_i2limit( int value ){
  int status;
#ifdef USE_PPS1
  return 0;
#endif
#ifdef V5KONLY
  status = PASS;
#else
  status = set_i2limit( value );
#endif
  return( status );
}

int comp_set_i3limit( int value ){
  int status;
#ifdef USE_PPS1
  return 0;
#endif
#ifdef V5KONLY
  SITE_MASK (1, status = set_pps_resource( PPS_VPP_ICLAMP, PPS_VPP_CLAMP_400mA ) );
#else
  status = set_i3limit( value );
#endif
  return( status );
}

int comp_set_i4limit( int value ){
  int status;
#ifdef USE_PPS1
  return 0;
#endif
#ifdef V5KONLY
  status = PASS;
#else
  status = set_i4limit( value );
#endif
  return( status );
}


// ****************************************************************************
// lbreg function
// ****************************************************************************
void comp_write_lbreg(int value){
#ifdef V5KONLY
  int inv_value;
  int lbbit[8];
  int clk_low_value;
  int clk_high_value;
  int i;
    // set cs low
  write_lbreg( 0xc );
  // /RESET: High,  /CS: High
  delay_timer( 1 usec_delay );
  write_lbreg( 0x8 );
  // /RESET: High,  /CS: Low
  inv_value = ~value;
  // Need inverte value
                                                              // Because V5K lbreg driver used inverted value
                                                              // Logic "1" is Relay ON. Logic "0" is Relay OFF
  for (i=7; i>=0; --i){
    lbbit[i] = (inv_value >> i) & 0x1;
    clk_low_value = 0x8 | (lbbit[i] << 1) | 0;
    clk_high_value = 0x8 | (lbbit[i] << 1) | 1;
    write_lbreg( clk_low_value );
    delay_timer( 5 usec_delay );
    write_lbreg( clk_high_value );
    delay_timer( 5 usec_delay );
  }
    // set cs high
  write_lbreg( 0xc );
  // /RESET: High,  /CS: High
  delay_timer( 1 usec_delay );
#else
  write_lbreg( value );
#endif
}


void comp_write_lbreg_pe( int value ){
  if( value & 0x01 )  add_vhh_mask( lb0_pin ); else remove_vhh_mask( lb0_pin );
  if( value & 0x02 )  add_vhh_mask( lb1_pin ); else remove_vhh_mask( lb1_pin );
  if( value & 0x04 )  add_vhh_mask( lb2_pin ); else remove_vhh_mask( lb2_pin );
  if( value & 0x08 )  add_vhh_mask( lb3_pin ); else remove_vhh_mask( lb3_pin );
}


// ****************************************************************************
// Execute Menu function
// ****************************************************************************

int temp_global_var1 = 0;

void Exe_Menu_With_Var1( int var1_value ){
#ifdef V4K5KTESTER
  int len;
  int status;
  mode_table_t optBuf;
  len = sizeof( optBuf );
#else
  char temp_buf[255];
#endif
#ifdef V4K5KTESTER
  var1 = temp_global_var1 = var1_value;
  DisplayMenu();
#else
  sprintf( temp_buf, "ex var1 %d", var1_value );
  system( temp_buf );
  system( "ex men" );
#endif
}


int temp_global_var2 = 0;

void Exe_Menu_With_Var2( int var2_value ){
#ifdef V4K5KTESTER
  int len;
  int status;
  mode_table_t optBuf;
  len = sizeof(optBuf);
#else
  char temp_buf[255];
#endif
#ifdef V4K5KTESTER
  var2 = temp_global_var2 = var2_value;
#else
  sprintf( temp_buf, "ex var2 %d", var2_value );
  system( temp_buf );
  system( "ex men" );
#endif
}


// ****************************************************************************
// Timing Function
// ****************************************************************************
int comp_set_tsmap( int *pinlist, int gts, int dsts ){
  int status;
#ifdef V4K5KTESTER
  status = set_tsmap( pinlist, gts, dsts );
#else
  status = set_drive_strobe_tsmap( pinlist, gts, dsts );
#endif
  return( status );
}


int comp_set_io_time_ns( int *pinlist, int lts, int io_str, int io_stop ){
  int status;
#ifdef V4K5KTESTER
  status = set_io_time_ns( pinlist, lts, io_str, io_stop );
#else
  status = set_iog_time_ns( pinlist, lts, io_str, io_stop );
#endif
  return(status);
}

// ****************************************************************************
// System command
// ****************************************************************************
void comp_system(char* input_str){
#if defined(V5KONLY)
  char temp_buf[256];
  char replace_str[270];
  char* find_str;
    // Initialize string
  strncpy( temp_buf, input_str, 256 );
  sprintf( replace_str, " " );
  temp_buf[255] = '\0';
  // Replace dbm,fbm, and mbm command. Because BM feature in V5k was replaced ECR. if you want to look true bm value, please type tdbm or tfbm.
  if ((find_str = strstr( temp_buf, "tdbm" )) != NULL){
      sprintf( replace_str, "dbm%s", find_str+4 );
  }else if ((find_str = strstr(temp_buf, "tfbm")) != NULL){
      sprintf( replace_str, "fbm%s", find_str+4 );
  }else if ((find_str = strstr(temp_buf, "tmbm")) != NULL){
      sprintf( replace_str, "mbm%s", find_str+4 );
  }else if ((find_str = strstr(temp_buf, "dbm")) != NULL){
      sprintf( replace_str, "decr%s", find_str+3 );
  }else if ((find_str = strstr(temp_buf, "fbm")) != NULL){
      sprintf( replace_str, "decr%s", find_str+3 );
  }else if ((find_str = strstr(temp_buf, "mbm")) != NULL){
      sprintf( replace_str, "mecr%s", find_str+3 );
  }else{
      sprintf( replace_str, temp_buf );
  }
  set_site_mask( 1 );
  system( replace_str );
  set_site_mask( 0 );
#else
  system( input_str );
#endif
}


// ****************************************************************************
// test command
// ****************************************************************************
extern void comp_run_apg_delay( void* entry_point, int mode, int delay ){
#ifdef V4K5KTESTER
  run_apg_delay( (char *)entry_point, mode, delay);
#else
  run_apg_delay( (int)entry_point, mode, delay);
#endif
}


// ****************************************************************************
// time command
// ****************************************************************************
time_t comp_time( time_t *tp ){
  time_t timer;
  timer=time( (time_t *)NULL );
#ifdef V4K5KTESTER
  timer += V4V5K_TIME_OFS;
  // Add Second of 16Hour (From U.S.)
#endif
  if( tp!=NULL ){
    *tp=timer;
  }
  return timer;
}


char *comp_ctime( time_t *tp ){
#ifdef V4K5KTESTER
  int i;
  int flag;
  int len;
  char *top;
  char *current;
  top = ctime((CTIME_T*) tp);
  if(NULL==top){
    return(NULL);
  }
  current = top;
  len = strlen(top);
  flag = OFF;
  for( i=0; i<len; i++ ){
    if( isalpha( *(current+i) ) ){
      if( OFF==flag ){
        flag = ON;
      }else if( ON==flag ){
        *(current+i) = (char)tolower( *(current+i) );
      }
    }else{
      flag = OFF;
    }
  }
  return( top );
#else
  return( ctime(tp) );
#endif
}

#endif
// end for _COMPAT_C_
