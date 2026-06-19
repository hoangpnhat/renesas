// ============================================================
// Write Sequencer State Machine
// Module: mrp0000cpc_lgxws_fsm
// Reference: fsm_sample2_hard_copy - Figure 3.8.4
// ============================================================

module mrp0000cpc_lgxws_fsm (
    // --------------------------------------------------------
    // PORT LIST
    // --------------------------------------------------------
    // Clock & Reset
    input  wire        clkr_l,           // CLKR clock         (VDD, TIMING: -)
    input  wire        rstn_ws_l,        // Internal reset signal (VDD)

    // Inputs
    input  wire        lgrp_popt_or_g,   // Pulse required/not required signal (VDD, TYPE: P)
    input  wire        lgrx_int_w_start_g, // Write start signal               (VDD, TYPE: P)
    input  wire        lgdn_cflag_g,     // End flag clear signal              (VDD)
    input  wire        lgrp_cntup_g,     // Count complete signal              (VDD, TIMING: CLKR_L)
    input  wire        lgrp_lpup_g,      // Write repeat complete signal       (VDD, TIMING: CLKR_L)
    input  wire        lgrp_setupskip_g, // Setup skip signal                  (VDD, TIMING: CLKR_L)
    input  wire        lgrp_itvlskip_g,  // Interval skip signal               (VDD, TIMING: CLKR_L)
    input  wire [3:0]  lgrx_state_g,     // State signal                       (VDD, TIMING: CLKR_L)
    input  wire [1:0]  lgrp_plscnt_g,    // Write cycle signal                 (VDD, TIMING: CLKR_L)
    input  wire [2:0]  lgrp_wseq_g,      // Write sequence signal              (VDD, TIMING: CLKR_L)

    // Outputs
    output reg         lgrp_wseqrst_g,   // Write sequence reset signal        (VDD, TIMING: CLKR_L)
    output reg         lgrp_wendfg_g     // Write end flag signal              (VDD, TYPE: P, TIMING: CLKR_L)
);

    // ============================================================
    // STATE ENCODING (4-bit binary)
    // ============================================================
    // No.  STATE      state[3:0]
    //  0   INIT       0000
    //  1   WSEQ0      0001
    //  2   DWSEQ0     0010  (page_1) / DWSEQ0 0010 (page_3 shows same)
    //  3   PLSCNT0    0011
    //  4   PLSCNT1    0100
    //  5   PLSCNT2    0101
    //  6   PLSCNT3    0110
    //  7   PLSCNT4    0111
    //  8   PLSCNT5    1000
    //  9   PLSCNT6    1001
    // 10   PLSCNT7    1010
    // 11   ITVL       1011
    // 12   DWSEQ0     1100
    // 13   DWSEQ1     1101
    // 14   END        1110

    localparam [3:0]
        ST_INIT    = 4'b0000,
        ST_WSEQ0   = 4'b0001,
        ST_DWSEQ0  = 4'b0010,   // entry DWSEQ0 (from page_1 table row 2)
        ST_PLSCNT0 = 4'b0011,
        ST_PLSCNT1 = 4'b0100,
        ST_PLSCNT2 = 4'b0101,
        ST_PLSCNT3 = 4'b0110,
        ST_PLSCNT4 = 4'b0111,
        ST_PLSCNT5 = 4'b1000,
        ST_PLSCNT6 = 4'b1001,
        ST_PLSCNT7 = 4'b1010,
        ST_ITVL    = 4'b1011,
        ST_DWSEQ0B = 4'b1100,   // DWSEQ0 second encoding (page_3 row 12)
        ST_DWSEQ1  = 4'b1101,
        ST_END     = 4'b1110;

    // ============================================================
    // Internal State Register
    // ============================================================
    reg [3:0] current_state, next_state;

    // ============================================================
    // State Register (Sequential Logic) — clocked on CLKR_L
    // ============================================================
    always @(posedge clkr_l or negedge rstn_ws_l) begin
        if (!rstn_ws_l)
            current_state <= ST_INIT;
        else
            current_state <= next_state;
    end

    // ============================================================
    // Next-State Logic (Combinational)
    //
    // Key signals:
    //   lgrp_cntup_g    — Wait time elapsed (1 = elapsed)
    //   lgrp_popt_or_g  — Pulse required(1) / not required(0)
    //   lgrp_lpup_g     — Write repeat complete (1 = complete)
    //   lgrp_itvlskip_g — Interval skip (1 = skip)
    //   lgrp_setupskip_g— Setup skip
    //   lgrx_int_w_start_g — Write start
    //   lgrp_plscnt_g   — Write cycle count [1:0]
    //   lgrp_wseq_g     — Write sequence [2:0]
    //
    // PLSCNT and Interval rule (from doc Section 1.1.1):
    //   PLSCNT0~7: after Tappl elapses (cntup=1), transition determined by:
    //     lgrp_popt_or_g, lgrp_lpup_g, lgrp_itvlskip_g
    //   Skip WSEQ0: when Tsetup=min(40ns), lgrp_setupskip_g=1 → skip to WSEQ0
    //
    // hold = retain previous output value
    // ============================================================
    always @(*) begin
        // Default: stay in current state
        next_state = current_state;

        case (current_state)

            // --------------------------------------------------
            // INIT
            // --------------------------------------------------
            ST_INIT : begin
                if (lgrx_int_w_start_g == 1'b1 && lgrp_popt_or_g == 1'b1)
                    next_state = ST_WSEQ0;          // 1'b1 start, pulse required
                else if (lgrx_int_w_start_g == 1'b1 && lgrp_popt_or_g == 1'b0)
                    next_state = ST_WSEQ0;          // start regardless
                else
                    next_state = ST_INIT;
            end

            // --------------------------------------------------
            // WSEQ0  — Setup period
            // --------------------------------------------------
            ST_WSEQ0 : begin
                if (lgrp_setupskip_g == 1'b1)
                    next_state = ST_WSEQ0;          // skip condition → stay/skip
                else if (lgrp_cntup_g == 1'b1)
                    next_state = ST_PLSCNT0;        // setup done → start pulse count
                else
                    next_state = ST_WSEQ0;
            end

            // --------------------------------------------------
            // DWSEQ0  — Inter-sequence delay 0
            // --------------------------------------------------
            ST_DWSEQ0 : begin
                if (lgrp_cntup_g == 1'b1)
                    next_state = ST_PLSCNT0;
                else
                    next_state = ST_DWSEQ0;
            end

            // --------------------------------------------------
            // PLSCNT0  (n=0, first pulse count state)
            // Transition table from doc (PLSCNT n, n=1~6 same pattern):
            //   cntup=0, ?    , ?    , ?     → PLSCNT n   (wait)
            //   cntup=1, po=1 , ?    , ?     → PLSCNT n+1 (more pulses needed)
            //   cntup=1, po=0 , lp=0 , ?     → PLSCNT0    (repeat)
            //   cntup=1, po=? , lp=? , iv=?  → DWSEQ0     (other conditions)
            // --------------------------------------------------
            ST_PLSCNT0 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT0;
                else begin // cntup = 1
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;           // pulse done, repeat done → interval
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT1;        // pulse needed, not done → next cnt
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;        // no pulse req, not done → stay
                    else
                        next_state = ST_DWSEQ0;         // other conditions
                end
            end

            ST_PLSCNT1 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT1;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT2;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            ST_PLSCNT2 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT2;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT3;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            ST_PLSCNT3 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT3;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT4;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            ST_PLSCNT4 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT4;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT5;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            ST_PLSCNT5 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT5;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT6;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            ST_PLSCNT6 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT6;
                else begin
                    if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b1)
                        next_state = ST_ITVL;
                    else if (lgrp_popt_or_g == 1'b1 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT7;
                    else if (lgrp_popt_or_g == 1'b0 && lgrp_lpup_g == 1'b0)
                        next_state = ST_PLSCNT0;
                    else
                        next_state = ST_DWSEQ0;
                end
            end

            // --------------------------------------------------
            // PLSCNT7  — Last pulse count state
            // cntup=0 → PLSCNT7
            // cntup=1, itvlskip=0 → PLSCNT0  (repeat from start)
            // cntup=1, itvlskip=1 → PLSCNT0  (skip interval)
            // other conditions    → DWSEQ0
            // --------------------------------------------------
            ST_PLSCNT7 : begin
                if (lgrp_cntup_g == 1'b0)
                    next_state = ST_PLSCNT7;
                else begin
                    if (lgrp_itvlskip_g == 1'b0)
                        next_state = ST_PLSCNT0;        // no skip → back to PLSCNT0
                    else
                        next_state = ST_DWSEQ0;         // other conditions
                end
            end

            // --------------------------------------------------
            // ITVL  — Interval state
            // --------------------------------------------------
            ST_ITVL : begin
                if (lgrp_cntup_g == 1'b1 && lgrp_itvlskip_g == 1'b0)
                    next_state = ST_PLSCNT0;            // interval done, no skip
                else if (lgrp_itvlskip_g == 1'b1)
                    next_state = ST_PLSCNT0;            // skip interval
                else
                    next_state = ST_ITVL;
            end

            // --------------------------------------------------
            // DWSEQ0B (second DWSEQ0, encoding 1100)
            // --------------------------------------------------
            ST_DWSEQ0B : begin
                if (lgrp_cntup_g == 1'b1)
                    next_state = ST_PLSCNT0;
                else
                    next_state = ST_DWSEQ0B;
            end

            // --------------------------------------------------
            // DWSEQ1
            // --------------------------------------------------
            ST_DWSEQ1 : begin
                if (lgrp_cntup_g == 1'b1)
                    next_state = ST_END;
                else
                    next_state = ST_DWSEQ1;
            end

            // --------------------------------------------------
            // END  — Sequence complete, return to INIT
            // --------------------------------------------------
            ST_END : begin
                if (lgdn_cflag_g == 1'b1)
                    next_state = ST_INIT;
                else
                    next_state = ST_END;
            end

            default: next_state = ST_INIT;

        endcase
    end

    // ============================================================
    // Output Logic (Mealy/Moore combined, registered outputs)
    //
    // Output columns from State Transition Table (page_1 / page_3):
    //   lgrp_plscnt_g  → lgrx_plscnt_g  (write cycle)
    //   lgrp_wseq_g    → lgrp_wseq_g    (write sequence)
    //   lgrp_wseqrst_g → write sequence reset
    //   lgrp_wendfg_g  → write end flag
    //
    // From page_1 output columns:
    //   lgrp_wseq_g  : 3'b000 for most states, increments at sequence boundaries
    //   lgrp_wseqrst_g: hold / 1'b0 / 1'b1
    //   lgrp_wendfg_g : hold / 1'b0 / 1'b1
    // ============================================================

    // Output registers
    reg [1:0] lgrx_plscnt_g_r;
    reg [2:0] lgrp_wseq_g_r;

    always @(posedge clkr_l or negedge rstn_ws_l) begin
        if (!rstn_ws_l) begin
            lgrp_wseqrst_g   <= 1'b0;
            lgrp_wendfg_g    <= 1'b0;
            lgrx_plscnt_g_r  <= 2'b00;
            lgrp_wseq_g_r    <= 3'b000;
        end
        else begin
            case (next_state)

                // ------------------------------------------------
                // INIT: reset all outputs
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b000
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=1'b0
                // ------------------------------------------------
                ST_INIT : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b000;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g; // hold
                    lgrp_wendfg_g   <= 1'b0;
                end

                // ------------------------------------------------
                // WSEQ0: setup phase
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b001
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=1'b0
                // ------------------------------------------------
                ST_WSEQ0 : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b001;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g; // hold
                    lgrp_wendfg_g   <= 1'b0;
                end

                // ------------------------------------------------
                // DWSEQ0: inter-sequence delay
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b000
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=hold
                // ------------------------------------------------
                ST_DWSEQ0 : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b000;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g; // hold
                    lgrp_wendfg_g   <= lgrp_wendfg_g;  // hold
                end

                // ------------------------------------------------
                // PLSCNT0~7: pulse counting states
                // lgrx_plscnt_g increments, lgrp_wseq_g=3'b111
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=hold
                // ------------------------------------------------
                ST_PLSCNT0 : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT1 : begin
                    lgrx_plscnt_g_r <= 2'b01;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT2 : begin
                    lgrx_plscnt_g_r <= 2'b10;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT3 : begin
                    lgrx_plscnt_g_r <= 2'b11;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT4 : begin
                    lgrx_plscnt_g_r <= 2'b00;   // wraps
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT5 : begin
                    lgrx_plscnt_g_r <= 2'b01;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT6 : begin
                    lgrx_plscnt_g_r <= 2'b10;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                ST_PLSCNT7 : begin
                    lgrx_plscnt_g_r <= 2'b11;
                    lgrp_wseq_g_r   <= 3'b111;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                // ------------------------------------------------
                // ITVL: interval state
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b011
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=hold
                // ------------------------------------------------
                ST_ITVL : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b011;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                // ------------------------------------------------
                // DWSEQ0B (1100): second DWSEQ0
                // ------------------------------------------------
                ST_DWSEQ0B : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b000;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                // ------------------------------------------------
                // DWSEQ1: final delay before END
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b010
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=hold
                // ------------------------------------------------
                ST_DWSEQ1 : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b010;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= lgrp_wendfg_g;
                end

                // ------------------------------------------------
                // END: assert end flag, reset sequence
                // lgrx_plscnt_g=3'b000, lgrp_wseq_g=3'b000
                // lgrp_wseqrst_g=hold, lgrp_wendfg_g=1'b1
                // ------------------------------------------------
                ST_END : begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b000;
                    lgrp_wseqrst_g  <= lgrp_wseqrst_g;
                    lgrp_wendfg_g   <= 1'b1;
                end

                default: begin
                    lgrx_plscnt_g_r <= 2'b00;
                    lgrp_wseq_g_r   <= 3'b000;
                    lgrp_wseqrst_g  <= 1'b0;
                    lgrp_wendfg_g   <= 1'b0;
                end

            endcase
        end
    end

endmodule
