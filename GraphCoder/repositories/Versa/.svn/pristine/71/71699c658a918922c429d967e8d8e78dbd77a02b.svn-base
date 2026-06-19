/***********************************************************************
 *
 *  PATTERN: RX200cyc.h
 *
 *  CONTENTS: This is a 128KB - 256KB flash application program
 *      for the V3300 test system.
 *
 *  COPYRIGHT 1999 HITACHI LTD. ALL RIGHTS RESERVED.
 *
 *  1999/12/16  rev.00  1'st created by M.Kimura
 *
 ***********************************************************************/
#ifndef CYC_H
#define CYC_H

/************************************************************************
 *      Define table (tCYC & etc)
 ************************************************************************/
/* --------- System cycle define -------------------------------------- */

#define START_CYCLE t=2,    inv_en,vhh_en   /* 150ns (Start cycle)  */
#define MIN_CYCLE   t=2,    inv_en,vhh_en   /* 150ns (Min cycle)    */
#define SMIN_CYCLE  t=2,    inv_en,vhh_en   /* 150ns (Min cycle)    */
#define IO_CYCLE    t=16,   inv_en,vhh_en   /* 850ns (Reg R/W cycle)*/
#define ECR_CYCLE   t=4,    inv_en,vhh_en   /* 250ns (ECR R/W cycle)*/

/* --------- Normal cycle define -------------------------------------- */
#define NSEC950     t=18,   inv_en,vhh_en   /* 950ns m_clock cycle  */
#define NSEC900     t=17,   inv_en,vhh_en   /* 900ns m_clock cycle  */
#define NSEC850     t=16,   inv_en,vhh_en   /* 850ns m_clock cycle  */
#define NSEC800     t=15,   inv_en,vhh_en   /* 800ns m_clock cycle  */
#define NSEC750     t=14,   inv_en,vhh_en   /* 750ns m_clock cycle  */
#define NSEC700     t=13,   inv_en,vhh_en   /* 700ns m_clock cycle  */
#define NSEC650     t=12,   inv_en,vhh_en   /* 650ns m_clock cycle  */
#define NSEC600     t=11,   inv_en,vhh_en   /* 600ns m_clock cycle  */
#define NSEC550     t=10,   inv_en,vhh_en   /* 550ns m_clock cycle  */
#define NSEC500     t=9,    inv_en,vhh_en   /* 500ns m_clock cycle  */
#define NSEC450     t=8,    inv_en,vhh_en   /* 450ns m_clock cycle  */
#define NSEC400     t=7,    inv_en,vhh_en   /* 400ns m_clock cycle  */
#define NSEC350     t=6,    inv_en,vhh_en   /* 350ns m_clock cycle  */
#define NSEC300     t=5,    inv_en,vhh_en   /* 300ns m_clock cycle  */
#define NSEC250     t=4,    inv_en,vhh_en   /* 250ns m_clock cycle  */
#define NSEC200     t=3,    inv_en,vhh_en   /* 200ns m_clock cycle  */
#define NSEC150     t=2,    inv_en,vhh_en   /* 150ns m_clock cycle  */
#define NSEC100     t=1,    inv_en,vhh_en   /* 100ns m_clock cycle  */
#define NSEC50      t=0,    inv_en,vhh_en   /* 50ns m_clock cycle   */
#define NSEC150NOINV  t=2,         vhh_en   /* 150ns m_clock cycle  */

#define USEC1       t=19,   inv_en,vhh_en   /* 1us                  */
#define USEC2       t=39,   inv_en,vhh_en   /* 2us                  */
#define USEC3       t=59,   inv_en,vhh_en   /* 3us                  */
#define USEC4       t=79,   inv_en,vhh_en   /* 4us                  */
#define USEC5       t=99,   inv_en,vhh_en   /* 5us                  */
#define USEC10      t=199,  inv_en,vhh_en   /* 10us                 */
#define USEC15      t=299,  inv_en,vhh_en   /* 15us                 */
#define USEC20      t=399,  inv_en,vhh_en   /* 20us                 */
#define USEC30      t=599,  inv_en,vhh_en   /* 30us                 */
#define USEC50      t=999,  inv_en,vhh_en   /* 50us                 */
#define USEC100     t=1999, inv_en,vhh_en   /* 100us                */

#define EXTCYLM     2048                                    /* set_cycle_time()          */
#define EXTCYLM2    0xA00                                   /* set_cycle_time()          */
#define EXTCYLM4    0xB00                                   /* set_cycle_time()          */
#define EXTCYLJ     0x900                                   /* set_cycle_time()          */
#define TS6         t=EXTCYLM ,  inv_en,vhh_en,POWER_KEEP
#define TS13        t=EXTCYLJ,  inv_en,vhh_en,POWER_KEEP
#define TS15        t=EXTCYLM4,  inv_en,vhh_en,POWER_KEEP


#define EXTCYL      t=EXTCYLM,  inv_en,vhh_en,POWER_KEEP   /* set_cycle_time() 102.45 us*/
#define EXTCYL2     t=EXTCYLJ,        inv_en,vhh_en,POWER_KEEP   /* set_cycle_time() 102.45 us*/
#define EXTCYL3     t=EXTCYLM4,        inv_en,vhh_en              /* set_cycle_time()   0.2  us*/


/* --------- System register address define --------------------------- */


#endif
