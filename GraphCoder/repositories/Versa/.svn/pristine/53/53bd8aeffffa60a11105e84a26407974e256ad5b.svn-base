/******************************************************************************/
/*!
  @file RX200iif.h
  @brief  RC01SN-FMONOS
  @author Y.Aoki
  @date 2010.08.03 Rev.00
  $LastChangedDate$
  $Revision$
*/
/******************************************************************************/
#ifndef IIF_H
#define IIF_H

/*---- 試験条件種別 ----------------------------------------------------------*/
typedef enum{
                                        /*   ETLRレジスタ  */
  IN_ETLR00,                            /*!  ETLR00レジスタに上書きする値     */
  IN_ETLR01,                            /*!  ETLR01レジスタに上書きする値     */
  IN_ETLR02,                            /*!  ETLR02レジスタに上書きする値     */
  IN_ETLR03,                            /*!  ETLR03レジスタに上書きする値     */
  IN_ETLR04,                            /*!  ETLR04レジスタに上書きする値     */
  IN_ETLR05,                            /*!  ETLR05レジスタに上書きする値     */
  IN_ETLR06,                            /*!  ETLR06レジスタに上書きする値     */
  IN_ETLR07,                            /*!  ETLR07レジスタに上書きする値     */
  IN_ETLR08,                            /*!  ETLR08レジスタに上書きする値     */
  IN_ETLR09,                            /*!  ETLR09レジスタに上書きする値     */
  IN_ETLR10,                            /*!  ETLR10レジスタに上書きする値     */
  IN_ETLR11,                            /*!  ETLR11レジスタに上書きする値     */
  IN_ETLR12,                            /*!  ETLR12レジスタに上書きする値     */
  IN_ETLR13,                            /*!  ETLR13レジスタに上書きする値     */
  IN_ETLR14,                            /*!  ETLR14レジスタに上書きする値     */
  IN_ETLR15,                            /*!  ETLR15レジスタに上書きする値     */
  IN_ETLR16,                            /*!  ETLR16レジスタに上書きする値     */
  IN_ETLR17,                            /*!  ETLR17レジスタに上書きする値     */
  IN_ETLR18,                            /*!  ETLR18レジスタに上書きする値     */
  IN_ETLR19,                            /*!  ETLR19レジスタに上書きする値     */
  IN_ETLR20,                            /*!  ETLR20レジスタに上書きする値     */
  IN_ETLR21,                            /*!  ETLR21レジスタに上書きする値     */
  IN_ETLR22,                            /*!  ETLR22レジスタに上書きする値     */
  IN_ETLR23,                            /*!  ETLR23レジスタに上書きする値     */
  IN_ETLR24,                            /*!  ETLR24レジスタに上書きする値     */
  IN_ETLR25,                            /*!  ETLR25レジスタに上書きする値     */
  IN_ETLR26,                            /*!  ETLR26レジスタに上書きする値     */
  IN_ETLR27,                            /*!  ETLR27レジスタに上書きする値     */
  IN_ETLR28,                            /*!  ETLR28レジスタに上書きする値     */
  IN_ETLR29,                            /*!  ETLR29レジスタに上書きする値     */
  IN_ETLR30,                            /*!  ETLR30レジスタに上書きする値     */
  IN_ETLR31,                            /*!  ETLR31レジスタに上書きする値     */
  IN_ETLR32,                            /*!  ETLR32レジスタに上書きする値     */
  IN_ETLR33,                            /*!  ETLR33レジスタに上書きする値     */
  IN_ETLR34,                            /*!  ETLR34レジスタに上書きする値     */
  IN_ETLR35,                            /*!  ETLR35レジスタに上書きする値     */
  IN_ETLR36,                            /*!  ETLR36レジスタに上書きする値     */
  IN_ETLR37,                            /*!  ETLR37レジスタに上書きする値     */
  IN_ETLR38,                            /*!  ETLR38レジスタに上書きする値     */
  IN_ETLR39,                            /*!  ETLR39レジスタに上書きする値     */
  
                                        /*    ETCRレジスタ  */
  IN_ETCR00,                            /*!  ETCR00レジスタに上書きする値     */
  IN_ETCR01,                            /*!  ETCR01レジスタに上書きする値     */
  IN_ETCR02,                            /*!  ETCR02レジスタに上書きする値     */
  IN_ETCR03,                            /*!  ETCR03レジスタに上書きする値     */
  IN_ETCR04,                            /*!  ETCR04レジスタに上書きする値     */
  IN_ETCR05,                            /*!  ETCR05レジスタに上書きする値     */
  IN_ETCR06,                            /*!  ETCR06レジスタに上書きする値     */
  IN_ETCR07,                            /*!  ETCR07レジスタに上書きする値     */
  IN_ETCR08,                            /*!  ETCR08レジスタに上書きする値     */
  IN_ETCR09,                            /*!  ETCR09レジスタに上書きする値     */
  IN_ETCR10,                            /*!  ETCR10レジスタに上書きする値     */
  IN_ETCR11,                            /*!  ETCR11レジスタに上書きする値     */
  IN_ETCR12,                            /*!  ETCR12レジスタに上書きする値     */
  IN_ETCR13,                            /*!  ETCR13レジスタに上書きする値     */
  
                                        /*   その他   */
  IN_EXTAL2,                            /*! 入力CLK周期[ns](下位8Bit) */
  IN_EXTAL1,                            /*! 入力CLK周期[ns](上位8Bit) */
  IN_PLLON,                             /*! PLL選択 */
  IN_PLL_MULT,                          /*! PLL逓倍率 */
  IN_PLL_MULT_VAL,                      /*! PLL逓倍率設定値 */
  IN_PLL_DIV,                           /*! PLL分周比 */
  IN_PLL_DIV_VAL,                       /*! PLL分周比設定値 */
  IN_ICLK,                              /*! ICLK分周比 */
  IN_ICLK_VAL,                          /*! ICLK分周比設定値 */
  IN_BCLK,                              /*! SOTBでは未使用 */
  IN_BCLK_VAL,                          /*! SOTBでは未使用 */
  IN_FCLK,                              /*! SOTBでは未使用 */
  IN_FCLK_VAL,                          /*! SOTBでは未使用 */
  IN_PCLK,                              /*! PCLKB分周比 */
  IN_PCLK_VAL,                          /*! PCLKB分周比設定値 */
  IN_MEMWAIT,                           /*! SOTBでは未使用 */
  IN_REG,                               /*! トリミング/テストレジスタの設定方法を指定 */
  IN_AREA,                              /*! 評価するAreaを指定 */
  IN_TESTSEL,                           /*! プログラム内で条件分岐などに使用 */
  IN_WRITESIZE,                         /*! 書込みサイズ(progsize)を指定 */
  IN_VCC_MODE,                          /*! VCCの動作モード設定 */
  IN_VDD_TAP,                           /*! VDDのトリミングTap変更値(ﾄﾘﾐﾝｸﾞｾﾝﾀ±Tap) */
  IN_VDDH_TAP,                          /*! VDDHのトリミングTap変更値(ﾄﾘﾐﾝｸﾞｾﾝﾀ±Tap) */
  IN_LOWPOWER,                          /*! Low Power Read  */
  IN_NOFAILSTOP,                        /*! FunctionテストがFailしても最後まで流す機能 */
  
  IN_BDATA00,                           /*!  汎用Byte型データ No.00      */
  IN_BDATA01,                           /*!  汎用Byte型データ No.01      */
  IN_BDATA02,                           /*!  汎用Byte型データ No.02      */
  IN_BDATA03,                           /*!  汎用Byte型データ No.03      */
  IN_BDATA04,                           /*!  汎用Byte型データ No.04      */
  IN_BDATA05,                           /*!  汎用Byte型データ No.05      */
  IN_BDATA06,                           /*!  汎用Byte型データ No.06      */
  IN_BDATA07,                           /*!  汎用Byte型データ No.07      */
  IN_BDATA08,                           /*!  汎用Byte型データ No.08      */
  IN_BDATA09,                           /*!  汎用Byte型データ No.09      */
  IN_BDATA10,                           /*!  汎用Byte型データ No.10      */
  IN_BDATA11,                           /*!  汎用Byte型データ No.11      */
  IN_BDATA12,                           /*!  汎用Byte型データ No.12      */
  IN_BDATA13,                           /*!  汎用Byte型データ No.13      */
  IN_BDATA14,                           /*!  汎用Byte型データ No.14      */
  IN_BDATA15,                           /*!  汎用Byte型データ No.15      */
  IN_BDATA16,                           /*!  汎用Byte型データ No.16      */

     
  IN_BYTEEND,                           /*!< Byte型データの終端          */

  IN_TOPADDR0,                          /*! 評価する領域のスタート番地を入力(FLP0) */
  IN_TESTSIZE0,                         /*! 評価する領域のサイズを入力(FLP0) */
  IN_TOPADDR1,                          /*! 評価する領域のスタート番地を入力(FLI1) */
  IN_TESTSIZE1,                         /*! 評価する領域のサイズを入力(FLI1) */
  IN_TOPADDR2,                          /*! 評価する領域のスタート番地を入力(FLI2) */
  IN_TESTSIZE2,                         /*! 評価する領域のサイズを入力(FLI2) */
  IN_LDATA0,                            /*!  汎用Long Byte型データ No.0  */
  IN_LDATA1,                            /*!  汎用Long Byte型データ No.1  */
  IN_LDATA2,                            /*!  汎用Long Byte型データ No.2  */
  IN_LDATA3,                            /*!  汎用Long Byte型データ No.3  */
  IN_LDATA4,                            /*!  汎用Long Byte型データ No.4  */
  IN_LDATA5,                            /*!  汎用Long Byte型データ No.5  */
  IN_EXTAL_PS,                          /*! Extal ps                     */
  IN_LWORDEND                           /*!< LongWord型データの終端      */
} InputIFPointer;

