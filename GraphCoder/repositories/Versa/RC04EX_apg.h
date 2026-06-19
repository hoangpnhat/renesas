/************************************************************************
 *  FILENAME    RX200apg.h
 *
 *  CONTENTS: This is a 128KB & 256KB  flash application program
 *  for the V3300 test system
 *
 *  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED
 *
 *  2008/11/01  rev.00  1'st created by H.Kobori
 ************************************************************************/
#ifndef APG_H
#define APG_H

#include "RC04EX_man.h"


/************************************************************************
 *  Difine Include fine
 ************************************************************************/

extern int  DeviceSpecificPowerUp(void);
extern int DeviceLevelsPowerUp(int,int,int,int);
extern void DeviceSpecificPowerDown(void);
extern int DeviceLevelsPowerDown(void);
extern int Ecrsetup8(int);
extern int OpensShorts(void);
extern int TesterTypeSelect(char*);
extern int CpuModeMonitor(int,int[],interface_t*,int,char*,char*,int*);
extern int CpuModeMonitor_Vrsg(int,int[],interface_t*,int,char*,char*,int*);
extern int CpuModeMonitor_VppTf(int,int[],interface_t*,int,char*,char*,int*);
extern int CpuModeMonitor_Repeat(int,int,int[],interface_t*,int,char*,char*,int*);
extern int CpuModeMonitor_current2(int,int,int[],interface_t*,int,char*,char*,int*);
extern int CpuModeMonitor_current_First(int,int,int[],interface_t*);
extern void GetLotName(char*,int);
extern void GetWaferName(char*,int);
extern int SetTimming_NS(int);
extern int SetTimming_PS(int);
extern int RAMRunApg(int,int,int);
extern int CpuModeFunc(int,int,int[],int,interface_t*,char*,char*);
extern int CpuModeFunc_PE(int,int,int,int[],int,interface_t*,char*,char*);
//extern int CpuModeStby(int,int,int[],int,interface_t*,char*,char*);
//extern int CpuModeFunc_FBM(int,int,int[],int,interface_t*,char*,char*);
extern int CpuModeFuncLogic(int,int,int[],int,interface_t*,char*,char*, int[]);
extern int CpuMode_VthRead(int,int[],interface_t*,int,char*,char*,int*);
extern int CpuMode_Monitor_PC(int,interface_t*,int,char*,char*,int*,int*,int*,int*);


extern int SelectPowerSupply(int,int[]);
extern int ReadEcrMode8(int,int);
extern void WriteEcrMode8(int,int,int);
extern int ReadBmMode8(int,int);
extern void WriteBmMode8(int,int,int);
extern void SwapBMData4byte(int,int);
extern void SwapECRData4byte(int,int);
extern int  DeviceSpecificInit(void);
extern char *CreatePatFileName( char* );
extern int SetVohVolVtt(int,int,int,int*);
extern int ParamTransBM(interface_t*);

extern int VthReadSearch(int,int[],interface_t*,int,int);
extern int VthReadDist(int,int,int,int,int[],int,interface_t*,char []);
extern int VthEdgeSearch(int,int,int[],int,interface_t*);
extern int DistributionDataOut(int,int[][],int,int,char[],interface_t*);
extern int MinoriModeFunc(int,int[],int,interface_t*,char*,char*);
extern int MinoriMonitor_Vrsg_First(int,int[],int,interface_t*,char*,char*,int*);
extern int MinoriMonitor_Vrsg_Repeat(int,int[],int,interface_t*,char*,char*,int,int*);
extern int TDR210(interface_t*);
extern int TDR04_03(interface_t*);
extern int TDR_BGR_Trim(int,int[],int,interface_t*);


#endif  /* APG_H */
