/******************************************************************************/
/*!
  @file RC04EX_slc_ReadShmooSelect.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the V3300,4000,5000 test system
  @author S.Yamaoka
  @date 2010.07.09 Rev.00
*/
/******************************************************************************/


#if ( PRODUCT_NAME==PRODUCT_EUROPA )

int ReadShmooSelect( int vcc[NUM_POWERSUPPLY], int waittime, interface_t* param ){

  int extal_pllx8_table[]={
   100000  ,    //  0 Dummy
  5000000  ,    //  1 if PLLx5  ICLK=  1MHz
  1000000  ,    //  2 if PLLx5  ICLK=  5MHz
   500000  ,    //  3 if PLLx5  ICLK= 10MHz
   250000  ,    //  4 if PLLx5  ICLK= 20MHz
   227272  ,    //  5 if PLLx5  ICLK= 22MHz
   208333  ,    //  6 if PLLx5  ICLK= 24MHz
   192307  ,    //  7 if PLLx5  ICLK= 26MHz
   178571  ,    //  8 if PLLx5  ICLK= 28MHz
   166666  ,    //  9 if PLLx5  ICLK= 30MHz
   156250  ,    // 10 if PLLx5  ICLK= 32MHz
   147058  ,    // 11 if PLLx5  ICLK= 34MHz
   138888  ,    // 12 if PLLx5  ICLK= 36MHz
   131578  ,    // 13 if PLLx5  ICLK= 38MHz
   125000  ,    // 14 if PLLx5  ICLK= 40MHz
   119047  ,    // 15 if PLLx5  ICLK= 42MHz
   113636  ,    // 16 if PLLx5  ICLK= 44MHz
   108695  ,    // 17 if PLLx5  ICLK= 46MHz
   104166  ,    // 18 if PLLx5  ICLK= 48MHz
   100000  ,    // 19 if PLLx5  ICLK= 50MHz
    96153  ,    // 20 if PLLx5  ICLK= 52MHz
    92592  ,    // 21 if PLLx5  ICLK= 54MHz
    89285  ,    // 22 if PLLx5  ICLK= 56MHz
    86206  ,    // 23 if PLLx5  ICLK= 58MHz
    83333  ,    // 24 if PLLx5  ICLK= 60MHz
    80645  ,    // 25 if PLLx5  ICLK= 62MHz
    78125  ,    // 26 if PLLx5  ICLK= 64MHz
    75757  ,    // 27 if PLLx5  ICLK= 66MHz
    73529  ,    // 28 if PLLx5  ICLK= 68MHz
    71428  ,    // 29 if PLLx5  ICLK= 70MHz
    69444  ,    // 30 if PLLx5  ICLK= 72MHz
    67567  ,    // 31 if PLLx5  ICLK= 74MHz
    65789  ,    // 32 if PLLx5  ICLK= 76MHz
    64102  ,    // 33 if PLLx5  ICLK= 78MHz
    62500  ,    // 34 if PLLx5  ICLK= 80MHz
    60975  ,    // 35 if PLLx5  ICLK= 82MHz
    59523  ,    // 36 if PLLx5  ICLK= 84MHz
    58139  ,    // 37 if PLLx5  ICLK= 86MHz
    56818  ,    // 38 if PLLx5  ICLK= 88MHz
    55555  ,    // 39 if PLLx5  ICLK= 90MHz
    54347  ,    // 40 if PLLx5  ICLK= 92MHz
    53191  ,    // 41 if PLLx5  ICLK= 94MHz
    52083  ,    // 42 if PLLx5  ICLK= 96MHz
    51020  ,    // 43 if PLLx5  ICLK= 98MHz
    50000  ,    // 44 if PLLx5  ICLK=100MHz
    49019  ,    // 45 if PLLx5  ICLK=102MHz
    48076  ,    // 46 if PLLx5  ICLK=104MHz
    47169  ,    // 47 if PLLx5  ICLK=106MHz
    46296  ,    // 48 if PLLx5  ICLK=108MHz
    45454  ,    // 49 if PLLx5  ICLK=110MHz
    44642  ,    // 50 if PLLx5  ICLK=112MHz
    43859  ,    // 51 if PLLx5  ICLK=114MHz
    43103  ,    // 52 if PLLx5  ICLK=116MHz
    42372  ,    // 53 if PLLx5  ICLK=118MHz
    41666  ,    // 54 if PLLx5  ICLK=120MHz
    40983  ,    // 55 if PLLx5  ICLK=122MHz
    40322  ,    // 56 if PLLx5  ICLK=124MHz
    39682  ,    // 57 if PLLx5  ICLK=126MHz
    39062  ,    // 58 if PLLx5  ICLK=128MHz
    38461  ,    // 59 if PLLx5  ICLK=130MHz
    37878  ,    // 60 if PLLx5  ICLK=132MHz
    37313  ,    // 61 if PLLx5  ICLK=134MHz
    36764  ,    // 62 if PLLx5  ICLK=136MHz
    36231  ,    // 63 if PLLx5  ICLK=138MHz
    35714  ,    // 64 if PLLx5  ICLK=140MHz
    35211  ,    // 65 if PLLx5  ICLK=142MHz
    34722  ,    // 66 if PLLx5  ICLK=144MHz
    34246  ,    // 67 if PLLx5  ICLK=146MHz
    33783  ,    // 68 if PLLx5  ICLK=148MHz
    33333  ,    // 69 if PLLx5  ICLK=150MHz
    32894  ,    // 70 if PLLx5  ICLK=152MHz
    32467  ,    // 71 if PLLx5  ICLK=154MHz
    32051  ,    // 72 if PLLx5  ICLK=156MHz
    31645  ,    // 73 if PLLx5  ICLK=158MHz
    31250  ,    // 74 if PLLx5  ICLK=160MHz
   100000  ,    // 75 if PLL OFF  ICLK= 10MHz
    50000  ,    // 76 if PLL OFF  ICLK= 20MHz
    40000  ,    // 77 if PLL OFF  ICLK= 25MHz
    33333  ,    // 78 if PLL OFF  ICLK= 30MHz
    28571  ,    // 79 if PLL OFF  ICLK= 35MHz
    25000  ,    // 80 if PLL OFF  ICLK= 40MHz
    22222  ,    // 81 if PLL OFF  ICLK= 45MHz
    20000  ,    // 82 if PLL OFF  ICLK= 50MHz
  };

  int vdd_500_table[]={
    0, 600, 575, 550, 525, 500, 475, 450, 425, 400,
  };

  interface_t param_back[TABLE_END+1];
  int vs[4];                            /* local vs[n] variable      */
  int ctr_exp, ctr_typ, ctr_mode, ctr_trans;
  int iclk_div, bclk_div;
  int ckscr, pllccr_div, pllccr_clk, pllccr_multiple, pllccr_range, flwt, hocomcr;
  int *extal_table, *vdd_table, extal_tbl_max, vdd_tbl_max;
  int y, y_loop_min, y_loop_max, x, x_loop_min, x_loop_max, x_loop_start;
  int x_step, y_step;
  int extal_ps;
  int tempo, tempo1, tempo_pass;
  char use_pat_name[64+4];
  char use_test_name[64+4];
  char sample_name[32+4];
  int test_temperature;
  int exvref_flag, stop_flag, skip_flag, break_flag, repeat_flag;
  time_t  stime_list, etime_list;
  char  stime_buf[32+4], etime_buf[32+4];
  char xy_passfail[100+4][100+4];
  int x_label[100+4], y_label[100+4];
  int fail_contents[100+4], fail_base[100+4], fail_point[100+4], fail_data[100+4][8+0];
  int in_ldata[16+2];
  int rc, time_out;
  int dbg_mosccr, dbg_pwstf, dbg_pllccr, dbg_flwt, dbg_smder, dbg_sckscr, dbg_cnt;
  int vdd_const_flag, vddh_const_flag;


  v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*TABLE_END );    /* input param store */


  vs[0] = vcc[0]; vs[1] = vcc[1]; vs[2] = vcc[2];
  in_ldata[4] = 0x00000000; /* UIArea */
  in_ldata[5] = 0x00000000; /* Top Address */
  in_ldata[6] = 0x00180000; /* End Address */
  in_ldata[7] = 0x40050202; /* bit0-7   expect 0=All0 1=All1 2=ChkA 3=ChkB */
                            /* bit8-15  bit8=1=Normal bit9=1=X-PingPong bit10=1=X-Complemental */
                            /* bit16-23 5=BOOST_PWST 4=NORMAL_PWST */
                            /* bit24-31 bit31=1=FACI Trans  bit30=1=GREG Info1 set  bit29=1=GREG Table set   bit28=1=GREG init */
  in_ldata[8] = 0x00000000; /* SCKDIVCR bit10-8=BCLK div  bit26-24=ICLK div  0=/1  1=/2  2=/4  3=/8  4=/16  5=/32  6=/64 */
  in_ldata[9] = 0x00000005; /* CKSCR 0=HOCO 1=MOCO 2=LOCO 3=MAIN 4=SubOSC 5=PLL */
  in_ldata[10]= 0x00004400; /* PLLCCR bit0-1 0=/1 1=/2 2=/3 3=/4  bit4=0=main bit4=1=HOCO  b10-b8+1=xPll bit14=0=32-48MHz bit14=1=32-64MHz */
  in_ldata[11]= 0x000411AA; /* EXTAL ps */
  in_ldata[12]= 0x00000000; /* FLWT 0=0wait  1=1wait */
  in_ldata[13]= 0x00000000; /* HOCOMCR 0=24MHz 1=32MHz 2=48MHz 3=64MHz */

  ckscr = pllccr_div = pllccr_clk = pllccr_multiple = pllccr_range = flwt = hocomcr = 0;
  ctr_exp = in_ldata[7]&0x00FF; ctr_typ = ( in_ldata[7]&0x00FF00 )>>8; ctr_mode = ( in_ldata[7]&0x00FF0000 )>>16; ctr_trans = ( in_ldata[7]&0xFF000000 )>>24;
  iclk_div = ( in_ldata[8]&0x07000000 )>>24; bclk_div =  ( in_ldata[8]&0x00000700 )>>8; ckscr = in_ldata[9]; 
  pllccr_div = in_ldata[10]&0x03; pllccr_clk = ( in_ldata[10]&0x08 )>>4; pllccr_multiple = ( in_ldata[10]&0x0700 )>>8; pllccr_range = ( in_ldata[10]&0x4000 )>>14;
  flwt = in_ldata[12]&0x07; hocomcr = in_ldata[13]&0x03;

  extal_table = extal_pllx8_table; extal_tbl_max = sizeof( extal_pllx8_table ) / sizeof( int );
  vdd_table   = vdd_500_table; vdd_tbl_max = sizeof( vdd_500_table ) / sizeof( int );
  x_step = 1; y_step = 1;
  y_loop_min = 1; y_loop_max = 9;    /* vdd  */
  x_loop_min = 9; x_loop_max = 44;   /* freq */
  if( CheckFK(7) ){ /* debug */
    y_loop_min = 4; y_loop_max = 6;    /* vdd  */
    x_loop_min = 9; x_loop_max = 74;   /* freq */
  }

  strcpy( use_pat_name, "read_fmax_v001" );
  strcpy( use_test_name, "Read Fmax Test" );
  sprintf( sample_name, "X:%dY:%d", xy_location[0], xy_location[1] );
  test_temperature = 25;
  exvref_flag = 1; stop_flag = 1; skip_flag = 0; break_flag = 0; repeat_flag = 0;
  dbg_mosccr = dbg_pwstf = dbg_pllccr = dbg_flwt = dbg_smder = dbg_sckscr = dbg_cnt = 0;
  vdd_const_flag = vddh_const_flag = -1;

  do{
    tempo = 1;
    while( tempo ){
      printf( "\n title   (change no)");
      printf( "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) printf( "All0" ); else if( ctr_exp==1 ) printf( "All1" ); else if( ctr_exp==2 ) printf( "CheckerA" ); else if( ctr_exp==3 ) printf( "CheckerB" ); else printf( "None" );
      printf( "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) printf( "Normal " ); else if( ctr_typ&0x02 ) printf( "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) printf( "Complemental X X+1 " ); else printf( "None" );
      printf( "\n Clock Source   (30) | " ); if( ckscr==0 ) printf( "HOCO" ); else if( ckscr==1 ) printf( "MOCO" ); else if( ckscr==2 ) printf( "LOCO" ); else if( ckscr==3 ) printf( "MAIN" ); else if( ckscr==4 ) printf( "SubOSC" ); else if( ckscr==5 ) printf( "PLL" ); else printf( "None" );
      printf( "\n PLLCCR         (40) | H'%04X PLL /%d x%d  FSEL0=%d ", in_ldata[10], pllccr_div + 1, pllccr_multiple + 1, pllccr_range ); if( pllccr_clk==0 ) printf( "MAIN" ); else if( pllccr_clk==1 ) printf( "HOCO" ); else;
                                                          if( pllccr_range==0 ) printf( "32-48MHz" ); else printf( "48-64MHz" );
      printf( "\n HOCOMCR        (50) | H'%02X ", hocomcr ); if( hocomcr==0 ) printf( "24MHz" ); else if( hocomcr==1 ) printf( "32MHz" ); else if( hocomcr==2 ) printf( "48MHz" ); else if( hocomcr==3 ) printf( "64MHz" ); else printf( "None" );
      printf( "\n FLWT           (60) | wait %d cyc", flwt );
      printf( "\n GREG           (70) | " ); if( ctr_trans&0x80 ) printf( "[FACI Trans Exe]" ); if( ctr_trans&0x40 ) printf( "[Info1 data set]" ); if( ctr_trans&0x20 ) printf( "[Table2 data set]" ); if( ctr_trans&0x10 ) printf( "[Init data set]" );
      printf( "\n Pattern        (80) | %s", use_pat_name );
      printf( "\n Sample Name    (1)  | %s", sample_name );
      printf( "\n Temperature    (2)  | %d'C", test_temperature );
      printf( "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] );
      printf( "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) printf( "ON ]" ); else printf( "OFF ]" );  printf( " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] ); printf( " Xstep[%d-%d] Extal[%d-%d]ns", x_loop_min, x_loop_max, extal_table[ x_loop_min ]/1000, extal_table[ x_loop_max ]/1000);
      printf( "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); if( -1==vdd_const_flag ) printf( " Vdd=%dmV ", vdd_const_flag ); if( -1==vddh_const_flag ) printf( " Vddh=%dmV ", vddh_const_flag );
      printf( "\n Option         (6)  | FailStop[ ", vs[0] ); if( stop_flag ) printf( "ON ]" ); else printf( "OFF ]" ); printf( "  ShortTime[ " ); if( skip_flag ) printf( "ON ]" ); else printf( "OFF ]" ); printf( "  HitKeyBreak[ ON ]  RepeatMode[ " ); if( repeat_flag ) printf( "ON ]" ); else printf( "OFF ]" ); 
      printf( "\n Break (99)  Test Start(0)\n   input no ==> " );
      DigitInput( &tempo );
      if( tempo==99 ) return( FAIL );
      if( tempo==1 ){ printf( "sample name[max 30 char] => " ); StringInput( sample_name ); }
      if( tempo==2 ){ printf( "temperature => " ); DigitInput( &test_temperature ); }
      if( tempo==3 ){ 
        printf( "\n                AreaSelect  Top Addr   End Addr+1" );
        printf( "\n UserArea         ( 0 )     0x00000000-0x00180000" );
        printf( "\n Extra1=Table1    ( 5 )     0x01010000-0x01010400" );
        printf( "\n Extra2=Info1     ( 1 )     0x01008000-0x01008400" );
        printf( "\n Extra3=Info2     ( 2 )     0x0100A000-0x0100A400" );
        printf( "\n Extra4=Info3     ( 3 )     0x0100C000-0x0100C400" );
        printf( "\n Extra5=Table2    ( 6 )     0x01012000-0x01012400" );
        printf( "\n Extra6=Info4     ( 4 )     0x01010000-0x01003000 or 0x01004000" );
        printf( "Area 0-6  => " ); DigitInput( &in_ldata[4] );
        printf( "Top Addr  => " ); DigitInput( &in_ldata[5] );
        printf( "End Addr  => " ); DigitInput( &in_ldata[6] );
      }
      if( tempo==4 ){
        tempo1 = exvref_flag;
        printf( "Exvref Off=0  On=1  => " ); DigitInput( &exvref_flag );
        if( exvref_flag!=tempo1 ){
          if( 0==exvref_flag ){ y_loop_min = 0; y_loop_max = 0;
          }else{                y_loop_min = 1; y_loop_max = 9; }
        }
        if( exvref_flag ){
          printf( " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] );
          printf( "\n Ystart => " ); DigitInput( &y_loop_min ); printf( "Yend max %d => ", vdd_tbl_max ); DigitInput( &y_loop_max );
        }
        printf( " Xstep[%d-%d] Extal[%d-%d]ps  PLLx8=1-74  PLLOFF=75-82", x_loop_min, x_loop_max, extal_table[ x_loop_min ], extal_table[ x_loop_max ] );
        printf( "\n Xstart => " ); DigitInput( &x_loop_min ); printf( "Xend max %d => ", extal_tbl_max ); DigitInput( &x_loop_max );
        if( y_loop_min<0 ) y_loop_min = 0; if( y_loop_max>vdd_tbl_max ) y_loop_max = vdd_tbl_max;
        if( x_loop_min<0 ) x_loop_min = 0; if( x_loop_max>extal_tbl_max ) x_loop_max = extal_tbl_max;
      }
      if( tempo==5 ){
        printf( "Vcc  => " ); DigitInput( &vs[0] );
        printf( "Const Vdd  No=-1 Yes=VddmV  => " ); DigitInput( &vdd_const_flag );
        printf( "Const Vddh No=-1 Yes=VddmV  => " ); DigitInput( &vddh_const_flag );
        if( -1!=vdd_const_flag && -1!=vddh_const_flag ){
          printf( "?? No Vddh/Vdd Shmoo yes=1 no=other => " ); DigitInput( &tempo1 ); if( 1!=tempo1 ) vdd_const_flag = vddh_const_flag = 0;
        }
      }
      if( tempo==6 ){
        printf( "Fail Stop  Off=0  On=1  => " ); DigitInput( &stop_flag );
        printf( "Short Time Off=0  On=1  => " ); DigitInput( &skip_flag );
        printf( "Repeat Shmoo Menu  Off=0  On=1  => " ); DigitInput( &repeat_flag );
      }

      if( tempo==10 ){ printf( "0=All0  1=All1  2=ChkA  3=ChkB  => " ); DigitInput( &ctr_exp ); }
      if( tempo==20 ){ printf( "bit0=1=Normal  bit1=1=PingPong  bit2=1=Complemental   => " ); DigitInput( &ctr_typ ); }
      if( tempo==30 ){ printf( "0=HOCO  1=MOCO  2=LOCO  3=MAIN  4=SubOSC  5=PLL   => " ); DigitInput( &ckscr ); }
      if( tempo==40 ){ printf( "bit0-1=PLLDIV  bit4=InputCLK  bit8-10=magnification  bit14=FSEL0   0xYYYY => " ); DigitInput( &tempo1 ); pllccr_div = tempo1&0x03; pllccr_clk = ( tempo1&0x08 )>>4; pllccr_multiple = ( tempo1&0x0700 )>>8; pllccr_range = ( tempo1&0x4000 )>>14; in_ldata[10] = tempo1; }
      if( tempo==50 ){ printf( "0=24MHz  1=32MHz  2=48MHz  3=64MHz  => " ); DigitInput( &hocomcr ); }
      if( tempo==60 ){ printf( "0=0 wait  1=1 wait  => " ); DigitInput( &flwt ); }
      if( tempo==70 ){ printf( "bit7=1=FACI Trans Exe  bit6=1=Info1 data set  bit5=Table2 data set  bit4=Init data set  0xYY => " ); DigitInput( &ctr_trans ); }
      if( tempo==80 ){ printf( "0=read_fmax_v001  1=read_fmax_a_v001  2=read_fmax_b_v001  3=read_fmax_c_v001  4=read_fmax_d_v001 => " ); DigitInput( &dbg_cnt );
                       if     ( dbg_cnt==1 ) strcpy( use_pat_name, "read_fmax_a_v001" );
                       else if( dbg_cnt==2 ) strcpy( use_pat_name, "read_fmax_b_v001" );
                       else if( dbg_cnt==3 ) strcpy( use_pat_name, "read_fmax_c_v001" );
                       else if( dbg_cnt==4 ) strcpy( use_pat_name, "read_fmax_d_v001" );
                       else                  strcpy( use_pat_name, "read_fmax_v001" );
                     }
    }

    in_ldata[7] = ((ctr_trans<<24)&0xFF000000) | ((ctr_mode<<16)&0x00FF0000) | ((ctr_typ<<8)&0x00FF00) | ( ctr_exp&0x00FF );
    in_ldata[8] = ((iclk_div<<24)&0x07000000) | ((bclk_div<<8)&0x00000700);
    in_ldata[9] = ckscr;
    in_ldata[10] = ((pllccr_range<<14)&0x4000) | ((pllccr_multiple<<8)&0x0700) | ((pllccr_clk<<4)&0x08) | ( pllccr_div&0x03 );
    in_ldata[12] = flwt;
    in_ldata[13] = hocomcr;
    if( ckscr==0 || ckscr==1 || ckscr==2 || ckscr==4 ){
      x_loop_min = 0; x_loop_max = 0;
    }


    /*-----------------------*/
    /* ログファイル設定      */
    /*-----------------------*/
    strcpy( baselog, glob_cdp );
    sprintf( filenamebuf, "\\DATALOG\\DATA.LOG_Fmax_%d_%d.csv", vs[0], get_site_number() );
    time( &stime_list );
    strcat( baselog, filenamebuf );
    if( ( Fptdata = fopen( baselog, "at" ) )==NULL ){ printf( " -- FAIL -- Not Open file:%s !\n", baselog ); return( FAIL ); }

    if( CheckFK(7) ){ /* debug */
      DataOut2( 1, Fptdata, "\n" ); for( tempo=4; tempo<14; tempo++ ){ sprintf( outbuf, " in_ldata[%d] = %08X", tempo, in_ldata[tempo] ); DataOut2( 1, Fptdata, outbuf ); }
      DataOut2( 1, Fptdata, "\n" ); sprintf( outbuf, " ckscr = %08X, pllccr_div = %08X, pllccr_clk = %08X, pllccr_multiple = %08X, pllccr_range = %08X, flwt = %08X, hocomcr = %08X", ckscr, pllccr_div, pllccr_clk, pllccr_multiple, pllccr_range, flwt, hocomcr ); DataOut2( 1, Fptdata, outbuf );
    }

    DataOut2( 1, Fptdata, "\n-------------------------------------------\n" );
    sprintf( outbuf, "%s\n", sample_name ); DataOut2( 1, Fptdata, outbuf );
    DataOut2( 1, Fptdata, "-------------------------------------------\n" );
    sprintf( outbuf, "Ta = %d 'C\n", test_temperature ); DataOut2( 1, Fptdata, outbuf );

    DataOut2( 1, Fptdata, "\n title   (change no)");
    DataOut2( 1, Fptdata, "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) DataOut2( 1, Fptdata, "All0" ); else if( ctr_exp==1 ) DataOut2( 1, Fptdata, "All1" ); else if( ctr_exp==2 ) DataOut2( 1, Fptdata, "CheckerA" ); else if( ctr_exp==3 ) DataOut2( 1, Fptdata, "CheckerB" ); else DataOut2( 1, Fptdata, "None" );
    DataOut2( 1, Fptdata, "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) DataOut2( 1, Fptdata, "Normal " ); else if( ctr_typ&0x02 ) DataOut2( 1, Fptdata, "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) DataOut2( 1, Fptdata, "Complemental X X+1 " ); else DataOut2( 1, Fptdata, "None" );
    DataOut2( 1, Fptdata, "\n Clock Source   (30) | " ); if( ckscr==0 ) DataOut2( 1, Fptdata, "HOCO" ); else if( ckscr==1 ) printf( "MOCO" ); else if( ckscr==2 ) DataOut2( 1, Fptdata, "LOCO" ); else if( ckscr==3 ) DataOut2( 1, Fptdata, "MAIN" ); else if( ckscr==4 ) DataOut2( 1, Fptdata, "SubOSC" ); else if( ckscr==5 ) DataOut2( 1, Fptdata, "PLL" ); else DataOut2( 1, Fptdata, "None" );
    sprintf( outbuf, "\n PLLCCR         (40) | H'%04X PLL /%d x%d  FSEL0=%d ", in_ldata[10], pllccr_div + 1, pllccr_multiple + 1, pllccr_range ); DataOut2( 1, Fptdata, outbuf ); if( pllccr_clk==0 ) DataOut2( 1, Fptdata, "MAIN" ); else if( pllccr_clk==1 ) DataOut2( 1, Fptdata, "HOCO" ); else;
    if( pllccr_range==0 ) DataOut2( 1, Fptdata, "32-48MHz" ); else DataOut2( 1, Fptdata, "48-64MHz" );
    sprintf( outbuf, "\n HOCOMCR        (50) | H'%02X ", hocomcr ); DataOut2( 1, Fptdata, outbuf ); if( hocomcr==0 ) DataOut2( 1, Fptdata, "24MHz" ); else if( hocomcr==1 ) DataOut2( 1, Fptdata, "32MHz" ); else if( hocomcr==2 ) DataOut2( 1, Fptdata, "48MHz" ); else if( hocomcr==3 ) DataOut2( 1, Fptdata, "64MHz" ); else DataOut2( 1, Fptdata, "None" );
    sprintf( outbuf, "\n FLWT           (60) | wait %d cyc", flwt ); DataOut2( 1, Fptdata, outbuf );
    DataOut2( 1, Fptdata, "\n GREG           (70) | " ); if( ctr_trans&0x80 ) DataOut2( 1, Fptdata, "[FACI Trans Exe]" ); if( ctr_trans&0x40 ) DataOut2( 1, Fptdata, "[Info1 data set]" ); if( ctr_trans&0x20 ) DataOut2( 1, Fptdata, "[Table2 data set]" ); if( ctr_trans&0x10 ) DataOut2( 1, Fptdata, "[Init data set]" );
    sprintf( outbuf, "\n Pattern             | %s", use_pat_name ); DataOut2( 1, Fptdata, outbuf );
    sprintf( outbuf, "\n Sample Name    (1)  | %s", sample_name ); DataOut2( 1, Fptdata, outbuf );
    sprintf( outbuf, "\n Temperature    (2)  | %d'C", test_temperature ); DataOut2( 1, Fptdata, outbuf );
    sprintf( outbuf, "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] ); DataOut2( 1, Fptdata, outbuf );
    DataOut2( 1, Fptdata, "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) DataOut2( 1, Fptdata, "ON ]" ); else DataOut2( 1, Fptdata, "OFF ]" ); sprintf( outbuf, " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV Xstep[%d-%d] Extal[%d-%d]ns", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max], x_loop_min, x_loop_max, extal_table[ x_loop_min ]/1000, extal_table[ x_loop_max ]/1000 ); DataOut2( 1, Fptdata, outbuf );
    sprintf( outbuf, "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); DataOut2( 1, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, " Vdd=%dmV ", vdd_const_flag ); DataOut2( 1, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, " Vddh=%dmV ", vddh_const_flag ); DataOut2( 1, Fptdata, outbuf ); }
    DataOut2( 1, Fptdata, "\n Option         (6)  | FailStop[ " ); if( stop_flag ) DataOut2( 1, Fptdata, "ON ]" ); else DataOut2( 1, Fptdata, "OFF ]" ); DataOut2( 1, Fptdata, "  Short Time[ " ); if( skip_flag ) DataOut2( 1, Fptdata, "ON ]" ); else DataOut2( 1, Fptdata, "OFF ]" ); DataOut2( 1, Fptdata, "  Hit Key Break[ ON ]" );
    DataOut2( 1, Fptdata, "\n\n" );

    /*-----------------------*/
    /* shmooパラメータ初期化 */
    /*-----------------------*/
    for( x = 0; x<100; x++ ){
      for( y = 0; y<100; y++ ) xy_passfail[x][y] = ' ';
      fail_base[x] = fail_point[x] = fail_contents[x] = 0x12345678;
      fail_data[x][0] = fail_data[x][1] = fail_data[x][2] = fail_data[x][3] = fail_data[x][4] = fail_data[x][5] = fail_data[x][6] = fail_data[x][7] = 0x12345678;
    }
    x_label[0] = y_label[0] = 0;
    if( x_loop_min==0 ){
      if( ckscr==0 ){
        if( hocomcr==0 ) x_label[0] = 24000;
        if( hocomcr==1 ) x_label[0] = 32000;
        if( hocomcr==2 ) x_label[0] = 48000;
        if( hocomcr==3 ) x_label[0] = 64000;
      }
      if( ckscr==1 ) x_label[0] = 2000;
      if( ckscr==2 ) x_label[0] = 32;
    }else{
      for( x = x_loop_min; x<=x_loop_max; x+=x_step ){
        if( ckscr==5 ){
          x_label[x] = ( 1000000000 / extal_table[x] ) * ( pllccr_multiple + 1 ) / ( pllccr_div + 1 );
        }else{
          x_label[x] = ( 1000000000 / extal_table[x] );
        }
      }
    }
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ) y_label[y] = vdd_table[ y ];
    for(  x = x_loop_min; x<=x_loop_max; x+=x_step ){ sprintf( outbuf, ",%3d.%03d", x_label[x]/1000, x_label[x]%1000 ); DataOut2( 1, Fptdata, outbuf ); }
    DataOut2( 1, Fptdata, ",,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );

    /***********************************************************************/
    /*******  vdd/vddh Y loop start  ***************************************/
    x_loop_start = x_loop_min;
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      if( break_flag ) break;
      if( -1==vddh_const_flag ) vs[1] = y_label[ y ]; else vs[1] = vddh_const_flag;
      if( -1==vdd_const_flag )  vs[2] = y_label[ y ]; else vs[2] = vdd_const_flag;
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 1, Fptdata, outbuf );

      /***********************************************************************/
      /*******  freq X loop start  ********************************************/
      for( x = x_loop_start; x<=x_loop_max; x+=x_step ){
        if( CheckHitKey() ){ printf( "\n Escape(1) / Next Y-vdd(2) / Next X-freq(0) --> " ); DigitInput( &break_flag ); }
        if( break_flag ){
          if( 2==break_flag ) break_flag = 0;
          break;
        }
        if     ( x_label[x]<= 1000 ) time_out = W1SEC*50;
        else if( x_label[x]<= 5000 ) time_out = W1SEC*24;
        else if( x_label[x]<=10000 ) time_out = W1SEC*12;
        else if( x_label[x]<=20000 ) time_out = W1SEC*6;
        else if( x_label[x]<=30000 ) time_out = W1SEC*3; //Versa 2.02s@25MHz  1.06@50MHz  Simulation 2.85s@10MHz
        else                         time_out = W1SEC*2;
        if( CheckFK(7) ) /*D*/time_out = time_out*3;
        SetValue( IN_EXTAL_PS, param, extal_table[x] );
        in_ldata[11] = extal_table[x];
        /*-------------*/
        /* pattern exe */
        /*-------------*/
        rc = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], time_out, param, CreatePatFileName( use_pat_name ), use_test_name, &in_ldata[0] );
        if( PASS!=rc && stop_flag==1 ) rc = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], time_out, param, CreatePatFileName( use_pat_name ), use_test_name, &in_ldata[0] );
        /*----------------*/
        /* result setting */
        /*----------------*/
        fail_contents[y] = ReadEcrMode8( 0x84, SIZE_L );
        fail_base[y] = ReadEcrMode8( 0xA0, SIZE_L );
        fail_point[y] = ReadEcrMode8( 0xA4, SIZE_L );
        for( tempo = 0; tempo<8; tempo++ ) fail_data[y][tempo] = ReadEcrMode8( 0xB0 + tempo*4, SIZE_L );
        if     ( PASS==rc ){ xy_passfail[x][y] = 'P';
                             dbg_mosccr = ReadEcrMode8( 0xE4, SIZE_L ); dbg_pwstf = ReadEcrMode8( 0xE8, SIZE_L ); dbg_pllccr = ReadEcrMode8( 0xDC, SIZE_L );
                             dbg_flwt = ReadEcrMode8( 0xF4, SIZE_L ); dbg_smder = ReadEcrMode8( 0xD0, SIZE_L ); dbg_sckscr = ReadEcrMode8( 0xD8, SIZE_L );
                           }
        else if( FAIL==rc ){ xy_passfail[x][y] = 'F'; }
        else               { xy_passfail[x][y] = 'N'; }
        sprintf( outbuf, ",%c(%d:%d)", xy_passfail[x][y], Cpuferr, Cputotal ); DataOut2( 1, Fptdata, outbuf );
        if( CheckFK(7) ){ /* debug */
          sprintf( outbuf, "(addr=%08X data=%08X result=%08X extal=%d)", ReadEcrMode8( 0x98, SIZE_L ), ReadEcrMode8( 0x9C, SIZE_L ), ReadEcrMode8( 0x80, SIZE_L ), extal_table[x] ); DataOut2( 2, Fptdata, outbuf );
          dbg_mosccr = ReadEcrMode8( 0xE4, SIZE_L ); dbg_pwstf = ReadEcrMode8( 0xE8, SIZE_L ); dbg_pllccr = ReadEcrMode8( 0xDC, SIZE_L ); dbg_sckscr = ReadEcrMode8( 0xD8, SIZE_L );
          dbg_flwt = ReadEcrMode8( 0xF4, SIZE_L ); dbg_smder = ReadEcrMode8( 0xD0, SIZE_L ); dbg_sckscr = ReadEcrMode8( 0xD8, SIZE_L );
        }
        if( PASS!=rc && stop_flag==1 ) break;
      }
      /*******  freq X loop end  *********************************************/
      /***********************************************************************/
      if( skip_flag ){
        if( x - 6 > x_loop_min ){
          x_loop_start = x - 6;
          for( x = x_loop_min; x<x_loop_start; x++ ){ xy_passfail[x][y+1] = '.'; };
        }else{
          x_loop_start = x_loop_min;
        }
      }
    }
    /*******  vdd Y loop end  **********************************************/
    /***********************************************************************/

    /**********************************/
    /* after result log out & display */
    /**********************************/

    DataOut2( 1, Fptdata, "\n=================================================================================\n" );
    sprintf( outbuf, "%s    Ta = %d\n", sample_name, test_temperature ); DataOut2( 1, Fptdata, outbuf );
    GetDateTime( &stime_list, stime_buf, 0 ); time( &etime_list ); GetDateTime( &etime_list, etime_buf, 0 );
    sprintf( outbuf, "Date %s - %s\n", stime_buf, etime_buf ); DataOut2( 1, Fptdata, outbuf );
