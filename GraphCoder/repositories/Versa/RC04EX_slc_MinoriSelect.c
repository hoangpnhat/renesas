/******************************************************************************/
/*!
  @file RC04EXslc_MonitorTrimingSelect.c
  @brief This is a 130nm(RC01SN) FLASH module user interface program for the 4000,5000 test system
  @author T.Morioka
  @date 2012.04.16 Rev.00
*/
/******************************************************************************/
int MinoriSelect(int vcc[NUM_POWERSUPPLY],int waittime,interface_t* param)
{
  int test_cond, result,result1,result2;
  int i,k,j, cnt;
  int tap,deb0,deb1;
  int select_target;
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7,tempo8,tempo9,tempo10,tempo11,tempo12,tempo13,tempo14,tempo15,tempo16,tempo17;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11,stack12;/*debug add*/
  int etlr25,etlr26,etlr27,etlr28,etlr29,etlr30,etlr32,etlr33,etlr34,cond2;//hashi
  int logparam[20];
  int  speed_bak, pllon_bak, pll_bak, pllon,vcc_bak[NUM_POWERSUPPLY],ffpt_bak;
  int speed,reg,freq,freqpara,freqinput;
  int loop,vddh,loop_num,loop_max,loop_min,loop_delta,loop_cyc,loop_cycmax,loop_cycmin,loop_nummin,loop_nummax,taploopnum,loop_deltac;
  int Voltage_Max,Voltage_Min,Voltage_Delta,Freq_num;
  int dataout[300][100];
  char module[20],clk_mode[20],lp_read_mode[20],read_bi[20];
  char wgl_name[256],name_parts0[20],name_parts1[20],name_parts2[20],name_parts3[20],name_parts4[20],tmpname[20];
  char mat_value[60];
  char headder[200],filename[200];
  char microcode[60],mesure_name[60];
  char test_name[128],vdd_name[128],vddh_name[128];
  int info[32];                    /* wmat chip information              */
  int *current;

  /****************************************************************************/
  /* INITIALIZE VALIABLE PARAMETOR                                            */
  /****************************************************************************/
  test_cond = INIT_COND;                /* INITIALIZE test_cond               */
  tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = tempo6 = tempo7 = tempo8 = 0;
  tempo9 = tempo10 = tempo11 = tempo12 = tempo13 = tempo14 = tempo15 = tempo16 = tempo17 = 0;

  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = stack7 = stack8 = stack9 = stack10 = stack11 = stack12 = 0;
  waittime = 16;
  SetValue(IN_BDATA03,param,1);//CLKQ Reverse
  SetValue(IN_BDATA08,param,0);

   cond2 = 0;

  /****************************************************************************/
  /* Ground Menu                                                              */
  /****************************************************************************/
  while( test_cond != QUIT){
  	//  speed change Freq   //
	freq = 1000/GetValue(IN_EXTAL1,param);
	freqpara = GetValue(IN_EXTAL1,param);
  
    if(GetValue(IN_LOWPOWER,param)==0){
      sprintf(lp_read_mode,"OFF");
    }else if(GetValue(IN_LOWPOWER,param)==1){
      sprintf(lp_read_mode," ON");
    }else{
      sprintf(lp_read_mode,"ERROR");
    }
    
    

  	 //  Module Select Monitor  //
     memset(mat_value, 0, sizeof(mat_value));
         if(strcmp(DispMatName(mat_value,param)," FLP0 & FLI1 & FLI2 " )==0){
       sprintf(name_parts0,"USER");
       sprintf(module," FLI3 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," FLI2 " )==0){
       sprintf(name_parts0,"USER");
       sprintf(module," FLI2 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," FLI1 " )==0){
       sprintf(name_parts0,"USER");
       sprintf(module," FLI1 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," FLP0 " )==0){
       sprintf(name_parts0,"USER");
       sprintf(module," FLP0 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA1 " )==0){
       sprintf(name_parts0,"EXT1");
       sprintf(module," EXTRA1 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA2 " )==0){
       sprintf(name_parts0,"EXT1");
       sprintf(module," EXTRA2 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA3 " )==0){
       sprintf(name_parts0,"EXT1");
       sprintf(module," EXTRA3 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA4 " )==0){
       sprintf(name_parts0,"EXT1");
       sprintf(module," EXTRA4 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA5 " )==0){
       sprintf(name_parts0,"EXT1");
       sprintf(module," EXTRA5 ");
    }
    else if(strcmp(DispMatName(mat_value,param)," EXTRA6 " )==0){
       sprintf(name_parts0,"EXT6");
       sprintf(module," EXTRA6 ");
    }
    SetValue(IN_BDATA01,param,GetValue(IN_LOWPOWER,param));
    SetValue(IN_BDATA02,param,GetValue(IN_LOWPOWER,param));
    
    
    if(GetValue(IN_BDATA03,param)==1) sprintf(clk_mode,"Reverse-Phase");
    else sprintf(clk_mode,"In-Phase");
    
    
    if(GetValue(IN_BDATA09,param)==1) sprintf(read_bi,"ON");
    else if(GetValue(IN_BDATA09,param)==0) sprintf(read_bi,"OFF");
	else sprintf(read_bi,"Other");
    
    printf("+=====================================================================================================+\n");
    printf("| MODE     | Minori Mode Menu\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Minori   | Read      : ALL 0/1(10) / CHK-A/B(15) / DIAG-XInc(20)\n");
    printf("|          | Shomm     : ALL 0/1(510) / CHK-A/B(515) / DIAG-XInc(520)\n");    
    printf("|          | Shommdelay: ALL 0/1(610) for DelayEval / CHK-A/B(615) for DelayEval\n");    
    printf("|          |           : DIAG-XInc(620) for DelayEval\n");    
    printf("|          | Shmoo*10  : Diag-10(700)\n");    
    printf("|          | Other     : ManualRead(99) / BI-ReadSet(987)\n");
    printf("| Minori   | Read      : CHKA(215) / CHKA-Debug(216)\n");
    printf("|(LowPower)| Shomm     : CHKA(815)\n");
    printf("|PULSE-GEN | Shomm     : CHKA(315)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Test    | Module   :%s(mdl)\n",module);
    printf("|          | RegSet   :%02X(reg)\n",GetValue(IN_REG,param) );
    printf("|          | WaitTime :%d *0x4000 cycle(wait)\n",waittime );
    printf("|          | Extal    :%d[ns] %d[MHz] (s)\n", GetValue(IN_EXTAL1,param),freq);
    printf("|          | [Other] CLK-Mode : %s (clk)\n",clk_mode);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Pattern | Read-BI    : %s(bi)\n",read_bi);
    printf("|  Option  | \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Power   | Vcc:%4dmV(vcc) Vddh:%4dmV(vddh) Vdd:%4dmV(vdd)\n", vcc[0],vcc[1],vcc[2]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  exit(e)\n");
    printf("+-----------------------------------------------------------------------------------------------------+\n");
    printf("Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();
    /**************************************************************************/
    /* Commans Analisys                                                       */
    /**************************************************************************/
    switch(test_cond){
    case EXIT:
      printf("\n--Minori Mode Exit--\n\n");
        DeviceSpecificPowerDown();
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0);
        SetValue(IN_BDATA01,param,0);
        SetValue(IN_BDATA02,param,0);
        SetValue(IN_BDATA03,param,0);
	SetValue(IN_BDATA07,param,0);
        SetValue(IN_BDATA09,param,0);
        return(PASS);
    case QUIT:
      DeviceSpecificPowerDown();
      if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
      PowerDown(1);
      break;
      
      break;
      
    /**************************************************************************/
    /* Test Condition                                                         */
    /**************************************************************************/
    case SET_CVCC:
      printf("enter vcc(mV) -> ");
      DigitInput(&tempo);
      vcc[0] = tempo;
      printf("\tset vcc %dmV\n",vcc[0]);
      break;
    case SET_CVDD:  //Add set vref 2018/0307
      printf("enter vdd(mV) -> ");
      DigitInput(&tempo);
      vcc[2] = tempo;
      printf("\tset VDD %dmV\n",vcc[2]);
      break;
    case SET_CVDDH: //Add set vrefh 2018/0307
      printf("enter vddh(mV) -> ");
      DigitInput(&tempo);
      vcc[1] = tempo;
      printf("\tset VDDH %dmV\n",vcc[1]);
      break;
    case SET_WAIT:                        /**** Set WaitTime ****/
      printf("enter WaitTime (sec) ->");DigitInput(&tempo);
	  waittime = tempo;
      printf("WaitTime %ds\n",waittime);
      break;
    case SET_SPEED:                       /**** Set SPEED ****/
      SpeedSelect(param,test_cond);
      break;
    case SET_REG:
      SetReg(param);
      break;
    case SET_MDL:                         /**** Set Mat ****/
      SelectAddress(param,test_cond);
      break;

    case SET_CLK:
      printf("enter CLK mode :In-Phase(0)/Reverse Phase(1) ");DigitInput(&tempo1);
      SetValue(IN_BDATA03,param,tempo1);
      break;
     
    case SET_BI:
      printf("Select Read-BI mode ON(1)/OFF(0) -> ");DigitInput(&tempo1);
      SetValue(IN_BDATA09,param,tempo1);
      break;
      
    /**************************************************************************/
    /* Main Read                                                              */
    /**************************************************************************/


      //----------- ALL pattern --------------//
    case 10:
      printf("Enter ALL0(0) / ALL1(1)");DigitInput(&tempo1);
      if(tempo1 == 0){
        sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS");
	
      }else if(tempo1 == 1){
        sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS");
      }else if(tempo1 == 2){
        sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS_PLL");SetValue(IN_BDATA15,param,1);
	
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
      MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
	  SetValue(IN_BDATA15,param,0);
      break;
    case 15:
      printf("Enter CHKA(0) / CHKB(1)");DigitInput(&tempo1);
      if(tempo1 == 0){
        sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS");
	
      }else if(tempo1 == 1){
        sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS");
	
      }else if(tempo1 == 2){
        sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS_PLL");SetValue(IN_BDATA15,param,1);
	
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
      MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
	  SetValue(IN_BDATA15,param,0);
      break;
    case 20:
      printf("Enter DIAG-XInc(0) / DIAG-XDec(1)");DigitInput(&tempo1);
      if(tempo1 == 0){
        sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	sprintf(name_parts3,"_MULTISCANI");
	sprintf(name_parts4,"_XS");
	
      }else{
        sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	sprintf(name_parts3,"_MULTISCAND");
	sprintf(name_parts4,"_XS");
	
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
      MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      break;

    case 99: //Manual Read
      

    sprintf(wgl_name,"%s",name_parts0);
    tempo1 = tempo2 = 99;      
    while(0!=tempo2){
      switch(tempo1){
        case 0:sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);tempo2=0;break;
        case 1:sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);tempo2=0;break;
        case 2:sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);tempo2=0;break;
        case 3:sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);tempo2=0;break;
        case 4:sprintf(name_parts1,"_DG");  SetValue(IN_TESTSEL,param,1);tempo2=0;break;
        case 5:sprintf(name_parts1,"_LD");  SetValue(IN_TESTSEL,param,2);tempo2=0;break;
        case 6:sprintf(name_parts1,"_LDS"); SetValue(IN_TESTSEL,param,3);tempo2=0;break;
        case 7:sprintf(name_parts1,"_DBUS");SetValue(IN_TESTSEL,param,6);tempo2=0;break;
        default:
          printf(" ================================ \n");
          printf(" PatternName  :%s\n",wgl_name);
          printf(" ========PatternSelect=========== \n");
          printf(" ALL0(0)      / ALL1(1)\n");
          printf(" CHKA(2)      / CHKB(3)\n");
          printf(" Diag(4)\n");
          printf(" LocalDiag(5) / LocalDiagShift(6) \n");
          printf(" DBUS(7)\n");
          printf(" ================================ \n");
          printf("Enter Pattern Select->");DigitInput(&tempo1);
		  break;
      }
    }

    sprintf(wgl_name,"%s%s",name_parts0,name_parts1);
    tempo1 = tempo2 = 99;      
    while(0!=tempo2){
      switch(tempo1){
        case 0:sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);tempo2=0;break;
        case 1:sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);tempo2=0;break;
        default:
          printf(" ================================ \n");
          printf(" PatternName  :%s\n",wgl_name);
          printf(" ========ScranbleSelect========== \n");
          printf(" M0(0)      / M1(1)\n");
          printf(" ================================ \n");
          printf("Enter Pattern Select->");DigitInput(&tempo1);
		  break;
      }
    }
    
    sprintf(wgl_name,"%s%s%s",name_parts0,name_parts1,name_parts2);
    tempo1 = tempo2 = 99;      
    while(0!=tempo2){
      switch(tempo1){
        case 0:sprintf(name_parts3,"_MULTISCANI");tempo2=0;break;
        case 1:sprintf(name_parts3,"_MULTISCAND");tempo2=0;break;
        case 2:sprintf(name_parts3,"_DSCAN1INCX");tempo2=0;break;
        case 3:sprintf(name_parts3,"_DSCAN1DECX");tempo2=0;break;
        case 4:sprintf(name_parts3,"_DSCAN2INCX");tempo2=0;break;
        case 5:sprintf(name_parts3,"_DSCAN2DECX");tempo2=0;break;
        case 6:sprintf(name_parts3,"_LOWFRQI1");  tempo2=0;break;
        case 7:sprintf(name_parts3,"_LOWFRQD1");  tempo2=0;break;
        case 8:sprintf(name_parts3,"_READPPXL");  tempo2=0;break;
        case 9:sprintf(name_parts3,"_READPPXU");  tempo2=0;break;
        case 10:sprintf(name_parts3,"_READPPYC"); tempo2=0;break;
        default:
          printf(" ================================ \n");
          printf(" PatternName  :%s\n",wgl_name);
          printf(" ==========ModeSelect=========== \n");
          printf(" MULTISCANI(0)\n");
          printf(" MULTISCAND(1)\n");
          printf(" DSCAN1INCX(2)\n");
          printf(" DSCAN1DECX(3)\n");
          printf(" DSCAN2INCX(4)\n");
          printf(" DSCAN2DECX(5)\n");
          printf(" LOWFRQI1 (6)\n");
          printf(" LOWFRQD1 (7)\n");
          printf(" READPPXL  (8)\n");
          printf(" READPPXU  (9)\n");
          printf(" READPPYC  (10)\n");
          printf(" ================================ \n");
          printf("Enter Pattern Select->");DigitInput(&tempo1);
		  break;
      }
    }


    sprintf(wgl_name,"%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3);
    tempo1 = tempo2 = 99;      
    while(0!=tempo2){
      switch(tempo1){
        case 0:sprintf(name_parts4,"_XS");tempo2=0;break;
        case 1:sprintf(name_parts4,"_YS");tempo2=0;break;
        case 2:sprintf(name_parts4,"_DS");tempo2=0;break;
        default:
          printf(" ================================ \n");
          printf(" PatternName  :%s\n",wgl_name);
          printf(" ===========ScanSelect=========== \n");
          printf(" X-Scan(0)  / Y-Scan(1)\n");
          printf(" Ping-Pong(2)\n");
          printf(" ================================ \n");
          printf("Enter Pattern Select->");DigitInput(&tempo1);
		  break;
      }
    }

    sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
    tempo1 = 99;      
    MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      break;

    /**************************************************************************/
    /* Shomm Mode                                                             */
    /**************************************************************************/

    case 510:
    case 515:
    case 520:
      switch(test_cond){
        case 510:
	      printf("Enter ALL0(0) / ALL1(1) ->");DigitInput(&tempo1);
          if(tempo1 == 1){
            sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
	        sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
	
          }else{
            sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
            sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
          }
	    break;
	    case 515:
          printf("Enter CHKA(0) / CHKB(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	        sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);
     	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
          }
      	break;
	    case 520:
          printf("Enter DIAG-XInc(0) / DIAG-XDec(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	        sprintf(name_parts3,"_MULTISCAND");
	        sprintf(name_parts4,"_XS");
          }
      	break;
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
	ffpt_bak = Ffpt;
	vcc[1]=vcc[2]=500;

          printf("vddh=1.50V -> 600(mV)\n"); // vddh=1.50V //
          printf("vddh=1.49V -> 596(mV)\n"); // vddh=1.49V //
          printf("vddh=1.48V -> 592(mV)\n"); // vddh=1.48V //
          printf("vddh=1.47V -> 588(mV)\n"); // vddh=1.47V //
          printf("vddh=1.46V -> 584(mV)\n"); // vddh=1.46V //
          printf("vddh=1.45V max2 -> 580(mV)\n"); // vddh=1.45V //
          printf("vddh=1.44V -> 576(mV)\n"); // vddh=1.44V //
          printf("vddh=1.43V -> 572(mV)\n"); // vddh=1.43V //
          printf("vddh=1.42V -> 568(mV)\n"); // vddh=1.42V //
          printf("vddh=1.41V -> 564(mV)\n"); // vddh=1.41V //
          printf("vddh=1.40V -> 560(mV)\n"); // vddh=1.40V //
          printf("vddh=1.39V -> 556(mV)\n"); // vddh=1.39V //
          printf("vddh=1.38V -> 552(mV)\n"); // vddh=1.38V //
          printf("vddh=1.37V -> 548(mV)\n"); // vddh=1.37V //
          printf("vddh=1.36V -> 544(mV)\n"); // vddh=1.36V //
          printf("vddh=1.35V max -> 540(mV)\n"); // vddh=1.35V //
          printf("vddh=1.34V -> 536(mV)\n"); // vddh=1.34V //
          printf("vddh=1.33V -> 532(mV)\n"); // vddh=1.33V //
          printf("vddh=1.32V -> 528(mV)\n"); // vddh=1.32V //
          printf("vddh=1.31V -> 524(mV)\n"); // vddh=1.31V //
          printf("vddh=1.30V -> 520(mV)\n"); // vddh=1.30V //
          printf("vddh=1.29V -> 516(mV)\n"); // vddh=1.29V //
          printf("vddh=1.28V -> 512(mV)\n"); // vddh=1.28V //
          printf("vddh=1.27V -> 508(mV)\n"); // vddh=1.27V //
          printf("vddh=1.26V -> 504(mV)\n"); // vddh=1.26V //
          printf("vddh=1.25V typ -> 500(mV)\n"); // vddh=1.25V //
          printf("vddh=1.24V -> 496(mV)\n"); // vddh=1.24V //
          printf("vddh=1.23V -> 492(mV)\n"); // vddh=1.23V //
          printf("vddh=1.22V -> 488(mV)\n"); // vddh=1.22V //
          printf("vddh=1.21V -> 484(mV)\n"); // vddh=1.21V //
          printf("vddh=1.20V -> 480(mV)\n"); // vddh=1.20V //
          printf("vddh=1.19V -> 476(mV)\n"); // vddh=1.19V //
          printf("vddh=1.18V -> 472(mV)\n"); // vddh=1.18V //
          printf("vddh=1.17V -> 468(mV)\n"); // vddh=1.17V //
          printf("vddh=1.16V -> 464(mV)\n"); // vddh=1.16V //
          printf("vddh=1.15V -> 460(mV)\n"); // vddh=1.15V //
          printf("vddh=1.14V -> 456(mV)\n"); // vddh=1.14V //
          printf("vddh=1.13V min -> 452(mV)\n"); // vddh=1.13V //
          printf("vddh=1.12V -> 448(mV)\n"); // vddh=1.12V //
          printf("vddh=1.11V -> 444(mV)\n"); // vddh=1.11V //
          printf("vddh=1.10V -> 440(mV)\n"); // vddh=1.10V //
          printf("vddh=1.09V -> 436(mV)\n"); // vddh=1.09V //
          printf("vddh=1.08V -> 432(mV)\n"); // vddh=1.08V //
          printf("vddh=1.07V -> 428(mV)\n"); // vddh=1.07V //
          printf("vddh=1.06V -> 424(mV)\n"); // vddh=1.06V //
          printf("vddh=1.05V -> 420(mV)\n"); // vddh=1.05V //
          printf("vddh=1.04V -> 416(mV)\n"); // vddh=1.04V //
          printf("vddh=1.03V -> 412(mV)\n"); // vddh=1.03V //
          printf("vddh=1.02V -> 408(mV)\n"); // vddh=1.02V //
          printf("vddh=1.01V -> 404(mV)\n"); // vddh=1.01V //
          printf("vddh=1.00V -> 400(mV)\n"); // vddh=1.00V //
          printf("Enter vddh setting? ex) 500(mV) -> ");DigitInput(&tempo6);

          printf("\nvdd=1.20V -> 759mV)\n"); // vdd=1.20V //
          printf("vdd=1.19V -> 753mV)\n"); // vdd=1.19V //
          printf("vdd=1.18V -> 747mV)\n"); // vdd=1.18V //
          printf("vdd=1.17V -> 741mV)\n"); // vdd=1.17V //
          printf("vdd=1.16V -> 734mV)\n"); // vdd=1.16V //
          printf("vdd=1.15V max2 -> 728mV)\n"); // vdd=1.15V //
          printf("vdd=1.14V -> 722mV)\n"); // vdd=1.14V //
          printf("vdd=1.13V -> 715mV)\n"); // vdd=1.13V //
          printf("vdd=1.12V -> 709mV)\n"); // vdd=1.12V //
          printf("vdd=1.11V -> 703mV)\n"); // vdd=1.11V //
          printf("vdd=1.10V max -> 696mV)\n"); // vdd=1.10V //
          printf("vdd=1.09V -> 690mV)\n"); // vdd=1.09V //
          printf("vdd=1.08V -> 684mV)\n"); // vdd=1.08V //
          printf("vdd=1.07V -> 677mV)\n"); // vdd=1.07V //
          printf("vdd=1.06V -> 671mV)\n"); // vdd=1.06V //
          printf("vdd=1.05V -> 665mV)\n"); // vdd=1.05V //
          printf("vdd=1.00V -> 633mV)\n"); // vdd=1.00V //
          printf("vdd=0.85V -> 538(mV)\n"); // vdd=0.85V //
          printf("vdd=0.84V -> 532(mV)\n"); // vdd=0.84V //
          printf("vdd=0.83V -> 525(mV)\n"); // vdd=0.83V //
          printf("vdd=0.82V -> 519(mV)\n"); // vdd=0.82V //
          printf("vdd=0.81V -> 513(mV)\n"); // vdd=0.81V //
          printf("vdd=0.80V -> 506(mV)\n"); // vdd=0.80V //
          printf("vdd=0.79V typ -> 500(mV)\n"); // vdd=0.79V //
          printf("vdd=0.78V -> 494(mV)\n"); // vdd=0.78V //
          printf("vdd=0.77V -> 487(mV)\n"); // vdd=0.77V //
          printf("vdd=0.76V -> 481(mV)\n"); // vdd=0.76V //
          printf("vdd=0.75V -> 475(mV)\n"); // vdd=0.75V //
          printf("vdd=0.74V -> 468(mV)\n"); // vdd=0.74V //
          printf("vdd=0.73V -> 462(mV)\n"); // vdd=0.73V //
          printf("vdd=0.72V min -> 456(mV)\n"); // vdd=0.72V //
          printf("vdd=0.71V -> 449(mV)\n"); // vdd=0.71V //
          printf("vdd=0.70V -> 443(mV)\n"); // vdd=0.70V //
          printf("vdd=0.69V -> 437(mV)\n"); // vdd=0.69V //
          printf("vdd=0.68V -> 430(mV)\n"); // vdd=0.68V //
          printf("vdd=0.67V -> 424(mV)\n"); // vdd=0.67V //
          printf("vdd=0.66V -> 418(mV)\n"); // vdd=0.66V //
          printf("vdd=0.65V -> 411(mV)\n"); // vdd=0.65V //
          printf("vdd=0.64V -> 405(mV)\n"); // vdd=0.64V //
          printf("vdd=0.63V -> 399(mV)\n"); // vdd=0.63V //
          printf("vdd=0.62V -> 392(mV)\n"); // vdd=0.62V //
          printf("vdd=0.61V -> 386(mV)\n"); // vdd=0.61V //
          printf("vdd=0.60V -> 380(mV)\n"); // vdd=0.60V //
          printf("Enter vdd setting? ex) 500(mV) -> ");DigitInput(&tempo7);

          printf("*** Test Temp   [-40:40 / 25C:25  / 90C:90 ] ->"); DigitInput(&cond2);
          if      (cond2 == 40) strcpy(test_name,"_-40C");
          else if (cond2 == 25) strcpy(test_name,"_25C");
          else if (cond2 == 90) strcpy(test_name,"_90C");
          else                  strcpy(test_name,"_000");

    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2
    MinoriModeFunc_Repeat_F(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");

      for(loop_num=1; loop_num<=2; loop_num++){
        switch(loop_num){
	  case 1:  // VDD
//	    loop_max   = 1130;//org//
	    loop_max   = 850;
	    loop_min   = 610;
	    loop_delta = 20;
	    sprintf(mesure_name,"VDD_mV"); 
	  break; 
          case 2:  // VDDH
	    loop_max   = 1330;
	    loop_min   = 1070;
	    loop_delta = 20;
	    sprintf(mesure_name,"VDDH_mV");
	  break;
          case 3:  // ETLR26
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR26_30");
	  break;
          case 4:  // ETLR28
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR28_40");
	  break;
          case 5:  // ETLR32[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR32_30");
	  break;
          case 6:  // ETLR32[7:4]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR32_74");
	  break;
          case 7:  // ETLR33[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR33_30");
	  break;
          case 8:  // ETLR34[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR34_30");
	  break;
	  
        }
        //Initial Setting
	      if ( loop_num == 1 ) { // VDD fix Eval //
              vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
              }
	      else if ( loop_num == 2 ) {   // VDDH fix Eval //
              vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
              }
	      else if ( loop_num != 1 && loop_num != 2 ) {   // VDDH fix Eval //
	      	vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
          }
	
	// FileName Setting
        sprintf(filename,"SHOMM%s_%s%s",name_parts1,mesure_name,test_name);
        if(PASS != OpenDataOutFile(filename)) delay_timer(2*1000*1000);
        fprintf(Fptdata," module,%s \n patten,%s \n vcc,%d,vddh,%d,vdd,%d,CLK-Mode,%s\n",module,wgl_name,vcc[0],vcc[1],vcc[2],clk_mode);

        fprintf(Fptdata,"*** etlr25<7:0>=0x%X (iref)\n",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));

        fprintf(Fptdata,"*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));
	    fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",mesure_name);
        CloseDataOutFile();delay_timer(200);

        
        for(loop=loop_max;loop>=loop_min;loop=loop-loop_delta){ //Loop
           OpenDataOutFile(filename);

	   
	   switch(loop_num){
	     case 1: //Vdd Setting
               vcc[2]=loop*100/158;
	       fprintf(Fptdata,"%d",loop); 
	     break; 
             case 2: //Vddh Setting
               vcc[1]=loop*10/25;
	       fprintf(Fptdata,"%d",loop); 
	     break;
             case 3: //ETLR33 Setting T4
////	       SetValue(IN_ETLR32,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break; //Def
	         case 5:tempo1 = 0x44 ;break;
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR33;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR33,param)); 
	     break;
             case 4: //ETLR34 Setting T6(T4-T5)
//	       SetValue(IN_ETLR32,param,0xFF); // T4 //
////	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break;
	         case 5:tempo1 = 0x44 ;break; //Def
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR34;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR34,param)); 
	     break;
             case 5: //ETLR32 Setting T5
//	       SetValue(IN_ETLR32,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
////	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x10 ;break;
	         case 3:tempo1 = 0x20 ;break;
	         case 4:tempo1 = 0x30 ;break; //Def
	         case 5:tempo1 = 0x40 ;break;
	         case 6:tempo1 = 0x50 ;break;
	         case 7:tempo1 = 0x60 ;break;
	         case 8:tempo1 = 0x70 ;break; 
	         case 9:tempo1 = 0x80 ;break; 
	         case 10:tempo1 = 0x90 ;break; 
	         case 11:tempo1 = 0xA0 ;break; 
	         case 12:tempo1 = 0xB0 ;break;
	         case 13:tempo1 = 0xC0 ;break;
	         case 14:tempo1 = 0xD0 ;break;
	         case 15:tempo1 = 0xE0 ;break;
	         case 16:tempo1 = 0xF0 ;break;
	       }
	       tempo2 = IN_ETLR32;
	       SetValue(tempo2,param, ((GetValue(tempo2,param) & 0x0F) | tempo1) );
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR32,param)); 
	     break;
	   }
	 
           for(tempo1=0;tempo1<=15;tempo1++){
               switch(tempo1){
	         case 0 :SetValue(IN_EXTAL1,param,1000);break;
	         case 1 :SetValue(IN_EXTAL1,param,100);break;
                 case 2 :SetValue(IN_EXTAL1,param,50);break;
	         case 3 :SetValue(IN_EXTAL1,param,40);break;
	         case 4 :SetValue(IN_EXTAL1,param,33);break;
	         case 5 :SetValue(IN_EXTAL1,param,32);break;
	         case 6 :SetValue(IN_EXTAL1,param,31);break;
	         case 7 :SetValue(IN_EXTAL1,param,30);break;
	         case 8 :SetValue(IN_EXTAL1,param,29);break;
	         case 9 :SetValue(IN_EXTAL1,param,28);break;
	         case 10:SetValue(IN_EXTAL1,param,25);break;
	         case 11:SetValue(IN_EXTAL1,param,24);break;
	         case 12:SetValue(IN_EXTAL1,param,23);break;
	         case 13:SetValue(IN_EXTAL1,param,22);break;
	         case 14:SetValue(IN_EXTAL1,param,21);break;
	         case 15:SetValue(IN_EXTAL1,param,20);break;
	       }
			  printf("loop = %4d ,Vdd =%4dmV, Vddh =%4dmV T = %4dns : ",loop,vcc[2],vcc[1],GetValue(IN_EXTAL1,param));
               DeviceSpecificPowerUp();delay_timer(200);

               waittime = 15;
			   Ffpt = 0;
               result = MinoriModeFunc_Repeat(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      		   delay_timer(1000);
               if(result == PASS){
                 printf("***PASS***\n");
                 fprintf(Fptdata,",P");
	       }else if(result == FAIL){
	         printf("---FAIL---\n");
	         fprintf(Fptdata,",F");
	       }else{
	         printf("-- ERROR!!! --\n");
	         fprintf(Fptdata,",E");
		   }
	     
           }//speed Loop
        fprintf(Fptdata,",etlr25=0x%X ",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"lr26=0x%X ",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"lr27=0x%X ",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"lr28=0x%X ",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"lr29=0x%X ",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"lr30=0x%X ",GetValue(IN_ETLR30,param));
        fprintf(Fptdata,"lr32=0x%X ",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"lr33=0x%X ",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"lr34=0x%X ",GetValue(IN_ETLR34,param));
	   fprintf(Fptdata,"\n");
	   CloseDataOutFile();delay_timer(200);
         }//vdd or reg Loop
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
        SetValue(IN_EXTAL1,param,freqpara); 
      }//loop_num Loop
	vcc[1]=vcc[2]=500;

        SetValue(IN_EXTAL1,param,freqpara); 
        
        SetValue(IN_ETLR25,param,etlr25);
        SetValue(IN_ETLR26,param,etlr26);
        SetValue(IN_ETLR27,param,etlr27);
        SetValue(IN_ETLR28,param,etlr28);
        SetValue(IN_ETLR29,param,etlr29);
        SetValue(IN_ETLR30,param,etlr30);
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
    DeviceSpecificPowerDown();
    DeviceLevelsPowerDown();

	  Ffpt = ffpt_bak;
    break;


    /**************************************************************************/
    /* Shomm Mode                                                             */
    /**************************************************************************/

    case 5100:
    case 5155:
    case 5200:
      switch(test_cond){
        case 5100:
	      printf("Enter ALL0(0) / ALL1(1) ->");DigitInput(&tempo1);
          if(tempo1 == 1){
            sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
	        sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
	
          }else{
            sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
            sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
          }
	    break;
	    case 5155:
          printf("Enter CHKA(0) / CHKB(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	        sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);
     	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
          }
      	break;
	    case 5200:
          printf("Enter DIAG-XInc(0) / DIAG-XDec(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,1);
	        sprintf(name_parts3,"_MULTISCAND");
	        sprintf(name_parts4,"_XS");
          }
      	break;
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
	ffpt_bak = Ffpt;
	vcc[1]=vcc[2]=500;

          printf("*** Test Temp   [-40:40 / 25C:25  / 95C:95 ] ->"); DigitInput(&cond2);
          if      (cond2 == 40) strcpy(test_name,"_-40C");
          else if (cond2 == 25) strcpy(test_name,"_25C");
          else if (cond2 == 95) strcpy(test_name,"_95C");
          else                  strcpy(test_name,"_000");


          printf("vddh=1.50V -> 600(mV)\n"); // vddh=1.50V //
          printf("vddh=1.49V -> 596(mV)\n"); // vddh=1.49V //
          printf("vddh=1.48V -> 592(mV)\n"); // vddh=1.48V //
          printf("vddh=1.47V -> 588(mV)\n"); // vddh=1.47V //
          printf("vddh=1.46V -> 584(mV)\n"); // vddh=1.46V //
          printf("vddh=1.45V max2 -> 580(mV)\n"); // vddh=1.45V //
          printf("vddh=1.44V -> 576(mV)\n"); // vddh=1.44V //
          printf("vddh=1.43V -> 572(mV)\n"); // vddh=1.43V //
          printf("vddh=1.42V -> 568(mV)\n"); // vddh=1.42V //
          printf("vddh=1.41V -> 564(mV)\n"); // vddh=1.41V //
          printf("vddh=1.40V -> 560(mV)\n"); // vddh=1.40V //
          printf("vddh=1.39V -> 556(mV)\n"); // vddh=1.39V //
          printf("vddh=1.38V -> 552(mV)\n"); // vddh=1.38V //
          printf("vddh=1.37V -> 548(mV)\n"); // vddh=1.37V //
          printf("vddh=1.36V -> 544(mV)\n"); // vddh=1.36V //
          printf("vddh=1.35V max -> 540(mV)\n"); // vddh=1.35V //
          printf("vddh=1.34V -> 536(mV)\n"); // vddh=1.34V //
          printf("vddh=1.33V -> 532(mV)\n"); // vddh=1.33V //
          printf("vddh=1.32V -> 528(mV)\n"); // vddh=1.32V //
          printf("vddh=1.31V -> 524(mV)\n"); // vddh=1.31V //
          printf("vddh=1.30V -> 520(mV)\n"); // vddh=1.30V //
          printf("vddh=1.29V -> 516(mV)\n"); // vddh=1.29V //
          printf("vddh=1.28V -> 512(mV)\n"); // vddh=1.28V //
          printf("vddh=1.27V -> 508(mV)\n"); // vddh=1.27V //
          printf("vddh=1.26V -> 504(mV)\n"); // vddh=1.26V //
          printf("vddh=1.25V typ -> 500(mV)\n"); // vddh=1.25V //
          printf("vddh=1.24V -> 496(mV)\n"); // vddh=1.24V //
          printf("vddh=1.23V -> 492(mV)\n"); // vddh=1.23V //
          printf("vddh=1.22V -> 488(mV)\n"); // vddh=1.22V //
          printf("vddh=1.21V -> 484(mV)\n"); // vddh=1.21V //
          printf("vddh=1.20V -> 480(mV)\n"); // vddh=1.20V //
          printf("vddh=1.19V -> 476(mV)\n"); // vddh=1.19V //
          printf("vddh=1.18V -> 472(mV)\n"); // vddh=1.18V //
          printf("vddh=1.17V -> 468(mV)\n"); // vddh=1.17V //
          printf("vddh=1.16V -> 464(mV)\n"); // vddh=1.16V //
          printf("vddh=1.15V -> 460(mV)\n"); // vddh=1.15V //
          printf("vddh=1.14V -> 456(mV)\n"); // vddh=1.14V //
          printf("vddh=1.13V min -> 452(mV)\n"); // vddh=1.13V //
          printf("vddh=1.12V -> 448(mV)\n"); // vddh=1.12V //
          printf("vddh=1.11V -> 444(mV)\n"); // vddh=1.11V //
          printf("vddh=1.10V -> 440(mV)\n"); // vddh=1.10V //
          printf("vddh=1.09V -> 436(mV)\n"); // vddh=1.09V //
          printf("vddh=1.08V -> 432(mV)\n"); // vddh=1.08V //
          printf("vddh=1.07V -> 428(mV)\n"); // vddh=1.07V //
          printf("vddh=1.06V -> 424(mV)\n"); // vddh=1.06V //
          printf("vddh=1.05V -> 420(mV)\n"); // vddh=1.05V //
          printf("vddh=1.04V -> 416(mV)\n"); // vddh=1.04V //
          printf("vddh=1.03V -> 412(mV)\n"); // vddh=1.03V //
          printf("vddh=1.02V -> 408(mV)\n"); // vddh=1.02V //
          printf("vddh=1.01V -> 404(mV)\n"); // vddh=1.01V //
          printf("vddh=1.00V -> 400(mV)\n"); // vddh=1.00V //
          printf("Enter vddh setting? ex) 500(mV) -> ");//DigitInput(&tempo6);

          printf("\nvdd=1.20V -> 759mV)\n"); // vdd=1.20V //
          printf("vdd=1.19V -> 753mV)\n"); // vdd=1.19V //
          printf("vdd=1.18V -> 747mV)\n"); // vdd=1.18V //
          printf("vdd=1.17V -> 741mV)\n"); // vdd=1.17V //
          printf("vdd=1.16V -> 734mV)\n"); // vdd=1.16V //
          printf("vdd=1.15V max2 -> 728mV)\n"); // vdd=1.15V //
          printf("vdd=1.14V -> 722mV)\n"); // vdd=1.14V //
          printf("vdd=1.13V -> 715mV)\n"); // vdd=1.13V //
          printf("vdd=1.12V -> 709mV)\n"); // vdd=1.12V //
          printf("vdd=1.11V -> 703mV)\n"); // vdd=1.11V //
          printf("vdd=1.10V max -> 696mV)\n"); // vdd=1.10V //
          printf("vdd=1.09V -> 690mV)\n"); // vdd=1.09V //
          printf("vdd=1.08V -> 684mV)\n"); // vdd=1.08V //
          printf("vdd=1.07V -> 677mV)\n"); // vdd=1.07V //
          printf("vdd=1.06V -> 671mV)\n"); // vdd=1.06V //
          printf("vdd=1.05V -> 665mV)\n"); // vdd=1.05V //
          printf("vdd=1.00V -> 633mV)\n"); // vdd=1.00V //
          printf("vdd=0.85V -> 538(mV)\n"); // vdd=0.85V //
          printf("vdd=0.84V -> 532(mV)\n"); // vdd=0.84V //
          printf("vdd=0.83V -> 525(mV)\n"); // vdd=0.83V //
          printf("vdd=0.82V -> 519(mV)\n"); // vdd=0.82V //
          printf("vdd=0.81V -> 513(mV)\n"); // vdd=0.81V //
          printf("vdd=0.80V -> 506(mV)\n"); // vdd=0.80V //
          printf("vdd=0.79V typ -> 500(mV)\n"); // vdd=0.79V //
          printf("vdd=0.78V -> 494(mV)\n"); // vdd=0.78V //
          printf("vdd=0.77V -> 487(mV)\n"); // vdd=0.77V //
          printf("vdd=0.76V -> 481(mV)\n"); // vdd=0.76V //
          printf("vdd=0.75V -> 475(mV)\n"); // vdd=0.75V //
          printf("vdd=0.74V -> 468(mV)\n"); // vdd=0.74V //
          printf("vdd=0.73V -> 462(mV)\n"); // vdd=0.73V //
          printf("vdd=0.72V min -> 456(mV)\n"); // vdd=0.72V //
          printf("vdd=0.71V -> 449(mV)\n"); // vdd=0.71V //
          printf("vdd=0.70V -> 443(mV)\n"); // vdd=0.70V //
          printf("vdd=0.69V -> 437(mV)\n"); // vdd=0.69V //
          printf("vdd=0.68V -> 430(mV)\n"); // vdd=0.68V //
          printf("vdd=0.67V -> 424(mV)\n"); // vdd=0.67V //
          printf("vdd=0.66V -> 418(mV)\n"); // vdd=0.66V //
          printf("vdd=0.65V -> 411(mV)\n"); // vdd=0.65V //
          printf("vdd=0.64V -> 405(mV)\n"); // vdd=0.64V //
          printf("vdd=0.63V -> 399(mV)\n"); // vdd=0.63V //
          printf("vdd=0.62V -> 392(mV)\n"); // vdd=0.62V //
          printf("vdd=0.61V -> 386(mV)\n"); // vdd=0.61V //
          printf("vdd=0.60V -> 380(mV)\n"); // vdd=0.60V //
          printf("Enter vdd setting? ex) 500(mV) -> ");//DigitInput(&tempo7);

    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2
    MinoriModeFunc_Repeat_F(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
    vcc[0]=1600; //vcc setting//

for(j=1; j<=2; j++){
  	      if ( j == 1 ) { // Vmin //
			printf("\nVminVminVminVminVminVmin\n");//
              tempo6=452; //vddh=1.13V setting//
              tempo7=456; //vdd=0.72V setting//
              }
	      else if ( j == 2 ) {   // Vmax //
			printf("\nVmaxVmaxVmaxVmaxVmaxVmax\n");//
              tempo6=580; //vddh=1.45V setting//
              tempo7=728; //vdd=1.15V setting//
              }

      for(loop_num=1; loop_num<=2; loop_num++){
        switch(loop_num){
	  case 1:  // VDD
			printf("\nDebug2-1\n");//
	    loop_max   = 1200;//org//
//	    loop_max   = 850;
	    loop_min   = 600;
	    loop_delta = 20;
	    sprintf(mesure_name,"VDD_mV"); 
	  break; 
          case 2:  // VDDH
	    loop_max   = 1500;
	    loop_min   = 1000;
	    loop_delta = 20;
	    sprintf(mesure_name,"VDDH_mV");
	  break;
          case 3:  // ETLR26
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR26_30");
	  break;
          case 4:  // ETLR28
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR28_40");
	  break;
          case 5:  // ETLR32[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR32_30");
	  break;
          case 6:  // ETLR32[7:4]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR32_74");
	  break;
          case 7:  // ETLR33[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR33_30");
	  break;
          case 8:  // ETLR34[3:0]
	    loop_max   = 5;
	    loop_min   = 1;
	    loop_delta = 1;
	    sprintf(mesure_name,"ETLR34_30");
	  break;
	  
        }
        //Initial Setting
	      if ( loop_num == 1 ) { // VDD fix Eval //
			printf("\nDebug2-1\n");//
              vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
              }
	      else if ( loop_num == 2 ) {   // VDDH fix Eval //
			printf("\nDebug2-1\n");//
              vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
              }
	      else if ( loop_num != 1 && loop_num != 2 ) {   // VDDH fix Eval //
			printf("\nDebug2-3\n");//

	      	vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
          }
	
	// FileName Setting
        sprintf(filename,"SHOMM%s_%s%s",name_parts1,mesure_name,test_name);
        if(PASS != OpenDataOutFile(filename)) delay_timer(2*1000*1000);
        fprintf(Fptdata," module,%s \n patten,%s \n vcc,%d,vddh,%d,vdd,%d,CLK-Mode,%s\n",module,wgl_name,vcc[0],vcc[1],vcc[2],clk_mode);

        fprintf(Fptdata,"*** etlr25<7:0>=0x%X (iref)\n",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));

        fprintf(Fptdata,"*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));
	    fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",mesure_name);
        CloseDataOutFile();delay_timer(200);

        
        for(loop=loop_max;loop>=loop_min;loop=loop-loop_delta){ //Loop
           OpenDataOutFile(filename);

	   
	   switch(loop_num){
	     case 1: //Vdd Setting
               vcc[2]=loop*100/158;
	       fprintf(Fptdata,"%d",loop); 
	     break; 
             case 2: //Vddh Setting
               vcc[1]=loop*10/25;
	       fprintf(Fptdata,"%d",loop); 
	     break;
             case 3: //ETLR33 Setting T4
////	       SetValue(IN_ETLR32,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break; //Def
	         case 5:tempo1 = 0x44 ;break;
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR33;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR33,param)); 
	     break;
             case 4: //ETLR34 Setting T6(T4-T5)
//	       SetValue(IN_ETLR32,param,0xFF); // T4 //
////	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break;
	         case 5:tempo1 = 0x44 ;break; //Def
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR34;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR34,param)); 
	     break;
             case 5: //ETLR32 Setting T5
//	       SetValue(IN_ETLR32,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
////	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x10 ;break;
	         case 3:tempo1 = 0x20 ;break;
	         case 4:tempo1 = 0x30 ;break; //Def
	         case 5:tempo1 = 0x40 ;break;
	         case 6:tempo1 = 0x50 ;break;
	         case 7:tempo1 = 0x60 ;break;
	         case 8:tempo1 = 0x70 ;break; 
	         case 9:tempo1 = 0x80 ;break; 
	         case 10:tempo1 = 0x90 ;break; 
	         case 11:tempo1 = 0xA0 ;break; 
	         case 12:tempo1 = 0xB0 ;break;
	         case 13:tempo1 = 0xC0 ;break;
	         case 14:tempo1 = 0xD0 ;break;
	         case 15:tempo1 = 0xE0 ;break;
	         case 16:tempo1 = 0xF0 ;break;
	       }
	       tempo2 = IN_ETLR32;
	       SetValue(tempo2,param, ((GetValue(tempo2,param) & 0x0F) | tempo1) );
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR32,param)); 
	     break;
	   }
	 
           for(tempo1=0;tempo1<=15;tempo1++){
               switch(tempo1){
	         case 0 :SetValue(IN_EXTAL1,param,1000);break;
	         case 1 :SetValue(IN_EXTAL1,param,100);break;
                 case 2 :SetValue(IN_EXTAL1,param,50);break;
	         case 3 :SetValue(IN_EXTAL1,param,40);break;
	         case 4 :SetValue(IN_EXTAL1,param,33);break;
	         case 5 :SetValue(IN_EXTAL1,param,32);break;
	         case 6 :SetValue(IN_EXTAL1,param,31);break;
	         case 7 :SetValue(IN_EXTAL1,param,30);break;
	         case 8 :SetValue(IN_EXTAL1,param,29);break;
	         case 9 :SetValue(IN_EXTAL1,param,28);break;
	         case 10:SetValue(IN_EXTAL1,param,25);break;
	         case 11:SetValue(IN_EXTAL1,param,24);break;
	         case 12:SetValue(IN_EXTAL1,param,23);break;
	         case 13:SetValue(IN_EXTAL1,param,22);break;
	         case 14:SetValue(IN_EXTAL1,param,21);break;
	         case 15:SetValue(IN_EXTAL1,param,20);break;
	       }
			  printf("loop = %4d ,Vdd =%4dmV, Vddh =%4dmV T = %4dns : ",loop,vcc[2],vcc[1],GetValue(IN_EXTAL1,param));
               DeviceSpecificPowerUp();delay_timer(200);

               waittime = 15;
			   Ffpt = 0;
               result = MinoriModeFunc_Repeat(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      		   delay_timer(10*1000);
               if(result == PASS){
                 printf("***PASS***\n");
                 fprintf(Fptdata,",P");
	       }else if(result == FAIL){
	         printf("---FAIL---\n");
	         fprintf(Fptdata,",F");
	       }else{
	         printf("-- ERROR!!! --\n");
	         fprintf(Fptdata,",E");
		   }
	     
           }//speed Loop
        fprintf(Fptdata,",etlr25=0x%X ",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"lr26=0x%X ",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"lr27=0x%X ",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"lr28=0x%X ",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"lr29=0x%X ",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"lr30=0x%X ",GetValue(IN_ETLR30,param));
        fprintf(Fptdata,"lr32=0x%X ",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"lr33=0x%X ",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"lr34=0x%X ",GetValue(IN_ETLR34,param));
	   fprintf(Fptdata,"\n");
	   CloseDataOutFile();delay_timer(200);
         }//vdd or reg Loop
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
        SetValue(IN_EXTAL1,param,freqpara); 
      }//loop_num Loop
}//for VminVmax
	vcc[1]=vcc[2]=500;

        SetValue(IN_EXTAL1,param,freqpara); 
        
        SetValue(IN_ETLR25,param,etlr25);
        SetValue(IN_ETLR26,param,etlr26);
        SetValue(IN_ETLR27,param,etlr27);
        SetValue(IN_ETLR28,param,etlr28);
        SetValue(IN_ETLR29,param,etlr29);
        SetValue(IN_ETLR30,param,etlr30);
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
    DeviceSpecificPowerDown();
    DeviceLevelsPowerDown();

	  Ffpt = ffpt_bak;
    break;

    /**************************************************************************/
    /* Shomm Mode2                                                            */
    /**************************************************************************/

    case 610:
    case 615:
    case 620:

