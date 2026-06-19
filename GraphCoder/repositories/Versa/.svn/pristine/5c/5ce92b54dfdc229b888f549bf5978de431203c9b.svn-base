/************************************************************************
 *      FILENAME        RX200agr.h
 *
 *      CONTENTS: This is a 0.15um FLASH module define program for the
 *                      V3300 test system
 *
 *       COPYRIGHT 2001 HITACHI LTD. ALL RIGHTS RESERVED
 *
 *  2008/11/01 rev.01 1'st created by H.Kobori
 *
 ************************************************************************/
#ifndef AGR_H
#define AGR_H

/************************************************************************
 * define logical pin name
 ************************************************************************/
pin dend_ext       = P_CH3;       /*** DEND ***/
pin daxs_datai_ext = P_CH2;       /*** DAXS_DATAI ***/
pin daxs_datao_ext = P_CH3;       /*** DAXS_DATAO ***/
pin idcode0_ext    = P_CH1;       /*** DAXS_DATAI ***/
pin idcode1_ext    = P_CH6;       /*** DAXS_DATAO ***/
pin tdi_ext        = P_CH2;       /*** TDI ***/
pin tdo_ext        = P_CH3;       /*** TDO ***/
pin tms_ext        = P_CH1;       /*** TMS ***/
pin tcheck_ext     = P_CH17;      /*** TCHECK ***/

/************************************************************************
 * define pin groupings for use in format statments
 ************************************************************************/

@group clk            { P_CH0  }/*** DAXS_CLK ***/
@group daxs_state     { P_CH1  }/*** DAXS_STATE/DPASS ***/
@group idcode0        { P_CH1  }/*** ID-Code0 ***/
@group tms            { P_CH1  }/*** TMS ***/
@group daxs_datai     { P_CH2  }/*** DAXS_DATAI ***/
@group tdi            { P_CH2  }/*** TDI ***/
@group daxs_datao     { P_CH3  }/*** DAXS_DATAO ***/
@group dend           { P_CH3  }/*** Dend ***/
@group tdo            { P_CH3  }/*** TDO  ***/
@group daxs_gen2sel   { P_CH4  }/*** DAXS_GEN2SEL ***/
@group dpass          { P_CH4  }/*** DPASS ***/
@group trst           { P_CH4  }/*** TRST  ***/
@group rstn           { P_CH5  }/*** RES# ***/
@group idcode1        { P_CH6  }/*** IDCODE1 ***/
@group delaytest      { P_CH6  }/*** DELAYTEST ***/
@group lp_clk_in      { P_CH7  }/*  XCIN   */
@group lp_clk_out     { P_CH8  }/*  XCOUT  */

@group debugpin       { P_CH9  }/* Debug Pin */


@group ex_vrefh       { P_CH10 }/*** ext_vreh(vhh) ***/
@group sw_vssmon      { P_CH11 }/*** vssmon ***/

#if ( PRODUCT_NAME==PRODUCT_EUROPA )
@group atb1           { P_CH2 } /*** ATB1 ***/
@group op_pins_0_31 {        /*** open/short ***/
        /*P_CH31, P_CH30,*/
        P_CH29, P_CH28, P_CH27, P_CH26, /*P_CH25, P_CH24,*/ P_CH23, P_CH22, P_CH21, P_CH20,
        P_CH19, P_CH18, P_CH17, P_CH16, /*P_CH15, P_CH14, P_CH13, P_CH12, */P_CH11, P_CH10,
        P_CH9,  P_CH8,/*  P_CH7,*/  P_CH6,  P_CH5,  P_CH4,  P_CH3,  P_CH2,  P_CH1,  P_CH0
}
#elif ( PRODUCT_NAME==PRODUCT_IO )
@group atb1           { P_CH13 }/*** ATB1 ***/
@group op_pins_0_31 {        /*** open/short ***/
        /*P_CH31, P_CH30,*/
        P_CH29, P_CH28, P_CH27, P_CH26, /*P_CH25, P_CH24,*/ P_CH23, P_CH22, P_CH21, P_CH20,
        P_CH19, P_CH18, P_CH17, P_CH16, /*P_CH15, P_CH14,*/ P_CH13,/* P_CH12, */P_CH11, P_CH10,
        P_CH9 , P_CH8 , P_CH7 , P_CH6 , P_CH5 , P_CH4 , P_CH3 , P_CH2 , P_CH1 , P_CH0
}
#endif

@group porlvdtst      { P_CH16 }/*** PORLVDTST ***/
@group tcheck         { P_CH17 }/*** T-Check ***/
@group ledi1          { P_CH18 }/*** (EXVREFIN)/PDSS_CG ***/
@group vcl1           { P_CH19 }/*** VCL1 ***/
@group vcl2           { P_CH20 }/*** VCL2 ***/
@group vbn            { P_CH21 }/*** vbn ***/
@group vbp            { P_CH22 }/*** vbp ***/
@group vsc_vcc        { P_CH23 }/*** vsc_vcc ***/

 /*** Relay Control Pin : high = 5V(for RC04EX) ***/
@group lb0            { P_CH26 }/*** RELAY LB0 ***/
@group lb1            { P_CH27 }/*** RELAY LB1 ***/
@group lb2            { P_CH28 }/*** RELAY LB2 ***/
@group lb3            { P_CH29 }/*** RELAY LB3 ***/

@group all_lb      {    /*** all RELAY *********************/
        P_CH29,P_CH28,P_CH27,P_CH26
}

@group all_pe_0_31 {    /*** all channel *******************/
    P_CH0 ,P_CH1 ,P_CH2 ,P_CH3 ,P_CH4 ,P_CH5 ,P_CH6 ,P_CH7 ,P_CH8 ,P_CH9 ,
    P_CH10,P_CH11,P_CH12,P_CH13,P_CH14,P_CH15,P_CH16,P_CH17,P_CH18,P_CH19,
    P_CH20,P_CH21,P_CH22,P_CH23,P_CH24,P_CH25,P_CH26,P_CH27,P_CH28,P_CH29,
    P_CH30,P_CH31
}


@group poweron_debug  {P_CH7,P_CH9,P_CH11,P_CH12,P_CH13,P_CH14,P_CH15  }/*** DELAYTEST ***/

#endif
