// FILENAME  fshell.c CONTENTS:   standard main program shell for test programs.  This module handles the system interface functions for the user test program. The user test program must provide the following public funtions and variables for use by this module: int DoTestProgram(void);        -- does test flow int DoProberAlignment(void);        -- does parametrics tests int DeviceSpecificInit(void);       -- called during init void DeviceSpecificPowerDown(void); -- called during PowerDown int DoSummary(void);            -- print and clear (begin) int DoPrintSummary(void);       -- print only (summary) int UserExCommand(int argc, char *argv[]); int REJ_BIN_LIST[];         -- list of pass bins. bin, rej, bin, rej, -1 rev 2.3 int  val_progrev;           -- program revision or -1. char *val_devtype           -- NULL or device type. char *program_name;         -- test program name. REVISION: 1.0 2/19/92 dja Versatest. - added slave mode support. 1.1 3/2/92 aka, dja Versatest. - modified PowerDown. 2.0 9/23/92 aka, dja Versatest. - changed PASS_BIN_LIST to REJ_BIN_LIST for compatability with future handlers where reject bin must be specified. - fixed bugs with debug variable.  == not =, and 0x80 not 80. - added debug test to initialize(0) in "init" section. - corrected semaphore use to semaphore 3. 4 d/n exist. - got rid of all if (xxx != FAIL) syntax. (!xxx). - added DIAGNOSTICS [LOT | WAFER | num] switch to option file so option file controls diag execution. 2.1   1/20/93 - Corrected slave mode / disable_unused_sites incompatability. System must not ever execute disable_slave_mode() command, nor run diagnostics in slave mode. 2.2   3/22/93 - Added user initialize functions after calls to diagnostics. - Added code to ignor "0", "1" values for X, Y ASR file names. - Added check for device type, revision. 2.3 7/1/93 - Added EX command support from Paul, Srini. - Cleaned up val_progrev problems. 2.4 7/7/93 - Modified the PowerDown routine to send reject bin instead of 0, or 1. 2.5 8/25/93 - Modified the PowerDown routine to allow both pass and fail bins to be specified in REJ_BIN_LIST without invalid operation when forced flow is enabled.  Reject bins specified in REJ_BIN_LIST will be treated as failure bins if any bit greater than 0xff is set. - Included shvar ex command to display the current status of the wafseq variables to the terminal. - Added check for argc >= 2 to all modify wafseq ex commands. 2.6 9/9/93 - modified "begin" to execute diags, calibration before host_begin() is executed.  This will insure that the next wafer is not loaded before diagnostics complete. COPYRIGHT (c) 1990, 92, 93 VERSATEST INC. ALL RIGHTS RESERVED.
// First thing in all programs should be dummy variable to avoid NULL pointers
int __DUMMY_VARIABLE;
char SHELL_VERSION[] = "2.5 (8/25/93)";

// include atomic definitions
#include <atomic.h>
#include <vscdef.h>

// Externals from user test program
extern int  DoTestProgram(void);
extern int  DoProberAlignment(void);
extern int  DeviceSpecificInit(void);
extern void DeviceSpecificPowerDown(void);
extern int  DoSummary(void);
extern int  DoPrintSummary(void);
extern int  UserExCommand(int argc, char *argv[]);
extern int  REJ_BIN_LIST[];
extern int  val_progrev;
extern char *val_devtype;
extern char *program_name;
extern int test;

// ***************** added hitachi maicon ltd 1994/12/27 ************** extern int   fof; extern int   relief; extern int   test_cond; extern FILE_T    *fpo; extern FILE_T    *fpd;

// #define macros
#define PASS      0
#define FAIL      -1

// #define macros 1995/07/12 m.kimura
// #define CHARACTERIZATION 3000

// ***************** End added hitachi maicon ltd  *******************

// Public variables
void PowerDown(int bin);
// Power down routine
int ovrhead;
// timer variables
int curtime, curtime2, curtime3;

// mode option variables
int dlog;
int lya;
int var1;
int var2;
int device;
int debug;

// Tester configuration
int io_process;
int test_site;
int test_mode;
int test_flow;
int test_type;
int forced_flow;
int leakage_test_flag;
int apc_test_flag;
int xy_location[2] = {0,0};

char    lot_name[0x20];
char    wafer_name[0x20];
char    device_name[0x20];
char    oper_name[0x20];
char    station_name[0x20];
char    tester_name[0x20];
char    probe_name[0x20];

