/******************************************************************************/
/*!
  @file RX200oif.h
  @brief  RC01SN-FMONOS
  @author K.Tanabe
  @date 2007.01.15 Rev.01
  $LastChangedDate: 2007-07-19 16:46:40 +0900 (ｨ, 19 7 2007) $
  $Revision: 340 $
*/
/******************************************************************************/
#ifndef OIF_H
#define OIF_H
/*----------------------------------------------------------------------------*/
/* define                                                                     */
/*----------------------------------------------------------------------------*/
#include "RC04EX_iif.h"

/*----------------------------------------------------------------------------*/
/* define                                                                     */
/*----------------------------------------------------------------------------*/
typedef enum{
                                        /*    ETLRレジスタ    */
  OUT_ETLR00=IN_LWORDEND+1,             /*!  ETLR00レジスタの値          */
  OUT_ETLR01,                           /*!  ETLR01レジスタの値          */
  OUT_ETLR02,                           /*!  ETLR02レジスタの値          */
  OUT_ETLR03,                           /*!  ETLR03レジスタの値          */
  OUT_ETLR04,                           /*!  ETLR04レジスタの値          */
  OUT_ETLR05,                           /*!  ETLR05レジスタの値          */
  OUT_ETLR06,                           /*!  ETLR06レジスタの値          */
  OUT_ETLR07,                           /*!  ETLR07レジスタの値          */
  OUT_ETLR08,                           /*!  ETLR08レジスタの値          */
  OUT_ETLR09,                           /*!  ETLR09レジスタの値          */
  OUT_ETLR10,                           /*!  ETLR10レジスタの値          */
  OUT_ETLR11,                           /*!  ETLR11レジスタの値          */
  OUT_ETLR12,                           /*!  ETLR12レジスタの値          */
  OUT_ETLR13,                           /*!  ETLR13レジスタの値          */
  OUT_ETLR14,                           /*!  ETLR14レジスタの値          */
  OUT_ETLR15,                           /*!  ETLR15レジスタの値          */
  OUT_ETLR16,                           /*!  ETLR16レジスタの値          */
  OUT_ETLR17,                           /*!  ETLR17レジスタの値          */
  OUT_ETLR18,                           /*!  ETLR18レジスタの値          */
  OUT_ETLR19,                           /*!  ETLR19レジスタの値          */
  OUT_ETLR20,                           /*!  ETLR20レジスタの値          */
  OUT_ETLR21,                           /*!  ETLR21レジスタの値          */
  OUT_ETLR22,                           /*!  ETLR22レジスタの値          */
  OUT_ETLR23,                           /*!  ETLR23レジスタの値          */
  OUT_ETLR24,                           /*!  ETLR24レジスタの値          */
  OUT_ETLR25,                           /*!  ETLR25レジスタの値          */
  OUT_ETLR26,                           /*!  ETLR26レジスタの値          */
  OUT_ETLR27,                           /*!  ETLR27レジスタの値          */
  OUT_ETLR28,                           /*!  ETLR28レジスタの値          */
  OUT_ETLR29,                           /*!  ETLR29レジスタの値          */
  OUT_ETLR30,                           /*!  ETLR30レジスタの値          */
  OUT_ETLR31,                           /*!  ETLR31レジスタの値          */
  OUT_ETLR32,                           /*!  ETLR32レジスタの値          */
  OUT_ETLR33,                           /*!  ETLR33レジスタの値          */
  OUT_ETLR34,                           /*!  ETLR34レジスタの値          */
  OUT_ETLR35,                           /*!  ETLR35レジスタの値          */
  OUT_ETLR36,                           /*!  ETLR36レジスタの値          */
  OUT_ETLR37,                           /*!  ETLR37レジスタの値          */
  OUT_ETLR38,                           /*!  ETLR38レジスタの値          */
  OUT_ETLR39,                           /*!  ETLR39レジスタの値          */
  
                                        /*    ETCRレジスタ    */  
  OUT_ETCR00,                           /*!  ETCR00レジスタの値          */
  OUT_ETCR01,                           /*!  ETCR01レジスタの値          */
  OUT_ETCR02,                           /*!  ETCR02レジスタの値          */
  OUT_ETCR03,                           /*!  ETCR03レジスタの値          */
  OUT_ETCR04,                           /*!  ETCR04レジスタの値          */
  OUT_ETCR05,                           /*!  ETCR05レジスタの値          */
  OUT_ETCR06,                           /*!  ETCR06レジスタの値          */
  OUT_ETCR07,                           /*!  ETCR07レジスタの値          */
  OUT_ETCR08,                           /*!  ETCR08レジスタの値          */
  OUT_ETCR09,                           /*!  ETCR09レジスタの値          */
  OUT_ETCR10,                           /*!  ETCR10レジスタの値          */
  OUT_ETCR11,                           /*!  ETCR11レジスタの値          */
  OUT_ETCR12,                           /*!  ETCR12レジスタの値          */
  OUT_ETCR13,                           /*!  ETCR13レジスタの値          */
  OUT_ETCR14,                           /*!  ETCR14レジスタの値          */
  OUT_ETCR15,                           /*!  ETCR15レジスタの値          */
  OUT_ETCR16,                           /*!  ETCR16レジスタの値          */
  
  OUT_ETCR17,                           /*!  3値多数決結果               */

  OUT_JUDGE1,                            /*!  判定結果                  */
  OUT_JUDGE2,                            /*!  判定結果                  */
  OUT_BDATA00,                          /*!  汎用Byte型データ No.00      */
  OUT_BDATA01,                          /*!  汎用Byte型データ No.01      */
  OUT_BDATA02,                          /*!  汎用Byte型データ No.02      */
  OUT_BDATA03,                          /*!  汎用Byte型データ No.03      */
  OUT_BDATA04,                          /*!  汎用Byte型データ No.04      */
  OUT_BDATA05,                          /*!  汎用Byte型データ No.05      */
  OUT_BDATA06,                          /*!  汎用Byte型データ No.06      */
  OUT_BDATA07,                          /*!  汎用Byte型データ No.07      */
  OUT_BDATA08,                          /*!  汎用Byte型データ No.08      */
  OUT_BDATA09,                          /*!  汎用Byte型データ No.09      */
  OUT_BDATA10,                          /*!  汎用Byte型データ No.10      */
  OUT_BDATA11,                          /*!  汎用Byte型データ No.11      */
  OUT_BDATA12,                          /*!  汎用Byte型データ No.12      */
  OUT_BDATA13,                          /*!  汎用Byte型データ No.13      */
  OUT_BDATA14,                          /*!  汎用Byte型データ No.14      */
  OUT_BDATA15,                          /*!  汎用Byte型データ No.15      */
  
  OUT_BYTEEND,                          /*!< Byte型データの終端      */
  
  OUT_LDATA0,                           /*!  汎用Long Byte型データ No.0  */
  OUT_LDATA1,                           /*!  汎用Long Byte型データ No.1  */
  OUT_LDATA2,                           /*!  汎用Long Byte型データ No.2  */
  OUT_LDATA3,                           /*!  汎用Long Byte型データ No.3  */
  OUT_LDATA4,                           /*!  汎用Long Byte型データ No.4  */
  
  OUT_LWORDEND                          /*!< LongWord型データの終端      */

} OutputIFPointer;

