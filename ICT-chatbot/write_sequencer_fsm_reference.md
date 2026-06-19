# Write Sequencer State Machine — RTL Reference
## Module: `mrp0000cpc_lgxws_fsm`
### Source: fsm_sample2_hard_copy — Figure 3.8.4

---

## 1. Port Variable Table

| PORT                  | I/O | TYPE | TIMING | POW    | Description                        |
|-----------------------|-----|------|--------|--------|------------------------------------|
| `clkr_l`              | I   | —    | —      | VDD    | CLKR clock                         |
| `rstn_ws_l`           | I   | —    | —      | VDD    | Internal reset signal              |
| `lgrp_popt_or_g`      | I   | P    | —      | VDD    | Pulse required/not required signal |
| `lgrx_int_w_start_g`  | I   | P    | —      | VDD    | Write start signal                 |
| `lgdn_cflag_g`        | I   | —    | —      | VDD    | End flag clear signal              |
| `lgrp_cntup_g`        | I   | —    | CLKR_L | VDD   | Count complete signal              |
| `lgrp_lpup_g`         | I   | —    | CLKR_L | VDD   | Write repeat complete signal       |
| `lgrp_setupskip_g`    | I   | —    | CLKR_L | VDD   | Setup skip signal                  |
| `lgrp_itvlskip_g`     | I   | —    | CLKR_L | VDD   | Interval skip signal               |
| `lgrx_state_g[3:0]`   | I   | —    | CLKR_L | VDD   | State signal                       |
| `lgrp_plscnt_g[1:0]`  | I   | —    | CLKR_L | VDD   | Write cycle signal                 |
| `lgrp_wseq_g[2:0]`    | I   | —    | CLKR_L | VDD   | Write sequence signal              |
| `lgrp_wseqrst_g`      | O   | —    | CLKR_L | VDD   | Write sequence reset signal        |
| `lgrp_wendfg_g`       | O   | P    | CLKR_L | VDD   | Write end flag signal              |

---

## 2. State Encoding Table

| No. | STATE    | state[3:0] |
|-----|----------|------------|
|  0  | INIT     | 0000       |
|  1  | WSEQ0    | 0001       |
|  2  | DWSEQ0   | 0010       |
|  3  | PLSCNT0  | 0011       |
|  4  | PLSCNT1  | 0100       |
|  5  | PLSCNT2  | 0101       |
|  6  | PLSCNT3  | 0110       |
|  7  | PLSCNT4  | 0111       |
|  8  | PLSCNT5  | 1000       |
|  9  | PLSCNT6  | 1001       |
| 10  | PLSCNT7  | 1010       |
| 11  | ITVL     | 1011       |
| 12  | DWSEQ0   | 1100       |
| 13  | DWSEQ1   | 1101       |
| 14  | END      | 1110       |

---

## 3. State Transition Summary

### INIT
| lgrx_int_w_start_g | lgrp_popt_or_g | Next State |
|--------------------|----------------|------------|
| 1'b0               | ?              | INIT       |
| 1'b1               | 1'b1           | WSEQ0      |
| 1'b1               | 1'b0           | WSEQ0      |

### WSEQ0 (Setup phase)
| lgrp_setupskip_g | lgrp_cntup_g | Next State |
|------------------|--------------|------------|
| 1'b1             | ?            | WSEQ0      |
| 1'b0             | 1'b0         | WSEQ0      |
| 1'b0             | 1'b1         | PLSCNT0    |

> **Skip WSEQ0**: When Tsetup = minimum (40ns), `lgrp_setupskip_g=1` → WSEQ0 is skipped

### PLSCNT n (n = 0~6, same pattern)
| lgrp_cntup_g | lgrp_popt_or_g | lgrp_lpup_g | Next State   |
|--------------|----------------|-------------|--------------|
| 1'b0         | ?              | ?           | PLSCNT n     |
| 1'b1         | 1'b1           | 1'b0        | PLSCNT n+1   |
| 1'b1         | 1'b1           | 1'b1        | ITVL         |
| 1'b1         | 1'b0           | 1'b0        | PLSCNT0      |
| 1'b1         | other          | other       | DWSEQ0       |

### PLSCNT7 (last pulse count)
| lgrp_cntup_g | lgrp_itvlskip_g | Next State |
|--------------|-----------------|------------|
| 1'b0         | ?               | PLSCNT7    |
| 1'b1         | 1'b0            | PLSCNT0    |
| 1'b1         | other           | DWSEQ0     |

