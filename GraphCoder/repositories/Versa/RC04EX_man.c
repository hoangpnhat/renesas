/*******************************************************************************
 *      FILENAME: RC04EXman.c
 *
 *      CONTENTS: This is a 128KB & 256KB flash application program
 *                for the V3300 test system.
 *
 *      COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.
 *                                     CREATED BY T.Shimozato
 *
 *  2008/11/01  rev.00  1'st created by H.Kobori
 *
 ******************************************************************************/

#include "common.h"
#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "compat.h"
#include "RC04EX_apg.h"
#include "RC04EX_tst.h"
#include "RC04EX_fnc.h"
//#include "RC04EX_WT_BGR.c"
#include "RC04EX_WT_AE.c"
#include "RC04EX_WT_VthTrim.c"
#include "RC04EX_QT_NoSet.c"
#include "RC04EX_WT_F_WS.c"

////////////////////////////////////////////////////////////////////////////////
//                      Shell2 Veriables                                      //
////////////////////////////////////////////////////////////////////////////////
int val_progrev = 0;  /*  Test Program Definitions  */
int used_sort_flows [] = {1, 2, 3, 4, 5, 6, 7, -1};

/*  Datalog Control Definitions */
dlog_bit_t  dlog_bit [] =
{
    /* bit 0 */     { ON,  "parametric tests" },
    /* bit 1 */     { ON,  "functional tests" },
    /* bit 2 */     { ON,  "speed tests"},
    /* bit 3 */     { ON,  "datalog mode"},
    /* bit 4 */     { ON,  "debug mode"},
    /* bit 5 */     { OFF, "unused"},
    /* bit 6 */     { OFF, "unused"},
    /* bit 7 */     { OFF, "unused"},
    /* bit 8 */     { OFF, "unused"},
    /* bit 9 */     { OFF, "unused"},
    /* bit 10 */    { OFF, "unused"},
    /* bit 11 */    { OFF, "unused"},
    /* bit 12 */    { OFF, "unused"},
    /* bit 13 */    { OFF, "unused"},
    /* bit 14 */    { OFF, "unused"},
    /* bit 15 is reserved by SHELL2, so don't modify it */
                    { ON,  "individual test time" },
    /* bit 16 */    { OFF, "unused"},
    /* bit 17 */    { OFF, "unused"},
    /* bit 18 */    { OFF, "unused"},
    /* bit 19 */    { OFF, "unused"},
    /* bit 20 */    { OFF, "unused"},
    /* bit 21 */    { OFF, "unused"},
    /* bit 22 */    { OFF, "unused"},
    /* bit 23 */    { OFF, "unused"},
    /* bit 24 */    { OFF, "unused"},
    /* bit 25 */    { OFF, "unused"},
    /* bit 26 */    { OFF, "unused"},
    /* bit 27 */    { OFF, "unused"},
    /* bit 28 */    { OFF, "unused"},
    /* bit 29 */    { OFF, "unused"},
    /* bit 30 */    { OFF, "unused"},
    /* bit 31 */    { OFF, "unused"}
};


/* Maps DUT pins to tester channels. */
int     pin_map [] =
{
    CH0 ,CH1 ,CH2 ,CH3 ,CH4 ,CH5 ,CH6 ,CH7 ,
    CH8 ,CH9 ,CH10,CH11,CH12,CH13,CH14,CH15,
    CH16,CH17,CH18,CH19,CH20,CH21,CH22,CH23,
    CH24,CH25,CH26,CH27,CH28,CH29,CH30,CH31,
    CH32,CH33,CH34,CH35,CH36,CH37,CH38,CH39,
    CH40,CH41,CH42,CH43,CH44,CH45,CH46,CH47,
    CH48,CH49,CH50,CH51,CH52,CH53,CH54,CH55,
    CH56,CH57,CH58,CH59,CH60,CH61,CH62,CH63,


    ENDLST
};