#define OUT_TERMINATE  0xFFFF

/*----------------------------------------------------------------------------*/
/* struct                                                                     */
/*----------------------------------------------------------------------------*/
struct testresult{

  Byte  ETLR00;                         /*!  ETLR00                */
  Byte  ETLR01;                         /*!  ETLR01                */
  Byte  ETLR02;                         /*!  ETLR02                */
  Byte  ETLR03;                         /*!  ETLR03                */
  Byte  ETLR04;                         /*!  ETLR04                */
  Byte  ETLR05;                         /*!  ETLR05                */
  Byte  ETLR06;                         /*!  ETLR06                */
  Byte  ETLR07;                         /*!  ETLR07                */
  Byte  ETLR08;                         /*!  ETLR08                */
  Byte  ETLR09;                         /*!  ETLR09                */
  Byte  ETLR10;                         /*!  ETLR10                */
  Byte  ETLR11;                         /*!  ETLR11                */
  Byte  ETLR12;                         /*!  ETLR12                */
  Byte  ETLR13;                         /*!  ETLR13                */
  Byte  ETLR14;                         /*!  ETLR14                */
  Byte  ETLR15;                         /*!  ETLR15                */
  Byte  ETLR16;                         /*!  ETLR16                */
  Byte  ETLR17;                         /*!  ETLR17                */
  Byte  ETLR18;                         /*!  ETLR18                */
  Byte  ETLR19;                         /*!  ETLR19                */
  Byte  ETLR20;                         /*!  ETLR20                */
  Byte  ETLR21;                         /*!  ETLR21                */
  Byte  ETLR22;                         /*!  ETLR22                */
  Byte  ETLR23;                         /*!  ETLR23                */
  Byte  ETLR24;                         /*!  ETLR24                */
  Byte  ETLR25;                         /*!  ETLR25                */
  Byte  ETLR26;                         /*!  ETLR26                */
  Byte  ETLR27;                         /*!  ETLR27                */
  Byte  ETLR28;                         /*!  ETLR28                */
  Byte  ETLR29;                         /*!  ETLR29                */
  Byte  ETLR30;                         /*!  ETLR30                */
  Byte  ETLR31;                         /*!  ETLR31                */
  Byte  ETLR32;                         /*!  ETLR32                */
  Byte  ETLR33;                         /*!  ETLR33                */
  Byte  ETLR34;                         /*!  ETLR34                */
  Byte  ETLR35;                         /*!  ETLR35                */
  Byte  ETLR36;                         /*!  ETLR36                */
  Byte  ETLR37;                         /*!  ETLR37                */
  Byte  ETLR38;                         /*!  ETLR38                */
  Byte  ETLR39;                         /*!  ETLR39                */
  Byte  ETCR00;                         /*!  ETCR00                */
  Byte  ETCR01;                         /*!  ETCR01                */
  Byte  ETCR02;                         /*!  ETCR02                */
  Byte  ETCR03;                         /*!  ETCR03                */
  Byte  ETCR04;                         /*!  ETCR04                */
  Byte  ETCR05;                         /*!  ETCR05                */
  Byte  ETCR06;                         /*!  ETCR06                */
  Byte  ETCR07;                         /*!  ETCR07                */
  Byte  ETCR08;                         /*!  ETCR08                */
  Byte  ETCR09;                         /*!  ETCR09                */
  Byte  ETCR10;                         /*!  ETCR10                */
  Byte  ETCR11;                         /*!  ETCR11                */
  Byte  ETCR12;                         /*!  ETCR12                */
  Byte  ETCR13;                         /*!  ETCR13                */
  Byte  ETCR14;                         /*!  ETCR14                */
  Byte  ETCR15;                         /*!  ETCR15                */
  Byte  ETCR16;                         /*!  ETCR16                */  
  Byte  ETCR17;                         /*!  3値多数決結果               */
  Byte  JUDGE1;                          /*!  判定結果                    */
  Byte  JUDGE2;                          /*!  判定結果                    */
  
