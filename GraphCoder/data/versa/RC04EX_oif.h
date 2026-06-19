// ****************************************************************************
// ! @file RX200oif.h @brief  RC01SN-FMONOS @author K.Tanabe @date 2007.01.15 Rev.01 $LastChangedDate: 2007-07-19 16:46:40 +0900 (, 19 7 2007) $ $Revision: 340 $
// ****************************************************************************
#ifndef OIF_H
#define OIF_H
// ----------------------------------------------------------------------------
// define
// ----------------------------------------------------------------------------
#include "RC04EX_iif.h"

// ----------------------------------------------------------------------------
// define
// ----------------------------------------------------------------------------
typedef enum{
                                                                                // ETLRWX^
  OUT_ETLR00=IN_LWORDEND+1,
  // !  ETLR00WX^̒l
  OUT_ETLR01,
  // !  ETLR01WX^̒l
  OUT_ETLR02,
  // !  ETLR02WX^̒l
  OUT_ETLR03,
  // !  ETLR03WX^̒l
  OUT_ETLR04,
  // !  ETLR04WX^̒l
  OUT_ETLR05,
  // !  ETLR05WX^̒l
  OUT_ETLR06,
  // !  ETLR06WX^̒l
  OUT_ETLR07,
  // !  ETLR07WX^̒l
  OUT_ETLR08,
  // !  ETLR08WX^̒l
  OUT_ETLR09,
  // !  ETLR09WX^̒l
  OUT_ETLR10,
  // !  ETLR10WX^̒l
  OUT_ETLR11,
  // !  ETLR11WX^̒l
  OUT_ETLR12,
  // !  ETLR12WX^̒l
  OUT_ETLR13,
  // !  ETLR13WX^̒l
  OUT_ETLR14,
  // !  ETLR14WX^̒l
  OUT_ETLR15,
  // !  ETLR15WX^̒l
  OUT_ETLR16,
  // !  ETLR16WX^̒l
  OUT_ETLR17,
  // !  ETLR17WX^̒l
  OUT_ETLR18,
  // !  ETLR18WX^̒l
  OUT_ETLR19,
  // !  ETLR19WX^̒l
  OUT_ETLR20,
  // !  ETLR20WX^̒l
  OUT_ETLR21,
  // !  ETLR21WX^̒l
  OUT_ETLR22,
  // !  ETLR22WX^̒l
  OUT_ETLR23,
  // !  ETLR23WX^̒l
  OUT_ETLR24,
  // !  ETLR24WX^̒l
  OUT_ETLR25,
  // !  ETLR25WX^̒l
  OUT_ETLR26,
  // !  ETLR26WX^̒l
  OUT_ETLR27,
  // !  ETLR27WX^̒l
  OUT_ETLR28,
  // !  ETLR28WX^̒l
  OUT_ETLR29,
  // !  ETLR29WX^̒l
  OUT_ETLR30,
  // !  ETLR30WX^̒l
  OUT_ETLR31,
  // !  ETLR31WX^̒l
  OUT_ETLR32,
  // !  ETLR32WX^̒l
  OUT_ETLR33,
  // !  ETLR33WX^̒l
  OUT_ETLR34,
  // !  ETLR34WX^̒l
  OUT_ETLR35,
  // !  ETLR35WX^̒l
  OUT_ETLR36,
  // !  ETLR36WX^̒l
  OUT_ETLR37,
  // !  ETLR37WX^̒l
  OUT_ETLR38,
  // !  ETLR38WX^̒l
  OUT_ETLR39,
  // !  ETLR39WX^̒l
  
                                                                                // ETCRWX^
  OUT_ETCR00,
  // !  ETCR00WX^̒l
  OUT_ETCR01,
  // !  ETCR01WX^̒l
  OUT_ETCR02,
  // !  ETCR02WX^̒l
  OUT_ETCR03,
  // !  ETCR03WX^̒l
  OUT_ETCR04,
  // !  ETCR04WX^̒l
  OUT_ETCR05,
  // !  ETCR05WX^̒l
  OUT_ETCR06,
  // !  ETCR06WX^̒l
  OUT_ETCR07,
  // !  ETCR07WX^̒l
  OUT_ETCR08,
  // !  ETCR08WX^̒l
  OUT_ETCR09,
  // !  ETCR09WX^̒l
  OUT_ETCR10,
  // !  ETCR10WX^̒l
  OUT_ETCR11,
  // !  ETCR11WX^̒l
  OUT_ETCR12,
  // !  ETCR12WX^̒l
  OUT_ETCR13,
  // !  ETCR13WX^̒l
  OUT_ETCR14,
  // !  ETCR14WX^̒l
  OUT_ETCR15,
  // !  ETCR15WX^̒l
  OUT_ETCR16,
  // !  ETCR16WX^̒l
  
  OUT_ETCR17,
  // !  3l

  OUT_JUDGE1,
  // !  茋
  OUT_JUDGE2,
  // !  茋
  OUT_BDATA00,
  // !  ėpByte^f[^ No.00
  OUT_BDATA01,
  // !  ėpByte^f[^ No.01
  OUT_BDATA02,
  // !  ėpByte^f[^ No.02
  OUT_BDATA03,
  // !  ėpByte^f[^ No.03
  OUT_BDATA04,
  // !  ėpByte^f[^ No.04
  OUT_BDATA05,
  // !  ėpByte^f[^ No.05
  OUT_BDATA06,
  // !  ėpByte^f[^ No.06
  OUT_BDATA07,
  // !  ėpByte^f[^ No.07
  OUT_BDATA08,
  // !  ėpByte^f[^ No.08
  OUT_BDATA09,
  // !  ėpByte^f[^ No.09
  OUT_BDATA10,
  // !  ėpByte^f[^ No.10
  OUT_BDATA11,
  // !  ėpByte^f[^ No.11
  OUT_BDATA12,
  // !  ėpByte^f[^ No.12
  OUT_BDATA13,
  // !  ėpByte^f[^ No.13
  OUT_BDATA14,
  // !  ėpByte^f[^ No.14
  OUT_BDATA15,
  // !  ėpByte^f[^ No.15
  
  OUT_BYTEEND,
  // !< Byte^f[^̏I[
  
  OUT_LDATA0,
  // !  ėpLong Byte^f[^ No.0
  OUT_LDATA1,
  // !  ėpLong Byte^f[^ No.1
  OUT_LDATA2,
  // !  ėpLong Byte^f[^ No.2
  OUT_LDATA3,
  // !  ėpLong Byte^f[^ No.3
  OUT_LDATA4,
  // !  ėpLong Byte^f[^ No.4
  
  OUT_LWORDEND
  // !< LongWord^f[^̏I[

} OutputIFPointer;