/**************************************************************************************/
/***   Chip-Infomation Read                                                         ***/
/**************************************************************************************/
      SetValue(IN_REG,param,0x03);
      SetValue(IN_TESTSEL,param,0);        // VthRead count mode
      SetValue(IN_BDATA00,param,0);
      stack0 = SetValue(IN_AREA,param,5);	 /* Extra5 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vcc[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B);
      info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B);
      info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B);
      info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[6] = ReadEcrMode8(FBM_READTOP+0x0026,SIZE_B);
      info[10]= ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11]= ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B);
      info[12]= ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);
      info[13]= ReadEcrMode8(FBM_READTOP+0x0098,SIZE_B);
      info[14]= ReadEcrMode8(FBM_READTOP+0x0099,SIZE_B);
      info[15]= ReadEcrMode8(FBM_READTOP+0x009A,SIZE_B);
      info[16]= ReadEcrMode8(FBM_READTOP+0x009B,SIZE_B);
      info[17]= ReadEcrMode8(FBM_READTOP+0x009C,SIZE_B);
      info[18]= ReadEcrMode8(FBM_READTOP+0x009D,SIZE_B);
      info[19]= ReadEcrMode8(FBM_READTOP+0x009E,SIZE_B);

      SetValue(IN_AREA,param,4);	 /* Extra4 */
      SetValue(IN_TOPADDR0,param,0);
      SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vcc[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");  /*�t�@�[������ŕύX����\������*/
      info[20] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_L);

      printf("\n");
      printf("*** L# : %c%c%c%c%c%c\n",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d\n",info[10]);
      printf("***  X : %3d\n",info[11]);
      printf("***  Y : %3d\n",info[12]);
      printf("***  S : %3d\n",info[20]);
      info[20] = info[20] - 10000;
	  SetValue(IN_AREA,param,stack0);
      

      switch(test_cond){
        case 610:
	  printf("Enter ALL0(0) / ALL1(1) ->");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	    sprintf(name_parts3,"_MULTISCANI");
	    sprintf(name_parts4,"_XS");
	
          }else{
            sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
            sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	    sprintf(name_parts3,"_MULTISCANI");
	    sprintf(name_parts4,"_XS");
	
          }
	  break;
	    case 615:
          printf("Enter CHKA(0) / CHKB(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	        sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);
     	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_YS");
          }
          break;
	    case 620:
          printf("Enter DIAG-XInc(0) / DIAG-XDec(1)");DigitInput(&tempo1);
          if(tempo1 == 0){
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCANI");
	        sprintf(name_parts4,"_XS");
			printf("\nDebug1\n");
          }else{
            sprintf(name_parts1,"_DG");SetValue(IN_TESTSEL,param,1);
	        sprintf(name_parts2,"_M1");SetValue(IN_BDATA00,param,0);
	        sprintf(name_parts3,"_MULTISCAND");
	        sprintf(name_parts4,"_XS");
          }
          break;
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
	ffpt_bak = Ffpt;

	vcc[1]=vcc[2]=500;

          printf("vddh=1.50V -> 600(mV)\n"); // vddh=1.50V //
          printf("vddh=1.49V -> 596(mV)\n"); // vddh=1.49V //
          printf("vddh=1.48V -> 592(mV)\n"); // vddh=1.48V //
          printf("vddh=1.47V -> 588(mV)\n"); // vddh=1.47V //
          printf("vddh=1.46V -> 584(mV)\n"); // vddh=1.46V //
          printf("vddh=1.45V max2 -> 580(mV)\n"); // vddh=1.45V //
          printf("vddh=1.44V -> 576(mV)\n"); // vddh=1.44V //
          printf("vddh=1.43V -> 572(mV)\n"); // vddh=1.43V //
          printf("vddh=1.42V -> 568(mV)\n"); // vddh=1.42V //
          printf("vddh=1.41V -> 564(mV)\n"); // vddh=1.41V //
          printf("vddh=1.40V -> 560(mV)\n"); // vddh=1.40V //
          printf("vddh=1.39V -> 556(mV)\n"); // vddh=1.39V //
          printf("vddh=1.38V -> 552(mV)\n"); // vddh=1.38V //
          printf("vddh=1.37V -> 548(mV)\n"); // vddh=1.37V //
          printf("vddh=1.36V -> 544(mV)\n"); // vddh=1.36V //
          printf("vddh=1.35V max -> 540(mV)\n"); // vddh=1.35V //
          printf("vddh=1.34V -> 536(mV)\n"); // vddh=1.34V //
          printf("vddh=1.33V -> 532(mV)\n"); // vddh=1.33V //
          printf("vddh=1.32V -> 528(mV)\n"); // vddh=1.32V //
          printf("vddh=1.31V -> 524(mV)\n"); // vddh=1.31V //
          printf("vddh=1.30V -> 520(mV)\n"); // vddh=1.30V //
          printf("vddh=1.29V -> 516(mV)\n"); // vddh=1.29V //
          printf("vddh=1.28V -> 512(mV)\n"); // vddh=1.28V //
          printf("vddh=1.27V -> 508(mV)\n"); // vddh=1.27V //
          printf("vddh=1.26V -> 504(mV)\n"); // vddh=1.26V //
          printf("vddh=1.25V typ -> 500(mV)\n"); // vddh=1.25V //
          printf("vddh=1.24V -> 496(mV)\n"); // vddh=1.24V //
          printf("vddh=1.23V -> 492(mV)\n"); // vddh=1.23V //
          printf("vddh=1.22V -> 488(mV)\n"); // vddh=1.22V //
          printf("vddh=1.21V -> 484(mV)\n"); // vddh=1.21V //
          printf("vddh=1.20V -> 480(mV)\n"); // vddh=1.20V //
          printf("vddh=1.19V -> 476(mV)\n"); // vddh=1.19V //
          printf("vddh=1.18V -> 472(mV)\n"); // vddh=1.18V //
          printf("vddh=1.17V -> 468(mV)\n"); // vddh=1.17V //
          printf("vddh=1.16V -> 464(mV)\n"); // vddh=1.16V //
          printf("vddh=1.15V -> 460(mV)\n"); // vddh=1.15V //
          printf("vddh=1.14V -> 456(mV)\n"); // vddh=1.14V //
          printf("vddh=1.13V min -> 452(mV)\n"); // vddh=1.13V //
          printf("vddh=1.12V -> 448(mV)\n"); // vddh=1.12V //
          printf("vddh=1.11V -> 444(mV)\n"); // vddh=1.11V //
          printf("vddh=1.10V -> 440(mV)\n"); // vddh=1.10V //
          printf("vddh=1.09V -> 436(mV)\n"); // vddh=1.09V //
          printf("vddh=1.08V -> 432(mV)\n"); // vddh=1.08V //
          printf("vddh=1.07V -> 428(mV)\n"); // vddh=1.07V //
          printf("vddh=1.06V -> 424(mV)\n"); // vddh=1.06V //
          printf("vddh=1.05V -> 420(mV)\n"); // vddh=1.05V //
          printf("vddh=1.04V -> 416(mV)\n"); // vddh=1.04V //
          printf("vddh=1.03V -> 412(mV)\n"); // vddh=1.03V //
          printf("vddh=1.02V -> 408(mV)\n"); // vddh=1.02V //
          printf("vddh=1.01V -> 404(mV)\n"); // vddh=1.01V //
          printf("vddh=1.00V -> 400(mV)\n"); // vddh=1.00V //
          printf("Enter vddh setting? ex) 500(mV) -> ");DigitInput(&tempo6);

          printf("\nvdd=1.20V -> 759mV)\n"); // vdd=1.20V //
          printf("vdd=1.19V -> 753mV)\n"); // vdd=1.19V //
          printf("vdd=1.18V -> 747mV)\n"); // vdd=1.18V //
          printf("vdd=1.17V -> 741mV)\n"); // vdd=1.17V //
          printf("vdd=1.16V -> 734mV)\n"); // vdd=1.16V //
          printf("vdd=1.15V max2 -> 728mV)\n"); // vdd=1.15V //
          printf("vdd=1.14V -> 722mV)\n"); // vdd=1.14V //
          printf("vdd=1.13V -> 715mV)\n"); // vdd=1.13V //
          printf("vdd=1.12V -> 709mV)\n"); // vdd=1.12V //
          printf("vdd=1.11V -> 703mV)\n"); // vdd=1.11V //
          printf("vdd=1.10V max -> 696mV)\n"); // vdd=1.10V //
          printf("vdd=1.09V -> 690mV)\n"); // vdd=1.09V //
          printf("vdd=1.08V -> 684mV)\n"); // vdd=1.08V //
          printf("vdd=1.07V -> 677mV)\n"); // vdd=1.07V //
          printf("vdd=1.06V -> 671mV)\n"); // vdd=1.06V //
          printf("vdd=1.05V -> 665mV)\n"); // vdd=1.05V //
          printf("vdd=1.00V -> 633mV)\n"); // vdd=1.00V //
          printf("vdd=0.85V -> 538(mV)\n"); // vdd=0.85V //
          printf("vdd=0.84V -> 532(mV)\n"); // vdd=0.84V //
          printf("vdd=0.83V -> 525(mV)\n"); // vdd=0.83V //
          printf("vdd=0.82V -> 519(mV)\n"); // vdd=0.82V //
          printf("vdd=0.81V -> 513(mV)\n"); // vdd=0.81V //
          printf("vdd=0.80V -> 506(mV)\n"); // vdd=0.80V //
          printf("vdd=0.79V typ -> 500(mV)\n"); // vdd=0.79V //
          printf("vdd=0.78V -> 494(mV)\n"); // vdd=0.78V //
          printf("vdd=0.77V -> 487(mV)\n"); // vdd=0.77V //
          printf("vdd=0.76V -> 481(mV)\n"); // vdd=0.76V //
          printf("vdd=0.75V -> 475(mV)\n"); // vdd=0.75V //
          printf("vdd=0.74V -> 468(mV)\n"); // vdd=0.74V //
          printf("vdd=0.73V -> 462(mV)\n"); // vdd=0.73V //
          printf("vdd=0.72V min -> 456(mV)\n"); // vdd=0.72V //
          printf("vdd=0.71V -> 449(mV)\n"); // vdd=0.71V //
          printf("vdd=0.70V -> 443(mV)\n"); // vdd=0.70V //
          printf("vdd=0.69V -> 437(mV)\n"); // vdd=0.69V //
          printf("vdd=0.68V -> 430(mV)\n"); // vdd=0.68V //
          printf("vdd=0.67V -> 424(mV)\n"); // vdd=0.67V //
          printf("vdd=0.66V -> 418(mV)\n"); // vdd=0.66V //
          printf("vdd=0.65V -> 411(mV)\n"); // vdd=0.65V //
          printf("vdd=0.64V -> 405(mV)\n"); // vdd=0.64V //
          printf("vdd=0.63V -> 399(mV)\n"); // vdd=0.63V //
          printf("vdd=0.62V -> 392(mV)\n"); // vdd=0.62V //
          printf("vdd=0.61V -> 386(mV)\n"); // vdd=0.61V //
          printf("vdd=0.60V -> 380(mV)\n"); // vdd=0.60V //
          printf("Enter vdd setting? ex) 500(mV) -> ");DigitInput(&tempo7);

          printf("*** Test Temp   [-40:40 / 25C:25  / 95C:95 ] ->"); DigitInput(&cond2);
          if      (cond2 == 40) strcpy(test_name,"_-40C");
          else if (cond2 == 25) strcpy(test_name,"_25C");
          else if (cond2 == 95) strcpy(test_name,"_95C");
          else                  strcpy(test_name,"_000");

    etlr25 = GetValue(IN_ETLR25,param);
    etlr32 = GetValue(IN_ETLR32,param);// T5T3
    etlr33 = GetValue(IN_ETLR33,param);// T4
    etlr34 = GetValue(IN_ETLR34,param);// T6

    etlr26 = GetValue(IN_ETLR26,param);// T1
    etlr27 = GetValue(IN_ETLR27,param);// T1
    etlr28 = GetValue(IN_ETLR28,param);// T2
    etlr29 = GetValue(IN_ETLR29,param);// T2
    etlr30 = GetValue(IN_ETLR30,param);// T2

      printf("1:vdd\n");
      printf("2:vddh\n");
      printf("3:ETLR33 T4 saout\n");
      printf("4:ETLR34 T6 (T4-T5)\n");
      printf("5:ETLR32<7:4> T5 saenb2\n");
      printf("6:ETLR32<3:0> T3 saenb\n");
      printf("7:ETLR26,27<7:0> T1 Precharge\n");
      printf("Enter loop_number Min?  -> ");DigitInput(&loop_nummin);
      printf("Enter loop_number Max?  -> ");DigitInput(&loop_nummax);
      for(loop_num=loop_nummin; loop_num<=loop_nummax; loop_num++){
        switch(loop_num){
	  case 1:  // VDD
//	    loop_max   = 790;
	    loop_max   = 1130;
	    loop_min   = 550;
	    loop_delta = 20;
	    loop_deltac = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"VDD_mV"); 
	  break; 
          case 2:  // VDDH
	    loop_max   = 1430;
	    loop_min   = 1070;
	    loop_delta = 20;
	    loop_deltac = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"VDDH_mV");
	  break;
          case 3:  // ETLR33[7:0] T4 saout
	    loop_max   = 16;
	    loop_min   = 1;
	    loop_deltac = 1;
	    loop_delta = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"ETLR33_70");
	  break;
          case 4:  // ETLR34[7:0] T6 (T4-T5)
	    loop_max   = 16;
	    loop_min   = 1;
	    loop_deltac = 1;
	    loop_delta = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"ETLR34_70");
	  break;
          case 5:  // ETLR32[7:4] T5 saenb2
	    loop_max   = 16;
	    loop_min   = 1;
	    loop_deltac = 1;
	    loop_delta = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"ETLR32_74");
	  break;
          case 6:  // ETLR32[3:0] T3 saenb
	    loop_max   = 16;
	    loop_min   = 1;
	    loop_deltac = 1;
	    loop_delta = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"ETLR32_30");
	  break;
          case 7:  // ETLR26[7:0],ETLR27[3:0] T1 Precharge
	    loop_max   = 16;
	    loop_min   = 1;
	    loop_deltac = 1;
	    loop_delta = 1;
	    loop_cycmax   = 16;
	    loop_cycmin   = 1;
	    sprintf(mesure_name,"ETLR26_70");
	  break;
        }
        //Initial Setting

              vcc[1]=tempo6; //vddh setting//
              vcc[2]=tempo7; //vdd setting//
      	
	// FileName Setting
        sprintf(filename,"SHOMM%s_%s%s_%c%c%c%c%c%c_w%3d_X%3dY%3d_s%3d",name_parts1,mesure_name,test_name,info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[11],info[12],info[20]);
        if(PASS != OpenDataOutFile(filename)) delay_timer(2*1000*1000);
        fprintf(Fptdata," module,%s \n patten,%s \n vcc,%d,vddh,%d,vdd,%d,CLK-Mode,%s\n",module,wgl_name,vcc[0],vcc[1],vcc[2],clk_mode);

        fprintf(Fptdata,"*** etlr25<7:0>=0x%X (iref)\n",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"*** etlr26<7:0>=0x%X (PrechargeTime FLI1/FLP)\n",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"*** etlr27<7:0>=0x%X (PrechargeTime FLI2)\n",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"*** etlr28<7:0>=0x%X (Discharge FLP0)\n",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"*** etlr29<7:0>=0x%X (Discharge FLI1)\n",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"*** etlr30<7:0>=0x%X (Discharge FLI2)\n",GetValue(IN_ETLR30,param));

        fprintf(Fptdata,"*** etlr32<7:0>=0x%X (T5 2nd-SA<7:4>/T3 saenb<3:0>)\n",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"*** etlr33<7:0>=0x%X (T4 saout FLI/FLP)\n",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"*** etlr34<7:0>=0x%X (T6 2nd-SA enable control time)\n",GetValue(IN_ETLR34,param));
        fprintf(Fptdata,"*** Extal=%dns \n",GetValue(IN_EXTAL1,param));


	      if ( loop_num == 1 ) { // VDD fix Eval //
	    fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",mesure_name);
              }
	      else if ( loop_num == 2 ) {   // VDDH fix Eval //
	    fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",mesure_name);
              }
	      else if ( loop_num != 1 && loop_num != 2 ) {   // VDDH fix Eval //
	    fprintf(Fptdata,"\n%s,1,10,20,25,30,31,32,33,34,36,40,42,43,45,48,50\n",mesure_name);
          }
          
        CloseDataOutFile();delay_timer(200);

        //Delay Resister Setting //
          printf("\n*** T4 saout setting!! ETLR33 Select ?  ex) 0xEF input ->"); tempo11=0x22;//DigitInput(&tempo11);
          SetValue(IN_ETLR33,param,tempo11);  /* 2ns Delay */
          printf("\n*** T6 (T4-T5) setting!! ETLR34 Select ?  ex) 0xEF input ->"); tempo12=0x22;//DigitInput(&tempo12);
          SetValue(IN_ETLR34,param,tempo12);  /* 2ns Delay 2.80ns Trimming Read*/
          printf("\n*** T5 saenb2/ T3 saenb setting!! ETLR32<7:0> Select ?  ex) 0xEF input ->"); tempo13=0x5E;//DigitInput(&tempo13);
          SetValue(IN_ETLR32,param,tempo13);  /* 2ns Delay 2.80ns Trimming Read*/