#define IN_TERMINATE           0xFFFF   /*!< I/Fの終端またはエラー            */
                                        /*!<                                  */
/*---- 試験条件初期値 --------------------------------------------------------*/
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
#define  INI_ETLR00           0xFF    /*!<  ETLR00         */
#define  INI_ETLR01           0xFF    /*!<  ETLR01         */
#define  INI_ETLR02           0xFF    /*!<  ETLR02         */
#define  INI_ETLR03           0xFF    /*!<  ETLR03         */
#define  INI_ETLR04           0xFF    /*!<  ETLR04         */
#define  INI_ETLR05           0xFF    /*!<  ETLR05         */
#define  INI_ETLR06           0xFF    /*!<  ETLR06         */
#define  INI_ETLR07           0xFF    /*!<  ETLR07         */
#define  INI_ETLR08           0xA0    /*!<  ETLR08         */
#define  INI_ETLR09           0xA0    /*!<  ETLR09         */
#define  INI_ETLR10           0xAD    /*!<  ETLR10         */
#define  INI_ETLR11           0xFF    /*!<  ETLR11         */
#define  INI_ETLR12           0x58    /*!<  ETLR12         */
#define  INI_ETLR13           0xB8    /*!<  ETLR13         */
#define  INI_ETLR14           0x88    /*!<  ETLR14         */
#define  INI_ETLR15           0xFF    /*!<  ETLR15         */
#define  INI_ETLR16           0xF8    /*!<  ETLR16         */
#define  INI_ETLR17           0xD0    /*!<  ETLR17         */
#define  INI_ETLR18           0xBF    /*!<  ETLR18         */
#define  INI_ETLR19           0x59    /*!<  ETLR19         */
#define  INI_ETLR20           0xA4    /*!<  ETLR20         */
#define  INI_ETLR21           0xA6    /*!<  ETLR21         */
#define  INI_ETLR22           0xFF    /*!<  ETLR22         */
#define  INI_ETLR23           0xFF    /*!<  ETLR23         */
#define  INI_ETLR24           0xFC    /*!<  ETLR24         */
#define  INI_ETLR25           0xFF    /*!<  ETLR25         */
#define  INI_ETLR26           0x00    /*!<  ETLR26         */
#define  INI_ETLR27           0xF0    /*!<  ETLR27         */
#define  INI_ETLR28           0xF6    /*!<  ETLR28         */
#define  INI_ETLR29           0xF6    /*!<  ETLR29         */
#define  INI_ETLR30           0xF6    /*!<  ETLR30         */
#define  INI_ETLR31           0xFF    /*!<  ETLR31         */
#define  INI_ETLR32           0x23    /*!<  ETLR32         */
#define  INI_ETLR33           0x33    /*!<  ETLR33         */
#define  INI_ETLR34           0x44    /*!<  ETLR34         */
#define  INI_ETLR35           0xFF    /*!<  ETLR35         */
#define  INI_ETLR36           0xFF    /*!<  ETLR36         */
#define  INI_ETLR37           0xFF    /*!<  ETLR37         */
#define  INI_ETLR38           0xFF    /*!<  ETLR38         */
#define  INI_ETLR39           0xFF    /*!<  ETLR39         */
#elif ( PRODUCT_NAME==PRODUCT_IO )
#define  INI_ETLR00           0xFF    /*!<  ETLR00         */
#define  INI_ETLR01           0x87    /*!<  ETLR01         */
#define  INI_ETLR02           0x7F    /*!<  ETLR02         */
#define  INI_ETLR03           0xF8    /*!<  ETLR03         */
#define  INI_ETLR04           0xFF    /*!<  ETLR04         */
#define  INI_ETLR05           0xEF    /*!<  ETLR05         */
#define  INI_ETLR06           0xFB    /*!<  ETLR06         */
#define  INI_ETLR07           0xFF    /*!<  ETLR07         */
#define  INI_ETLR08           0xA0    /*!<  ETLR08         */
#define  INI_ETLR09           0xA0    /*!<  ETLR09         */
#define  INI_ETLR10           0xDA    /*!<  ETLR10         */
#define  INI_ETLR11           0xF0    /*!<  ETLR11         */
#define  INI_ETLR12           0x58    /*!<  ETLR12         */
#define  INI_ETLR13           0xB8    /*!<  ETLR13         */
#define  INI_ETLR14           0x88    /*!<  ETLR14         */
#define  INI_ETLR15           0xFB    /*!<  ETLR15         */
#define  INI_ETLR16           0xE8    /*!<  ETLR16         */
#define  INI_ETLR17           0xD4    /*!<  ETLR17         */
#define  INI_ETLR18           0xB6    /*!<  ETLR18         */
#define  INI_ETLR19           0x0C    /*!<  ETLR19         */
#define  INI_ETLR20           0xA9    /*!<  ETLR20         */
#define  INI_ETLR21           0xA9    /*!<  ETLR21         */
#define  INI_ETLR22           0xFB    /*!<  ETLR22         */
#define  INI_ETLR23           0xFD    /*!<  ETLR23         */
#define  INI_ETLR24           0xFC    /*!<  ETLR24         */
#define  INI_ETLR25           0xFA    /*!<  ETLR25         */
#define  INI_ETLR26           0x77    /*!<  ETLR26         */
#define  INI_ETLR27           0xF7    /*!<  ETLR27         */
#define  INI_ETLR28           0xFC    /*!<  ETLR28         */
#define  INI_ETLR29           0xFB    /*!<  ETLR29         */
#define  INI_ETLR30           0xFB    /*!<  ETLR30         */
#define  INI_ETLR31           0xFF    /*!<  ETLR31         */
#define  INI_ETLR32           0x5E    /*!<  ETLR32         */
#define  INI_ETLR33           0x22    /*!<  ETLR33         */
#define  INI_ETLR34           0x22    /*!<  ETLR34         */
#define  INI_ETLR35           0xFF    /*!<  ETLR35         */
#define  INI_ETLR36           0xFF    /*!<  ETLR36         */
#define  INI_ETLR37           0xFF    /*!<  ETLR37         */
#define  INI_ETLR38           0xA5    /*!<  ETLR38         */
#define  INI_ETLR39           0xFE    /*!<  ETLR39         */
#endif

