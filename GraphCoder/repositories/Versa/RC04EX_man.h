/*******************************************************************************
 *  FILENAME    RX200man.h
 *
 *  CONTENTS: This is a 128KB & 256KB  flash application program
 *  for the V3300 test system
 *
 *  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED
 *
 *  2008/11/01  rev.00  1'st created by H.Kobori
 ******************************************************************************/
#ifndef MAN_H
#define MAN_H

/*******************************************************************************
 *  Difine Include fine
 ******************************************************************************/

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

#ifndef NUL               //V4000
#define NUL '\0'          //V4000
#endif  /* NUL */         //V4000


/*************************************/
/*  TestModeParameter                */
/*************************************/
enum testmode_control{
  RELEASEMODE,
  DEBUGMODE,
  SCIDEBUGMODE
};


/*************************************/
/*  Tester Interface Table           */
/*************************************/
struct interface_t {
    int     addr;
    int     size;
    int     value;
    int     wk;
};
typedef struct interface_t interface_t;

/*************************************/
/*  FIND_T Structure                 */
/*************************************/
struct find_t {
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_data;
    long size;
    char name[14];
} __attribute__((packed));
typedef struct find_t find_t;

/*******************************************************************************
 *  Define external value
 ******************************************************************************/

extern mode_table_t optBuf;

extern FILE_T *Fptdata;     /* for datalog file */
extern int  Ffpt;           /* print&disp=2 disp=1 noout=0 */
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

extern int Cputotal;    /* Cpu mode total test time */
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


/************************************************************************
 *  External function common
 ************************************************************************/

extern int  AnalisysTestProgram(interface_t*,int);
extern int  AnalyzeCommonCommand(int , interface_t *, int* , int*[]);
extern int  DisturbSelect(int *,int ,interface_t* );
extern int  FBMSelect(int *,int ,interface_t* );
extern int  RetentionSelect(int *,int ,interface_t* );
extern int  Cell(int *,int ,interface_t* );
extern int  Cell2(int *,int ,interface_t* );


extern int  WaferTestProgram(int);

/************************************************************************
 *  External function V2000 common
 ************************************************************************/
extern int  UserExCommand(int,char*[]);
extern void DisplayMenu(void);
extern void DisplayHelp(void);

/************************************************************************
 * Write Debug Data file define
 ************************************************************************/
extern char dataout_logfname[];  /* defalut \DATALOG\DATA.LOG_%d.csv */


#define V4V5K_TIME_OFS  16*60*60    /* Add Second of 16Hour (From U.S.) */

extern int fc_key;
extern int shot_number;

/************************************************************************
 * DAXS TRANS CONDITION
 ************************************************************************/
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

/*--------------------------*/
/*---- Wmat Information ----*/
/*--------------------------*/
#define PRODUCT_INFO_SIZE        16
#define LOTNAMAE_SIZE            10
#define CORDINATE_SIZE            2
#define FLI_FAILPOINT_SIZE       16     /* 最大128ポイント(4KBブロック×128＝512KBまで対応可) */

/*! フェイル箇所情報                                                          */
typedef struct{
  Word contents;                        /*!< FAIL内容                         */
#ifdef V5KONLY
  Byte area:4;                          /*!< FAIL領域                         */
  Byte vdd:4;                           /*!< FAILVDD                          */
#endif
#ifdef V4KONLY
  Byte vdd:4;                           /*!< FAILVDD                          */
  Byte area:4;                          /*!< FAIL領域                         */
#endif
  Byte mat;                             /*!< FAILマット                       */
} FailPoint;

/*! フェイル情報                                                              */
typedef struct{
  FailPoint failpoint;                  /*!< フェイルポイント                 */
  void *failaddress;                    /*!< フェイルアドレス                 */
} FailInfo;

/*! トリミングレジスタ情報構造体宣言                                          */
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