//         printf("*** T3 saenb setting!! ETLR32<3:0> Select ?  ex) 0xFC input ->"); DigitInput(&tempo10);
//          SetValue(IN_ETLR26,param,0x77),SetValue(IN_ETLR27,param,0x77 & 0xF0);  /* 8ns Delay Setting Read*/
          printf("\n*** T1 Precharge setting!! ETLR26<7:0>,ETLR27<3:0> Select ?  ex) 0x77 input ->"); tempo16=0xEE;//DigitInput(&tempo16);
          SetValue(IN_ETLR26,param,tempo16),SetValue(IN_ETLR27,param,tempo16 & 0x0F);  /* 2ns Delay 2.80ns Trimming Read*/

          printf("\n*** and Discharge TapLoop enale?  Enable:2 / Disable:1 ? ->"); DigitInput(&taploopnum);
      
      DeviceSpecificPowerUp();delay_timer(200);
      MinoriModeFunc_Repeat_F(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");

      for(loop=loop_max;loop>=loop_min;loop=loop-loop_delta){ //Loop
           OpenDataOutFile(filename);
	   switch(loop_num){
	     case 1: //Vdd Setting
               vcc[2]=loop*100/158;
	       fprintf(Fptdata,"%d",loop); 
	     break; 
         case 2: //Vddh Setting
               vcc[1]=loop*10/25;
	       fprintf(Fptdata,"%d",loop); 
	     break;
         case 3: //ETLR33 Setting T4
////	       SetValue(IN_ETLR33,param,0xFF); // T4 //
	       SetValue(IN_ETLR34,param,0xFF); // T6 //
	       SetValue(IN_ETLR32,param,0xFE); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break; //Def
	         case 5:tempo1 = 0x44 ;break;
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR33;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR33,param)); 
	     break;
         case 4: //ETLR34 Setting T6(T4-T5)
//	       SetValue(IN_ETLR33,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
	       SetValue(IN_ETLR32,param,0xFE); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x11 ;break;
	         case 3:tempo1 = 0x22 ;break;
	         case 4:tempo1 = 0x33 ;break;
	         case 5:tempo1 = 0x44 ;break; //Def
	         case 6:tempo1 = 0x55 ;break;
	         case 7:tempo1 = 0x66 ;break;
	         case 8:tempo1 = 0x77 ;break; 
	         case 9:tempo1 = 0x88 ;break; 
	         case 10:tempo1 = 0x99 ;break; 
	         case 11:tempo1 = 0xAA ;break; 
	         case 12:tempo1 = 0xBB ;break;
	         case 13:tempo1 = 0xCC ;break;
	         case 14:tempo1 = 0xDD ;break;
	         case 15:tempo1 = 0xEE ;break;
	         case 16:tempo1 = 0xFF ;break;
	       }
	       tempo2 = IN_ETLR34;
	       SetValue(tempo2,param, tempo1);
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR34,param)); 
	     break;
         case 5: //ETLR32 Setting T5