#define  INI_ETCR00           0x00    /*!<  ETCR00         */
#define  INI_ETCR01           0x00    /*!<  ETCR01         */
#define  INI_ETCR02           0x00    /*!<  ETCR02         */
#define  INI_ETCR03           0x00    /*!<  ETCR03         */
#define  INI_ETCR04           0x00    /*!<  ETCR04         */
#define  INI_ETCR05           0x00    /*!<  ETCR05         */
#define  INI_ETCR06           0x00    /*!<  ETCR06         */
#define  INI_ETCR07           0x00    /*!<  ETCR07         */
#define  INI_ETCR08           0x00    /*!<  ETCR08         */
#define  INI_ETCR09           0x00    /*!<  ETCR09         */
#define  INI_ETCR10           0x00    /*!<  ETCR10         */
#define  INI_ETCR11           0x00    /*!<  ETCR11         */
#define  INI_ETCR12           0x00    /*!<  ETCR12         */
#define  INI_ETCR13           0x00    /*!<  ETCR13         */

#define   INI_EXTAL1          31        /*!<  EXTAL1    */
#define   INI_PLLON           0         /*!<  PLLON    */
#define   INI_PLL_MULT        1         /*!<  PLL_MULT    */
#define   INI_PLL_MULT_VAL    0         /*!<  PLL_MULT_VAL    */
#define   INI_PLL_DIV         1         /*!<  PLL_DIV    */
#define   INI_PLL_DIV_VAL     0         /*!<  PLL_DIV_VAL    */
#define   INI_ICLK            1         /*!<  ICLK    */
#define   INI_ICLK_VAL        0         /*!<  ICLK_VAL    */
#define   INI_BCLK            0x00      /*!<  BCLK    */
#define   INI_BCLK_VAL        0x00      /*!<  BCLK_VAL    */
#define   INI_FCLK            0x00      /*!<  FCLK    */
#define   INI_FCLK_VAL        0x00      /*!<  FCLK_VAL    */
#define   INI_PCLK            1         /*!<  PCLK    */
#define   INI_PCLK_VAL        0         /*!<  PCLK_VAL    */
#define   INI_MEMWAIT         0x00      /*!<  MEMWAIT    */
#define   INI_REG             3         /*!<  REG    */
#define   INI_AREA            0         /*!<  AREA    */
#define   INI_TESTSEL         0x00      /*!<  TESTSEL    */
#define   INI_WRITESIZE       0x00      /*!<  WRITESIZE    */
#define   INI_VCC_MODE        1         /*!<  VCC_MODE    */
#define   INI_VDD_TAP         0x00      /*!<  VDD_TAP    */
#define   INI_VDDH_TAP        0x00      /*!<  VDDH_TAP    */
#define   INI_LOWPOWER        0x00      /*!<  LOWPOWER    */
#define   INI_NOFAILSTOP      0x00      /*!<  NOFAILSTOP    */