// Local static variables
static  int diag_mode;
// 0 = never, 1=lot, 2=wafer, 3=count
static  int diag_count;
// count for mode 3
static  int diag_wafer;
// current count
// PowerDown() Called at end of test to bin out device.  If bin is in REJ_BIN_LIST Then the DeviceSpecificPowerDown is called and the device is binned out as a pass.  If the bin is not a pass bin then the forced_flow option will be tested. If forced_flow is not set, then the device will be binned out as a fail. bin -1 is used to cause a power down without stoping the test.
void PowerDown(int bin)
{
char    resp;
int tmptime;
int i;
int device_rej;
int device_failed;
    i = 0;
    device_failed = 1;
    device_rej = 1;
    while (REJ_BIN_LIST[i] != -1)
    {
        if (bin == REJ_BIN_LIST[i])
        {
            device_rej = REJ_BIN_LIST[i+1] & 0xff;
            device_failed = REJ_BIN_LIST[i+1] & ~0xff;
            break;
        }
        i += 2;
    }
    if ( forced_flow && device_failed ) {
        tmptime = bentime() - curtime - ovrhead;
        printf("Test Failed, bin %d, elapsed time: %d.%dms", bin,tmptime/1000,tmptime%1000);
        printf("Forced flow... continue test (y/n)?");
        fflush(stdout);
        flush_console();
        fflush(stdin);
        scanf("%c", &resp);
        if ( (resp == 'y') || (resp == 'Y') )
            return;
    }
    curtime = bentime() - curtime - ovrhead;
    curtime2 = curtime/1000;
    curtime3 = curtime2/1000;
    curtime2 = curtime2%1000;
    if ( ! device_failed ) {
        if (dlog || var1)
            printf("** PASS **, Bin=%d, Testtime: %d.%03dsec", bin,curtime3,curtime2);
        end_test(bin, device_rej);
    } else {
        if (dlog || var1)
              printf("-- FAIL --, Bin=%d, Testtime: %d.%03dsec", bin, curtime3,curtime2);
        end_test(bin, device_rej);
    }

}