  Byte  BDATA00;                        /*!  汎用Byte型データ No.00      */
  Byte  BDATA01;                        /*!  汎用Byte型データ No.01      */
  Byte  BDATA02;                        /*!  汎用Byte型データ No.02      */
  Byte  BDATA03;                        /*!  汎用Byte型データ No.03      */
  Byte  BDATA04;                        /*!  汎用Byte型データ No.04      */
  Byte  BDATA05;                        /*!  汎用Byte型データ No.05      */
  Byte  BDATA06;                        /*!  汎用Byte型データ No.06      */
  Byte  BDATA07;                        /*!  汎用Byte型データ No.07      */
  Byte  BDATA08;                        /*!  汎用Byte型データ No.08      */
  Byte  BDATA09;                        /*!  汎用Byte型データ No.09      */
  Byte  BDATA10;                        /*!  汎用Byte型データ No.10      */
  Byte  BDATA11;                        /*!  汎用Byte型データ No.11      */
  Byte  BDATA12;                        /*!  汎用Byte型データ No.12      */
  Byte  BDATA13;                        /*!  汎用Byte型データ No.13      */
  Byte  BDATA14;                        /*!  汎用Byte型データ No.14      */
  Byte  BDATA15;                        /*!  汎用Byte型データ No.15      */
  
  Lword  LDATA0;                        /*!  汎用Long Byte型データ No.0  */
  Lword  LDATA1;                        /*!  汎用Long Byte型データ No.1  */
  Lword  LDATA2;                        /*!  汎用Long Byte型データ No.2  */
  Lword  LDATA3;                        /*!  汎用Long Byte型データ No.3  */
  Lword  LDATA4;                        /*!  汎用Long Byte型データ No.4  */
};

/******************************************************************************/
#endif