//    sprintf( outbuf, "lot=%-10.10s , wafer=%-2.2s , X=%3d , Y=%3d \n", lotname, wafername, xy_location[0], xy_location[1] ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, "wafer=%-2.2s , X=%3d , Y=%3d \n", wafername, xy_location[0], xy_location[1] ); DataOut2( 1, Fptdata, outbuf );
    sprintf( outbuf, " PAT = [%s] VCC=%dmV", use_pat_name, vs[0] ); DataOut2( 1, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, "  Vdd=%dmV ", vdd_const_flag ); DataOut2( 3, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, "  Vddh=%dmV", vddh_const_flag ); DataOut2( 3, Fptdata, outbuf ); } DataOut2( 3, Fptdata, "\n" );
    sprintf( outbuf, " Last Pass PWSTF=%02X MOSCCR=%02X PLLCCR=%04X FLWT=%02X SMDER=%04X SCKSCR=%02X\n", dbg_pwstf, dbg_mosccr, dbg_pllccr, dbg_flwt, dbg_smder, dbg_sckscr ); DataOut2( 3, Fptdata, outbuf );
    DataOut2( 1, Fptdata, "\nFreq=" );
    for( x = x_loop_min; x<=x_loop_max; x+=x_step ){ sprintf( outbuf, ",%3d.%03d", x_label[x]/1000, x_label[x]%1000 ); DataOut2( 1, Fptdata, outbuf ); }
    DataOut2( 1, Fptdata, ",,pass freq,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );
    if( -1!=vdd_const_flag ){ sprintf( outbuf, "vdd=[%dmV]", vdd_const_flag ); DataOut2( 1, Fptdata, outbuf ); }else{ DataOut2( 1, Fptdata, "vdd" ); }
    if( -1!=vddh_const_flag ){ sprintf( outbuf, "/vddh=[%dmV]", vddh_const_flag ); DataOut2( 1, Fptdata, outbuf ); }else{ DataOut2( 1, Fptdata, "/vddh" ); }

    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 1, Fptdata, outbuf );
      tempo_pass = -1;
      for( x = x_loop_min; x<=x_loop_max; x+=x_step ){
        sprintf( outbuf, ",%c", xy_passfail[x][y] ); DataOut2( 1, Fptdata, outbuf );
        if( xy_passfail[x][y]=='P' ) tempo_pass = x;
      }
      if( tempo_pass == -1 ) { sprintf( outbuf, ",,%3d.%03d", 0, 0 ); DataOut2( 1, Fptdata, outbuf ); }
      else                   { sprintf( outbuf, ",,%3d.%03d", x_label[tempo_pass]/1000, x_label[tempo_pass]%1000 ); DataOut2( 1, Fptdata, outbuf ); }
      sprintf( outbuf, ",,0x%08X,0x%08X,0x%08X", fail_contents[y], fail_base[y], fail_point[y] ); DataOut2( 1, Fptdata, outbuf );
      for( tempo = 0; tempo<8; tempo++ ){ sprintf( outbuf, ",0x%08X", fail_data[y][tempo] ); DataOut2( 1, Fptdata, outbuf ); }
    }
    DataOut2( 1, Fptdata, "\n\n\n=================================================================================\n" );
    fclose( Fptdata );
  }while( repeat_flag );

  v_memcpy( (void*)param, (void*)param_back, sizeof(interface_t)*TABLE_END );    /* input param restore */

  return(PASS);
}