// LoadHardwareFiles() This routine loads the pattern generator and buffer memory files as requested in the option file. returns:    0   files loaded ok. -1  file load error.
int LoadHardwareFiles(void)
{
char    optbuf[200];
int loadsum;
int expect_sum;
        // check for CROSSOVER file
    if ( (!read_option("CROSSOVER", optbuf)) && optbuf[0] )
    {
        printf("loading Crossover configuration file: %s", optbuf);
        if (load_crossover(optbuf))
        {
            printf("Failed XY crossover load");
            return(FAIL);
        }
    } else {
        printf("No XY crossover file found in option file");
    }
        // check for XASR file
    if ( (!read_option("XASR", optbuf)) && optbuf[0] )
    {
        if ( (sscanf(optbuf, "%i", &loadsum) != 1) ||
             ((loadsum != 0) && (loadsum != 1)) )
        {
            printf("loading X address scramble file: %s", optbuf);
            if (load_xasr(optbuf))
            {
                printf("Failed X ASR load");
                return(FAIL);
            }
        }
    } else {
        printf("No X ASR file found in option file", optbuf);
    }
        // check for YASR file
    if ( (!read_option("YASR", optbuf)) && optbuf[0] )
    {
        if ( (sscanf(optbuf, "%i", &loadsum) != 1) ||
             ((loadsum != 0) && (loadsum != 1)) )
        {
            printf("loading Y address scramble file: %s", optbuf);
            if (load_yasr(optbuf))
            {
                printf("Failed Y ASR load");
                return(FAIL);
            }
        }
    } else {
        printf("No Y ASR file found in option file");
    }
        // check for buffer memory PATTERN file
    if ( ( !read_option("PATTERN", optbuf) ) && optbuf[0] )
    {
        if ( (sscanf(optbuf, "%i", &loadsum) != 1) ||
             ((loadsum != 0) && (loadsum != 1)) )
        {
            printf("Loading buffer memory pattern: %s", optbuf);
            loadsum = load_bm(optbuf);
            if (!read_option("PATSUM", optbuf))
            {
                sscanf(optbuf, "%i", &expect_sum);
                if (loadsum != expect_sum)
                {
                    if (loadsum == FAIL)
                        printf("Fatal error loading pattern file");
                    else
                    {
                        printf("Pattern checksum does not match expected value");
                        printf("expected: %X, actual: %X", expect_sum, loadsum);
                    }
                    return(FAIL);
                }
            } else
                printf("!!! Warning: No checksum specified for pattern file");
        }
    } else {
        printf("No pattern file found in option file");
    }
        // check for vector file
    if ( (!read_option("VECTOR", optbuf)) && optbuf[0] )
    {
        printf("loading APG vector pattern file: %s", optbuf);
        if (load_apg(optbuf))
        {
            printf("Failed apg load");
            return(FAIL);
        }
    } else {
        printf("No APG pattern found in option file");
    }
    return(0);
}
// DoInitialSetup This function implements the "init" flow from the main test program.  The operatinig system will execute this flow when the test program is first loaded or the option file is changed. This routine should be used to re-initialize the system and place it in a known state ready to run the test program.
int DoInitialSetup(void)
{
char    optbuf[200];
int     use_slave_mode;
int     err;
int     i;
    printf("Versatest shell version %s", SHELL_VERSION);
    printf("Program: %s", program_name);
    if ( (val_devtype != NULL) && (val_progrev != -1) )
        printf(" Device: %s, Version: %d.%02d", val_devtype, val_progrev / 100, val_progrev % 100);
    else
        printf("");
    use_slave_mode = 0;
    if (!read_option("ENABLE_SLAVE_MODE", optbuf))
        if (strnicmp(optbuf, "OFF", 3) != 0)
            use_slave_mode = 1;
    if ( use_slave_mode )
    {
        diag_mode = 0;
        diag_count = 0;
        diag_wafer = 0;
        if (enable_slave_mode("init"))
            exit(3);
    } else {
                // Diagnostics must execute outside of slave mode
        diag_mode = 0;
        diag_count = 0;
        diag_wafer = 0;
        if (!read_option("DIAGNOSTICS", optbuf))
        {
            if (strnicmp(optbuf, "LOT", 3) == 0)
               diag_mode = 1;
            else if (strnicmp(optbuf, "WAFER", 4) == 0)
               diag_mode = 2;
            else if (sscanf(optbuf, "%d", &diag_count) == 1)
               diag_mode = 3;
            if (diag_mode == 1)
            {
                                // run diagnostics
                system("diag powerup");
                system("diag");
            }
        }
    }
    if ( !(debug & 0x80) &&
           read_option("NOINIT", optbuf) )
    {
        if (initialize(0) == FAIL)
        {
            printf("System failed initialization.");
            return(FAIL);
        }
    }
    if (LoadHardwareFiles())
        return(FAIL);
        // Get station configuration
    test_site = get_site_number();
    // init site number
    strcpy(optbuf, _READ_IOPROCID());
    if (strnicmp(optbuf, "VERSA", 5) == 0)
        io_process = 2;
    else
        if (strnicmp(optbuf, "TAHOE", 5) == 0)
            io_process = 1;
        else
            io_process = 0;
        // Load Station setup strings
    _read_wafseq(_RUN_OFS, 0x20, lot_name);
    _read_wafseq(_WAFER_OFS, 0x20, wafer_name);
    _read_wafseq(_DEVICE_OFS, 0x20, device_name);
    _read_wafseq(_OPER_OFS, 0x20, oper_name);
    _read_wafseq(_STATION_OFS, 0x20, station_name);
    _read_wafseq(_TESTER_OFS, 0x20, tester_name);
    _read_wafseq(_PROBE_OFS, 0x20, probe_name);
    if ( DeviceSpecificInit() != PASS )
        return(FAIL);
    err = 0;
    if ( ( val_devtype != NULL ) &&
         ( stricmp(device_name, val_devtype) != 0 ))
    {
        err = 1;
        printf("!!! Warning:  Test program device type does not match Plan file.");
    }
    if ( ( val_progrev >= 0 ) && ( val_progrev != _READ_VERSION() ) )
    {
        err = 1;
        printf("!!! Warning: Test program revision does not match Plan file.");
    }
        // Force error if version was incorrect
    if (err)
    {
        _read_wafseq(_VALIDATE_OFS, 0x2, (char *) &i);
        if (i & 0x20)
            return(FAIL);
    }
    return(0);
}
// procedure to update wafer sequence numeric variables
void    UpdateWafseq(int ofs, int len, char *msg, char *buf)
{
int cnt;
int i;
    i = 0;
    if ((buf[1] == 'x') || (buf[1] == 'X'))
        cnt = sscanf(&buf[2], "%x", &i);
    else
        cnt = sscanf(buf, "%u", &i);
    if (len == 2)
        i &= 0xffff;
    if (len == 1)
        i &= 0xff;
    printf("%s set: %X", msg, i);
    lock(3);
    _write_wafseq(ofs, len, (char *) &i);
    free(3);
}
// procedure to update wafer sequence string variables
void    UpdateWafString(int ofs, int len, char *msg, char *buf)
{
int cnt;
int i;
    if (buf != NULL)
    {
        if (strlen(buf) >= len)
            buf[len-1] = '\0';
        printf("%s: %s", msg, buf);
        lock(3);
        _write_wafseq(ofs, len, buf);
        free(3);
    }
}

 // *********************************************************************** Main test program entry point.    Put main at end of program    * so we don't have to forward reference all of the mode routines. * ***********************************************************************