#define  INI_BDATA00          0x00    /*!<  BDATA00        */
#define  INI_BDATA01          0x00    /*!<  BDATA01        */
#define  INI_BDATA02          0x00    /*!<  BDATA02        */
#define  INI_BDATA03          0x00    /*!<  BDATA03        */
#define  INI_BDATA04          0x00    /*!<  BDATA04        */
#define  INI_BDATA05          0x00    /*!<  BDATA05        */
#define  INI_BDATA06          0x00    /*!<  BDATA06        */
#define  INI_BDATA07          0x00    /*!<  BDATA07        */
#define  INI_BDATA08          0x00    /*!<  BDATA08        */
#define  INI_BDATA09          0x00    /*!<  BDATA09        */
#define  INI_BDATA10          0x00    /*!<  BDATA10        */
#define  INI_BDATA11          0x00    /*!<  BDATA11        */
#define  INI_BDATA12          0x00    /*!<  BDATA12        */
#define  INI_BDATA13          0x00    /*!<  BDATA13        */
#define  INI_BDATA14          0x01    /*!<  BDATA14        */
#define  INI_BDATA15          0x00    /*!<  BDATA15        */
#define  INI_BDATA16          0x00    /*!<  BDATA16        */

#define   INI_TOPADDR0        FLP0_TOP   /*!<  TOPADDR0    */
#define   INI_TESTSIZE0       FLP0_SIZE   /*!<  TESTSIZE0    */
#define   INI_TOPADDR1        FLI1_TOP   /*!<  TOPADDR1    */
#define   INI_TESTSIZE1       FLI1_SIZE   /*!<  TESTSIZE1    */
#define   INI_TOPADDR2        FLI2_TOP   /*!<  TOPADDR2    */
#define   INI_TESTSIZE2       FLI2_SIZE   /*!<  TESTSIZE2    */