//	       SetValue(IN_ETLR33,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x10 ;break;
	         case 3:tempo1 = 0x20 ;break;
	         case 4:tempo1 = 0x30 ;break; //Def
	         case 5:tempo1 = 0x40 ;break;
	         case 6:tempo1 = 0x50 ;break;
	         case 7:tempo1 = 0x60 ;break;
	         case 8:tempo1 = 0x70 ;break; 
	         case 9:tempo1 = 0x80 ;break; 
	         case 10:tempo1 = 0x90 ;break; 
	         case 11:tempo1 = 0xA0 ;break; 
	         case 12:tempo1 = 0xB0 ;break;
	         case 13:tempo1 = 0xC0 ;break;
	         case 14:tempo1 = 0xD0 ;break;
	         case 15:tempo1 = 0xE0 ;break;
	         case 16:tempo1 = 0xF0 ;break;
	       }
	       tempo2 = IN_ETLR32;
	       SetValue(tempo2,param, ((GetValue(tempo2,param) & 0x0F) | tempo1) );
	       tempo3 = GetValue(tempo2,param);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR32,param)); 
	     break;
         case 6: //ETLR26[7:0],ETLR27[3:0] T3 saenb
////	       SetValue(IN_ETLR33,param,0xFF); // T4 //
////	       SetValue(IN_ETLR34,param,0xFF); // T6 //
////	       SetValue(IN_ETLR32,param,0xF3); // T5 //
//	       SetValue(IN_ETLR26,param,0x77); // T1 Precharge Max//
//	       SetValue(IN_ETLR27,param,0xF7); // T1 Precharge Max//
//          SetValue(IN_ETLR26,param,tempo16),SetValue(IN_ETLR27,param,tempo16 & 0xF0);  /* 2ns Delay 2.80ns Trimming Read*/
//          SetValue(IN_ETLR26,param,0x77),SetValue(IN_ETLR27,param,0x77 & 0xF0);  /* 8ns Delay Setting Read*/
	       SetValue(IN_ETLR28,param,0xF5); // T2 Discharge target //
	       SetValue(IN_ETLR29,param,0xF5); // T2 Discharge target //
	       SetValue(IN_ETLR30,param,0xF5); // T2 Discharge target //
	       switch(loop){
	         case 1:tempo1 = 0x00 ;break;
	         case 2:tempo1 = 0x01 ;break;
	         case 3:tempo1 = 0x02 ;break;
	         case 4:tempo1 = 0x03 ;break; //Def
	         case 5:tempo1 = 0x04 ;break;
	         case 6:tempo1 = 0x05 ;break;
	         case 7:tempo1 = 0x06 ;break;
	         case 8:tempo1 = 0x07 ;break; 
	         case 9:tempo1 = 0x08 ;break; 
	         case 10:tempo1 = 0x09 ;break; 
	         case 11:tempo1 = 0x0A ;break; 
	         case 12:tempo1 = 0x0B ;break;
	         case 13:tempo1 = 0x0C ;break;
	         case 14:tempo1 = 0x0D ;break;
	         case 15:tempo1 = 0x0E ;break;
	         case 16:tempo1 = 0x0F ;break;
	       }
	       tempo2 = IN_ETLR32;
	       SetValue(tempo2,param, ((GetValue(tempo2,param) & 0xF0) | tempo1) );
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR32,param)); 
	     break;
         case 7: //ETLR26[7:0],ETLR27[3:0] T3 saenbPrecharge
