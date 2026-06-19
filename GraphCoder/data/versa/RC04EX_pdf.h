// ************************************************************************************************

// PATTERN:                RX200PDF.h

// CONTENTS: This is a 128KB flash application program
// for the V2100 test system.

// COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.

// 2008/11/01      rev.00  1'st created by H.Kobori

// ************************************************************************************************
#ifndef PDF_H
#define PDF_H

// ************************************************************************************************
// TM5F mode define table (pattern formater)
// ************************************************************************************************
// --------- Control pin Condition (/RESET /STBY) -----------------------------------------------
#define RES_L         format rstn nr=0 , format porlvdtst nr=1
#define RES_H         format rstn nr=1 , format porlvdtst nr=1,format trst nr=1
#define PORLVDTST_L   format porlvdtst nr=0
#define PORLVDTST_H   format porlvdtst nr=1

// --------- relay control condition (vector mode) ----------------------------------------------
#define RELAY_FF      format all_lb nr=0xff
// relay ctl=H'ff
#define RELAY_00      format all_lb nr=0x00
// relay ctl=H'00
#define RELAY_HZ      format all_lb nc=0
// relay ctl=open
#define RELAY_LB0_ON  vhh_en,format lb0 nr =1
#define RELAY_LB1_ON  vhh_en,format lb1 nr =1
#define RELAY_LB2_ON  vhh_en,format lb2 nr =1
#define RELAY_LB3_ON  vhh_en,format lb3 nr =1
#define RELAY_LB3_OFF vhh_en,format lb3 nr =0

// --------- relay & power keep ------------------
#define POWER_KEEP    format ex_vrefh nr=1,format all_lb nr=0x00, format vsc_vcc nr=0


// --------- Constant Condition (BIST/NMI/IDDQN) ------------------------------------------------
// caution!! bist mode pll NG or no problem

//#define CONST format exvrefin nr=0, format iddqn nr=0

// --------- Internal Vdd Condition -------------------------------------------------------------
//#define VDD_OFF       format vclvldn nr=1    // External Power Voltage
//#define VDD_ON        format vclvldn nr=0    // Internal Power Voltage

// --------- Mode Condition ---------------------------------------------------------------------
#define  MODEENTRY_STARTBIT1      format idcode0 nr=1, format idcode0 nr=1,vec
#define  MODEENTRY_STARTBIT2      format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_SECURITYBIT7   format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_SECURITYBIT6   format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_SECURITYBIT5   format idcode0 nr=1, format idcode0 nr=1,vec
#define  MODEENTRY_SECURITYBIT4   format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_SECURITYBIT3   format idcode0 nr=1, format idcode0 nr=1,vec
#define  MODEENTRY_SECURITYBIT2   format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_SECURITYBIT1   format idcode0 nr=1, format idcode0 nr=1,vec
#define  MODEENTRY_SECURITYBIT0   format idcode0 nr=0, format idcode0 nr=0,vec
#define  MODEENTRY_EXVREFHEN      format idcode0_ext nr
#define  MODEENTRY_EXVREFEN       format idcode0_ext nr
#define  MODEENTRY_DAXS_SMD4      format idcode0 nr=0,vec
#define  MODEENTRY_DAXS_SMD3      format idcode0 nr=0,vec
#define  MODEENTRY_DAXS_SMD2      format idcode0 nr=0,vec
#define  MODEENTRY_DAXS_SMD1      format idcode0 nr=0,vec
#define  MODEENTRY_DAXS_SMD0      format idcode0 nr=1,vec
#define  MODEENTRY_RAM_SMD4       format idcode0 nr=0,vec
#define  MODEENTRY_RAM_SMD3       format idcode0 nr=0,vec
#define  MODEENTRY_RAM_SMD2       format idcode0 nr=0,vec
#define  MODEENTRY_RAM_SMD1       format idcode0 nr=1,vec
#define  MODEENTRY_RAM_SMD0       format idcode0 nr=1,vec
#define  MODEENTRY_ROM_SMD4       format idcode0 nr=1,vec
#define  MODEENTRY_ROM_SMD3       format idcode0 nr=0,vec
#define  MODEENTRY_ROM_SMD2       format idcode0 nr=0,vec
#define  MODEENTRY_ROM_SMD1       format idcode0 nr=1,vec
#define  MODEENTRY_ROM_SMD0       format idcode0 nr=0,vec
#define  MODEENTRY_MINORI_SMD4    format idcode0 nr=0,vec
#define  MODEENTRY_MINORI_SMD3    format idcode0 nr=1,vec
#define  MODEENTRY_MINORI_SMD2    format idcode0 nr=1,vec
#define  MODEENTRY_MINORI_SMD1    format idcode0 nr=1,vec
#define  MODEENTRY_MINORI_SMD0    format idcode0 nr=1,vec


// ---------- ID-Code Bit    ---------------------------------------------------------------------