#define OUT_TERMINATE  0xFFFF

// ----------------------------------------------------------------------------
// struct
// ----------------------------------------------------------------------------
struct testresult{

  Byte  ETLR00;
  // !  ETLR00
  Byte  ETLR01;
  // !  ETLR01
  Byte  ETLR02;
  // !  ETLR02
  Byte  ETLR03;
  // !  ETLR03
  Byte  ETLR04;
  // !  ETLR04
  Byte  ETLR05;
  // !  ETLR05
  Byte  ETLR06;
  // !  ETLR06
  Byte  ETLR07;
  // !  ETLR07
  Byte  ETLR08;
  // !  ETLR08
  Byte  ETLR09;
  // !  ETLR09
  Byte  ETLR10;
  // !  ETLR10
  Byte  ETLR11;
  // !  ETLR11
  Byte  ETLR12;
  // !  ETLR12
  Byte  ETLR13;
  // !  ETLR13
  Byte  ETLR14;
  // !  ETLR14
  Byte  ETLR15;
  // !  ETLR15
  Byte  ETLR16;
  // !  ETLR16
  Byte  ETLR17;
  // !  ETLR17
  Byte  ETLR18;
  // !  ETLR18
  Byte  ETLR19;
  // !  ETLR19
  Byte  ETLR20;
  // !  ETLR20
  Byte  ETLR21;
  // !  ETLR21
  Byte  ETLR22;
  // !  ETLR22
  Byte  ETLR23;
  // !  ETLR23
  Byte  ETLR24;
  // !  ETLR24
  Byte  ETLR25;
  // !  ETLR25
  Byte  ETLR26;
  // !  ETLR26
  Byte  ETLR27;
  // !  ETLR27
  Byte  ETLR28;
  // !  ETLR28
  Byte  ETLR29;
  // !  ETLR29
  Byte  ETLR30;
  // !  ETLR30
  Byte  ETLR31;
  // !  ETLR31
  Byte  ETLR32;
  // !  ETLR32
  Byte  ETLR33;
  // !  ETLR33
  Byte  ETLR34;
  // !  ETLR34
  Byte  ETLR35;
  // !  ETLR35
  Byte  ETLR36;
  // !  ETLR36
  Byte  ETLR37;
  // !  ETLR37
  Byte  ETLR38;
  // !  ETLR38
  Byte  ETLR39;
  // !  ETLR39
  Byte  ETCR00;
  // !  ETCR00
  Byte  ETCR01;
  // !  ETCR01
  Byte  ETCR02;
  // !  ETCR02
  Byte  ETCR03;
  // !  ETCR03
  Byte  ETCR04;
  // !  ETCR04
  Byte  ETCR05;
  // !  ETCR05
  Byte  ETCR06;
  // !  ETCR06
  Byte  ETCR07;
  // !  ETCR07
  Byte  ETCR08;
  // !  ETCR08
  Byte  ETCR09;
  // !  ETCR09
  Byte  ETCR10;
  // !  ETCR10
  Byte  ETCR11;
  // !  ETCR11
  Byte  ETCR12;
  // !  ETCR12
  Byte  ETCR13;
  // !  ETCR13
  Byte  ETCR14;
  // !  ETCR14
  Byte  ETCR15;
  // !  ETCR15
  Byte  ETCR16;
  // !  ETCR16
  Byte  ETCR17;
  // !  3l
  Byte  JUDGE1;
  // !  茋
  Byte  JUDGE2;
  // !  茋
  
