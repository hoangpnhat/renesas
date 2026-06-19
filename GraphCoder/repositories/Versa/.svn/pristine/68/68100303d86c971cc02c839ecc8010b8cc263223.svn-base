/************************************************************************
*  FILENAME RX200fnc.h
*
  CONTENTS: This is the 90nm(RC03F) FLASH module function program
  for the V3300,4000,5000 test system
  COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED
  2008/11/01  rev.00  1'st created by H.Kobori
 ************************************************************************/
#ifndef FNC_H
#define FNC_H

#include "RC04EX_man.h"


/*---- DispEcrBmData() only use ----*/
#define DISP_BM      0x00010000
#define DISP_ECR     0x00020000
#define DISP_NO_FPT  0x00080000
#define DISP_4ADDR   0x00000008
#define DISP_4       0x00000004
#define DISP_1       0x00000001

/*-----------------------------------------------------------------------------------------------*/

typedef struct{
	int   Data[10];
} IntDataList;
typedef struct{
	IntDataList   DataSet[10];
} FirmParam;

extern FirmParam FirmDebug;

/************************************************************************
 * Difine Include fine
 ************************************************************************/
extern int  ErrorPrint(char*);
extern int  CheckPatLength(char *);
extern char *DispMatName(char *,interface_t*);
extern void DispAddres(interface_t*,char *,char *);
extern void DispVSTapAll(void);
extern void DispIVSAAll(void);
extern int  pow(int,int);
extern int  log2(int);
extern int  absolute(int);
extern int  strtod(char*);
extern int  strtoh(char*);
extern int  strtohn(char*,int);
extern int  StrCut(char*,char*,int,int);
extern int  DigitInput(int*);
extern char *str_bstrim(char *);
extern void StringInput(char*);
extern int  vcc_input(int);
extern int  SelectFfpt( interface_t*, int*, int );
extern int  FprintDeviceInfo( FILE_T* );
extern int  SpeedSelect(interface_t*,int);
extern int  OpenDataOutFile(const char*);
extern int  DataOut(int,char*);
extern int  DataOut2( int, FILE_T*, char* );
extern int  CloseDataOutFile(void);
extern void ParamConditionDisp(int,int,interface_t*);
//extern void RegConditionDisp(int,interface_t*); /* çÌèúó\íË => ParamConditionDispÇ…à⁄çs */
extern void TestConditionDisp(int,interface_t*,int,int[]);
extern void CpuModeOutput(int,interface_t*);
extern int  FbmWriteEcrToFile(char *,int,int,int);
extern int  EcrTextOut(int,int,int,int,char*);
//extern void MakeFBMFileName_Analyze(char* , interface_t* , int );
extern int  FbmDataOut(int,char,int,int,int);
extern int  FbmDataOut_BIST(int,int,char *);
extern int  FbmFileOut(int,char *,int,int);
extern void InitialValue(interface_t*);
extern int  SetValue(int,interface_t*,int);
extern int  GetValue(int,interface_t*);
extern int  SetReg(interface_t*);
extern int  SetInBDataLData(interface_t*);
extern int  SetMdl(interface_t*);
//extern int  SetIVSA(interface_t*, int);
extern int  ReturnTapV(int);
extern int  SelectAddress(interface_t*,int);
extern int  ChangeMat(int,interface_t*);
extern int  SaveRegFile(char *,interface_t*);
extern int  LoadRegFile(char *,interface_t*);
//extern int ReadLittleEndian(int, unsigned char*);
extern void DistributionForRetention(int[],interface_t*);
extern char *C_time(int *timer);

extern int DispEcrBmData( int addr, int cnt, int mode );
extern void v_memcpy( void* daddr, void* saddr, int size );

extern int CheckFK( int fk );

extern void GetDateTime( time_t*, char*, int );
extern void SetWmatStructPadding( WmatStructure*, const char* );
//extern int AddStringFile2FILE( char *, char * );
extern void DispWmatStruct( WmatStructure* );

extern int WaitHitKey( const char* );
extern int CheckHitKey( void );

extern int StoreInParam( const interface_t* );
extern int ReStoreInParam( interface_t* );
extern int GetValueEcrIIF( int );
extern int GetValueBmIIF( int );

extern int ReadEcrMode8_FF( int, int );
extern int TableDataOut(int[][],int,int,char*,char*);
extern int ChangeTimming_GTSLTS(int, int, int);
extern int ChangeTimming_GTSLTS_LP(int, int, int);
extern int ChangeTimming_GTSLTS_LP2(int, int, int);
extern int DelayTestSetting(int, int, int);
extern int ChangeAddress(int , int , int , interface_t* );
extern int VccSlow(int , int );
extern int PmuSlow(int , int );
extern int MINORI_BM_Setting(interface_t* , char*,int*);
extern int VrsgAnalyze(int[], int*);
extern void ParamInitialize(interface_t*);
extern void TempCheck();
extern int BGR_REG_CHECK(int, interface_t*);
extern void BL_DATACLEAR(interface_t*);
extern void Tokunin_Init(interface_t*,char*);
extern int WSLot_CHECK( void );
extern int PMU_ADCDIGI_OUT(int,interface_t*, char* );
extern void Firm_debugparam_Out(void);
extern int StrobeSetting(int*,int );


#endif