/* Maps DUT pin numbers to DUT pin names. */
dut_pin_t   dut_pin [] =
{
    {100,  "CH0 "},//for DUT pin name
    {101,  "CH1 "},//for DUT pin name
    {102,  "CH2 "},//for DUT pin name
    {103,  "CH3 "},//for DUT pin name
    {104,  "CH4 "},//for DUT pin name
    {105,  "CH5 "},//for DUT pin name
    {106,  "CH6 "},//for DUT pin name
    {107,  "CH7 "},//for DUT pin name
    {108,  "CH8 "},//for DUT pin name
    {109,  "CH9 "},//for DUT pin name
    {110,  "CH10"},//for DUT pin name
    {111,  "CH11"},//for DUT pin name
    {112,  "CH12"},//for DUT pin name
    {113,  "CH13"},//for DUT pin name
    {114,  "CH14"},//for DUT pin name
    {115,  "CH15"},//for DUT pin name
    {116,  "CH16"},//for DUT pin name
    {117,  "CH17"},//for DUT pin name
    {118,  "CH18"},//for DUT pin name
    {119,  "CH19"},//for DUT pin name
    {120,  "CH20"},//for DUT pin name
    {121,  "CH21"},//for DUT pin name
    {122,  "CH22"},//for DUT pin name
    {123,  "CH23"},//for DUT pin name
    {124,  "CH24"},//for DUT pin name
    {125,  "CH25"},//for DUT pin name
    {126,  "CH26"},//for DUT pin name
    {127,  "CH27"},//for DUT pin name
    {128,  "CH28"},//for DUT pin name
    {129,  "CH29"},//for DUT pin name
    {130,  "CH30"},//for DUT pin name
    {131,  "CH31"},//for DUT pin name
    {132,  "CH32"},//for DUT pin name
    {133,  "CH33"},//for DUT pin name
    {134,  "CH34"},//for DUT pin name
    {135,  "CH35"},//for DUT pin name
    {136,  "CH36"},//for DUT pin name
    {137,  "CH37"},//for DUT pin name
    {138,  "CH38"},//for DUT pin name
    {139,  "CH39"},//for DUT pin name
    {140,  "CH40"},//for DUT pin name
    {141,  "CH41"},//for DUT pin name
    {142,  "CH42"},//for DUT pin name
    {143,  "CH43"},//for DUT pin name
    {144,  "CH44"},//for DUT pin name
    {145,  "CH45"},//for DUT pin name
    {146,  "CH46"},//for DUT pin name
    {147,  "CH47"},//for DUT pin name
    {148,  "CH48"},//for DUT pin name
    {149,  "CH49"},//for DUT pin name
    {150,  "CH50"},//for DUT pin name
    {151,  "CH51"},//for DUT pin name
    {152,  "CH52"},//for DUT pin name
    {153,  "CH53"},//for DUT pin name
    {154,  "CH54"},//for DUT pin name
    {155,  "CH55"},//for DUT pin name
    {156,  "CH56"},//for DUT pin name
    {157,  "CH57"},//for DUT pin name
    {158,  "CH58"},//for DUT pin name
    {159,  "CH59"},//for DUT pin name
    {160,  "CH60"},//for DUT pin name
    {161,  "CH61"},//for DUT pin name
    {162,  "CH62"},//for DUT pin name
    {163,  "CH63"},//for DUT pin name
    {200,  "V1  "},//for DUT pin name
    {300,  "V2  "},//for DUT pin name
    {400,  "V3  "},//for DUT pin name
    {500,  "V4  "},//for DUT pin name

    {0,  "   "}     /* end of list */
};


/* Maps channel numbers to channel names. */
char    chan_name [] [6] =
{
    "PE0  ", "PE1  ", "PE2  ", "PE3  ", "PE4  ", "PE5  ", "PE6  ", "PE7  ",
    "PE8  ", "PE9  ", "PE10 ", "PE11 ", "PE12 ", "PE13 ", "PE14 ", "PE15 ",
    "PE16 ", "PE17 ", "PE18 ", "PE19 ", "PE20 ", "PE21 ", "PE22 ", "PE23 ",
    "PE24 ", "PE25 ", "PE26 ", "PE27 ", "PE28 ", "PE29 ", "PE30 ", "PE31 ",
    "PE32 ", "PE33 ", "PE34 ", "PE35 ", "PE36 ", "PE37 ", "PE38 ", "PE39 ",
    "PE40 ", "PE41 ", "PE42 ", "PE43 ", "PE44 ", "PE45 ", "PE46 ", "PE47 ",
    "PE48 ", "PE49 ", "PE50 ", "PE51 ", "PE52 ", "PE53 ", "PE54 ", "PE55 ",
    "PE56 ", "PE57 ", "PE58 ", "PE59 ", "PE60 ", "PE61 ", "PE62 ", "PE63 ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
    "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     "

};

