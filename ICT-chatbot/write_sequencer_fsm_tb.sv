// ============================================================
// Testbench: Write Sequencer State Machine
// Reference: fsm_sample2_hard_copy - Figure 3.8.4
// ============================================================

`timescale 1ns/1ps

module write_sequencer_fsm_tb;

    // --------------------------------------------------------
    // DUT Port Connections
    // --------------------------------------------------------
    reg        clkr_l;
    reg        rstn_ws_l;
    reg        lgrp_popt_or_g;
    reg        lgrx_int_w_start_g;
    reg        lgdn_cflag_g;
    reg        lgrp_cntup_g;
    reg        lgrp_lpup_g;
    reg        lgrp_setupskip_g;
    reg        lgrp_itvlskip_g;
    reg [3:0]  lgrx_state_g;
    reg [1:0]  lgrp_plscnt_g;
    reg [2:0]  lgrp_wseq_g;

    wire       lgrp_wseqrst_g;
    wire       lgrp_wendfg_g;

    // --------------------------------------------------------
    // DUT Instantiation
    // --------------------------------------------------------
    mrp0000cpc_lgxws_fsm u_dut (
        .clkr_l              (clkr_l),
        .rstn_ws_l           (rstn_ws_l),
        .lgrp_popt_or_g      (lgrp_popt_or_g),
        .lgrx_int_w_start_g  (lgrx_int_w_start_g),
        .lgdn_cflag_g        (lgdn_cflag_g),
        .lgrp_cntup_g        (lgrp_cntup_g),
        .lgrp_lpup_g         (lgrp_lpup_g),
        .lgrp_setupskip_g    (lgrp_setupskip_g),
        .lgrp_itvlskip_g     (lgrp_itvlskip_g),
        .lgrx_state_g        (lgrx_state_g),
        .lgrp_plscnt_g       (lgrp_plscnt_g),
        .lgrp_wseq_g         (lgrp_wseq_g),
        .lgrp_wseqrst_g      (lgrp_wseqrst_g),
        .lgrp_wendfg_g       (lgrp_wendfg_g)
    );

    // --------------------------------------------------------
    // Clock Generation: CLKR_L (e.g. 10ns period)
    // --------------------------------------------------------
    initial clkr_l = 0;
    always #5 clkr_l = ~clkr_l;

    // --------------------------------------------------------
    // Task: apply one clock cycle
    // --------------------------------------------------------
    task tick;
        @(posedge clkr_l); #1;
    endtask

    // --------------------------------------------------------
    // Task: display current state info
    // --------------------------------------------------------
    task show_state;
        input [63:0] label;
        $display("[%0t] %s | state=%b | wseqrst=%b wendfg=%b",
                 $time, label,
                 u_dut.current_state,
                 lgrp_wseqrst_g, lgrp_wendfg_g);
    endtask

    // --------------------------------------------------------
    // Main Test Sequence
    // --------------------------------------------------------
    initial begin
        $dumpfile("write_sequencer_fsm.vcd");
        $dumpvars(0, write_sequencer_fsm_tb);

        // ---- Initialize all inputs ----
        rstn_ws_l          = 0;
        lgrp_popt_or_g     = 0;
        lgrx_int_w_start_g = 0;
        lgdn_cflag_g       = 0;
        lgrp_cntup_g       = 0;
        lgrp_lpup_g        = 0;
        lgrp_setupskip_g   = 0;
        lgrp_itvlskip_g    = 0;
        lgrx_state_g       = 4'b0000;
        lgrp_plscnt_g      = 2'b00;
        lgrp_wseq_g        = 3'b000;

        // ---- Release reset ----
        repeat(3) @(posedge clkr_l);
        rstn_ws_l = 1;
        #1;
        $display("=== Reset Released: Expect INIT state ===");
        show_state("INIT     ");

        // ====================================================
        // TEST 1: INIT → WSEQ0 (write start asserted)
        // ====================================================
        $display("\n--- TEST 1: INIT -> WSEQ0 ---");
        lgrx_int_w_start_g = 1;
        lgrp_popt_or_g     = 1;
        tick;
        lgrx_int_w_start_g = 0;
        show_state("WSEQ0    ");

        // ====================================================
        // TEST 2: WSEQ0 → PLSCNT0 (setup count complete)
        // ====================================================
        $display("\n--- TEST 2: WSEQ0 -> PLSCNT0 (cntup=1) ---");
        lgrp_cntup_g = 1;
        tick;
        lgrp_cntup_g = 0;
        show_state("PLSCNT0  ");

        // ====================================================
        // TEST 3: PLSCNT0 → PLSCNT1 (pulse needed, not done)
        // ====================================================
        $display("\n--- TEST 3: PLSCNT0 -> PLSCNT1 ---");
        lgrp_popt_or_g = 1;
        lgrp_lpup_g    = 0;
        lgrp_cntup_g   = 1;
        tick;
        lgrp_cntup_g   = 0;
        show_state("PLSCNT1  ");

        // ====================================================
        // TEST 4: PLSCNT1 → PLSCNT2
        // ====================================================
        $display("\n--- TEST 4: PLSCNT1 -> PLSCNT2 ---");
        lgrp_cntup_g = 1;
        tick;
        lgrp_cntup_g = 0;
        show_state("PLSCNT2  ");

        // ====================================================
        // TEST 5: PLSCNT2 → PLSCNT3
        // ====================================================
        $display("\n--- TEST 5: PLSCNT2 -> PLSCNT3 ---");
        lgrp_cntup_g = 1;
        tick;
        lgrp_cntup_g = 0;
        show_state("PLSCNT3  ");

        // ====================================================
        // TEST 6: PLSCNT3 → ITVL (pulse done, repeat done)
        // ====================================================
        $display("\n--- TEST 6: PLSCNT3 -> ITVL (lpup=1) ---");
        lgrp_popt_or_g = 1;
        lgrp_lpup_g    = 1;
        lgrp_cntup_g   = 1;
        tick;
        lgrp_cntup_g   = 0;
        lgrp_lpup_g    = 0;
        show_state("ITVL     ");

        // ====================================================
        // TEST 7: ITVL → PLSCNT0 (interval done)
        // ====================================================
        $display("\n--- TEST 7: ITVL -> PLSCNT0 (cntup=1, no skip) ---");
        lgrp_itvlskip_g = 0;
        lgrp_cntup_g    = 1;
        tick;
        lgrp_cntup_g    = 0;
        show_state("PLSCNT0  ");

        // ====================================================
        // TEST 8: PLSCNT0 → DWSEQ0 (other conditions)
        // ====================================================
        $display("\n--- TEST 8: PLSCNT0 -> DWSEQ0 (other cond) ---");
        lgrp_popt_or_g = 1;
        lgrp_lpup_g    = 0;
        lgrp_cntup_g   = 1;
        // Force "other conditions" by setting lpup=1, popt=0
        lgrp_popt_or_g = 0;
        lgrp_lpup_g    = 1;
        tick;
        lgrp_cntup_g   = 0;
        show_state("DWSEQ0   ");

        // ====================================================
        // TEST 9: DWSEQ0 → PLSCNT0 (cntup=1)
        // ====================================================
        $display("\n--- TEST 9: DWSEQ0 -> PLSCNT0 ---");
        lgrp_cntup_g = 1;
        tick;
        lgrp_cntup_g = 0;
        show_state("PLSCNT0  ");

        // ====================================================
        // TEST 10: Full sequence to END state
        // PLSCNT0 → PLSCNT1..7 → DWSEQ1 → END → INIT
        // ====================================================
        $display("\n--- TEST 10: Walk to END state ---");
        // PLSCNT0 → PLSCNT1..7 with popt=1, lpup=0
        lgrp_popt_or_g = 1;
        lgrp_lpup_g    = 0;
        repeat(7) begin
            lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0;
            show_state("PLSCNTn  ");
        end

        // PLSCNT7 → DWSEQ0 (other conditions, itvlskip=0)
        lgrp_itvlskip_g = 0;
        lgrp_cntup_g    = 1;
        tick;
        lgrp_cntup_g    = 0;
        show_state("DWSEQ0   ");

        // DWSEQ0 → PLSCNT0
        lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0;
        show_state("PLSCNT0  ");

        // Force to DWSEQ1 path (simulate sequence done)
        // PLSCNT0 → DWSEQ0 → DWSEQ1 → END
        lgrp_popt_or_g = 0;
        lgrp_lpup_g    = 1;
        lgrp_cntup_g   = 1; tick; lgrp_cntup_g = 0;
        show_state("DWSEQ0   ");

        lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0;
        show_state("DWSEQ1?  ");

        lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0;
        show_state("END?     ");

        // END → INIT (cflag asserted)
        lgdn_cflag_g = 1;
        tick;
        lgdn_cflag_g = 0;
        show_state("INIT     ");

        // ====================================================
        // TEST 11: Skip WSEQ0 (setupskip=1)
        // ====================================================
        $display("\n--- TEST 11: INIT -> WSEQ0 -> skip (setupskip=1) ---");
        lgrx_int_w_start_g = 1;
        lgrp_popt_or_g     = 1;
        tick;
        lgrx_int_w_start_g = 0;
        show_state("WSEQ0    ");

        lgrp_setupskip_g = 1;
        tick;
        lgrp_setupskip_g = 0;
        show_state("WSEQ0skip");

        // ====================================================
        // TEST 12: ITVL skip (itvlskip=1)
        // ====================================================
        $display("\n--- TEST 12: ITVL skip ---");
        lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0; // WSEQ0 → PLSCNT0
        lgrp_popt_or_g = 1; lgrp_lpup_g = 1;
        lgrp_cntup_g = 1; tick; lgrp_cntup_g = 0; // PLSCNT0 → ITVL
        show_state("ITVL     ");

        lgrp_itvlskip_g = 1;
        tick;
        lgrp_itvlskip_g = 0;
        show_state("PLSCNT0  "); // skipped interval

        $display("\n=== Simulation Complete ===");
        #20;
        $finish;
    end

endmodule
