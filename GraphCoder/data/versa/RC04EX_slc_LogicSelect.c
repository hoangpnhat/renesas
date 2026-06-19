// ****************************************************************************
// ! @file RX200slc_LogicSelect.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the V3300,4000,5000 test system @author S.Yamaoka @date 2010.07.09 Rev.00
// ****************************************************************************

//int LogicSelect( int *[], int, interface_t* );


int LogicSelect( int vs[NUM_POWERSUPPLY], int waittime, interface_t* param ){

  interface_t param_back[TABLE_END];

  int tempo1, tempo2, i;
  unsigned int cnt, in_cnt, greg66, greg67, greg76, greg77, etlr07_04, start_tap, end_tap, step_tap, shift_flag, in_reg, t_trim_b, logflag;
  FILE_T *fpt2;
  int in_ldata[16+2];
  char file_name[256];
  char datapat[256];
  char time_buf[32], time_buf2[32], dump_filename[256];

    //v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*TABLE_END );    // Store Input/Output Parameter
  v_memcpy( (void*)param_back, (void*)param, sizeof(interface_t)*IN_LWORDEND );
  // input param store

  SetValue( IN_EXTAL1, param, 100 );
  // EXTAL rate(ns)
  tempo1 = 1; logflag = 0; fpt2 = 0;
  while( tempo1!=999 ){
    printf( "[0]=OCO monitor  [-1]=Info1/2->GREG  [-2]=Init GREG  [-3]=No Set GREG  [-4]=TapKeepOff  !! if log file use [0xF10]  or menu 112 before ->log ->2  !!" );
    printf( "[1]=ILOCO        64tap    [10]=ILOCO GREG66 input [11]=ILOCO GREG66 1tap shift [12]=ILOCO GREG66 0tap shift" );
    printf( "[2]=LOCO         64tap    [20]=ETLR07_04   input  [21]=ETLR07_04 1tap shift    [22]=ETLR07_04 0tap shift" );
    printf( "[3]=MOCO         64tap    [30]=ETLR07_04   input  [31]=ETLR07_04 1tap shift    [32]=ETLR07_04 0tap shift" );
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
    printf( "[4]=HOCO 48MHz 1024tap    [40]=HOCO GREG77 input  [41]=HOCO GREG77 1tap shift  [42]=HOCO GREG77 0tap shift" );
    printf( "[5]=HOCO 64MHz 1024tap    [50]=HOCO GREG76 input  [51]=HOCO GREG76 1tap shift  [52]=HOCO GREG76 0tap shift" );
#elif ( PRODUCT_NAME==PRODUCT_IO )
    printf( "[4]=HOCO 48MHz FFI 256tap [40]=HOCO GREG76 input  [41]=HOCO GREG76 1tap shift  [42]=HOCO GREG76 0tap shift" );
    printf( "[5]=HOCO 64MHz FFI 256tap [50]=HOCO GREG77 input  [51]=HOCO GREG77 1tap shift  [52]=HOCO GREG77 0tap shift" );
    printf( "[6]=HVOCO        64tap    [60]=GREG67      input  [61]=GREG67 1tap shift       [62]=GREG67 0tap shift" );
#endif
    printf( "[123456]=Info1/2/3/4/Table1/2 FBM  [123457]=UserMat FBM   fk=%08X", fc_key );
    printf( "[9]=BGR VCL1/2          [90]=BGR DATAI ATBM1 Mon  [91]=ETLR01_00 input BGR ATBM1 Mon" );
    printf( "[98]=Extal Change  [ Other ]=Exit & need tpreload" );
    printf( "enter temp set number->" ); DigitInput( &tempo1 );
    for( i = 0; i<16; i++ ) in_ldata[ i ] = 0x55345678;
    start_tap = 0; end_tap = 0; step_tap = 1;
    shift_flag = 0; in_cnt = 0; in_reg = 0x55345678; t_trim_b = 0;
    switch( tempo1 ){
    case 98:
      printf( "Set Extal [ns] -> " ); DigitInput( &tempo2 );
      SetValue( IN_EXTAL1, param, tempo2 );
      // EXTAL rate(ns)
      break;
    case 9: case 90: case 91: case 92:
      break;
    case 123456: case 123457:
      break;
    case 0xF10:
    // LogFile
      fclose( fpt2 );
      printf( "DATALOG\\ file name input -> " ); StringInput( file_name );
      sprintf( datapat, "%s%s%s_%d.csv", glob_cdp, glob_datalog, file_name, get_site_number() );
      if( NULL==( fpt2 = fopen( datapat, "at" ) ) ){ printf( "OpenDataOutFile Fptdata Open Err[%s]", datapat ); return(FAIL); } logflag = 1;
      break;
    case 0: case -1: case -2: case -3: case -4: case -5: case -6:
      if     (  0==tempo1 ) in_ldata[ 4 ] = 0x00080001;
      else if( -1==tempo1 ) in_ldata[ 4 ] = 0x10080000;
      else if( -2==tempo1 ) in_ldata[ 4 ] = 0x20080000;
      else if( -3==tempo1 ) in_ldata[ 4 ] = 0x00080000;
      else if( -4==tempo1 ) in_ldata[ 4 ] = 0x00180001;
      break;
    case 1: case 10: case 11: case 12:
      in_ldata[ 4 ] = 0x00080101;
      in_cnt= 6;
      if( 1==tempo1 )                    { start_tap = 0x00000000; end_tap = 0x003F0000; step_tap = 0x00010000; }
      else if( 11==tempo1 || 12==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 10==tempo1 ){ printf( "GREG66 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
    case 2: case 20: case 21: case 22:
      in_ldata[ 4 ] = 0x00080801;
      in_cnt= 9;
      if( 2==tempo1 )                    { start_tap = 0x00000000; end_tap = 0x0007E000; step_tap = 0x00002000; }
      else if( 21==tempo1 || 22==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 20==tempo1 ){ printf( "ETLR07_04 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
    case 3: case 30: case 31: case 32:
      in_ldata[ 4 ] = 0x00080801;
      in_cnt= 9;
      if( 3==tempo1 )                    { start_tap = 0x00000000; end_tap = 0x00001F80; step_tap = 0x00000080; }
      else if( 31==tempo1 || 32==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 30==tempo1 ){ printf( "ETLR07_04 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
    case 4: case 40: case 41: case 42:
      in_ldata[ 4 ] = 0x00080401;
      in_cnt= 8;
      if( 4==tempo1 ){
        printf( "T_trimB[3:0] data set 0xX -> " ); DigitInput( &in_reg ); in_reg = ( in_reg<<20 )&0x00F00000;
        start_tap = 0x000003FF | in_reg; end_tap = 0x000FFFFF | in_reg; step_tap = 0x00000400; in_reg = 0x55345678;
      }
      else if( 41==tempo1 || 42==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 40==tempo1 ){ printf( "GREG77 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
    case 5: case 50: case 51: case 52:
      in_ldata[ 4 ] = 0x00080201;
      in_cnt= 7;
      if( 5==tempo1 ){
        printf( "T_trimB[3:0] data set 0xX -> " ); DigitInput( &in_reg ); in_reg = ( in_reg<<20 )&0x00F00000;
        start_tap = 0x000003FF | in_reg; end_tap = 0x000FFFFF | in_reg; step_tap = 0x00000400; in_reg = 0x55345678;
      }
      else if( 51==tempo1 || 52==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 50==tempo1 ){ printf( "GREG76 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
#elif ( PRODUCT_NAME==PRODUCT_IO )
    case 4: case 40: case 41: case 42:
      in_ldata[ 4 ] = 0x00080201;
      in_cnt= 7;
      if( 4==tempo1 ){
        printf( "KT1TRIM[5:0] data set 0-63 -> " ); DigitInput( &in_reg ); in_reg = ( in_reg<<11 )&0x0001F800;
        in_reg |= 0x300;
        // FCO=1 / FADJ=100
        start_tap = 0x0001FFFF | in_reg; end_tap = 0x01FE0000 | in_reg; step_tap = 0x00020000; in_reg = 0x55345678;
      }
      else if( 41==tempo1 || 42==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 40==tempo1 ){ printf( "GREG76 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
    case 5: case 50: case 51: case 52:
      in_ldata[ 4 ] = 0x00080401;
      in_cnt= 8;
      if( 5==tempo1 ){
        printf( "KT1TRIM[5:0] data set 0-63 -> " ); DigitInput( &in_reg ); in_reg = ( in_reg<<11 )&0x0001F800;
        in_reg |= 0x500;
        // FCO=2 / FADJ=100
        start_tap = 0x0001FFFF | in_reg; end_tap = 0x01FE0000 | in_reg; step_tap = 0x00020000; in_reg = 0x55345678;
      }
      else if( 51==tempo1 || 52==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 50==tempo1 ){ printf( "GREG77 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
#endif
    case 6: case 60: case 61: case 62:
      in_ldata[ 4 ] = 0x00084001;
      in_cnt= 10;
      if( 6==tempo1 )                    { start_tap = 0x00000000; end_tap = 0x3F000000; step_tap = 0x01000000; }
      else if( 61==tempo1 || 62==tempo1 ){ start_tap = 0; end_tap = 32; step_tap = 1; shift_flag = 1; }
      else if( 60==tempo1 ){ printf( "GREG67 data set 0xXXXXXXXX -> " ); DigitInput( &in_reg ); } break;
    default:
      tempo1 = 999;
      break;
    }

    if( tempo1==999 || tempo1==98 ){
    }else if( tempo1==9 || tempo1==90 || tempo1==91 ){
      if( tempo1==91 ){ printf( "ETLR01_00 data set 0xXXXX -> " ); DigitInput( &in_reg ); in_ldata[ 6 ] = in_reg; in_ldata[ 4 ] = 0x00000007; } CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], W1SEC*2, param, CreatePatFileName("bgr_monitor_v002"),"BGR Monitor", &in_ldata[0] );
    }else if( tempo1==123456 || tempo1==123457 ){
      for( cnt = 0; cnt<=6; cnt++ ){
        if( cnt==0 && tempo1==123456 ) continue;
        if( cnt==1 && tempo1==123457 ) break;
        time( &time_list2 ); GetDateTime( &time_list2, time_buf2, 1 );
        in_ldata[4] = cnt; tempo2 = 0x400;
        sprintf( dump_filename, "%s_L%s_W%s_x%03d_y%03d_Info%d_%s_%02d%02d%02d_%d.bin", devicename, lotname, wafername, xy_location[0], xy_location[1], cnt, &info_extra[cnt][0], thour, tmin, tsec, get_site_number() );
        switch( cnt ){
        case 0:
          sprintf( dump_filename, "%s_L%s_W%s_x%03d_y%03d_User_%s_%02d%02d%02d_%d.bin", devicename, lotname, wafername, xy_location[0], xy_location[1], &info_extra[cnt][0], thour, tmin, tsec, get_site_number() );
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
                in_ldata[5] = 0x00000000; in_ldata[6] = 0x00180000; tempo2 = 0x00020000; break;
#elif ( PRODUCT_NAME==PRODUCT_IO )
                in_ldata[5] = 0x00000000; in_ldata[6] = 0x00040000; tempo2 = 0x00010000; break;
#endif
        case 1: in_ldata[5] = 0x01008000; in_ldata[6] = 0x01008400; tempo2 = 0x400; break;
        case 2: in_ldata[5] = 0x0100A000; in_ldata[6] = 0x0100A400; tempo2 = 0x400; break;
        case 3: in_ldata[5] = 0x0100C000; in_ldata[6] = 0x0100C400; tempo2 = 0x400; break;
        case 4: in_ldata[5] = 0x01000000; in_ldata[6] = 0x01003000; tempo2 = 0x3000; break;
        case 5: in_ldata[5] = 0x01010000; in_ldata[6] = 0x01010400; tempo2 = 0x400; break;
        case 6: in_ldata[5] = 0x01012000; in_ldata[6] = 0x01012400; tempo2 = 0x400; break;
        default: break;
        }
        in_ldata[8] = tempo2;
        if( 0==cnt ){
          while( in_ldata[5]<in_ldata[6] ){
            CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], W1SEC*1, param, CreatePatFileName("read_dump_v001"),"Memory Read FBM", &in_ldata[0] );
            copy_bmecr_offset( ecr1Memory, 0x8000, tempo2, ecr1Memory, 0x40000 + in_ldata[5] );
            in_ldata[5] += tempo2;
          }
          FbmDataOut_BIST( 0x40000, in_ldata[6], dump_filename );
        }else{
          CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], W1SEC*1, param, CreatePatFileName("read_dump_v001"),"Memory Read FBM", &in_ldata[0] );
          FbmDataOut_BIST( RAM_TOP + 0x8000, tempo2, dump_filename );
        }
      }
      // for cnt end
    }else{
      for( cnt = 0; start_tap<=end_tap; start_tap+=step_tap, cnt++ ){
        if( shift_flag==1 ){
          in_ldata[ in_cnt ] = 0x00000001<<cnt;
          if( 2==tempo1%10 ) in_ldata[ in_cnt ] = ~in_ldata[ in_cnt ];
        }else{
          if( in_cnt!=0 ){
            if( in_reg != 0x55345678 ) in_ldata[ in_cnt ] = in_reg;
            else                       in_ldata[ in_cnt ] = start_tap;
          }
        }
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
        CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], W1SEC*1, param, CreatePatFileName("oco_monitor_v004"),"OCO Monitor", &in_ldata[0] );
#elif ( PRODUCT_NAME==PRODUCT_IO )
        CpuModeFuncLogic( MNORMAL, MSNORMAL, &vs[0], W1SEC*1, param, CreatePatFileName("oco_monitor_io_v001"),"OCO Monitor", &in_ldata[0] );
#endif
        greg66 = ReadEcrMode8( RAM_TOP + 0xA0, SIZE_L ); greg67 = ReadEcrMode8( RAM_TOP + 0xE4, SIZE_L ); greg76 = ReadEcrMode8( RAM_TOP + 0xA4, SIZE_L ); greg77 = ReadEcrMode8( RAM_TOP + 0xA8, SIZE_L ); etlr07_04 = ReadEcrMode8( RAM_TOP + 0xAC, SIZE_L );
        if( cnt==0 ){  sprintf( outbuf, "ETLR01_00=%04X,   SubOSC Trim=%02X,   BGRADJT=%02X,   BGRTRMV=%02X", ReadEcrMode8( RAM_TOP + 0x8C, SIZE_W ), ReadEcrMode8( RAM_TOP + 0x8A, SIZE_B ), ReadEcrMode8( RAM_TOP + 0x88, SIZE_B ), ReadEcrMode8( RAM_TOP + 0x89, SIZE_B ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 ){ DataOut( Ffpt, " OCO meas " ); if( logflag ) DataOut2( 2, fpt2, " OCO meas " ); }
        if( cnt==0 ){ DataOut( Ffpt,   "Name           ,Trim     ,count    ,Freq[Hz]\n" ); if( logflag ) DataOut2( 2, fpt2, "                                     ,Name           ,Trim     ,count    ,Freq[Hz]\n" ); }
//        if( cnt==0 ){ sprintf( outbuf, "ccc   2.048KHz ,         ,%08X     ,%9d  , -1 skip / other MOCO use", ReadEcrMode8( RAM_TOP + 0xD8, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xDC, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 || tempo1==1 || tempo1==11 || tempo1==12 ){ s
        if( cnt==0 || tempo1==2 || tempo1==21 || tempo1==22 ){ sprintf( outbuf, "LOCO 32.768KHz ,%4X     ,%08X     ,%9d", ( etlr07_04&0x0007E000 )>>13,  ReadEcrMode8( RAM_TOP + 0xC8, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xCC, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 || tempo1==3 || tempo1==31 || tempo1==32 ){ s
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
        if( cnt==0 || tempo1==4 || tempo1==41 || tempo1==42 ){ sprintf( outbuf, "HOCO 48MHz     ,%8X ,%08X     ,%9d", ( greg77&0x00FFFC00 )>>10, ReadEcrMode8( RAM_TOP + 0xB0, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xB4, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 || tempo1==5 || tempo1==51 || tempo1==52 ){ s
#elif ( PRODUCT_NAME==PRODUCT_IO )
        if( cnt==0 || tempo1==4 || tempo1==41 || tempo1==42 ){ sprintf( outbuf, "HOCO 48MHz     ,%8X ,%08X     ,%9d", ( greg76&0x01FFFFFF ), ReadEcrMode8( RAM_TOP + 0xB0, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xB4, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 || tempo1==5 || tempo1==51 || tempo1==52 ){ s
        if( cnt==0 || tempo1==6 || tempo1==61 || tempo1==62 ){ sprintf( outbuf, "HVOCO 2MHz     ,%4X     ,%08X     ,%9d", ( greg67&0x3F000000 )>>24, ReadEcrMode8( RAM_TOP + 0xD8, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xDC, SIZE_L ) ); DataOut( Ffpt, outbuf ); } #endif if( cnt==0 || tempo1==2 || tempo1==21 || tempo1==22 || tempo1==3 || tempo1==31 || tempo1==32 ){ s
        if( cnt==0 || tempo1==1 || tempo1==11 || tempo1==12 ){ sprintf( outbuf, "%4d  GREG66    = %08X -> %08X  ", cnt, ReadEcrMode8( RAM_TOP + 0x90, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xA0, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( cnt==0 || tempo1==5 || tempo1==51 || tempo1==52 ){ s
        if( cnt==0 || tempo1==4 || tempo1==41 || tempo1==42 ){ sprintf( outbuf, "%4d  GREG77    = %08X -> %08X  ", cnt, ReadEcrMode8( RAM_TOP + 0x98, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xA8, SIZE_L ) ); DataOut( Ffpt, outbuf ); } if( logflag ){ if( cnt==0 || tempo1==1 || tempo1==11 || tempo1==12 ){ s
          if( cnt==0 || tempo1==2 || tempo1==21 || tempo1==22 ){ sprintf( outbuf, "%4d  ETLR07_04 = %08X -> %08X ,LOCO 32.768KHz ,%4X     ,%08X     ,%9d", cnt, ReadEcrMode8( RAM_TOP + 0x9C, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xAC, SIZE_L ), ( etlr07_04&0x0007E000 )>>13,  ReadEcrMode8( RAM_TOP + 0xC8, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xCC, SIZE_L ) ); DataOut2( 2, fpt2, outbuf ); } if( cnt==0 || tempo1==3 || tempo1==31 || tempo1==32 ){ s
          if( cnt==0 || tempo1==4 || tempo1==41 || tempo1==42 ){ sprintf( outbuf, "%4d  GREG77    = %08X -> %08X ,HOCO 48MHz     ,%8X ,%08X     ,%9d", cnt, ReadEcrMode8( RAM_TOP + 0x98, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xA8, SIZE_L ), ( greg77&0x00FFFC00 )>>10, ReadEcrMode8( RAM_TOP + 0xB0, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xB4, SIZE_L ) ); DataOut2( 2, fpt2, outbuf ); } if( cnt==0 || tempo1==5 || tempo1==51 || tempo1==52 ){ s
          if( cnt==0 || tempo1==6 || tempo1==61 || tempo1==62 ){ sprintf( outbuf, "%4d  GREG67    = %08X -> %08X ,HVOCO 2MHz     ,%4X     ,%08X     ,%9d", cnt, ReadEcrMode8( RAM_TOP + 0xE0, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xE4, SIZE_L ), ( greg67&0x3F000000 )>>24, ReadEcrMode8( RAM_TOP + 0xD8, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xDC, SIZE_L ) ); DataOut2( 2, fpt2, outbuf ); } } #if ( PRODUCT_NAME==PRODUCT_IO ) if( cnt==0 || tempo1==4 || tempo1==41 || tempo1==42 ){ s
        if( cnt==0 || tempo1==5 || tempo1==51 || tempo1==52 ){ sprintf( outbuf, "WT1 KT32 64MHz ,%8X ,%08X     ,%9d", ( ReadEcrMode8( RAM_TOP + 0xFC, SIZE_L )&0x01FFFFFF ), ReadEcrMode8( RAM_TOP + 0xF0, SIZE_L ), ReadEcrMode8( RAM_TOP + 0xF4, SIZE_L ) ); DataOut( Ffpt, outbuf ); } #endif } } }   fclose( fpt2 );
//  v_memcpy( (void*)param, (void*)param_back, sizeof(interface_t)*TABLE_END );    // ReStore Input/Output Parameter
  v_memcpy( (void*)param, (void*)param_back, sizeof(interface_t)*IN_LWORDEND );
  // input param restore

  return(PASS);
}

