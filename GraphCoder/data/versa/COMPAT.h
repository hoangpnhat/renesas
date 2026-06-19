 // ****************************************************************************** FILENAME COMPAT.h  CONTENTS: This file is used for compatibility for V3k to V5k test program.  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.  1998/08/19  rev.1.0  Created by Ken-K *****************************************************************************

#ifndef _COMPAT_H_
#define _COMPAT_H_

// ****************************************************************************
// BM/Ecr Function
// ****************************************************************************

extern void comp_set_bmecr_8bit(void);
extern void comp_set_bmecr_16bit(void);
extern int comp_set_bm_address_mask(int mode, int sel, int jam);
extern int comp_set_ecr_address_mask(int mode, int sel, int jam);
extern void comp_set_ecr_write_mask(int mask);
extern void comp_set_ecr_postdecode_mask(int mask);
extern int comp_set_ecr_mode(int mode);
extern int comp_load_bm(char *filename);
extern int comp_load_ecr(char *filename);
extern int comp_write_ecr_to_file(char *filename, int mode, int address, int length);
extern int comp_write_buf_to_file( char *filename, int mode, int address, int length );
extern void comp_copy_ecr_to_buf(int start_addr, int end_addr);
extern int comp_write_bm(int address, int data);
extern int comp_read_bm(int address);
extern void comp_fill_bm(int data);
extern void comp_fill_ecr(int data);


// ****************************************************************************
// PMU function
// ****************************************************************************

extern int comp_set_ilimit(int value);
extern int comp_set_iparh(int value);
extern int comp_set_cvpar(int value);

// ****************************************************************************
// PPS function
// ****************************************************************************

extern int comp_set_v1(int value);
extern int comp_set_v2(int value);
extern int comp_set_v3(int value);
extern int comp_set_v4(int value);

extern int comp_set_i1limit(int value);
extern int comp_set_i2limit(int value);
extern int comp_set_i3limit(int value);
extern int comp_set_i4limit(int value);

// ****************************************************************************
// lbreg function
// ****************************************************************************

extern void comp_write_lbreg(int value);
extern void comp_write_lbreg_pe(int value);

// ****************************************************************************
// Execute Menu function
// ****************************************************************************

extern int temp_global_var1;
extern void Exe_Menu_With_Var1(int var1_value);
extern int temp_global_var2;
extern void Exe_Menu_With_Var2(int var2_value);

// ****************************************************************************
// Timing Function
// ****************************************************************************

extern int comp_set_tsmap(int *pinlist, int gts, int dsts);
extern int comp_set_io_time_ns(int *pinlist, int lts, int io_str, int io_stop);

// ****************************************************************************
// System command
// ****************************************************************************

extern void comp_system(char* input_str);

// ****************************************************************************
// time command
// ****************************************************************************
#ifndef V4K5KTESTER
typedef int time_t;
#endif

extern time_t comp_time(time_t*);
extern char* comp_ctime(time_t *);
// ****************************************************************************
// test command
// ****************************************************************************

extern void comp_run_apg_delay( void* entry_point, int mode, int delay);

// ****************************************************************************
// Defined value for compatibility
// ****************************************************************************

#ifdef V4K5KTESTER
#define PAT_FILE_HEADER_SIZE    36

#else
#define PAT_FILE_HEADER_SIZE    7

#endif

#if defined(V5KONLY)
#define MAX_VOH_VALUE 6500
#define MIN_VOL_VALUE -500

//#elif defined(V4KONLY)
#else
#ifdef V4KONLY
#define MAX_VOH_VALUE 6250
#define MIN_VOL_VALUE -250

#else
#define MAX_VOH_VALUE 7000
#define MIN_VOL_VALUE -1000
#endif
#endif

#ifdef V5KONLY
// V5K can force up to 50 mA through PPS
// V4k can force up to 1 A through PPS
#define MA250       PPS_50mA_RNG
#define MA1000      PPS_50mA_RNG
#endif

#ifndef ADC_VMEAS
// ADC_VSENSE don't defined in V3300 header file
#define ADC_VMEAS   0x0d
#endif

#endif
// end for _COMPAT_H_