//	       SetValue(IN_ETLR33,param,0xFF); // T4 //
//	       SetValue(IN_ETLR34,param,0xFF); // T6 //
//	       SetValue(IN_ETLR32,param,0xF3); // T5 //
	       SetValue(IN_ETLR32,param,((GetValue(IN_ETLR32,param) & 0xF0) | 0x0F)); // T3 ETLR32<3:0> saout Max //
//	       SetValue(IN_ETLR32,param,0xF3); // T1 Precharge //
	       switch(loop){
	         case 1:tempo1 = 0x88, tempo15 = 0xF8 ;break;
	         case 2:tempo1 = 0x99, tempo15 = 0xF9 ;break;
	         case 3:tempo1 = 0xAA, tempo15 = 0xFA ;break;
	         case 4:tempo1 = 0xBB, tempo15 = 0xFB ;break; //Def
	         case 5:tempo1 = 0xCC, tempo15 = 0xFC ;break;
	         case 6:tempo1 = 0xDD, tempo15 = 0xFD ;break;
	         case 7:tempo1 = 0xEE, tempo15 = 0xFE ;break;
	         case 8:tempo1 = 0xFF, tempo15 = 0xFF ;break; 
	         case 9:tempo1 = 0x00, tempo15 = 0xF0 ;break; 
	         case 10:tempo1 = 0x11, tempo15 = 0xF1 ;break; 
	         case 11:tempo1 = 0x22, tempo15 = 0xF2 ;break; 
	         case 12:tempo1 = 0x33, tempo15 = 0xF3 ;break;
	         case 13:tempo1 = 0x44, tempo15 = 0xF4 ;break;
	         case 14:tempo1 = 0x55, tempo15 = 0xF5 ;break;
	         case 15:tempo1 = 0x66, tempo15 = 0xF6 ;break;
	         case 16:tempo1 = 0x77, tempo15 = 0xF7 ;break;
	       }
	       tempo2 = IN_ETLR26;
	       tempo14 = IN_ETLR27;
	       SetValue(tempo2,param, tempo1);
	       SetValue(tempo14,param,tempo15);
	       fprintf(Fptdata,"0x%X ",GetValue(IN_ETLR26,param)); 
	     break;
	   }//loop_num//

