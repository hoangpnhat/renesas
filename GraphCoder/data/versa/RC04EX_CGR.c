 // *********************************************************************** FILENAME        RX200CGR.C  CONTENTS: This is a RX200 pin assagin program for the V3300 test system.  COPYRIGHT 1997 HITACHI LTD. ALL RIGHTS RESERVED.  2008/11/01      rev.00  1'st created by H.Kobori  ***********************************************************************

#include "RC04EX_man.h"
#include "RC04EX_def.h"
#include "RC04EX.hpg"
#include "RC04EX_CGR.h"

 // *********************************************************************** pin lists ***********************************************************************

int control_pins[] = {
        CH0 , CH1 ,
        // CH2 , CH3 ,
        CH10, CH11, CH12, CH13, CH14, CH15, CH16, CH17, CH18, CH19,
        CH20, CH21, CH22, CH23, CH24, CH25, CH26, CH27, CH28, CH29,
        CH30, CH31,
        ENDLST
};

int data_pins[] ={
        CH2,CH3,
        ENDLST
};

int vec_fly_pin[] ={
        CH1,CH6,
		ENDLST
};

int all_pins[] = {
        CH0 , CH1 , CH2 , CH3 , CH4 , CH5 , CH6 , CH7 , CH8 , CH9,
        CH10, CH11, CH12, CH13, CH14, CH15, CH16, CH17, CH18,
#if ( PRODUCT_NAME==PRODUCT_EUROPA )
        CH19,CH20,
#endif
        CH21, CH22, CH23, CH24, CH25, CH26, CH27, CH28, CH29,
        CH30, CH31,
        ENDLST
};



int os_pins[] = {
        CH1 , CH2 , CH3 , CH4 , CH5 , CH6 ,CH10,
        
        ENDLST
};

int clk_pin[] = {
        CH0 ,
        // EXTAL,DAXS_CLK
        ENDLST
};

int poll_pins[] = {
        CH3,
        ENDLST
};

int dend_pin[] = {
        CH3,
        // DEND
        ENDLST
};


int bistpoll_pins[] = {
        CH3,
        // DEND,DPASS
        ENDLST
};

int tcheck_poll_pins[] = {
        CH17,
        // TCHECK
        ENDLST
};

int dpass_pins[] = {
        CH4,
        // TCHECK
        ENDLST
};

int dpass_tcheck_pins[] = {
        CH4,CH17,
        ENDLST
};

int daxs_datao_pin[] = {
        CH3,
        // DATAO
        ENDLST
};

int dend_pass_pin[] = {
        CH3,CH4,
        ENDLST
};

int daxs_datai_pin[] = {
        CH1,
        // DATAI
        ENDLST
};

int idcode_pin[] = {
        CH1,CH6,
        // ID-Code͗p
	ENDLST
};

int	debug_pin[] = {
        CH9,
        ENDLST
};

int	lp_clk[] = {
        CH7,CH8,
        ENDLST
};

int	lp_clk_out[] = {
        CH8,
        ENDLST
};

int vbn_pin[] = {
        CH21,
        // VBN Pin̐ݒ
        ENDLST
};

int vbp_pin[] = {
        CH22,
        // VBP Pin̐ݒ
        ENDLST
};


int daxs_ddbg_pin[] = {
        CH2,
        // DDBG
        ENDLST
};

int minori_datai_pin[] = {
        CH1,CH2,
        ENDLST
};

int monitor_vdd_pin[] = {
        CH19,
        // Vddj^pPin̐ݒ
        ENDLST
};

int monitor_vddh_pin[] = {
        CH20,
        // Vddhj^pPin̐ݒ
        ENDLST
};

int ext_vrefh_pin[] = {
        CH10,
        // VrefhpPin̐ݒ
	ENDLST
};

int ext_vref_pin[] = {
        V3,
        // VrefpPin̐ݒ
	ENDLST
};

int delay_test_pins[] = {
        CH6,CH11,
        // DelayTestPin̐ݒ(Vssmon&DelayTest)
	ENDLST
};

int delay_pin[] = {
        CH6,
        // DelayTestPin̐ݒ(DelayTest)
	ENDLST
};
int vssmon_pe_pin[] = {
        CH11,
        // DelayTestPin̐ݒ(Vssmons)
	ENDLST
};

int vcl1_pin[] = {
        CH19,

	ENDLST
};
int vcl2_pin[] = {
        CH20,

	ENDLST
};
int bgr_pin[] = {
        CH2,
        // DelayTestPin̐ݒ(Vssmons)
	ENDLST
};


int v1_pin[]            = { V1, ENDLST };
int v2_pin[]            = { V2, ENDLST };
int v3_pin[]            = { V3, ENDLST };
int v4_pin[]            = { V4, ENDLST };
int v5_pin[]            = { V5, ENDLST };
int no_pin[]            = {     ENDLST };

int lb_all_pins[] = {
        CH26,CH27,CH28,CH29,CH3,
        // Relay LB0-LB3
        ENDLST
};

int lb0_pin[]            = { CH26, ENDLST };
// Relay LB0
int lb1_pin[]            = { CH27, ENDLST };
// Relay LB1
int lb2_pin[]            = { CH28, ENDLST };
// Relay LB2
int lb3_pin[]            = { CH29, ENDLST };
// Relay LB3

