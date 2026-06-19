 // ****************************************************************************** FILENAME    RX200man.h  CONTENTS: This is a 128KB & 256KB  flash application program for the V3300 test system  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED  2008/11/01  rev.00  1'st created by H.Kobori *****************************************************************************
#ifndef MAN_H
#define MAN_H

 // ****************************************************************************** Difine Include fine *****************************************************************************

#include <ppscon.h>
#include "RC04EX_def.h"

#ifdef V5KONLY
#include <atomic.h>
#include <vscdef.h>
#include <shell2b.h>
#include <VK_hw.h>
#endif
#ifdef V4KONLY
#include <atomic.h>
#include <vscdef.h>
#include <shell2a.h>
#include <V4400HW.h>
#endif

//#define Byte unsigned char
//#define Lword unsigned long
typedef unsigned char  Byte;
typedef unsigned short Word;
typedef unsigned long  Lword;

#include "RC04EX_iif.h"
#include "RC04EX_oif.h"
#define TABLE_END OUT_LWORDEND+1

#ifndef NUL
//V4000
#define NUL '\0'
//V4000
#endif
// NUL


// ***********************************
// TestModeParameter
// ***********************************
enum testmode_control{
  RELEASEMODE,
  DEBUGMODE,
  SCIDEBUGMODE
};


// ***********************************
// Tester Interface Table
// ***********************************
struct interface_t {
    int     addr;
    int     size;
    int     value;
    int     wk;
};
typedef struct interface_t interface_t;

// ***********************************
// FIND_T Structure
// ***********************************
struct find_t {
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_data;
    long size;
    char name[14];
} __attribute__((packed));
typedef struct find_t find_t;

 // ****************************************************************************** Define external value *****************************************************************************

extern mode_table_t optBuf;

extern FILE_T *Fptdata;
// for datalog file
extern int  Ffpt;
// print&disp=2 disp=1 noout=0
extern char *program_name;
extern char *val_devtype;
extern char *Tester_name;
extern char *glob_cdp;
extern char *glob_fbm;
extern char *glob_probe;
extern char *glob_binpat_at;
extern char *glob_binpat_wt;
extern char *glob_binpat_logic;
extern char *glob_binpat_boot;
extern char *glob_binpat_bist;
extern char *glob_datalog;
extern char *glob_initlog;
extern char *glob_currentlog;
extern char *glob_bunpu;
extern char *glob_cond;
extern char *glob_retention;
extern char *glob_Extralog;

extern char lotname[32];
extern char wafername[32];
extern char devicename[32];
extern char temp_comment[32];
extern int test;

extern time_t time_list;
extern time_t time_list2;
extern int tyear,tmon,tday,thour,tmin,tsec;
extern char datetime_string[15];

extern int Cputotal;
// Cpu mode total test time
extern int Cpuferr;

extern int configration;
extern char configstate[16];

extern char binpat[256];
extern char datapat[256];
extern char baselog[256];
extern char filenamebuf[128];
extern char outbuf[256];
extern int ebrbitmask1[10];
extern int ebrbitmask2[10];
extern int bitmask0[8];
extern int bitmask1[8];
extern int bitmask[];
extern int REJ_BIN_LIST[];

extern int glb_temp;
extern int glob_pmode_flp;
extern int glob_pmode_e2p;

extern interface_t interface_store_buff[];

extern int Extra5_ETLR00[BGR_ARRAY_VCC];
extern int Extra5_ETLR01[BGR_ARRAY_VCC];
extern int Extra5_TempCheck;
extern int SampleNo;


 // *********************************************************************** External function common ***********************************************************************

extern int  AnalisysTestProgram(interface_t*,int);
extern int  AnalyzeCommonCommand(int , interface_t *, int* , int*[]);
extern int  DisturbSelect(int *,int ,interface_t* );
extern int  FBMSelect(int *,int ,interface_t* );
extern int  RetentionSelect(int *,int ,interface_t* );
extern int  Cell(int *,int ,interface_t* );
extern int  Cell2(int *,int ,interface_t* );


extern int  WaferTestProgram(int);

 // *********************************************************************** External function V2000 common ***********************************************************************
extern int  UserExCommand(int,char*[]);
extern void DisplayMenu(void);
extern void DisplayHelp(void);

 // *********************************************************************** Write Debug Data file define ***********************************************************************
extern char dataout_logfname[];
// defalut \DATALOG\DATA.LOG_%d.csv


#define V4V5K_TIME_OFS  16*60*60
// Add Second of 16Hour (From U.S.)

