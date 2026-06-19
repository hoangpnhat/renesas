/******************************************************************************/
/*
  FILENAME    RX200def.c
  CONTENTS: This is a 90nm(RC03F) FLASH module define program for the
            V3300,4000,5000 test system
  COPYRIGHT 2007 Renesas Technology ALL RIGHTS RESERVED
  2011/04/25  rev.00  1'st created by H.Kobori
*/
/******************************************************************************/

#include "RC04EX_def.h"

  char ivsa_string[16][5] = {
       { "None" },
       { "1uA" },
       { "3uA" },
       { "5uA" },
       { "8uA" },
       { "10uA" },
       { "13uA" },
       { "15uA" },
       { "18uA" },
       { "20uA" },
       { "23uA" },
       { "30uA" },
       { "40uA" },
       { "50uA" },
       { "70uA" },
       { "Half" },
     };
     
  char mat_string[9][8] = {
       { "FLP0" },      /* FLP0   */
       { "FLI1" },      /* FLI1   */
       { "FLI2" },      /* FLI2   */
       { "EXTRA1" },    /* EXTRA1 */
       { "EXTRA2" },    /* EXTRA2 */
       { "EXTRA3" },    /* EXTRA3 */
       { "EXTRA4" },    /* EXTRA4 */
       { "EXTRA5" },    /* EXTRA5 */
       { "EXTRA6" }     /* EXTRA6 */
     };

  char mat_module[9] = {
       CODE_FLASH,  /* FLP0 */
       DATA_FLASH,  /* FLI1 */
       DATA_FLASH,  /* FLI2 */
       CODE_FLASH,  /* EXTRA1 */
       CODE_FLASH,  /* EXTRA2 */
       CODE_FLASH,  /* EXTRA3 */
       CODE_FLASH,  /* EXTRA4 */
       CODE_FLASH,  /* EXTRA5 */
       CODE_FLASH   /* EXTRA6 */
     };

  char info_extra[7][14] = {
       { "User" },           /* Info0 */
       { "Extra2" },         /* Info1 */
       { "Extra3" },         /* Info2 */
       { "Extra4" },         /* Info3 */
       { "Extra6_Boot" },    /* Info4 boot */
       { "Extra1_Table1" },  /* Info5 table1 */
       { "Extra5_work" }     /* Info6 table2 work */
     };


  const char str_ivsa[16][5] = {
    {"None"},{"1uA"},{"3uA"},{"5uA"},{"8uA"},{"10uA"},{"13uA"},{"15uA"},{"18uA"},{"20uA"},{"23uA"},{"30uA"},{"40uA"},{"50uA"},{"70uA"},{"Half"}
  };
  const char str_area[3][5] = {
    { "DATA" },{ "ECC" },{ "LK" }  /* caution!! none ECC ?? */
  };
  const char str_posinega[3][7] = {
    { "Normal" },{ "Posi" },{ "Nega" }
  };
  const char str_relief[3][8] = {
    { "DUMMY" },{ "KEEP" },{ "RELEASE" }
  };

  char readpatname[3][16] = {
       { "distribution_at" },
       { "readallfbm_at" },
       { "vreadallfbm_at" }
     };
  char readparamname[3][16] = {
       { "WT_DISTRIBUTION" },
       { "WT_READALLFBM" },
       { "WT_VREADALLFBM" }
     };

  int mat_size[9] = {
        FLP0_SIZE,   /* FLP0   */
        FLI1_SIZE,   /* FLI1   */
        FLI2_SIZE,   /* FLI2   */
        EXTRAS_SIZE, /* EXTRA1 */
        EXTRAS_SIZE, /* EXTRA2 */
	EXTRAS_SIZE, /* EXTRA3 */
	EXTRAS_SIZE, /* EXTRA4 */
	EXTRAS_SIZE, /* EXTRA5 */
	EXTRAB_SIZE  /* EXTRA6 */
      };

  const char vthpoint_string[11][8] = {
    { "100%" },
    { "99.999%" },
    { "99.99%" },
    { "99.9%" },
    { "75%" },
    { "50%" },
    { "25%" },
    { "0.1%" },
    { "0.01%" },
    { "0.001%" },
    { "0%" }
  };

  char ws1lot_string[WS1LOT_NUM][6] = {
       { "EPG802" },
       { "EPA834" },
       { "FPD839" },
       { "FPD840" }
     };