### ITVL (Interval)
| lgrp_cntup_g | lgrp_itvlskip_g | Next State |
|--------------|-----------------|------------|
| 1'b0         | 1'b0            | ITVL       |
| 1'b1         | 1'b0            | PLSCNT0    |
| ?            | 1'b1            | PLSCNT0    |

### DWSEQ0
| lgrp_cntup_g | Next State |
|--------------|------------|
| 1'b0         | DWSEQ0     |
| 1'b1         | PLSCNT0    |

### DWSEQ1
| lgrp_cntup_g | Next State |
|--------------|------------|
| 1'b0         | DWSEQ1     |
| 1'b1         | END        |

### END
| lgdn_cflag_g | Next State |
|--------------|------------|
| 1'b0         | END        |
| 1'b1         | INIT       |

---

## 4. Output Table (from State Transition Table)

| State    | lgrx_plscnt_g | lgrp_wseq_g | lgrp_wseqrst_g | lgrp_wendfg_g |
|----------|---------------|-------------|----------------|---------------|
| INIT     | 3'b000        | 3'b000      | hold           | 1'b0          |
| WSEQ0    | 3'b000        | 3'b001      | hold           | 1'b0          |
| DWSEQ0   | 3'b000        | 3'b000      | hold           | hold          |
| PLSCNT0  | 3'b000        | 3'b111      | hold           | hold          |
| PLSCNT1  | 3'b001        | 3'b111      | hold           | hold          |
| PLSCNT2  | 3'b010        | 3'b111      | hold           | hold          |
| PLSCNT3  | 3'b011        | 3'b111      | hold           | hold          |
| PLSCNT4  | 3'b100        | 3'b111      | hold           | hold          |
| PLSCNT5  | 3'b101        | 3'b111      | hold           | hold          |
| PLSCNT6  | 3'b110        | 3'b111      | hold           | hold          |
| PLSCNT7  | 3'b111        | 3'b111      | hold           | hold          |
| ITVL     | 3'b000        | 3'b011      | hold           | hold          |
| DWSEQ1   | 3'b000        | 3'b010      | hold           | hold          |
| END      | 3'b000        | 3'b000      | hold           | 1'b1          |

---

## 5. FSM Flow Diagram (Text)

```
                    ┌─────────────────────────────────────────────────────┐
                    │                                                     │
         reset      ▼                                                     │
        ──────► [INIT] ──w_start=1──► [WSEQ0] ──cntup=1──► [PLSCNT0]   │
                                         │                     │  ▲      │
                                    setupskip=1                │  │      │
                                    (stay/skip)           popt=1  │      │
                                                          lpup=0  │      │
                                                               ▼  │      │
                                                          [PLSCNT1]      │
                                                               │         │
                                                              ...        │
                                                               │         │
                                                          [PLSCNT7]      │
                                                               │         │
                                                         itvlskip=0      │
                                                               │         │
                                                               ▼         │
                                                          [PLSCNT0] ◄────┘
                                                               │
                                                          popt=1,lpup=1
                                                               │
                                                               ▼
                                                           [ITVL] ──cntup=1──► [PLSCNT0]
                                                               │
                                                          itvlskip=1
                                                               │
                                                               ▼
                                                          [PLSCNT0]

    Other conditions from any PLSCNTn:
    ──────────────────────────────────
    PLSCNTn ──other──► [DWSEQ0] ──cntup=1──► [PLSCNT0]
                                          └──► [DWSEQ1] ──cntup=1──► [END] ──cflag=1──► [INIT]
```

---

## 6. Key Behavioral Notes

1. **Wait Notification**: `lgrp_cntup_g` notifies the FSM that wait time has elapsed.
   In states without Wait, `lgrp_cntup_g = 1` is always set.

2. **PLSCNT and Interval**: When in PLSCNT0~7, after Tappl elapses (`cntup=1`),
   transition is determined by `lgrp_popt_or_g`, `lgrp_lpup_g`, `lgrp_itvlskip_g`.

3. **Skip WSEQ0**: When Tsetup = minimum (40ns), `lgrp_setupskip_g=1` → WSEQ0 skipped.

4. **hold**: Output retains the value from the previous state (registered output).

5. **Clock Domain**: All sequential logic clocked on `clkr_l` rising edge,
   active-low async reset `rstn_ws_l`.