#endif


#if ( PRODUCT_NAME==PRODUCT_IO )

int ReadShmooSelect( int vcc[NUM_POWERSUPPLY], int waittime, interface_t* param ){
  /*
  int vdd_500_table[]={
    0, 600, 575, 550, 525, 500, 475, 450, 425, 400,
  };*/
  int vdd_500_table[]={
    0, 530, 500, 455, 500, 530, 500, 455, 425, 400,
  };
  int vddh_500_table[]={
    0, 550, 550, 550, 500, 450, 450, 450, 425, 400,
  };

  interface_t param_back[TABLE_END+1];
  int vs[4];                            /* local vs[n] variable      */
  int ctr_exp, ctr_typ, flwt;
  int *vdd_table, vdd_tbl_max;
  int y, y_loop_min, y_loop_max;
  int y_step;
  int tempo, tempo1, tempo2, tempo3, tempo0;
  char use_pat_name[64+4];
  char use_test_name[64+4];
  char sample_name[32+4];
  int test_temperature;
  int exvref_flag, break_flag;
  time_t  stime_list, etime_list;
  char  stime_buf[32+4], etime_buf[32+4];
  int x_label[100+4], y_label[100+4];
  int pass_freq[100+4], fail_freq[100+4], fail_greg[100+4], fail_contents[100+4], fail_base[100+4], fail_point[100+4], fail_data[100+4][8+0];
  int in_ldata[16+2];
  int rc, n;
  int dbg_mosccr, dbg_pwstf, dbg_flwt, dbg_smder, dbg_sckscr, dbg_flag;
  int vdd_const_flag, vddh_const_flag, time_out, extal_ps;
  unsigned int freq_t[41], greg77_t[41];


  v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*TABLE_END );    /* input param store */


  vs[0] = vcc[0]; vs[1] = vcc[1]; vs[2] = vcc[2];
  in_ldata[4] = 0x00000000; /* UIArea */
  in_ldata[5] = 0x00000000; /* Top Address */
  in_ldata[6] = 0x00040000; /* End Address */
  in_ldata[7] = 0x00000202; /* bit0-7   expect 0=All0 1=All1 2=ChkA 3=ChkB */
                            /* bit8-15  bit8=1=Normal bit9=1=X-PingPong bit10=1=X-Complemental */
  in_ldata[8] = 0x00000000; /* 0  start n */
  in_ldata[9] = 0x00000027; /* 39   end n */
  in_ldata[12]= 0x00000000; /* FLWT 0=0wait  1=1wait */


  vdd_table   = vdd_500_table; vdd_tbl_max = sizeof( vdd_500_table ) / sizeof( int );
  y_step = 1;
  y_loop_min = 1; y_loop_max = 9-2;    /* vdd  */
  time_out = W1SEC*30;
  extal_ps = 100000;
  vs[0]=3800;

  strcpy( use_pat_name, "read_fmax_io_v001" );
  strcpy( use_test_name, "Read Fmax Test" );
  sprintf( sample_name, "X:%dY:%d", xy_location[0], xy_location[1] );
  test_temperature = 25;
  exvref_flag = 1; break_flag = 0;
  vdd_const_flag = vddh_const_flag = -1;
  ctr_exp = in_ldata[7]&0x00FF;
  ctr_typ = ( in_ldata[7]>>8 )&0x00FF;
  flwt = in_ldata[12];

  do{
    tempo = tempo0 = 12345678;
    while( tempo ){
      printf( "\n title   (change no)");
      printf( "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) printf( "All0" ); else if( ctr_exp==1 ) printf( "All1" ); else if( ctr_exp==2 ) printf( "CheckerA" ); else if( ctr_exp==3 ) printf( "CheckerB" ); else printf( "None" );
      printf( "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) printf( "Normal " ); else if( ctr_typ&0x02 ) printf( "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) printf( "Complemental X X+1 " ); else printf( "None" );
      printf( "\n FLWT           (60) | wait %d cyc", flwt );
      printf( "\n Extal          (70) | %d ps", extal_ps );
      printf( "\n Sample Name    (1)  | %s", sample_name );
      printf( "\n Temperature    (2)  | %d'C", test_temperature );
      printf( "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] );
      printf( "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) printf( "ON ]" ); else printf( "OFF ]" );  printf( " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] );
      printf( "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); if( -1==vdd_const_flag ) printf( " Vdd=%dmV ", vdd_const_flag ); if( -1==vddh_const_flag ) printf( " Vddh=%dmV ", vddh_const_flag );
      printf( "\n Freq n max39   (6)  | %d - %d", in_ldata[8], in_ldata[9] );
      printf( "\n flash_boot          (77)  UI=0-1000 Program Func  1000-3FFFF expect ChkA" );
      printf( "\n flash_boot_port     (772) UI=0-1000 Program Func" );
      printf( "\n flash_boot_port23s  (773) UI=0-1000 Program Func" );
      printf( "\n flash_boot_dijkstra (774) UI=0-1000 Program Func" );
      printf( "\n flash_boot_jmp      (78)  7s time jmp UI 101 address" );
      printf( "\n flash_boot_jmp2     (782) 30s time out jmp UI 101 address" );
      printf( "\n flash_boot_jmp3     (783) ETCR0-11 ETLR0-39 Change Jmp" );
      printf( "\n flash_boot_jmp4     (784) ETCR0-11 ETLR0-39 Change void jmp" );
      printf( "\n User Boot TM18      (79)" );
      printf( "\n Break (99)  Test Start(0)\n   input no ==> " );
      DigitInput( &tempo );
      if( tempo==99 ) return( FAIL );
      if( tempo==1 ){ printf( "sample name[max 30 char] => " ); StringInput( sample_name ); }
      if( tempo==2 ){ printf( "temperature => " ); DigitInput( &test_temperature ); }
      if( tempo==3 ){ 
        printf( "\n                AreaSelect  Top Addr   End Addr+1" );
        printf( "\n UserArea         ( 0 )     0x00000000-0x00040000" );
        printf( "\n Extra1=Table1    ( 5 )     0x01010000-0x01010400" );
        printf( "\n Extra2=Info1     ( 1 )     0x01008000-0x01008400" );
        printf( "\n Extra3=Info2     ( 2 )     0x0100A000-0x0100A400" );
        printf( "\n Extra4=Info3     ( 3 )     0x0100C000-0x0100C400" );
        printf( "\n Extra5=Table2    ( 6 )     0x01012000-0x01012400" );
        printf( "\n Extra6=Info4     ( 4 )     0x01010000-0x01003000 or 0x01004000" );
        printf( "Area 0-6  => " ); DigitInput( &in_ldata[4] );
        printf( "Top Addr  => " ); DigitInput( &in_ldata[5] );
        printf( "End Addr  => " ); DigitInput( &in_ldata[6] );
      }
      if( tempo==4 ){
        tempo1 = exvref_flag;
        printf( "Exvref Off=0  On=1  => " ); DigitInput( &exvref_flag );
        if( exvref_flag!=tempo1 ){
          if( 0==exvref_flag ){ y_loop_min = 0; y_loop_max = 0;
          }else{                y_loop_min = 1; y_loop_max = 9; }
        }
        if( exvref_flag ){
          printf( " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] );
          printf( "\n Ystart => " ); DigitInput( &y_loop_min ); printf( "Yend max %d => ", vdd_tbl_max ); DigitInput( &y_loop_max );
        }
        if( y_loop_min<0 ) y_loop_min = 0; if( y_loop_max>vdd_tbl_max ) y_loop_max = vdd_tbl_max;
      }
      if( tempo==5 ){
        printf( "Vcc  => " ); DigitInput( &vs[0] );
        printf( "Const Vdd  No=-1 Yes=VddmV  => " ); DigitInput( &vdd_const_flag );
        printf( "Const Vddh No=-1 Yes=VddmV  => " ); DigitInput( &vddh_const_flag );
        if( -1!=vdd_const_flag && -1!=vddh_const_flag ){
          printf( "?? No Vddh/Vdd Shmoo yes=1 no=other => " ); DigitInput( &tempo1 ); if( 1!=tempo1 ) vdd_const_flag = vddh_const_flag = 0;
        }
      }
      if( tempo==6 ){
        printf( "\n n:  0,  1,  2,  3,  4,  5,  6,  7,  8,  9\n" );
        printf( "00: 32, 36, 40, 44, 48, 50, 52, 54, 56, 58\n" );
        printf( "10: 60, 62, 64, 66, 68, 70, 72, 74, 76, 78\n" );
        printf( "20: 80, 82, 84, 86, 88, 90, 92, 94, 96, 98\n" );
        printf( "30:100,102,104,106,108,110,112,114,116,118\n" );
        printf( "Const start n  => " ); DigitInput( &in_ldata[8] );
        printf( "Const   end n  => " ); DigitInput( &in_ldata[9] );
        if( in_ldata[8]>in_ldata[9] || in_ldata[8]<0 ) in_ldata[8] = 0;
        if( in_ldata[9]>39 ) in_ldata[9] = 39;
      }
      if( tempo==10 ){ printf( "0=All0  1=All1  2=ChkA  3=ChkB  => " ); DigitInput( &ctr_exp ); }
      if( tempo==20 ){ printf( "bit0=1=Normal  bit1=1=PingPong  bit2=1=Complemental   => " ); DigitInput( &ctr_typ ); }
      if( tempo==60 ){ printf( "0=0 wait  1=1 wait  => " ); DigitInput( &flwt ); }
      if( tempo==70 ){ printf( "Extal Xps  X=> " ); DigitInput( &extal_ps ); }
      if( tempo==77 ){ strcpy( use_pat_name, "flash_boot_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==772 ){ strcpy( use_pat_name, "flash_boot_port_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==773 ){ strcpy( use_pat_name, "flash_boot_port23s_v001" ); time_out = W1SEC*30; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==774 ){ strcpy( use_pat_name, "flash_boot_dijkstra_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==78 ){ strcpy( use_pat_name, "flash_boot_jmp_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==782 ){ strcpy( use_pat_name, "flash_boot_jmp2_v001" ); time_out = W1SEC*30; y_step = 4; }
      if( tempo==783 ){ strcpy( use_pat_name, "flash_boot_jmp3_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==784 ){ strcpy( use_pat_name, "flash_boot_jmp4_v001" ); time_out = W1SEC*7; y_step = 1; }
      if( tempo==79 ){ strcpy( use_test_name, "UserBoot TM18 Test" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo!=0 ) tempo0 = tempo;
    }

    in_ldata[7] = ((ctr_typ<<8)&0x00FF00) | ( ctr_exp&0x00FF );
    in_ldata[12] = flwt;

    dbg_flag = 0;
    if( tempo0==783 || tempo0==784 ){
      for( n = 0; n<7; n++ ) in_ldata[ n ] = 0x55345678;
      n = 0;
      do{
        printf( "No Change=0  ETLR Change=1  ETCR Change=2   => " ); DigitInput( &tempo2 );
        if( tempo2==1 || tempo2==2 ){
          tempo2 = ( tempo2<<24 ) & 0xFF000000;
          printf( "Reg No ? ETLR=0-39  ETCR=0-11   No => " ); DigitInput( &tempo3 );
          tempo2 = tempo2 | ( ( tempo3<<16 ) & 0x00FF0000 );
          printf( "Reg Data 0xXX => " ); DigitInput( &tempo3 );
          tempo2 = tempo2 | ( tempo3&0x000000FF );
          in_ldata[ n ] = tempo2;
          n++;
          if( n>6 )  tempo1 = 0;
          else       tempo1 = 1;
        }else{
          tempo1 = 0;
        }
      }while( tempo1 );
      if( n ) dbg_flag = 1;
    }

    /*-----------------------*/
    /* ログファイル設定      */
    /*-----------------------*/
    strcpy( baselog, glob_cdp );
    sprintf( filenamebuf, "\\DATALOG\\DATA.LOG_Fmax_%d_%d.csv", vs[0], get_site_number() );
    time( &stime_list );
    strcat( baselog, filenamebuf );
    if( ( Fptdata = fopen( baselog, "at" ) )==NULL ){ printf( " -- FAIL -- Not Open file:%s !\n", baselog ); return( FAIL ); }

    DataOut2( 2, Fptdata, "\n-------------------------------------------\n" );
    sprintf( outbuf, "%s\n", sample_name ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "-------------------------------------------\n" );
    sprintf( outbuf, "Ta = %d 'C\n", test_temperature ); DataOut2( 2, Fptdata, outbuf );

    sprintf( outbuf, "\n title   (change no) %d", tempo0 ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) DataOut2( 2, Fptdata, "All0" ); else if( ctr_exp==1 ) DataOut2( 2, Fptdata, "All1" ); else if( ctr_exp==2 ) DataOut2( 2, Fptdata, "CheckerA" ); else if( ctr_exp==3 ) DataOut2( 2, Fptdata, "CheckerB" ); else DataOut2( 2, Fptdata, "None" );
    DataOut2( 2, Fptdata, "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) DataOut2( 2, Fptdata, "Normal " ); else if( ctr_typ&0x02 ) DataOut2( 2, Fptdata, "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) DataOut2( 2, Fptdata, "Complemental X X+1 " ); else DataOut2( 2, Fptdata, "None" );
    sprintf( outbuf, "\n FLWT           (60) | wait %d cyc", flwt ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Extal          (70) | %d ps", extal_ps ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Pattern             | %s", use_pat_name ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Sample Name    (1)  | %s", sample_name ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Temperature    (2)  | %d'C", test_temperature ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) DataOut2( 2, Fptdata, "ON ]" ); else DataOut2( 2, Fptdata, "OFF ]" ); sprintf( outbuf, " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); DataOut2( 2, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, " Vdd=%dmV ", vdd_const_flag ); DataOut2( 2, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, " Vddh=%dmV ", vddh_const_flag ); DataOut2( 2, Fptdata, outbuf ); }
    sprintf( outbuf, "\n Freq n max39   (6)  | %d - %d", in_ldata[8], in_ldata[9] ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n\n" );

    /*-----------------------*/
    /* shmooパラメータ初期化 */
    /*-----------------------*/
    x_label[0] = y_label[0] = 0;
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      y_label[y] = vdd_table[ y ];
      fail_base[y] = fail_point[y] = fail_contents[y] = 0x12345678;
      fail_data[y][0] = fail_data[y][1] = fail_data[y][2] = fail_data[y][3] = fail_data[y][4] = fail_data[y][5] = fail_data[y][6] = fail_data[y][7] = 0x12345678;
    }
    SetValue( IN_EXTAL_PS, param, extal_ps ); /* def 100ns */

    /***********************************************************************/
    /*******  vdd/vddh Y loop start  ***************************************/
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      if( CheckHitKey() ){ printf( "\n Escape(1) / Next(0) --> " ); DigitInput( &break_flag ); }
      if( break_flag )  break;

      if( -1==vddh_const_flag ) vs[1] = y_label[ y ]; else vs[1] = vddh_const_flag;
      if( -1==vdd_const_flag )  vs[2] = /*y_label[ y ]*/vddh_500_table[y]; else vs[2] = vdd_const_flag;
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 2, Fptdata, outbuf );

      /*-------------*/
      /* pattern exe */
      /*-------------*/
      rc = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], time_out, param, CreatePatFileName( use_pat_name ), use_test_name, &in_ldata[0] );
      pass_freq[y] = ReadEcrMode8( 0x98, SIZE_L );
      fail_freq[y] = ReadEcrMode8( 0x9C, SIZE_L );
      fail_contents[y] = ReadEcrMode8( 0x84, SIZE_L );
      fail_base[y] = ReadEcrMode8( 0xA0, SIZE_L );
      fail_point[y] = ReadEcrMode8( 0xA4, SIZE_L );
      for( tempo = 0; tempo<8; tempo++ ) fail_data[y][tempo] = ReadEcrMode8( 0xB0 + tempo*4, SIZE_L );
      if( tempo0==78 || tempo0==79 || tempo0==782 || tempo0==783 || tempo0==784 ){
        fail_contents[y] = ReadEcrMode8( 0x80, SIZE_L ); fail_base[y] = ReadEcrMode8( 0x84, SIZE_L ); fail_point[y] = ReadEcrMode8( 0x88, SIZE_L );
        for( tempo = 0; tempo<8; tempo++ ) fail_data[y][tempo] = ReadEcrMode8( 0x90 + tempo*4, SIZE_L );
      }
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        freq_t[n] = ReadEcrMode8( 0x3E00 + n*4, SIZE_L );
        greg77_t[n] = ReadEcrMode8( 0x3F00 + n*4, SIZE_L );
        if( y==y_loop_min ){ sprintf( outbuf, ",%3d.%03d", (freq_t[n]/1000)/1000, (freq_t[n]/1000)%1000 ); DataOut2( 2, Fptdata, outbuf ); x_label[n] = freq_t[n]/1000; }
      }
      if( y==y_loop_min ){
        dbg_mosccr = ReadEcrMode8( 0xE4, SIZE_L ); dbg_pwstf = ReadEcrMode8( 0xE8, SIZE_L );
        dbg_flwt = ReadEcrMode8( 0xF4, SIZE_L ); dbg_smder = ReadEcrMode8( 0xD0, SIZE_L ); dbg_sckscr = ReadEcrMode8( 0xD8, SIZE_L );
        x_label[40] = ReadEcrMode8( 0x3E00 + 40*4, SIZE_L );
        x_label[41] = ReadEcrMode8( 0x3F00 + 40*4, SIZE_L );
        DataOut2( 2, Fptdata, ",,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );
      }
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        sprintf( outbuf, ",%3d.%03d", (freq_t[n]/1000)/1000, (freq_t[n]/1000)%1000 ); DataOut2( 2, Fptdata, outbuf );
      }
      DataOut2( 2, Fptdata, "\n" );
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        sprintf( outbuf, ",%08X(%08X)", greg77_t[n], freq_t[n] ); DataOut2( 2, Fptdata, outbuf );
      }
      if( dbg_flag ){
        DataOut2( 2, Fptdata, ",InitReg   ETCR0-17 ETLR0-39,0x3D10-->" );
        for( n = 0x3D10; n<0x3D50; n+=4 ){ sprintf( outbuf, ",%08X", ReadEcrMode8( n, SIZE_L ) ); DataOut2( 2, Fptdata, outbuf ); }
        DataOut2( 2, Fptdata, ",ChangeReg ETCR0-17 ETLR0-39,0x3D50-->" );
        for( n = 0x3D50; n<0x3D90; n+=4 ){ sprintf( outbuf, ",%08X", ReadEcrMode8( n, SIZE_L ) ); DataOut2( 2, Fptdata, outbuf ); }
      }
      sprintf( outbuf, ",%3d.%03d MHz  FAIL   TestTime = %d[%d]   PF(flag)=%08X(%08X)\n", (fail_freq[y]/1000)/1000, (fail_freq[y]/1000)%1000, Cputotal, Cpuferr, ReadEcrMode8( 0x80, SIZE_L ), ReadEcrMode8( 0x84, SIZE_L ) ); DataOut2( 3, Fptdata, outbuf );
    }
    /*******  vdd Y loop end  **********************************************/
    /***********************************************************************/

    /**********************************/
    /* after result log out & display */
    /**********************************/

    DataOut2( 3, Fptdata, "\n=================================================================================\n" );
    sprintf( outbuf, "%s    Ta = %d\n", sample_name, test_temperature ); DataOut2( 3, Fptdata, outbuf );
    GetDateTime( &stime_list, stime_buf, 0 ); time( &etime_list ); GetDateTime( &etime_list, etime_buf, 0 );
    sprintf( outbuf, "Date %s - %s\n", stime_buf, etime_buf ); DataOut2( 3, Fptdata, outbuf );
//    sprintf( outbuf, "lot=%-10.10s , wafer=%-2.2s , X=%3d , Y=%3d \n", lotname, wafername, xy_location[0], xy_location[1] ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, "wafer=%-2.2s , X=%3d , Y=%3d \n", wafername, xy_location[0], xy_location[1] ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, " PAT = [%s] Extal=%dps VCC=%dmV", use_pat_name, extal_ps, vs[0] ); DataOut2( 3, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, "  Vdd=%dmV ", vdd_const_flag ); DataOut2( 3, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, "  Vddh=%dmV", vddh_const_flag ); DataOut2( 3, Fptdata, outbuf ); } DataOut2( 3, Fptdata, "\n" );
    sprintf( outbuf, " 1st Test PWSTF=%02X MOSCCR=%02X  FLWT=%02X SMDER=%04X SCKSCR=%02X\n", dbg_pwstf, dbg_mosccr, dbg_flwt, dbg_smder, dbg_sckscr ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, " Trim 64MHz = %08X = %3d.%03d MHz\n", x_label[41], ( x_label[40]/1000 )/1000, ( x_label[40]/1000 )%1000 ); DataOut2( 3, Fptdata, outbuf );
    
    DataOut2( 3, Fptdata, "\nFreq=" );
    for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
      sprintf( outbuf, ",%3d.%03d", x_label[n]/1000, x_label[n]%1000 ); DataOut2( 3, Fptdata, outbuf );
    }
    DataOut2( 3, Fptdata, "\n,,pass freq,fail freq,,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );
    if( -1!=vdd_const_flag ){ sprintf( outbuf, "vdd=[%dmV]", vdd_const_flag ); DataOut2( 3, Fptdata, outbuf ); }else{ DataOut2( 3, Fptdata, "vdd" ); }
    if( -1!=vddh_const_flag ){ sprintf( outbuf, "/vddh=[%dmV]", vddh_const_flag ); DataOut2( 3, Fptdata, outbuf ); }else{ DataOut2( 3, Fptdata, "/vddh" ); }

    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, " / %3d.%03d", vddh_500_table[y]/1000, vddh_500_table[y]%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, ",,%3d.%03d,%3d.%03d", (pass_freq[y]/1000)/1000, (pass_freq[y]/1000)%1000, (fail_freq[y]/1000)/1000, (fail_freq[y]/1000)%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, ",,,0x%08X,0x%08X,0x%08X", fail_contents[y], fail_base[y], fail_point[y] ); DataOut2( 3, Fptdata, outbuf );
      for( tempo = 0; tempo<8; tempo++ ){ sprintf( outbuf, ",0x%08X", fail_data[y][tempo] ); DataOut2( 3, Fptdata, outbuf ); }
    }
    DataOut2( 3, Fptdata, "\n\n\n=================================================================================\n" );
    fclose( Fptdata );
  }while( 1 );

  v_memcpy( (void*)param, (void*)param_back, sizeof(interface_t)*TABLE_END );    /* input param restore */

  return(PASS);
}