#define  ID_CODE_BIT31  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT30  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT29  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT28  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT27  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT26  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT25  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT24  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT23  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT22  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT21  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT20  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT19  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT18  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT17  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT16  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT15  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT14  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT13  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT12  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT11  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT10  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT9  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT8  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT7  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT6  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT5  format  idcode0  nr=1,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT4  format  idcode0  nr=0,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT3  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT2  format  idcode0  nr=1,  format  idcode1  nr=1,vec
#define  ID_CODE_BIT1  format  idcode0  nr=0,  format  idcode1  nr=0,vec
#define  ID_CODE_BIT0  format  idcode0  nr=0,  format  idcode1  nr=0,vec

// --------- extal control ----------------------------------------------------------------------
#define EXH           format clk nr=1
#define CLK           format clk rto=0
#define CLK_H         format clk nr=1
#define CLK_L         format clk nr=0

#define LP_CLK        format lp_clk_in  rto=0
#define LP_CLK_H      format lp_clk_in  nr =1
#define LP_CLK_L      format lp_clk_in  nr =0
#define XCOUT_L       format lp_clk_out nr =0

// --------- DAXS Control -----------------------------------------------
#define DAXS_STATE_L  format daxs_state nr=0, format daxs_gen2sel nr=0,vec
#define DAXS_STATE_H  format daxs_state nr=1, format daxs_gen2sel nr=0,vec
#define DAXS_DATA_CP  format daxs_datao_ext cn, format daxs_datai nc=0,format debugpin rto = 0
// data=comp(strobe) d
#define DAXS_DATA_DR  format daxs_datai_ext nr, format daxs_datao nc=0
// data=drive(input) d
#define DAXS_DATA_NC  format daxs_datai nr=0, format daxs_datao   nc=0
// data=drive(input) d

// ------------RAM Boot Control------------------------------------------------------
#define BOOT_CTRL1
// format atb1 nr=0,
#define POLL_NC       format dend nr, vec
#define POLL_PASS     format dend nr, vec
#define POLL_CP       format dend cn=0, vec
#define DELAY_H       format delaytest nr=1, format sw_vssmon nr=1,vec
#define DELAY_L       format delaytest nr=0, format sw_vssmon nr=0,vec
#define TCHECK_CP     format tcheck_ext cn
#define DPASS_CP      format dpass cn=0,vec




// --------- Test mode initalize  ---------------------------------------------------------------
#define MODE_ENTRY0    vec, format idcode0 nr=0, format idcode1 nr=0
//#define MODE_ENTRY1    format
#define MODE_ENTRY_ID  vec, format idcode0_ext nr  , format idcode1_ext nr
//#define DAXS_STATE_H   format  DAXS_STATE nr=1, format DAXS_GEN2SEL nr=0
//#define DAXS_STATE_L   format  DAXS_STATE nr=0, format DAXS_GEN2SEL nr=0


// --------- MINORI Control ------------------------------------------------------
#define TRST_L format trst nr=0
#define TRST_H format trst nr=1
#define MINORI_LP_APG  format daxs_datai_ext nr  , format daxs_datao_ext nr, format idcode0_ext nr, format idcode1_ext nr
#define TMS_L vec, format tms nr=0,format trst nr=1
#define TMS_H vec, format tms nr=1,format trst nr=1
#define TDI_NP     format tdi_ext nr
#define TDI_DR     format tdi_ext nr
#define TD_BM      format tms_ext nr, format tdi_ext nr,format trst nr=1
#define TDO_CP     format tdo_ext cn
#define TDM_OUT0   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT1   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT2   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT3   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT4   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT5   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT6   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT7   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT8   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT9   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT10   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT11   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT12   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT13   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT14   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT15   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT16   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT17   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT18   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT19   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT20   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT21   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT22   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT23   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT24   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT25   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT26   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT27   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT28   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT29   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT30   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT31   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT32   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT33   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT34   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT35   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT36   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT37   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT38   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_OUT39   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT40   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT41   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT42   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT43   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT44   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT45   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT46   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_OUT47   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT01   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT02   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT03   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT04   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT05   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT06   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT07   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT08   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT09   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT10   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT11   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT12   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT13   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT14   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT15   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT16   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT17   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT18   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT19   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT20   format tms nr=0, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT21   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT22   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT23   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT24   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT25   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT26   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT27   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT28   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT29   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT30   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT31   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT32   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT33   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT34   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT35   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT36   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT37   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT38   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT39   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT40   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT41   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT42   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT43   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT44   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT45   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT46   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT47   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT48   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT49   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT50   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT51   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT52   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT53   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT54   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT55   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT56   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT57   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT58   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT59   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT60   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT61   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT62   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT63   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT64   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT65   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT66   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT67   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT68   format tms nr=0, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT69   format tms nr=1, format tdi nr=1,vec,format trst nr=1
#define TDM_PLLOUT70   format tms nr=1, format tdi nr=0,vec,format trst nr=1
#define TDM_PLLOUT71   format tms nr=0, format tdi nr=0,vec,format trst nr=1
 


// Debug Pin define

#define DEBUG_L format debugpin nr=0
#define DEBUG_H format debugpin nr=1


#endif