///*! 救済レジスタ情報構造体宣言                                                */
//typedef struct{
//  Byte reg00;                           /*!< REG00                            */
//  Byte reg01;                           /*!< REG01                            */
//} RedReg;

/*! 初期Vthサーチ結果構造体宣言                                               */
typedef struct{
  Byte centereven;                      /*!< EvenWL 初期Vth50%サーチ結果      */
  Byte centerodd;                       /*!< OddWL  初期Vth50%サーチ結果      */
  Byte low;                             /*!< 初期Vth_Lowレベルサーチ結果      */
  Byte high;                            /*!< 初期Vth_Highレベルサーチ結果     */
} VthiSearch;

/*! Vthサーチ結果(WT1)構造体宣言                                              */
typedef struct{
  Byte beforedisturb;                   /*!< WT1ディスターブ前Vthレベルサーチ結果 */
  Byte afterdisturb;                    /*!< WT1ディスターブ後Vthレベルサーチ結果 */
  Byte beforeretention10ua;             /*!< WT1リテンション前(10uA)Vth_Lowレベルサーチ結果 */
  Byte beforeretention3ua;              /*!< WT1リテンション前(3uA)Vth_Lowレベルサーチ結果 */
} VthSearchWT1;

/*! 書込み消去情報構造体宣言                                                  */
typedef struct{
  Byte max;                             /*!< 最大値                           */
  Word ave;                             /*!< 平均値                           */
} PEChara;

/*! Vthサーチ結果(WT2)構造体宣言                                              */
typedef struct{
  Byte afterretention10ua;              /*!< WT2リテンション後(10uA)Vth_Lowレベルサーチ結果 */
  Byte afterretention3ua;               /*!< WT2リテンション後(3uA)Vth_Lowレベルサーチ結果 */
  Byte beforedisturb;                   /*!< WT2ディスターブ前Vthレベルサーチ結果 */
  Byte afterdisturb;                    /*!< WT2ディスターブ後Vthレベルサーチ結果 */
  Byte afterdisturba;                   /*!< WT2ディスターブA後Vthレベルサーチ結果 */
} VthSearchWT2;

/*! 特性情報構造体宣言                                                        */
typedef struct{
  VthiSearch vthisearch;                /*!< WT1 初期Vthサーチ結果            */
  VthSearchWT1 vthsearchwt1;            /*!< WT1 Vthサーチ結果                */
  PEChara pcharastripeawt1;             /*!< WT1 ストライプA書込み情報        */
  PEChara pcharastripebwt1;             /*!< WT1 ストライプB書込み情報        */
  PEChara pcharaall1wt1;                /*!< WT1 ALL1書込み情報(FLD)          */
  Byte    pad0;                         /*!< Padding                          */
  PEChara pcharadiagawt1;               /*!< WT1 ダイアグA書込み情報          */
  PEChara pcharachkawt1;                /*!< WT1 チェッカーA書込み情報        */
  PEChara echarainitialwt1;             /*!< WT1 初期消去情報                 */
  Word echaratimetrimwt1_max;           /*!< WT1 時間トリミング後消去最大値   */
  Word echaratimetrimwt1_ave;           /*!< WT1 時間トリミング後消去平均値   */
  Byte    pad1;                         /*!< Padding                          */
  PEChara echaralowvoltwt1;             /*!< WT1 低電圧消去情報               */
  Word echaraallselwt1_max;             /*!< WT1 MG全選択消去最大値           */
  Word echaraallselwt1_ave;             /*!< WT1 MG全選択消去平均値           */
  Byte    pad2;                         /*!< Padding                          */
  PEChara echarajudgewt1;               /*!< WT1 平均値チェック消去情報       */
  Byte    pad3[ 5 ];                    /*!< Padding                          */
  VthSearchWT2 vthsearchwt2;            /*!< WT2 Vthサーチ結果                */
  Byte    pad4[ 3 ];                    /*!< Padding                          */
  PEChara pcharaall0wt2;                /*!< WT2 ALL0書込み情報               */
  PEChara pcharalowvoltwt2;             /*!< WT2 低電圧ALL0書込み情報         */
  Byte    pad5[ 2 ];                    /*!< Padding                          */
  PEChara pcharachkbwt2;                /*!< WT2 チェッカーB書込み情報        */
  PEChara echarainitialwt2;             /*!< WT2 初期消去情報                 */
  Byte    pad6[ 2 ];                    /*!< Padding                          */
  PEChara echaralowvoltwt2;             /*!< WT2 低電圧消去情報               */
  PEChara echarajudgewt2;               /*!< WT2 平均値チェック消去情報       */
  Byte    pad7[ 2 ];                    /*!< Padding                          */
  PEChara echaralastwt2;                /*!< WT2 最終消去情報                 */
  Byte    pad8[ 29 ];                   /*!< Padding                          */
} Character;