#define  INI_LDATA0           0x00    /*!<  LDATA0         */
#define  INI_LDATA1           0x00    /*!<  LDATA1         */
#define  INI_LDATA2           0x00    /*!<  LDATA2         */
#define  INI_LDATA3           0x00    /*!<  LDATA3         */
#define  INI_LDATA4           0x00    /*!<  LDATA4         */
#define  INI_LDATA5           0x00    /*!<  LDATA5         */
#define  INI_EXTAL_PS         31000   /*! Extal ps         */


/*----------------------------------------------------------------------------*/
/* struct                                                                     */
/*----------------------------------------------------------------------------*/
struct testcondition{


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
  Byte  EXTAL2;                                /*! 入力CLK周期[ns](上位8Bit) */
  Byte  EXTAL1;                                /*! 入力CLK周期[ns](下位8Bit) */
  Byte  PLLON;                                /*! PLL選択 */
  Byte  PLL_MULT;                             /*! PLL逓倍率 */
  Byte  PLL_MULT_VAL;                         /*! PLL逓倍率設定値 */
  Byte  PLL_DIV;                              /*! PLL分周比 */
  Byte  PLL_DIV_VAL;                          /*! PLL分周比設定値 */
  Byte  ICLK;                                 /*! ICLK分周比 */
  Byte  ICLK_VAL;                             /*! ICLK分周比設定値 */
  Byte  BCLK;                                 /*! SOTBでは未使用 */
  Byte  BCLK_VAL;                             /*! SOTBでは未使用 */
  Byte  FCLK;                                 /*! SOTBでは未使用 */
  Byte  FCLK_VAL;                             /*! SOTBでは未使用 */
  Byte  PCLK;                                 /*! PCLKB分周比 */
  Byte  PCLK_VAL;                             /*! PCLKB分周比設定値 */
  Byte  MEMWAIT;                              /*! SOTBでは未使用 */
  Byte  REG;                                  /*! トリミング/テストレジスタの設定方法を指定 */
  Byte  AREA;                                 /*! 評価するAreaを指定 */
  Byte  TESTSEL;                              /*! プログラム内で条件分岐などに使用 */
  Byte  WRITESIZE;                            /*! 書込みサイズ(progsize)を指定 */
  Byte  VCC_MODE;                             /*! VCCの動作モード設定 */
  Byte  VDD_TAP;                              /*! VDDのトリミングTap変更値(ﾄﾘﾐﾝｸﾞｾﾝﾀ±Tap) */
  Byte  VDDH_TAP;                             /*! VDDHのトリミングTap変更値(ﾄﾘﾐﾝｸﾞｾﾝﾀ±Tap) */
  Byte  LOWPOWER;                             /*! Low Power Read  */
  Byte  NOFAILSTOP;                           /*! FunctionテストがFailしても最後まで流す機能 */
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
  Byte  BDATA16;                        /*!  汎用Byte型データ No.16      */
  Lword  TOPADDR0;                             /*! 評価する領域のスタート番地を入力(FLP0) */
  Lword  TESTSIZE0;                            /*! 評価する領域のサイズを入力(FLP0) */
  Lword  TOPADDR1;                             /*! 評価する領域のスタート番地を入力(FLI1) */
  Lword  TESTSIZE1;                            /*! 評価する領域のサイズを入力(FLI1) */
  Lword  TOPADDR2;                             /*! 評価する領域のスタート番地を入力(FLI2) */
  Lword  TESTSIZE2;                            /*! 評価する領域のサイズを入力(FLI2) */
  Lword  LDATA0;                        /*!  汎用Long Byte型データ No.0  */
  Lword  LDATA1;                        /*!  汎用Long Byte型データ No.1  */
  Lword  LDATA2;                        /*!  汎用Long Byte型データ No.2  */
  Lword  LDATA3;                        /*!  汎用Long Byte型データ No.3  */
  Lword  LDATA4;                        /*!  汎用Long Byte型データ No.4  */
  Lword  LDATA5;                        /*!  汎用Long Byte型データ No.5  */

};

