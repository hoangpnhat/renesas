/*******************************************************************************
*  FILENAME RC04EX_FT2.c
*
  CONTENTS: This is the RC04EEX(SOTB) FLASH module function program
  for the 5000 test system
  COPYRIGHT 2018 Renesas Technology ALL RIGHTS RESERVED
  1'st created by Y.Aoki(SRM13)
 ******************************************************************************/
#include "RC04EX_WT.h"

FLOW_COND_ST ft2_flow[] = {
// RC04EX_IO Logic Trimming  2019/09/26

//+----+------+------+------+---------------+-+---+------------------------+-----+-----+-----+-------+---+-+-+----------+----------+----------+-------+-----------------------------+-----------------------------+-----------------------------+-------------------------
//| S  | T    | F    | F    | T             |E| C | I                      | V   | V   | V   | E     | T |P|P| M        | L        | L        | S     | P                           | P                           | T                           | I                       
//| N  | N    | K    | K    | I             |X| A | T                      | C   | R   | R   | X     | I |O|O| O        | I        | I        | U     | A                           | A                           | R                           | T                       
//| O  | O    |      |      | M             |P| T | E                      | C   | E   | E   | T     | M |N|F| N        | M        | M        | P     | T                           | R                           | A                           | E                       
//|    |      | O    | O    | M             | |   | M                      |     | F   | F   | A     | E | |F| T        | I        | I        | P     |                             | A                           | N                           | M                       
//|    |      | N    | F    | I             | |   |                        |     |     | H   | L     | O | | | Y        | T        | T        | L     |                             | M                           | S                           | 2                       
//|    |      |      | F    | N             | |   |                        |     |     |     |       | U | | | P        | L        | H        | Y     |                             |                             |                             |                         
//|    |      |      |      | G             | |   |                        |     |     |     |       | T | | | E        | O        | I        |       |                             |                             |                             |                         
//+----+------+------+------+---------------+-+---+------------------------+-----+-----+-----+-------+---+-+-+----------+----------+----------+-------+-----------------------------+-----------------------------+-----------------------------+-------------------------
  {   0, 10100,0x0000,0x0000,"FL_OPST"      ,3,'A',"Open"                  ,    0,    0,    0,      0,  0,0,0,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"DC "                             },
  {  10, 10500,0x0000,0x0000,"FL_STD_F"     ,1,'2',"Selftest FAIL"         , 1600,  455,  452, 500000,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_FNC000_MT_V010000","RC04EX_IO_WT1_P1050_V010000",""                           ,"Basic Function "                 },
  {  20, 10600,0x0000,0x0000,"FL_STD"       ,3,'2',"Selftest PASS"         , 1600,  455,  452, 500000,  1,0,0,""        ,         0,         0,      0,"RC04EX_IO_FNC000_MT_V010000","RC04EX_IO_WT1_P1060_V010000",""                           ,"Basic Function "                 },
  {  20, 10850,0x0000,0x0000,"FL_STD"       ,3,'w',"Initialize Work for Re", 3300,  500,  500,  31250,  1,0,0,""        ,         0,         0,      0,"RC04EX_IO_WRK020_MT_V010000","RC04EX_IO_WT1_P1085_V010000",""                           ,"Work E5 test"                    },
  {  30, 10900,0x0000,0x0000,"FL_STD"       ,3,'?',"Reject WT1 PASS Device", 3300,  500,  500,  31250,  1,0,0,""        ,         0,         0,      0,"RC04EX_IO_CRD004_MT_V010000","RC04EX_IO_WT1_P1090_V010000",""                           ,"Work E5 "                        },
  {  40, 11000,0x0000,0x0000,"FL_EXT"       ,3,'5',"Program Initial Trimmi", 3300,  500,  500, 500000,  1,0,0,""        ,         0,         0,      0,"RC04EX_IO_WRK022_MT_V010000","RC04EX_IO_WT1_P1100_V010100","RC04EX_IO_Extra1_V000"      ,"Work E1 E5 ng Value &Product Tr"},
  {  50, 15400,0x0000,0x0000,"FL_EXT"       ,3,'y',"Program Option Byte Bu", 3300,  500,  500,  31250,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1540_V010000","RC04EX_IO_OptionByte_V000"  ,"Configuration E5 ffer"           },

//{  50,     0,0x0000,0x0000,""             ,3,' ',"LogicWT1"              ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },

  { 100, 80042,0x0000,0x0000,"FL_LOGICTRM"  ,3,'N',"BGR_Trim LT           ", 2830,    0,    0, 100000, 20,1,0,""        ,         0,         0,      0,"bgr1_trim_v001"             ,""                           ,""                           ,""                                },
  {   0, 80044,0x0000,0x0000,"FL_STD"       ,3,'N',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80142,0x0000,0x0000,"FL_LOGICTRM"  ,3,'N',"BGR_Trim LT ADJT7 Set ", 3300,    0,    0, 100000, 20,1,0,""        ,         0,         0,      0,"bgr1_5_trim_v001"           ,""                           ,""                           ,""                                },
  {   0, 80144,0x0000,0x0000,"FL_STD"       ,3,'N',"ReProgram Product Trim", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK029_MT_V010000","RC04EX_IO_WT1_P1000_V010000",""                           ,"Logic E1 E5 ming Data for WT"    },
  { 100, 80000,0x0000,0x0000,"FL_LOGICTRM"  ,3,'N',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },

  { 100, 80002,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"LOCO_Trim             ", 3300,    0,    0, 100000, 10,1,0,""        ,         0,         0,      0,"loco_trim_v001"             ,""                           ,""                           ,""                                },
  {   0, 80004,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Product Trim", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK029_MT_V010000","RC04EX_IO_WT1_P1000_V010000",""                           ,"Logic E1 E5 ming Data for WT"    },
  { 100, 80012,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"MOCO_Trim             ", 3300,    0,    0, 100000, 10,1,0,""        ,         0,         0,      0,"moco_trim_v001"             ,""                           ,""                           ,""                                },
  {   0, 80014,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Product Trim", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK029_MT_V010000","RC04EX_IO_WT1_P1000_V010000",""                           ,"Logic E1 E5 ming Data for WT"    },
  { 100, 80022,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"ILOCO_Trim            ", 3300,    0,    0, 100000,  2,1,0,""        ,         0,         0,      0,"iloco_trim_io_v001"         ,""                           ,""                           ,""                                },
  {   0, 80024,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80026,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"HVOCO_Trim            ", 3300,    0,    0, 100000,  2,1,0,""        ,         0,         0,      0,"hvoco_trim_v001"            ,""                           ,""                           ,""                                },
  {   0, 80028,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80032,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"HOCO_Trim LT          ", 3300,    0,    0, 100000, 30,1,0,""        ,         0,         0,      0,"hoco1_trim_io_v001"         ,""                           ,""                           ,""                                },
  {   0, 80034,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80052,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"Sub OSC Trim          ", 3300,    0,    0, 100000,  2,1,0,""        ,         0,         0,   1088,"subosc_trim_v001"           ,""                           ,""                           ,""                                },
  {   0, 80054,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80102,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"OCO monitor           ", 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"oco_monitor_io_v001"        ,""                           ,""                           ,""                                },
  { 100, 80104,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },

  {  60, 19860,0x0000,0x0000,"FL_WRK"       ,3,'p',"Program Extra1 for Era", 3300,  500,  500,  31250,  1,1,1,""        ,         0,         0,      0,"RC04EX_IO_WRK014_MT_V010000","RC04EX_IO_WT1_P1986_V010000",""                           ,"Trimming E1 E5 se Vth Trimming"  },
  {  80, 34300,0x0000,0x0000,"FL_STD"       ,3,'w',"Program WT1 Passcode"  , 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK012_MT_V010000","RC04EX_IO_WT1_P3430_V010000",""                           ,"Work E5 "                        },


//{  50,     0,0x0000,0x0000,""             ,3,' ',"LogicWT8"              ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },

  { 100, 80042,0x0000,0x0000,"FL_LOGICTRM"  ,3,'N',"BGR_Trim HT 125'C     ", 2830,    0,    0, 100000, 20,1,0,""        ,         0,         0,      0,"bgr2_trim_v001"             ,""                           ,""                           ,""                                },
  {   0, 80044,0x0000,0x0000,"FL_STD"       ,3,'N',"ReProgram Product Trim", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK029_MT_V010000","RC04EX_IO_WT1_P1000_V010000",""                           ,"Logic E1 E5 ming Data for WT"    },
  { 100, 80000,0x0000,0x0000,"FL_LOGICTRM"  ,3,'N',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },

  { 100, 80032,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"HOCO_Trim HT 125'C    ", 3300,    0,    0, 100000, 10,1,0,""        ,         0,         0,      0,"hoco2_trim_io_v001"         ,""                           ,""                           ,""                                },
  {   0, 80034,0x0000,0x0000,"FL_STD"       ,3,'D',"ReProgram Option Byte ", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK027_MT_V010000","RC04EX_IO_WT1_P1010_V010000",""                           ,"Logic E5 for WT"                 },
  { 100, 80102,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"OCO monitor           ", 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"oco_monitor_io_v001"        ,""                           ,""                           ,""                                },
  { 100, 80104,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },

  { 100, 20600,0x0000,0x0000,"FL_WRK"       ,3,'w',"Program WT8_Rev & Year", 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK010_MT_V010000","RC04EX_IO_WT8_P2060_V010000",""                           ,"Work E5 /Month/Date code"        },
  { 120, 46700,0x0000,0x0000,"FL_STD"       ,3,'w',"Program WT8 Passcode"  , 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK012_MT_V010000","RC04EX_IO_WT8_P4670_V010000",""                           ,"Work E5 "                        },

//{  50,     0,0x0000,0x0000,""             ,3,' ',"LogicWT2"              ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },

  { 100, 80042,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"BGR_Trim HT 95'C      ", 2830,    0,    0, 100000, 20,1,0,""        ,         0,         0,      0,"bgr3_trim_v001"             ,""                           ,""                           ,""                                },
  {   0, 80044,0x0000,0x0000,"FL_STD"       ,3,'L',"ReProgram Product Trim", 3300,  500,  500, 500000,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_WRK029_MT_V010000","RC04EX_IO_WT1_P1000_V010000",""                           ,"Logic E1 E5 ming Data for WT"    },
  { 100, 80104,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },

  { 100, 80102,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"OCO monitor           ", 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"oco_monitor_io_v001"        ,""                           ,""                           ,""                                },

  {  50, 20700,0x0000,0x0000,"FL_WRK"       ,3,'w',"Program WT2_Rev & Year", 3300,  500,  500,  31250,  1,1,1,""        ,         0,         0,      0,"RC04EX_IO_WRK010_MT_V010000","RC04EX_IO_WT2_P2070_V010000",""                           ,"Work E5 /Month/Date code"        },
  {  70, 46800,0x0000,0x0000,"FL_STD"       ,3,'w',"Program WT2 Passcode"  , 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK012_MT_V010000","RC04EX_IO_WT2_P4680_V010000",""                           ,"Work E5 "                        },

//{  50,     0,0x0000,0x0000,""             ,3,' ',"LogicWT4"              ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },

  {  70, 22500,0x0000,0x0000,"FL_WRK"       ,3,'w',"Program WT4_Rev & Year", 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK010_MT_V010000","RC04EX_IO_WT4_P2250_V010000",""                           ,"Work E5 /Month/Date code"        },
  {  70, 29000,0x0000,0x0000,"FL_STD"       ,3,'e',"Erase with PreWrite"   , 3300,  500,  500,  31250, 38,1,1,""        ,         0,         0,      0,"RC04EX_IO_ERS002_MT_V010000","RC04EX_IO_WT4_P2900_V010000",""                           ,"Erase UI E2 E3 E4 E6 "           },
  {  70, 29300,0x0000,0x0000,"FL_TRM"       ,3,'v',"VthRead All1"          , 3300,  500,  500, 100000,  5,1,1,"Vssmon"  ,         0,         0,   -770,"RC04EX_IO_CRD012_MT_V010000","RC04EX_IO_WT4_P2930_V010000",""                           ,"Erase UI "                       },

  {  70, 49000,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"Program Extra2 Data"   , 3300,  500,  500,  31250,  0,1,1,""        ,         0,         0,      0,"program_info1_v001"         ,""                           ,""                           ,"Logic E2 "                       },
  {  70, 49060,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"Program Extra6 Boot_Fi", 3300,    0,    0,  31250,  0,1,1,""        ,         0,         0,      0,"program_boot_v001"          ,""                           ,""                           ,"Logic E6 rm"                     },
  {  70, 49100,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra1 Flash_T", 3300,  500,  500, 100000,  5,1,0,"Vssmon"  ,         0,         0,    830,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4910_V010000",""                           ,"Configuration E1 rimming for Vth"},
  {  70, 49120,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra1 Flash_T", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,   -570,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4912_V010000",""                           ,"Configuration E1 rimming for Vth"},
  {  70, 49140,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra2 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,    830,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4914_V010000",""                           ,"Configuration E2 r VthH"         },
  {  70, 49160,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra2 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,   -570,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4916_V010000",""                           ,"Configuration E2 r VthL"         },
  {  70, 49180,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra3 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,    830,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4918_V010000",""                           ,"Configuration E3 r VthH"         },
  {  70, 49200,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra3 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,   -570,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4920_V010000",""                           ,"Configuration E3 r VthL"         },
  {  70, 49220,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra4 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,    830,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4922_V010000",""                           ,"Configuration E4 r VthH"         },
  {  70, 49240,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra4 Data fo", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,   -570,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4924_V010000",""                           ,"Configuration E4 r VthL"         },
  {  70, 49260,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra6 Boot_Fi", 3300,  500,  500, 100000,  5,0,0,"Vssmon"  ,         0,         0,    830,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4926_V010000",""                           ,"Configuration E6 rm for VthH"    },
  {  70, 49280,0x0000,0x0000,"FL_TRM"       ,3,'y',"VthRead Extra6 Boot_Fi", 3300,  500,  500, 100000,  5,0,1,"Vssmon"  ,         0,         0,   -570,"RC04EX_IO_VRD022_MT_V010000","RC04EX_IO_WT4_P4928_V010000",""                           ,"Configuration E6 rm for VthL"    },
  {  70, 49300,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"CPU Read Extra2 Data"  , 3300,  500,  500,  27620,  1,1,0,""        ,         0,         0,      0,"read_info1_v001"            ,""                           ,""                           ,"Logic E2 "                       },
  {  70, 49320,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"CPU Read Extra3 Data"  , 3300,  500,  500,  27620,  1,0,0,""        ,         0,         0,      0,"read_info2_v001"            ,""                           ,""                           ,"Logic E3 "                       },
  {  70, 49340,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"CPU Read Extra4 Data"  , 3300,  500,  500,  27620,  1,0,0,""        ,         0,         0,      0,"read_info3_v001"            ,""                           ,""                           ,"Logic E4 "                       },
  {  70, 49360,0x0000,0x0000,"FL_LOGICTRM"  ,3,' ',"CPU Read Extra6 Boot_F", 3300,  500,  500,  27620,  1,0,1,""        ,         0,         0,      0,"read_info4_v001"            ,""                           ,""                           ,"Logic E6 irm"                    },
//{  70,     0,0x0000,0x0000,""             ,1,' ',"Logic3"                ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },
  {  70, 46300,0x0000,0x0000,"FL_STD"       ,3,'w',"ReProgram Reset Transf", 3300,  500,  500,  31250,  1,1,1,""        ,         0,         0,      0,"RC04EX_IO_WRK019_MT_V010000","RC04EX_IO_WT4_P4630_V010000",""                           ,"Work E1 er Data before Shipment" },
  {  70, 46400,0x0000,0x0000,"FL_TRM"       ,3,'w',"CPU Read Work Data / V", 3300,  500,  500, 100000,  5,1,0,"Vssmon"  ,         0,         0,   1030,"RC04EX_IO_WRK017_MT_V010000","RC04EX_IO_WT4_P4640_V010000",""                           ,"Work E1 E5 thRead Trimming Data "},
  {  70, 46500,0x0000,0x0000,"FL_TRM"       ,3,'w',"CPU Read Work Data / V", 3300,  500,  500, 100000,  5,0,1,"Vssmon"  ,         0,         0,  -1170,"RC04EX_IO_WRK017_MT_V010000","RC04EX_IO_WT4_P4650_V010000",""                           ,"Work E1 E5 thRead Trimming Data "},
  {  80, 46600,0x0000,0x0000,"FL_INT"       ,3,'w',"Transmit Result"       , 3300,  500,  500,  31250,  1,1,1,""        ,         0,         0,      0,"RC04EX_IO_WRK000_MT_V010000","RC04EX_IO_WT4_P4660_V010000",""                           ,"Work E5 "                        },
//{  90,     0,0x0000,0x0000,""             ,1,' ',"Logic4"                ,    0,    0,    0,      0,  0,1,1,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Logic "                          },
  {  90, 46800,0x0000,0x0000,"FL_STD"       ,3,'w',"Program WT4 Passcode"  , 3300,  500,  500,  31250,  1,1,0,""        ,         0,         0,      0,"RC04EX_IO_WRK012_MT_V010000","RC04EX_IO_WT4_P4680_V010000",""                           ,"Work E5 "                        },
  {  90, 47000,0x0000,0x0000,"FL_STD"       ,3,'w',"Check WT1/WT8/WT2/WT4 ", 3300,  500,  500,  31250,  1,0,1,""        ,         0,         0,      0,"RC04EX_IO_CRD004_MT_V010000","RC04EX_IO_WT4_P4700_V010000",""                           ,"Work E5 Passcode"                },


  { 150,  8001,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Info1   Extra 2"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_info1_v001"       ,""                           ,""                        ,""                                   },
  { 150,  8002,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Info2   Extra 3"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_info2_v001"       ,""                           ,""                        ,""                                   },
  { 150,  8003,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Info3   Extra 4"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_info3_v001"       ,""                           ,""                        ,""                                   },
  { 150,  8004,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Info4   Extra 6"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_info4_v001"       ,""                           ,""                        ,""                                   },
  { 150,  8005,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Table1  Extra 1"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_table1_v001"      ,""                           ,""                        ,""                                   },
  { 150,  8006,0x0000,0x0000,"FL_LOGICTRM"  ,3,'L',"Read Table2  Extra 5"  , 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"read_dump_table2_v001"      ,""                           ,""                        ,""                                   },


  { 100, 80104,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"BGR monitor           ", 3300,    0,    0, 100000,  1,1,1,"VCL"     ,         0,         0,      0,"bgr_monitor_v001"           ,""                           ,""                           ,""                                },
  { 100, 80102,0x0000,0x0000,"FL_LOGICTRM"  ,3,'D',"OCO monitor           ", 3300,    0,    0, 100000,  1,1,1,""        ,         0,         0,      0,"oco_monitor_io_v001"        ,""                           ,""                           ,""                                },



  {   0, 99990,0x0000,0x0000,""             ,1,'/',"TEST PASS"             ,    0,    0,    0,      0,  0,0,0,""        ,         0,         0,      0,""                           ,""                           ,""                           ,"Final judge "                    },



};

