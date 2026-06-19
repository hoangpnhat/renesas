 // *********************************************************************** FILENAME RX200fnc.c  CONTENTS: This is the 90nm(RC03F) FLASH module function program for the V3300,4000,5000 test system COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED 2008/11/01  rev.00  1'st created by H.Kobori  ***********************************************************************
#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "RC04EX.hpg"
#include "RC04EX_CGR.h"
#include "RC04EX_cyc.h"
#include "compat.h"
#include "RC04EX_apg.h"
#include "RC04EX_tst.h"
#include "RC04EX_fnc.h"
#include "RC04EX_iif.h"

FirmParam FirmDebug;

 // *********************************************************************** Select MAT Function ***********************************************************************
char *DispMatName(char *mat_value, interface_t* param)
{
  int value;
  int i;
  value =0;
  if(GetValue(IN_TESTSIZE0,param)!=0)value++;
  if(GetValue(IN_TESTSIZE1,param)!=0)value++;
  if(GetValue(IN_TESTSIZE2,param)!=0)value++;
  
  *mat_value='\0';
  
  switch(GetValue(IN_AREA,param)){
    case 0:
      if(value == 3){
        strcat(mat_value," FLP0 & FLI1 & FLI2 ");
      }else if(value == 2){
        if(GetValue(IN_TESTSIZE0,param)==0) strcat(mat_value," FLI1 & FLI2 ");
        else if(GetValue(IN_TESTSIZE1,param)==0) strcat(mat_value," FLP0 & FLI2 ");
        else if(GetValue(IN_TESTSIZE2,param)==0) strcat(mat_value," FLP0 & FLI1 ");
      }else if(value ==1){
        if(GetValue(IN_TESTSIZE0,param)!=0) strcat(mat_value," FLP0 ");
        else if(GetValue(IN_TESTSIZE1,param)!=0) strcat(mat_value," FLI1 ");
        else if(GetValue(IN_TESTSIZE2,param)!=0) strcat(mat_value," FLI2 ");
      }else if(value ==0){
        strcat(mat_value," NO SELECT");
      }
      break;
    case 1:
         strcat(mat_value," EXTRA1 ");
         break;
    case 2:
         strcat(mat_value," EXTRA2 ");
         break;
    case 3:
         strcat(mat_value," EXTRA3 ");
         break;
    case 4:
         strcat(mat_value," EXTRA4 ");
         break;
    case 5:
         strcat(mat_value," EXTRA5 ");
         break;
    case 6:
         strcat(mat_value," EXTRA6 ");
         break;
  }
  return(mat_value);
}

 // *********************************************************************** Display Addres & MAT Function ***********************************************************************
void DispAddres(interface_t* param,char* add_space,char* disp2_value)
{
  int value;
  char mat_value[60];
  memset(mat_value, 0, sizeof(mat_value));

  if(strcmp(DispMatName(mat_value,param)," FLP0 & FLI1 & FLI2 ")==0){
//      if((GetValue( IN_TOPADDR0,param) == GetValue( IN_TOPADDR1,param))
//       &&(GetValue( IN_TOPADDR1,param) == GetValue( IN_TOPADDR2,param))
//       &&(GetValue(IN_TESTSIZE0,param) == GetValue(IN_TESTSIZE1,param))
//       &&(GetValue(IN_TESTSIZE1,param) == GetValue(IN_TESTSIZE2,param))){
//        sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ) );
//      }else{
//        sprintf(disp2_value,"FLP0 : Address:H'%X <-> H'%X(adr) / FLI1 : Address:H'%X <-> H'%X(adr) / FLI2 : Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ), //    // add_space,GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ), //	  /*add_space,*/GetValue(IN_TOPADDR2,param),( GetValue(IN_TOPADDR2,param) + GetValue(IN_TESTSIZE2,param) - 1 ));

//      }
  }else if(strcmp(DispMatName(mat_value,param)," FLP0 & FLI1 ")==0){
      if((GetValue( IN_TOPADDR0,param) == GetValue( IN_TOPADDR1,param))
       &&(GetValue(IN_TESTSIZE0,param) == GetValue(IN_TESTSIZE1,param))){
//        sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ) );
      }else{
//        sprintf(disp2_value,"FLP0 : Address:H'%X <-> H'%X(adr)%sFLI1 : Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ), //	  add_space,GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ) );
      }
  }else if(strcmp(DispMatName(mat_value,param)," FLP0 & FLI2 ")==0){
      if((GetValue( IN_TOPADDR0,param) == GetValue( IN_TOPADDR2,param))
       &&(GetValue(IN_TESTSIZE0,param) == GetValue(IN_TESTSIZE2,param))){
//        sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ) );
      }else{
//        sprintf(disp2_value,"FLP0 : Address:H'%X <-> H'%06X(adr)%sFLI2 : Address:H'%X <-> H'%06X(adr)", //          GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ), //	  add_space,GetValue(IN_TOPADDR2,param),( GetValue(IN_TOPADDR2,param) + GetValue(IN_TESTSIZE2,param) - 1 ) );
      }
  }else if(strcmp(DispMatName(mat_value,param)," FLI1 & FLI2 ")==0){
      if((GetValue( IN_TOPADDR1,param) == GetValue( IN_TOPADDR2,param))
       &&(GetValue(IN_TESTSIZE1,param) == GetValue(IN_TESTSIZE2,param))){
//        sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ) );
      }else{
//        sprintf(disp2_value,"FLI1 : Address:H'%X <-> H'%06X(adr)%sFLI2 : Address:H'%X <-> H'%X(adr)", //          GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ), //	  add_space,GetValue(IN_TOPADDR2,param),( GetValue(IN_TOPADDR2,param) + GetValue(IN_TESTSIZE2,param) - 1 ) );
      }
//  }else if((strcmp(DispMatName(mat_value,param)," FLP2 "==0))){
//      sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //        GetValue(IN_TOPADDR2,param),( GetValue(IN_TOPADDR2,param) + GetValue(IN_TESTSIZE2,param) - 1 ));
//  }else if((strcmp(DispMatName(mat_value,param)," FLP1 "==0))){
//      sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //        GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ));
  }else if((strcmp(DispMatName(mat_value,param)," FLP0 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA1 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA2 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA3 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA4 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA5 "==0))
   ||(strcmp(DispMatName(mat_value,param)," EXTRA6 "==0))){
//      sprintf(disp2_value,"Address:H'%X <-> H'%X(adr)", //        GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ));
  }else{
      printf("DispAddres Function Code ERROR!!! ==%s==",DispMatName(mat_value,param));
  }

        sprintf(disp2_value,"FLP0 : Address:H'%X <-> H'%X(adr) / FLI1 : Address:H'%X <-> H'%X(adr) / FLI2 : Address:H'%X <-> H'%X(adr)", GetValue(IN_TOPADDR0,param),( GetValue(IN_TOPADDR0,param) + GetValue(IN_TESTSIZE0,param) - 1 ), GetValue(IN_TOPADDR1,param),( GetValue(IN_TOPADDR1,param) + GetValue(IN_TESTSIZE1,param) - 1 ), GetValue(IN_TOPADDR2,param),( GetValue(IN_TOPADDR2,param) + GetValue(IN_TESTSIZE2,param) - 1 ));



}

// ****************************************************************************


// ****************************************************************************
int CheckPatLength(char * patpath)
{
  FILE_T *fpt;
  int cnt;
  if( ( fpt=fopen( patpath, "rb" ) )==NULL ){
  // Open file
    ErrorPrint( patpath ); return( FAIL );
    // fopen Error
  }
  fseek( fpt, 0, SEEK_END );
  // file size get
  cnt = ftell( fpt );
  if( fclose( fpt ) ) exit(-10);
  // Close file Err exit
  return( cnt );
  // file size return
}



 // *********************************************************************** Data Print Function ***********************************************************************
int DataOut(int outmode,char *dat)
{
  switch(outmode){
    case 0:
      break;
    case 1:
      printf("%s",dat);
      break;
    case 2:
      printf("%s",dat);
    case 3:
      fprintf(Fptdata,"%s",dat);
      break;
  }
  return(PASS);
}

int DataOut2( int outmode, FILE_T* buff, char* data ){
  if( 0x01&outmode ){
    printf( "%s", data );
  }
  if( 0x02&outmode ){
    fprintf( buff, "%s", data );
  }
  return( PASS );
}

 // *********************************************************************** DataOut used Fptdata open ***********************************************************************
int OpenDataOutFile( const char *fname ){
  char buff[256];
  CloseDataOutFile();
  sprintf( buff, "%s%s%s_%d.csv", glob_cdp, glob_datalog, fname, get_site_number() );
  if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "OpenDataOutFile Fptdata Open Err[%s]", buff ); exit( -99 ); } return( PASS );
}

 // *********************************************************************** DataOut used Fptdata close ***********************************************************************
int CloseDataOutFile( void ){
  if( fclose( Fptdata ) ){ printf( "CloseDataOutFile Fptdata Close Err" ); exit( -99 ); } return( PASS );
}

 // *********************************************************************** Error Print Function ***********************************************************************
int ErrorPrint(char *mess)
{
  printf(" -- FAIL -- Not Open file:%s !",mess);
  return(FAIL);
}

 // *********************************************************************** string to digit function ***********************************************************************
int strtod(char *str)
{
  int d, base, minus;

  while(isspace(*str)) str++;
  if( '-' == *str ){
    minus = -1;
    str++;
  } else minus = 1;
  if( !strnicmp(str, "0x", 2) ){
    base = 16;
    str += 2;
  } else base = 10;
  for( d = 0; NUL != *str && !isspace(*str); str++ ){
    if( 10 == base ){
      if(!isdigit(*str))    break;
    }
    else{
      if(!isxdigit(*str))   break;
    }
    d *= base;
    d += toint(*str);
  }
  return(d * minus);
}

 // *********************************************************************** string to hex function ***********************************************************************
int strtoh(char *str)
{
  int d, base, minus;

  base = 16;
  while(isspace(*str)) str++;
  if( '-' == *str ){
    minus = -1;
    str++;
  } else minus = 1;
  for( d = 0; NUL != *str && !isspace(*str); str++ ){
    d *= base;
    d += toint(*str);
  }
  return(d * minus);
}
 // *********************************************************************** string to hex ncount function ***********************************************************************
int strtohn(char *str,int byteend)
{
  int d, base, minus, stopinc;

  base = 16;
  while(isspace(*str)) str++;
  if( '-' == *str ){
    minus = -1;
    str++;
  } else minus = 1;
  for( d = 0,stopinc = 0; NUL != *str && !isspace(*str) && byteend > stopinc; str++,stopinc++ ){
    d *= base;
    d += toint(*str);
  }
  return(d * minus);
}

 // *********************************************************************** digit input function ***********************************************************************
int DigitInput(int *d)
{
  int i;
  int str_length;
  char buf[256],buf2[256];
  int stdin_flag;

// for( i = 0; NULL == fgets(buf, 256, stdin) && i < 5; i++ );

  fflush(stdout);

  while(gets(buf),str_bstrim(buf),*d=strtod(buf),0==strlen(buf)||(0==*d && 0==isalnum(buf[0]))){
    printf("Please enter valid values -> ");
    fflush(stdout);
  }

  str_length=strlen(buf);

  if(0==strnicmp(buf,"sy",2)) {
    while(1){
      printf("sys>> ");
      fflush(stdout);
      for(i=0; NULL==fgets(buf2,256,stdin) && i<5; i++ );
      if(!strnicmp(buf2,"q",1)) {*d=99999;return(PASS);}
      if(!strnicmp(buf2,"e",1)) {*d=99999;return(PASS);}

      comp_system(buf2);

    }
  }
  
    // **  ͕ɍ킹*do͂B傫ɋL  **

  if(0==strnicmp(buf,"indata",6)) {*d=SET_INDATA;  return(PASS);}
  if(0==strnicmp(buf,"fstop",5))  {*d=SET_FSTOP;   return(PASS);}
  if(0==strnicmp(buf,"vmode",5))  {*d=SET_VCC_MODE;return(PASS);}
  if(0==strnicmp(buf,"vddht",5))  {*d=SET_VDDHT;   return(PASS);}
  if(0==strnicmp(buf,"vddsel",5)) {*d=SET_BIVDDSEL;return(PASS);}
  if(0==strnicmp(buf,"micro",5)) {*d=SET_MICRO;    return(PASS);}
  if(0==strnicmp(buf,"vddh",4))   {*d=SET_CVDDH;   return(PASS);}
  //Add set vrefh 2017/0307
  if(0==strnicmp(buf,"vmax",4))   {*d=SET_CVMAX;   return(PASS);}
  if(0==strnicmp(buf,"vmin",4))   {*d=SET_CVMIN;   return(PASS);}
  if(0==strnicmp(buf,"vtyp",4))   {*d=SET_CVTYP;   return(PASS);}
  if(0==strnicmp(buf,"vddt",4))   {*d=SET_VDDT;    return(PASS);}
  if(0==strnicmp(buf,"per",4))    {*d=SET_PERCR;   return(PASS);}
  if(0==strnicmp(buf,"page",4))   {*d=SET_PAGE;    return(PASS);}
  if(0==strnicmp(buf,"sreg",4))   {*d=SAVE_REG;    return(PASS);}
  // Save Register
  if(0==strnicmp(buf,"lreg",4))   {*d=LOAD_REG;    return(PASS);}
  // Load Register
  if(0==strnicmp(buf,"data",4))   {*d=SET_DATA;    return(PASS);}
  if(0==strnicmp(buf,"wait",4))   {*d=SET_WAIT;    return(PASS);}
  if(0==strnicmp(buf,"quit",4))   {*d=QUIT;        return(PASS);}
  if(0==strnicmp(buf,"exit",4))   {*d=EXIT;        return(PASS);}
  if(0==strnicmp(buf,"tsec",4))   {*d=SET_SECT;    return(PASS);}
  if(0==strnicmp(buf,"iref",4))   {*d=SET_IREF;    return(PASS);}
  if(0==strnicmp(buf,"puls",4))   {*d=SET_PULS;    return(PASS);}
  if(0==strnicmp(buf,"loop",4))   {*d=SET_LOOP;    return(PASS);}
  if(0==strnicmp(buf,"vcc",3))    {*d=SET_CVCC;    return(PASS);}
  if(0==strnicmp(buf,"pll",3))    {*d=SET_PLL;     return(PASS);}
  if(0==strnicmp(buf,"vdd",3))    {*d=SET_CVDD;    return(PASS);}
  //Add set vref  2017/0307
  if(0==strnicmp(buf,"log",3))    {*d=SET_LOG;     return(PASS);}
  if(0==strnicmp(buf,"reg",3))    {*d=SET_REG;     return(PASS);}
  if(0==strnicmp(buf,"cfg",3))    {*d=SET_CONFIG;  return(PASS);}
  if(0==strnicmp(buf,"mod",3))    {*d=SET_MODULE;  return(PASS);}
  if(0==strnicmp(buf,"adr",3))    {*d=SET_ADR;     return(PASS);}
  if(0==strnicmp(buf,"mdl",3))    {*d=SET_MDL;     return(PASS);}
  if(0==strnicmp(buf,"mon",3))    {*d=SET_MON;     return(PASS);}
  if(0==strnicmp(buf,"mul",3))    {*d=SET_MUL;     return(PASS);}
  if(0==strnicmp(buf,"clk",3))    {*d=SET_CLK;     return(PASS);}
  if(0==strnicmp(buf,"tap",3))    {*d=SET_TAP;     return(PASS);}
  if(0==strnicmp(buf,"bgr",3))    {*d=SET_BGR;     return(PASS);}
  if(0==strnicmp(buf,"cut",3))    {*d=SET_CUT;     return(PASS);}
  if(0==strnicmp(buf,"lk",2))     {*d=SET_LK;      return(PASS);}
  if(0==strnicmp(buf,"bi",2))     {*d=SET_BI;      return(PASS);}
  if(0==strnicmp(buf,"ws",2))     {*d=SET_WSIZE;   return(PASS);}
  if(0==strnicmp(buf,"lp",2))     {*d=SET_LPREAD;  return(PASS);}
  if(0==strnicmp(buf,"vs",2))     {*d=SET_VSTEP;   return(PASS);}
  if(0==strnicmp(buf,"cg",2))     {*d=SET_CLKFCG;  return(PASS);}
  if(0==strnicmp(buf,"s",1))      {*d=SET_SPEED;   return(PASS);}
  if(0==strnicmp(buf,"vt",1))     {*d=SET_EDGE;    return(PASS);}
  if(0==strnicmp(buf,"q",1))      {*d=QUIT;        return(PASS);}
  if(0==strnicmp(buf,"e",1))      {*d=EXIT;        return(PASS);}
}

 // *********************************************************************** string input function ***********************************************************************
void StringInput( char *str )
{
  int n, i;
  char  buf[1024];

  fflush( stdout );
  buf[0] = NUL;
  for( i = 0; NULL == fgets(buf, 1024, stdin) && i < 5; i++ );
  n = strlen( buf ) - 1;
  if( n >= 0 && '\n' == buf[n] )  buf[n] = NUL;
  // cut last LF
  strcpy( str, buf );
}


 // *********************************************************************** DataOut Log Mode Select ***********************************************************************
int SelectFfpt( interface_t *param, int *vcc, int mode ){
  int i, taddr, tempo;

  if( 2!=mode && 1!=mode ){
    printf( "enter output log (0=off, 1=display, 2=disp+file) Ffpt=%d -> ", Ffpt );
    DigitInput( &tempo );
    if( Ffpt==tempo ){ printf( "!!! allready set !!!" ); return( FAIL ); } }else{ tempo = mode;
  }
  if( 2==tempo ){
    Ffpt = 2;
    OpenDataOutFile( dataout_logfname );
    FprintDeviceInfo( Fptdata );
  }else{
    if( 2==Ffpt ) CloseDataOutFile();
    Ffpt = tempo;
  }
  return( PASS );
}

int FprintDeviceInfo( FILE_T* fpt ){
  char time_buf[32];
  time( &time_list ); GetDateTime( &time_list, time_buf, 1 );
  fprintf( fpt, "+======================================================+");
  fprintf( fpt, "| %s device analysis program %s", DEVICENAME, Tester_name );
  fprintf( fpt, "| program  : %s", program_name );
  fprintf( fpt, "| exec date: %s", time_buf );
  fprintf( fpt, "| lot name : %.30s  wafer: %d", lotname, atoi(wafername) );
  fprintf( fpt, "| device   : %.30s  X:%d  Y:%d", devicename, xy_location[0], xy_location[1] );
  fprintf( fpt, "| comments : %.30s", temp_comment );
  fprintf( fpt, "+======================================================+");
}

 // *********************************************************************** Read speed select function ***********************************************************************
int SpeedSelect(interface_t* param,int command)
{
  int tempo, tempo1, keep;

  keep = 1;

  while(keep){
    keep = 0;
        // Speed Set
    if(command == SET_SPEED){
      printf( "Enter EXTAL rate(ns)-> " ); DigitInput( &tempo ); SetValue( IN_EXTAL1, param, tempo );
    }else{
            // PLL Set
      printf( "Enter PLL ON(1)/OFF(0)-> " ); DigitInput( &tempo ); SetValue( IN_PLLON, param, tempo );
      if(tempo == 1){
      // PLL Set for PLL-ON 
        printf("Enter PLL MLUT speed X (2~8)->"); DigitInput( &tempo ); SetValue( IN_PLL_MULT, param, tempo );
        printf("Enter PLL DIV  speed / (1~4)->"); DigitInput( &tempo ); SetValue( IN_PLL_DIV , param, tempo );
      }
    
            // ICLK Set
      printf("Enter ICLK DIV speed / (1,2,4,8,16,32,64)->"); DigitInput( &tempo ); SetValue( IN_ICLK, param, tempo );

            // PCLK Set
      printf("Enter PCLK DIV speed / (1,2,4,8,16,32,64)->"); DigitInput( &tempo ); SetValue( IN_PCLK, param, tempo );


            // PLL_**_VAL Set
      SetValue( IN_PLL_MULT_VAL, param, GetValue(IN_PLL_MULT,param)-1 );
      SetValue( IN_PLL_DIV_VAL, param, GetValue(IN_PLL_DIV,param)-1 );

            // ICLK_VAL Set
      SetValue( IN_ICLK_VAL, param, log2(GetValue(IN_ICLK,param)) );

            // PCLK_VAL Set
      SetValue( IN_PCLK_VAL, param, log2(GetValue(IN_PCLK,param)) );
    }


        //--- Error flag Set ---//
        // PLL Error
    if( ((2<=GetValue(IN_PLL_MULT,param)) && (GetValue(IN_PLL_MULT,param)<=8)) || (GetValue(IN_PLLON,param)==0) ){
    }else{
      printf("PLL_MULT Setting ERROR!!! PLL_MULT Value = %d",GetValue(IN_PLL_MULT,param));
      keep = 1;
    }
    if( ((1<=GetValue(IN_PLL_DIV,param)) && (GetValue(IN_PLL_DIV,param)<=4)) || (GetValue(IN_PLLON,param)==0) ){
    }else{
      printf("PLL_DIV Setting ERROR!!! PLL_DIV Value = %d",GetValue(IN_PLL_DIV,param));
      keep = 1;
    }
    
        // ICLK Error
    switch(GetValue(IN_ICLK,param)){
      case 1:
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
        break;
      default:
        printf("ICLK Setting ERROR!!! ICLK Value = %d",GetValue(IN_ICLK,param));
        keep = 1;
        break;
    }

        // PCLK Error
    switch(GetValue(IN_PCLK,param)){
      case 1:
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
        break;
      default:
        printf("PCLK Setting ERROR!!! PCLK Value = %d",GetValue(IN_PCLK,param));
        keep = 1;
        break;
    }

    if(keep==1) printf(" SPEED Setting ERROR!!! ReStart SPEED Setting!!!");

  }


  // printf( "analyzetest.c Condition has indicated to productparameter.h" ); if( 1==tempo ){ printf( "Enter PLL multiple -> " ); DigitInput( &tempo1 ); SetValue( IN_PLL_MULT, param, tempo1 ); printf( "  PLLON = 0x01(0)              |  0x41(1)               |  0x81(2)" ); printf( "   ICLK = ICLKDIVIDE_PLLON_AT  |  PCLKBDIVIDE_PLLON_AT  |  ICLKDIVIDE_PLLON_AT" ); printf( "   FCLK = FCLKDIVIDE_PLLON_AT  |  ICLKDIVIDE_PLLON_AT   |  ICLKDIVIDE_PLLON_AT" ); printf( "Enter Normal(0)/Special(1or2) -> "); DigitInput( &tempo1 ); }else{ SetValue( IN_PLL_MULT, param, 1 ); printf( "  PLLON = 0x00(0)               |  0x40(1)  |  0x80(2)" ); printf( "   ICLK = ICLKDIVIDE_PLLOFF_AT  |  x1/2     |  x1" ); printf( "   FCLK = FCLKDIVIDE_PLLOFF_AT  |  x1       |  x1/2" ); printf( "Enter Normal(0)/Special(1or2) -> "); DigitInput( &tempo1 ); } SetValue( IN_PLLON, param, (tempo1<<6) | tempo );
  return( PASS );
}



 // *********************************************************************** Parameter output function ***********************************************************************