#define INI_PERCR0_E2P_PVBC     0x00    /*!< PVモードBC  */
#define INI_PERCR1_E2P_PVBC     0x10    /*!< PVモードBC  */
#define INI_PERCR2_E2P_PVBC     0x00    /*!< PVモードBC  */
#define INI_PERCR3_E2P_PVBC     0x00    /*!< PVモードBC  */
#define INI_PERCR4_E2P_PVBC     0x00    /*!< PVモードBC  */
#define INI_PERCR5_E2P_PVBC     0xD0    /*!< PVモードBC  */
#define INI_PERCR6_E2P_PVBC     0x12    /*!< PVモードBC  */

#define INI_PERCR0_FLP_E        0x1B    /*!< PERCR0_FLP_ERASE MG */
#define INI_PERCR1_FLP_E        0x54    /*!< PERCR1_FLP_ERASE ベリファイ*/
#define INI_PERCR2_FLP_E        0x1B    /*!< PERCR2_FLP_ERASE SL */
#define INI_PERCR3_FLP_E        0x0B    /*!< PERCR3_FLP_ERASE */
#define INI_PERCR4_FLP_E        0x8A    /*!< PERCR4_FLP_ERASE */
#define INI_PERCR5_FLP_E        0x00    /*!< PERCR5_FLP_ERASE */
#define INI_PERCR6_FLP_E        0x10    /*!< PERCR6_FLP_ERASE */