extern int fc_key;
extern int shot_number;

 // *********************************************************************** DAXS TRANS CONDITION ***********************************************************************
typedef struct {
  int vcc;
  int vrefh;
  int vref;
  int write_rate;
  int read_rate;
  int voh;
  int vol;
  int vtt;
} DAXS_TRANS_COND_T;
extern DAXS_TRANS_COND_T daxs_trans;

// --------------------------
// ---- Wmat Information ----
// --------------------------
#define PRODUCT_INFO_SIZE        16
#define LOTNAMAE_SIZE            10
#define CORDINATE_SIZE            2
#define FLI_FAILPOINT_SIZE       16
// ő128|Cg(4KBubN~128512KB܂őΉ)

// ! tFCӏ
typedef struct{
  Word contents;
  // !< FAILe
#ifdef V5KONLY
  Byte area:4;
  // !< FAIL̈
  Byte vdd:4;
  // !< FAILVDD
#endif
#ifdef V4KONLY
  Byte vdd:4;
  // !< FAILVDD
  Byte area:4;
  // !< FAIL̈
#endif
  Byte mat;
  // !< FAIL}bg
} FailPoint;

// ! tFC
typedef struct{
  FailPoint failpoint;
  // !< tFC|Cg
  void *failaddress;
  // !< tFCAhX
} FailInfo;

// ! g~OWX^\̐錾
typedef struct{
  Byte  reg00;
  Byte  reg01;
  Byte  reg02;
  Byte  reg03;
  Byte  reg04;
  Byte  reg05;
  Byte  reg06;
  Byte  reg07;
  Byte  reg08;
  Byte  reg09;
  Byte  reg10;
  Byte  reg11;
  Byte  reg12;
  Byte  reg13;
  Byte  reg14;
  Byte  reg15;
  Byte  reg16;
  Byte  reg17;
  Byte  reg18;
  Byte  reg19;
  Byte  reg20;
  Byte  reg21;
  Byte  reg22;
  Byte  reg23;
  Byte  reg24;
  Byte  reg25;
  Byte  reg26;
  Byte  reg27;
  Byte  reg28;
  Byte  reg29;
  Byte  reg30;
  Byte  reg31;
  Byte  reg32;
  Byte  reg33;
  Byte  reg34;
  Byte  reg35;
  Byte  reg36;
  Byte  reg37;
  Byte  reg38;
  Byte  reg39;
} TrimReg;

//                                      // ! ~σWX^\̐錾
//typedef struct{
//  Byte reg00;                         // !< REG00
//  Byte reg01;                         // !< REG01
//} RedReg;

// ! VthT[`ʍ\̐錾
typedef struct{
  Byte centereven;
  // !< EvenWL Vth50%T[`
  Byte centerodd;
  // !< OddWL  Vth50%T[`
  Byte low;
  // !< Vth_LowxT[`
  Byte high;
  // !< Vth_HighxT[`
} VthiSearch;

// ! VthT[`(WT1)\̐錾
typedef struct{
  Byte beforedisturb;
  // !< WT1fBX^[uOVthxT[`
  Byte afterdisturb;
  // !< WT1fBX^[uVthxT[`
  Byte beforeretention10ua;
  // !< WT1eVO(10uA)Vth_LowxT[`
  Byte beforeretention3ua;
  // !< WT1eVO(3uA)Vth_LowxT[`
} VthSearchWT1;

// ! ݏ\̐錾
typedef struct{
  Byte max;
  // !< ől
  Word ave;
  // !< ϒl
} PEChara;

// ! VthT[`(WT2)\̐錾
typedef struct{
  Byte afterretention10ua;
  // !< WT2eV(10uA)Vth_LowxT[`
  Byte afterretention3ua;
  // !< WT2eV(3uA)Vth_LowxT[`
  Byte beforedisturb;
  // !< WT2fBX^[uOVthxT[`
  Byte afterdisturb;
  // !< WT2fBX^[uVthxT[`
  Byte afterdisturba;
  // !< WT2fBX^[uAVthxT[`
} VthSearchWT2;