////////////////////////////////////////////////////////////////////////////////
//                      Shell2 Functions                                      //
////////////////////////////////////////////////////////////////////////////////

/*
--------------------------------------------------------------------------------
        Function:       PrintTestProgramBanner

        Description:    A general purpose function that Shell2 automatically
                        executes during test program initialization only.
--------------------------------------------------------------------------------
*/

void    PrintTestProgramBanner (void)
{
    print_dlog("\n");
    print_dlog("Device Type:   %s\n", val_devtype);
    print_dlog("Test Program:  %s\n", program_name);
    print_dlog("Revision:      %d\n", val_progrev);
}

/*
--------------------------------------------------------------------------------
        Function:       PrintTestNameFirst

        Description:    A general purpose function that Shell2 automatically
                        executes at the beginning of every test block.
--------------------------------------------------------------------------------
*/

void    PrintTestNameFirst (int dlog, char * tname)
{
    print_dlog ("\n");

    Do_For_Each_Alive_DUT
    {
        if (dlog & DLOG_DEBUG)
            print_dlog ("Performing %s test on DUT %c.\n", tname,
                    (int) 'A' + dut_count);
    }
}

/*
--------------------------------------------------------------------------------
        Function:       PrintTestFunctionResult

        Description:    A general purpose function that Shell2 automatically
                        executes at the end of every test block.
--------------------------------------------------------------------------------
*/

void    PrintTestFunctionResult (int dlog, int dlog_mask, int test_time,
            int status, char * tname)
{
    Do_For_Each_Alive_DUT
    {
        datalog_dut_num (dut_count);

        if (status & (1 << dut_count))
            print_dlog ("DUT %c failed %s.\n", (int) 'A' + dut_count, tname);
        else
            print_dlog ("DUT %c passed %s.\n", (int) 'A' + dut_count, tname);

        if (dlog & LOG_TEST_TIME)
        {
            printf ("T_%s= %d (uS)\n", tname, test_time);
        }
    }
}

/*
--------------------------------------------------------------------------------
        Function:       PrintDieResult

        Description:    A general purpose function that Shell2 automatically
                        executes at the end of every die tested, just prior
                        to binning.
--------------------------------------------------------------------------------
*/

void    PrintDieResult (int dlog, int dut_num, int status, int bin,
            int test_time)
{
    datalog_dut_num (dut_num);

    if (status)
        print_dlog ("\nDUT %c Failed.\n", (int) 'A' + dut_num);
    else
        print_dlog ("\nDUT %c Passed.\n", (int) 'A' + dut_num);

    print_dlog ("Bin= %d\n", bin);
    print_dlog ("TOTAL_TIME= %d.%03d s.\n", test_time / 1000000, (test_time / 1000) % 1000);
}




/*
--------------------------------------------------------------------------------
    Function:       CheckBoardConfig

    Description:    A function that verifies the proper board config
                    during test program initialization.
--------------------------------------------------------------------------------
*/
int     CheckBoardConfig (void)
{
int     status;

    status = PASS;
    return (status);
}


/*******************************************************************************
 *  Define external value
 ******************************************************************************/
char binpat[256];
char datapat[256];
char baselog[256];
char filenamebuf[128];
char outbuf[256];

char  *val_devtype  = devicename;               /* Device name      */
char  *program_name = PROGRAMNAME;              /* Program revision */
char  *Tester_name  = "V___________________";   /* default 20char   */
/*------------------------*/
/*---- PROBE DIRECTRY ----*/
/*------------------------*/
char  *glob_cdp     = CURRENTDR;
char  *glob_fbm     = "\\FBM\\";
char  *glob_probe   = "\\PROBE\\";
char  *glob_binpat_at   = "\\BINPAT\\AT\\";
char  *glob_binpat_wt   = "\\BINPAT\\WT\\";
char  *glob_binpat_logic ="\\BINPAT\\LOGIC\\";
char  *glob_binpat_boot = "\\BINPAT\\BOOT\\";
char  *glob_binpat_bist = "\\BINPAT\\MINORI\\";
char  *glob_datalog = "\\DATALOG\\";
char  *glob_initlog = "\\INITLOG\\";
char  *glob_currentlog = "\\CURRENT\\"; 
char  *glob_bunpu   = "\\BUNPU\\";
char  *glob_cond    = "\\CONDITION\\";
char  *glob_retention = "\\RETENTION\\";
char  *glob_Extralog = "\\EXTRA\\";