#define INI_PERCR0_FLP_P        0x6A    /*!< PERCR0_FLP_PROG  MG */
#define INI_PERCR1_FLP_P        0x6F    /*!< PERCR1_FLP_PROG  ベリファイ*/
#define INI_PERCR2_FLP_P        0x0B    /*!< PERCR2_FLP_PROG  SL */
#define INI_PERCR3_FLP_P        0x4B    /*!< PERCR3_FLP_PROG  */
#define INI_PERCR4_FLP_P        0xE3    /*!< PERCR4_FLP_PROG  */
#define INI_PERCR5_FLP_P        0x01    /*!< PERCR5_FLP_PROG  */
#define INI_PERCR6_FLP_P        0x13    /*!< PERCR6_FLP_PROG  */

#define INI_PERCR0_E2P_E        0x1B    /*!< PERCR0_E2P_ERASE MG */
#define INI_PERCR1_E2P_E        0x1E    /*!< PERCR1_E2P_ERASE ベリファイ*/
#define INI_PERCR2_E2P_E        0x1B    /*!< PERCR2_E2P_ERASE SL */
#define INI_PERCR3_E2P_E        0x09    /*!< PERCR3_E2P_ERASE */
#define INI_PERCR4_E2P_E        0x8A    /*!< PERCR4_E2P_ERASE */
#define INI_PERCR5_E2P_E        0x03    /*!< PERCR5_E2P_ERASE */
#define INI_PERCR6_E2P_E        0x10    /*!< PERCR6_E2P_ERASE */

#define INI_PERCR0_E2P_P        0x69    /*!< PERCR0_E2P_PROG  MG */
#define INI_PERCR1_E2P_P        0x22    /*!< PERCR1_E2P_PROG  ベリファイ*/
#define INI_PERCR2_E2P_P        0x0B    /*!< PERCR2_E2P_PROG  SL */
#define INI_PERCR3_E2P_P        0x4B    /*!< PERCR3_E2P_PROG  */
#define INI_PERCR4_E2P_P        0xE3    /*!< PERCR4_E2P_PROG  */
#define INI_PERCR5_E2P_P        0x02    /*!< PERCR5_E2P_PROG  */
#define INI_PERCR6_E2P_P        0x13    /*!< PERCR6_E2P_PROG  */

/******************************************************************************/
#endif
