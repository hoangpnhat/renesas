 // *********************************************************************** FILENAME        RX200CGR.H  CONTENTS: This is a RX200 pin assagin program for the V3300 test system.  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.  2008/11/01      rev.00  1'st created by H.Kobori  ***********************************************************************
#ifndef CGR_H
#define CGR_H

 // *********************************************************************** pin lists ***********************************************************************

extern int control_pins[];
extern int all_pins[];
extern int os_pins[];
extern int clk_pin[];
extern int poll_pins[];
extern int dend_pin[];
extern int dpass_pin[];
extern int bistpoll_pins[];
extern int tcheck_poll_pins[];
extern int dpass_pins[];
extern int daxs_datao_pin[];
extern int daxs_datai_pin[];
extern int dpass_tcheck_pins[];
extern int idcode_pin[];
extern int daxs_ddbg_pin[];
extern int minori_datai_pin[];
extern int ext_vrefh_pin[];
extern int ext_vref_pin[];
extern int data_pins[];
extern int dend_pass_pin[];
extern int debug_pin[];
extern int lp_clk[];
extern int lp_clk_out[];

extern int vbn_pin[];
extern int vbp_pin[];

extern int monitor_vdd_pin[];
extern int monitor_vddh_pin[];

extern int vec_fly_pin[];

extern int delay_test_pins[];
extern int delay_pin[];
extern int vssmon_pe_pin[];

extern int bgr_pin[];

extern int vcl1_pin[];
extern int vcl2_pin[];



extern int v1_pin[];
extern int v2_pin[];
extern int v3_pin[];
extern int v4_pin[];
extern int v5_pin[];
extern int no_pin[];

extern int lb_all_pins[];
// Relay LB0-7 CH24-31
extern int lb0_pin[];
// Relay LB0   CH24
extern int lb1_pin[];
// Relay LB1   CH25
extern int lb2_pin[];
// Relay LB2   CH26
extern int lb3_pin[];
// Relay LB3   CH27


#endif