/*! ワークマット構造体宣言                                                    */
typedef struct{
  Byte productinfo[ PRODUCT_INFO_SIZE ];/*!< デバイス名                       */
  Byte lotname[ LOTNAMAE_SIZE ];        /*!< ロット名                         */
  Byte wafernumber;                     /*!< ウェハ番号                       */
  Byte pad0;                            /*!< Padding                          */
  Word processcode;                     /*!< 工程コード                       */
  Word footholdcode;                    /*!< 拠点コード                       */
  Word xchipdirection;                  /*!< X方向チップ座標                  */
  Word ychipdirection;                  /*!< Y方向チップ座標                  */
  Byte umathistory;                     /*!< USBファーム履歴                  */
  Byte programversion;                  /*!< プログラムバージョン             */
  Byte testversion;                     /*!< TESTバージョン                   */
  Byte flowversion;                     /*!< FLOWバージョン                   */
  Byte passcode[ 4 ];                   /*!< パスコード                       */
  Byte ptimetrimfli;                    /*!< 時間トリミングFLI(Pro)Vth50%値   */
  Byte ptimetrimfld;                    /*!< 時間トリミングFLD(Pro)Vth50%値   */
  Byte etimetrimfli;                    /*!< 時間トリミングFLI(Ers)Vth50%値   */
  Byte etimetrimfld;                    /*!< 時間トリミングFLD(Ers)Vth50%値   */
  FailInfo failinfo;                    /*!< フェイル情報                     */
  Byte reliefcategory;                  /*!< 救済カテゴリ                     */
  Byte reliefcount;                     /*!< 救済数                           */
  Byte pad1[ 6 ];                       /*! Padding                           */
  Byte failpoint_fli0[ FLI_FAILPOINT_SIZE ]; /*!< FLP 救済フェイルポイント    */
  Byte failpoint_fli1[ FLI_FAILPOINT_SIZE ]; /*!< FLI1救済フェイルポイント    */
  Byte failpoint_fli2[ FLI_FAILPOINT_SIZE ]; /*!< FLI2救済フェイルポイント    */
  Byte failpoint_fli3[ FLI_FAILPOINT_SIZE ]; /*!< FLI3救済フェイルポイント    */
  Lword chipidcrccode;                  /*!< CHIPIDのCRC32データ              */
  Byte pad2[ 4 ];                       /*! Padding                           */
  TrimReg trimreg;                      /*!< トリミングレジスタ値             */
  Byte pad3[ 6 ];                       /*!< Padding                          */
  Character flp;                        /*!< FLP特性                          */
  Character e2p;                        /*!< E2P特性                          */
  Lword rosc_logic;                     /*!< ロジック用オシレータ周期         */
  Lword rosc_chgpump;                   /*!< チャージポンプ用オシレータ周期   */
  Lword rosc_subact;                    /*!< サブアクト用オシレータ周期       */
  Byte pad4[ 84 ];                      /*!< Padding 書込みサイズ境界         */
} WmatStructure;

#endif
