int FCUMenu(int vs[NUM_POWERSUPPLY],int waittime,interface_t* param){
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6;
  int i,k,j,monitor_pin,vs_bak[NUM_POWERSUPPLY];
  int test_cond,testflag,ffpt_bak;
  int info[NUM_CHIPINFO];                    /* wmat chip information              */
  int result,target,Vresult;
  int loop,loop_max,loop_min,loop_delta;
  int first_flag,repeat_mode,inif_flag;
  char buff[200];
  char item_name[200];
  char item_name2[200];
  char file_name[200];
  char testname[20];
  interface_t* param_ini;
	int module_sel,adr,module_sel2,adr2;
	int vth_mode,min_vol,max_vol,freq_bak,bit_count,delta_vol,search_mode;
  waittime = 20;
  tempo = tempo0 = tempo1 = tempo2 = tempo3 = tempo4 = tempo5 = tempo6 = 0;
  stack0 = stack1 = stack2 = stack3 = stack4 = stack5 = stack6 = 0;
  test_cond = testflag = 0;
  module_sel = module_sel2 = adr = adr2 = 0;
	vs_bak[0] = 3300;
	vs_bak[1] = vs_bak[2] = 500;

  while( test_cond != EXIT){
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| FCU Time | PETime  : ALL(101)\n");
    printf("|          | PEStTime: Write(111) / Erase(112) / PreWrite(113) ALL(114)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| Suspend  | FCU  : PE>Sus>Res(201) / E>Sus>P>Res(202)\n");
    printf("| & Resume | FACI : Sus-Timer-1st(203) / Sus-Timer-2nd(204) / Sus-Timer-ALL(205)\n");
    printf("|          | FACI : Renzoku-Suspend(206)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|   BGO    | FCU  : Wave_check(301)\n");
    printf("|          | FACI : Read_Check(302)\n");
    printf("| PE - BGO | Write(303) / Erase(304) \n");
    printf("| PE-Abort | FACI : Abort Time(401) \n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("|  Other   | Vnoemi-Vrsg Leak Check(501)\n");
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    TestConditionDisp(0,param,waittime,&vs[0]);
    printf("+----------+------------------------------------------------------------------------------------------+\n");
    printf("| exit(e) / quit(q) / system(sy) / save_Reg.(sreg) / load_Reg.(lreg) / logcap:%1d(log)\n",Ffpt);
    printf("+-----------------------------------------------------------------------------------------------------+\n");
    printf("Please enter Test Condition -> ");
    DigitInput(&test_cond);
    DeviceSpecificPowerUp();
    AnalyzeCommonCommand(test_cond,param,&waittime,&vs[0]);

    switch(test_cond){
      case EXIT:
          return(PASS);
        break;
      case QUIT:
        DeviceSpecificPowerDown();
        if(Ffpt==2) if(fclose(Fptdata)) exit(-99);
        PowerDown(1);
        break;
	  case 101:
	    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("FACI_Timer"),"PE Time");
	    tempo1 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA0,param) / 1000; //time[us] = T[ns]*COUNT/1000
	    tempo2 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA1,param) / 1000; //time[us] = T[ns]*COUNT/1000
	    tempo3 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA2,param) / 1000; //time[us] = T[ns]*COUNT/1000
	    printf("FACI command Write Time(256B) = %d[us]\n",tempo1);
	    printf("FACI command Write Time(  8B) = %d[us]\n",tempo2);
	    printf("FACI command Erase Time       = %d[us]\n",tempo3);
	  
	  break;
	  
	  case 111:
	  case 112:
	  case 113:	  
	    if(test_cond == 111){
	      SetValue(IN_TESTSEL,param,0);
	      SetValue(IN_BDATA00,param,1);//Write Mode
	      SetValue(IN_BDATA01,param,0);//ALL0 pattern
	      SetValue(IN_BDATA02,param,0);
	    
	    }else if(test_cond == 112){
	      SetValue(IN_TESTSEL,param,0);
	      SetValue(IN_BDATA00,param,2);//Erase Mode
	      SetValue(IN_BDATA01,param,0);
	      SetValue(IN_BDATA02,param,0);
	    
	    }else if(test_cond == 113){
	      SetValue(IN_TESTSEL,param,0);
	      SetValue(IN_BDATA00,param,3);//Erase Mode
	      SetValue(IN_BDATA01,param,0);
	      SetValue(IN_BDATA02,param,0);
	    
	    }
	  
	    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("FCU_PEStTime"),"PE Time");
	    tempo1 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA0,param); //time[ns] = T[ns]*COUNT
	    printf("FACI command PE StTime = %d[ns]\n",tempo1);
	  case 114:
	    result = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("FlashSM_7_9_WaitTime"),"PE Time");
	    tempo1 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA0,param); //time[us] = T[ns]*COUNT/1000
	    tempo2 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA1,param); //time[us] = T[ns]*COUNT/1000
	    tempo3 = GetValue(IN_EXTAL1,param) * GetValue(OUT_LDATA2,param); //time[us] = T[ns]*COUNT/1000
	    printf("Test command St Write Time(256B) = %d[ns]\n",tempo1);
	    printf("Test command St Erase Time       = %d[ns]\n",tempo2);
	    printf("Test command St PreWrite Time    = %d[ns]\n",tempo3);
	  
	  break;

	  break;
		case 201:
		  testflag = 0;
			while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| Module       : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("|   PE         : ");
				switch(GetValue(IN_BDATA00,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("|  PE~Sus Wait : %5d [us]  (4)\n",GetValue(IN_LDATA0,param));
				printf("| Sus~Res Wait : %5d [us]  (5)\n",GetValue(IN_LDATA1,param));
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_BDATA00,param,tempo1);
					break;
				case 4:
				case 5:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-4),param,tempo1);
					break;
				default:
					break;
				}
			}
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,0);
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_FCU"),"Firm Debug");

      Firm_debugparam_Out();
		break;
		case 202:
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
		  	printf("|      ----- Erase -----\n");
				printf("| Module       : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "       (1)\n");
				printf("| Address      : 0x%6X   (2)\n",adr);
		  	printf("|      ----- Write -----\n");
				printf("| Module       : ");
				switch(module_sel2){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "       (3)\n");
				printf("| Address      : 0x%6X   (4)\n",adr2);
		  	printf("|      ----- Wait  -----\n");
				printf("|  E ~Sus Wait : %5d [us] (5)\n",GetValue(IN_LDATA0,param));
				printf("| Sus~ P  Wait : %5d [us] (6)\n",GetValue(IN_LDATA1,param));
				printf("|  P ~Res Wait : %5d [us] (7)\n",GetValue(IN_LDATA2,param));
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 3:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel2);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 4:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr2);
					break;
				case 5:
				case 6:
				case 7:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-5),param,tempo1);
					break;
				default:
					break;
				}
			}
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_BDATA01,param,module_sel2);
			SetValue(IN_LDATA5,param,adr2);
			SetValue(IN_TESTSEL,param,1);
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_FCU"),"Firm Debug");
      Firm_debugparam_Out();
		break;
		case 203:
		  SetValue(IN_LDATA0,param,0);
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| Module       : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("|   PE         : ");
				switch(GetValue(IN_BDATA00,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("|  PE~Sus Wait : %5d [us]  (4)\n",GetValue(IN_LDATA0,param));
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_BDATA00,param,tempo1);
					break;
				case 4:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-4),param,tempo1);
					break;
				default:
					break;
				}
			}
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,0);
			result = 0;
			
			//File Name Set
      sprintf(item_name,"%s_W%d_X%dY%d_SuspendTime_1st",lotname,atoi(wafername),xy_location[0],xy_location[1]);
	  switch(GetValue(IN_BDATA00,param)){
	  	case 0 : sprintf(item_name2,"_WRITE");break;
	  	case 1 : sprintf(item_name2,"_ERASE");break;
	  	default: sprintf(item_name2,"_ERROR");break;
	  }
      sprintf( file_name, "%s%s%s%s.csv", glob_cdp, glob_datalog, item_name, item_name2 );
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }

			fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz]\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param));
			fprintf(Fptdata,"PE-Wait[us],Suspend-Wait[us],Pass/Fail,\n");
			fclose(Fptdata);
			Ffpt = 0;
			while(result !=55){
			
			printf(" ***** Wait Time = %d[us] ~ =+99[us] ***** \n",GetValue(IN_LDATA0,param));
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_FACI"),"Firm Debug");
      //Firm_debugparam_Out();
      
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }	
			for(i=0;i<=(GetValue(OUT_BDATA00,param));i++){

				if((GetValue(OUT_BDATA00,param) == i) && (GetValue(OUT_JUDGE1,param) != 0 )){
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
					switch (GetValue(OUT_BDATA01,param))
					{
					case 2:fprintf(Fptdata,"FASTAT_FAIL,");break;
					case 3:fprintf(Fptdata,"FHVEERR_FAIL,");break;
					case 4:fprintf(Fptdata,"PRGERR_FAIL,");break;
					case 5:fprintf(Fptdata,"ERSERR_FAIL,");break;
					case 6:fprintf(Fptdata,"ILGLERR_FAIL,");break;
					case 7:fprintf(Fptdata,"OTERR_FAIL,");break;
					case 8:fprintf(Fptdata,"ILGCOMERR_FAIL,");break;
					case 9:fprintf(Fptdata,"SUSPEND_FAIL,");break;
					default:fprintf(Fptdata,"Firm_ERROR,");break;
					}
					fprintf(Fptdata,"REG_FASTAT = 0x%X,",GetValue(OUT_LDATA1,param));
					fprintf(Fptdata,"REG_FSTATR = 0x%X,",GetValue(OUT_LDATA2,param));
					fprintf(Fptdata,"READ_DATA = 0x%X,",GetValue(OUT_LDATA3,param));
			      fprintf(Fptdata,"\n");
				  result = 55;
				}else if(GetValue(OUT_BDATA00,param) == i){
				}else{
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
				  fprintf(Fptdata,"PASS,");
     			  fprintf(Fptdata,"\n");
				}
			}
			fclose(Fptdata);
			SetValue(IN_LDATA0,param,GetValue(OUT_LDATA0,param)+1);
			}
		Ffpt = 1;
		break;
		case 204:
		  SetValue(IN_LDATA0,param,0);
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| Module       : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("|   PE         : ");
				switch(GetValue(IN_BDATA00,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("|  PE~Sus Wait : %5d [us]  (4)\n",GetValue(IN_LDATA0,param));
				printf("|Tspd(Sus-Time): 0x%3X       (5)\n",tempo2);
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_BDATA00,param,tempo1);
					break;
				case 4:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-4),param,tempo1);
					break;
				case 5:
					printf(" ----------------------\n");
					printf("  Tspd =    0 [us] (0)\n");
					printf("  Tspd = invalid   (1)\n");
					printf("  Tspd = 3000 [us] (2)\n");
					printf("  Tspd = 2500 [us] (3)\n");
					printf("  Tspd = 2000 [us] (4)\n");
					printf("  Tspd = 1000 [us] (5)\n");
					printf("  Tspd =  500 [us] (6)\n");
					printf("  Tspd = 1500 [us] (7)\n");
					printf(" ----------------------\n");
					printf(" Select Tspd Time ->\n");
					DigitInput(&tempo2);
					break;
				default:
					break;
				}
			}
			 SetValue(IN_BDATA03,param,tempo2);

			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_Tspd_Set"),"Firm Debug");
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,1);
			result = 0;
			
			//File Name Set
      sprintf(item_name,"%s_W%d_X%dY%d_SuspendTime_2nd_Tspd%X",lotname,atoi(wafername),xy_location[0],xy_location[1],tempo2);
	  switch(GetValue(IN_BDATA00,param)){
	  	case 0 : sprintf(item_name2,"_WRITE");break;
	  	case 1 : sprintf(item_name2,"_ERASE");break;
	  	default: sprintf(item_name2,"_ERROR");break;
	  }
      sprintf( file_name, "%s%s%s%s.csv", glob_cdp, glob_datalog, item_name, item_name2 );
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }

			fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz]\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param));
			fprintf(Fptdata,"PE-Wait[us],Suspend-Wait[us],Pass/Fail,\n");
			fclose(Fptdata);
			Ffpt = 0;
			result = tempo3 = 0;
			while(result !=55){
			  //result = 55;//Debug
			  if(tempo3 != PASS)break;
			printf(" ***** Wait Time = %d[us] ~ =+99[us] ***** \n",GetValue(IN_LDATA0,param));
			tempo3 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_FACI"),"Firm Debug");
            //Firm_debugparam_Out();
      
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }	
			for(i=0;i<=(GetValue(OUT_BDATA00,param));i++){
				if((GetValue(OUT_BDATA00,param) == i) && (GetValue(OUT_JUDGE1,param) != 0 )){
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
					switch (GetValue(OUT_BDATA01,param))
					{
					case 2:fprintf(Fptdata,"FASTAT_FAIL,");result = 55;break;
					case 3:fprintf(Fptdata,"FHVEERR_FAIL,");result = 55;break;
					case 4:fprintf(Fptdata,"PRGERR_FAIL,");result = 55;break;
					case 5:fprintf(Fptdata,"ERSERR_FAIL,");result = 55;break;
					case 6:fprintf(Fptdata,"ILGLERR_FAIL,");result = 55;break;
					case 7:fprintf(Fptdata,"OTERR_FAIL,");result = 55;break;
					case 8:fprintf(Fptdata,"ILGCOMERR_FAIL,");result = 55;break;
					case 9:fprintf(Fptdata,"SUSPEND_FAIL,");break;
					default:fprintf(Fptdata,"Firm_ERROR,");result = 55;break;
					}
					fprintf(Fptdata,"REG_FASTAT = 0x%X,",GetValue(OUT_LDATA1,param));
					fprintf(Fptdata,"REG_FSTATR = 0x%X,",GetValue(OUT_LDATA2,param));
					fprintf(Fptdata,"READ_DATA = 0x%X,",GetValue(OUT_LDATA3,param));
			      fprintf(Fptdata,"\n");
				}else if(GetValue(OUT_BDATA00,param) == i){
				}else{
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
				  fprintf(Fptdata,"PASS,");
     			  fprintf(Fptdata,"\n");
				}
			}
			fclose(Fptdata);
			SetValue(IN_LDATA0,param,GetValue(IN_LDATA0,param)+i-1);
			}
			Ffpt = 1;
		break;
		
		case 205:

		for(k=1;k<=2;k++){
			switch (k){
			case 1: SetValue(IN_EXTAL1,param,31);break;
			case 2: SetValue(IN_EXTAL1,param,1000);break;
			}

		for(j=0;j<=8;j++){//Suspend 1st & 2nd & Tspd
		  if(j == 0){
			  SetValue(IN_BDATA03,param,0);
				SetValue(IN_TESTSEL,param,0);
			}else{
			  SetValue(IN_BDATA03,param,j-1);
				CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_Tspd_Set"),"Firm Debug");
				SetValue(IN_TESTSEL,param,1);
			}
		  for(tempo1 = 0; tempo1 <= 1; tempo1++){//Write&Erase
			  SetValue(IN_BDATA00,param,tempo1);
				SetValue(IN_LDATA0,param,0);

				//File Name Set
	      switch(j){
	  	    case 0 : sprintf(item_name2,"_1st_TspdNon");break;
	  	    case 1 : 
	  	    case 2 : 
	  	    case 3 : 
	  	    case 4 : 
	  	    case 5 : 
	  	    case 6 : 
	  	    case 7 : 
	  	    case 8 : sprintf(item_name2,"_2nd_Tspd%X",j-1);break;
					default: sprintf(item_name2,"_Error%X",j);break;
	      }
				sprintf(item_name,"%s_W%d_X%dY%d_SuspendTime%s_CLKF%dMHz",lotname,atoi(wafername),xy_location[0],xy_location[1],item_name2,1000/GetValue(IN_EXTAL1,param));
	      switch(GetValue(IN_BDATA00,param)){
	  	    case 0 : sprintf(item_name2,"_WRITE");break;
	  	    case 1 : sprintf(item_name2,"_ERASE");break;
	  	    default: sprintf(item_name2,"_ERROR");break;
	      }
        sprintf( file_name, "%s%s%s%s_AUTO.csv", glob_cdp, glob_datalog, item_name, item_name2 );
        if((Fptdata=fopen(file_name,"at")) == NULL) {//File Open
          printf(" -- FAIL -- not open file:%s !\n",file_name);
          break;
        }
        fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz],ETLR10=0x%X\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param),GetValue(IN_ETLR10,param));
			  fprintf(Fptdata,"PE-Wait[us],Suspend-Wait[us],Pass/Fail,\n");
			  fclose(Fptdata);
				
				result = tempo3 = 0;
				Ffpt = 0;
			  
				while(result !=55){
					//result = 55;//Debug
			    if(tempo3 != PASS)break;
			    
					printf(" ***** Wait Time = %d[us] ~ =+99[us] ***** \n",GetValue(IN_LDATA0,param));
			    tempo3 = CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_FACI"),"Firm Debug");
          //Firm_debugparam_Out();
      
          if((Fptdata=fopen(file_name,"at")) == NULL) {
            printf(" -- FAIL -- not open file:%s !\n",file_name);
            break;
          }	
			    for(i=0;i<=(GetValue(OUT_BDATA00,param));i++){
				    if((GetValue(OUT_BDATA00,param) == i) && (GetValue(OUT_JUDGE1,param) != 0 )){
			        fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
					    switch (GetValue(OUT_BDATA01,param)){
					      case 2 : fprintf(Fptdata,"FASTAT_FAIL,");result = 55;break;
					      case 3 : fprintf(Fptdata,"FHVEERR_FAIL,");result = 55;break;
					      case 4 : fprintf(Fptdata,"PRGERR_FAIL,");result = 55;break;
					      case 5 : fprintf(Fptdata,"ERSERR_FAIL,");result = 55;break;
					      case 6 : fprintf(Fptdata,"ILGLERR_FAIL,");result = 55;break;
					      case 7 : fprintf(Fptdata,"OTERR_FAIL,");result = 55;break;
					      case 8 : fprintf(Fptdata,"ILGCOMERR_FAIL,");result = 55;break;
					      case 9 : fprintf(Fptdata,"SUSPEND_FAIL,");break;
					      default: fprintf(Fptdata,"Firm_ERROR,");result = 55;break;
					    }
					    fprintf(Fptdata,"REG_FASTAT = 0x%X,",GetValue(OUT_LDATA1,param));
					    fprintf(Fptdata,"REG_FSTATR = 0x%X,",GetValue(OUT_LDATA2,param));
					    fprintf(Fptdata,"READ_DATA = 0x%X,",GetValue(OUT_LDATA3,param));
			        fprintf(Fptdata,"\n");
				    }else if(GetValue(OUT_BDATA00,param) == i){
				    }else{
			        fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
				      fprintf(Fptdata,"PASS,");
     			    fprintf(Fptdata,"\n");
				    }
			    }
			    fclose(Fptdata);
			    SetValue(IN_LDATA0,param,GetValue(IN_LDATA0,param)+i-1);
			  }
			  Ffpt = 1;
		  }
		}
		}

		break;
		case 206:
		  SetValue(IN_LDATA0,param,0);
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("|   PE         : ");
				switch(GetValue(IN_BDATA00,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("|Tspd(Sus-Time): 0x%3X       (5)\n",tempo2);
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_BDATA00,param,tempo1);
					break;
				case 4:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-4),param,tempo1);
					break;
				case 5:
					printf(" ----------------------\n");
					printf("  Tspd =    0 [us] (0)\n");
					printf("  Tspd = invalid   (1)\n");
					printf("  Tspd = 3000 [us] (2)\n");
					printf("  Tspd = 2500 [us] (3)\n");
					printf("  Tspd = 2000 [us] (4)\n");
					printf("  Tspd = 1000 [us] (5)\n");
					printf("  Tspd =  500 [us] (6)\n");
					printf("  Tspd = 1500 [us] (7)\n");
					printf(" ----------------------\n");
					printf(" Select Tspd Time ->\n");
					DigitInput(&tempo2);
					break;
				default:
					break;
				}
			}
			 SetValue(IN_BDATA03,param,tempo2);

			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("SuspendResume_Tspd_Set"),"Firm Debug");
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,0);
			SetValue(IN_LDATA0,param,adr);
			result = 0;
			
			//File Name Set
      sprintf(item_name,"%s_W%d_X%dY%d_Renzoku_Suspend_Tspd%X",lotname,atoi(wafername),xy_location[0],xy_location[1],tempo2);
	  switch(GetValue(IN_BDATA00,param)){
	  	case 0 : sprintf(item_name2,"_WRITE");break;
	  	case 1 : sprintf(item_name2,"_ERASE");break;
	  	default: sprintf(item_name2,"_ERROR");break;
	  }
      sprintf( file_name, "%s%s%s%s.csv", glob_cdp, glob_datalog, item_name, item_name2 );
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }

			fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz]\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param));
			fprintf(Fptdata,"PE-Wait[us],Suspend-Wait[us],Pass/Fail,\n");
			fclose(Fptdata);
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("FlashSM_7_5_SuspendResume"),"Firm Debug");
            Firm_debugparam_Out();
      
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }	
	        for(j=0;j<10;j++){
	        k=0;
	        fprintf(Fptdata,"0x%X,0x%X,0x%X,0x%X,",FirmDebug.DataSet[j].Data[k+0],FirmDebug.DataSet[j].Data[k+1],FirmDebug.DataSet[j].Data[k+2],FirmDebug.DataSet[j].Data[k+3]);
		    fprintf(Fptdata,"%d,",FirmDebug.DataSet[j].Data[k+4]*GetValue(IN_EXTAL1,param));
     		fprintf(Fptdata,"\n");
			k=5;
	        fprintf(Fptdata,"0x%X,0x%X,0x%X,0x%X,",FirmDebug.DataSet[j].Data[k+0],FirmDebug.DataSet[j].Data[k+1],FirmDebug.DataSet[j].Data[k+2],FirmDebug.DataSet[j].Data[k+3]);
		    fprintf(Fptdata,"%d,",FirmDebug.DataSet[j].Data[k+4]*GetValue(IN_EXTAL1,param));
     		fprintf(Fptdata,"\n");
     		}
			fclose(Fptdata);
			Ffpt = 1;
		break;

		case 301:
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| PE Module    : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("| PE Mode      : ");
				switch(GetValue(IN_TESTSEL,param)){
					case 0 : printf(       "Write_BGO" );break;
					case 1 : printf(       "Erase_BGO" );break;
					case 2 : printf(       "Write" );break;
					case 3 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("| Read Module  : ");
				switch(module_sel2){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (4)\n");
				printf("| Read Addr : 0x00000 <-> 0x7FFF0\n");
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 4:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel2);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write_BGO(0) / Erase_BGO(1) / Write(2) / Erase(3) ->");DigitInput(&tempo1);
					SetValue(IN_TESTSEL,param,tempo1);
					break;
				default:
					break;
				}
			}
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_BDATA00,param,module_sel2);
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("BGO_FCU"),"Firm Debug");
			result = 0;
    break;
		case 302:
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| PE Module    : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X~0x%6X (2)\n",adr,adr+adr2-1);
				printf("| PE Mode      : ");
				switch(GetValue(IN_TESTSEL,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("| Read Module  : ");
				switch(module_sel2){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (4)\n");
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 4:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel2);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_TESTSEL,param,tempo1);
					if(tempo1 == 0){
						adr2 = 0x100;
					}else{
						adr2 = 0x1000;
					}
					break;
				default:
					break;
				}
			}
			if(GetValue(IN_TESTSEL,param) == 0){
				sprintf(item_name2,"WRITE");
			}else{
			  sprintf(item_name2,"ERASE");
			}
			sprintf(item_name,"%s_W%d_X%dY%d_BGO_ReadCheck_%s",lotname,atoi(wafername),xy_location[0],xy_location[1],item_name2);
      sprintf( file_name, "%s%s%s.csv", glob_cdp, glob_datalog, item_name );
		  if((Fptdata=fopen(file_name,"rt")) == NULL) {
        if((Fptdata=fopen(file_name,"at")) == NULL) {
          printf(" -- FAIL -- not open file:%s !\n",file_name);
          break;
        }
			  fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz],vcc=%d,vddh=%d,vdd=%d\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param),vs[0],vs[1],vs[2]);
			  fprintf(Fptdata,"PE-Module,PE-Address,Read-Module,Read-Address,Read-Time[us],");
			  fprintf(Fptdata,"BGO-Read Pass/Fail,Fail-Address,After-Read Pass/Fail,FASTAT Register,FSTATR Register\n");
			  fclose(Fptdata);
      }

			tempo1 = SetValue(IN_TESTSEL,param,0xFE);
			SetValue(IN_BDATA00,param,module_sel2);
			SetValue(IN_LDATA0,param,0);
			SetValue(IN_LDATA1,param,0x80000);
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("BGO_FACI"),"BGO ReadCheck");

			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,tempo1);
			SetValue(IN_LDATA1,param,0x20000);
			
			Ffpt =1;

			for(i=0;i<4;i++){
				if((Fptdata=fopen(file_name,"at")) == NULL) {
          printf(" -- FAIL -- not open file:%s !\n",file_name);
          break;
        }

				SetValue(IN_LDATA0,param,	GetValue(IN_LDATA1,param) * i);
			  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("BGO_FACI"),"BGO ReadCheck");
				Firm_debugparam_Out();
				for(j=0;j<GetValue(OUT_BDATA00,param);j++){
					//PE Module Name File Out
				  if(FirmDebug.DataSet[j].Data[0]<0x80000){
						fprintf(Fptdata,"FLP0,");
					}else if(FirmDebug.DataSet[j].Data[0]<0x100000){
						fprintf(Fptdata,"FLI1,");
					}else if(FirmDebug.DataSet[j].Data[0]<0x180000){
						fprintf(Fptdata,"FLI2,");
					}else{
						fprintf(Fptdata,"ERROR,");
					}

					//PE Address File Out
			  	fprintf(Fptdata,"0x%X~0x%X,",FirmDebug.DataSet[j].Data[0],FirmDebug.DataSet[j].Data[0]+FirmDebug.DataSet[j].Data[1]-1);

					//Read Module Name File Out
				  if(FirmDebug.DataSet[j].Data[2]<0x80000){
						fprintf(Fptdata,"FLP0,");
					}else if(FirmDebug.DataSet[j].Data[2]<0x100000){
						fprintf(Fptdata,"FLI1,");
					}else if(FirmDebug.DataSet[j].Data[2]<0x180000){
						fprintf(Fptdata,"FLI2,");
					}else{
						fprintf(Fptdata,"ERROR,");
					}

					//Read Address File Out
			  	fprintf(Fptdata,"0x%X~0x%X,",FirmDebug.DataSet[j].Data[2],FirmDebug.DataSet[j].Data[2]+FirmDebug.DataSet[j].Data[3]-1);

					//Read Time File Out
			  	fprintf(Fptdata,"%d,",(FirmDebug.DataSet[j].Data[6]*GetValue(IN_EXTAL1,param))/1000);
          
					//BGO-Read Pass/Fail
					if(FirmDebug.DataSet[j].Data[9]==0xFFFFFFFF){
					  fprintf(Fptdata,"PASS,0x%X,",FirmDebug.DataSet[j].Data[9]);
					}else{
						fprintf(Fptdata,"FAIL,0x%X,",FirmDebug.DataSet[j].Data[9]);
					}

					//After-Read Pass/Fail
					if((GetValue(OUT_JUDGE1,param) & 0x1000) != 0x1000){
					  fprintf(Fptdata,"PASS,");
					}else{
						fprintf(Fptdata,"FAIL,");
					}

					//Flash-SM Register Set
					fprintf(Fptdata,"0x%X,0x%X,",FirmDebug.DataSet[j].Data[7],FirmDebug.DataSet[j].Data[8]);

					// End
					fprintf(Fptdata,"\n");
				}

			  result = 0;
				fclose(Fptdata);
			}
			Ffpt = 1;
    break;
		case 303:
		case 304:
		  BL_DATACLEAR(param);
		  printf("Input Temperature -40 / 25 / 95 -> ");DigitInput(&tempo1);
		  SetValue(IN_TESTSEL,param,0);
		  SetValue(IN_BDATA00,param,0);
			SetValue(IN_BDATA01,param,0);

			//’PˆêModule‚Ì‚Ý•]‰¿‚·‚é
			if(GetValue(IN_TESTSIZE0,param) !=0){
				SetValue(IN_TESTSIZE1,param,0);
				SetValue(IN_TESTSIZE2,param,0);
			}else if(GetValue(IN_TESTSIZE1,param) !=0){
				SetValue(IN_TESTSIZE0,param,0);
				SetValue(IN_TESTSIZE2,param,0);
			}else if(GetValue(IN_TESTSIZE2,param) !=0){
				SetValue(IN_TESTSIZE0,param,0);
				SetValue(IN_TESTSIZE1,param,0);
			}

		  switch (test_cond){
			case 303:
			  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
				SetValue(IN_TESTSEL,param,0);
				sprintf(item_name2,"WRITE");
				search_mode = 3;  //Loss Down
				vth_mode = 23;    //VthRead2&3
				max_vol = 2500;   //max = 2.5V
        min_vol = 0;      //min = 0V
				delta_vol = 100;  //VthRead Step
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern

			break;
			case 304:
			  CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
				SetValue(IN_TESTSEL,param,1);
				sprintf(item_name2,"ERASE");
				search_mode = 1;      //Gain Down
				vth_mode = 1;         //VthRead1
				max_vol = 0;          //max = 0V
        min_vol = -2000;      //min = -2.0V
				delta_vol = 100;      //VthRead Step
        bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1; //*1=ALL Pattern
			break;
			}
			//File Name Setting
			sprintf(item_name,"%s_W%d_X%dY%d_PE-BGO_Vcc%d_%dC_VthRead",lotname,atoi(wafername),xy_location[0],xy_location[1],vs[0],tempo1);
      /*
			sprintf( file_name, "%s%s%s.csv", glob_cdp, glob_datalog, item_name );
		  if((Fptdata=fopen(file_name,"rt")) == NULL) {
        if((Fptdata=fopen(file_name,"at")) == NULL) {
          printf(" -- FAIL -- not open file:%s !\n",file_name);
          break;
        }
			  fprintf(Fptdata,"Temp[C],Vcc,Vddh,Vdd,PE-Module,PE-Address,Vth[mV],");
    		// ETLR Data Out
    		for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      		fprintf(Fptdata,"ETLR%2d,",(i - IN_ETLR00));
    		}
    		// ETCR Data Out
    		for(i=IN_ETCR00;i<=IN_ETCR13;i++){
      		fprintf(Fptdata,"ETCR%2d,",(i - IN_ETCR00));
    		}
				fprintf(Fptdata,"\n");
			  fclose(Fptdata);
      }
			*/

			//Test Main
			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime*3,param,CreatePatFileName("PE_BGO"),"BGO ReadCheck");
			freq_bak = SetValue(IN_EXTAL1,param,VTH_FREQ);
			min_vol = VthEdgeSearch(search_mode,100,&vs_bak[0],W1SEC*waittime, param);
			VthReadDist(vth_mode, bit_count, min_vol, delta_vol, &vs_bak[0],W1SEC*waittime, param, item_name); //Vth•ª•z‘ª’è
			//Test Main End

			SetValue(IN_EXTAL1,param,freq_bak);
			/*
			if((Fptdata=fopen(file_name,"at")) == NULL) {//File Open
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }
			fprintf(Fptdata,"%d,%d,%d,%d,",tempo1,vs[0],vs[1]*25/10,vs[2]*158/100);//Temp,Vcc,Vddh,Vdd
			//Module & Address
			if(GetValue(IN_TESTSIZE0,param) !=0){
				fprintf(Fptdata,"FLP0,");
				tempo2 = GetValue(IN_TESTSIZE0,param);
				tempo4 = GetValue(IN_TOPADDR0,param);
			}else if(GetValue(IN_TESTSIZE1,param) !=0){
				fprintf(Fptdata,"FLI1,");
				tempo2 = GetValue(IN_TESTSIZE1,param);
				tempo4 = GetValue(IN_TOPADDR1,param);
			}else if(GetValue(IN_TESTSIZE2,param) !=0){
				fprintf(Fptdata,"FLI2,");
				tempo2 = GetValue(IN_TESTSIZE2,param);
				tempo4 = GetValue(IN_TOPADDR2,param);
			}
			fprintf(Fptdata,"0x%X~0x%X,",tempo4,tempo4+tempo2-1);
			fprintf(Fptdata,"%d,",result);
    	//ETLR Data Out
    	for(i=IN_ETLR00;i<=IN_ETLR39;i++){
      	fprintf(Fptdata,"0x%X,",GetValue(i,param));
    	}
    	// ETCR Data Out
    	for(i=IN_ETCR00;i<=IN_ETCR13;i++){
      	fprintf(Fptdata,"0x%X,",GetValue(i,param));
    	}
			fprintf(Fptdata,"\n");
			fclose(Fptdata);
			*/
		break;

		case 401:
		  SetValue(IN_LDATA0,param,0);
		  testflag = 0;
		  while(testflag != 99 ){
		  	printf("=================================\n");
				printf("| Module       : ");
				switch(module_sel){
					case 0 : printf(       "FLP0" );break;
					case 1 : printf(       "FLI1" );break;
					case 2 : printf(       "FLI2" );break;
					default: printf(       "ERROR");break;
				}
				printf(                       "        (1)\n");
				printf("| Address      : 0x%6X    (2)\n",adr);
				printf("|   PE         : ");
				switch(GetValue(IN_BDATA00,param)){
					case 0 : printf(       "Write" );break;
					case 1 : printf(       "Erase" );break;
					default: printf(       "ERROR" );break;
				}
				printf(                        "       (3)\n");
				printf("|  PE~Abo Wait : %5d [us]  (4)\n",GetValue(IN_LDATA0,param));
		  	printf("=================================\n");
				printf("|       PE Start (99)\n");
		  	printf("=================================\n");
        printf("Input Number -> ");DigitInput(&testflag);
				switch (testflag){
				case 1:
					printf("Select Module : FLP0(0) / FLI1(1) / FLI2(2) ->");DigitInput(&module_sel);
					break;
				case 2:
					printf("Input Address {ex.""0x1F00""}->");DigitInput(&adr);
					break;
				case 3:
					printf("Select Test : Write(0) / Erase(1) ->");DigitInput(&tempo1);
					SetValue(IN_BDATA00,param,tempo1);
					break;
				case 4:
					printf("Input Wait Time {ex.""100"" -> 100[us]}->");DigitInput(&tempo1);
					SetValue(IN_LDATA0+(testflag-4),param,tempo1);
					break;
				default:
					break;
				}
			}
			ChangeAddress(module_sel,adr,0xFF,param);
			SetValue(IN_TESTSEL,param,0);
			result = 0;
			
			//File Name Set
      sprintf(item_name,"%s_W%d_X%dY%d_AbortTime",lotname,atoi(wafername),xy_location[0],xy_location[1]);
	  switch(GetValue(IN_BDATA00,param)){
	  	case 0 : sprintf(item_name2,"_WRITE");break;
	  	case 1 : sprintf(item_name2,"_ERASE");break;
	  	default: sprintf(item_name2,"_ERROR");break;
	  }
      sprintf( file_name, "%s%s%s%s.csv", glob_cdp, glob_datalog, item_name, item_name2 );
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }

			fprintf(Fptdata,"TestInfo,T=%d[ns],F=%d[MHz]\n",GetValue(IN_EXTAL1,param),1000/GetValue(IN_EXTAL1,param));
			fprintf(Fptdata,"PE-Wait[us],Abort-Wait[ns],Pass/Fail,\n");
			fclose(Fptdata);
			Ffpt = 0;
			for(k=0;k<=(40*(GetValue(IN_BDATA00,param)+1));k++){
			if(k==0){
			  repeat_mode =INITIAL_SET | OUTIF_READ | RAMBOOT_CHANGE;
			}else{
			  repeat_mode = OUTIF_READ | RAMBOOT_CHANGE | INIF_SET;
			}
			if(result==55)break;
			printf(" ***** Wait Time = %d[us] ~ =+99[us] ***** \n",GetValue(IN_LDATA0,param));
			//CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("FACI_Abort"),"Firm Debug");
      		CpuModeMonitor_Repeat(repeat_mode,MNORMAL_APP,&vs[0],param,W1SEC*waittime*10,CreatePatFileName("FACI_Abort"),"MONITOR_READDELAY(DELAYTEST)",0);
      //Firm_debugparam_Out();
      
      if((Fptdata=fopen(file_name,"at")) == NULL) {
        printf(" -- FAIL -- not open file:%s !\n",file_name);
        break;
      }	
			for(i=0;i<=(GetValue(OUT_BDATA00,param));i++){
				
				if((GetValue(OUT_BDATA00,param) == i) && (GetValue(OUT_JUDGE1,param) != 0 )){
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
					switch (GetValue(OUT_BDATA01,param))
					{
					case 2:fprintf(Fptdata,"FASTAT_FAIL,");break;
					case 3:fprintf(Fptdata,"FHVEERR_FAIL,");break;
					case 4:fprintf(Fptdata,"PRGERR_FAIL,");break;
					case 5:fprintf(Fptdata,"ERSERR_FAIL,");break;
					case 6:fprintf(Fptdata,"ILGLERR_FAIL,");break;
					case 7:fprintf(Fptdata,"OTERR_FAIL,");break;
					case 8:fprintf(Fptdata,"ILGCOMERR_FAIL,");break;
					//case 9:fprintf(Fptdata,"SUSPEND_FAIL,");break;
					default:fprintf(Fptdata,"Firm_ERROR,");break;
					}
					fprintf(Fptdata,"REG_FASTAT = 0x%X,",GetValue(OUT_LDATA1,param));
					fprintf(Fptdata,"REG_FSTATR = 0x%X,",GetValue(OUT_LDATA2,param));
					fprintf(Fptdata,"READ_DATA = 0x%X,",GetValue(OUT_LDATA3,param));
			      fprintf(Fptdata,"\n");
				  result = 55;
				}else if(GetValue(OUT_BDATA00,param) == i){
				}else{
			      fprintf(Fptdata,"%d,%d,",GetValue(IN_LDATA0,param)+i,(GetValue(IN_EXTAL1,param) * ReadEcrMode8(FBM_READTOP + (4*i),SIZE_L)));
				  fprintf(Fptdata,"PASS,");
     			  fprintf(Fptdata,"\n");
				}
			}
			fclose(Fptdata);
			SetValue(IN_LDATA0,param,GetValue(OUT_LDATA0,param)+1);
			}
		Ffpt = 1;
		break;

	  case 501:
	    SetValue(IN_TESTSEL,param,0);
      printf("================\n");
      printf("| Read OFF (0) |\n");
      printf("| Read ON  (1) |\n");
      printf("================\n");
			DigitInput(&tempo0);
	    SetValue(IN_BDATA00,param,tempo0);
	    SetValue(IN_LDATA0,param,10);
			
			// Tcheck Setting
			SetValue(IN_ETCR11,param,(GetValue(IN_ETCR11,param)&0x1F) | (2 << 5));
	    
			// Monitor Mode Set
			SetValue(IN_ETCR01,param,(GetValue(IN_ETCR01,param)&0xE0)|0x02); // Vnoemi monitor mode
      SetValue(IN_ETCR02,param,(GetValue(IN_ETCR02,param)&0xC0)|0x22); // VPP monitor mode

			CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*5,param,CreatePatFileName("FCU_BGO_Vnoemi_Check"),"BGO Debug");
			if((GetValue(OUT_ETCR16,param) & 0x40) == 0x40){
        printf("\n\n  --- TimeOut Check ERROR!!! --- \n");
        printf(    "      ETCR16 = 0x%X\n\n\n",GetValue(OUT_ETCR16,param));
      }

	  break;
	  
	  
	  
    }//switch test_cond end
  }//Menu While end
  
}//FCU Menu end