#endif

int RomBoot_ShmooTest(int mode ,int vcc[NUM_POWERSUPPLY], int waittime, interface_t* param ){
  /*
  int vdd_500_table[]={
    0, 600, 575, 550, 525, 500, 475, 450, 425, 400,
  };*/
  int vdd_500_table[]={
     530, 500, 455, 500, 530, 500, 455, 425, 400,
  };
  int vddh_500_table[]={
     550, 550, 550, 500, 450, 450, 450, 425, 400,
  };
  int Freq_table[]={
     550, 550, 550, 500, 450, 450, 450, 425, 400,
  };
  int FreqMax = 8;
  int VddhMax = 8;
  int VddMax = 8;
  int ShmooData_Vddh[VddhMax+1][FreqMax+1],ShmooData_Vdd[VddMax+1][FreqMax+1];
  int powerkind,PowerNum,PowerMax,FreqNum,fpower;
  char FileName[100],AddName[20];

  interface_t param_back[TABLE_END+1];
  int vs[4];                            /* local vs[n] variable      */
  int ctr_exp, ctr_typ, flwt;
  int *vdd_table, vdd_tbl_max;
  int y, y_loop_min, y_loop_max;
  int y_step;
  int tempo, tempo1, tempo2, tempo3, tempo0;
  char use_pat_name[64+4];
  char use_test_name[64+4];
  char sample_name[32+4];
  int test_temperature;
  int exvref_flag, break_flag;
  time_t  stime_list, etime_list;
  char  stime_buf[32+4], etime_buf[32+4];
  int x_label[100+4], y_label[100+4];
  int pass_freq[100+4], fail_freq[100+4], fail_greg[100+4], fail_contents[100+4], fail_base[100+4], fail_point[100+4], fail_data[100+4][8+0];
  int in_ldata[16+2];
  int rc, n;
  int dbg_mosccr, dbg_pwstf, dbg_flwt, dbg_smder, dbg_sckscr, dbg_flag;
  int vdd_const_flag, vddh_const_flag, time_out, extal_ps;
  unsigned int freq_t[41], greg77_t[41];


  v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*TABLE_END );    /* input param store */


  vs[0] = vcc[0]; vs[1] = vcc[1]; vs[2] = vcc[2];
  in_ldata[4] = 0x00000000; /* UIArea */
  in_ldata[5] = 0x00000000; /* Top Address */
  in_ldata[6] = 0x00040000; /* End Address */
  in_ldata[7] = 0x00000202; /* bit0-7   expect 0=All0 1=All1 2=ChkA 3=ChkB */
                            /* bit8-15  bit8=1=Normal bit9=1=X-PingPong bit10=1=X-Complemental */
  in_ldata[8] = 0x00000000; /* 0  start n */
  in_ldata[9] = 0x00000027; /* 39   end n */
  in_ldata[12]= 0x00000000; /* FLWT 0=0wait  1=1wait */


  vdd_table   = vdd_500_table; vdd_tbl_max = sizeof( vdd_500_table ) / sizeof( int );
  y_step = 1;
  y_loop_min = 1; y_loop_max = 9-2;    /* vdd  */
  time_out = W1SEC*30;
  extal_ps = 100000;
  vs[0]=3300;

  strcpy( use_pat_name, "read_fmax_io_v001" );
  strcpy( use_test_name, "Read Fmax Test" );
  sprintf( sample_name, "X:%dY:%d", xy_location[0], xy_location[1] );
  test_temperature = 25;
  exvref_flag = 1; break_flag = 0;
  vdd_const_flag = vddh_const_flag = -1;
  ctr_exp = in_ldata[7]&0x00FF;
  ctr_typ = ( in_ldata[7]>>8 )&0x00FF;
  flwt = in_ldata[12];

    tempo = tempo0 = 12345678;
      printf( "\n title   (change no)");
      printf( "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) printf( "All0" ); else if( ctr_exp==1 ) printf( "All1" ); else if( ctr_exp==2 ) printf( "CheckerA" ); else if( ctr_exp==3 ) printf( "CheckerB" ); else printf( "None" );
      printf( "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) printf( "Normal " ); else if( ctr_typ&0x02 ) printf( "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) printf( "Complemental X X+1 " ); else printf( "None" );
      printf( "\n FLWT           (60) | wait %d cyc", flwt );
      printf( "\n Extal          (70) | %d ps", extal_ps );
      printf( "\n Sample Name    (1)  | %s", sample_name );
      printf( "\n Temperature    (2)  | %d'C", test_temperature );
      printf( "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] );
      printf( "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) printf( "ON ]" ); else printf( "OFF ]" );  printf( " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] );
      printf( "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); if( -1==vdd_const_flag ) printf( " Vdd=%dmV ", vdd_const_flag ); if( -1==vddh_const_flag ) printf( " Vddh=%dmV ", vddh_const_flag );
      printf( "\n Freq n max39   (6)  | %d - %d", in_ldata[8], in_ldata[9] );
      printf( "\n flash_boot          (77)  UI=0-1000 Program Func  1000-3FFFF expect ChkA" );
      printf( "\n flash_boot_port     (772) UI=0-1000 Program Func" );
      printf( "\n flash_boot_port23s  (773) UI=0-1000 Program Func" );
      printf( "\n flash_boot_dijkstra (774) UI=0-1000 Program Func" );
      printf( "\n flash_boot_jmp      (78)  7s time jmp UI 101 address" );
      printf( "\n flash_boot_jmp2     (782) 30s time out jmp UI 101 address" );
      printf( "\n flash_boot_jmp3     (783) ETCR0-11 ETLR0-39 Change Jmp" );
      printf( "\n flash_boot_jmp4     (784) ETCR0-11 ETLR0-39 Change void jmp" );
      printf( "\n User Boot TM18      (79)" );
      printf( "\n Break (99)  Test Start(0)\n   input no ==> " );
      //DigitInput( &tempo );
      tempo = 784;
      if( tempo==10 ){ printf( "0=All0  1=All1  2=ChkA  3=ChkB  => " ); DigitInput( &ctr_exp ); }
      if( tempo==20 ){ printf( "bit0=1=Normal  bit1=1=PingPong  bit2=1=Complemental   => " ); DigitInput( &ctr_typ ); }
      if( tempo==60 ){ printf( "0=0 wait  1=1 wait  => " ); DigitInput( &flwt ); }
      if( tempo==70 ){ printf( "Extal Xps  X=> " ); DigitInput( &extal_ps ); }
      if( tempo==77 ){ strcpy( use_pat_name, "flash_boot_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==772 ){ strcpy( use_pat_name, "flash_boot_port_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==773 ){ strcpy( use_pat_name, "flash_boot_port23s_v001" ); time_out = W1SEC*30; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==774 ){ strcpy( use_pat_name, "flash_boot_dijkstra_v004" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==78 ){ strcpy( use_pat_name, "flash_boot_jmp_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==782 ){ strcpy( use_pat_name, "flash_boot_jmp2_v001" ); time_out = W1SEC*30; y_step = 4; }
      if( tempo==783 ){ strcpy( use_pat_name, "flash_boot_jmp3_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==784 ){ strcpy( use_pat_name, "flash_boot_jmp4_v004" ); time_out = W1SEC*7; y_step = 1; }
      if( tempo==79 ){ strcpy( use_test_name, "UserBoot TM18 Test" ); time_out = W1SEC*7; y_step = 4; }
    

    in_ldata[7] = ((ctr_typ<<8)&0x00FF00) | ( ctr_exp&0x00FF );
    in_ldata[12] = flwt;

    dbg_flag = 0;


    /*-----------------------*/
    /* shmooパラメータ初期化 */
    /*-----------------------*/
    x_label[0] = y_label[0] = 0;
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      y_label[y] = vdd_table[ y ];
      fail_base[y] = fail_point[y] = fail_contents[y] = 0x12345678;
      fail_data[y][0] = fail_data[y][1] = fail_data[y][2] = fail_data[y][3] = fail_data[y][4] = fail_data[y][5] = fail_data[y][6] = fail_data[y][7] = 0x12345678;
    }
    SetValue( IN_EXTAL_PS, param, extal_ps ); /* def 100ns */

    /***********************************************************************/
    /*******  vdd/vddh Y loop start  ***************************************/

      /*-------------*/
      /* pattern exe */
      /*-------------*/
    RomBoot_FirstSetting(0,&vcc[0],waittime,param);
    
    in_ldata[0] = InLdataSet_Reg(1,26,0xFB);
    in_ldata[1] = InLdataSet_Reg(1,26,0xFB);
    in_ldata[2] = InLdataSet_Reg(1,26,0xFB);
    in_ldata[3] = InLdataSet_Reg(1,26,0xFB);

    for(powerkind = 1; powerkind<=2; powerkind++){
      if(powerkind == 2) PowerMax = VddMax;
      else PowerMax = VddhMax;
      for(PowerNum = 0; PowerNum<=PowerMax; PowerNum++){
        vs[1] = vcc[1]; vs[2] = vcc[2];
        if(powerkind == 2) vs[powerkind] = vdd_500_table[PowerNum];
        else vs[powerkind] = vddh_500_table[PowerNum];
        for(FreqNum = 0; FreqNum<=FreqMax; FreqNum++){
          SetValue( IN_EXTAL_PS, param, Freq_table[FreqNum]*1000 );
      
          rc = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], time_out, param, CreatePatFileName( use_pat_name ), use_test_name, &in_ldata[0] );
          if(powerkind == 2) ShmooData_Vdd[PowerNum][FreqNum] = rc;
          else ShmooData_Vddh[PowerNum][FreqNum] = rc;
        
        }
      }
    }

    /******************
     * ログ出力
     * *****************/

    sprintf(AddName,"RomBootShmoo_DataLog");
    for(powerkind = 1; powerkind<=2; powerkind++){
      if(powerkind == 2){
        PowerMax = VddMax;
        sprintf(FileName,"%s_Vdd",AddName);
      }else{
        PowerMax = VddhMax;
        sprintf(FileName,"%s_Vddh",AddName);
      }
      CloseDataOutFile();
      OpenDataOutFile(FileName);
      for(PowerNum = 0; PowerNum<=PowerMax; PowerNum++){
        //--- Make Headder ---//
        if(PowerNum == 0){
          fprintf(Fptdata,"RomBootShmoo-Test,Vcc,%d,Vddh,%d,Vdd,%d",vs[0],vs[1],vs[2]);
          fprintf(Fptdata,",",1000/Freq_table[FreqNum]);
          for(FreqNum = 0; FreqNum<=FreqMax; FreqNum++){
            fprintf(Fptdata,"%d,",1000/Freq_table[FreqNum]);
          }
          fprintf(Fptdata,"\n");
        }
        if(powerkind == 2) fpower = vdd_500_table[PowerNum]*158/100;
        else fpower = vddh_500_table[PowerNum]*25/10;
        fprintf(Fptdata,"%d,",fpower);

        for(FreqNum = 0; FreqNum<=FreqMax; FreqNum++){
          if(powerkind == 2) tempo3 = ShmooData_Vdd[PowerNum][FreqNum];
          else tempo3 = ShmooData_Vddh[PowerNum][FreqNum];
          if(tempo3 == PASS){
            fprintf(Fptdata,"P,");
          }else{
            fprintf(Fptdata,"F,");
          }
        }
        fprintf(Fptdata,"\n");
      }
    }


    CloseDataOutFile();



  return(PASS);
}

int RomBoot_FirstSetting(int mode ,int vcc[NUM_POWERSUPPLY], int waittime, interface_t* param ){
  /*
  int vdd_500_table[]={
    0, 600, 575, 550, 525, 500, 475, 450, 425, 400,
  };*/
  int vdd_500_table[]={
    0, 530, 500, 455, 500, 530, 500, 455, 425, 400,
  };
  int vddh_500_table[]={
    0, 550, 550, 550, 500, 450, 450, 450, 425, 400,
  };

  interface_t param_back[TABLE_END+1];
  int vs[4];                            /* local vs[n] variable      */
  int ctr_exp, ctr_typ, flwt;
  int *vdd_table, vdd_tbl_max;
  int y, y_loop_min, y_loop_max;
  int y_step;
  int tempo, tempo1, tempo2, tempo3, tempo0;
  char use_pat_name[64+4];
  char use_test_name[64+4];
  char sample_name[32+4];
  int test_temperature;
  int exvref_flag, break_flag;
  time_t  stime_list, etime_list;
  char  stime_buf[32+4], etime_buf[32+4];
  int x_label[100+4], y_label[100+4];
  int pass_freq[100+4], fail_freq[100+4], fail_greg[100+4], fail_contents[100+4], fail_base[100+4], fail_point[100+4], fail_data[100+4][8+0];
  int in_ldata[16+2];
  int rc, n;
  int dbg_mosccr, dbg_pwstf, dbg_flwt, dbg_smder, dbg_sckscr, dbg_flag;
  int vdd_const_flag, vddh_const_flag, time_out, extal_ps;
  unsigned int freq_t[41], greg77_t[41];


  v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*TABLE_END );    /* input param store */


  vs[0] = vcc[0]; vs[1] = vcc[1]; vs[2] = vcc[2];
  in_ldata[4] = 0x00000000; /* UIArea */
  in_ldata[5] = 0x00000000; /* Top Address */
  in_ldata[6] = 0x00040000; /* End Address */
  in_ldata[7] = 0x00000202; /* bit0-7   expect 0=All0 1=All1 2=ChkA 3=ChkB */
                            /* bit8-15  bit8=1=Normal bit9=1=X-PingPong bit10=1=X-Complemental */
  in_ldata[8] = 0x00000000; /* 0  start n */
  in_ldata[9] = 0x00000027; /* 39   end n */
  in_ldata[12]= 0x00000000; /* FLWT 0=0wait  1=1wait */


  vdd_table   = vdd_500_table; vdd_tbl_max = sizeof( vdd_500_table ) / sizeof( int );
  y_step = 1;
  y_loop_min = 1; y_loop_max = 9-2;    /* vdd  */
  time_out = W1SEC*30;
  extal_ps = 100000;
  vs[0]=3800;

  strcpy( use_pat_name, "read_fmax_io_v001" );
  strcpy( use_test_name, "Read Fmax Test" );
  sprintf( sample_name, "X:%dY:%d", xy_location[0], xy_location[1] );
  test_temperature = 25;
  exvref_flag = 1; break_flag = 0;
  vdd_const_flag = vddh_const_flag = -1;
  ctr_exp = in_ldata[7]&0x00FF;
  ctr_typ = ( in_ldata[7]>>8 )&0x00FF;
  flwt = in_ldata[12];

      //DigitInput( &tempo );
      tempo = 774;

      if( tempo==10 ){ printf( "0=All0  1=All1  2=ChkA  3=ChkB  => " ); DigitInput( &ctr_exp ); }
      if( tempo==20 ){ printf( "bit0=1=Normal  bit1=1=PingPong  bit2=1=Complemental   => " ); DigitInput( &ctr_typ ); }
      if( tempo==60 ){ printf( "0=0 wait  1=1 wait  => " ); DigitInput( &flwt ); }
      if( tempo==70 ){ printf( "Extal Xps  X=> " ); DigitInput( &extal_ps ); }
      if( tempo==77 ){ strcpy( use_pat_name, "flash_boot_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==772 ){ strcpy( use_pat_name, "flash_boot_port_v001" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==773 ){ strcpy( use_pat_name, "flash_boot_port23s_v001" ); time_out = W1SEC*30; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==774 ){ strcpy( use_pat_name, "flash_boot_dijkstra_v004" ); time_out = W1SEC*10; exvref_flag = 0; y_loop_min = 0; y_loop_max = 0; }
      if( tempo==78 ){ strcpy( use_pat_name, "flash_boot_jmp_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==782 ){ strcpy( use_pat_name, "flash_boot_jmp2_v001" ); time_out = W1SEC*30; y_step = 4; }
      if( tempo==783 ){ strcpy( use_pat_name, "flash_boot_jmp3_v001" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo==784 ){ strcpy( use_pat_name, "flash_boot_jmp4_v001" ); time_out = W1SEC*7; y_step = 1; }
      if( tempo==79 ){ strcpy( use_test_name, "UserBoot TM18 Test" ); time_out = W1SEC*7; y_step = 4; }
      if( tempo!=0 ) tempo0 = tempo;


    in_ldata[7] = ((ctr_typ<<8)&0x00FF00) | ( ctr_exp&0x00FF );
    in_ldata[12] = flwt;

    dbg_flag = 0;
    if( tempo0==783 || tempo0==784 ){
      for( n = 0; n<7; n++ ) in_ldata[ n ] = 0x55345678;
      n = 0;
      do{
        printf( "No Change=0  ETLR Change=1  ETCR Change=2   => " ); //DigitInput( &tempo2 );
        tempo2 = 0;
        if( tempo2==1 || tempo2==2 ){
          tempo2 = ( tempo2<<24 ) & 0xFF000000;
          printf( "Reg No ? ETLR=0-39  ETCR=0-11   No => " ); DigitInput( &tempo3 );
          tempo2 = tempo2 | ( ( tempo3<<16 ) & 0x00FF0000 );
          printf( "Reg Data 0xXX => " ); DigitInput( &tempo3 );
          tempo2 = tempo2 | ( tempo3&0x000000FF );
          in_ldata[ n ] = tempo2;
          n++;
          if( n>6 )  tempo1 = 0;
          else       tempo1 = 1;
        }else{
          tempo1 = 0;
        }
      }while( tempo1 );
      if( n ) dbg_flag = 1;
    }

    /*-----------------------*/
    /* ログファイル設定      */
    /*-----------------------*/
    strcpy( baselog, glob_cdp );
    sprintf( filenamebuf, "\\DATALOG\\DATA.LOG_Fmax_%d_%d.csv", vs[0], get_site_number() );
    time( &stime_list );
    strcat( baselog, filenamebuf );
    if( ( Fptdata = fopen( baselog, "at" ) )==NULL ){ printf( " -- FAIL -- Not Open file:%s !\n", baselog ); return( FAIL ); }

    DataOut2( 2, Fptdata, "\n-------------------------------------------\n" );
    sprintf( outbuf, "%s\n", sample_name ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "-------------------------------------------\n" );
    sprintf( outbuf, "Ta = %d 'C\n", test_temperature ); DataOut2( 2, Fptdata, outbuf );

    sprintf( outbuf, "\n title   (change no) %d", tempo0 ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n EXPECT PATTERN (10) | " ); if( ctr_exp==0 ) DataOut2( 2, Fptdata, "All0" ); else if( ctr_exp==1 ) DataOut2( 2, Fptdata, "All1" ); else if( ctr_exp==2 ) DataOut2( 2, Fptdata, "CheckerA" ); else if( ctr_exp==3 ) DataOut2( 2, Fptdata, "CheckerB" ); else DataOut2( 2, Fptdata, "None" );
    DataOut2( 2, Fptdata, "\n Read Type      (20) | " ); if( ctr_typ&0x01 ) DataOut2( 2, Fptdata, "Normal " ); else if( ctr_typ&0x02 ) DataOut2( 2, Fptdata, "Ping Pong 8 Read X-X+1 " ); else if( ctr_typ&0x04 ) DataOut2( 2, Fptdata, "Complemental X X+1 " ); else DataOut2( 2, Fptdata, "None" );
    sprintf( outbuf, "\n FLWT           (60) | wait %d cyc", flwt ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Extal          (70) | %d ps", extal_ps ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Pattern             | %s", use_pat_name ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Sample Name    (1)  | %s", sample_name ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Temperature    (2)  | %d'C", test_temperature ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Target         (3)  | Area[%d]  Address[%08X]-[%08X]", in_ldata[4], in_ldata[5], in_ldata[6] ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n Range          (4)  | Exvref[ " ); if( exvref_flag ) DataOut2( 2, Fptdata, "ON ]" ); else DataOut2( 2, Fptdata, "OFF ]" ); sprintf( outbuf, " Ystep[%d-%d] Vdd/Vddh[%d-%d]mV", y_loop_min, y_loop_max, vdd_table[y_loop_min], vdd_table[y_loop_max] ); DataOut2( 2, Fptdata, outbuf );
    sprintf( outbuf, "\n Voltage        (5)  | VCC=%dmV  ", vs[0] ); DataOut2( 2, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, " Vdd=%dmV ", vdd_const_flag ); DataOut2( 2, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, " Vddh=%dmV ", vddh_const_flag ); DataOut2( 2, Fptdata, outbuf ); }
    sprintf( outbuf, "\n Freq n max39   (6)  | %d - %d", in_ldata[8], in_ldata[9] ); DataOut2( 2, Fptdata, outbuf );
    DataOut2( 2, Fptdata, "\n\n" );

    /*-----------------------*/
    /* shmooパラメータ初期化 */
    /*-----------------------*/
    x_label[0] = y_label[0] = 0;
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      y_label[y] = vdd_table[ y ];
      fail_base[y] = fail_point[y] = fail_contents[y] = 0x12345678;
      fail_data[y][0] = fail_data[y][1] = fail_data[y][2] = fail_data[y][3] = fail_data[y][4] = fail_data[y][5] = fail_data[y][6] = fail_data[y][7] = 0x12345678;
    }
    SetValue( IN_EXTAL_PS, param, extal_ps ); /* def 100ns */

    /***********************************************************************/
    /*******  vdd/vddh Y loop start  ***************************************/
    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      if( CheckHitKey() ){ printf( "\n Escape(1) / Next(0) --> " ); DigitInput( &break_flag ); }
      if( break_flag )  break;

      if( -1==vddh_const_flag ) vs[1] = y_label[ y ]; else vs[1] = vddh_const_flag;
      if( -1==vdd_const_flag )  vs[2] = /*y_label[ y ]*/vddh_500_table[y]; else vs[2] = vdd_const_flag;
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 2, Fptdata, outbuf );

      /*-------------*/
      /* pattern exe */
      /*-------------*/
      rc = CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], time_out, param, CreatePatFileName( use_pat_name ), use_test_name, &in_ldata[0] );
      pass_freq[y] = ReadEcrMode8( 0x98, SIZE_L );
      fail_freq[y] = ReadEcrMode8( 0x9C, SIZE_L );
      fail_contents[y] = ReadEcrMode8( 0x84, SIZE_L );
      fail_base[y] = ReadEcrMode8( 0xA0, SIZE_L );
      fail_point[y] = ReadEcrMode8( 0xA4, SIZE_L );
      for( tempo = 0; tempo<8; tempo++ ) fail_data[y][tempo] = ReadEcrMode8( 0xB0 + tempo*4, SIZE_L );
      if( tempo0==78 || tempo0==79 || tempo0==782 || tempo0==783 || tempo0==784 ){
        fail_contents[y] = ReadEcrMode8( 0x80, SIZE_L ); fail_base[y] = ReadEcrMode8( 0x84, SIZE_L ); fail_point[y] = ReadEcrMode8( 0x88, SIZE_L );
        for( tempo = 0; tempo<8; tempo++ ) fail_data[y][tempo] = ReadEcrMode8( 0x90 + tempo*4, SIZE_L );
      }
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        freq_t[n] = ReadEcrMode8( 0x3E00 + n*4, SIZE_L );
        greg77_t[n] = ReadEcrMode8( 0x3F00 + n*4, SIZE_L );
        if( y==y_loop_min ){ sprintf( outbuf, ",%3d.%03d", (freq_t[n]/1000)/1000, (freq_t[n]/1000)%1000 ); DataOut2( 2, Fptdata, outbuf ); x_label[n] = freq_t[n]/1000; }
      }
      if( y==y_loop_min ){
        dbg_mosccr = ReadEcrMode8( 0xE4, SIZE_L ); dbg_pwstf = ReadEcrMode8( 0xE8, SIZE_L );
        dbg_flwt = ReadEcrMode8( 0xF4, SIZE_L ); dbg_smder = ReadEcrMode8( 0xD0, SIZE_L ); dbg_sckscr = ReadEcrMode8( 0xD8, SIZE_L );
        x_label[40] = ReadEcrMode8( 0x3E00 + 40*4, SIZE_L );
        x_label[41] = ReadEcrMode8( 0x3F00 + 40*4, SIZE_L );
        DataOut2( 2, Fptdata, ",,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );
      }
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        sprintf( outbuf, ",%3d.%03d", (freq_t[n]/1000)/1000, (freq_t[n]/1000)%1000 ); DataOut2( 2, Fptdata, outbuf );
      }
      DataOut2( 2, Fptdata, "\n" );
      for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
        sprintf( outbuf, ",%08X(%08X)", greg77_t[n], freq_t[n] ); DataOut2( 2, Fptdata, outbuf );
      }
      if( dbg_flag ){
        DataOut2( 2, Fptdata, ",InitReg   ETCR0-17 ETLR0-39,0x3D10-->" );
        for( n = 0x3D10; n<0x3D50; n+=4 ){ sprintf( outbuf, ",%08X", ReadEcrMode8( n, SIZE_L ) ); DataOut2( 2, Fptdata, outbuf ); }
        DataOut2( 2, Fptdata, ",ChangeReg ETCR0-17 ETLR0-39,0x3D50-->" );
        for( n = 0x3D50; n<0x3D90; n+=4 ){ sprintf( outbuf, ",%08X", ReadEcrMode8( n, SIZE_L ) ); DataOut2( 2, Fptdata, outbuf ); }
      }
      sprintf( outbuf, ",%3d.%03d MHz  FAIL   TestTime = %d[%d]   PF(flag)=%08X(%08X)\n", (fail_freq[y]/1000)/1000, (fail_freq[y]/1000)%1000, Cputotal, Cpuferr, ReadEcrMode8( 0x80, SIZE_L ), ReadEcrMode8( 0x84, SIZE_L ) ); DataOut2( 3, Fptdata, outbuf );
    }
    /*******  vdd Y loop end  **********************************************/
    /***********************************************************************/

    /**********************************/
    /* after result log out & display */
    /**********************************/

    DataOut2( 3, Fptdata, "\n=================================================================================\n" );
    sprintf( outbuf, "%s    Ta = %d\n", sample_name, test_temperature ); DataOut2( 3, Fptdata, outbuf );
    GetDateTime( &stime_list, stime_buf, 0 ); time( &etime_list ); GetDateTime( &etime_list, etime_buf, 0 );
    sprintf( outbuf, "Date %s - %s\n", stime_buf, etime_buf ); DataOut2( 3, Fptdata, outbuf );
//    sprintf( outbuf, "lot=%-10.10s , wafer=%-2.2s , X=%3d , Y=%3d \n", lotname, wafername, xy_location[0], xy_location[1] ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, "wafer=%-2.2s , X=%3d , Y=%3d \n", wafername, xy_location[0], xy_location[1] ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, " PAT = [%s] Extal=%dps VCC=%dmV", use_pat_name, extal_ps, vs[0] ); DataOut2( 3, Fptdata, outbuf ); if( -1!=vdd_const_flag ){ sprintf( outbuf, "  Vdd=%dmV ", vdd_const_flag ); DataOut2( 3, Fptdata, outbuf ); } if( -1!=vddh_const_flag ){ sprintf( outbuf, "  Vddh=%dmV", vddh_const_flag ); DataOut2( 3, Fptdata, outbuf ); } DataOut2( 3, Fptdata, "\n" );
    sprintf( outbuf, " 1st Test PWSTF=%02X MOSCCR=%02X  FLWT=%02X SMDER=%04X SCKSCR=%02X\n", dbg_pwstf, dbg_mosccr, dbg_flwt, dbg_smder, dbg_sckscr ); DataOut2( 3, Fptdata, outbuf );
    sprintf( outbuf, " Trim 64MHz = %08X = %3d.%03d MHz\n", x_label[41], ( x_label[40]/1000 )/1000, ( x_label[40]/1000 )%1000 ); DataOut2( 3, Fptdata, outbuf );
    
    DataOut2( 3, Fptdata, "\nFreq=" );
    for( n = in_ldata[8]; n<=in_ldata[9]; n++ ){
      sprintf( outbuf, ",%3d.%03d", x_label[n]/1000, x_label[n]%1000 ); DataOut2( 3, Fptdata, outbuf );
    }
    DataOut2( 3, Fptdata, "\n,,pass freq,fail freq,,,contents,base,point,read1,read2,read3,read4,read5,read6,read7,read8\n" );
    if( -1!=vdd_const_flag ){ sprintf( outbuf, "vdd=[%dmV]", vdd_const_flag ); DataOut2( 3, Fptdata, outbuf ); }else{ DataOut2( 3, Fptdata, "vdd" ); }
    if( -1!=vddh_const_flag ){ sprintf( outbuf, "/vddh=[%dmV]", vddh_const_flag ); DataOut2( 3, Fptdata, outbuf ); }else{ DataOut2( 3, Fptdata, "/vddh" ); }

    for( y = y_loop_min; y<=y_loop_max; y+=y_step ){
      sprintf( outbuf, "\n%3d.%03d", y_label[y]/1000, y_label[y]%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, " / %3d.%03d", vddh_500_table[y]/1000, vddh_500_table[y]%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, ",,%3d.%03d,%3d.%03d", (pass_freq[y]/1000)/1000, (pass_freq[y]/1000)%1000, (fail_freq[y]/1000)/1000, (fail_freq[y]/1000)%1000 ); DataOut2( 3, Fptdata, outbuf );
      sprintf( outbuf, ",,,0x%08X,0x%08X,0x%08X", fail_contents[y], fail_base[y], fail_point[y] ); DataOut2( 3, Fptdata, outbuf );
      for( tempo = 0; tempo<8; tempo++ ){ sprintf( outbuf, ",0x%08X", fail_data[y][tempo] ); DataOut2( 3, Fptdata, outbuf ); }
    }
    DataOut2( 3, Fptdata, "\n\n\n=================================================================================\n" );
    fclose( Fptdata );

  v_memcpy( (void*)param, (void*)param_back, sizeof(interface_t)*TABLE_END );    /* input param restore */

  return(PASS);
}

int InLdataSet_Reg(int RegKind, int RegNum, int RegData){
  int result;
  result = (( RegKind<<24 ) & 0xFF000000) | (( RegNum<<16 ) & 0x00FF0000 ) | ( RegData&0x000000FF );
  return(result);
}