// ! \̐錾
typedef struct{
  VthiSearch vthisearch;
  // !< WT1 VthT[`
  VthSearchWT1 vthsearchwt1;
  // !< WT1 VthT[`
  PEChara pcharastripeawt1;
  // !< WT1 XgCvAݏ
  PEChara pcharastripebwt1;
  // !< WT1 XgCvBݏ
  PEChara pcharaall1wt1;
  // !< WT1 ALL1ݏ(FLD)
  Byte    pad0;
  // !< Padding
  PEChara pcharadiagawt1;
  // !< WT1 _CAOAݏ
  PEChara pcharachkawt1;
  // !< WT1 `FbJ[Aݏ
  PEChara echarainitialwt1;
  // !< WT1
  Word echaratimetrimwt1_max;
  // !< WT1 ԃg~Oől
  Word echaratimetrimwt1_ave;
  // !< WT1 ԃg~Oϒl
  Byte    pad1;
  // !< Padding
  PEChara echaralowvoltwt1;
  // !< WT1 d
  Word echaraallselwt1_max;
  // !< WT1 MGSIől
  Word echaraallselwt1_ave;
  // !< WT1 MGSIϒl
  Byte    pad2;
  // !< Padding
  PEChara echarajudgewt1;
  // !< WT1 ϒl`FbN
  Byte    pad3[ 5 ];
  // !< Padding
  VthSearchWT2 vthsearchwt2;
  // !< WT2 VthT[`
  Byte    pad4[ 3 ];
  // !< Padding
  PEChara pcharaall0wt2;
  // !< WT2 ALL0ݏ
  PEChara pcharalowvoltwt2;
  // !< WT2 dALL0ݏ
  Byte    pad5[ 2 ];
  // !< Padding
  PEChara pcharachkbwt2;
  // !< WT2 `FbJ[Bݏ
  PEChara echarainitialwt2;
  // !< WT2
  Byte    pad6[ 2 ];
  // !< Padding
  PEChara echaralowvoltwt2;
  // !< WT2 d
  PEChara echarajudgewt2;
  // !< WT2 ϒl`FbN
  Byte    pad7[ 2 ];
  // !< Padding
  PEChara echaralastwt2;
  // !< WT2 ŏI
  Byte    pad8[ 29 ];
  // !< Padding
} Character;

// ! [N}bg\̐錾
typedef struct{
  Byte productinfo[ PRODUCT_INFO_SIZE ];
  // !< foCX
  Byte lotname[ LOTNAMAE_SIZE ];
  // !< bg
  Byte wafernumber;
  // !< EFnԍ
  Byte pad0;
  // !< Padding
  Word processcode;
  // !< HR[h
  Word footholdcode;
  // !< _R[h
  Word xchipdirection;
  // !< X`bvW
  Word ychipdirection;
  // !< Y`bvW
  Byte umathistory;
  // !< USBt@[
  Byte programversion;
  // !< vOo[W
  Byte testversion;
  // !< TESTo[W
  Byte flowversion;
  // !< FLOWo[W
  Byte passcode[ 4 ];
  // !< pXR[h
  Byte ptimetrimfli;
  // !< ԃg~OFLI(Pro)Vth50%l
  Byte ptimetrimfld;
  // !< ԃg~OFLD(Pro)Vth50%l
  Byte etimetrimfli;
  // !< ԃg~OFLI(Ers)Vth50%l
  Byte etimetrimfld;
  // !< ԃg~OFLD(Ers)Vth50%l
  FailInfo failinfo;
  // !< tFC
  Byte reliefcategory;
  // !< ~σJeS
  Byte reliefcount;
  // !< ~ϐ
  Byte pad1[ 6 ];
  // ! Padding
  Byte failpoint_fli0[ FLI_FAILPOINT_SIZE ];
  // !< FLP ~σtFC|Cg
  Byte failpoint_fli1[ FLI_FAILPOINT_SIZE ];
  // !< FLI1~σtFC|Cg
  Byte failpoint_fli2[ FLI_FAILPOINT_SIZE ];
  // !< FLI2~σtFC|Cg
  Byte failpoint_fli3[ FLI_FAILPOINT_SIZE ];
  // !< FLI3~σtFC|Cg
  Lword chipidcrccode;
  // !< CHIPIDCRC32f[^
  Byte pad2[ 4 ];
  // ! Padding
  TrimReg trimreg;
  // !< g~OWX^l
  Byte pad3[ 6 ];
  // !< Padding
  Character flp;
  // !< FLP
  Character e2p;
  // !< E2P
  Lword rosc_logic;
  // !< WbNpIV[^
  Lword rosc_chgpump;
  // !< `[W|vpIV[^
  Lword rosc_subact;
  // !< TuANgpIV[^
  Byte pad4[ 84 ];
  // !< Padding ݃TCYE
} WmatStructure;

#endif