  Byte  BDATA00;
  // !  ėpByte^f[^ No.00
  Byte  BDATA01;
  // !  ėpByte^f[^ No.01
  Byte  BDATA02;
  // !  ėpByte^f[^ No.02
  Byte  BDATA03;
  // !  ėpByte^f[^ No.03
  Byte  BDATA04;
  // !  ėpByte^f[^ No.04
  Byte  BDATA05;
  // !  ėpByte^f[^ No.05
  Byte  BDATA06;
  // !  ėpByte^f[^ No.06
  Byte  BDATA07;
  // !  ėpByte^f[^ No.07
  Byte  BDATA08;
  // !  ėpByte^f[^ No.08
  Byte  BDATA09;
  // !  ėpByte^f[^ No.09
  Byte  BDATA10;
  // !  ėpByte^f[^ No.10
  Byte  BDATA11;
  // !  ėpByte^f[^ No.11
  Byte  BDATA12;
  // !  ėpByte^f[^ No.12
  Byte  BDATA13;
  // !  ėpByte^f[^ No.13
  Byte  BDATA14;
  // !  ėpByte^f[^ No.14
  Byte  BDATA15;
  // !  ėpByte^f[^ No.15
  
  Lword  LDATA0;
  // !  ėpLong Byte^f[^ No.0
  Lword  LDATA1;
  // !  ėpLong Byte^f[^ No.1
  Lword  LDATA2;
  // !  ėpLong Byte^f[^ No.2
  Lword  LDATA3;
  // !  ėpLong Byte^f[^ No.3
  Lword  LDATA4;
  // !  ėpLong Byte^f[^ No.4
};

// ****************************************************************************
#endif