FILE_T  *Fptdata;

int   Ffpt;
char  lotname[32];
char  wafername[32];
char  devicename[32];
char  temp_comment[32];

int   test;          /* test number */

time_t   time_list;  /* Date & Time value  */
time_t   time_list2; /* Date & Time value2 */
int   Cputotal; /* Cpu mode total test time */
int   Cpuferr;

int   configration = 0;             /* Release = 0, Debug = 1 */
char  configstate[16] = "Release";  /* Release or Debug       */

int   Extra5_ETLR00[BGR_ARRAY_VCC];
int   Extra5_ETLR01[BGR_ARRAY_VCC];
int   Extra5_TempCheck;

////////////////////////////////////////////////////////////////////////////////
/* Following value is ecrmemory analisys variable (I don't understand.) */
int    ebrbitmask1[10] = {0x0001,0x0002,0x0004,0x0008,
      0x0010,0x0020,0x0040,0x0080,0x0100,0x0200};
int    ebrbitmask2[10] = {0xfffe,0xfffd,0xfffb,0xfff7,
      0xffef,0xffdf,0xffbf,0xff7f,0xfeff,0xfdff};
int    bitmask0[8] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
int    bitmask1[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
int    bitmask[] = {0xfffe,0xfffd,0xfffb,0xfff7,0xffef,0xffdf,0xffbf,0xff7f,
      0xfeff,0xfdff,0xfbff,0xf7ff,0xefff,0xdfff,0xbfff,0x7fff};
int    REJ_BIN_LIST[] = {1,0,-1}; /* if bin=1 pass else fail */
////////////////////////////////////////////////////////////////////////////////



int tyear,tmon,tday,thour,tmin,tsec;
char datetime_string[15];
int fc_key = 0;
int shot_number = 0;
int SampleNo;
int QT_SampleNo;

DAXS_TRANS_COND_T daxs_trans = {
  INIT_VCC,
  INIT_VREFH,       /* ext_VrefH(VDDH) */
  INIT_VREF,        /* ext_vref(VDD) */
  INIT_DAXS_WR_RATE,
  INIT_DAXS_RD_RATE,
  INIT_DAXS_VOH,    /* VOH *//* caution!! DeviceLevelsPowerUp() */
  INIT_DAXS_VOL,    /* VOL */
  INIT_DAXS_VTT,    /* VTT V5000 */
};

int glb_temp;
int glob_pmode_flp = 0;  /* FLP PROGRAM MODE */
int glob_pmode_e2p = 1;  /* E2P PROGRAM MODE */

interface_t interface_store_buff[TABLE_END];

/*******************************************************************************
 * Cpu function test pattern define
 ******************************************************************************/
/*******************************************************************************
 * Write Debug Data file define
 ******************************************************************************/
char dataout_logfname[255] = "DATA.LOG";  /* defalut \DATALOG\DATA.LOG_%d.csv */

/******************************************************************************/
/*  Main Test Program Flow                                                    */
/*  Use Test button or F11 to execute this flow                               */
/******************************************************************************/
int DoTestProgram(void)
{
  int pass_bin,fail_bin,result_bin, i, taddr;
  int tempo1,tempo2;
  static interface_t interface[TABLE_END];
  char TesterName[10];
  char optbuf[0x40];
  int  NameLen;

// Modified by Kenji Kasuya at Verigy on 5/23/2007
// Some test program have stack over flow error.
// Because "ex k" command in V3300 used "ex menu" command in Exe_Menu_With_Var1
// function.
// We have to modify this function and set always as below variable
// when we fix this bug.
// Please see an Exe_Menu_With_Var1 function in compat.c file
  var1 = temp_global_var1;
  var2 = temp_global_var2; fc_key = var2;
  GetLotName(lotname, sizeof(lotname));         /* Get Lot Name   */
  GetWaferName(wafername, sizeof(wafername));   /* Get Wafer Name */
  strcpy( devicename, DEVICENAME );             /* Set Device Name */
  strcpy( temp_comment, "Typ 25'C" );           /* Init temp_comment */
  NameLen = 10;
  get_system_option("__szTesterID",TesterName,&NameLen);


  if(strcmp(TesterName,"V5000-3")==0) glob_cdp = CURRENTDR_D_DRIVE;
  if(strcmp(TesterName,"V5000-4")==0) glob_cdp = CURRENTDR_D_DRIVE;

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


  pass_bin = 1;
  strcpy( datetime_string, "20110913084500" );

  printf("\n********************************\n");
  printf( "%.30s\n", devicename );
  printf("RAM   : %dKB\n", RAM_SIZE/1024 );
  printf("********************************\n");

  if (test_flow == 1){
    if (NORMAL_FLOW || auto_ex_cmd_only){
      if(var1 == 0) {

        printf("\n Hello Versa Test World !! ");

        if(DeviceSpecificInit() != PASS){
          printf("Device Initialize Error !!\n");
          return(FAIL);
        }
        printf("\nPASS Initialize\n");

        DeviceSpecificPowerDown();
        printf("PASS SpecificPowerDown\n");

        if(AnalisysTestProgram(interface,0) != PASS){
          printf("Select Program ERROR !!\n");
        }
        DeviceSpecificPowerDown();
        PowerDown(1);
        return(PASS);

      }else{
        if(DeviceSpecificInit() != PASS){
          printf("Device Initialize Error !!\n");
          PowerDown(255);
        }
        printf("\nPASS Initialize\n");
        DeviceSpecificPowerDown();
        printf("PASS SpecificPowerDown\n");

        switch(var1){
        case BWT_FL0:
        case BWT_FL1:
        case BWT_FL2:
        case BWT_FL4:
        case BWT_FL8:
        case BWT_FL9:
        case BWT_FL0D:
        case BWT_FL1D:
        case BWT_FL2D:
        case BWT_FL4D:
        case BWT_FL8D:
        case BWT_FL9D:
        case BFT_FL1:
        case BFT_FL2:
        case BFT_FL1D:
        case BFT_FL2D:
          result_bin = WaferTestProgram( var1&0x00FF ); /* Wafer Test Versa Emulation */
          break;
        /*
        case BGR_WT1:                             
          result_bin = BGR_WT(interface,1);     
        break;                                         
        case BGR_WT2:
          tempo1 = bentime();
          result_bin = BGR_WT_HT(interface);     
          tempo2 = (bentime()-tempo1-init_bentime())/1000;
          printf("\n BGR_WT2 RunTime = %d[ms]\n",tempo2);
          break;                                            
        case BGR_WT4:                                        
          result_bin = BGR_WT(interface,4);    
          break;                                     
        case BGR_WT5:                                      
          result_bin = BGR_WT(interface,5); 
          break;
        */                                          
        case AE_WT:
        	 result_bin = AE_WT1(interface,0);
        	break;
        case AE_WT2:
     	    result_bin = AE_WT1(interface,1);
      	   break;
        case VTHTRIM_WT:
        	result_bin = WT_Vth_Trim(interface);
      	break;
        case WT_F_WS:
        	result_bin = WT_WS_Flash(interface);
        break;
      	case QT_SNO:
      	    result_bin = BGR_QT_NoSet(interface,QT_SampleNo);
            printf("+====================+\n");
            printf("| Sample No, ->%d\n",QT_SampleNo);
            printf("+====================+\n");
      	    QT_SampleNo = QT_SampleNo + 4;
      	break;
       default:
          return( PASS );
        }  /* end switch */
        DeviceSpecificPowerDown();
        if( result_bin == PASS ){ PowerDown( 1 );} else { PowerDown( result_bin ); }
      }
    }
  }  /* end if test_flow == 1 */
}

/*******************************************************************************
 * ExCommand
 ******************************************************************************/

#define ExCommand(string) (strnicmp(argv[0],string,(strlen(argv[0])>strlen(string) ? strlen(argv[0]):strlen(string)))==0)

/*******************************************************************************
 * User EX command Function
 ******************************************************************************/
int UserExCommand(int argc,char *argv[])
{
  int fk_bit, i, j;
  int len;
  mode_table_t optBuf;
  len = sizeof(optBuf);


  if(ExCommand("k"))     { Exe_Menu_With_Var1( 0x0000 ) ; exit(PASS); }
  if(ExCommand("ws"))    { Exe_Menu_With_Var1( WT_F_WS ) ; exit(PASS); }
  if(ExCommand("wt0"))   { Exe_Menu_With_Var1( BWT_FL0 ); exit(PASS); }
  if(ExCommand("wt1"))   { Exe_Menu_With_Var1( BWT_FL1 ); exit(PASS); }
  if(ExCommand("wt2"))   { Exe_Menu_With_Var1( BWT_FL2 ); exit(PASS); }
  if(ExCommand("wt4"))   { Exe_Menu_With_Var1( BWT_FL4 ); exit(PASS); }
  if(ExCommand("wt8"))   { Exe_Menu_With_Var1( BWT_FL8 ); exit(PASS); }
  if(ExCommand("wt9"))   { Exe_Menu_With_Var1( BWT_FL9 ); exit(PASS); }
  if(ExCommand("ft1"))   { Exe_Menu_With_Var1( BFT_FL1 ); exit(PASS); }
  if(ExCommand("ft2"))   { Exe_Menu_With_Var1( BFT_FL2 ); exit(PASS); }

  if( ExCommand("wt0d") || ExCommand("wt1d") || ExCommand("wt2d") || ExCommand("wt4d") || ExCommand("wt8d") || ExCommand("wt9d")
    || ExCommand("ft1d") || ExCommand("ft2d") ){
    fc_key = 0x03900000; var2 = fc_key; Exe_Menu_With_Var2( var2 ); printf( "debug FK set = 20 23 24 25, other clear\n" );
  }
  if(ExCommand("wt0d"))   { Exe_Menu_With_Var1( BWT_FL0D ); exit(PASS); }
  if(ExCommand("wt1d"))   { Exe_Menu_With_Var1( BWT_FL1D ); exit(PASS); }
  if(ExCommand("wt2d"))   { Exe_Menu_With_Var1( BWT_FL2D ); exit(PASS); }
  if(ExCommand("wt4d"))   { Exe_Menu_With_Var1( BWT_FL4D ); exit(PASS); }
  if(ExCommand("wt8d"))   { Exe_Menu_With_Var1( BWT_FL8D ); exit(PASS); }
  if(ExCommand("wt9d"))   { Exe_Menu_With_Var1( BWT_FL9D ); exit(PASS); }
  if(ExCommand("ft1d"))   { Exe_Menu_With_Var1( BFT_FL1D ); exit(PASS); }
  if(ExCommand("ft2d"))   { Exe_Menu_With_Var1( BFT_FL2D ); exit(PASS); }
  if(ExCommand("bgr1"))   { Exe_Menu_With_Var1( BGR_WT1  ); exit(PASS); }
  if(ExCommand("bgr5"))   { Exe_Menu_With_Var1( BGR_WT5  ); exit(PASS); }
  if(ExCommand("bgr2"))   { Exe_Menu_With_Var1( BGR_WT2  ); exit(PASS); }
  if(ExCommand("bgr4"))   { Exe_Menu_With_Var1( BGR_WT4  ); exit(PASS); }
  if(ExCommand("vthtrim")){ Exe_Menu_With_Var1(VTHTRIM_WT);  exit(PASS); }
  if(ExCommand("ae"))     { Exe_Menu_With_Var1( AE_WT    );  exit(PASS); }
  if(ExCommand("ae2"))    { Exe_Menu_With_Var1( AE_WT2  );  exit(PASS); }
  if(ExCommand("help"))   { DisplayHelp();               exit(PASS); }
  if(ExCommand("men"))    { DisplayMenu();               exit(PASS); }
  if(ExCommand("qt_sno")) { 
    Exe_Menu_With_Var1( QT_SNO   ); 
    printf("Input First Sample Number ->");DigitInput(&QT_SampleNo);
    QT_SampleNo = QT_SampleNo + get_site_number();
    exit(PASS);
  }
  if( ExCommand( "fk" ) ||  ExCommand( "FK" ) ){
    /* function key set */
    if( argc>1 ){
      if( 0==strncmp( argv[1], "0x", 2 ) ){
        fc_key = strtod( argv[1] );      /* fc_key int set */
      }else{
        for( i=1; i<argc; i++ ){
          fk_bit = 0x01;
          for( j=0; j<32; j++, fk_bit = fk_bit<<1 ){
            if( j==strtod( argv[i] ) ){
              if( fc_key&fk_bit )  fc_key &= ~fk_bit;  /* fc_key bit clear */
              else                 fc_key |=  fk_bit;  /* fc_key bit set */
            }
          }
        }
      }
      var2 = fc_key; Exe_Menu_With_Var2( var2 );  /* set var2 no mean? */
    }
    DisplayMenu();
    exit(PASS);
  }  /* end else if FK */

  printf("\n!!! input err (ex ?) !!!\n");
  return(FAIL);  /* unrecognized EX command */

}

/*============================================================================*/
void DisplayMenu(void)
/*============================================================================*/
{
  int i;
  int length;
  char device_id[100];

  length = sizeof(device_id);

  printf("| "); printf( "%.30s", devicename );
  printf("\n");
  printf("+==========================================\n");
  printf("|   TEST COMMAND MENU\n");
  get_system_option("__szDeviceID", device_id, &length); /* Get Device ID */
  printf("|   [ %s ]\n", device_id);
  printf("+==========================================\n");
  printf("|  TEST CONDITION      | COMMAND\n");
  printf("+------------------------------------------\n");
  printf("| KAISEKI              | ex k\n");
  printf("| Wafer Test 1         | ex wt1\n");
  printf("| Wafer Test 2         | ex wt2\n");
  printf("| Tokusei Nintei       | ex tk1\n");
  printf("+---- Function Key ------------------------\n");
  for( i=0; i<32; i++ ){
    if( CheckFK(i) ) printf(" %d," , i);
  };
  printf("\n+------------------------------------------\n");

  if(var1 != 0) {
    if     ( BWT_FL0==var1 ) printf("|  MODE CONDITION -> WT0\n");
    else if( BWT_FL1==var1 ) printf("|  MODE CONDITION -> WT1\n");
    else if( BWT_FL2==var1 ) printf("|  MODE CONDITION -> WT2\n");
    else if( BWT_FL4==var1 ) printf("|  MODE CONDITION -> WT4\n");
    else if( BWT_FL8==var1 ) printf("|  MODE CONDITION -> WT8\n");
    else if( BWT_FL9==var1 ) printf("|  MODE CONDITION -> WT9\n");
    else                     printf("|  condition now : ????????????? \n");
    printf("|  ***** please start Probing ***** \n");
  } else {
    printf("|  MODE CONDITION NOW -> KAISEKI \n");
    printf("|  !!! caution !!! No Probe \n");
  }
  printf("+------------------------------------------\n");
}

/*============================================================================*/
void DisplayHelp(void)
/*============================================================================*/
{
  printf("\n***********************************************************\n");
  printf("* Main Test Program Flow\n");
  printf("* Use Test button or F11 to execute this flow\n");
  printf("***********************************************************\n");
  printf("ex 0:         Device analisys program(analisys,debug,etc.)\n");
  printf("ex wtX:       Wafer Test FlowX exe\n");
  printf("ex ftX:       Final Test FlowX exe\n");
  printf("ex wt*d/ft*d: FK=20 23 24 25 & Test Debug exe\n");
  printf("ex fk a b c:  Function Key a b c toggle set/clear\n");
  printf("ex fk 0x1234: Function Key 12 9 5 4 3 set other clear\n");
}
/******************************************************************************/
/* Do summary Function                                                        */
/******************************************************************************/
int DoSummary(void)
{
  return(PASS);
}

/*******************************************************************************
 * Do Print summary Function
 ******************************************************************************/
int DoPrintSummary (void)
{
  printf("\nPerforming Partial Summary operation for %s.\n\n",
    program_name);

  return(PASS);
}

/*******************************************************************************
 * Prober Alignment Function (F12)
 *    Use Test button or F12 to execute this flow
 ******************************************************************************/

int DoProberAlignment (void)
{
  DeviceSpecificPowerDown();

  return(PASS);
}

