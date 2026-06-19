// ****************************************************************************
// ! @file RX200slc_MonitorTrimingSelect.c @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system @author K.Watanabe @date 2012.04.16 Rev.00
// ****************************************************************************
int MonitorTrimingSelect(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int test_cond, result;
  int i,j,k, cnt;
  int tap,deb0,deb1;
  int select_target,selvol,selvol_step,module_loop,delay_result[3];
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,percr0,percr1;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11,stack12;
  // debug add
  int vol[V_VPP_TF+1];
  // Monitored Voltages
  int trimvalue[V_VPP_TF+1];
  // Triming Target Voltages
  int logparam[20],testcond[10];
  int mon_rosc_pump=0;
  int mon_rosc_pump1=0;
  int mon_rosc_pump2=0;
  int speed, speed_bak, pllon_bak, pll_bak, pllon,etcr11_bak,ffpt_bak,vcc_bak,etlr26_bk1,etlr26_bk2;
  int repeat_mode,first_flag,inif_flag,err_flag,pass_flag,temp_check,data_num,vcc_num;
  int monitor_mode;
  int data_array[30][60],vcc_array[10];
  int info[NUM_CHIPINFO],Vresult[BGR_ARRAY_VCC];
  char chip_name[100],Lname[50],file_name[200],file_name2[200],file_name3[200], buff[300];
  char moni_pin[30];
  int vsum[16],vnum,judge_max,judge_min,bgr_reg,reg_st,reg_en,reg_inc,judge;

    // **************************************************************************
    // INITIALIZE VALIABLE PARAMETOR
    // **************************************************************************
  test_cond = INIT_COND;
  // INITIALIZE test_cond
  tap = selvol = select_target = tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = 0;
  pllon = GetValue( IN_PLLON, param );
  for(cnt=1;cnt<=V_VPP_TF;cnt++){
  // INITIALIZE All vol[n]
    vol[cnt] = 0;
  }
  for(cnt=1;cnt<=V_VPP_TF;cnt++){
  // INITIALIZE All trimvalue[n]
    trimvalue[cnt] = 0;
  }

  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = stack7 = stack8 = stack9 = stack10 = stack11 = stack12 = 0;
  
    // **************************************************************************
    // Set Auto Triming Target Value i
    // **************************************************************************
  trimvalue[V_VDD]                = TARGET_VDD_25C;
  trimvalue[V_VDDH]               = TARGET_VDDH_25C;
  trimvalue[V_VREF10]             = TARGET_VREF10_25C;
  trimvalue[V_VPP_E]              = TARGET_VPP_E_25C;
  trimvalue[V_VPP_P]              = TARGET_VPP_P_25C;
  trimvalue[V_VPP_PW]             = TARGET_VPP_PW_25C;
  trimvalue[V_VHH_E]              = TARGET_VHH_E_25C;
  trimvalue[V_VHH_P]              = TARGET_VHH_P_25C;
  trimvalue[V_VRSG]               = TARGET_VRSG_25C;
  trimvalue[V_V33R]               = TARGET_V33R_25C;
  trimvalue[V_VWI]                = TARGET_VWI_25C;
  trimvalue[V_VDEMG]              = TARGET_VDEMG_25C;
  trimvalue[V_VNOEMI_PE]          = TARGET_VNOEMI_25C;
  trimvalue[I_IMONI_I01U_NOTEMP]  = TARGET_IREF_25C;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;
  trimvalue[I_IMONI_I01U_TEMP]    = TARGET_IREF_T_25C;
  trimvalue[I_IMONI_FLASH_TEMP]   = TARGET_IREF_T_F_25C;
  trimvalue[T_OSC32]              = TARGET_OSC32;
  trimvalue[T_OSC1]               = TARGET_OSC1;
  trimvalue[T_READDELAY8]         = TARGET_DELAY8_25C;
  trimvalue[T_READDELAY10]        = TARGET_DELAY10_25C;
  trimvalue[V_VPP_TF]             = TARGET_VPP_TF_25C;

    // **************************************************************************
    // TestCond
    // **************************************************************************
  testcond[0]=7;
  testcond[1]=8;
  testcond[2]=9;
  testcond[3]=21;
  testcond[4]=22;
  testcond[5]=30;
  testcond[6]=31;
  testcond[7]=32;
  testcond[8]=36;



  etcr11_bak = SetValue(IN_ETCR11,param,(GetValue(IN_ETCR11,param) | 0x04));
  // ETCR11[2] CountStop




    // **************************************************************************
    // Ground Menu
    // **************************************************************************
  while((test_cond != QUIT)&&(test_cond != EXIT)){
    printf("+-[monitor function]----+--------+ +-------+--------+------------+--------------------+");
    printf("| No| Item              |  Mon   | |Monitor| TARGET |   Trim     |   Tokusei          |");
    printf("|   |                   |        | |  Pad  |        | Man | Auto |vs-Tap| vs-V | Temp |");
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("|  2|ATB                |%6dmV| |  CH2  |        |  -  |   -  |   -  |   -  |   -  |",vol[V_BGR08]);
    //ő҂ 
    printf("|  4|VDDH               |%6dmV| |VCLMON |        |  -  |   -  |   -  |  504 |   -  |",vol[V_VDDH]);
    printf("|  5|Vref10             |%6dmV| |VSSMON |%6dmV|  -  |   -  |   -  |  505 |   -  |",vol[V_VREF10],trimvalue[V_VREF10]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("|  7|vpp (Erase)        |%6dmV| |VSSMON |%6dmV|  -  |   -  |  507 |   -  |   -  |",vol[V_VPP_E],trimvalue[V_VPP_E]);
    printf("|  8|vpp (Write)        |%6dmV| |VSSMON |%6dmV|  -  |   -  |  508 |   -  |   -  |",vol[V_VPP_P],trimvalue[V_VPP_P]);
    printf("|  9|vpp (Pre-Write)    |%6dmV| |VSSMON |%6dmV|  -  |   -  |  509 |   -  |   -  |",vol[V_VPP_PW],trimvalue[V_VPP_PW]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 21|Vhh (Erase)        |%6dmV| |VCCMON |%6dmV|  -  |   -  |  521 |   -  |  721 |",vol[V_VHH_E],trimvalue[V_VHH_E]);
    printf("| 22|Vhh (Write)        |%6dmV| |VCCMON |%6dmV|  -  |   -  |  522 |   -  |  722 |",vol[V_VHH_P],trimvalue[V_VHH_P]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 30|Vrsg               |%6dmV| |VCCMON |%6dmV|  -  |   -  |  530 |   -  |   -  |",vol[V_VRSG],trimvalue[V_VRSG]);
    printf("| 31|V33r               |%6dmV| |VCCMON |%6dmV|  -  |   -  |  531 |   -  |   -  |",vol[V_V33R],trimvalue[V_V33R]);
    printf("| 32|Vwi                |%6dmV| |VCCMON |%6dmV|  -  |   -  |  532 |   -  |   -  |",vol[V_VWI],trimvalue[V_VWI]);
    printf("| 33|Vdemg              |%6dmV| |VSSMON |%6dmV|  -  |   -  |  533 |   -  |   -  |",vol[V_VDEMG],trimvalue[V_VDEMG]);
    //Add trim VDEMG 2018/0327 
        //printf("| 35|Vnoemi             |%6dmV| |VCCMON |%6dmV|  -  |   -  |   -  |   -  |   -  |",vol[V_VNOEMI],trimvalue[V_VNOEMI]);
    printf("| 36|Vnoemi(PE)         |%6dmV| |VCCMON |%6dmV|  -  |   -  |  536 |   -  |   -  |",vol[V_VNOEMI_PE],trimvalue[V_VNOEMI_PE]);
        //printf("| 37|Vnoemi(Read)       |%6dmV| |VCCMON |%6dmV|  -  |   -  |   -  |   -  |   -  |",vol[V_VNOEMI_R],trimvalue[V_VNOEMI_R]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 40|imoni(I01U)  notemp|%6dnA| |VSSMON |%6dnA|  -  |   -  |  540 |   -  |   -  |",vol[I_IMONI_I01U_NOTEMP],trimvalue[I_IMONI_I01U_NOTEMP]);
    printf("| 41|imoni(Flash) notemp|%6dnA| |VSSMON |%6dnA|  -  |   -  |  541 |   -  |   -  |",vol[I_IMONI_FLASH_NOTEMP],trimvalue[I_IMONI_FLASH_NOTEMP]);
    printf("| 42|imoni(I01U)  temp  |%6dnA| |VSSMON |%6dnA|  -  |   -  |  542 |   -  |   -  |",vol[I_IMONI_I01U_TEMP],trimvalue[I_IMONI_I01U_TEMP]);
    printf("| 43|imoni(Flash) temp  |%6dnA| |VSSMON |%6dnA|  -  |   -  |  543 |   -  |  743 |",vol[I_IMONI_FLASH_TEMP],trimvalue[I_IMONI_FLASH_TEMP]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 50|OSC_32MHz          |%d.%03dMHz| |   -   |%6dus|  -  |   -  |  550 |   -  |   -  |",vol[T_OSC32]*4/trimvalue[T_OSC32],(vol[T_OSC32]*4%trimvalue[T_OSC32])*1000/trimvalue[T_OSC32],trimvalue[T_OSC32]);
    printf("| 51|OSC_1MHz           |%d.%03dMHz| |   -   |%6dus|  -  |   -  |  551 |   -  |   -  |",vol[T_OSC1]/trimvalue[T_OSC1],(vol[T_OSC1]%trimvalue[T_OSC1])*1000/trimvalue[T_OSC1],trimvalue[T_OSC1]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 60|ReadDelay 8ns FLP0 |%6dns| |VSSMON |%6dns|  -  |   -  |  562 |   -  |   -  |",vol[T_READDELAY8_FLP0],trimvalue[T_READDELAY8]);
    printf("|   |              FLI1 |%6dns| |VSSMON |%6dns|  -  |   -  |  563 |   -  |   -  |",vol[T_READDELAY8_FLI1],trimvalue[T_READDELAY8]);
    printf("|   |              FLI2 |%6dns| |VSSMON |%6dns|  -  |   -  |  564 |   -  |   -  |",vol[T_READDELAY8_FLI2],trimvalue[T_READDELAY8]);
    printf("| 61|ReadDelay10ns FLP0 |%6dns| |VSSMON |%6dns|  -  |   -  |  565 |   -  |   -  |",vol[T_READDELAY10_FLP0],trimvalue[T_READDELAY10]);
    printf("|   |              FLI1 |%6dns| |VSSMON |%6dns|  -  |   -  |  566 |   -  |   -  |",vol[T_READDELAY10_FLI1],trimvalue[T_READDELAY10]);
    printf("|   |              FLI2 |%6dns| |VSSMON |%6dns|  -  |   -  |  567 |   -  |   -  |",vol[T_READDELAY10_FLI2],trimvalue[T_READDELAY10]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("| 70|Vpp Tf (Only Trim) |%3d.%d ns| |   -   |%6dus|  -  |   -  |  670 |   -  |   -  |",vol[V_VPP_TF]/1000,(vol[V_VPP_TF]/100)%10,trimvalue[V_VPP_TF]);
    printf("+---+-------------------+--------+ +-------+--------+-----+------+------+------+------+");
    printf("+---------------------------------------------------------------------------------------------------------------+");
        //printf("| Monitor All(99)  Manual Triming ALL (299) Manual Trim͖                                               |");
    printf("| Monitor All(99) / Monitor All(With out Delay-MON)(98) / Monitor Data Out(198)                                 |");
    printf("| Auto Triming (200 + n)                                                                                        |");
    printf("| Auto Triming All(300) / Set Target Value(301)                                                                 |");
    printf("| Iref Temp Abso Trimming(643) / Iref Temp ALL monitor (600) / Iref Trim Rext Mode(602)                         |");
    printf("| Iref&VhhW/E Temp Monitor ALL(603) / VhhW Temp Monitor ALL(604)                                                |");
    printf("| Vnoemi Leak Monitor(601)                                                                                      |");
    printf("| Pump Current(800) / PumpCurrent zitan(801) / PumpCurrent vccALL(802)                                                                    |");
    printf("+---------------------------------------------------------------------------------------------------------------+");
    TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+-------+---------------------------------------------------------------------------------------------+");
    printf("| exit(e)/quit(q)/system(sy)/save_Reg.(sreg)/load_Reg.(lreg)/logcap:%1d(log)", Ffpt);
    printf("+-----------------------------------------------------------------------------------------------------+");
    printf("Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();
    BL_DATACLEAR(param);

        // ************************************************************************
        // Commans Analisys
        // ************************************************************************
    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);
        // ************************************************************************
        // RegSetMonitor  Add 2018/0419
        // ************************************************************************
    if((test_cond>500) && (test_cond<600)){
      printf("Reg Set Mon");
      if(test_cond >=550){
        MONIvsTap(test_cond-500,&vs[0],waittime,param);
      }else{
        printf("Input Tempurature -> ");DigitInput(&tempo1);
        MonitorALL_Tap(&vs[0],param,test_cond - 500,tempo1);
      }

    }else if((test_cond>700) && (test_cond<800)){
      printf("Input Tempurature -> ");DigitInput(&tempo1);
      MonitorALL_Tap(&vs[0],param,test_cond - 500,tempo1);
// printf("Reg Set Mon"); MONIvsTap(test_cond-500,&vs[0],waittime,param); }else if((test_cond==721) || (test_cond==722)){ MONIvsTap(test_cond,&vs[0],waittime,param);
    }
    switch(test_cond){
    case EXIT:
    // **** Exit **************************
      SetValue(IN_ETCR11,param,etcr11_bak);
      // ETCR11[2] CountStop
      return(PASS);
      break;
    case QUIT:
    // **** Quit **************************
      DeviceSpecificPowerDown();
      if(Ffpt==2){
        if(fclose(Fptdata)){
          exit(-99);
        }
      }
      PowerDown(1);
      break;

        // ************************************************************************
        // MONITOR(separate) Add 20180320
        // ************************************************************************
    case 2:
      printf("==== MONITOR ATB Menu ====");
      printf(" (0) : BGR790_direct");
      printf(" (1) : VDD_ISO1");
      printf(" (2) : VDD_ISO2");
      printf(" (3) : VDD_ISO3{Flash}");
      printf(" (4) : BGR790_LVD");
      printf(" (5) : VDDH_ISO1");
      printf(" (6) : VDDH_ISO2");
      printf(" (7) : VDDH_ISO3{Flash}");
      printf(" Select Monitor No, ->");DigitInput(&tempo1);
      SetValue(IN_TESTSEL,param,MONITOR_BGR);
      SetValue(IN_BDATA00,param,tempo1);
      SetValue(IN_BDATA01,param,0);
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("atb_monitor"),"MONITOR_ATB",&selvol);
      printf(" Result = %dmV",selvol);
      vol[V_BGR08] = selvol;
      break;

    case 3:
      printf("MONITOR VDD");
      SetValue(IN_TESTSEL,param,MONITOR_VDD);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
      vol[V_VDD] = selvol;
      break;

    case 4:
      printf("MONITOR VDDH");
      SetValue(IN_TESTSEL,param,MONITOR_VDDH);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDDH",&selvol);
      vol[V_VDDH] = selvol;
      break;

    case 5:
      printf("MONITOR Vref10");
      SetValue(IN_TESTSEL,param,MONITOR_VREF10);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Vref10",&selvol);
      vol[V_VREF10] = selvol;
      break;

    case 7:
      printf("MONITOR Vpp Erase");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol);
      vol[V_VPP_E] = selvol;
      break;

    case 8:
      printf("MONITOR Vpp Write");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol);
      vol[V_VPP_P] = selvol;
      break;

    case 9:
      printf("MONITOR Vpp Pre-Write");
      SetValue(IN_TESTSEL,param,MONITOR_VPP_PW);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_PW",&selvol);
      vol[V_VPP_PW] = selvol;
      break;

    case 21:
      printf("MONITOR Vhh Erase");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol);
      vol[V_VHH_E] = selvol;
      break;

    case 22:
      printf("MONITOR Vhh Write");
      SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol);
      vol[V_VHH_P] = selvol;
      break;

    case 30:
      printf("MONITOR Vrsg");
      SetValue(IN_TESTSEL,param,MONITOR_VRSG);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VRSG",&selvol);
      vol[V_VRSG] = selvol;
      break;

    case 31:
      printf("MONITOR V33R");
      SetValue(IN_TESTSEL,param,MONITOR_V33R);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_V33R",&selvol);
      vol[V_V33R] = selvol;
      break;

    case 32:
      printf("MONITOR Vwi");
      SetValue(IN_TESTSEL,param,MONITOR_VWI);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VWI",&selvol);
      vol[V_VWI] = selvol;
      break;

    case 33:
      printf("MONITOR Vdemg");
      SetValue(IN_TESTSEL,param,MONITOR_VDEMG);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VDEMG",&selvol);
      vol[V_VDEMG] = selvol;
      break;

    case 34:
      printf("MONITOR Vreg20");
      SetValue(IN_TESTSEL,param,MONITOR_VREG20);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VREG20",&selvol);
      vol[V_VREG20] = selvol;
      break;

    case 36:
      printf("MONITOR Vnoemi(PE)");
      SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_PE",&selvol);
      vol[V_VNOEMI_PE] = selvol;
      break;
    
        //case 37:                          // Vnoemi(Read)̓j^s
        //  printf("MONITOR Vnoemi(Read)");
        //  SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_R);
        //  result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_R",&selvol);
        //  vol[V_VNOEMI_R] = selvol;
        //  break;

    case 40:
      printf("MONITOR imoni(I01U) notemp");
      SetValue(IN_TESTSEL,param,MONITOR_IMONI_I01U_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_NOTEMP",&selvol);
      if( selvol>=24000 ){
        SetValue(IN_BDATA15,param,UA250);
        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_NOTEMP",&selvol);
      }
      vol[I_IMONI_I01U_NOTEMP] = selvol;
      SetValue(IN_BDATA15,param,0);
      break;

    case 41:
      printf("MONITOR imoni(Flash) notemp");
      SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
      if( selvol>=24000 ){
        SetValue(IN_BDATA15,param,UA250);
        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol);
      }
      vol[I_IMONI_FLASH_NOTEMP] = selvol;
      SetValue(IN_BDATA15,param,0);
      break;

    case 42:
      printf("MONITOR imoni(I01U) temp");
      SetValue(IN_TESTSEL,param,MONITOR_IMONI_I01U_TEMP);
      SetValue(IN_BDATA15,param,UA25);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_TEMP",&selvol);
      if( selvol>=24000 ){
        SetValue(IN_BDATA15,param,UA250);
        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_TEMP",&selvol);
      }
      vol[I_IMONI_I01U_TEMP] = selvol;
      SetValue(IN_BDATA15,param,0);
      break;

    case 43:
      printf("MONITOR imoni(Flash) temp");
      SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP);
      SetValue(IN_BDATA15,param,UA25);
      result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
      if( selvol>=24000 ){
        SetValue(IN_BDATA15,param,UA250);
        result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol);
      }
      vol[I_IMONI_FLASH_TEMP] = selvol;
      SetValue(IN_BDATA15,param,0);
      break;

    case 50:
      printf("Monitor OSC_32MHz");
      SetValue(IN_TESTSEL,param,MONITOR_OSC32);
      SetValue(IN_LDATA0,param,268*10);
      // Target=270us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("monitorcycle_at"),"MONITOR_OSC32");
      result = 0xFFFF-GetValue(OUT_LDATA0,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA0,param), result );
      vol[T_OSC32] = result;
      break;

    case 51:
      printf("Monitor OSC_1MHz");
      SetValue(IN_TESTSEL,param,MONITOR_OSC1);
      SetValue(IN_LDATA0,param,98*10);
      // Target=100us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("monitorcycle_at"),"MONITOR_OSC1");
      result = 0xFFFF-GetValue(OUT_LDATA1,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA1,param), result );
      vol[T_OSC1] = result;
      break;

    case 60:
    case 61:
      stack10 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      stack11 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      stack12 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      SetValue(IN_LDATA0,param,100*10000);
      // 100ms(10000*100ns=1ms)
      if(test_cond==60){
        printf("MONITOR Read Delay8");
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY8);
        // AUTOTRIM_READDELAY8 = MONITOR_READDELAY8
        i = 0;
      }else{
        printf("MONITOR Read Delay10");
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY10);
        // AUTOTRIM_READDELAY10 = MONITOR_READDELAY10
        i = 1;
      }
            //printf("Select Module (0:FLP0 / 1:FLI1 / 2:FLI2 / 3:ALL) -> ");
            //DigitInput(&tempo0);
      tempo0 = 0;
      if( tempo0==3 ) {tempo0 = 0; tempo1 = 2;}
      else            {tempo1 = tempo0;}
      printf("Which Reference Signal? (0:DELAYTEST / 1:VSSMON) -> ");
      DigitInput(&tempo);
      stack0=SetValue(IN_BDATA00,param,tempo);
      printf("Enter selvol step[ns](%dns(1tap) or 1ns(high accuracy) or other -> ", 6+3*i);
      DigitInput(&selvol_step);
      if( selvol_step>(6+3*i) ) selvol_step = (6+3*i);
        for(module_loop=tempo0; module_loop<=tempo1; module_loop++){
          first_flag =1;
          pass_flag = err_flag = cnt =0;
          if(test_cond==60) selvol = trimvalue[T_READDELAY8];
          else              selvol = trimvalue[T_READDELAY10];
          while(1){
        if(first_flag==1){
        // Initial mode
          first_flag = 0;
          repeat_mode =INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
	    }else{
          repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
        }
            if( tempo==1 ) CpuModeMonitor_Repeat(repeat_mode,VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcycle_at"),"MONITOR_READDELAY(VSSMON)",&selvol);
            else           CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcycle_at"),"MONITOR_READDELAY(DELAYTEST)",&selvol);
            delay_result[module_loop] = (GetValue(OUT_JUDGE1,param)>>8)&(1<<module_loop);
            result = ((0x00FF&GetValue(OUT_JUDGE1,param))<<8)|((0xFF00&GetValue(OUT_JUDGE1,param))>>8);
            if(test_cond==60) result = result & 0x170;
            else              result = result & 0x180;

           	if( GetValue(OUT_JUDGE1,param) == 0x00 ){
              pass_flag = 1;
              if(err_flag!=1){
                printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol+selvol_step);
                selvol=selvol+selvol_step;
              	 judge = 1;
	             }
            }else if(result == 0x170 || result == 0x180 ){
              if( delay_result[module_loop]==0x00 ){
                pass_flag = 1;
                if(err_flag!=1){
                  printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol+selvol_step);
                  selvol=selvol+selvol_step;
                 	judge = 1;
		              }
              }else if( delay_result[module_loop]==(1<<module_loop)) {
                err_flag = 1;
                if(pass_flag!=1){
                  printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol-selvol_step);
                  selvol=selvol-selvol_step;
                 	judge = 0;
		              }
              }
            }else{
              printf(" Monitor Error");
              selvol = 0;
              pass_flag=1;
              err_flag==1;
            }

            if( (pass_flag==1)&&(err_flag==1) ) break;
            cnt++;
            if( (selvol<0)||(cnt>(15*(i+1))*((6+3*i)/selvol_step)) ){
              selvol=0;
              printf( " Monitor Error !!" );
              break;
            }
          }
        if(test_cond==60) vol[T_READDELAY8_FLP0+module_loop]  = selvol - judge;
        else              vol[T_READDELAY10_FLP0+module_loop] = selvol - judge;
        DeviceLevelsPowerDown();
        // edOFFR}h
      }
      SetValue(IN_BDATA00,param,stack0);
      SetValue(IN_TESTSIZE0,param,stack10);
      SetValue(IN_TESTSIZE1,param,stack11);
      SetValue(IN_TESTSIZE2,param,stack12);
      break;

      case 70:
        CpuModeMonitor_VppTf(MNORMAL,&vs[0],param,W1SEC*3,CreatePatFileName("VppTf_Check"),"VppTf Monitor",&selvol);
        vol[V_VPP_TF] = selvol * GetValue(IN_EXTAL1,param);
      break;
      case 670:
        printf("Input Tempurature ->");DigitInput(&tempo0);
        VppTf_vs_Tap(&vs[0],param,0,tempo0);
      break;

        // ************************************************************************
        // MONITOR(Tester)
        // ************************************************************************
    case 98:
    // Monitor ALL
    case 99:
    // Monitor ALL
      SetValue(IN_BDATA00,param,0);
      printf("MONITOR ALL");

            // Monitor VDD
      printf("MONITOR VDD");
      SetValue(IN_TESTSEL,param,MONITOR_VDD);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDD",&selvol);
      vol[V_VDD] = selvol;

            // Monitor VDDH
      printf("MONITOR VDDH");
      SetValue(IN_TESTSEL,param,MONITOR_VDDH);
      SetValue(IN_BDATA00,param,0);
      result = CpuModeMonitor(VCLMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VDDH",&selvol);
      vol[V_VDDH] = selvol;

      SetValue(IN_TESTSEL,param,99);
      //dnSăj^[
      SetValue(IN_LDATA0,param,12);
      //12ڂœdj^[Ɉڍs
      SetValue(IN_BDATA05,param,13);
      //14Repeat
      SetValue(IN_LDATA1,param,7);
      //7ڂVSSMONɐ؂ւ
      CpuModeMonitor_ALL(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("MonitorALL_Voltage"),"AUTOTRIM_VWI",&vsum[0]);
      result = GetValue(OUT_JUDGE1,param);
	    for(i=0;i<=GetValue(IN_BDATA05,param);i++){
        switch (i){
        case 0 : vol[V_VHH_P]     = vsum[i]; break;
        case 1 : vol[V_VHH_E]     = vsum[i]; break;
        case 2 : vol[V_VNOEMI_PE] = vsum[i]; break;
        case 3 : vol[V_VREG20]    = vsum[i]; break;
        case 4 : vol[V_VWI]       = vsum[i]; break;
        case 5 : vol[V_V33R]      = vsum[i]; break;
        case 6 : vol[V_VRSG]      = vsum[i]; break;
        case 7 : vol[V_VPP_P]     = vsum[i]; break;
        case 8 : vol[V_VPP_E]     = vsum[i]; break;
        case 9 : vol[V_VPP_PW]    = vsum[i]; break;
        case 10 : vol[V_VDEMG]    = vsum[i]; break;
        case 11 : vol[V_VREF10]   = vsum[i]; break;
        case 12 : vol[I_IMONI_FLASH_NOTEMP]   = vsum[i]; break;
        case 13 : vol[I_IMONI_FLASH_TEMP]   = vsum[i]; break;
        }
	    }
      
      // // Monitor Vref10 printf("MONITOR Vref10"); SetValue(IN_TESTSEL,param,MONITOR_VREF10); SetValue(IN_BDATA00,param,0); result = CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_Vref10",&selvol); vol[V_VREF10] = selvol;   // Monitor Vpp Erase printf("MONITOR Vpp Erase"); SetValue(IN_TESTSEL,param,MONITOR_VPP_E); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_E",&selvol); vol[V_VPP_E] = selvol;   // Monitor Vpp Write printf("MONITOR Vpp Write"); SetValue(IN_TESTSEL,param,MONITOR_VPP_P); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_P",&selvol); vol[V_VPP_P] = selvol;   // Monitor Vpp Pre-Write printf("MONITOR Vpp Pre-Write"); SetValue(IN_TESTSEL,param,MONITOR_VPP_PW); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VPP_PW",&selvol); vol[V_VPP_PW] = selvol;   // Monitor Vhh Erase printf("MONITOR Vhh Erase"); SetValue(IN_TESTSEL,param,MONITOR_VHH_E); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_E",&selvol); vol[V_VHH_E] = selvol;   // Monitor Vhh Write printf("MONITOR Vhh Write"); SetValue(IN_TESTSEL,param,MONITOR_VHH_P); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VHH_P",&selvol); vol[V_VHH_P] = selvol;   // Monitor Vrsg printf("MONITOR Vrsg"); SetValue(IN_TESTSEL,param,MONITOR_VRSG); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VRSG",&selvol); vol[V_VRSG] = selvol;   // Monitor V33r printf("MONITOR V33R"); SetValue(IN_TESTSEL,param,MONITOR_V33R); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_V33R",&selvol); vol[V_V33R] = selvol;   // Monitor Vwi printf("MONITOR Vwi"); SetValue(IN_TESTSEL,param,MONITOR_VWI); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VWI",&selvol); vol[V_VWI] = selvol;   // Monitor Vdemg printf("MONITOR Vdemg"); SetValue(IN_TESTSEL,param,MONITOR_VDEMG); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VDEMG",&selvol); vol[V_VDEMG] = selvol;   // Monitor Vdeg20 printf("MONITOR Vreg20"); SetValue(IN_TESTSEL,param,MONITOR_VREG20); result = CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V_VREG20",&selvol); vol[V_VREG20] = selvol;   // Monitor Vnoemi(PE) printf("MONITOR Vnoemi(PE)"); SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE); CpuModeMonitor(VCCMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_PE",&selvol); vol[V_VNOEMI_PE] = selvol;

            // Monitor imoni(I01U) notemp
            //printf("MONITOR imoni(I01U) notemp");
            //SetValue(IN_TESTSEL,param,MONITOR_IMONI_I01U_NOTEMP);
            //SetValue(IN_BDATA15,param,UA25);
            //CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_NOTEMP",&selvol);
            //if( selvol>=24000 ){
            //  SetValue(IN_BDATA15,param,UA250);
            //  CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_NOTEMP",&selvol);
            //}
            //vol[I_IMONI_I01U_NOTEMP] = selvol;
            //SetValue(IN_BDATA15,param,0);
      
            // Monitor imoni(Flash) notemp
	  // printf("MONITOR imoni(Flash) notemp"); SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_NOTEMP); SetValue(IN_BDATA15,param,UA25); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol); if( selvol>=24000 ){ SetValue(IN_BDATA15,param,UA250); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_NOTEMP",&selvol); } vol[I_IMONI_FLASH_NOTEMP] = selvol; SetValue(IN_BDATA15,param,0);
            // Monitor imoni(I01U) temp
            //printf("MONITOR imoni(I01U) temp");
            //SetValue(IN_TESTSEL,param,MONITOR_IMONI_I01U_TEMP);
            //SetValue(IN_BDATA15,param,UA25);
            //CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_TEMP",&selvol);
            //if( selvol>=24000 ){
            //  SetValue(IN_BDATA15,param,UA250);
            //  CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_I01U_TEMP",&selvol);
            //}
            //vol[I_IMONI_I01U_TEMP] = selvol;
            //SetValue(IN_BDATA15,param,0);

            // Monitor imoni(Flash) temp
	  // printf("MONITOR imoni(Flash) temp"); SetValue(IN_TESTSEL,param,MONITOR_IMONI_FLASH_TEMP); SetValue(IN_BDATA15,param,UA25); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol); if( selvol>=24000 ){ SetValue(IN_BDATA15,param,UA250); CpuModeMonitor(VSSMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcurrent_at"),"MONITOR_I_IMONI_FLASH_TEMP",&selvol); } vol[I_IMONI_FLASH_TEMP] = selvol; SetValue(IN_BDATA15,param,0);
            // Monitor HOSC(32MHz)
      printf("Monitor OSC_32MHz");
      SetValue(IN_TESTSEL,param,MONITOR_OSC32);
      SetValue(IN_LDATA0,param,268*10);
      // Target=270us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("monitorcycle_at"),"MONITOR_OSC32");
      result = 0xFFFF-GetValue(OUT_LDATA0,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA0,param), result );
      vol[T_OSC32] = result;

            // Monitor LOSC(1MHz)
      printf("Monitor OSC_1MHz");
      SetValue(IN_TESTSEL,param,MONITOR_OSC1);
      SetValue(IN_LDATA0,param,98*10);
      // Target=100us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("monitorcycle_at"),"MONITOR_OSC1");
      result = 0xFFFF-GetValue(OUT_LDATA1,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA1,param), result );
      vol[T_OSC1] = result;

    	 if ( test_cond == 99 ) {
                // Monitor Read Delay8[ns]
                // Monitor Read Delay10[ns]
        stack10 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
        stack11 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
        stack12 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
        stack0=SetValue(IN_BDATA00,param,0);
        SetValue(IN_LDATA0,param,100*10000);
        // 100ms(10000*100ns=1ms)
	       for(i=0 ; i < 2 ;i++){
          switch(i){
            case 0:
              printf("MONITOR Read Delay8");
              SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY8);
              // AUTOTRIM_READDELAY8 = MONITOR_READDELAY8
		            break;
            case 1:
              printf("MONITOR Read Delay10");
              SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY10);
              // AUTOTRIM_READDELAY10 = MONITOR_READDELAY10
		            break;
          }

          for(module_loop=0; module_loop<=2; module_loop++){
            first_flag =1;
            pass_flag = err_flag = cnt =0;
            while(1){
              if(first_flag==1){
              // Initial mode
                first_flag = 0;
                repeat_mode =INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
                switch(i){
                  case 0:
                    selvol = trimvalue[T_READDELAY8];
		                  break;
                  case 1:
                    selvol = trimvalue[T_READDELAY10];
		                  break;
		              }
	             }else{
                repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
              }

              CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorcycle_at"),"MONITOR_READDELAY(DELAYTEST)",&selvol);
              delay_result[module_loop] = (GetValue(OUT_JUDGE1,param)>>8)&(1<<module_loop);
              result = ((0x00FF&GetValue(OUT_JUDGE1,param))<<8)|((0xFF00&GetValue(OUT_JUDGE1,param))>>8);
              switch(i){
                case 0:
                  result = result & 0x170;
		                break;
                case 1:
                  result = result & 0x180;
		                break;
		            }
             	if( GetValue(OUT_JUDGE1,param) == 0x00 ){
                pass_flag = 1;
                if(err_flag!=1){
                  printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol+1);
                  selvol=selvol+1;
              	   judge = 1;
	               }
              }else if(result == 0x170 || result == 0x180 ){
                if( delay_result[module_loop]==0x00 ){
                  pass_flag = 1;
                  if(err_flag!=1){
                    printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol+1);
                    selvol=selvol+1;
                  	 judge = 1;
		                }
                }else if( delay_result[module_loop]==(1<<module_loop)) {
                  err_flag = 1;
                  if(pass_flag!=1){
                    printf(" Monitor PASS timing => Result %dns -> %dns", selvol, selvol-1);
                    selvol=selvol-1;
                    judge =  0;
		                }
                }
              }else{
                printf(" Monitor Error");
                selvol = 0;
                pass_flag=1;
                err_flag==1;
              }

              if( (pass_flag==1)&&(err_flag==1) ) break;
              cnt++;
              if( selvol<21 || cnt>270 ){
                selvol=0;
                printf( " Monitor Error !!" );
                break;
              }
            }
            if(i==0) vol[T_READDELAY8_FLP0+module_loop]  = selvol - judge;
            else    vol[T_READDELAY10_FLP0+module_loop] = selvol - judge;
            DeviceLevelsPowerDown();
            // edOFFR}h
          }
	       }
        SetValue(IN_BDATA00,param,stack0);
        SetValue(IN_TESTSIZE0,param,stack10);
        SetValue(IN_TESTSIZE1,param,stack11);
        SetValue(IN_TESTSIZE2,param,stack12);
	     }
     break;

        // ************************************************************************
        // AUTO TRIMING Add 20180320
        // ************************************************************************

    case 203:
	  Trim_VddVddh(1,param);
      break;
    case 204:
	  Trim_VddVddh(2,param);
      break;



    case 205:
      printf("AutoTrim Vref10:%06d[mV]", trimvalue[V_VREF10]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VREF10);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VREF10",&trimvalue[V_VREF10]);
      break;

    case 207:
      printf("AutoTrim Vpp Erase:%06d[mV]", trimvalue[V_VPP_E]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_E);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_E",&trimvalue[V_VPP_E]);
      break;

    case 208:
      printf("AutoTrim Vpp Write:%06d[mV]", trimvalue[V_VPP_P]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_P);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_W",&trimvalue[V_VPP_P]);
      break;

    case 209:
      printf("AutoTrim Vpp Pre-Write:%06d[mV]", trimvalue[V_VPP_PW]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_PW);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_PW",&trimvalue[V_VPP_PW]);
      break;

    case 221:
      printf("AutoTrim Vhh Erase:%06d[mV]", trimvalue[V_VHH_E]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_E);
      SetValue(IN_BDATA00,param,0);
      SetValue(IN_LDATA0,param,40);
      SetValue(IN_LDATA1,param,29);
      SetValue(IN_LDATA2,param,31);
      SetValue(IN_LDATA3,param,30);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("triming_Vhh"),"AUTOTRIM_VHH_E",&trimvalue[V_VHH_E]);
      break;

    case 222:
      printf("AutoTrim Vhh Write:%06d[mV]", trimvalue[V_VHH_P]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
	    SetValue(IN_BDATA00,param,1);
      SetValue(IN_LDATA0,param,40);
      SetValue(IN_LDATA1,param,29);
      SetValue(IN_LDATA2,param,31);
      SetValue(IN_LDATA3,param,30);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("triming_Vhh"),"AUTOTRIM_VHH_W",&trimvalue[V_VHH_P]);
      break;

    case 230:
      printf("AutoTrim Vrsg:%06d[mV]", trimvalue[V_VRSG]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VRSG);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VRSG",&trimvalue[V_VRSG]);
      break;

    case 231:
      printf("AutoTrim V33r:%06d[mV]", trimvalue[V_V33R]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_V33R);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_V33R",&trimvalue[V_V33R]);
      break;

    case 232:
      printf("AutoTrim Vwi:%06d[mV]", trimvalue[V_VWI]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VWI);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("triming_Vwi"),"AUTOTRIM_VWI",&trimvalue[V_VWI]);
      break;

        //case 233:                         // Vdemg̓g~OȂ(eXgt@[Ή)
        //  printf("AutoTrim Vdemg:%06d[mV]", trimvalue[V_VDEMG]);
        //  SetValue(IN_TESTSEL,param,AUTOTRIM_VDEMG);
        //  CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VDEMG",&trimvalue[V_VDEMG]);
        //  break;

        //case 235:
        //  printf("AutoTrim Vnoemi:%06d[mV]", trimvalue[V_VNOEMI]);
        //  SetValue(IN_TESTSEL,param,AUTOTRIM_VNOEMI);
        //  CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VNOEMI",&trimvalue[V_VNOEMI]);
        //  break;

    case 236:
      printf("AutoTrim Vnoemi(PE)", trimvalue[V_VNOEMI_PE]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VNOEMI_PE);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VNOEMI_PE",&trimvalue[V_VNOEMI_PE]);
      break;

        //case 237:                         // Vnoemi(Read)̓g~OȂ(eXgt@[Ή)
        //  printf("AutoTrim Vnoemi(Read)", trimvalue[V_VNOEMI_R]);
        //  SetValue(IN_TESTSEL,param,AUTOTRIM_VNOEMI_R);
        //  CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VNOEMI_READ",&trimvalue[V_VNOEMI_R]);
        //  break;

    case 240:
      printf("AutoTrim inmoni(I01U) notemp:%06d[nA]", trimvalue[I_IMONI_I01U_NOTEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_I01U_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_NOTEMP",&trimvalue[I_IMONI_I01U_NOTEMP]);
            //CpuModeMonitor(VSSMON_20K_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_NOTEMP",&trimvalue[I_IMONI_I01U_NOTEMP]);
      SetValue(IN_BDATA15,param,0);
      break;

    case 241:
      printf("AutoTrim inmoni(Flash) notemp:%06d[nA]", trimvalue[I_IMONI_FLASH_NOTEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);
            //CpuModeMonitor(VSSMON_20K_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);
      SetValue(IN_BDATA15,param,0);
      break;

    case 242:
      printf("AutoTrim inmoni(I01U) temp:%06d[nA]", trimvalue[I_IMONI_I01U_TEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_I01U_TEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_TEMP",&trimvalue[I_IMONI_I01U_TEMP]);
            //CpuModeMonitor(VSSMON_20K_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_TEMP",&trimvalue[I_IMONI_I01U_TEMP]);
      SetValue(IN_BDATA15,param,0);
      break;

    case 243:
      printf("AutoTrim inmoni(Flash) temp:%06d[nA]", trimvalue[I_IMONI_FLASH_TEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_TEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_TEMP",&trimvalue[I_IMONI_FLASH_TEMP]);
            //CpuModeMonitor(VSSMON_20K_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_TEMP",&trimvalue[I_IMONI_FLASH_TEMP]);
      SetValue(IN_BDATA15,param,0);
      break;

    case 250:
      printf("AutoTrim OSC_32MHz:%d[us]", trimvalue[T_OSC32]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_OSC32);
      SetValue(IN_LDATA0,param,268*10);
      // Target=270us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_OSC32");
      result = 0xFFFF-GetValue(OUT_LDATA0,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA0,param), result );
      printf( " HOSC = %d.%03d[MHz]", result*4/trimvalue[T_OSC32], (result*4%trimvalue[T_OSC32])*1000/trimvalue[T_OSC32] );
      printf( " ETLR10 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR10,param), GetValue(OUT_ETLR10,param) );
      SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param));
      break;

    case 251:
      printf("AutoTrim OSC_1MHz:%d[us]", trimvalue[T_OSC1]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_OSC1);
      SetValue(IN_LDATA0,param,98*10);
      // Target=100us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_OSC1");
      result = 0xFFFF-GetValue(OUT_LDATA1,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA1,param), result );
      printf( " LOSC = %d.%03d[MHz]", result/trimvalue[T_OSC1], (result%trimvalue[T_OSC1])*1000/trimvalue[T_OSC1] );
      printf( " ETLR11 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR11,param), GetValue(OUT_ETLR11,param) );
      SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
      break;

    case 260:
    case 261:
	     stack7 = stack8 =stack9 = 0;
      stack10 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      stack11 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      stack12 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      SetValue(IN_LDATA0,param,100*10000);
      // 100ms(10000*100ns=1ms)
      first_flag =1;
      if(test_cond==260){
        printf("AutoTrim Read Delay8:%d[ns]", trimvalue[T_READDELAY8]);
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY8);
        i = 0;
      }else{
        printf("AutoTrim Read Delay10:%d[ns]", trimvalue[T_READDELAY10]);
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY10);
        i = 1;
      }
      printf("Which Reference Signal? (0:DELAYTEST / 1:VSSMON) -> ");
      DigitInput(&tempo);
      stack0=SetValue(IN_BDATA00,param,tempo);
      for(cnt=0; cnt<16*(i+1); cnt++){
        if(first_flag==1){
        // Initial mode
          first_flag = 0;
          repeat_mode = INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
	       }else{
          repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
        }
        if     ( (test_cond==260)&&(tempo==1) ) CpuModeMonitor_Repeat(repeat_mode,VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY8(VSSMON)",&trimvalue[T_READDELAY8]);
        else if( (test_cond==260)&&(tempo==0) ) CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY8(DELAYTEST)",&trimvalue[T_READDELAY8]);
        else if( (test_cond==261)&&(tempo==1) ) CpuModeMonitor_Repeat(repeat_mode,VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY10(VSSMON)",&trimvalue[T_READDELAY10]);
        else                                    CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY10(DELAYTEST)",&trimvalue[T_READDELAY10]);
     
      	 delay_result[0] = (GetValue(OUT_JUDGE1,param)>>8) & 0x01;
      	 delay_result[1] = (GetValue(OUT_JUDGE1,param)>>8) & 0x02;
      	 delay_result[2] = (GetValue(OUT_JUDGE1,param)>>8) & 0x04;

      	 if(stack7 == 0){
      	   if(delay_result[0] == 0x00){
            printf(" FLP0 Delay Trimming OK !!");
      	     stack7 = 1;
		          if(test_cond==260)  etlr26_bk1 = (GetValue(OUT_ETLR26,param) & 0x0F);
            else if(test_cond==261)  SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param));
      	   }
      	 }
      	 if(stack8 == 0){
      	   if(delay_result[1] == 0x00){
            printf(" FLI1 Delay Trimming OK !!");
      	 	   stack8 = 1;
		          if(test_cond==260)  etlr26_bk2 = (GetValue(OUT_ETLR26,param)&0xF0);
            else if(test_cond==261)  SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
      	   }
      	 }
      	 if(stack9 == 0){
      	   if(delay_result[2] == 0x00){
            printf(" FLI2 Delay Trimming OK !!");
      	     stack9 = 1;
		          if(test_cond==260)  SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param));
            else if(test_cond==261)  SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param));
      	   }
      	 }
      	
        if(test_cond==260){
          printf(" ETLR26 = 0x%02X", GetValue(OUT_ETLR26,param));
          printf(" ETLR27 = 0x%02X", GetValue(OUT_ETLR27,param));
        }else{
          printf(" ETLR28 = 0x%02X", GetValue(OUT_ETLR28,param));
          printf(" ETLR29 = 0x%02X", GetValue(OUT_ETLR29,param));
          printf(" ETLR30 = 0x%02X", GetValue(OUT_ETLR30,param));
        }

        result = 0;
        result = stack7 + stack8 + stack9;
        if(result == 3){
          printf(" Delay Trimming OK !!");
          if(test_cond==260){
          	 SetValue(IN_ETLR26,param,(etlr26_bk1|etlr26_bk2));
            printf(" ETLR26 = 0x%02X", GetValue(IN_ETLR26,param));
            printf(" ETLR27 = 0x%02X", GetValue(IN_ETLR27,param));
          }else{
            printf(" ETLR28 = 0x%02X", GetValue(IN_ETLR28,param));
            printf(" ETLR29 = 0x%02X", GetValue(IN_ETLR29,param));
            printf(" ETLR30 = 0x%02X", GetValue(IN_ETLR30,param));
          }
          break;
        }

        err_flag = ((0x0F00&GetValue(OUT_JUDGE1,param))>>8);
        printf(" Status Register(ETCR16[4:2]) = 0x%X", err_flag);
        if(cnt==(16*(i+1)-1)) printf( " Register Max => Trimming Error !!" );
      }
      DeviceLevelsPowerDown();
      // edOFFR}h
      SetValue(IN_BDATA00,param,stack0);
      SetValue(IN_TESTSIZE0,param,stack10);
      SetValue(IN_TESTSIZE1,param,stack11);
      SetValue(IN_TESTSIZE2,param,stack12);
      break;

    case 270:
      printf("AutoTrim Vpp Tf:%d[us]", trimvalue[V_VPP_TF]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_TF);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_VPP_TF");
      result = GetValue(OUT_JUDGE1,param);
      if( result==PASS ){
        printf(" ETLR15 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR15,param), GetValue(OUT_ETLR15,param));
        SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param));
        printf( " Vpp Tf Trimming OK !!" );
      }
      break;

        // ************************************************************************
        // Auto Triming ALL
        // ************************************************************************
    case 300:
    // Auto Trim ALL ͌ʂAutoTrimmFAǉ\
      printf("AutoTrim ALL");
	  
	  Trim_VddVddh(1,param);
	  // Vdd Trim
	  Trim_VddVddh(2,param);
	  // Vddh Trim
	  DeviceSpecificPowerUp();

            // Auto Trim imoni(I01U) notemp
            //printf("AutoTrim inmoni(I01U) notemp:%06d[nA]", trimvalue[I_IMONI_I01U_NOTEMP]);
            //SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_I01U_NOTEMP);
            //SetValue(IN_BDATA15,param,UA25);
            //CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_NOTEMP",&trimvalue[I_IMONI_I01U_NOTEMP]);
            //SetValue(IN_BDATA15,param,0);

            // Auto Trim imoni(I01U) temp
            //printf("AutoTrim inmoni(I01U) temp:%06d[nA]", trimvalue[I_IMONI_I01U_TEMP]);
            //SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_I01U_TEMP);
            //SetValue(IN_BDATA15,param,UA25);
            //CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_I01U_TEMP",&trimvalue[I_IMONI_I01U_TEMP]);
            //SetValue(IN_BDATA15,param,0);

            // Auto Trim imoni(Flash) temp
      printf("AutoTrim inmoni(Flash) temp:%06d[nA]", trimvalue[I_IMONI_FLASH_TEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_TEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_TEMP",&trimvalue[I_IMONI_FLASH_TEMP]);
      SetValue(IN_BDATA15,param,0);

            // Auto Trim imoni(Flash) notemp
      printf("AutoTrim inmoni(Flash) notemp:%06d[nA]", trimvalue[I_IMONI_FLASH_NOTEMP]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_IMONI_FLASH_NOTEMP);
      SetValue(IN_BDATA15,param,UA25);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcurrent_at"),"AUTOTRIM_IMONI_FLASH_NOTEMP",&trimvalue[I_IMONI_FLASH_NOTEMP]);
      SetValue(IN_BDATA15,param,0);

            // Auto Trim Vref10
      printf("AutoTrim Vref10:%06d[mV]", trimvalue[V_VREF10]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VREF10);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VREF10",&trimvalue[V_VREF10]);

            // Auto Trim V33r
      printf("AutoTrim V33r:%06d[mV]", trimvalue[V_V33R]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_V33R);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_V33R",&trimvalue[V_V33R]);
            // ܂ŎdlŃg~O̎wL

            // Auto Trim Vpp Erase
      printf("AutoTrim Vpp Erase:%06d[mV]", trimvalue[V_VPP_E]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_E);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_E",&trimvalue[V_VPP_E]);

            // Auto Trim Vpp Write
      printf("AutoTrim Vpp Write:%06d[mV]", trimvalue[V_VPP_P]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_P);
      CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_W",&trimvalue[V_VPP_P]);

            // Auto Trim Vpp PreWrite
            //printf("AutoTrim Vpp Pre-Write:%06d[mV]", trimvalue[V_VPP_PW]);
            //SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_PW);
            //CpuModeMonitor(VSSMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VPP_PW",&trimvalue[V_VPP_PW]);

            // Auto Trim Vhh Erase
      printf("AutoTrim Vhh Erase:%06d[mV]", trimvalue[V_VHH_E]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_E);
      SetValue(IN_LDATA0,param,30*10000);
      // 30ms(10000*100ns=1ms)
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_Vhh_at"),"AUTOTRIM_VHH_E",&trimvalue[V_VHH_E]);

            // Auto Trim Vhh Write
      printf("AutoTrim Vhh Write:%06d[mV]", trimvalue[V_VHH_P]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
      SetValue(IN_LDATA0,param,30*10000);
      // 30ms(10000*100ns=1ms)
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_Vhh_at"),"AUTOTRIM_VHH_W",&trimvalue[V_VHH_P]);

            // Auto Trim Vrsg
      printf("AutoTrim Vrsg:%06d[mV]", trimvalue[V_VRSG]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VRSG);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VRSG",&trimvalue[V_VRSG]);

            // Auto Trim Vwi
      printf("AutoTrim Vwi:%06d[mV]", trimvalue[V_VWI]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VWI);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VWI",&trimvalue[V_VWI]);

            // Auto Trim Vnoemi(PE)
      printf("AutoTrim Vnoemi(PE)", trimvalue[V_VNOEMI_PE]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VNOEMI_PE);
      CpuModeMonitor(VCCMON_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_at"),"AUTOTRIM_VNOEMI_PE",&trimvalue[V_VNOEMI_PE]);

            // Auto Trim OSC32MHz
      printf("AutoTrim OSC_32MHz:%d[cyc]", trimvalue[T_OSC32]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_OSC32);
      SetValue(IN_LDATA0,param,268*10);
      // Target=270us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_OSC32");
      result = 0xFFFF-GetValue(OUT_LDATA0,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA0,param), result );
      printf( " HOSC = %d.%03d[MHz]", result*4/trimvalue[T_OSC32], (result*4%trimvalue[T_OSC32])*1000/trimvalue[T_OSC32] );
      printf( " ETLR10 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR10,param), GetValue(OUT_ETLR10,param) );
      SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param));

            // Auto Trim OSC1MHz
      printf("AutoTrim OSC_1MHz:%d[us]", trimvalue[T_OSC1]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_OSC1);
      SetValue(IN_LDATA0,param,98*10);
      // Taeget=100us(10*100ns=1us)
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_OSC1");
      result = 0xFFFF-GetValue(OUT_LDATA1,param);
      printf( " OCC(ETCR15&14) = 0x%04X ->> %d[cyc]", GetValue(OUT_LDATA1,param), result );
      printf( " LOSC = %d.%03d[MHz]", result/trimvalue[T_OSC1], (result%trimvalue[T_OSC1])*1000/trimvalue[T_OSC1] );
      printf( " ETLR11 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR11,param), GetValue(OUT_ETLR11,param) );
      SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));

            // Auto Trim Read Delay 8[ns]
            // Auto Trim Read Delay 10[ns]
	     stack7 = stack8 =stack9 = 0;
      stack10 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
      stack11 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
      stack12 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
      SetValue(IN_LDATA0,param,100*10000);
      // 100ms(10000*100ns=1ms)
      first_flag =1;
      if(test_cond==260){
        printf("AutoTrim Read Delay8:%d[ns]", trimvalue[T_READDELAY8]);
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY8);
        i = 0;
      }else{
        printf("AutoTrim Read Delay10:%d[ns]", trimvalue[T_READDELAY10]);
        SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY10);
        i = 1;
      }

	     for(i=0;i<2;i++){
	       stack7 = stack8 =stack9 = 0;
        stack10 = SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
        stack11 = SetValue(IN_TESTSIZE1,param,FLI1_SIZE);
        stack12 = SetValue(IN_TESTSIZE2,param,FLI2_SIZE);
        SetValue(IN_LDATA0,param,100*10000);
        // 100ms(10000*100ns=1ms)
        first_flag =1;
        if(i==0){
           printf("AutoTrim Read Delay8:%d[ns]", trimvalue[T_READDELAY8]);
           SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY8);        i = 0;
        }else{
          printf("AutoTrim Read Delay10:%d[ns]", trimvalue[T_READDELAY10]);
          SetValue(IN_TESTSEL,param,AUTOTRIM_READDELAY10);
        }

        stack0=SetValue(IN_BDATA00,param,0);
        for(cnt=0; cnt<32; cnt++){
          if(first_flag==1){
          // Initial mode
            first_flag = 0;
            repeat_mode = INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
	         }else{
            repeat_mode = OUTIF_READ | RAMBOOT_CHANGE;
          }
          if(i==0 ) CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY8(DELAYTEST)",&trimvalue[T_READDELAY8]);
          else      CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingcycle_at"),"MONITOR_READDELAY10(DELAYTEST)",&trimvalue[T_READDELAY10]);
      
       	  delay_result[0] = (GetValue(OUT_JUDGE1,param)>>8) & 0x01;
       	  delay_result[1] = (GetValue(OUT_JUDGE1,param)>>8) & 0x02;
       	  delay_result[2] = (GetValue(OUT_JUDGE1,param)>>8) & 0x04;

       	  if(stack7 == 0){
       	    if(delay_result[0] == 0x00){
              printf(" FLP0 Delay Trimming OK !!");
       	      stack7 = 1;
		            if(i==0)   etlr26_bk1 = (GetValue(OUT_ETLR26,param) & 0x0F);
              else if(i==1)  SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param));
       	    }
       	  }
       	  if(stack8 == 0){
       	    if(delay_result[1] == 0x00){
              printf(" FLI1 Delay Trimming OK !!");
       	 	    stack8 = 1;
		            if(i==0)   etlr26_bk2 = (GetValue(OUT_ETLR26,param) & 0x0F);
              else if(i==1)  SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
       	    }
       	  }
       	  if(stack9 == 0){
       	    if(delay_result[2] == 0x00){
              printf(" FLI2 Delay Trimming OK !!");
       	      stack9 = 1;
		            if(i==0)  SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param));
              else if(i==1)  SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param));
       	    }
       	  }
       	
          if(i==0){
            printf(" ETLR26 = 0x%02X", GetValue(OUT_ETLR26,param));
            printf(" ETLR27 = 0x%02X", GetValue(OUT_ETLR27,param));
          }else{
            printf(" ETLR28 = 0x%02X", GetValue(OUT_ETLR28,param));
            printf(" ETLR29 = 0x%02X", GetValue(OUT_ETLR29,param));
            printf(" ETLR30 = 0x%02X", GetValue(OUT_ETLR30,param));
          }

          result = 0;
          result = stack7 + stack8 + stack9;
          if(result == 3){
            printf(" Delay Trimming OK !!");
            if(i==260){
            	 SetValue(IN_ETLR26,param,(etlr26_bk1|etlr26_bk2));
              printf(" ETLR26 = 0x%02X", GetValue(IN_ETLR26,param));
              printf(" ETLR27 = 0x%02X", GetValue(IN_ETLR27,param));
            }else{
              printf(" ETLR28 = 0x%02X", GetValue(IN_ETLR28,param));
              printf(" ETLR29 = 0x%02X", GetValue(IN_ETLR29,param));
              printf(" ETLR30 = 0x%02X", GetValue(IN_ETLR30,param));
            }
            break;
          }
          err_flag = ((0x0F00&GetValue(OUT_JUDGE1,param))>>8);
          printf(" Status Register(ETCR16[4:2]) = 0x%X", err_flag);
          if(cnt==32) printf( " Register Max => Trimming Error !!" );
      }
      DeviceLevelsPowerDown();
      // edOFFR}h
      SetValue(IN_BDATA00,param,stack0);
      SetValue(IN_TESTSIZE0,param,stack10);
      SetValue(IN_TESTSIZE1,param,stack11);
      SetValue(IN_TESTSIZE2,param,stack12);
	     }
    	
      DeviceSpecificPowerUp();
      // DelayTrimPinݒύXɔĐݒ肪Kv(ʏ̓j[CvbgɎ{)
            // Auto Trim Vpp tf
      printf("AutoTrim Vpp Tf:%d[us]", trimvalue[V_VPP_TF]);
      SetValue(IN_TESTSEL,param,AUTOTRIM_VPP_TF);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("trimingcycle_at"),"AUTOTRIM_VPP_TF");
      result = GetValue(OUT_JUDGE1,param);
      if( result==PASS ){
        printf(" ETLR15 = 0x%02X ->> 0x%02X", GetValue(IN_ETLR15,param), GetValue(OUT_ETLR15,param));
        SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param));
        printf( " Vpp Tf Trimming OK !!" );
      }

      SetValue(IN_TESTSEL,param,0);
      break;

        // ************************************************************************
        // [Set Target Value] Menu
        // ************************************************************************
    case 301:
      printf("[Set Target Value] Menu");
      do{
        printf("+----+-------------------+----------+");
        // Display Menu
        printf("+----+-------------------+----------+");
        printf("| (2)|Bgr08              |[%6d]mV|",trimvalue[V_BGR08]);
        printf("| (3)|Vdd                |[%6d]mV|",trimvalue[V_VDD]);
        printf("| (4)|Vddh               |[%6d]mV|",trimvalue[V_VDDH]);
        printf("| (5)|Vref10             |[%6d]mV|",trimvalue[V_VREF10]);
        printf("| (7)|Vpp_erase          |[%6d]mV|",trimvalue[V_VPP_E]);
        printf("| (8)|Vpp_write          |[%6d]mV|",trimvalue[V_VPP_P]);
        printf("| (9)|Vpp_prewrite       |[%6d]mV|",trimvalue[V_VPP_PW]);
        printf("|(21)|Vhh_erase          |[%6d]mV|",trimvalue[V_VHH_E]);
        printf("|(22)|Vhh_write          |[%6d]mV|",trimvalue[V_VHH_P]);
        printf("|(30)|Vrsg               |[%6d]mV|",trimvalue[V_VRSG]);
        printf("|(31)|V33r               |[%6d]mV|",trimvalue[V_V33R]);
        printf("|(32)|Vwi                |[%6d]mV|",trimvalue[V_VWI]);
                //printf("|(33)|Vdemg              |[%6d]mV|",trimvalue[V_VDEMG]);
                //printf("|(35)|Vnoemi             |[%6d]mV|",trimvalue[V_VNOEMI]); //del 20180329 
                //printf("|(37)|Vnoemi(Read)       |[%6d]mV|",trimvalue[V_VNOEMI_R]);
        printf("|(40)|imoni(I01U)  notemp|[%6d]nA|",trimvalue[I_IMONI_I01U_NOTEMP]);
        printf("|(41)|imoni(Flash) notemp|[%6d]nA|",trimvalue[I_IMONI_FLASH_NOTEMP]);
        printf("|(42)|imoni(I01U)  temp  |[%6d]nA|",trimvalue[I_IMONI_I01U_TEMP]);
        printf("|(43)|imoni(Flash) temp  |[%6d]nA|",trimvalue[I_IMONI_FLASH_TEMP]);
                //printf("|(50)|OSC32MHz           |[%6d]us|",trimvalue[T_OSC32]);
                //printf("|(51)|OSC1MHz            |[%6d]us|",trimvalue[T_OSC1]);
        printf("|(60)|Read Delay8        |[%6d]ns|",trimvalue[T_READDELAY8]);
        printf("|(61)|Read Delay10       |[%6d]ns|",trimvalue[T_READDELAY10]);
                //printf("|(70)|Vpp Tf             |[%6d]us|",trimvalue[V_VPP_TF]);
        printf("+----+-------------------+----------+");
        printf("|Return(e)                          |");
        printf("+-----------------------------------+");
        printf("Select No ->");
        DigitInput(&tempo);
        if( 2<=tempo && tempo<=70 ){
          switch(tempo){
            case 2:  select_target=V_BGR08;              break;
            case 3:  select_target=V_VDD;                break;
            case 4:  select_target=V_VDDH;               break;
            case 5:  select_target=V_VREF10;             break;
            case 7:  select_target=V_VPP_E;              break;
            case 8:  select_target=V_VPP_P;              break;
            case 9:  select_target=V_VPP_PW;             break;
            case 21: select_target=V_VHH_E;              break;
            case 22: select_target=V_VHH_P;              break;
            case 30: select_target=V_VRSG;               break;
            case 31: select_target=V_V33R;               break;
            case 32: select_target=V_VWI;                break;
                        //case 33: select_target=V_VDEMG;              break;
                        //case 35: select_target=V_VNOEMI;             break; //del 20180329
            case 36: select_target= V_VNOEMI_PE;         break;
                        //case 37: select_target= V_VNOEMI_R;          break;
            case 40: select_target=I_IMONI_I01U_NOTEMP;  break;
            case 41: select_target=I_IMONI_FLASH_NOTEMP; break;
            case 42: select_target=I_IMONI_I01U_TEMP;    break;
            case 43: select_target=I_IMONI_FLASH_TEMP;   break;
                        //case 50: select_target=T_OSC32;              break;
                        //case 51: select_target=T_OSC1;               break;
            case 60: select_target=T_READDELAY8;         break;
            case 61: select_target=T_READDELAY10;        break;
                        //case 70: select_target=V_VPP_TF;             break;
            default: printf("!!!!!!!!!!! select err !!!!!!!!!!!!!"); break;
          }
          printf("Set No.%d Value (ex.1900)->", tempo);
          DigitInput(&tempo);
          trimvalue[select_target] = tempo;
        }
      }while(tempo!=EXIT);
      // end do
      break;

    case 800:
    		stack0 = SetValue(IN_BDATA15,param,UA250);
      stack1 = SetValue(IN_BDATA00,param,0);
      stack2 = GetValue(IN_ETCR03,param);
      while(1){
    	   printf("+---+---------------------+");
    	   printf("| No|Item                 |");
    	   printf("+---+---------------------+");
    	   printf("|  7|vpp (Erase)          |");
    	   printf("|  8|vpp (Write)          |");
    	   printf("|  9|vpp (Pre-Write)      |");
    	   printf("| 21|Vhh (Erase)          |");
    	   printf("| 22|Vhh (Write)          |");
    	   printf("| 30|VRSG                 |");
    	   printf("| 31|V33r                 |");
    	   printf("| 32|Vwi                  |");
    	   printf("| 36|Vnoemi(PE)           |");
        printf("|121|Vhh_E Leak           |");
    	   printf("|122|Vhh_W Leak           |");
    	   printf("+---+---------------------+");
    	   printf("|199|Tap Dependency ALL   +");
    	   printf("|200|Tap Dependency       +");
    	   printf("+---+---------------------+");
    	   printf("|IN_ETCR03  = 0x%2X (Tap) |",GetValue(IN_ETCR03,param));
    	   printf("|IN_BDATA00 = 0x%2X (Tap) |",GetValue(IN_BDATA00,param));
    	   printf("+---+---------------------+");
    	   printf("|e  |Exit                 +");
    	   printf("+---+---------------------+");
    	   printf("Input Item No ->");
        DigitInput(&tempo0);
		      if(tempo0 == EXIT){
          break;
          }else if(tempo0 < 150){
          printf("Target Voltage ->");
          DigitInput(&selvol);
        }
    	   switch(tempo0){
          case 7:
            SetValue(IN_TESTSEL,param,MONITOR_VPP_E);
            result = CpuModeMonitor(VSSMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VPP_E",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 8:
            SetValue(IN_TESTSEL,param,MONITOR_VPP_P);
  	         SetValue(IN_BDATA15,param,UA2500);
            result = CpuModeMonitor(VSSMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VPP_P",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 9:
            SetValue(IN_TESTSEL,param,MONITOR_VPP_PW);
  	         SetValue(IN_BDATA15,param,UA2500);
            result = CpuModeMonitor(VSSMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VPP_PW",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 21:
            SetValue(IN_TESTSEL,param,MONITOR_VHH_E);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VHH_E",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 22:
            SetValue(IN_TESTSEL,param,MONITOR_VHH_P);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VHH_P",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 30:
            SetValue(IN_TESTSEL,param,MONITOR_VRSG);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VRSG",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 31:
            SetValue(IN_TESTSEL,param,MONITOR_V33R);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_V33R",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 32:
            SetValue(IN_TESTSEL,param,MONITOR_VWI);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VWI",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 36:
            SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
            result = CpuModeMonitor(VCCMON_CPSUP,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_PE",&selvol);
            printf("CP current = %dnA",selvol);
    		      break;
          case 121:
            SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_E);
            stack4 = SetValue(IN_LDATA0,param,30*10000);
            // 30ms(10000*100ns=1ms)
            CpuModeMonitor(VCCMON_CPLK,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_Vhh_at"),"LEAK-MONITOR_VHH_E",&selvol);
            printf("  Leak monitor = %d[nA]",selvol);
    	   	   SetValue(IN_LDATA0,param,stack4);
    	   	   break;
          case 122:
            SetValue(IN_TESTSEL,param,AUTOTRIM_VHH_P);
            stack4 = SetValue(IN_LDATA0,param,30*10000);
            // 30ms(10000*100ns=1ms)
            CpuModeMonitor(VCCMON_CPLK,&vs[0],param,W1SEC*waittime,CreatePatFileName("trimingvoltage_Vhh_at"),"LEAK-MONITOR_VHH_P",&selvol);
            printf("  Leak monitor = %d[nA]",selvol);
    	   	   SetValue(IN_LDATA0,param,stack4);
    	   	   break;
          case 199:
    	       for(i =0; i<=9; i++){
  	 	         test_cond = testcond[i];
              CPvsVolall(test_cond,&vs[0],waittime,param);
    	         printf("+---+Use tst is CPvsVolall---+");
    	       }
 	       	  break;
    		    case 200:
    	       printf("+---+---------------------+");
    	       printf("+---+Use tst is CPvsVo----+");
    	       printf("| No|Item                 |");
    	       printf("+---+---------------------+");
    	       printf("|  7|vpp (Erase)          |");
    	       printf("|  8|vpp (Write)          |");
    	       printf("|  9|vpp (Pre-Write)      |");
    	       printf("| 21|Vhh (Erase)          |");
    	       printf("| 22|Vhh (Write)          |");
    	       printf("| 30|VRSG                 |");
    	       printf("| 31|V33r                 |");
    	       printf("| 32|Vwi                  |");
    	       printf("| 36|Vnoemi(PE)           |");
            printf("|121|Vhh_E Leak           |");
    	       printf("|122|Vhh_W Leak           |");
    	       printf("+---+---------------------+");
    	       printf("|IN_ETCR03  = 0x%2X       +",GetValue(IN_ETCR03,param));
    	       printf("|IN_BDATA00 = 0x%2X       +",GetValue(IN_BDATA00,param));
    	       printf("+---+---------------------+");
    	       printf("|999|Exit                 +");
    	       printf("+---+---------------------+");
            printf("Input Target Item No ->");
            DigitInput(&tempo0);
            printf(" Item No= %d",tempo0);
            CPvsVol(tempo0,&vs[0],waittime,param);
 	       	  break;
    	     case SET_TAP:
            printf("IN_ETCR03 = 0x%2x",GetValue(IN_ETCR03,param));
            printf("enter set Value(ex. 0x8f) -> ");
    		      DigitInput(&tempo0);
    		      SetValue(IN_ETCR03,param,tempo0);
            printf("IN_BDATA00 = 0x%2x",GetValue(IN_BDATA00,param));
            printf("enter set Value(ex. 0x8f) -> ");
    		      DigitInput(&tempo0);
    		      SetValue(IN_BDATA00,param,tempo0);
    		      break;
    	     default:  break;
        }
      }
      SetValue(IN_BDATA15,param,stack0);
      SetValue(IN_BDATA00,param,stack1);
	  SetValue(IN_ETCR03,param,stack2);
    break;
  case 801:
    printf("Input temprature -> ");DigitInput(&tempo0);
    CPcurrent_ALL(MONITOR_VPP_E,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VPP_P,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VPP_PW,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VHH_E,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VHH_P,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VRSG,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VWI,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_VNOEMI,&vs[0],waittime,param,tempo0);
    CPcurrent_ALL(MONITOR_V33R,&vs[0],waittime,param,tempo0);
  break;
  case 802:
    printf("Input temprature -> ");DigitInput(&tempo0);
    for(i=1;i<5;i++){
      switch(i){
        case 0: vs[0] = 1500;break;
        case 1: vs[0] = 1500;break;
        case 2: vs[0] = 2000;break;
        case 3: vs[0] = 3300;break;
        case 4: vs[0] = 3700;break;
      }
      CPcurrent_ALL(MONITOR_VPP_E, &vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VPP_P, &vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VPP_PW,&vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VHH_E, &vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VHH_P, &vs[0],waittime,param,tempo0);
            //CPcurrent_ALL(MONITOR_VRSG,&vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VWI,   &vs[0],waittime,param,tempo0);
      CPcurrent_ALL(MONITOR_VNOEMI,&vs[0],waittime,param,tempo0);
            //CPcurrent_ALL(MONITOR_V33R,&vs[0],waittime,param,tempo0);
    }
    vs[0] = 3300;
    MonitorALL_Tap(&vs[0],param,V_VREG20,tempo0);
    MonitorALL_Tap(&vs[0],param,V_VREF10,tempo0);
    SetValue(IN_LDATA4,param,tempo0);
        //MONIvsTap(T_READDELAY8_FLP0,&vs[0],waittime,param);
        //MONIvsTap(T_READDELAY10_FLP0,&vs[0],waittime,param);
  break;


  case 198:
    sprintf(file_name,"Monitor_ALL_DataOut_Vcc%d_Vddh%d_Vdd%d",vs[0],vs[1],vs[2]);
    sprintf( buff, "%s%s%s.csv", glob_cdp, glob_datalog, file_name );
    printf(" -- FileOut --");
    delay_timer(get_site_number()*1000*500);
    //DUT500ms Delayǉ(File̋h)
    if((Fptdata=fopen(buff,"rt")) == NULL) {
        //------ headder -----//
    if((Fptdata=fopen(buff,"at")) == NULL) {
      printf(" -- FAIL -- not open file:%s !",buff);
      return(FAIL);
    }
        //Initial Set
    fprintf(Fptdata,"Lot,W,S,X,Y,DUT,");
        //Monitor data Out
    fprintf(Fptdata,"V_VPP_E,");
    fprintf(Fptdata,"V_VPP_P,");
    fprintf(Fptdata,"V_VPP_PW,");
    fprintf(Fptdata,"V_VHH_E,");
    fprintf(Fptdata,"V_VHH_P,");
    fprintf(Fptdata,"V_VRSG,");
    fprintf(Fptdata,"V_V33R,");
    fprintf(Fptdata,"V_VWI,");
    fprintf(Fptdata,"V_VDEMG,");
    fprintf(Fptdata,"V_VREG20,");
    fprintf(Fptdata,"V_VNOEMI_PE,");
    fprintf(Fptdata,"I_IMONI_I01U_NOTEMP,");
    fprintf(Fptdata,"I_IMONI_FLASH_NOTEMP,");
    fprintf(Fptdata,"I_IMONI_I01U_TEMP,");
    fprintf(Fptdata,"I_IMONI_FLASH_TEMP,");
    fprintf(Fptdata,"T_OSC32,");
    fprintf(Fptdata,"T_OSC1,");
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"ETLR%02d,",(i - IN_ETLR00));
    }
    fprintf(Fptdata,"");
    
    if(fclose(Fptdata)) exit(-10);
  }else{
        //--------- File Close ---------//
    if(fclose(Fptdata)) exit(-10);
  }

    // File Open
  if( NULL==( Fptdata = fopen( buff, "at" ) ) ){ printf( "OpenDataOutFile Fptdata Open Err[%s]", buff ); exit( -99 ); } delay_timer(200);

        //Sample Info + Dut
    fprintf(Fptdata,"%s,W%d,%d,%d,%d,%d,",lotname,atoi(wafername),SampleNo,xy_location[0],xy_location[1],get_site_number()+1);
        //Monitor Data Out
    fprintf(Fptdata,"%d,",vol[V_VPP_E]);
    fprintf(Fptdata,"%d,",vol[V_VPP_P]);
    fprintf(Fptdata,"%d,",vol[V_VPP_PW]);
    fprintf(Fptdata,"%d,",vol[V_VHH_E]);
    fprintf(Fptdata,"%d,",vol[V_VHH_P]);
    fprintf(Fptdata,"%d,",vol[V_VRSG]);
    fprintf(Fptdata,"%d,",vol[V_V33R]);
    fprintf(Fptdata,"%d,",vol[V_VWI]);
    fprintf(Fptdata,"%d,",vol[V_VDEMG]);
    fprintf(Fptdata,"%d,",vol[V_VREG20]);
    fprintf(Fptdata,"%d,",vol[V_VNOEMI_PE]);
    fprintf(Fptdata,"%d,",vol[I_IMONI_I01U_NOTEMP]);
    fprintf(Fptdata,"%d,",vol[I_IMONI_FLASH_NOTEMP]);
    fprintf(Fptdata,"%d,",vol[I_IMONI_I01U_TEMP]);
    fprintf(Fptdata,"%d,",vol[I_IMONI_FLASH_TEMP]);
    fprintf(Fptdata,"%d.%03d,",vol[T_OSC32]*4/trimvalue[T_OSC32],(vol[T_OSC32]*4%trimvalue[T_OSC32])*1000/trimvalue[T_OSC32]);
    fprintf(Fptdata,"%d.%03d,",vol[T_OSC1]/trimvalue[T_OSC1],(vol[T_OSC1]%trimvalue[T_OSC1])*1000/trimvalue[T_OSC1]);
        // ETLR Data Out
    for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      fprintf(Fptdata,"%02X,",GetValue(i,param));
    }

    fprintf(Fptdata,"");
        //File Close
    delay_timer(200);
    if( fclose( Fptdata ) ){ printf( "CloseDataOutFile Fptdata Close Err" ); exit( -99 ); } delay_timer(200);

    printf(" -- FileOut Finish --");

  break;

  case 643:
    Iref_Trim_ALL(0,param);
  break;

  case 600:
    Iref_Temp_moni_ALL(0,param);
  break;
  case 602:
    Iref_Trim_forRext(0,param);
  break;
  case 603:
  vs[0]=3300;
  //Vcc=3.3V
  vs[2]=500;
  //Vdd=0.79V
  for(i=1;i<=3;i++){
    switch(i){
      case 1:
        vs[1]=580;
      break;
      case 2:
        vs[1]=500;
      break;
      case 3:
        vs[1]=452;
      break;
    }
    DeviceSpecificPowerUp();
    MONIvsTap(221,&vs[0],waittime,param);
    // VHH E Monitor
    DeviceSpecificPowerUp();
    MONIvsTap(222,&vs[0],waittime,param);
    // VHH W Monitor
    DeviceSpecificPowerUp();
    MONIvsTap(243,&vs[0],waittime,param);
    // Iref Monitor
  }
  
  break;
  case 604:
  vs[0]=3300;
  //Vcc=3.3V
  vs[2]=500;
  //Vdd=0.79V
    DeviceSpecificPowerUp();
    MONIvsTap(223,&vs[0],waittime,param);
    // VHH W Monitor
  
  break;
  case 601:
    printf("MONITOR Vnoemi(PE)");
    SetValue(IN_TESTSEL,param,MONITOR_VNOEMI_PE);
    SetValue(IN_BDATA00,param,0);
    SetValue(IN_BDATA16,param,12);
    SetValue(IN_BDATA15,param,UA2500);

    printf("Input Vrsg Level ->");DigitInput(&selvol);

    result = CpuModeMonitor(VRSGMON_MON,&vs[0],param,W1SEC*waittime,CreatePatFileName("monitorvoltage_at"),"MONITOR_VNOEMI_PE",&selvol);
  break;

    default:
      continue;
    }
    // end switch test_cond
  }
  // end while (test_cond != QUIT)&&(test_cond != EXIT)

  return(PASS);

}