void main(int argc, char *argv[])
{
char optbuf[0x40];
char *input;
int  i, cnt;
     // *************************************************************** Make sure we have the required program arguments   * ***************************************************************
    if (argc == 0)
    {
        printf("!!! EX command requires an argument !!!");
        printf("The Following Variables Can be Modified with EX command");
        printf("\tApc test flag     --  ex apc");
        printf("\tDatalog flag      --  ex dlog");
        printf("\tDebug control     --  ex debug");
        printf("\tDevice name       --  ex device");
        printf("\tForced flow flag  --  ex force");
        printf("\tLeakage test flag --  ex leak");
        printf("\tLot name          --  ex lot");
        printf("\tLYA control       --  ex lya");
        printf("\tOperator name     --  ex oper");
        printf("\tProbe card        --  ex probe");
        printf("\tShow Variables    --  ex shvar");
        printf("\tStation name      --  ex stat");
        printf("\tTest flow         --  ex flow");
        printf("\tTest Number       --  ex testnum");
        printf("\tTest mode         --  ex mode");
        printf("\tTester type       --  ex type");
        printf("\tVAR1 flag         --  ex var1");
        printf("\tVAR2 flag         --  ex var2");
        printf("\tWafer name        --  ex wafer");
        exit( PASS );
    }
     // **************************************************************** Start the elapsed test time bench mark timer         * ****************************************************************
    ovrhead = init_bentime();
    curtime = bentime();
     // **************************************************************** Re-initialize test site hardware so that test site is    * in a known state at the start of each test.      * (skip if debug bit 7 is set)                 * ****************************************************************
     if (!(debug & 0x80))
        initialize(0x10);

     // **************************************************************** Load program control variables from the 386 station  * control software.  This data will be available to the    * test program in the form of public variables.        * ****************************************************************
        // Load program control variables
    test_mode = read_mode();
    // DLOG, LYA, VAR1, VAR2 are set by the IO process depending upon the current test mode.  They can also be set from the terminal with the corresponding commands below.  These variables are intended to be used as bit masks with each bit enableing a different feature in the test program.  The terminal value and the mode dependant value are OR'ed togeather to generate the value used by the rest of the program.
    _read_wafseq(_DLOG_OFS, 0x4, (char *) &dlog);
    _read_wafseq(_DLOG_TERM_OFS, 0x4, (char *) &i);
    dlog |= i;
    _read_wafseq(_LYA_OFS, 0x4, (char *) &lya);
    _read_wafseq(_LYA_TERM_OFS, 0x4, (char *) &i);
    lya |= i;
    _read_wafseq(_VAR1_OFS, 0x4, (char *) &var1);
    _read_wafseq(_VAR1_TERM_OFS, 0x4, (char *) &i);
    var1 |= i;
    _read_wafseq(_VAR2_OFS, 0x4, (char *) &var2);
    _read_wafseq(_VAR2_TERM_OFS, 0x4, (char *) &i);
    var2 |= i;

    // These values are set when the plan file is loaded they can be changed from the terminal, but will remain changed until the plan file is re-loaded. These are short (16 bit) variables on the 386, but the V1000 only supports 32 bit integers so the data must be masked to 16 bits.
    _read_wafseq(_SORT_FLOW_OFS, 0x2, (char *) &test_flow);
    test_flow &= 0xffff;
    _read_wafseq(_SORT_TYPE_OFS, 0x2, (char *) &test_type);
    test_type &= 0xffff;
    _read_wafseq(_FORCE_FLOW_OFS, 0x2, (char *) &forced_flow);
    forced_flow &= 0xffff;
    _read_wafseq(_LKG_OFS, 0x2, (char *) &leakage_test_flag);
    leakage_test_flag &= 0xffff;
    _read_wafseq(_APC_OFS, 0x2, (char *) &apc_test_flag);
    apc_test_flag &= 0xffff;
        // Lot/ Wafer number will change so need to read each die
    _read_wafseq(_RUN_OFS, 0x20, lot_name);
    _read_wafseq(_WAFER_OFS, 0x20, wafer_name);
        // Current wafer map X, Y coordinates
    req_xy();
    console_gets(optbuf);
    sscanf(optbuf, "X:%dY:%d",
        &xy_location[0],
        &xy_location[1]);

     // **************************************************************** Determine which test command has been executed.      * ****************************************************************
        // "test" is used to tell us to execute the test flow.  It is the normal test flow
    if ( strnicmp(argv[0], "test", 4) == 0 )
    {
        DoTestProgram();
        printf("End of Program");
        exit(PASS);
        // just in case
    }
    // "init" is used when the program is first loaded or the option file is changed.  It is executed rarely to initialize or change the program setup.
    if ( strnicmp(argv[0], "init", 4) == 0 )
    {
        if (DoInitialSetup())
            exit(FAIL);
        else
            exit(PASS);
    }
    // "begin" is executed when the user does a print and clear summary operation or the prober sends an end of wafer signal.  It is used for the test program to print summary information and reset any variables for the next wafer.
    if ( strnicmp(argv[0], "begin", 4) == 0 )
    {
                // Re-load Station setup strings
        _read_wafseq(_DEVICE_OFS, 0x20, device_name);
        _read_wafseq(_STATION_OFS, 0x20, station_name);
        _read_wafseq(_TESTER_OFS, 0x20, tester_name);
        _read_wafseq(_PROBE_OFS, 0x20, probe_name);
        printf("%s", station_name);
        printf("Device: %s, Begin.", device_name);
        if ( DoSummary() != PASS )
            i = FAIL;
        else
            i = PASS;
                // check for diagnostics
        if (diag_mode == 2)
        {
            system("diag powerup");
            system("diag");
            if ( LoadHardwareFiles() )
                i = FAIL;
            if ( DeviceSpecificInit() != PASS )
                i = FAIL;
        } else if (diag_mode == 3)
        {
            if (++diag_wafer == diag_count)
            {
                diag_wafer = 0;
                system("diag powerup");
                system("diag");
                if ( LoadHardwareFiles() )
                    i = FAIL;
                if ( DeviceSpecificInit() != PASS )
                    i = FAIL;
            }
        }
                // reset and re-cal system
        initialize(0x18);
                // Tell host to execute summary
        host_begin();
        if (i)
            exit(FAIL);
        else
            exit(PASS);
    }
    // "print" is executed when the user does a print operation.  This should execute the same as the begin routine but should not clear or reset any variables.
    if ( strnicmp(argv[0], "print", 4) == 0 )
    {
        if (DoPrintSummary())
            exit(FAIL);
        else {
            host_summary();
            exit(PASS);
        }
    }
    // "funct0" is sent when the operator presses the function button on the front pannel.  This is intended for use as a prober alignment tool.
    if ( strnicmp(argv[0], "funct0", 6) == 0 )
    {
        if (DoProberAlignment())
            exit(FAIL);
        else
            exit(PASS);
    }
    if ( strnicmp(argv[0], "debug", 5) == 0 )
    {
        if ((argv[1][1] == 'x') || (argv[1][1] == 'X'))
            cnt = sscanf(argv[1], "%x", &i);
        else
            cnt = sscanf(argv[1], "%u", &i);
        debug=i;
        exit(PASS);
    }
    if (strnicmp( argv[0], "shvars", 4) == 0 )
    {
        printf("APC:    %8.8X", apc_test_flag);
        printf("Device: %s", device_name);
        printf("DLOG:   %8.8X", dlog);
        printf("Flow:   %4.4X", test_flow);
        printf("Force:  %8.8X", forced_flow);
        printf("LKG:    %8.8X", leakage_test_flag);
        printf("Lot:    %s", lot_name);
        printf("LYA:    %8.8X", lya);
        printf("Mode:   %4.4X", test_mode);
        printf("Oper:   %s", oper_name );
        printf("Probe:  %s", probe_name );
        printf("Station %s", station_name );
        printf("Tester  %s", tester_name );
        printf("Type:   %4.4X", test_type);
        printf("VAR1:   %8.8X", var1);
        printf("VAR2:   %8.8X", var2);
        printf("Wafer:  %s", wafer_name);
        printf("X: %d,  Y: %d", xy_location[0], xy_location[1]);
        exit (PASS);
    }

     // **************************************************************** The following commands are used to modify the setup  * from the terminal.  They are not allowed if validate     * bit 0x8 is set in the current plan file.        * ****************************************************************
    _read_wafseq(_VALIDATE_OFS, 0x2, (char *) &i);
    if ( ( (i & 0x8) == 0 ) && ( argc >= 2 ) )
    {
      if ( strnicmp(argv[0], "dlog", 4) == 0 )
      {
        UpdateWafseq(_DLOG_TERM_OFS, 4, "Datalog", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "lya", 3) == 0 )
      {
        UpdateWafseq(_LYA_TERM_OFS, 4, "Lya", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "var1", 4) == 0 )
      {
        UpdateWafseq(_VAR1_TERM_OFS, 4, "Var1", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "var2", 4) == 0 )
      {
        UpdateWafseq(_VAR2_TERM_OFS, 4, "Var2", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "force", 5) == 0 )
      {
        UpdateWafseq(_FORCE_FLOW_OFS, 2, "Forced flow", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "flow", 4) == 0 )
      {
        UpdateWafseq(_SORT_FLOW_OFS, 2, "Test flow", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "mode", 4) == 0 )
      {
        if (sscanf(argv[1], "%i", &i) == 1)
        {
            printf("Set Mode: %d", i);
            lock(0);
            set_mode(i);
            free(0);
        }
        exit(PASS);
      }
      if ( strnicmp(argv[0], "leak", 4) == 0 )
      {
        UpdateWafseq(_LKG_OFS, 2, "Leakage test flag", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "apc", 3) == 0 )
      {
        UpdateWafseq(_APC_OFS, 2, "APC test flag", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "lot", 3) == 0 )
      {
        UpdateWafString(_RUN_OFS, 0x20, "Lot number", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "wafer", 3) == 0 )
      {
        UpdateWafString(_WAFER_OFS, 0x20, "Wafer name", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "device", 3) == 0 )
      {
        UpdateWafString(_DEVICE_OFS, 0x20, "Device type", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "oper", 4) == 0 )
      {
        UpdateWafString(_OPER_OFS, 0x20, "Operator ID", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "stat", 4) == 0 )
      {
        UpdateWafString(_STATION_OFS, 0x20, "Station ID", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "type", 4) == 0 )
      {
        UpdateWafString(_TESTER_OFS, 0x20, "Tester type", argv[1]);
        exit(PASS);
      }
      if ( strnicmp(argv[0], "probe", 5) == 0 )
      {
        UpdateWafString(_PROBE_OFS, 0x20, "Probe card ID", argv[1]);
        exit(PASS);
      }
    }
    if (UserExCommand(argc, argv))
    {
        printf("The Following Variables Can be Modified with EX command");
        printf("\tApc test flag     --  ex apc");
        printf("\tDatalog flag      --  ex dlog");
        printf("\tDebug control     --  ex debug");
        printf("\tDevice name       --  ex device");
        printf("\tForced flow flag  --  ex force");
        printf("\tLeakage test flag --  ex leak");
        printf("\tLot name          --  ex lot");
        printf("\tLYA control       --  ex lya");
        printf("\tOperator name     --  ex oper");
        printf("\tProbe card        --  ex probe");
        printf("\tShow Variables    --  ex shvar");
        printf("\tStation name      --  ex stat");
        printf("\tTest flow         --  ex flow");
        printf("\tTest mode         --  ex mode");
        printf("\tTest number       --  ex num");
        printf("\tTester type       --  ex type");
        printf("\tVAR1 flag         --  ex var1");
        printf("\tVAR2 flag         --  ex var2");
        printf("\tWafer name        --  ex wafer");
    }
    exit(PASS);
}