/*
			 printf(" === EXTAL1 parameter === \n");
			 printf(" 20ns : 50.00MHz\n ");
			 printf(" 21ns : 47.62MHz\n ");
			 printf(" 22ns : 45.45MHz\n ");
			 printf(" 23ns : 43.48MHz\n ");
			 printf(" 24ns : 41.67MHz\n ");
			 printf(" 25ns : 40.00MHz\n ");
			 printf(" 28ns : 35.71MHz\n ");
			 printf(" 29ns : 34.48MHz\n ");
			 printf(" 30ns : 33.33MHz\n ");
			 printf(" 31ns : 32.26MHz\n ");
			 printf(" 32ns : 31.25MHz\n ");
			 printf(" 33ns : 30.30MHz\n ");
			 printf(" 40ns : 25.00MHz\n ");
			 printf(" 50ns : 20.00MHz\n ");
			 printf(" 100ns: 10.00MHz\n ");
			 printf(" Select EXTAL1 T=%3dns /(ex: 20ns)  -> ",GetValue(IN_EXTAL1,param));
*/
       if(taploopnum == 1){
	       for(loop_cyc=loop_cycmax; loop_cyc>=loop_cycmin; loop_cyc=loop_cyc-loop_deltac){
            switch(loop_cyc){
	         case 1:tempo1 = 20 ;break;
	         case 2:tempo1 = 21 ;break;
	         case 3:tempo1 = 22 ;break;
	         case 4:tempo1 = 23 ;break;
	         case 5:tempo1 = 24 ;break;
	         case 6:tempo1 = 25 ;break;
	         case 7:tempo1 = 28 ;break;
	         case 8:tempo1 = 29 ;break; 
	         case 9:tempo1 = 30 ;break; 
	         case 10:tempo1 = 31 ;break; 
	         case 11:tempo1 = 32 ;break; 
	         case 12:tempo1 = 33 ;break;
	         case 13:tempo1 = 40 ;break;
	         case 14:tempo1 = 50 ;break;
	         case 15:tempo1 = 100 ;break;
	         case 16:tempo1 = 1000 ;break;
   	       }//switch//
	       SetValue(IN_EXTAL1,param, tempo1);
	       tempo3 = GetValue(IN_EXTAL1,param);

			  printf("looptmp=%2d ,Vdd=%4dmV, Vddh=%4dmV, extal=%3dns : ",loop,vcc[2],vcc[1],GetValue(IN_EXTAL1,param));
               DeviceSpecificPowerUp();delay_timer(200);

               waittime = 15;
			   Ffpt = 0;
               result = MinoriModeFunc_Repeat(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      		   delay_timer(10*1000);
               if(result == PASS){
                 printf("***PASS***\n");
                 fprintf(Fptdata,",P");
	       }else if(result == FAIL){
	         printf("---FAIL---\n");
	         fprintf(Fptdata,",F");
	       }else{
	         printf("-- ERROR!!! --\n");
	         fprintf(Fptdata,",E");
		   }
	      }//loop_cyc//
       }//if(taploopnum

        else if (taploopnum == 2){
         for ( i=1 ; i<17 ; i++ ) {
         	       switch(i){
	         case 1:tempo1 = 0xF0;break;//min
	         case 2:tempo1 = 0xF1;break;
	         case 3:tempo1 = 0xF2;break;
	         case 4:tempo1 = 0xF3;break;
	         case 5:tempo1 = 0xF4;break;
	         case 6:tempo1 = 0xF5;break;
	         case 7:tempo1 = 0xF6;break;//Def
	         case 8:tempo1 = 0xF7;break;
	         case 9:tempo1 = 0xF8;break;
	         case 10:tempo1 = 0xF9;break;
	         case 11:tempo1 = 0xFA;break;
	         case 12:tempo1 = 0xFB;break;
	         case 13:tempo1 = 0xFC;break;
	         case 14:tempo1 = 0xFD;break;
	         case 15:tempo1 = 0xFE;break;
	         case 16:tempo1 = 0xFF;break;
//	         case 17:tempo1 = 0x00;break;
//	         case 18:tempo1 = 0x01;break;
//	         case 19:tempo1 = 0x02;break;
//	         case 20:tempo1 = 0x03;break;
//	         case 21:tempo1 = 0x04;break;
//	         case 22:tempo1 = 0x05;break;
//	         case 23:tempo1 = 0x06;break;
//	         case 24:tempo1 = 0x07;break;
//	         case 25:tempo1 = 0x08;break;
//	         case 26:tempo1 = 0x09;break;
//	         case 27:tempo1 = 0x0A;break;
//	         case 28:tempo1 = 0x0B;break;
//	         case 29:tempo1 = 0x0C;break;
//	         case 30:tempo1 = 0x0D;break;
//	         case 31:tempo1 = 0x0E;break;
//	         case 32:tempo1 = 0x0F;break;//max
	       }
	       SetValue(IN_ETLR28,param, tempo1);
	       SetValue(IN_ETLR29,param, tempo1);
	       SetValue(IN_ETLR30,param, tempo1);

	       for(loop_cyc=loop_cycmax; loop_cyc>=loop_cycmin; loop_cyc=loop_cyc-loop_deltac){ //if(taploopnum == 2)
            switch(loop_cyc){ 
	         case 1:tempo1 = 20 ;break;
	         case 2:tempo1 = 21 ;break;
	         case 3:tempo1 = 22 ;break;
	         case 4:tempo1 = 23 ;break;
	         case 5:tempo1 = 24 ;break;
	         case 6:tempo1 = 25 ;break;
	         case 7:tempo1 = 28 ;break;
	         case 8:tempo1 = 29 ;break; 
	         case 9:tempo1 = 30 ;break; 
	         case 10:tempo1 = 31 ;break; 
	         case 11:tempo1 = 32 ;break; 
	         case 12:tempo1 = 33 ;break;
	         case 13:tempo1 = 40 ;break;
	         case 14:tempo1 = 50 ;break;
	         case 15:tempo1 = 100 ;break;
	         case 16:tempo1 = 1000 ;break;
   	       }//switch//
	       SetValue(IN_EXTAL1,param, tempo1);
	       tempo3 = GetValue(IN_EXTAL1,param);

			  printf("looptmp=%2d ,Vdd=%4dmV, Vddh=%4dmV, extal=%3dns, ETLR28=%2X, ETLR26=%2X : ",loop,vcc[2],vcc[1],GetValue(IN_EXTAL1,param),GetValue(IN_ETLR28,param),GetValue(IN_ETLR26,param));
               DeviceSpecificPowerUp();delay_timer(200);

               waittime = 15;
			   Ffpt = 0;
               result = MinoriModeFunc_Repeat(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      		   delay_timer(10*1000);
               if(result == PASS){
                 printf("***PASS***\n");
                 fprintf(Fptdata,",P");
	       }else if(result == FAIL){
	         printf("---FAIL---\n");
	         fprintf(Fptdata,",F");
	       }else{
	         printf("-- ERROR!!! --\n");
	         fprintf(Fptdata,",E");
		   }
	      }//loop_cyc// //if(taploopnum == 2)
	       fprintf(Fptdata,",lr28=0x%X",GetValue(IN_ETLR28,param)); 
       }//for ( i=0 ; i<32
      }//if(taploopnum


        fprintf(Fptdata,",etlr25=0x%X ",GetValue(IN_ETLR25,param));
        fprintf(Fptdata,"lr26=0x%X ",GetValue(IN_ETLR26,param));
        fprintf(Fptdata,"lr27=0x%X ",GetValue(IN_ETLR27,param));
        fprintf(Fptdata,"lr28=0x%X ",GetValue(IN_ETLR28,param));
        fprintf(Fptdata,"lr29=0x%X ",GetValue(IN_ETLR29,param));
        fprintf(Fptdata,"lr30=0x%X ",GetValue(IN_ETLR30,param));
        fprintf(Fptdata,"lr32=0x%X ",GetValue(IN_ETLR32,param));
        fprintf(Fptdata,"lr33=0x%X ",GetValue(IN_ETLR33,param));
        fprintf(Fptdata,"lr34=0x%X ",GetValue(IN_ETLR34,param));
	   fprintf(Fptdata,"\n");
	   CloseDataOutFile();delay_timer(200);

         }//vdd or reg Loop
        //Resister Reset//
        SetValue(IN_EXTAL1,param,freqpara); 
        SetValue(IN_ETLR25,param,etlr25);
        SetValue(IN_ETLR26,param,etlr26);
        SetValue(IN_ETLR27,param,etlr27);
        SetValue(IN_ETLR28,param,etlr28);
        SetValue(IN_ETLR29,param,etlr29);
        SetValue(IN_ETLR30,param,etlr30);
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
      }//loop_num Loop
	vcc[1]=vcc[2]=500;

       	SetValue(IN_EXTAL1,param,freqpara); 
        SetValue(IN_ETLR25,param,etlr25);
        SetValue(IN_ETLR26,param,etlr26);
        SetValue(IN_ETLR27,param,etlr27);
        SetValue(IN_ETLR28,param,etlr28);
        SetValue(IN_ETLR29,param,etlr29);
        SetValue(IN_ETLR30,param,etlr30);
        SetValue(IN_ETLR32,param,etlr32);
        SetValue(IN_ETLR33,param,etlr33);
        SetValue(IN_ETLR34,param,etlr34);
	  Ffpt = ffpt_bak;
    break;

    case 215:
      SetValue(IN_LDATA0,param,0x3F7FE);
      SetValue(IN_LDATA1,param,0x405E3);
      SetValue(IN_LDATA2,param,0x40E50);
      SetValue(IN_LDATA3,param,0x41000);
      SetValue(IN_BDATA10,param,0);
      Minori_LPRead(MNORMAL,&vcc[0],waittime,param,"FLP0_CHKA_DSCAN1INCX_DS_LPREAD_WS2_BASE","MINORI LP-READ");
    break;
    case 815:
	    Shmoo_LPRead(1,param);
    break;
    case 816:
	    Shmoo_LPRead_vs1MOSC(1,param,&vcc[0]);
    break;
    case 700:
      SetValue(IN_TESTSEL,param,1);SetValue(IN_BDATA00,param,1);SetValue(IN_BDATA11,param,1);
	    waittime = 16*10;
      printf("Select temprature -> ");DigitInput(&tempo1);
      MINORI_Shmoo_Loop(0,&vcc[0],waittime,param,tempo1,"USER_DG_M1_MULTISCANI_10LOOP_XS");
    break;
    case 315:
      printf("Enter CHKA(0) / CHKB(1)");DigitInput(&tempo1);
	  SetValue(IN_BDATA09,param,3);//ExtCLK Mode
      if(tempo1 == 0){
        sprintf(name_parts1,"_CHKA");SetValue(IN_TESTSEL,param,4);
	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	    sprintf(name_parts3,"_MULTISCANI");
	    sprintf(name_parts4,"_YS");
      }else if(tempo1 == 1){
        sprintf(name_parts1,"_CHKB");SetValue(IN_TESTSEL,param,5);
	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	    sprintf(name_parts3,"_MULTISCANI");
	    sprintf(name_parts4,"_YS");
      }
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
	  printf("Enter MaxVoltage Vdd[mV]");DigitInput(&Voltage_Max);
	  printf("Enter MinVoltage Vdd[mV]");DigitInput(&Voltage_Min);
	  printf("Enter DeltaVoltage Vdd[mV]");DigitInput(&Voltage_Delta);
	  i=1;
	  for(tempo1 = Voltage_Max; tempo1>=Voltage_Min; tempo1 = tempo1 - Voltage_Delta){
	    dataout[0][i] = tempo1;
		i++;
      }
	  loop_max = i-1;
	  printf("Enter Freq loop num");DigitInput(&Freq_num);
	  for(j=1;j<=Freq_num;j++){
	    printf("Enter Freq[MHz]");DigitInput(&tempo1);
		dataout[j][0] = tempo1;
		for(i=1;i<=loop_max;i++){
		  vcc[2] = (dataout[0][i]*100)/158;
		  Ffpt = 0;
		  DeviceSpecificPowerUp();
          dataout[j][i] = MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
		  printf(" Freq = %03d MHz : Vdd = %04d mV : Result = %04X\n",dataout[j][0],dataout[0][i],dataout[j][i]);
		  Ffpt = 1;
		}
  	  }
      sprintf(filename,"SHOMM%s%s_ExCLK_%s",name_parts1,name_parts4,clk_mode);
      if(PASS != OpenDataOutFile(filename)) delay_timer(2*1000*1000);
      fprintf(Fptdata," module,%s \n patten,%s \n vcc,%d,vddh,%d,vdd,%d,CLK-Mode,%s\n",module,wgl_name,vcc[0],vcc[1],vcc[2],clk_mode);
	  for(i=0;i<=loop_max;i++){
		for(j=0;j<=Freq_num;j++){
		  if((i==0) && (j==0) ){
		  	fprintf(Fptdata,",");
		  }else if(i==0){
		  	fprintf(Fptdata,"%d,",dataout[j][i]);
		  }else if(j==0){
		  	fprintf(Fptdata,"%d,",dataout[j][i]);
		  }else{
		  	if(dataout[j][i] == PASS){
			  fprintf(Fptdata,"P,");
			}else if(dataout[j][i] == FAIL){
			  fprintf(Fptdata,"F,");
			}else{
			  fprintf(Fptdata,"E,");
			}
		  }
		}
	    fprintf(Fptdata,"\n");
  	  }
	  CloseDataOutFile();delay_timer(200);
  	break;
    case 9999: //For-Debug
      printf("Enter ALL0(0) / ALL1(1)");DigitInput(&tempo1);
      if(tempo1 == 0){
        sprintf(name_parts1,"_ALL0");SetValue(IN_TESTSEL,param,0);
      }else if(tempo1 == 1){
        sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
      }
	    sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
      printf("Enter ReadON(0) / ReadOff(1)");DigitInput(&tempo1);
      if(tempo1 == 0){
	      sprintf(name_parts3,"_MULTISCAN");
      }else if(tempo1 == 1){
	      sprintf(name_parts3,"_MULTINOREAD");
      }
	    sprintf(name_parts4,"_NS");
      
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);
      
      MinoriModeFunc(MNORMAL,&vcc[0],waittime,param,wgl_name,"MINORI READ");
      break;
    break;





    /**************************************************************************/
    /*  Read-BI                                                              */
    /**************************************************************************/
	case 987:
	  vcc_bak[1] = vcc[1];
	  vcc_bak[2] = vcc[2];
	  vcc[1] = vcc[2] = 0;

      printf("ALL0 Read (Not Compare)\n");
      sprintf(name_parts1,"_ALL1");SetValue(IN_TESTSEL,param,7);
	  sprintf(name_parts2,"_M0");SetValue(IN_BDATA00,param,0);
	  sprintf(name_parts3,"_MULTISCANI");
	  sprintf(name_parts4,"_XS");
      sprintf(wgl_name,"%s%s%s%s%s",name_parts0,name_parts1,name_parts2,name_parts3,name_parts4);

      //Select Read-BI Mode
	  stack1 = SetValue(IN_BDATA09,param,1);

	  // Select MULTI-Read_No-Scan
      stack2 = SetValue(IN_BDATA08,param,3);

	  //Select ALL User Module
      stack3 = SetValue(IN_ETCR00,param,( GetValue(IN_ETCR00,param) | 0x10 ));

	  //Select sg-Y even mode
      stack4 = SetValue(IN_ETCR08,param,( GetValue(IN_ETCR08,param) & 0x67 | 0x90 ));
      stack5 = SetValue(IN_ETCR07,param,( GetValue(IN_ETCR07,param) & 0xE3 | 0x14 ));

	  MinoriModeFunc(MNORMAL,&vcc[0],1,param,wgl_name,"MINORI READ");

	  //Select sg-Y odd mode
      SetValue(IN_ETCR08,param,( GetValue(IN_ETCR08,param) & 0x67 | 0x88 ));
      SetValue(IN_ETCR07,param,( GetValue(IN_ETCR07,param) & 0xE3 | 0x0C ));

	  MinoriModeFunc(MNORMAL,&vcc[0],1,param,wgl_name,"MINORI READ");


      //Return
	  SetValue(IN_BDATA09,param,stack1);
	  SetValue(IN_BDATA08,param,stack2);
	  SetValue(IN_ETCR00,param,stack3);
	  SetValue(IN_ETCR08,param,stack4);
	  SetValue(IN_ETCR07,param,stack5);
	  vcc[1] = 	vcc_bak[1];
	  vcc[2] = 	vcc_bak[2];



    break;


    }
    
  }
}  
    /**************************************************************************/
    