void ParamConditionDisp(int mode , int mode_mem , interface_t* param)
{
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);

  if(( mode & PDIN_REG ) == PDIN_REG){
    if(TRIM_REG & GetValue(IN_REG,param)) {
    // Trim Reg
      DataOut( Ffpt, "| Trim  |" );
      sprintf(outbuf, "IN_ETLR00 : H'%02X  ",GetValue(IN_ETLR00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR01 : H'%02X  ",GetValue(IN_ETLR01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR02 : H'%02X  ",GetValue(IN_ETLR02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR03 : H'%02X  ",GetValue(IN_ETLR03,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR04 : H'%02X  ",GetValue(IN_ETLR04,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR05 : H'%02X  ",GetValue(IN_ETLR05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR06 : H'%02X  ",GetValue(IN_ETLR06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR07 : H'%02X  ",GetValue(IN_ETLR07,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR08 : H'%02X  ",GetValue(IN_ETLR08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR09 : H'%02X  ",GetValue(IN_ETLR09,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR10 : H'%02X  ",GetValue(IN_ETLR10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR11 : H'%02X  ",GetValue(IN_ETLR11,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR12 : H'%02X  ",GetValue(IN_ETLR12,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR13 : H'%02X  ",GetValue(IN_ETLR13,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR14 : H'%02X  ",GetValue(IN_ETLR14,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR15 : H'%02X  ",GetValue(IN_ETLR15,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR16 : H'%02X  ",GetValue(IN_ETLR16,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR17 : H'%02X  ",GetValue(IN_ETLR17,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR18 : H'%02X  ",GetValue(IN_ETLR18,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR19 : H'%02X  ",GetValue(IN_ETLR19,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR20 : H'%02X  ",GetValue(IN_ETLR20,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR21 : H'%02X  ",GetValue(IN_ETLR21,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR22 : H'%02X  ",GetValue(IN_ETLR22,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR23 : H'%02X  ",GetValue(IN_ETLR23,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR24 : H'%02X  ",GetValue(IN_ETLR24,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR25 : H'%02X  ",GetValue(IN_ETLR25,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR26 : H'%02X  ",GetValue(IN_ETLR26,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR27 : H'%02X  ",GetValue(IN_ETLR27,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR28 : H'%02X  ",GetValue(IN_ETLR28,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR29 : H'%02X  ",GetValue(IN_ETLR29,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR30 : H'%02X  ",GetValue(IN_ETLR30,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR31 : H'%02X  ",GetValue(IN_ETLR31,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR32 : H'%02X  ",GetValue(IN_ETLR32,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR33 : H'%02X  ",GetValue(IN_ETLR33,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR34 : H'%02X  ",GetValue(IN_ETLR34,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETLR35 : H'%02X  ",GetValue(IN_ETLR35,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR36 : H'%02X  ",GetValue(IN_ETLR36,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR37 : H'%02X  ",GetValue(IN_ETLR37,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR38 : H'%02X  ",GetValue(IN_ETLR38,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETLR39 : H'%02X  ",GetValue(IN_ETLR39,param)); DataOut(Ffpt,outbuf);

    }else{
      DataOut( Ffpt, "| Trim  | <<<<< TRIM REG No SET >>>>>\n" );
    }

  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);

    if(TEST_REG & GetValue(IN_REG,param)) {
    // Test Reg
      DataOut( Ffpt, "| Test  |" );
      sprintf(outbuf, "IN_ETCR00 : H'%02X  ",GetValue(IN_ETCR00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR01 : H'%02X  ",GetValue(IN_ETCR01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR02 : H'%02X  ",GetValue(IN_ETCR02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR03 : H'%02X  ",GetValue(IN_ETCR03,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR04 : H'%02X  ",GetValue(IN_ETCR04,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETCR05 : H'%02X  ",GetValue(IN_ETCR05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR06 : H'%02X  ",GetValue(IN_ETCR06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR07 : H'%02X  ",GetValue(IN_ETCR07,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR08 : H'%02X  ",GetValue(IN_ETCR08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR09 : H'%02X  ",GetValue(IN_ETCR09,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_ETCR10 : H'%02X  ",GetValue(IN_ETCR10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR11 : H'%02X  ",GetValue(IN_ETCR11,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_ETCR12 : H'%02X  ",GetValue(IN_ETCR12,param)); DataOut(Ffpt,outbuf);
    }else{
      DataOut( Ffpt, "| Test  | <<<<< TEST REG No SET >>>>>\n" );
      // Test Reg Off
    }
  }
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);

  if(( mode & PDIN_BDATA ) == PDIN_BDATA){

      DataOut( Ffpt, "| Bdata |" );
      sprintf(outbuf, "IN_BDATA00 : H'%02X  ",GetValue(IN_BDATA00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA01 : H'%02X  ",GetValue(IN_BDATA01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA02 : H'%02X  ",GetValue(IN_BDATA02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA03 : H'%02X  ",GetValue(IN_BDATA03,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA04 : H'%02X  ",GetValue(IN_BDATA04,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_BDATA05 : H'%02X  ",GetValue(IN_BDATA05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA06 : H'%02X  ",GetValue(IN_BDATA06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA07 : H'%02X  ",GetValue(IN_BDATA07,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA08 : H'%02X  ",GetValue(IN_BDATA08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA09 : H'%02X  ",GetValue(IN_BDATA09,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_BDATA10 : H'%02X  ",GetValue(IN_BDATA10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA11 : H'%02X  ",GetValue(IN_BDATA11,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA12 : H'%02X  ",GetValue(IN_BDATA12,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA13 : H'%02X  ",GetValue(IN_BDATA13,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA14 : H'%02X  ",GetValue(IN_BDATA14,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "IN_BDATA15 : H'%02X  ",GetValue(IN_BDATA15,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "IN_BDATA16 : H'%02X  ",GetValue(IN_BDATA16,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);

  }
  if(( mode & PDIN_LDATA ) == PDIN_LDATA){

      DataOut( Ffpt, "| Ldata |" );
      sprintf(outbuf, " IN_LDATA0: H'%08X",GetValue(IN_LDATA0,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_LDATA1: H'%08X",GetValue(IN_LDATA1,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_LDATA2: H'%08X",GetValue(IN_LDATA2,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_LDATA3: H'%08X",GetValue(IN_LDATA3,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
  }
  if(( mode & PDIN_OTHER ) == PDIN_OTHER){
  
      DataOut( Ffpt, "| other |" );
      sprintf(outbuf, " IN_EXTAL2      : H'%02X",GetValue(IN_EXTAL2, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_EXTAL1      : H'%02X",GetValue(IN_EXTAL1, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PLLON       : H'%02X",GetValue(IN_PLLON, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PLL_MULT    : H'%02X",GetValue(IN_PLL_MULT, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_PLL_MULT_VAL: H'%02X",GetValue(IN_PLL_MULT_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PLL_DIV     : H'%02X",GetValue(IN_PLL_DIV, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PLL_DIV_VAL : H'%02X",GetValue(IN_PLL_DIV_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_ICLK        : H'%02X",GetValue(IN_ICLK, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_ICLK_VAL    : H'%02X",GetValue(IN_ICLK_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_BCLK        : H'%02X",GetValue(IN_BCLK, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_BCLK_VAL    : H'%02X",GetValue(IN_BCLK_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_FCLK        : H'%02X",GetValue(IN_FCLK, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_FCLK_VAL    : H'%02X",GetValue(IN_FCLK_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PCLK        : H'%02X",GetValue(IN_PCLK, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_PCLK_VAL    : H'%02X",GetValue(IN_PCLK_VAL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_MEMWAIT     : H'%02X",GetValue(IN_MEMWAIT, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_REG         : H'%02X",GetValue(IN_REG, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_AREA        : H'%02X",GetValue(IN_AREA, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_TESTSEL     : H'%02X",GetValue(IN_TESTSEL, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_WRITESIZE   : H'%02X",GetValue(IN_WRITESIZE, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_VCC_MODE    : H'%02X",GetValue(IN_VCC_MODE, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_VDD_TAP     : H'%02X",GetValue(IN_VDD_TAP, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_VDDH_TAP    : H'%02X",GetValue(IN_VDDH_TAP, param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, " IN_LOWPOWER    : H'%02X",GetValue(IN_LOWPOWER, param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, " IN_NOFAILSTOP  : H'%02X",GetValue(IN_NOFAILSTOP, param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
  }
  if(( mode & PDOUT_REG ) == PDOUT_REG){
  
      DataOut( Ffpt, "\n| Trim  |" );
      sprintf(outbuf, "OUT_ETLR00: H'%02X ",GetValue(OUT_ETLR00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR01: H'%02X ",GetValue(OUT_ETLR01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR02: H'%02X ",GetValue(OUT_ETLR02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR03: H'%02X ",GetValue(OUT_ETLR03,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR04: H'%02X ",GetValue(OUT_ETLR04,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR05: H'%02X ",GetValue(OUT_ETLR05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR06: H'%02X ",GetValue(OUT_ETLR06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR07: H'%02X ",GetValue(OUT_ETLR07,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR08: H'%02X ",GetValue(OUT_ETLR08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR09: H'%02X ",GetValue(OUT_ETLR09,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR10: H'%02X ",GetValue(OUT_ETLR10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR11: H'%02X ",GetValue(OUT_ETLR11,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR12: H'%02X ",GetValue(OUT_ETLR12,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR13: H'%02X ",GetValue(OUT_ETLR13,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR14: H'%02X ",GetValue(OUT_ETLR14,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR15: H'%02X ",GetValue(OUT_ETLR15,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR16: H'%02X ",GetValue(OUT_ETLR16,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR17: H'%02X ",GetValue(OUT_ETLR17,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR18: H'%02X ",GetValue(OUT_ETLR18,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR19: H'%02X ",GetValue(OUT_ETLR19,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR20: H'%02X ",GetValue(OUT_ETLR20,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR21: H'%02X ",GetValue(OUT_ETLR21,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR22: H'%02X ",GetValue(OUT_ETLR22,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR23: H'%02X ",GetValue(OUT_ETLR23,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR24: H'%02X ",GetValue(OUT_ETLR24,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR25: H'%02X ",GetValue(OUT_ETLR25,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR26: H'%02X ",GetValue(OUT_ETLR26,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR27: H'%02X ",GetValue(OUT_ETLR27,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR28: H'%02X ",GetValue(OUT_ETLR28,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR29: H'%02X ",GetValue(OUT_ETLR29,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR30: H'%02X ",GetValue(OUT_ETLR30,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR31: H'%02X ",GetValue(OUT_ETLR31,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR32: H'%02X ",GetValue(OUT_ETLR32,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR33: H'%02X ",GetValue(OUT_ETLR33,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR34: H'%02X ",GetValue(OUT_ETLR34,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR35: H'%02X ",GetValue(OUT_ETLR35,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETLR36: H'%02X ",GetValue(OUT_ETLR36,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR37: H'%02X ",GetValue(OUT_ETLR37,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR38: H'%02X ",GetValue(OUT_ETLR38,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETLR39: H'%02X ",GetValue(OUT_ETLR39,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "| Test  |" );
      sprintf(outbuf, "OUT_ETCR00: H'%02X ",GetValue(OUT_ETCR00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR01: H'%02X ",GetValue(OUT_ETCR01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR02: H'%02X ",GetValue(OUT_ETCR02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR03: H'%02X ",GetValue(OUT_ETCR03,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETCR04: H'%02X ",GetValue(OUT_ETCR04,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR05: H'%02X ",GetValue(OUT_ETCR05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR06: H'%02X ",GetValue(OUT_ETCR06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR07: H'%02X ",GetValue(OUT_ETCR07,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETCR08: H'%02X ",GetValue(OUT_ETCR08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR09: H'%02X ",GetValue(OUT_ETCR09,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR10: H'%02X ",GetValue(OUT_ETCR10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR11: H'%02X ",GetValue(OUT_ETCR11,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_ETCR12: H'%02X ",GetValue(OUT_ETCR12,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR16: H'%02X ",GetValue(OUT_ETCR16,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_ETCR17: H'%02X ",GetValue(OUT_ETCR17,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
  }
  if(( mode & PDOUT_BDATA ) == PDOUT_BDATA){
  
      DataOut( Ffpt, "| Bdata |" );
      sprintf(outbuf, "OUT_BDATA00: H'%02X ",GetValue(OUT_BDATA00,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA01: H'%02X ",GetValue(OUT_BDATA01,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA02: H'%02X ",GetValue(OUT_BDATA02,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA03: H'%02X ",GetValue(OUT_BDATA03,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_BDATA04: H'%02X ",GetValue(OUT_BDATA04,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA05: H'%02X ",GetValue(OUT_BDATA05,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA06: H'%02X ",GetValue(OUT_BDATA06,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA07: H'%02X ",GetValue(OUT_BDATA07,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_BDATA08: H'%02X ",GetValue(OUT_BDATA08,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA09: H'%02X ",GetValue(OUT_BDATA09,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA10: H'%02X ",GetValue(OUT_BDATA10,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA11: H'%02X ",GetValue(OUT_BDATA11,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_BDATA12: H'%02X ",GetValue(OUT_BDATA12,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA13: H'%02X ",GetValue(OUT_BDATA13,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA14: H'%02X ",GetValue(OUT_BDATA14,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_BDATA15: H'%02X ",GetValue(OUT_BDATA15,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
  }
  if(( mode & PDOUT_LDATA ) == PDOUT_LDATA){
  
      DataOut( Ffpt, "| Ldata |" );
      sprintf(outbuf, "OUT_LDATA0: H'%08X ",GetValue(OUT_LDATA0,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_LDATA1: H'%08X ",GetValue(OUT_LDATA1,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_LDATA2: H'%08X ",GetValue(OUT_LDATA2,param)); DataOut(Ffpt,outbuf);
      DataOut( Ffpt, "\n|       |" );
      sprintf(outbuf, "OUT_LDATA3: H'%08X ",GetValue(OUT_LDATA3,param)); DataOut(Ffpt,outbuf);
      sprintf(outbuf, "OUT_LDATA4: H'%08X ",GetValue(OUT_LDATA4,param)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);
  }
  if(( mode & PDOUT_OTHER ) == PDOUT_OTHER){
      DataOut( Ffpt, "| other |" );
      sprintf(outbuf, "OUT_JUDGE: H'%04X ",((0x00FF&GetValue(OUT_JUDGE1,param))<<8)|((0xFF00&GetValue(OUT_JUDGE1,param))>>8)); DataOut(Ffpt,outbuf);
  sprintf(outbuf, "+---------------------------------------------------------------------------------------------------------------+");
  DataOut(Ffpt,outbuf);

  }
  DataOut( Ffpt, "\n" );
  
  if(( mode_mem & DP_BM ) == DP_BM){
    DispEcrBmData( INPUTIF_TOP, 0x100, DISP_BM|DISP_1 );
    DispEcrBmData( OUTPUTIF_TOP, 0x100, DISP_BM|DISP_1 );
    DataOut( Ffpt, "\n" );
  }
  if(( mode_mem & DP_ECR ) == DP_ECR){
    DispEcrBmData( INPUTIF_TOP, 0x100, DISP_ECR|DISP_1 );
    DispEcrBmData( OUTPUTIF_TOP, 0x100, DISP_ECR|DISP_1 );
    DataOut( Ffpt, "\n" );
  }
}




// ****************************************************************************
// Test Condition Display
// ****************************************************************************
void TestConditionDisp(int mode,interface_t* param,int waittime,int vs[NUM_POWERSUPPLY])
{
  char mat_value[60];
  char disp2_value[256];
  char lp_read_mode[5];
  char fail_stop[5],bgr_status[6],cg_status[6];
  int  mat,write_size;
  int freq;

  freq = 1000/GetValue(IN_EXTAL1,param);
  DispAddres(param,"|       | ",disp2_value);

  if(GetValue(IN_WRITESIZE,param)==0){
    write_size = 256;
  }else{
    write_size = GetValue(IN_WRITESIZE,param);
  }

  if(GetValue(IN_LOWPOWER,param)==0){
    sprintf(lp_read_mode,"OFF");
  }else if(GetValue(IN_LOWPOWER,param)==1){
    sprintf(lp_read_mode," ON");
  }else{
    sprintf(lp_read_mode,"ERROR");
  }

  if(GetValue(IN_NOFAILSTOP,param)==0){
    sprintf(fail_stop,"OFF");
  }else if(GetValue(IN_NOFAILSTOP,param)==1){
    sprintf(fail_stop," ON");
  }else{
    sprintf(fail_stop,"ERROR");
  }

  if(GetValue(IN_BDATA14,param) == 1){
    sprintf(cg_status," ON");
  }else{
    sprintf(cg_status,"OFF");
  }

  
  memset(mat_value, 0, sizeof(mat_value));
  printf("|  Test    | Module   :%s(mdl)",DispMatName(mat_value,param));
  printf("|          | %s",disp2_value);
  printf("|          | RegSet   :%02X(reg) / Monitor-Set(mon) / PE-PULS(puls) / Iref(iref) / Multi-Select(mul)",GetValue(IN_REG,param) );
  printf("|          | WaitTime :%ds(wait)",waittime );
  printf("|          | Extal    :%d[ns] %d[MHz] (s)", GetValue(IN_EXTAL1,param),freq);
  if(GetValue(IN_PLLON,param)==1){
  printf("|          | PLL_MULT :X%d, PLL_DIV:/%d (pll)",GetValue(IN_PLL_MULT,param),GetValue(IN_PLL_DIV,param));
  }
  printf("|          | ICLK_DIV :/%d , PCLK_DIV:/%d (pll)",GetValue(IN_ICLK,param),GetValue(IN_PCLK,param));
  printf("|          | [Other] Write Size : %dB (ws), LowPowerRead : %s (lp), NoFailStop : %s (fstop)",write_size,lp_read_mode,fail_stop);
  printf("|          | CLKF-CG : %s(cg)",cg_status);
  printf("+----------+------------------------------------------------------------------------------------------+");
  printf("|  Power   | Vcc:%4dmV(vcc) Vddh:%4dmV(vddh) Vdd:%4dmV(vdd)", vs[0],vs[1],vs[2]);
  printf("|          | ");

}

// ****************************************************************************
// make Dunplist or V2kfile  Function
// ****************************************************************************
void CpuModeOutput(int mode,interface_t* param)
{
  int param_mode,bmecr_mode;
  
  switch(mode){
  // modeɉparam̏o͂ύX
    case MNORMAL:
      param_mode = PDOUT_ALL;
      bmecr_mode = NO_BMECR;
      break;
    


    default:
      param_mode = PDOUT_ALL;
      bmecr_mode = NO_BMECR;
      break;
   }

  ParamConditionDisp( param_mode, bmecr_mode, param);
  //param̃WX^{Othero

}

 // *********************************************************************** Write Ecr to File Function(for PC-WFBM) ***********************************************************************
int FbmWriteEcrToFile(char *fname,int mode,int start,int len)
{
  FILE_T *binfpt;
  int i,data;
  char wdata[2];
  char base[384];
  switch(mode) {
  case 0:
  // ** only make File(no data) **
    if((binfpt=fopen(fname,"wb"))==NULL) return(FAIL);
    if(fclose(binfpt)) exit(-10);
    break;
  case 1:
  // ** for readmode of 8bit **
    if((binfpt=fopen(fname,"wb"))==NULL) return(FAIL);
    for(i=start;i<(len/2);i+=2) {
      wdata[0]=(char)((read_ecr(i)>>8)&0xff);
      wdata[1]=(char)((read_ecr(i+1)>>8)&0xff);
      fwrite((VOID_T *)wdata,(unsigned)sizeof(char),2,binfpt);
    }
    if(fclose(binfpt)) exit(-10);
    break;
  case 4:
  // ** for ecr mode of 8bit **
    if(comp_write_ecr_to_file(fname,2,start,len)) return(FAIL);
    break;
  }
  return(PASS);
}

 // *********************************************************************** FbmDataOut Function ***********************************************************************
int FbmDataOut(int mode,char fwacat,int start,int len,int fbmkind)
{
  int i;
    // get file path
  strcpy( baselog, glob_cdp );
  if( CHIP&mode ){
    strcat( baselog, glob_fbm ); strcat( baselog, "XxxxYxxx_xx.bin" );
  }else{
    strcat( baselog, lotname ); strcat( baselog, "_" ); strcat( baselog, wafername );
    sprintf( filenamebuf, "\\X%03dY%03d_%02X.bin", xy_location[0], xy_location[1], fwacat );
    strcat( baselog, filenamebuf );
  }
    // make fbm file for FWA
  if(FbmWriteEcrToFile(baselog,4,start,len)) {
    printf(" -- FAIL -- not open file:%s !",baselog);
    return(FAIL);
  }

  return(PASS);
}

 // *********************************************************************** FbmDataOut Function ***********************************************************************
int FbmDataOut_BIST(int start,int len,char *filename)
{
    // get file path
  strcpy( baselog, glob_cdp ); strcat( baselog, glob_fbm );
  strcat( baselog, filename );
  if( CheckFK(FK_DISP_DEBUG) ) printf( "FBM File=[%s]path=[%s] start=[0x%08X] len=[%d]", filename, baselog, start, len );
    // make fbm file for FWA
  if(FbmWriteEcrToFile(baselog,4,start,len)) {
    printf(" -- FAIL -- not open file:%s !",baselog);
    return(FAIL);
  }

  return(PASS);
}

 // *********************************************************************** FbmFileOut Function ***********************************************************************
int FbmFileOut(int mode,char* fname,int start,int len)
{
  char baselog[128];

  switch (mode) {
    case 0:
    // get file path
             strcpy(baselog,glob_cdp);
             strcat(baselog,"\\FBM\\");
             strcat(baselog,fname);
            break;
    case 1:
    // get file path
             strcpy(baselog,fname);
            break;
    default: break;
  }

    // make fbm file for FWA
  if(FbmWriteEcrToFile(baselog,4,start,len)) {
    printf(" -- FAIL -- not open file:%s !",baselog);
    return(FAIL);
  }

  return(PASS);
}

 // *********************************************************************** MakeFBMFileName for ANALYZE Function ***********************************************************************
// void MakeFBMFileName_Analyze(char* fname, interface_t* param, int patkind) { int i, mat, tap, ivsa, area, posinega, relief;  mat = GetValue( IN_MAT, param ); area = GetValue( IN_SW_AREA, param ); for( i = 0; i<3; i++ ){ if( 0!=((area>>i)&0x01) ) break; } area = i+1; tap = GetValue( IN_VPOINT, param ); if( CODE_FLASH==mat_module[mat] ){ posinega = 1; ivsa = GetValue( IN_TESTR16_FLP, param )&0x0F; }else{ if( ( GetValue( IN_TESTR10_E2P, param )&0x01 ) && ( GetValue( IN_TESTR8_E2P, param )&0x02 ) && 2==patkind ) posinega = 2; else                                                                                                        posinega = 1; ivsa = ( GetValue( IN_TESTR9_E2P, param )>>4 )&0x0F; } relief = ( ( GetValue( IN_TESTSEL, param )>>7 ) & 0x01 ) + 1;  if( 2==patkind ){ // VF FBM sprintf( fname,FBMFILENAME, xy_location[0], xy_location[1], devicename, lotname, atoi(wafername), mat_string[ mat ], area, posinega, relief, voltage_string[ tap ], ivsa_string[ ivsa ] ); }else if( 1==patkind ){ // RD FBM sprintf( fname,FBMFILENAME, xy_location[0], xy_location[1], devicename, lotname, atoi(wafername), mat_string[ mat ], area, posinega, relief, "no", "no" ); } return; }

 // *********************************************************************** Set Data To Value Function ***********************************************************************
int SetValue(int pname,interface_t* initparam,int data)
{
  int old_data,b2data_u,b2data_d,tempo1,tempo2;
  
  switch(pname){
    case IN_EXTAL2:
    case IN_EXTAL1:
    
      b2data_u = (  data       & 0xFF );
      b2data_d = ( (data >> 8) & 0xFF );
      tempo1 = (initparam + IN_EXTAL1) -> value;
      //get value
      tempo2 = (initparam + IN_EXTAL2) -> value;
      //get value
      (initparam + IN_EXTAL1) -> value = b2data_d;
      //set value
      (initparam + IN_EXTAL2) -> value = b2data_u;
      //set value
      old_data = (tempo1 << 8) + tempo2;
      if( data<=TM_SET_NS_MAX ) (initparam + IN_EXTAL_PS) -> value = data*1000;
      else                      (initparam + IN_EXTAL_PS) -> value = data;
	  break;
    case OUT_JUDGE2:
    case OUT_JUDGE1:
    
      b2data_u = (  data       & 0xFF );
      b2data_d = ( (data >> 8) & 0xFF );
      tempo1 = (initparam + OUT_JUDGE1) -> value;
      //get value
      tempo2 = (initparam + OUT_JUDGE2) -> value;
      //get value
      (initparam + OUT_JUDGE1) -> value = b2data_d;
      //set value
      (initparam + OUT_JUDGE2) -> value = b2data_u;
      //set value
      old_data = (tempo2 << 8) + tempo1;
	  break;
    default:
      old_data = (initparam + pname) -> value;
      // get value
      (initparam + pname) -> value = data;
      // set value
	  break;
  }
  return(old_data);
}


 // *********************************************************************** Set Data To Register Function ***********************************************************************
int SetReg(interface_t* param)
{
  int tempo;
  int tempo0;
  int tempo1;

  printf(" NO SET : 0x00, TRIM REG : 0x01, TEST REG : 0x02, ALL REG(TRIM + TEST) : 0x03 -> ");
  DigitInput(&tempo);
  if(TRIM_REG & tempo){
    SetValue(IN_REG,param,TRIM_REG);
    while(1){
      ParamConditionDisp(PDIN_REG,NO_BMECR,param);
      printf("enter Chenge TRIM REG Number(0~39) or exit(e) ->");
      DigitInput(&tempo0);
      if(EXIT==tempo0) break;
      printf("IN_ETLR%d = 0x%2x",tempo0,(param+tempo0+IN_ETLR00) -> value);
      printf("enter set Value(ex. 0x8f) -> ");
      DigitInput(&tempo1);
      switch(tempo0){
      case   0:SetValue(IN_ETLR00,param,tempo1);break;
      case   1:SetValue(IN_ETLR01,param,tempo1);break;
      case   2:SetValue(IN_ETLR02,param,tempo1);break;
      case   3:SetValue(IN_ETLR03,param,tempo1);break;
      case   4:SetValue(IN_ETLR04,param,tempo1);break;
      case   5:SetValue(IN_ETLR05,param,tempo1);break;
      case   6:SetValue(IN_ETLR06,param,tempo1);break;
      case   7:SetValue(IN_ETLR07,param,tempo1);break;
      case   8:SetValue(IN_ETLR08,param,tempo1);break;
      case   9:SetValue(IN_ETLR09,param,tempo1);break;
      case  10:SetValue(IN_ETLR10,param,tempo1);break;
      case  11:SetValue(IN_ETLR11,param,tempo1);break;
      case  12:SetValue(IN_ETLR12,param,tempo1);break;
      case  13:SetValue(IN_ETLR13,param,tempo1);break;
      case  14:SetValue(IN_ETLR14,param,tempo1);break;
      case  15:SetValue(IN_ETLR15,param,tempo1);break;
      case  16:SetValue(IN_ETLR16,param,tempo1);break;
      case  17:SetValue(IN_ETLR17,param,tempo1);break;
      case  18:SetValue(IN_ETLR18,param,tempo1);break;
      case  19:SetValue(IN_ETLR19,param,tempo1);break;
      case  20:SetValue(IN_ETLR20,param,tempo1);break;
      case  21:SetValue(IN_ETLR21,param,tempo1);break;
      case  22:SetValue(IN_ETLR22,param,tempo1);break;
      case  23:SetValue(IN_ETLR23,param,tempo1);break;
      case  24:SetValue(IN_ETLR24,param,tempo1);break;
      case  25:SetValue(IN_ETLR25,param,tempo1);break;
      case  26:SetValue(IN_ETLR26,param,tempo1);break;
      case  27:SetValue(IN_ETLR27,param,tempo1);break;
      case  28:SetValue(IN_ETLR28,param,tempo1);break;
      case  29:SetValue(IN_ETLR29,param,tempo1);break;
      case  30:SetValue(IN_ETLR30,param,tempo1);break;
      case  31:SetValue(IN_ETLR31,param,tempo1);break;
      case  32:SetValue(IN_ETLR32,param,tempo1);break;
      case  33:SetValue(IN_ETLR33,param,tempo1);break;
      case  34:SetValue(IN_ETLR34,param,tempo1);break;
      case  35:SetValue(IN_ETLR35,param,tempo1);break;
      case  36:SetValue(IN_ETLR36,param,tempo1);break;
      case  37:SetValue(IN_ETLR37,param,tempo1);break;
      case  38:SetValue(IN_ETLR38,param,tempo1);break;
      case  39:SetValue(IN_ETLR39,param,tempo1);break;
      default:printf(" !!! Input Error !!!");break;
      }
      // switch
    }
    // while
  }
  if(TEST_REG & tempo){
    SetValue(IN_REG,param,TEST_REG);
    while(1){
      ParamConditionDisp(PDIN_REG,NO_BMECR,param);
      printf("enter Chenge TEST REG Number(0~13) or exit(e) ->");
      DigitInput(&tempo0);
      if(EXIT==tempo0) break;
      printf("IN_ETCR%2d = 0x%2x",tempo0,(param+tempo0+IN_ETCR00) -> value);
      printf("enter set Value(ex. 0x8f) -> ");
      DigitInput(&tempo1);
      switch(tempo0){
      case   0:SetValue(IN_ETCR00,param,tempo1);break;
      case   1:SetValue(IN_ETCR01,param,tempo1);break;
      case   2:SetValue(IN_ETCR02,param,tempo1);break;
      case   3:SetValue(IN_ETCR03,param,tempo1);break;
      case   4:SetValue(IN_ETCR04,param,tempo1);break;
      case   5:SetValue(IN_ETCR05,param,tempo1);break;
      case   6:SetValue(IN_ETCR06,param,tempo1);break;
      case   7:SetValue(IN_ETCR07,param,tempo1);break;
      case   8:SetValue(IN_ETCR08,param,tempo1);break;
      case   9:SetValue(IN_ETCR09,param,tempo1);break;
      case  10:SetValue(IN_ETCR10,param,tempo1);break;
      case  11:SetValue(IN_ETCR11,param,tempo1);break;
      case  12:SetValue(IN_ETCR12,param,tempo1);break;
      case  13:SetValue(IN_ETCR13,param,tempo1);break;
      default:printf(" !!! Input Error !!!");break;
      }
      // switch
    }
    // while
  }
  SetValue(IN_REG,param,tempo);

}

 // *********************************************************************** Set IN_BDATA IN_LDATA ***********************************************************************
int SetInBDataLData(interface_t* param)
{
  int i, tempo0, tempo, b_max, l_max;
  b_max = IN_BYTEEND - IN_BDATA00;
  l_max = IN_LWORDEND - IN_LDATA0;
  tempo = 0;
  while( EXIT!=tempo0 ){
    for( i=0; i<b_max; i++ ){
      printf( "IN_BDATA%d = 0x%02X (%d)", i, GetValue( IN_BDATA00+i, param ), i );
      if( 3==i%4 ) printf( "" );
      else         printf( " / " );
    }
    printf( "" );
    for( i=0; i<l_max; i++ ){
      printf( "IN_LDATA%d = 0x%08X (%d)", i, GetValue( IN_LDATA0+i, param ), i+20 );
      if( 3==i%4 ) printf( "" );
      else         printf( " / " );
    }
    printf( "exit(e)  enter Chenge Number -> " ); DigitInput( &tempo0 );
    if( ( 0<=tempo0 && tempo0<b_max ) || ( 20<=tempo0 && tempo0<l_max+20 ) ){
      printf( "enter set Value(ex. 0x8f) -> " ); DigitInput( &tempo );
      if( tempo0<b_max ) SetValue( IN_BDATA00+tempo0, param, tempo );
      else               SetValue( IN_LDATA0+tempo0-20, param, tempo );
    }
  }
  return( PASS );
}

// ********************************************************************** Get Value To Data Function **********************************************************************
int GetValue(int pname,interface_t* initparam)
{
  int old_data,tempo1,tempo2;
  
  switch(pname){
  
    case IN_EXTAL2 :
    case IN_EXTAL1 :
      tempo2 = (initparam + IN_EXTAL1) -> value;
      //get value
      tempo1 = (initparam + IN_EXTAL2) -> value;
      //get value
      old_data = (tempo2 << 8) + tempo1;
	  break;
    case OUT_JUDGE1 :
    case OUT_JUDGE2 :
      tempo2 = (initparam + OUT_JUDGE1) -> value;
      //get value
      tempo1 = (initparam + OUT_JUDGE2) -> value;
      //get value
      old_data = (tempo2 << 8) + tempo1;
	  break;
  
    default:
      old_data = (initparam + pname) -> value;
      // get value
	  break;
  }
  return(old_data);
}

 // *********************************************************************** InitialValue Function ***********************************************************************
void InitialValue(interface_t* initparam)
{
  int cnt;
    // init parameter value set
  SetValue(IN_ETLR00, initparam,  INI_ETLR00    );
  SetValue(IN_ETLR01, initparam,  INI_ETLR01    );
  SetValue(IN_ETLR02, initparam,  INI_ETLR02    );
  SetValue(IN_ETLR03, initparam,  INI_ETLR03    );
  SetValue(IN_ETLR04, initparam,  INI_ETLR04    );
  SetValue(IN_ETLR05, initparam,  INI_ETLR05    );
  SetValue(IN_ETLR06, initparam,  INI_ETLR06    );
  SetValue(IN_ETLR07, initparam,  INI_ETLR07    );
  SetValue(IN_ETLR08, initparam,  INI_ETLR08    );
  SetValue(IN_ETLR09, initparam,  INI_ETLR09    );
  SetValue(IN_ETLR10, initparam,  INI_ETLR10    );
  SetValue(IN_ETLR11, initparam,  INI_ETLR11    );
  SetValue(IN_ETLR12, initparam,  INI_ETLR12    );
  SetValue(IN_ETLR13, initparam,  INI_ETLR13    );
  SetValue(IN_ETLR14, initparam,  INI_ETLR14    );
  SetValue(IN_ETLR15, initparam,  INI_ETLR15    );
  SetValue(IN_ETLR16, initparam,  INI_ETLR16    );
  SetValue(IN_ETLR17, initparam,  INI_ETLR17    );
  SetValue(IN_ETLR18, initparam,  INI_ETLR18    );
  SetValue(IN_ETLR19, initparam,  INI_ETLR19    );
  SetValue(IN_ETLR20, initparam,  INI_ETLR20    );
  SetValue(IN_ETLR21, initparam,  INI_ETLR21    );
  SetValue(IN_ETLR22, initparam,  INI_ETLR22    );
  SetValue(IN_ETLR23, initparam,  INI_ETLR23    );
  SetValue(IN_ETLR24, initparam,  INI_ETLR24    );
  SetValue(IN_ETLR25, initparam,  INI_ETLR25    );
  SetValue(IN_ETLR26, initparam,  INI_ETLR26    );
  SetValue(IN_ETLR27, initparam,  INI_ETLR27    );
  SetValue(IN_ETLR28, initparam,  INI_ETLR28    );
  SetValue(IN_ETLR29, initparam,  INI_ETLR29    );
  SetValue(IN_ETLR30, initparam,  INI_ETLR30    );
  SetValue(IN_ETLR31, initparam,  INI_ETLR31    );
  SetValue(IN_ETLR32, initparam,  INI_ETLR32    );
  SetValue(IN_ETLR33, initparam,  INI_ETLR33    );
  SetValue(IN_ETLR34, initparam,  INI_ETLR34    );
  SetValue(IN_ETLR35, initparam,  INI_ETLR35    );
  SetValue(IN_ETLR36, initparam,  INI_ETLR36    );
  SetValue(IN_ETLR37, initparam,  INI_ETLR37    );
  SetValue(IN_ETLR38, initparam,  INI_ETLR38    );
  SetValue(IN_ETLR39, initparam,  INI_ETLR39    );
  SetValue(IN_ETCR00, initparam,  INI_ETCR00    );
  SetValue(IN_ETCR01, initparam,  INI_ETCR01    );
  SetValue(IN_ETCR02, initparam,  INI_ETCR02    );
  SetValue(IN_ETCR03, initparam,  INI_ETCR03    );
  SetValue(IN_ETCR04, initparam,  INI_ETCR04    );
  SetValue(IN_ETCR05, initparam,  INI_ETCR05    );
  SetValue(IN_ETCR06, initparam,  INI_ETCR06    );
  SetValue(IN_ETCR07, initparam,  INI_ETCR07    );
  SetValue(IN_ETCR08, initparam,  INI_ETCR08    );
  SetValue(IN_ETCR09, initparam,  INI_ETCR09    );
  SetValue(IN_ETCR10, initparam,  INI_ETCR10    );
  SetValue(IN_ETCR11, initparam,  INI_ETCR11    );
  SetValue(IN_ETCR12, initparam,  INI_ETCR12    );
  SetValue(IN_ETCR13, initparam,  INI_ETCR13    );
  SetValue(IN_EXTAL1, initparam,  INI_EXTAL1    );
  SetValue(IN_PLLON, initparam,  INI_PLLON    );
  SetValue(IN_PLL_MULT, initparam,  INI_PLL_MULT    );
  SetValue(IN_PLL_MULT_VAL, initparam,  INI_PLL_MULT_VAL    );
  SetValue(IN_PLL_DIV, initparam,  INI_PLL_DIV    );
  SetValue(IN_PLL_DIV_VAL, initparam,  INI_PLL_DIV_VAL    );
  SetValue(IN_ICLK, initparam,  INI_ICLK    );
  SetValue(IN_ICLK_VAL, initparam,  INI_ICLK_VAL    );
  SetValue(IN_BCLK, initparam,  INI_BCLK    );
  SetValue(IN_BCLK_VAL, initparam,  INI_BCLK_VAL    );
  SetValue(IN_FCLK, initparam,  INI_FCLK    );
  SetValue(IN_FCLK_VAL, initparam,  INI_FCLK_VAL    );
  SetValue(IN_PCLK, initparam,  INI_PCLK    );
  SetValue(IN_PCLK_VAL, initparam,  INI_PCLK_VAL    );
  SetValue(IN_MEMWAIT, initparam,  INI_MEMWAIT    );
  SetValue(IN_REG, initparam,  INI_REG    );
  SetValue(IN_AREA, initparam,  INI_AREA    );
  SetValue(IN_TESTSEL, initparam,  INI_TESTSEL    );
  SetValue(IN_WRITESIZE, initparam,  INI_WRITESIZE    );
  SetValue(IN_VCC_MODE, initparam,  INI_VCC_MODE    );
  SetValue(IN_VDD_TAP, initparam,  INI_VDD_TAP    );
  SetValue(IN_VDDH_TAP, initparam,  INI_VDDH_TAP    );
  SetValue(IN_LOWPOWER, initparam,  INI_LOWPOWER    );
  SetValue(IN_NOFAILSTOP, initparam,  INI_NOFAILSTOP    );
  SetValue(IN_BDATA00, initparam,  INI_BDATA00    );
  SetValue(IN_BDATA01, initparam,  INI_BDATA01    );
  SetValue(IN_BDATA02, initparam,  INI_BDATA02    );
  SetValue(IN_BDATA03, initparam,  INI_BDATA03    );
  SetValue(IN_BDATA04, initparam,  INI_BDATA04    );
  SetValue(IN_BDATA05, initparam,  INI_BDATA05    );
  SetValue(IN_BDATA06, initparam,  INI_BDATA06    );
  SetValue(IN_BDATA07, initparam,  INI_BDATA07    );
  SetValue(IN_BDATA08, initparam,  INI_BDATA08    );
  SetValue(IN_BDATA09, initparam,  INI_BDATA09    );
  SetValue(IN_BDATA10, initparam,  INI_BDATA10    );
  SetValue(IN_BDATA11, initparam,  INI_BDATA11    );
  SetValue(IN_BDATA12, initparam,  INI_BDATA12    );
  SetValue(IN_BDATA13, initparam,  INI_BDATA13    );
  SetValue(IN_BDATA14, initparam,  INI_BDATA14    );
  SetValue(IN_BDATA15, initparam,  INI_BDATA15    );
  SetValue(IN_BDATA16, initparam,  INI_BDATA16    );
  SetValue(IN_TOPADDR0, initparam,  INI_TOPADDR0    );
  SetValue(IN_TESTSIZE0, initparam,  INI_TESTSIZE0    );
  SetValue(IN_TOPADDR1, initparam,  INI_TOPADDR1    );
  SetValue(IN_TESTSIZE1, initparam,  INI_TESTSIZE1    );
  SetValue(IN_TOPADDR2, initparam,  INI_TOPADDR2    );
  SetValue(IN_TESTSIZE2, initparam,  INI_TESTSIZE2    );
  SetValue(IN_LDATA0, initparam,  INI_LDATA0    );
  SetValue(IN_LDATA1, initparam,  INI_LDATA1    );
  SetValue(IN_LDATA2, initparam,  INI_LDATA2    );
  SetValue(IN_LDATA3, initparam,  INI_LDATA3    );
  SetValue(IN_LDATA4, initparam,  INI_LDATA4    );
  SetValue(IN_LDATA5, initparam,  INI_LDATA5    );

  for(cnt=OUT_ETLR00;cnt<=OUT_LDATA4;cnt++){
    SetValue(cnt,initparam,0);
  }
}


 // *********************************************************************** Return to TAP Voltage ***********************************************************************
int ReturnTapV(int tap)
{
  static int tap_v[144] = {
    -4800, -4725, -4650, -4575, -4500, -4425, -4350, -4275, -4200, -4125,
    -4050, -3975, -3900, -3825, -3750, -3675, -3600, -3525, -3450, -3375,
    -3300, -3225, -3150, -3075, -3000, -2925, -2850, -2775, -2700, -2625,
    -2550, -2475, -2400, -2325, -2250, -2175, -2100, -2025, -1950, -1875,
    -1800, -1725, -1650, -1575, -1500, -1425, -1350, -1275, -1200, -1125,
    -1050,  -975,  -900,  -825,  -750,  -675,  -600,  -525,  -450,  -375,
     -300,  -225,  -150,   -75,     0,    75,   150,   225,   300,   375,
      450,   525,
      // tap No 0-71   EV
                    600,   675,   750,   825,   900,   975,  1050,  1125,
                    // tap No 72-143 PV
     1200,  1275,  1350,  1425,  1500,  1575,  1650,  1725,  1800,  1875,
     1950,  2025,  2100,  2175,  2250,  2325,  2400,  2475,  2550,  2625,
     2700,  2775,  2850,  2925,  3000,  3075,  3150,  3225,  3300,  3375,
     3450,  3525,  3600,  3675,  3750,  3825,  3900,  3975,  4050,  4125,
     4200,  4275,  4350,  4425,  4500,  4575,  4650,  4725,  4800,  4875,
     4950,  5025,  5100,  5175,  5250,  5325,  5400,  5475,  5550,  5625,
     5700,  5775,  5850,  5925
  };
//  return(tap*75-4800);
  return(tap_v[tap]);
}

 // *********************************************************************** Read Ecr Base Function ***********************************************************************

// int EcrTextOut(int mode,int top_addr,int end_addr,int readsize,char* filename) {  //--------------------------------------------------------------------------  // Values  //-------------------------------------------------------------------------- int i, j, k; int data, max_data; int tempo; int level[13]; char l_label[13][7]; char baselog[256]; char filename_tempo[256]; int mat, ivsa, area, posinega, open_flag; char time_buf[32]; FILE_T *temp_ptr;   //--------------------------------------------------------------------------  // Initialize  //-------------------------------------------------------------------------- i = j = k = data = max_data = tempo = 0; for(i=0; i<13; i++){ level[i]=0; } strcpy(l_label[0],  "    Hi"); strcpy(l_label[1],  "99.999"); strcpy(l_label[2],  " 99.99"); strcpy(l_label[3],  "  99.9"); strcpy(l_label[4],  "    99"); strcpy(l_label[5],  "    90"); strcpy(l_label[6],  "    50"); strcpy(l_label[7],  "    10"); strcpy(l_label[8],  "     1"); strcpy(l_label[9],  "   0.1"); strcpy(l_label[10], "  0.01"); strcpy(l_label[11], " 0.001"); strcpy(l_label[12], "    Lo"); memset(baselog, 0, sizeof(baselog)); memset(outbuf, 0, sizeof(outbuf));   //--------------------------------------------------------------------------  // Main  //-------------------------------------------------------------------------- open_flag = 0; if( mode==4 ){ strcpy( baselog, glob_cdp ); strcat( baselog, glob_probe ); strcpy( baselog, filename ); }else if ( mode==0 || mode==1 ){ sprintf( filename_tempo, "%s_%d.csv", filename, get_site_number() ); strcpy(baselog,CURRENTDR); strcat(baselog,glob_bunpu); strcat(baselog,filename_tempo); if( (temp_ptr=fopen(baselog,"rt"))==NULL ){ open_flag = 1; }else{ if( fclose(temp_ptr) ) exit(-10); } } else if ( mode == 20) { strcpy(baselog,CURRENTDR); strcat(baselog,glob_datalog); strcat(baselog,filename); }else{ strcpy(baselog,CURRENTDR); strcat(baselog,glob_bunpu); strcat(baselog,filename); }  if((temp_ptr=fopen(baselog,"at"))==NULL) { printf(" -- FAIL -- not open file:%s !",baselog); return(FAIL); }  if( mode==1 ){ // get mat ivsa from bm data if( open_flag==1 ){ fprintf( temp_ptr, "[X-Y],Mat,measI,Area,Time" ); for( i=143; i>=0; i-- ) fprintf( temp_ptr, ",%7.7s", &voltage_string[i][0] ); fprintf( temp_ptr, "" ); } mat = GetValueBmIIF( IN_MDL ); posinega = 0; if( mat==FLI1 ){ ivsa = ( GetValueBmIIF( IN_TESTR9_E2P )>>4 )&0x0F; posinega = 1; if( GetValueBmIIF( IN_TESTR8_E2P )==0x02 && GetValueBmIIF( IN_TESTR10_E2P )==0x01 ) posinega = 2; if( 0x010255AA==ReadEcrMode8( DISTRIBUTION_END+4, SIZE_L ) ) posinega = 3; // PosiNega Log header flag }else{ ivsa = GetValueBmIIF( IN_TESTR16_FLP )&0x0F; } fprintf( temp_ptr, "[%3d-%3d],%4.4s,%4.4s,", xy_location[0], xy_location[1], &mat_string[mat][0], &ivsa_string[ivsa][0] ); area = GetValueBmIIF( IN_SW_AREA ); if( 0x01&area ) fprintf( temp_ptr, "DATA" ); if( 0x04&area ) fprintf( temp_ptr, "LK" ); if( posinega==1 || 3==posinega ) fprintf( temp_ptr, " Posi" ); if( posinega==2 || 3==posinega ) fprintf( temp_ptr, " Nega" ); time( &time_list2 ); GetDateTime( &time_list2, time_buf, 1 ); fprintf( temp_ptr, ",%s,", time_buf ); }  if(mode==0 || mode==1 || mode==4 || mode==20 ){ //Yoko if(top_addr>=end_addr){ for(i=top_addr;i>=end_addr;i-=readsize){ fprintf(temp_ptr,"%d,",ReadEcrMode8(i,readsize)); } }else if(top_addr<=end_addr){ for(i=end_addr-3;i>=top_addr;i-=readsize){ fprintf(temp_ptr,"%d,",ReadEcrMode8(i,readsize)); } } }else if(mode==2 || mode==3){ //Tate data=ReadEcrMode8(end_addr-3,readsize); printf("*** Max bit ***"); printf("TOTAL        :%8d (0)",data); printf("FLI-ALL[1536K]: 12582912 (1)"); printf("FLI     [512K]:  4194304 (2)"); printf("Big-EB   [64K]:   524288 (3)"); printf("FLD-ALL  [32K]:   262144 (4)"); printf("1Unit    [16K]:   131072 (5)"); printf("Other (6)"); printf("Input Number ->"); DigitInput(&tempo); switch (tempo) { case 0: max_data=data; break; // Max Bit = Total Count case 1: max_data=4194304; break; // Max Bit = 4194304 case 2: max_data=4194304; break; // Max Bit = 4194304 case 3: max_data=524288; break; // Max Bit = 524288 case 4: max_data=262144; break; // Max Bit = 262144 case 5: max_data=131072; break; // Max Bit = 131072 case 6: // Max Bit Input printf("Input Serch Bit -> "); DigitInput(&max_data); break; default: break; } if(mode==3){ // CHK               // Checker -> Max Bit / 2 max_data=max_data/2; } if(top_addr>=end_addr){ for(i=top_addr;i>=end_addr;i-=readsize){ fprintf(temp_ptr,"%d",ReadEcrMode8(i,readsize)); } }else if(top_addr<=end_addr){ j=0; i=end_addr-3; for(k=0; k<13; k++){ // Search (level[0]~[12]) for(;i>=top_addr;i-=readsize){ data=ReadEcrMode8(i,readsize); if(0==k){ // Search High Level (level[0]) if(data<(max_data)){ break; } }else if( (1<=k)&&(k<=5) ){ // Search 99.999%~90% (level[1]~[5]) if( data<( max_data-( max_data/pow(10,(6-k)) ) ) ){ break; } }else if(6==k){ // Search 50% (level[6]) if (data<(max_data/2)){ break; } }else if( (7<=k)&&(k<=11) ){ // Search 10%~0.001% (level[7]~[11]) if( data<( max_data/pow(10,(k-6)) ) ){ break; } }else{ // Search Low Level (level[12]) if(data<1){ break; } } j++; } level[k]=j-1; sprintf(outbuf,"%s: %5dmV",l_label[k],ReturnTapV(level[k])); DataOut(Ffpt,outbuf); }  //----------------------------------------------------------------------  // Print Distribution Graph  //---------------------------------------------------------------------- sprintf(outbuf,"      |----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+"); DataOut(Ffpt,outbuf); for(k=0; k<13; k++){ // Output Distribution Data if( (4<k)&&(k<9) ){ // Print Spacer at Middle Range sprintf(outbuf,"      |"); DataOut(Ffpt,outbuf); }  //---- Label --------------------------------------------------------- sprintf(outbuf,"%s|",l_label[k]); // Print Label DataOut(Ffpt,outbuf);  //---- Holyzontal Space & Line --------------------------------------- for (i=0;i<119-level[k];i++){ if(6==k){ // Distribution 50% sprintf(outbuf,"-"); //  Hilyzontal Line (Before Point) }else{                        // Other s } DataOut(Ffpt,outbuf); }  //---- Point --------------------------------------------------------- sprintf(outbuf,"*"); // Print Search Level DataOut(Ffpt,outbuf);  //---- Hilyzontal Line ----------------------------------------------- if(6==k){ // Only Distribution 50% for(i=119-level[k];i<119;i++){ sprintf(outbuf,"-"); // Hilyzontal Line (After Point) DataOut(Ffpt,outbuf); } }  //---- CRLF ---------------------------------------------------------- sprintf(outbuf,""); DataOut(Ffpt,outbuf); } sprintf(outbuf,"      |----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+"); DataOut(Ffpt,outbuf); sprintf(outbuf,"     -4        -3        -2        -1         0         1         2         3         4         5         6         7         8"); DataOut(Ffpt,outbuf); } }  fprintf(temp_ptr,"");  if(fclose(temp_ptr)){ exit(-10); } return(PASS);  }

 // *********************************************************************** Analyze Probe AT Test End Function ***********************************************************************

// ****************************************************************************

int SelectAddress(interface_t* param,int command)
{

  int tempo,tempo0,tempo2,size_tempo,addr_tempo,addr_end_tempo;
  int flp0_sel,fli1_sel,fli2_sel,com_flag;
  char flp0_status[3],fli1_status[3],fli2_status[3],ext1_status[3],ext2_status[3],ext3_status[3],ext4_status[3],ext5_status[3],ext6_status[3];

  tempo=1;
  com_flag = 1;
  while(tempo){
     
        //---------------status & size & addr Initialize---------------//
    sprintf(flp0_status,"OFF");
    sprintf(fli1_status,"OFF");
    sprintf(fli2_status,"OFF");
    sprintf(ext1_status,"OFF");
    sprintf(ext2_status,"OFF");
    sprintf(ext3_status,"OFF");
    sprintf(ext4_status,"OFF");
    sprintf(ext5_status,"OFF");
    sprintf(ext6_status,"OFF");
    size_tempo = addr_tempo = 0;

    if(GetValue(IN_AREA,param) == 0){
    // User Mat Select
      if(GetValue(IN_TESTSIZE0,param) != 0){
      // FLP0 Select
        sprintf(flp0_status,"ON ");
        size_tempo = GetValue(IN_TESTSIZE0,param);
        addr_tempo = GetValue( IN_TOPADDR0,param);
      }
      if(GetValue(IN_TESTSIZE1,param) != 0){
      // FLI1 Select
        sprintf(fli1_status,"ON ");
        if(((size_tempo != GetValue(IN_TESTSIZE1,param))
          ||(addr_tempo != GetValue(IN_TOPADDR1,param) ))
          &&(GetValue(IN_TESTSIZE0,param) != 0         )){
          // FLP0FLI1
	  size_tempo=0;
        }else{
        // FLP0=FLI1(or NO FLP0)
          size_tempo = GetValue(IN_TESTSIZE1,param);
          addr_tempo = GetValue(IN_TOPADDR1,param);
        }
      }
      if(GetValue(IN_TESTSIZE2,param) != 0){
      // FLI2 Select
        sprintf(fli2_status,"ON ");
        if( ((size_tempo != GetValue(IN_TESTSIZE2,param))
           ||(addr_tempo != GetValue( IN_TOPADDR2,param)))
          &&((GetValue(IN_TESTSIZE0,param) != 0         )
           ||(GetValue(IN_TESTSIZE1,param) != 0         ))){
           // FLP0FLI2 or FLI1FLI2
	  size_tempo=0;
        }else{
        // FLI1=FLI2(or NO FLI1,FLP0)
          size_tempo = GetValue(IN_TESTSIZE2,param);
          addr_tempo = GetValue(IN_TOPADDR2,param);
        }
      }

    }else{
    // EXTRA Select
      if(GetValue(IN_AREA,param) == 1) sprintf(ext1_status,"ON ");
      if(GetValue(IN_AREA,param) == 2) sprintf(ext2_status,"ON ");
      if(GetValue(IN_AREA,param) == 3) sprintf(ext3_status,"ON ");
      if(GetValue(IN_AREA,param) == 4) sprintf(ext4_status,"ON ");
      if(GetValue(IN_AREA,param) == 5) sprintf(ext5_status,"ON ");
      if(GetValue(IN_AREA,param) == 6) sprintf(ext6_status,"ON ");
      size_tempo = GetValue(IN_TESTSIZE0,param);
      addr_tempo = GetValue( IN_TOPADDR0,param);
    }
    
        //-------------------Addres Menu--------------------------//
    printf(" ===========  Module(mdl)  ===========");
    printf(" FLP0 : %s, FLI1 : %s, FLI2 : %s",flp0_status,fli1_status,fli2_status);
    printf(" EXT1 : %s, EXT2 : %s, EXT3 : %s",ext1_status,ext2_status,ext3_status);
    printf(" EXT4 : %s, EXT5 : %s, EXT6 : %s",ext4_status,ext5_status,ext6_status);
    printf(" ===========  Addres(adr)  ===========");
    if(size_tempo != 0){
      printf(" Addres = 0x%X ~ 0x%X ",addr_tempo,addr_tempo + size_tempo - 1);
    }else{
      printf(" FLP0 Addres = 0x%X ~ 0x%X ",GetValue( IN_TOPADDR0,param),GetValue( IN_TOPADDR0,param)+GetValue(IN_TESTSIZE0,param)-1);
      printf(" FLI1 Addres = 0x%X ~ 0x%X ",GetValue( IN_TOPADDR1,param),GetValue( IN_TOPADDR1,param)+GetValue(IN_TESTSIZE1,param)-1);
      printf(" FLI2 Addres = 0x%X ~ 0x%X ",GetValue( IN_TOPADDR2,param),GetValue( IN_TOPADDR2,param)+GetValue(IN_TESTSIZE2,param)-1);
    }
    printf(" ===========     Other     ===========");
    printf(" flexible    (99)");
    printf("-> ");
    if(com_flag != 1) DigitInput(&tempo0);
    else tempo0 = command;
    com_flag = 0;
    
        //----------------------first Command----------------------------//    
    switch(tempo0){
      case EXIT:
      case QUIT:
        tempo = 0;
        break;
      case SET_MDL:
      // Module Select
        printf(" ====== Select Module =====");
	printf(" UserMAT ALL (99)");
        printf(" FLP0   ( 0)");
        printf(" FLI1   ( 1)");
        printf(" FLI2   ( 2)");
        printf(" EXTRA1 (11)");
        printf(" EXTRA2 (12)");
        printf(" EXTRA3 (13)");
        printf(" EXTRA4 (14)");
        printf(" EXTRA5 (15)");
        printf(" EXTRA6 (16)");
	printf(" ==========================");
        printf("-> ");
        DigitInput(&tempo2);
	
	switch(tempo2){
	  case 99:
	    SetValue(IN_AREA,param,0);
	    SetValue(IN_TOPADDR0,param,FLP0_TOP);
	    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
	    SetValue(IN_TOPADDR1,param,FLI1_TOP);
	    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
	    SetValue(IN_TOPADDR2,param,FLI2_TOP);
	    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
	    break;
	  case 0:
	    SetValue(IN_AREA,param,0);
	    SetValue(IN_TOPADDR0,param,FLP0_TOP);
	    SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
	    SetValue(IN_TOPADDR1,param,0);
	    SetValue(IN_TESTSIZE1,param,0);
	    SetValue(IN_TOPADDR2,param,0);
	    SetValue(IN_TESTSIZE2,param,0);
	    break;
	  case 1:
	    SetValue(IN_AREA,param,0);
	    SetValue(IN_TOPADDR0,param,0);
	    SetValue(IN_TESTSIZE0,param,0);
	    SetValue(IN_TOPADDR1,param,FLI1_TOP);
	    SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
	    SetValue(IN_TOPADDR2,param,0);
	    SetValue(IN_TESTSIZE2,param,0);
	    break;
	  case 2:
	    SetValue(IN_AREA,param,0);
	    SetValue(IN_TOPADDR0,param,0);
	    SetValue(IN_TESTSIZE0,param,0);
	    SetValue(IN_TOPADDR1,param,0);
	    SetValue(IN_TESTSIZE1,param,0);
	    SetValue(IN_TOPADDR2,param,FLI2_TOP);
	    SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
	    break;
	  case 11:
	  case 12:
	  case 13:
	  case 14:
	  case 15:
	    SetValue(IN_AREA,param,tempo2-10);
	    SetValue(IN_TOPADDR0,param,EXTRAS_TOP);
	    SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
	    SetValue(IN_TOPADDR1,param,0);
	    SetValue(IN_TESTSIZE1,param,0);
	    SetValue(IN_TOPADDR2,param,0);
	    SetValue(IN_TESTSIZE2,param,0);
	    break;
	  case 16:
	    SetValue(IN_AREA,param,tempo2-10);
	    SetValue(IN_TOPADDR0,param,EXTRAB_TOP);
	    SetValue(IN_TESTSIZE0,param,EXTRAB_SIZE);
	    SetValue(IN_TOPADDR1,param,0);
	    SetValue(IN_TESTSIZE1,param,0);
	    SetValue(IN_TOPADDR2,param,0);
	    SetValue(IN_TESTSIZE2,param,0);
	    break;
	}
	break;
      case SET_ADR:
      //Addres Select
        printf(" Start Addres =>");DigitInput(&tempo2);
	addr_tempo = tempo2;
	printf(" End Addres =>");DigitInput(&tempo2);
	size_tempo = tempo2 - addr_tempo +1;
	if(GetValue(IN_AREA,param) == 0){
          if(strcmp(&flp0_status,"ON ")==0){
          //FLP0 Addres Set
	    SetValue(IN_TOPADDR0,param,addr_tempo);
            SetValue(IN_TESTSIZE0,param,size_tempo);
	  }
	  if(strcmp(&fli1_status,"ON ")==0){
	  //FLI1 Addres Set
	    SetValue(IN_TOPADDR1,param,addr_tempo);
	    SetValue(IN_TESTSIZE1,param,size_tempo);
	  }
	  if(strcmp(&fli2_status,"ON ")==0){
	  //FLI2 Addres Set
	    SetValue(IN_TOPADDR2,param,addr_tempo);
	    SetValue(IN_TESTSIZE2,param,size_tempo);
	  }
        }else{
	  SetValue(IN_TOPADDR0,param,addr_tempo);
          SetValue(IN_TESTSIZE0,param,size_tempo);
	}
	break;
      case 99:
      //flexible Set
        printf(" AREA(User:0,EXTRA1:1 ~ EXTRA6:6)=>");
	DigitInput(&tempo2);SetValue(IN_AREA,param,tempo2);
        printf(" TOP_ADDR0(FLP0) =>");
        DigitInput(&tempo2);SetValue(IN_TOPADDR0,param,tempo2);
	printf(" TEST_SIZE0(FLP0) =>");
        DigitInput(&tempo2);SetValue(IN_TESTSIZE0,param,tempo2);
	printf(" TOP_ADDR1(FLI1) =>");
        DigitInput(&tempo2);SetValue(IN_TOPADDR1,param,tempo2);
	printf(" TEST_SIZE1(FLI1) =>");
        DigitInput(&tempo2);SetValue(IN_TESTSIZE1,param,tempo2);
	printf(" TOP_ADDR2(FLI2) =>");
        DigitInput(&tempo2);SetValue(IN_TOPADDR2,param,tempo2);
	printf(" TEST_SIZE2(FLI2) =>");
        DigitInput(&tempo2);SetValue(IN_TESTSIZE2,param,tempo2);
	break;
    }
     
    
        // -----------Area & Addres Check---------------//
    if(0<=GetValue(IN_AREA,param) && GetValue(IN_AREA,param)<=6){
    //Area Check
    
    }else{
      printf("AREA Select ERROR!!! AREA = %d, Area & Addres Value Reset!! => NO Select",GetValue(IN_AREA,param));
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    }
    if(GetValue(IN_TESTSIZE0,param) != 0){
    //FLP0 or EXTRA Check
      addr_end_tempo = GetValue(IN_TESTSIZE0,param) + GetValue(IN_TOPADDR0,param) -1;
      addr_tempo = GetValue(IN_TOPADDR0,param);
      switch(GetValue(IN_AREA,param)){
        case 0:
        //Addres Cehck for FLP0
	  if((FLP0_TOP<=addr_tempo) && (addr_tempo<=FLP0_END)){
	    
          }else{
	    printf("TopAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  if((FLP0_TOP<=addr_end_tempo) && (addr_end_tempo<=FLP0_END)){
	    
          }else{
	    printf("EndAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_end_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  break;
	case 1:
	//Addres Cehck for EXTRA1
	case 2:
	//Addres Cehck for EXTRA2
	case 3:
	//Addres Cehck for EXTRA3
	case 4:
	//Addres Cehck for EXTRA4
	case 5:
	//Addres Cehck for EXTRA5
	  if((EXTRAS_TOP<=addr_tempo) && (addr_tempo<=EXTRAS_END)){
	    
          }else{
	    printf("TopAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  if((EXTRAS_TOP<=addr_end_tempo) && (addr_end_tempo<=EXTRAS_END)){
	    
          }else{
	    printf("EndAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_end_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  break;
	case 6:
	//Addres Cehck for EXTRA6
	  if((EXTRAB_TOP<=addr_tempo) && (addr_tempo<=EXTRAB_END)){
	    
          }else{
	    printf("TopAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  if((EXTRAB_TOP<=addr_end_tempo) && (addr_end_tempo<=EXTRAB_END)){
	    
          }else{
	    printf("EndAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_end_tempo);
	    SetValue(IN_AREA,param,0);
            SetValue(IN_TOPADDR0,param,0);
            SetValue(IN_TESTSIZE0,param,0);
            SetValue(IN_TOPADDR1,param,0);
            SetValue(IN_TESTSIZE1,param,0);
            SetValue(IN_TOPADDR2,param,0);
            SetValue(IN_TESTSIZE2,param,0);
	  }
	  break;
      }
    }
    if(GetValue(IN_TESTSIZE1,param) != 0){
      addr_end_tempo = GetValue(IN_TESTSIZE1,param) + GetValue(IN_TOPADDR1,param) -1;
      addr_tempo = GetValue(IN_TOPADDR1,param);
      if((FLI1_TOP<=addr_tempo) && (addr_tempo<=FLI1_END)){
      //Addres Cehck for FLI1
	    
      }else{
        printf("TopAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_tempo);
        SetValue(IN_AREA,param,0);
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }
      if((FLI1_TOP<=addr_end_tempo) && (addr_end_tempo<=FLI1_END)){
	    
      }else{
        printf("EndAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_end_tempo);
        SetValue(IN_AREA,param,0);
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }
    }
    if(GetValue(IN_TESTSIZE2,param) != 0){
      addr_end_tempo = GetValue(IN_TESTSIZE2,param) + GetValue(IN_TOPADDR2,param) -1;
      addr_tempo = GetValue(IN_TOPADDR2,param);
      if((FLI2_TOP<=addr_tempo) && (addr_tempo<=FLI2_END)){
      //Addres Cehck for FLI2
	    
      }else{
        printf("TopAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_tempo);
        SetValue(IN_AREA,param,0);
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }
      if((FLI2_TOP<=addr_end_tempo) && (addr_end_tempo<=FLI2_END)){
	    
      }else{
        printf("EndAddres Select ERROR!!! Addres = %d, Area & Addres Value Reset!! => NO Select",addr_end_tempo);
        SetValue(IN_AREA,param,0);
        SetValue(IN_TOPADDR0,param,0);
        SetValue(IN_TESTSIZE0,param,0);
        SetValue(IN_TOPADDR1,param,0);
        SetValue(IN_TESTSIZE1,param,0);
        SetValue(IN_TOPADDR2,param,0);
        SetValue(IN_TESTSIZE2,param,0);
      }
    }
  }
  return(PASS);
}

int ChangeMat(int mat,interface_t* param)
{

  int oldmat;
  // switch
// ****************************************************************************
// int SaveRegFile(char *filename,interface_t *table)
// ****************************************************************************
  // File Discripter
  // **************************************************************************
  // Generate Data Log File Name & Open This File
  // **************************************************************************
  // Open Data Log File
    // fopen Error
  // **************************************************************************
  // Close Data File
  // **************************************************************************
  // Close Data Log File
    // fclose Error , Exit [code (-10)]
  // Test Complete Sucsesflly
// ****************************************************************************
// int LoadRegFile(char *filename,interface_t *table)
// ****************************************************************************
  // File Discripter
  // **************************************************************************
  // Generate Data Log File Name & Open This File
  // **************************************************************************
  // Open Data Log File
    // fopen Error
  // **************************************************************************
  // Close Data File
  // **************************************************************************
  // Close Data Log File
    // fclose Error , Exit [code (-10)]
  // Test Complete Sucsesflly
// ****************************************************************************

// void DispVSTap(void)

// Display VerifyScan Tap All

// 2004/09/28 Y.Sakurai

// ****************************************************************************
// ****************************************************************************

// void DispIVSAAll(void)

// Display IV-SA All

// 2004/11/26 Y.Sakurai

// ****************************************************************************
// ****************************************************************************

// int pow(int x,int y)

// ****************************************************************************
// ****************************************************************************

// int log_2(x)

// ****************************************************************************
// ****************************************************************************

// int absolute(int num)

// ****************************************************************************
  // Source Position
  // Destination Position
  // text buffer
  // --------------------------------------------------------------------------
  // Initialyze
  // --------------------------------------------------------------------------
  // Back Space Code
  // --------------------------------------------------------------------------
  // Main
  // --------------------------------------------------------------------------
    // ERROR
    // Find Back Space Code
      // Delete Previous Charactor
  // buffer termination
  // ERROR
  // SUCCESS
    // copy available data only
// ****************************************************************************


// ****************************************************************************
// oldmat = GetValue(IN_MDL,param); SetValue(IN_MDL,param,mat);  switch(mat){ case FLP0: SetValue(IN_TOPADDR,param,FLP0_TOP); SetValue(IN_ENDADDR,param,FLP0_END); break; case FLI1: SetValue(IN_TOPADDR,param,FLI1_TOP); SetValue(IN_ENDADDR,param,FLI1_END); break; case FLI2: SetValue(IN_TOPADDR,param,FLI2_TOP); SetValue(IN_ENDADDR,param,FLI2_END); break; case EXTRA1: SetValue(IN_TOPADDR,param,EXTRAS_TOP); SetValue(IN_ENDADDR,param,EXTRAS_END); break; case EXTRA2: SetValue(IN_TOPADDR,param,EXTRAS_TOP); SetValue(IN_ENDADDR,param,EXTRAS_END); break; case EXTRA3: SetValue(IN_TOPADDR,param,EXTRAS_TOP); SetValue(IN_ENDADDR,param,EXTRAS_END); break; case EXTRA4: SetValue(IN_TOPADDR,param,EXTRAS_TOP); SetValue(IN_ENDADDR,param,EXTRAS_END); break; case EXTRA5: SetValue(IN_TOPADDR,param,EXTRAS_TOP); SetValue(IN_ENDADDR,param,EXTRAS_END); break; case EXTRA6: SetValue(IN_TOPADDR,param,EXTRAB_TOP); SetValue(IN_ENDADDR,param,EXTRAB_END); break; default: if((0x01<=mat)&&(mat<=0xFF)){ printf("Selection is not single mat."); }else{ printf("!!! Input Error !!!"); return(FAIL); } break; } return(oldmat); }      int SaveRegFile(char *filename,interface_t *table) { FILE_T *fpt; int i;     strcpy( baselog, glob_cdp ); strcat( baselog, glob_cond ); strcat( baselog, filename ); printf( "%s", baselog ); if((fpt=fopen(baselog,"at"))==NULL){ ErrorPrint(baselog); return(FAIL); }  for(i=IN_ETLR00; i<=IN_ETLR39; i++){ fprintf(fpt, "%d,0x%.2X", i, GetValue(i,table)); }     if(fclose(fpt)){ exit(-10); }  return(PASS); }     int LoadRegFile(char *filename,interface_t *table) { FILE_T *fpt; int i; int num; int count; unsigned int value;     strcpy( baselog, glob_cdp ); strcat( baselog, glob_cond ); strcat( baselog, filename ); printf( "%s", baselog ); if((fpt=fopen(baselog,"r"))==NULL){ ErrorPrint(baselog); return(FAIL); }  for(i=IN_ETLR00; i<=IN_ETLR39; i++){ count = fscanf(fpt, "%d,0x%X",&num ,&value );  //    printf("%d,0x%.2X : %d", num, value, count ); SetValue(num , table, (int)value ); }     if(fclose(fpt)){ exit(-10); }  return(PASS); }            void DispVSTapAll(void) { int i;  printf("+---+------+ +---+------+ +---+------+ +---+------+ +---+------+"); printf("|tap| V(mV)| |tap| V(mV)| |tap| V(mV)| |tap| V(mV)| |tap| V(mV)|"); printf("+---+------+ +---+------+ +---+------+ +---+------+ +---+------+"); for(i=0; i<30; i++){ printf("|%3d| %5d| |%3d| %5d| |%3d| %5d| |%3d| %5d| |%3d| %5d|", i, ReturnTapV(i), i+30, ReturnTapV(i+30), i+60, ReturnTapV(i+60), i+90, ReturnTapV(i+90), i+114, ReturnTapV(i+114)); } printf("+---+------+ +---+------+ +---+------+ +---+------+ +---+------+");  }           void DispIVSAAll(void) {  printf("Select Value   1uA=1, 3uA=2, 5uA=3, 8uA=4, 10uA=5, 13uA=6, 15uA=7, 18uA=8, 20uA=9,"); printf("Select Value   23uA=10, 30uA=11, 40uA=12, 50uA=13, 70uA=14, (invalid(default)=0 -> ");  }       int pow(int x,int y) { int i; int temp;  i=0; temp=1;  for(i=0; i<y; i++){ temp = temp * x; }  return(temp); }      int log2(int x) { int i; int temp;  i=0; temp=1;  for(i=0; i<=32; i++){ if((x>>i)==1) break; }  return(i); }       int absolute(int num) { if( 0 > num ) return -1*num; else return num; }  char *str_bstrim(char *txt) { int bs_code; int s_position; int d_position; char buffer[1024];     s_position = 0; d_position = 0; bs_code = 0x08;     if( NULL==txt ){ return((char *)NULL); }  for(s_position=0,d_position=0; (s_position<strlen(txt))&&(d_position<1023); s_position++){ if(txt[s_position]==bs_code){ if( 0<d_position ){ d_position--; buffer[d_position] = '\0'; } }else{ buffer[d_position] = txt[s_position]; d_position++; } } buffer[d_position] = '\0';  if( d_position<0 ){ return((char *)NULL); }else{ sprintf(txt,"%s",buffer); } }      /* void DistributionForRetention(int vs[NUM_POWERSUPPLY],interface_t* param) { int CurrentMode,CurrentCnt; int ivsa_val2[8]; int mat; int old_testr10; int AdrMode,StartAdr,range,diff,AdrCnt; int stack0,stack1,stack2,stack3; int cnt,tempo,tempo0,tempo1,tempo2,tempo8,tempo9; int i,j,k,l; char *current_s;  CurrentCnt=0; current_s="NULL";  printf("Select Current Mode: Flexible(0) / ALL[1uA-40uA](1) / FLI[10uA,20uA](2) / FLD[5uA,10uA](3) / Cancel(9) -> "); DigitInput(&CurrentMode);  if(CurrentMode>=0 && CurrentMode<=3){ stack0 = GetValue(IN_BDATA0,param); stack1 = GetValue(IN_BDATA1,param); stack2 = GetValue(IN_TESTSEL,param);  //    stack3 = GetValue(IN_VF_IVSA,param);  if(CurrentMode==0){ for(l=0;l<8;l++){ printf("Select Current: 1uA(1)/3uA(2)/5uA(7)/10uA(6)/20uA(4)/30uA(3)/40uA(5)/Exit Select Current(0) -> "); DigitInput(&ivsa_val2[l]); if(!ivsa_val2[l]) break; CurrentCnt++; } }else if(CurrentMode==1){ ivsa_val2[0]=1; //   1uA ivsa_val2[1]=2; //   3uA ivsa_val2[2]=7; //   5uA ivsa_val2[3]=6; //  10uA ivsa_val2[4]=4; //  20uA ivsa_val2[5]=3; //  30uA ivsa_val2[6]=5; //  40uA CurrentCnt=7; }else if(CurrentMode==2){ ivsa_val2[0]=6; //  10uA ivsa_val2[1]=4; //  20uA CurrentCnt=2; }else if(CurrentMode==3){ ivsa_val2[0]=7; //   5uA ivsa_val2[1]=6; //  10uA CurrentCnt=2; }  printf("Select Address range: Flexible(0) / FLI[0x10000-0x30000](1) / FLD[0x4000-0xE000](2) / FLD[0x4000-0x10000](3) -> "); DigitInput(&AdrMode);  if(AdrMode==0){ printf("Select Mat: M-mat(0) / D-mat(1) -> "); DigitInput(&mat); if(mat==0){ ChangeMat(FLP0_MDL,param);} else if(mat==1){ ChangeMat(FLI1_MDL,param);} printf("Set StartAdr (ex.0x10000) -> "); DigitInput(&StartAdr); printf("Select Address Range: Flexible(0) / FLI[0x8000](1) / FLD[0x2000](2) -> "); DigitInput(&range); if(range==0){ printf("Set Area Range (ex.CASE:FLI 0x8000 / CASE:FLD 0x2000) -> "); DigitInput(&diff); }else if(range==1){ diff=0x8000; }else if(range==2){ diff=0x2000; } printf("Set Loop Count -> "); DigitInput(&AdrCnt); }else if(AdrMode==1){ ChangeMat(FLP0_MDL,param); StartAdr=0x10000; diff=0x8000; AdrCnt=4; }else if(AdrMode==2){ ChangeMat(FLI1_MDL,param); StartAdr=0x4000; diff=0x2000; AdrCnt=5; }else if(AdrMode==3){ ChangeMat(FLI1_MDL,param); StartAdr=0x4000; diff=0x2000; AdrCnt=6; }   //      printf("Input Retention hour(ex.20hr->20) -> ");  //      DigitInput(&tempo7);  //      sprintf(DistName,"Distribution_%dh.csv",tempo7);  for(k=0;k<AdrCnt;k++){ SetValue(IN_TOPADDR,param,StartAdr+k*diff); SetValue(IN_ENDADDR,param,StartAdr+(k+1)*diff);  for(i=0;i<CurrentCnt;i++){  //        DigitInput(&tempo2); tempo2=ivsa_val2[i]; switch(tempo2){ case 1: current_s = "1uA"; break; case 2: current_s = "3uA"; break; case 3: current_s = "30uA"; break; case 4: current_s = "20uA"; break; case 5: current_s = "40uA"; break; case 6: current_s = "10uA"; break; case 7: current_s = "5uA"; break; default: break; } printf("***************Current: %s ****************",current_s); old_testr10 = SetIVSA(param,tempo2); // IVSA(TESTR10) setting  //    printf("Glaph OFF(0) / ON(1)) -> ");  //    DigitInput(&tempo9); tempo9=0; //Glaph OFF(0)Fix tempo0=0; tempo1=104; SetValue(IN_BDATA1,param,tempo1); SetValue(IN_BDATA0,param,tempo0);  //    printf("select Pattern");  //    printf("NORMAL=0/EVEN=1/ODD=2 -> ");  //    DigitInput(&tempo); tempo=0;                          /*Pattern NORMAL=0 Fix SetValue(IN_TESTSEL,param,tempo); CpuModeFunc(MNORMAL,MSDISTRI,&vs[0],W1MIN*3,param,"DISTRIBUTION.pat","Distribution");  //    RAMRunApg(1,DISTRIBUTION_TOP,DISTRIBUTION_END); tempo=0; tempo8=0; cnt=0; l=0; for(j=DISTRIBUTION_TOP;j<DISTRIBUTION_END;j=j+0x04){ tempo=ReadEcrMode8(j,SIZE_L); if(l>104){ printf("   N/A        -        -"); //     SH72544 }else{ tempo8=tempo; // Store Now Data } l++; } if(tempo9==0){ // Graph OFF EcrTextOut(1,DISTRIBUTION_TOP,DISTRIBUTION_END,SIZE_L,"Distribution");  //              EcrTextOut(0,DISTRIBUTION_TOP,DISTRIBUTION_END,SIZE_L,DistName); }else{ // Graph ON EcrTextOut(1,DISTRIBUTION_TOP,DISTRIBUTION_END,SIZE_L,"Distribution"); EcrTextOut(2,DISTRIBUTION_TOP,DISTRIBUTION_END,SIZE_L,"tempo.csv"); } SetValue(IN_BDATA0,param,stack0); SetValue(IN_BDATA1,param,stack1); SetValue(IN_TESTSEL,param,stack2);  //      SetValue(IN_VF_IVSA,param,stack3);// SetValue(IN_TESTR10_FLP,param,old_testr10); // restore TESTR10(IVSA) } }  }else if(CurrentMode==9){ printf("Exit this command."); }else{ printf("Invalid value entered!!"); } }


// ****************************************************************************

// char *C_time(int *timer)

// ****************************************************************************
char *C_time(int *timer)
{
  int i;
  int flag;
  int len;
  char *top;
  char *current;

  top = ctime((CTIME_T*) timer);
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

  return(top);
}


// ===============================================================================================
char *CreatePatFileName( char *patternname )
// ===============================================================================================
{
    int counter = 0;
    static char filename[ 5 ][ 128 ];

    counter = ( ++counter )%5;
    if( configration==RELEASEMODE ){
      sprintf( filename[ counter ], "%s%s", patternname , ".pat" );
    }else if( configration==DEBUGMODE ){
      sprintf( filename[ counter ], "%s%s", patternname , "_Debug.pat" );
    }else if( configration==SCIDEBUGMODE ){
      sprintf( filename[ counter ], "%s%s", patternname , "_SCIDebug.pat" );
    }
    return filename[ counter ];
}

// ===============================================================================================
// Description Ecr BM Data Display int         addr :start ecr/bm address int         cnt  :display Byte Count+1 int         mode :display format
int DispEcrBmData( int addr, int cnt, int mode ){
// ===============================================================================================

  int data, i;
  int bak_fpt;
  char mem_name[4];
  char outbuf[256];

  if( DISP_BM&mode ) strcpy( mem_name, "BM " );
  else               strcpy( mem_name, "ECR" );
  bak_fpt = Ffpt;
  if( DISP_NO_FPT&mode ) Ffpt=1;
  // print&fprint=2 else print

  switch( mode&0x00FF ){
  case DISP_4ADDR:
  // 4byte Address Data Disp
    sprintf( outbuf, "[ Display %s Addr=0x%08X ]", mem_name, addr ); DataOut( Ffpt, outbuf );
    for( i=0; i<cnt; i+=4 ){
      if( DISP_BM&mode ) data = ReadBmMode8( addr+i, SIZE_L );
      else               data = ReadEcrMode8( addr+i, SIZE_L );
      sprintf( outbuf, " Addr=0x%08X, Data=0x%08X, ", addr+i, data );
      if( 0==((i+4)%16) ) strcat( outbuf, "\n" );
      DataOut( Ffpt, outbuf );
    }
    DataOut( Ffpt, "\n" );
    break;
  case DISP_4:
  // 4bye read disp
    sprintf( outbuf, "[ Display %s Addr=0x%08X ]", mem_name, addr ); DataOut( Ffpt, outbuf );
    DataOut( Ffpt, "              , 0 1 2 3, 4 5 6 7, 8 9 A B, C D E F,10111213,14151617,18191A1B,1C1D1E1F" );
    for( i=0; i<cnt; i+=4 ){
      if( DISP_BM&mode ) data = ReadBmMode8( addr+i, SIZE_L );
      else               data = ReadEcrMode8( addr+i, SIZE_L );
      if( 0==i%32 ){
        sprintf( outbuf, " Addr=%08X,", addr+i );
        DataOut( Ffpt, outbuf );
      }
      sprintf( outbuf, "%08X,", data );
      DataOut( Ffpt, outbuf );
    }
    DataOut( Ffpt, "\n" );
    break;
  case DISP_1:
  // 1byte read BM disp
    sprintf( outbuf, "[ Display %s Addr=0x%08X by Read Byte ]", mem_name, addr ); DataOut( Ffpt, outbuf );
    DataOut( Ffpt, "              , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F,10,11,12,13,14,15,16,17,18,19,1A,1B,1C,1D,1E,1F" );
    for( i=0; i<cnt; i++ ){
      if( DISP_BM&mode ) data = ReadBmMode8( addr+i, SIZE_B );
      else               data = ReadEcrMode8( addr+i, SIZE_B );
      if( 0==i%32 ){
        sprintf( outbuf, " Addr=%08X,", addr+i );
        DataOut( Ffpt, outbuf );
      }
      sprintf( outbuf, "%02X,", data );
      DataOut( Ffpt, outbuf );
    }
    DataOut( Ffpt, "\n" );
    break;
  default:
  // none
    break;
  }
  Ffpt = bak_fpt;
  // store Ffpt
  return(PASS);
}
// end DispEcrBmData()



// ----------------------------------------------------------------------------
// Description     memcpy  char->void ver
// ----------------------------------------------------------------------------
void v_memcpy( void* daddr, void* saddr, int size ){
    char *d, *s;
    int i;
    d = (char *)daddr; s = (char *)saddr;
    for( i=0; i<size; i++ ) *(d+i) = *(s+i);
}

// ----------------------------------------------------------------------------
// Description 32 bit check in  int     fk 0-31 in  extern  fc_key out bool    bit(fk)=1 -> TRUE
// ----------------------------------------------------------------------------
int CheckFK( int fk ){
    if( fk>31 || fk<0 )             return( FALSE );
    if( fc_key&(0x00000001<<fk) )   return( TRUE  );
    else                            return( FALSE );
}

// ----------------------------------------------------------------------------
// Description tyear,tmon,tday,thour,tmin,tsec global valiable Set & mode=0 char buff YYYYMMDDHHMMSS Set mode=1 char buff YYYY/MM/DD HH:MM:SS Set other mode  buff Sun Apr 01 12:34:56 2012 caution!! buff over flow  need plenty of buff size
// ----------------------------------------------------------------------------
extern void GetDateTime( time_t* ts, char* buff, int mode ){
  char* cp;
#ifdef V5KONLY
  struct tm *time_struct;
  time_struct = localtime( ts );
  tyear = time_struct->tm_year+1900;
  tmon = time_struct->tm_mon+1;
  tday = time_struct->tm_mday;
  thour = time_struct->tm_hour;
  tmin = time_struct->tm_min;
  tsec = time_struct->tm_sec;
  if( 0==mode ){
    sprintf( buff, "%04d%02d%02d%02d%02d%02d", tyear, tmon, tday, thour, tmin, tsec );
  }else if( 1==mode ){
    sprintf( buff, "%04d/%02d/%02d %02d:%02d:%02d", tyear, tmon, tday, thour, tmin, tsec );
  }else{
    if( sprintf( buff, "%s", C_time( (int *)ts ) )!=0 ){ if( ( cp = strstr( buff, "" ) )!=NULL ) *cp = NUL;
    }
  }
#else
  char time_buf[32];
  char time_buf2[32];
  if( sprintf( time_buf, "%s", C_time( (int *)ts ) )!=0 ){ if( ( cp = strstr( time_buf, "" ) )!=NULL ) *cp = NUL;
  }
  buff[0] = time_buf[20]; buff[1] = time_buf[21]; buff[2] = time_buf[22]; buff[3] = time_buf[23];
  if( 0!=strstr( time_buf, "Jan" ) ){ buff[4] = '0'; buff[5] = '1'; }
  if( 0!=strstr( time_buf, "Feb" ) ){ buff[4] = '0'; buff[5] = '2'; }
  if( 0!=strstr( time_buf, "Mar" ) ){ buff[4] = '0'; buff[5] = '3'; }
  if( 0!=strstr( time_buf, "Apr" ) ){ buff[4] = '0'; buff[5] = '4'; }
  if( 0!=strstr( time_buf, "May" ) ){ buff[4] = '0'; buff[5] = '5'; }
  if( 0!=strstr( time_buf, "Jun" ) ){ buff[4] = '0'; buff[5] = '6'; }
  if( 0!=strstr( time_buf, "Jul" ) ){ buff[4] = '0'; buff[5] = '7'; }
  if( 0!=strstr( time_buf, "Aug" ) ){ buff[4] = '0'; buff[5] = '8'; }
  if( 0!=strstr( time_buf, "Sep" ) ){ buff[4] = '0'; buff[5] = '9'; }
  if( 0!=strstr( time_buf, "Oct" ) ){ buff[4] = '1'; buff[5] = '0'; }
  if( 0!=strstr( time_buf, "Nov" ) ){ buff[4] = '1'; buff[5] = '1'; }
  if( 0!=strstr( time_buf, "Dec" ) ){ buff[4] = '1'; buff[5] = '2'; }
  buff[6] = time_buf[8]; buff[7] = time_buf[9];
  buff[8] = time_buf[11]; buff[9] = time_buf[12];
  buff[10] = time_buf[14]; buff[11] = time_buf[15];
  buff[12] = time_buf[17]; buff[13] = time_buf[18];
  buff[14] = '\0';
  strcpy( time_buf2, buff );
  tsec = strtod( &time_buf2[12] ); time_buf2[12] = '\0';
  tmin = strtod( &time_buf2[10] ); time_buf2[10] = '\0';
  thour = strtod( &time_buf2[8] ); time_buf2[8] = '\0';
  tday = strtod( &time_buf2[6] ); time_buf2[6] = '\0';
  tmon = strtod( &time_buf2[4] ); time_buf2[4] = '\0';
  tyear = strtod( &time_buf2[0] );
  if( 0==mode ){
        //sprintf( buff, "%04d%02d%02d%02d%02d%02d", tyear, tmon, tday, thour, tmin, tsec );
  }else if( 1==mode ){
    sprintf( buff, "%04d/%02d/%02d %02d:%02d:%02d", tyear, tmon, tday, thour, tmin, tsec );
  }else{
    strcpy( buff, time_buf );
  }
#endif
  return;
}

// ----------------------------------------------------------------------------
// Description Wmat FBM->WmatStructure Struct Set danger char *wmat buffer over no care
// ----------------------------------------------------------------------------
void SetWmatStructPadding( WmatStructure *wsp, const char *fbm_data ){
  int i, j;
  char *wmat;
#ifdef V5KONLY
  char d0, d1, d2, d3;
  int cnt, ofs;
  static int word_point[44] =
    { 0x000,
      0x01C, 0x01E, 0x020, 0x022, 0x030,
      0x0C9, 0x0CC, 0x0CF, 0x0D3, 0x0D6, 0x0D9, 0x0DB, 0x0DD, 0x0E1, 0x0E3, 0x0E5, 0x0E9, 0x0F9, 0x0FC, 0x101, 0x104, 0x109, 0x10C, 0x111,
      0x139, 0x13C, 0x13F, 0x143, 0x146, 0x149, 0x14B, 0x14D, 0x151, 0x153, 0x155, 0x159, 0x169, 0x16C, 0x171, 0x174, 0x179, 0x17C, 0x181
    };
  static int lword_point[6] =
    { 0x000,
      0x034, 0x080, 0x1A0, 0x1A4, 0x1A8
    };
#endif

  wmat = (char *)wsp;
#ifdef V4KONLY
  for( i=0, j=0; i<WMAT_STRUCT_SIZE; i++, j++ ){
  // caution!! WMAT_SIZE>WmatStructure
        // ---- structure align padding ----
    if( 0x0088>i ){
    }else if( 0x130>i ){
      if( 0x88==i ) j+=8;
      else if( 0x9C ==i || 0xC0 ==i ) j+=12;
      else if( 0xC9 ==i || 0xCC ==i || 0xCF ==i || 0xD3 ==i || 0xD6 ==i || 0xD9 ==i || 0xE1 ==i || 0xE9 ==i || 0xF8==i || 0xF9 ==i || 0xFC ==i ||
               0x101==i || 0x104==i || 0x109==i || 0x10C==i || 0x111==i ) j+=1;
      else if( 0xD2 ==i || 0xE0 ==i || 0xE8 ==i || 0xF5 ==i ) j+=3;
      else if( 0x100==i || 0x108==i || 0x110==i ) j+=2;
      else if( 0xF0 ==i ) j+=7;
      else ;
    }else{
      if( 0x130==i || 0x1A0==i ) j+=7;
      else if( 0x139==i || 0x13C==i || 0x13F==i || 0x143==i || 0x146==i || 0x149==i || 0x151==i || 0x159==i || 0x168==i || 0x169==i || 0x16C==i ||
               0x171==i || 0x174==i || 0x179==i || 0x17C==i || 0x181==i  )  j+=1;
      else if( 0x142==i || 0x150==i || 0x158==i || 0x165==i )  j+=3;
      else if( 0x170==i || 0x178==i || 0x180==i ) j+=2;
      else if( 0x160 ==i ) j+=7;
      else ;
    }
    wmat[ j ] = fbm_data[ i ];
  }
#endif
#ifdef V5KONLY
  for( i=0, j=0; i<WMAT_STRUCT_SIZE; i++, j++ ){
  // caution!! WMAT_SIZE>WmatStructure
    if( 0x00C0>i ){
    }else if( 0x130>i ){
      if( 0x0C9==i || 0x0CC==i || 0x0CF==i || 0x0D2==i  || 0x0D3==i  || 0x0D6==i || 0x0D9==i || 0x0E0==i || 0x0E1==i || 0x0E8==i || 0x0E9==i || 0x0F8==i || 0x0F9==i || 0x0FC==i || 0x101==i || 0x104==i || 0x109==i || 0x10C==i || 0x111==i ) j++;
    }else{
      if( 0x130==i || 0x1A0==i ) j++;
      if( 0x139==i || 0x13C==i || 0x13F==i || 0x142==i  || 0x143==i  || 0x146==i || 0x149==i || 0x150==i || 0x151==i || 0x158==i || 0x159==i || 0x168==i || 0x169==i || 0x16C==i || 0x171==i || 0x174==i || 0x179==i || 0x17C==i || 0x181==i ) j++;
    }
        // ---- structure align padding ----
    wmat[ j ] = fbm_data[ i ];
  }
    // ---- Lword endian change ----
  ofs = 0;
  for( cnt = 1; cnt<6; cnt++ ){
    for( i = lword_point[cnt-1]+1; i<=lword_point[cnt]; i++ ){
      if( 0x00C0>i ){
      }else if( 0x130>i ){
        if( 0x0C9==i || 0x0CC==i || 0x0CF==i || 0x0D2==i  || 0x0D3==i  || 0x0D6==i || 0x0D9==i || 0x0E0==i || 0x0E1==i || 0x0E8==i || 0x0E9==i || 0x0F8==i || 0x0F9==i || 0x0FC==i || 0x101==i || 0x104==i || 0x109==i || 0x10C==i || 0x111==i ) ofs++;
      }else{
        if( 0x130==i || 0x1A0==i ) ofs++;
        if( 0x139==i || 0x13C==i || 0x13F==i || 0x142==i  || 0x143==i  || 0x146==i || 0x149==i || 0x150==i || 0x151==i || 0x158==i || 0x159==i || 0x168==i || 0x169==i || 0x16C==i || 0x171==i || 0x174==i || 0x179==i || 0x17C==i || 0x181==i ) ofs++;
      }
    }
    d0 = wmat[lword_point[cnt]+ofs]; d1 = wmat[lword_point[cnt]+ofs+1]; d2 = wmat[lword_point[cnt]+ofs+2]; d3 = wmat[lword_point[cnt]+ofs+3];
    wmat[lword_point[cnt]+ofs+3] = d0; wmat[lword_point[cnt]+ofs+2] = d1; wmat[lword_point[cnt]+ofs+1] = d2; wmat[lword_point[cnt]+ofs] = d3;
  }
    // ---- Word endian change ----
  ofs = 0;
  for( cnt = 1; cnt<44; cnt++ ){
    for( i = word_point[cnt-1]+1; i<=word_point[cnt]; i++ ){
      if( 0x00C0>i ){
      }else if( 0x130>i ){
        if( 0x0C9==i || 0x0CC==i || 0x0CF==i || 0x0D2==i  || 0x0D3==i  || 0x0D6==i || 0x0D9==i || 0x0E0==i || 0x0E1==i || 0x0E8==i || 0x0E9==i || 0x0F8==i || 0x0F9==i || 0x0FC==i || 0x101==i || 0x104==i || 0x109==i || 0x10C==i || 0x111==i ) ofs++;
      }else{
        if( 0x130==i || 0x1A0==i ) ofs++;
        if( 0x139==i || 0x13C==i || 0x13F==i || 0x142==i  || 0x143==i  || 0x146==i || 0x149==i || 0x150==i || 0x151==i || 0x158==i || 0x159==i || 0x168==i || 0x169==i || 0x16C==i || 0x171==i || 0x174==i || 0x179==i || 0x17C==i || 0x181==i ) ofs++;
      }
    }
    d0 = wmat[word_point[cnt]+ofs]; d1 = wmat[word_point[cnt]+ofs+1];
    wmat[word_point[cnt]+ofs+1] = d0; wmat[word_point[cnt]+ofs] = d1;
  }
#endif
  return;
}

// ----------------------------------------------------------------------------
// Description WmatStructure DataOut
// ----------------------------------------------------------------------------
void DispWmatStruct( WmatStructure *chipinfo ){
  int cnt;

  sprintf( outbuf, "productinfo=S'[%-15.15s] , { [0]=H'[%02X] , [15]=H'[%02X] }", &chipinfo->productinfo[0], chipinfo->productinfo[0], chipinfo->productinfo[15] ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "lotname[0]=S'[%-10.10s] , { [0]=H'[%02X] , [9]=H'[%02X] }", &chipinfo->lotname[0], chipinfo->lotname[0], chipinfo->lotname[9] ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "wafernumber=C'[%c]", chipinfo->wafernumber ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "processcode=S'[%-2.2s] , footholdcode=S'[%-2.2s]", &chipinfo->processcode, &chipinfo->footholdcode ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "xchipdirection=D'[%d] , ychipdirection=D'[%d]", chipinfo->xchipdirection, chipinfo->ychipdirection ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "umathistory=H'[%02X] , programversion=H'[%02X] , testversion=H'[%02X] , flowversion=H'[%02X]", chipinfo->umathistory, chipinfo->programversion, chipinfo->testversion, chipinfo->flowversion ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "passcode[0]=H'[%02X] , passcode[1]=H'[%02X] , passcode[2]=H'[%02X] , passcode[3]=H'[%02X]", chipinfo->passcode[0], chipinfo->passcode[1], chipinfo->passcode[2], chipinfo->passcode[3] ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "ptimetrimfli=H'[%02X] , ptimetrimfld=H'[%02X] , etimetrimfli=H'[%02X] , etimetrimfld=H'[%02X]", chipinfo->ptimetrimfli, chipinfo->ptimetrimfld, chipinfo->etimetrimfli, chipinfo->etimetrimfld ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "failinfo.failpoint.contents=H'[%04X]", chipinfo->failinfo.failpoint.contents ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "failinfo.failpoint.vdd=H'[%1X] , failinfo.failpoint.area=H'[%1X]", chipinfo->failinfo.failpoint.vdd, chipinfo->failinfo.failpoint.area ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "failinfo.failpoint.mat=H'[%02X]", chipinfo->failinfo.failpoint.mat ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "failinfo.failaddress=H'[%08X]", chipinfo->failinfo.failaddress ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "reliefcategory=H'[%02X] , reliefcount=H'[%02X]", chipinfo->reliefcategory, chipinfo->reliefcount ); DataOut( Ffpt, outbuf );
  DataOut( Ffpt, "failpoint_fli0=H'[" ); for( cnt=0; cnt<FLI_FAILPOINT_SIZE; cnt++ ){ sprintf( outbuf, "%02X", chipinfo->failpoint_fli0[cnt] ); DataOut( Ffpt, outbuf ); }; DataOut( Ffpt, "]" );
  DataOut( Ffpt, "failpoint_fli1=H'[" ); for( cnt=0; cnt<FLI_FAILPOINT_SIZE; cnt++ ){ sprintf( outbuf, "%02X", chipinfo->failpoint_fli1[cnt] ); DataOut( Ffpt, outbuf ); }; DataOut( Ffpt, "]" );
  DataOut( Ffpt, "failpoint_fli2=H'[" ); for( cnt=0; cnt<FLI_FAILPOINT_SIZE; cnt++ ){ sprintf( outbuf, "%02X", chipinfo->failpoint_fli2[cnt] ); DataOut( Ffpt, outbuf ); }; DataOut( Ffpt, "]" );
  DataOut( Ffpt, "failpoint_fli3=H'[" ); for( cnt=0; cnt<FLI_FAILPOINT_SIZE; cnt++ ){ sprintf( outbuf, "%02X", chipinfo->failpoint_fli3[cnt] ); DataOut( Ffpt, outbuf ); }; DataOut( Ffpt, "]" );
  sprintf( outbuf, "chipidcrccode=H'[%08X]", chipinfo->chipidcrccode ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "trimreg.reg00=H'[%02X] , trimreg.reg01=H'[%02X] , trimreg.reg02=H'[%02X] , trimreg.reg03=H'[%02X]", chipinfo->trimreg.reg00, chipinfo->trimreg.reg01, chipinfo->trimreg.reg02, chipinfo->trimreg.reg03 ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "trimreg.reg04=H'[%02X] , trimreg.reg05=H'[%02X] , trimreg.reg06=H'[%02X] , trimreg.reg07=H'[%02X]", chipinfo->trimreg.reg04, chipinfo->trimreg.reg05, chipinfo->trimreg.reg06, chipinfo->trimreg.reg07 ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "trimreg.reg08=H'[%02X] , trimreg.reg09=H'[%02X] , trimreg.reg10=H'[%02X] , trimreg.reg11=H'[%02X]", chipinfo->trimreg.reg08, chipinfo->trimreg.reg09, chipinfo->trimreg.reg10, chipinfo->trimreg.reg11 ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "trimreg.reg12=H'[%02X] , trimreg.reg13=H'[%02X] , trimreg.reg14=H'[%02X] , trimreg.reg15=H'[%02X]", chipinfo->trimreg.reg12, chipinfo->trimreg.reg13, chipinfo->trimreg.reg14, chipinfo->trimreg.reg15 ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "trimreg.reg16=H'[%02X] , trimreg.reg17=H'[%02X] , trimreg.reg18=H'[%02X] , trimreg.reg19=H'[%02X]", chipinfo->trimreg.reg16, chipinfo->trimreg.reg17, chipinfo->trimreg.reg18, chipinfo->trimreg.reg19 ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthisearch.centereven=H'[%02X] , flp.vthisearch.centerodd=H'[%02X]", chipinfo->flp.vthisearch.centereven, chipinfo->flp.vthisearch.centerodd ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthisearch.low=H'[%02X] , flp.vthisearch.high=H'[%02X]", chipinfo->flp.vthisearch.low, chipinfo->flp.vthisearch.high ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthsearchwt1.beforedisturb=H'[%02X] , flp.vthsearchwt1.afterdisturb=H'[%02X]", chipinfo->flp.vthsearchwt1.beforedisturb, chipinfo->flp.vthsearchwt1.afterdisturb ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthsearchwt1.beforeretention10ua=H'[%02X] , flp.vthsearchwt1.beforeretention3ua=H'[%02X]", chipinfo->flp.vthsearchwt1.beforeretention10ua, chipinfo->flp.vthsearchwt1.beforeretention3ua ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharastripeawt1.max=D'[%d] , flp.pcharastripeawt1.ave=D'[%d]", chipinfo->flp.pcharastripeawt1.max, chipinfo->flp.pcharastripeawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharastripebwt1.max=D'[%d] , flp.pcharastripebwt1.ave=D'[%d]", chipinfo->flp.pcharastripebwt1.max, chipinfo->flp.pcharastripebwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharaall1wt1.max=D'[%d] , flp.pcharaall1wt1.ave=D'[%d]", chipinfo->flp.pcharaall1wt1.max, chipinfo->flp.pcharaall1wt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharadiagawt1.max=D'[%d] , flp.pcharadiagawt1.ave=D'[%d]", chipinfo->flp.pcharadiagawt1.max, chipinfo->flp.pcharadiagawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharachkawt1.max=D'[%d] , flp.pcharachkawt1.ave=D'[%d]", chipinfo->flp.pcharachkawt1.max, chipinfo->flp.pcharachkawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echarainitialwt1.max=D'[%d] , flp.echarainitialwt1.ave=D'[%d]", chipinfo->flp.echarainitialwt1.max, chipinfo->flp.echarainitialwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echaratimetrimwt1_max=D'[%d] , flp.echaratimetrimwt1_ave=D'[%d]", chipinfo->flp.echaratimetrimwt1_max, chipinfo->flp.echaratimetrimwt1_ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echaralowvoltwt1.max=D'[%d] , flp.echaralowvoltwt1.ave=D'[%d]", chipinfo->flp.echaralowvoltwt1.max, chipinfo->flp.echaralowvoltwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echaraallselwt1_max=D'[%d] , flp.echaraallselwt1_ave=D'[%d]", chipinfo->flp.echaraallselwt1_max, chipinfo->flp.echaraallselwt1_ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echarajudgewt1.max=D'[%d] , flp.echarajudgewt1.ave=D'[%d]", chipinfo->flp.echarajudgewt1.max, chipinfo->flp.echarajudgewt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthsearchwt2.afterretention10ua=H'[%02X] , flp.vthsearchwt2.afterretention3ua=H'[%02X]", chipinfo->flp.vthsearchwt2.afterretention10ua, chipinfo->flp.vthsearchwt2.afterretention3ua ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.vthsearchwt2.beforedisturb=H'[%02X] , flp.vthsearchwt2.afterdisturb=H'[%02X] , flp.vthsearchwt2.afterdisturba=H'[%02X]" , chipinfo->flp.vthsearchwt2.beforedisturb, chipinfo->flp.vthsearchwt2.afterdisturb, chipinfo->flp.vthsearchwt2.afterdisturba ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharaall0wt2.max=D'[%d] , flp.pcharaall0wt2.ave=D'[%d]", chipinfo->flp.pcharaall0wt2.max, chipinfo->flp.pcharaall0wt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharalowvoltwt2.max=D'[%d] , flp.pcharalowvoltwt2.ave=D'[%d]", chipinfo->flp.pcharalowvoltwt2.max, chipinfo->flp.pcharalowvoltwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.pcharachkbwt2.max=D'[%d] , flp.pcharachkbwt2.ave=D'[%d]", chipinfo->flp.pcharachkbwt2.max, chipinfo->flp.pcharachkbwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echarainitialwt2.max=D'[%d] , flp.echarainitialwt2.ave=D'[%d]", chipinfo->flp.echarainitialwt2.max, chipinfo->flp.echarainitialwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echaralowvoltwt2.max=D'[%d] , flp.echaralowvoltwt2.ave=D'[%d]", chipinfo->flp.echaralowvoltwt2.max, chipinfo->flp.echaralowvoltwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echarajudgewt2.max=D'[%d] , flp.echarajudgewt2.ave=D'[%d]", chipinfo->flp.echarajudgewt2.max, chipinfo->flp.echarajudgewt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "flp.echaralastwt2.max=D'[%d] , flp.echaralastwt2.ave=D'[%d]", chipinfo->flp.echaralastwt2.max, chipinfo->flp.echaralastwt2.ave ); DataOut( Ffpt, outbuf );

  sprintf( outbuf, "e2p.vthisearch.centereven=H'[%02X] , e2p.vthisearch.centerodd=H'[%02X]", chipinfo->e2p.vthisearch.centereven, chipinfo->e2p.vthisearch.centerodd ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.vthisearch.low=H'[%02X] , e2p.vthisearch.high=H'[%02X]", chipinfo->e2p.vthisearch.low, chipinfo->e2p.vthisearch.high ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.vthsearchwt1.beforedisturb=H'[%02X] , e2p.vthsearchwt1.afterdisturb=H'[%02X]", chipinfo->e2p.vthsearchwt1.beforedisturb, chipinfo->e2p.vthsearchwt1.afterdisturb ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.vthsearchwt1.beforeretention10ua=H'[%02X] , e2p.vthsearchwt1.beforeretention3ua=H'[%02X]", chipinfo->e2p.vthsearchwt1.beforeretention10ua, chipinfo->e2p.vthsearchwt1.beforeretention3ua ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharastripeawt1.max=D'[%d] , e2p.pcharastripeawt1.ave=D'[%d]", chipinfo->e2p.pcharastripeawt1.max, chipinfo->e2p.pcharastripeawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharastripebwt1.max=D'[%d] , e2p.pcharastripebwt1.ave=D'[%d]", chipinfo->e2p.pcharastripebwt1.max, chipinfo->e2p.pcharastripebwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharaall1wt1.max=D'[%d] , e2p.pcharaall1wt1.ave=D'[%d]", chipinfo->e2p.pcharaall1wt1.max, chipinfo->e2p.pcharaall1wt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharadiagawt1.max=D'[%d] , e2p.pcharadiagawt1.ave=D'[%d]", chipinfo->e2p.pcharadiagawt1.max, chipinfo->e2p.pcharadiagawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharachkawt1.max=D'[%d] , e2p.pcharachkawt1.ave=D'[%d]", chipinfo->e2p.pcharachkawt1.max, chipinfo->e2p.pcharachkawt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echarainitialwt1.max=D'[%d] , e2p.echarainitialwt1.ave=D'[%d]", chipinfo->e2p.echarainitialwt1.max, chipinfo->e2p.echarainitialwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echaratimetrimwt1_max=D'[%d] , e2p.echaratimetrimwt1_ave=D'[%d]", chipinfo->e2p.echaratimetrimwt1_max, chipinfo->e2p.echaratimetrimwt1_ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echaralowvoltwt1.max=D'[%d] , e2p.echaralowvoltwt1.ave=D'[%d]", chipinfo->e2p.echaralowvoltwt1.max, chipinfo->e2p.echaralowvoltwt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echaraallselwt1_max=D'[%d] , e2p.echaraallselwt1_ave=D'[%d]", chipinfo->e2p.echaraallselwt1_max, chipinfo->e2p.echaraallselwt1_ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echarajudgewt1.max=D'[%d] , e2p.echarajudgewt1.ave=D'[%d]", chipinfo->e2p.echarajudgewt1.max, chipinfo->e2p.echarajudgewt1.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.vthsearchwt2.afterretention10ua=H'[%02X] , e2p.vthsearchwt2.afterretention3ua=H'[%02X]", chipinfo->e2p.vthsearchwt2.afterretention10ua, chipinfo->e2p.vthsearchwt2.afterretention3ua ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.vthsearchwt2.beforedisturb=H'[%02X] , e2p.vthsearchwt2.afterdisturb=H'[%02X] , e2p.vthsearchwt2.afterdisturba=H'[%02X]" , chipinfo->e2p.vthsearchwt2.beforedisturb, chipinfo->e2p.vthsearchwt2.afterdisturb, chipinfo->e2p.vthsearchwt2.afterdisturba ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharaall0wt2.max=D'[%d] , e2p.pcharaall0wt2.ave=D'[%d]", chipinfo->e2p.pcharaall0wt2.max, chipinfo->e2p.pcharaall0wt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharalowvoltwt2.max=D'[%d] , e2p.pcharalowvoltwt2.ave=D'[%d]", chipinfo->e2p.pcharalowvoltwt2.max, chipinfo->e2p.pcharalowvoltwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.pcharachkbwt2.max=D'[%d] , e2p.pcharachkbwt2.ave=D'[%d]", chipinfo->e2p.pcharachkbwt2.max, chipinfo->e2p.pcharachkbwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echarainitialwt2.max=D'[%d] , e2p.echarainitialwt2.ave=D'[%d]", chipinfo->e2p.echarainitialwt2.max, chipinfo->e2p.echarainitialwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echaralowvoltwt2.max=D'[%d] , e2p.echaralowvoltwt2.ave=D'[%d]", chipinfo->e2p.echaralowvoltwt2.max, chipinfo->e2p.echaralowvoltwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echarajudgewt2.max=D'[%d] , e2p.echarajudgewt2.ave=D'[%d]", chipinfo->e2p.echarajudgewt2.max, chipinfo->e2p.echarajudgewt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "e2p.echaralastwt2.max=D'[%d] , e2p.echaralastwt2.ave=D'[%d]", chipinfo->e2p.echaralastwt2.max, chipinfo->e2p.echaralastwt2.ave ); DataOut( Ffpt, outbuf );
  sprintf( outbuf, "rosc_logic=D'[%d] , rosc_chgpump=D'[%d] , rosc_subact=D'[%d]", chipinfo->rosc_logic , chipinfo->rosc_chgpump , chipinfo->rosc_subact ); DataOut( Ffpt, outbuf );

  return;
}

// ----------------------------------------------------------------------------
// Description Keyboard Any key hit wait loop
// ----------------------------------------------------------------------------
int WaitHitKey( const char *mess ){
  flush_console();
  fflush( stdin );
  printf( "%s", mess );
  fflush( stdout );
  while( !kbhit() );
  flush_console();
  fflush( stdin );
}

// ----------------------------------------------------------------------------
// Description Keyboard Any key hit Check
// ----------------------------------------------------------------------------
int CheckHitKey( void ){
  int retcode;
//  flush_console();
//  fflush(stdin);
  retcode = kbhit();
  flush_console();
  fflush( stdin );
  fflush( stdout );
  return retcode;
}

// ----------------------------------------------------------------------------
// Description interface_t*TABLE_END data store use global interface_store_buff[TABLE_END]
// ----------------------------------------------------------------------------
int StoreInParam( const interface_t *param ){
//  v_memcpy( (void*)interface_store_buff, (void*)param, sizeof(interface_t)*TABLE_END );    // input output param store
  v_memcpy( (void*)interface_store_buff, (void*)param, sizeof(interface_t)*IN_LWORDEND );
  // input param store
  return( sizeof(interface_t)*IN_LWORDEND );
}

// ----------------------------------------------------------------------------
// Description interface_t*TABLE_END data restore use global interface_store_buff[TABLE_END]
// ----------------------------------------------------------------------------
int ReStoreInParam( interface_t *param ){
  v_memcpy( (void*)param, (void*)interface_store_buff, sizeof(interface_t)*IN_LWORDEND );
  // input param restore
  return( sizeof(interface_t)*IN_LWORDEND );
}

// ----------------------------------------------------------------------------
// ECR Input parameter get
// ----------------------------------------------------------------------------
int GetValueEcrIIF( int i ){
  int adr, data;
  adr = INPUTIF_TOP;
  if( i>IN_BYTEEND ){
    adr += IN_BYTEEND-1;
    adr += ( i-IN_BYTEEND-1 )*4;
    data = ReadEcrMode8( adr, SIZE_L );
  }else{
    adr += i;
    data = ReadEcrMode8( adr, SIZE_B );
  }
  return( data );
}

// ----------------------------------------------------------------------------
// BM Input parameter get
// ----------------------------------------------------------------------------
int GetValueBmIIF( int i ){
  int adr, data;
  adr = INPUTIF_TOP;
  if( i>IN_BYTEEND ){
    adr += IN_BYTEEND-1;
    adr += ( i-IN_BYTEEND-1 )*4;
    data = ReadBmMode8( adr, SIZE_L );
  }else{
    adr += i;
    data = ReadBmMode8( adr, SIZE_B );
  }
  return( data );
}

// ----------------------------------------------------------------------------
// SIZE_B / SIZE_W  0xFF/0xFFFF = -1 set ReadEcrMode8
// ----------------------------------------------------------------------------
int ReadEcrMode8_FF( int addr, int size ){
  int data;
  data = ReadEcrMode8( addr, size );
  if( SIZE_B==size && 0x00FF==data ) data = -1;
  if( SIZE_W==size && 0x00FFFF==data ) data = -1;
  return( data );
}


int TableDataOut(int tabledata[500][100], int xdata_num, int ydata_num, char *header, char *filename){
  int i,j;
  
  if(PASS != OpenDataOutFile(filename)) return(FAIL);

  fprintf(Fptdata,"%s",header);

  for(i=0;i<=ydata_num;i++){
    for(j=0;j<=xdata_num;j++){
      fprintf(Fptdata,"%d,",tabledata[i][j]);
    }
    fprintf(Fptdata,"",tabledata[i][j]);
  }
  
  if(PASS != CloseDataOutFile()) return(FAIL);
  return(PASS);
}

int StrCut( char *t, char *s, int pos, int len )
{
    if( pos < 0 || len < 0 || len > strlen(s) )
        return FAIL;
    for( s += pos; *s != '\0' && len > 0; len-- )
        *t++ = *s++;
    *t = '\0';
    return PASS;
}


int DelayTestSetting(int time, int vddh, int vdd){

  set_pe_resource( delay_pin, PE_VIH1, vdd );
  set_pe_resource( vssmon_pe_pin, PE_VIH1, vddh );
    
  ChangeTimming_GTSLTS( EXTCYLJ, 1, time );
  set_drive_time_ns( clk_pin, 1, 0, time/2 );

}

int ChangeTimming_GTSLTS(int gts, int lts, int rate)
{
  int tim_1,tim_2,tim_3;
  int err_flag;

  tim_1 = 0;
  tim_2 = (rate/4)+1;
  tim_3 = (rate/2)+1;

  err_flag = PASS;

  if( comp_set_tsmap      (all_pins, gts, lts)         ){ err_flag |= 0x01; };
  if( comp_set_io_time_ns (all_pins, lts, 0,     rate )){ err_flag |= 0x02; };
  if( set_drive_time_ns   (all_pins, lts, tim_1, rate )){ err_flag |= 0x04; };
  if( set_drive_time_ns   (clk_pin,  lts, tim_1, tim_3)){ err_flag |= 0x08; };
  if( set_drive_time_ns   (lp_clk ,  lts, tim_1, tim_3)){ err_flag |= 0x40; };
  if( set_strobe_time_ns  (all_pins, lts, rate-2,rate )){ err_flag |= 0x10; };
  if( set_cycle_time      (          gts, rate )       ){ err_flag |= 0x20; };


  if( PASS!=err_flag ){
	sprintf(outbuf, "ChangeTimming_GTSLTS Error  [0x%X]", err_flag );
    DataOut(Ffpt,outbuf);
        //DeviceLevelsPowerDown();
  }

  return( err_flag );

}

int ChangeTimming_GTSLTS_LP(int gts, int lts, int rate)
{
  int tim_0,tim_1,tim_2,tim_3,tim_4,tim_5;
  int err_flag;

  tim_0 = 0;
  tim_1 = ((rate*1)/5);
  tim_2 = ((rate*2)/5);
  tim_3 = ((rate*3)/5);
  tim_4 = ((rate*4)/5);
  tim_5 = rate;

  err_flag = PASS;

  if( set_cycle_time      (          gts, rate )       )     { err_flag |= 0x01; };
  if( comp_set_tsmap      (all_pins, gts, lts)         )     { err_flag |= 0x02; };
  if( set_drive_time_ns   (all_pins, lts, tim_0, tim_2))     { err_flag |= 0x04; };
  if( set_strobe_time_ns  (all_pins, lts, tim_0, tim_0 + 5 )){ err_flag |= 0x08; };
  if( set_drive_time_ns   (clk_pin,  lts, tim_0, tim_2))     { err_flag |= 0x10; };


  if( PASS!=err_flag ){
	sprintf(outbuf, "ChangeTimming GTS:%x LTS:%d Error  [0x%X]",gts,lts, err_flag );
    DataOut(Ffpt,outbuf);
        //DeviceLevelsPowerDown();
  }

  return( err_flag );

}
int ChangeTimming_GTSLTS_LP2(int gts, int lts, int rate)
{
  int tim_0,tim_1,tim_2,tim_3,tim_4,tim_5;
  int err_flag;

  tim_0 = 0;
  tim_1 = ((rate*1)/4);
  tim_2 = ((rate*2)/4);
  tim_3 = ((rate*3)/4);
  tim_4 = ((rate*4)/4);
  tim_5 = rate;

  if(tim_2 >= 20*1000){
    tim_2 = 20*1000;
  }

  err_flag = PASS;

  if( set_cycle_time      (          gts, rate )       )     { err_flag |= 0x01; };
  if( comp_set_tsmap      (all_pins, gts, lts)         )     { err_flag |= 0x02; };
  if( set_drive_time_ns   (all_pins, lts, tim_0, tim_2))     { err_flag |= 0x04; };
  if( set_strobe_time_ns  (all_pins, lts, tim_0, tim_0 + 5 )){ err_flag |= 0x08; };
  if( set_drive_time_ns   (clk_pin,  lts, tim_0, tim_2))     { err_flag |= 0x10; };


  if( PASS!=err_flag ){
	sprintf(outbuf, "ChangeTimming GTS:%x LTS:%d Error2  [0x%X]",gts,lts, err_flag );
    DataOut(Ffpt,outbuf);
        //DeviceLevelsPowerDown();
  }

  return( err_flag );

}


int ChangeAddress(int mat, int addres1, int addres2, interface_t* param){
  int select_size;
  
  select_size = ( addres2 - addres1 + 1 );
  switch(mat){
    case FLP0:
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,addres1);
      SetValue(IN_TESTSIZE0,param,select_size);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    break;
    case FLI1:
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1,param,addres1);
      SetValue(IN_TESTSIZE1,param,select_size);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    break;
    case FLI2:
      SetValue(IN_AREA,param,0);
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,0);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,addres1);
      SetValue(IN_TESTSIZE2,param,select_size);
    break;
    case EXTRA1:
    case EXTRA2:
    case EXTRA3:
    case EXTRA4:
    case EXTRA5:
    case EXTRA6:
      SetValue(IN_AREA,param,(mat-EXTRA1)+1);
      SetValue(IN_TOPADDR0,param,addres1);
      SetValue(IN_TESTSIZE0,param,select_size);
      SetValue(IN_TOPADDR1,param,0);
      SetValue(IN_TESTSIZE1,param,0);
      SetValue(IN_TOPADDR2,param,0);
      SetValue(IN_TESTSIZE2,param,0);
    break;
  }
	    
  return(PASS);
}

int VccSlow(int NewVcc, int OldVcc){
  int i,j;
  int VccStep;
  int SetVcc;
  int Sign;
  int result;
  
    //Initial
  VccStep = 4;
  //Vcc=0.05V  
  if(NewVcc == OldVcc){
	return(PASS);
  }else if(NewVcc < OldVcc){
    Sign = -1;
  }else{
    Sign =  1;
  }
  
  if( ((NewVcc <=WAR_MAX_VCC ) & (NewVcc >=WAR_MIN_VCC ))){
  }else{
    printf("-- FAIL -- vcc, ext_vrefH, ext_vref level over limit Error");
    return(FAIL);
  }

  SetVcc = OldVcc;
  for(SetVcc = OldVcc; (absolute(SetVcc - NewVcc) >= VccStep); SetVcc=SetVcc+Sign*VccStep){
    comp_set_v1(SetVcc);
    delay_timer(200);
  }

  result = comp_set_v1(NewVcc);
  delay_timer(100);
  sprintf(outbuf,"Settimg VccSlow Vcc = %d mV",NewVcc);
  DataOut(Ffpt,outbuf);
  
  set_vih1(NewVcc);
  
  return(result);
}

int PmuSlow(int NewPmu, int OldPmu){
  int i,j;
  int PmuStep;
  int SetPmu;
  int Sign;
  int result;
  
    //Initial
  PmuStep = 50;
  //Vcc=0.05V  
  if(NewPmu == OldPmu){
	return(PASS);
  }else if(NewPmu < OldPmu){
    Sign = -1;
  }else{
    Sign =  1;
  }

  SetPmu = OldPmu;
  for(SetPmu = OldPmu; (absolute(SetPmu - NewPmu) >= PmuStep); SetPmu=SetPmu+Sign*PmuStep){
    set_vlimit(SetPmu);
    delay_timer(100);
  }

  result = set_vlimit(NewPmu);
  delay_timer(100);
  sprintf(outbuf,"Settimg PmuSlow Pmu = %d mV",NewPmu);
  DataOut(Ffpt,outbuf);


  return(result);
}


int  MINORI_BM_Setting(interface_t* param, char *pat ,int *PatSize)
{
int Ldata[20];
int i,j,addr;
int datasize,patsize;
int tempo1;
char LastWord[2][10] = { {"_Main"}, {"_Out"} };

  for(addr=MINORI_BM_TOP_ADDR_MAIN;addr<=(MINORI_BM_END_ADDR_MAIN+0xFF);addr++){
  //BM All"0" Set
    WriteBmMode8(addr,0x00,SIZE_B);
  }
  
  for(addr=MINORI_BM_TOP_ADDR_OUT;addr<=(MINORI_BM_END_ADDR_OUT+0xFF);addr++){
  //BM All"0" Set
    WriteBmMode8(addr,0x00,SIZE_B);
    WriteEcrMode8(addr,0xFF,SIZE_B);
  }
  
  for(i=0;i<=1;i++){
    sprintf(binpat,"%s%s%s%s.pat",glob_cdp,glob_binpat_bist,pat,LastWord[i]);

        // ******* FILE BM WRITE *********
    patsize = CheckPatLength(binpat) - PAT_FILE_HEADER_SIZE;
    datasize = comp_load_bm (binpat);
    // comp_load_bm:file pattern => BM 
    if(FAIL==datasize) {
    // path/fail check 
      sprintf(outbuf," !!! NOT OPEN FILE PATH -> %s (checksum:H'%X) !!!",binpat,datasize);
      DataOut(Ffpt,outbuf);
            //DeviceLevelsPowerDown();
      return(NOPATFAIL);
    }else{
      sprintf(outbuf," LOAD FILE PATH -> %s (checksum:H'%X)",binpat,datasize);
      DataOut(Ffpt,outbuf);
    }
    sprintf(outbuf," FILE SIZE -> H'%X",patsize);
    DataOut(Ffpt,outbuf);
	*(PatSize+i) = patsize;
//	if(i==0){*PatSize1 = patsize;}
//	else{*PatSize2 = patsize;}
  }
//	printf(" Pat Size -> 0x%X",*(PatSize+i));
//	printf(" Pat Size -> 0x%X",*PatSize2);

  return(PASS);
}

int VrsgAnalyze(int Vresult[BGR_ARRAY_REPEAT], int *Vanalyze)
{
  int i,j;
  int period;
  int Vrsg_Max,Vrsg_Min;
  int Vsum,Vnum;

    //Vrsg̔背x
  Vrsg_Max = 3500;
  Vrsg_Min = 3000;
  
  period = 10;
  //1ʂ܂ŌvZB
  

    //------- d̕ϒľvZ -------//
    //for(i=0;i<=(BGR_ARRAY_VCC-1);i++){
    Vnum = 0;
    Vsum = 0;
    for(j=0;j<=(BGR_ARRAY_REPEAT-1);j++){
      if( (Vrsg_Max >= Vresult[j]) && (Vrsg_Min <= Vresult[j]) ){
        Vnum = Vnum + 1;
	Vsum = Vsum + Vresult[j];
      }else{

      }
    }
    if(Vnum >= (BGR_ARRAY_REPEAT/5)){
    //񐔂5ȉ̃j^[sFAILƂB
            //όʏó@
      *Vanalyze = Vsum/Vnum;
            //Vanalyze[i][1] = ((Vsum*period) / Vnum) % period;
    }else{
      *Vanalyze
      // = Vanalyze[i][1]
    }
    //}
    //printf("VrsgAnalyze : Vanalyze = %d mV",Vanalyze);
  return(PASS);

}

void ParamInitialize(interface_t* interface)
{
  int taddr,i;
  
  taddr = INPUTIF_TOP;
  for(i=0;i<IN_BYTEEND;i++){
    interface[i].addr = taddr;
    interface[i].size = 1;
    interface[i].value = 0;
    interface[i].wk = 0;
    taddr+=1;
  }
  for(i=IN_BYTEEND+1;i<IN_LWORDEND;i++){
    interface[i].addr = taddr;
    interface[i].size = 4;
    interface[i].value = 0;
    interface[i].wk = 0;
    taddr+=4;
  }

  taddr = OUTPUTIF_TOP;
  for(i=IN_LWORDEND+1;i<OUT_BYTEEND;i++){
    interface[i].addr = taddr;
    interface[i].size = 1;
    interface[i].value = 0;
    interface[i].wk = 0;
    taddr+=1;
  }
  for(i=OUT_BYTEEND+1;i<OUT_LWORDEND;i++){
    interface[i].addr = taddr;
    interface[i].size = 4;
    interface[i].value = 0;
    interface[i].wk = 0;
    taddr+=4;
  }
  
  InitialValue(interface);
  
}

void TempCheck()
{
  int tempo;
  
  printf("");
  printf("+------------------------------+");
  printf("+ Temp Check ");
  switch(Extra5_TempCheck){
    case EXTRA5_TEMP_ON:
      printf(" BGR Trim Code ON +");
    break;
    case EXTRA5_TEMP_OFF:
      printf(" BGR Trim Code OFF+");
    break;
    default:
      printf("No Select!!       +");
    break;
  }
  printf("+------------------------------+");
  printf("Enter Select Trim Code ON(1) / OFF(0) ->");
  DigitInput(&tempo);
  
  if(tempo == 1){
  	Extra5_TempCheck = EXTRA5_TEMP_ON;
    printf(" BGR Trim Code ON");
  }else if(tempo == 0){
  	Extra5_TempCheck = EXTRA5_TEMP_OFF;
    printf(" BGR Trim Code OFF");
  }else{
  	Extra5_TempCheck = EXTRA5_TEMP_OFF;
    printf(" ERROR!!! Input Number Error!!!");
    printf(" Code Select -> OFF Mode");
  }

  return(PASS);
}

int WSLot_CHECK(void)
{
  int i,result;
  
  result = PASS;
  
  for(i=0;i<=(WS1LOT_NUM-1);i++){
    if(0==strnicmp(lotname,ws1lot_string[i],6)) result = FAIL;
  }
  
  return(result);
}





int BGR_REG_CHECK(int vcc,interface_t* param)
{
int VccHitNum,tempo1;

  if(Extra5_TempCheck != EXTRA5_TEMP_ON) return(PASS);

        if( (1300 <= vcc) && (vcc <= 1400) ){
          VccHitNum = 0;
  }else if( (1500 <= vcc) && (vcc <= 1800) ){
          VccHitNum = 1;
  }else if( (2500 <= vcc) && (vcc <= 3000) ){
          VccHitNum = 2;
  }else if( (3100 <= vcc) && (vcc <= 3400) ){
          VccHitNum = 3;
  }else if( (3500 <= vcc) && (vcc <= 3700) ){
          VccHitNum = 4;
  }else{
    printf("+----------------------------+");
    printf("| VCC     ETLR00  ETLR01     |");
    printf("| %4dmV  0x%X    0x%X   (1) |",VCC_ARRAY0,Extra5_ETLR00[0],Extra5_ETLR01[0]);
    printf("| %4dmV  0x%X    0x%X   (2) |",VCC_ARRAY1,Extra5_ETLR00[1],Extra5_ETLR01[1]);
    printf("| %4dmV  0x%X    0x%X   (3) |",VCC_ARRAY2,Extra5_ETLR00[2],Extra5_ETLR01[2]);
    printf("| %4dmV  0x%X    0x%X   (4) |",VCC_ARRAY3,Extra5_ETLR00[3],Extra5_ETLR01[3]);
    printf("| %4dmV  0x%X    0x%X   (5) |",VCC_ARRAY4,Extra5_ETLR00[4],Extra5_ETLR01[4]);
    printf("+----------------------------+");
    printf("Select Use Code (1)/(2)/(3)/(4)/(5) ->");DigitInput(&tempo1);
    VccHitNum = tempo1 - 1;
  }


  SetValue(IN_ETLR00,param,Extra5_ETLR00[VccHitNum]);
  SetValue(IN_ETLR01,param,( (Extra5_ETLR01[VccHitNum] & 0x07) | (GetValue(IN_ETLR01,param) & 0xF8 ) ));

  DataOut(Ffpt,"  --  BGR Register Change  --  \n");
  
  return(PASS);

}



void BL_DATACLEAR(interface_t* param)
{
  
  SetValue(IN_BDATA00, param,  INI_BDATA00    );
  SetValue(IN_BDATA01, param,  INI_BDATA01    );
  SetValue(IN_BDATA02, param,  INI_BDATA02    );
  SetValue(IN_BDATA03, param,  INI_BDATA03    );
  SetValue(IN_BDATA04, param,  INI_BDATA04    );
  SetValue(IN_BDATA05, param,  INI_BDATA05    );
  SetValue(IN_BDATA06, param,  INI_BDATA06    );
  SetValue(IN_BDATA07, param,  INI_BDATA07    );
  SetValue(IN_BDATA08, param,  INI_BDATA08    );
  SetValue(IN_BDATA09, param,  INI_BDATA09    );
  SetValue(IN_BDATA10, param,  INI_BDATA10    );
  SetValue(IN_BDATA11, param,  INI_BDATA11    );
  SetValue(IN_BDATA12, param,  INI_BDATA12    );
  SetValue(IN_BDATA13, param,  INI_BDATA13    );
//  SetValue(IN_BDATA14, param,  INI_BDATA14    );
  SetValue(IN_BDATA15, param,  INI_BDATA15    );
  SetValue(IN_BDATA16, param,  INI_BDATA16    );
  SetValue(IN_LDATA0, param,   INI_LDATA0    );
  SetValue(IN_LDATA1, param,   INI_LDATA1    );
  SetValue(IN_LDATA2, param,   INI_LDATA2    );
  SetValue(IN_LDATA3, param,   INI_LDATA3    );
  SetValue(IN_LDATA4, param,   INI_LDATA4    );
  SetValue(IN_LDATA5, param,   INI_LDATA5    );

  return;
}


void Tokunin_Init(interface_t* param , char* FreeWord)
{
  int vs[NUM_POWERSUPPLY];
  int i,j,l;
  int exvref[3],exvrefh[3],vdd[3],vddh[3];
  int waittime,datasize,selvol,result,passfail_flag;
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  char filename[100],filepath[200];
  char time_buf[50];
  int first_flag,repeat_mode,inif_flag;
  char TesterName[10];
  int  NameLen;
  int info[32];
  // wmat chip information
  
    // ----- Initial ----- //
  exvref[0] = 443; exvrefh[0] = 452;
  exvref[1] = 500; exvrefh[1] = 500;
  exvref[2] = 728; exvrefh[2] = 580;
  waittime = 1;
  vdd[0]=vdd[1]=vdd[2]=0;
  vddh[0]=vddh[1]=vddh[2]=0;
  vs[0] = 3300;
  vs[1] = 500;
  vs[2] = 500;
  first_flag = 1;
  
    // ----- Get tester ID ----- //
  NameLen = 10;
  get_system_option("__szTesterID",TesterName,&NameLen);

// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_AREA,param,4);
      // Extra4
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      // t@[ŕύX\
      info[20]  = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);
      // Sample No

    // ----- File Name & Path ----- //
  sprintf(filename, "%s_W%d_X%dY%d_S%03d_InitLog",lotname,atoi(wafername),xy_location[0],xy_location[1],info[20]);
  sprintf(filepath, "%s%s%s%s.csv", glob_cdp, glob_datalog, glob_initlog, filename);
  
    // ----- Vdd/Vddh monitor ----- //
  SetValue(IN_BDATA00,param,0);
  
  for(i=0;i<=2;i++){
    Ffpt = 0;
    repeat_mode = RAMBOOT_CHANGE | INIF_SET;
    if(first_flag==1){
    // Initial mode
       Ffpt = 1;
       first_flag = 0;
       repeat_mode = repeat_mode | INITIAL_SET;
    }


    vs[2] = exvref[i];
    DeviceSpecificPowerUp();
    printf("MONITOR VDD");
    SetValue(IN_TESTSEL,param,MONITOR_VDD);
//    result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
    result = CpuModeMonitor_Repeat(repeat_mode,VCLMON_MON, &vs[0], param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
    vdd[i] = selvol;
    vs[2] = 500;
    if(result !=PASS) break;

    vs[1] = exvrefh[i];
    DeviceSpecificPowerUp();
    printf("MONITOR VDDH");
    SetValue(IN_TESTSEL,param,MONITOR_VDDH);
//    result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDDH",&selvol);
    result = CpuModeMonitor_Repeat(repeat_mode,VCLMON_MON, &vs[0], param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDDH",&selvol);
    vddh[i] = selvol;
    vs[1] = 500;
    if(result !=PASS) break;
  }
  Ffpt = 1;
  DeviceLevelsPowerDown();
  if(result !=PASS){
    printf(" Vdd / Vddh monitor Function FAIL!!!");
    return(FAIL);
  }
  

    // ----- Vdd monitor check ----- //
  passfail_flag = 0;
  for(i=0;i<=2;i++){
    if( (( (exvref[i]*158/100) - (VDD_DELTA*2) ) <= vdd[i] ) &&
        ( vdd[i] <= ( (exvref[i]*158/100) + (VDD_DELTA*2) ))  ){
    }else{
      passfail_flag++;
    }
  }
  
  if(passfail_flag == 0){
    printf(" Vdd  Monitor *** PASS ***");
  }else{
    printf(" Warning!!!");
    printf(" Vdd  Monitor --- FAIL ---");
  }

    // ----- Vdd monitor check ----- //
  passfail_flag = 0;
  for(i=0;i<=2;i++){
    if( (( (exvrefh[i]*25/10) - (VDDH_DELTA*2) ) <= vddh[i] ) &&
        ( vddh[i] <= ( (exvrefh[i]*25/10) + (VDDH_DELTA*2) ))  ){
    }else{
      passfail_flag++;
    }
  }
  
  if(passfail_flag == 0){
    printf(" Vddh Monitor *** PASS ***");
  }else{
    printf(" Warning!!!");
    printf(" Vddh Monitor --- FAIL ---");
  }




    // ----- File Open ----- //
  if((Fptdata=fopen(filepath,"at")) == NULL) {
    printf(" -- FAIL -- not open file:%s !",filepath);
    return(FAIL);
  }
  
    // ----- Initial Log ----- //
  time( &time_list ); GetDateTime( &time_list, time_buf, 1 );
  fprintf(Fptdata,"InitLog,Ver,1.2");
  fprintf(Fptdata,"Program-Info,Rev.%d",0);
  fprintf(Fptdata,"Time Stamp,%s",time_buf);
  fprintf(Fptdata,"Tester Info,%s,DUT,%d",TesterName,(get_site_number()+1));
  fprintf(Fptdata,"***** Free-Word,%s *****",FreeWord);
  fprintf(Fptdata, "***** Lot:%s,w#:W%d,Xlocation:X%d,Ylocation:Y%d,Sno:%03d *****",lotname,atoi(wafername),xy_location[0],xy_location[1],info[20]);
  
  
    // ----- Vdd/Vddh Log ----- //
  fprintf(Fptdata,"Vdd/Vddh Log");
  fprintf(Fptdata,"Exvref[mV],Vdd[mV],expect[mV],,Exvrefh[mV],Vddh[mV],expect[mV]");
  for(i=0;i<=2;i++){
    fprintf(Fptdata,"%d,%d,%d,,%d,%d,%d",exvref[i],vdd[i],exvref[i]*158/100,exvrefh[i],vddh[i],exvrefh[i]*25/10);
  }

    // ----- BGR Log ----- //
  fprintf(Fptdata,"BGR Log");
  if(Extra5_TempCheck == EXTRA5_TEMP_ON) fprintf(Fptdata,"WS1 SP Code,ON");
  else                                   fprintf(Fptdata,"WS1 SP Code,OFF");

  fprintf(Fptdata,"Vcc[mV],ETLR00,ETLR01");
  fprintf(Fptdata,"%d,0x%X,0x%X",VCC_ARRAY0,Extra5_ETLR00[0],Extra5_ETLR01[0]);
  fprintf(Fptdata,"%d,0x%X,0x%X",VCC_ARRAY1,Extra5_ETLR00[1],Extra5_ETLR01[1]);
  fprintf(Fptdata,"%d,0x%X,0x%X",VCC_ARRAY2,Extra5_ETLR00[2],Extra5_ETLR01[2]);
  fprintf(Fptdata,"%d,0x%X,0x%X",VCC_ARRAY3,Extra5_ETLR00[3],Extra5_ETLR01[3]);
  fprintf(Fptdata,"%d,0x%X,0x%X",VCC_ARRAY4,Extra5_ETLR00[4],Extra5_ETLR01[4]);
  
    // ----- ETLR Log ----- //
  fprintf(Fptdata,"ETLR Log");
  fprintf(Fptdata," ,ETLR*0,ETLR*1,ETLR*2,ETLR*3,ETLR*4,ETLR*5,ETLR*6,ETLR*7,ETLR*8,ETLR*9");
  for(i=0;i<=3;i++){
  fprintf(Fptdata,"ETLR%d*,",i);
    for(j=0;j<=9;j++){
      fprintf(Fptdata,"0x%X,",GetValue( (IN_ETLR00+(i*10)+j) ,param));
    }
  fprintf(Fptdata,",");
  }

  i=0;
    // ----- ETCR Log ----- //
  fprintf(Fptdata,"ETCR Log");
  fprintf(Fptdata," ,ETCR*0,ETCR*1,ETCR*2,ETCR*3,ETCR*4,ETCR*5,ETCR*6,ETCR*7,ETCR*8,ETCR*9");
  fprintf(Fptdata,"ETCR%d*,",i);

  for(j=0;j<=9;j++){
    fprintf(Fptdata,"0x%X,",GetValue( (IN_ETCR00+(i*10)+j) ,param));
  }
  fprintf(Fptdata,",");

  i++;
  fprintf(Fptdata,"ETCR%d*,",i);
  for(j=0;j<=7;j++){
    fprintf(Fptdata,"0x%X,",GetValue( (OUT_ETCR00+(i*10)+j) ,param));
  }
  fprintf(Fptdata,",");
  
  CloseDataOutFile();

  return;
}


int PMU_ADCDIGI_OUT(int mode,interface_t* param, char* filename)
{
  int sample_num,sample_rate;
  char data_name[200];
  int i;


  int  data_array1[1024],data_array2[1024],data_array3[1024],data_array4[1024];
  int  data_array5[1024],data_array6[1024],data_array7[1024],data_array8[1024];
  int  data_array9[1024],data_array10[1024],data_array11[1024],data_array12[1024];
  int  data_array13[1024],data_array14[1024],data_array15[1024],data_array16[1024];


  switch(mode){
    case 1:
      sample_num  = 1023;
      sample_rate = 5;
      //sample rate:5us
    break;
  }



#ifdef V5KONLY
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array1);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array2);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array3);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array4);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array5);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array6);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array7);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array8);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array9);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array10);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array11);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array12);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array13);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array14);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array15);
    read_adc_digitizer(ADC_IMEAS,0,sample_num,sample_rate,data_array16);
#endif

    // -- data_array Set -- //
    printf("ADC_DIGI FileOut");

    sprintf(data_name,"%s%s_T%d_01",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array1[i]/1000,data_array1[i]%1000,data_array1[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_02",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array2[i]/1000,data_array2[i]%1000,data_array2[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_03",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array3[i]/1000,data_array3[i]%1000,data_array3[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_04",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array4[i]/1000,data_array4[i]%1000,data_array4[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_05",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array5[i]/1000,data_array5[i]%1000,data_array5[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_06",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array6[i]/1000,data_array6[i]%1000,data_array6[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_07",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array7[i]/1000,data_array7[i]%1000,data_array7[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_08",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array8[i]/1000,data_array8[i]%1000,data_array8[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_09",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array9[i]/1000,data_array9[i]%1000,data_array9[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_10",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array10[i]/1000,data_array10[i]%1000,data_array10[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_11",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array11[i]/1000,data_array11[i]%1000,data_array11[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_12",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array12[i]/1000,data_array12[i]%1000,data_array12[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_13",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array13[i]/1000,data_array13[i]%1000,data_array13[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_14",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array14[i]/1000,data_array14[i]%1000,data_array14[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_15",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array15[i]/1000,data_array15[i]%1000,data_array15[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);
    sprintf(data_name,"%s%s_T%d_16",glob_currentlog,filename,GetValue(IN_EXTAL1,param));
    if(PASS != OpenDataOutFile(data_name)) delay_timer(2*1000*1000);
    for(i=0;i<=sample_num-1;i++){
      fprintf(Fptdata,"data_array,%02d,%d,%d.%03d,uA,%07dnA",i,i*sample_rate,data_array16[i]/1000,data_array16[i]%1000,data_array16[i]%10000000);
    }
    CloseDataOutFile();delay_timer(200);



}


void Firm_debugparam_Out(void){
  int i,j,addr,DataSet_num,Ldata_num;
  addr = FBM_READTOP;
  DataSet_num = 10-1;
  Ldata_num = 10-1;
  
  sprintf(outbuf,"===================================================================================================");DataOut(Ffpt,outbuf);
  for(i=0;i<=DataSet_num;i++){
    sprintf(outbuf,"------ DataSet%2d ------",i);DataOut(Ffpt,outbuf);
    for(j=0;j<=Ldata_num;j++){
      if((j%5 == 0) && (j != 0)){
        sprintf(outbuf,"");DataOut(Ffpt,outbuf);
      }
      sprintf(outbuf," Ldata%2d:0x%8X ",j,ReadEcrMode8(addr,SIZE_L));DataOut(Ffpt,outbuf);
      FirmDebug.DataSet[i].Data[j] = ReadEcrMode8(addr,SIZE_L);
      addr += 4;
    }
    sprintf(outbuf,"");DataOut(Ffpt,outbuf);
  }
  sprintf(outbuf,"===================================================================================================");DataOut(Ffpt,outbuf);

  return;
}

int StrobeSetting(int* strobepin, int judge_vol ){
  int result;
  result = 0;
  if(set_pe_resource(dpass_pins,PE_VOH,judge_vol)) result = result | 0x0001;
  if(set_pe_resource(dpass_pins,PE_VOL,judge_vol)) result = result | 0x0002;
  if(set_pe_resource(dpass_pins,PE_VTT,judge_vol)) result = result | 0x0004;
  if(add_strobe_mask(dpass_pins)) result = result | 0x0008;
  if(result != 0x0000){
    printf(" Strobe Setting Error!!! Result = %04X",result);
  }
  return(result);
}