int MinoriRead_Kani(int mode, int vs[NUM_POWERSUPPLY], int waittime, interface_t* param, int *current)
{
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8,stack9,stack10,stack11,stack12;/*debug add*/
  int current,result,selvol,start_time;
  char wgl_name[200];



  stack0 = GetValue(IN_TESTSEL,param);
  stack1 = GetValue(IN_BDATA00,param); // �X�N�����u�� M0 or M1
  stack2 = GetValue(IN_BDATA01,param); // LowPowerRead - Off
  stack3 = GetValue(IN_BDATA02,param); // Back-Bias Off
  stack4 = GetValue(IN_BDATA03,param); // CLK Setting 1:�t�� 0:����
  stack5 = GetValue(IN_BDATA07,param); // Loop Times
  stack6 = GetValue(IN_BDATA08,param); // Micro-Code
  stack7 = GetValue(IN_BDATA09,param); // MINORI-Current Mode

	SetValue(IN_BDATA07,param,0xFFFFF); // Loop Times
  SetValue(IN_BDATA09,param,2); // MINORI-Current Mode

  switch(mode & 0xFFFFFFF0){
    case WHILE1_ALL0:
        SetValue(IN_TESTSEL,param,0);
        SetValue(IN_BDATA00,param,0); // �X�N�����u�� M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,1); // CLK Setting 1:�t�� 0:����
        
	      if(mode & READ_ON){
	        sprintf(wgl_name,"USER_ALL0_M0_MULTISCAN_NS");
        }else if(mode & READ_OFF){
	        sprintf(wgl_name,"USER_ALL0_M0_MULTINOREAD_NS");
        }
    break; 
    case WHILE1_ALL1:
        SetValue(IN_TESTSEL,param,7);
        SetValue(IN_BDATA00,param,0); // �X�N�����u�� M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,1); // CLK Setting 1:�t�� 0:����
        
	      if(mode & READ_ON){
	        sprintf(wgl_name,"USER_ALL1_M0_MULTISCAN_NS");
        }else if(mode & READ_OFF){
	        sprintf(wgl_name,"USER_ALL1_M0_MULTINOREAD_NS");
        }
    break; 

    case NORMAL_READ:
        SetValue(IN_TESTSEL,param,4); // CHKA-�p�^��
        SetValue(IN_BDATA00,param,0); // �X�N�����u�� M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,1); // CLK Setting 1:�t�� 0:����
        
	      if(mode & READ_ON){
	        sprintf(wgl_name,"USER_CHKA_M0_MULTISCANI_MASK_YS");
        }else if(mode & READ_OFF){
	        sprintf(wgl_name,"USER_CHKA_M0_MULTISCANINOREAD_YS");
        }

    break; 
    case RANDOM_READ:
        SetValue(IN_TESTSEL,param,4);
        SetValue(IN_BDATA00,param,0); // �X�N�����u�� M0 or M1
        SetValue(IN_BDATA01,param,0); // LowPowerRead - Off
        SetValue(IN_BDATA02,param,0); // Back-Bias Off
        SetValue(IN_BDATA03,param,1); // CLK Setting 1:�t�� 0:����
        
	      if(mode & READ_ON){
	        sprintf(wgl_name,"USER_ALL0_M0_RANDOMREAD_NS");
        }else if(mode & READ_OFF){
	        sprintf(wgl_name,"USER_ALL0_M0_RANDOMNOREAD_NS");
        }
    break; 
    default:printf("\nMinoriRead_Kani Func Error!!! C-Program\n\n");
  }
  
  start_time = bentime(); /* Get Test Start Time */
  
  result = MinoriModeFunc(MNORMAL,&vs[0],waittime,param,wgl_name,"MINORI READ");

  printf("MinoriModeFunc Time = %d\n",(bentime()-start_time-init_bentime())/1000);
  
  SetValue(IN_TESTSEL,param,stack0);
  SetValue(IN_BDATA00,param,stack1);
  SetValue(IN_BDATA01,param,stack2);
  SetValue(IN_BDATA02,param,stack3);
  SetValue(IN_BDATA03,param,stack4);
  SetValue(IN_BDATA07,param,stack5);
  SetValue(IN_BDATA08,param,stack6);
  SetValue(IN_BDATA09,param,stack7);

  if( (result == CPUFAIL) || (result == FAIL) ) return(FAIL);

  return(selvol);
}