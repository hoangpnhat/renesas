#include "RC04EX_registerall.c"

int AE_WT1(interface_t* param,int judge)
{
  FILE_T *fpt;
  int tempo,tempo0,tempo1,tempo2,tempo3,tempo4,tempo5,tempo6;
  int stack0,stack1,stack2,stack3,stack4,stack5,stack6,stack7,stack8;
  int min_vol,result,vth_mode,bit_count,selvol;
  int i,j,l,mat,area,st_addr,cond0,cond1,cond2,cond3,cond4;
  int judg,result_loss,result_gain,loop_loss,loop_gain,vth_loss,vth_gain;
  int result_iref,result_vhhw,result_vppw,result_vhhe,result_vppe;
  int etlr_25;
  int error_flag,fail_flag,waittime;
  int info[32];
  // wmat chip information
  int trimvalue[8];

  char file_name[128];

  char item_name[128];

  char sample_name[128];

  char test_name[128];

  char test_item[128];

  char vthread_i[128];

  char test_temp[128];

  int  vs[NUM_POWERSUPPLY];
  FILE_T *file_log;
  waittime = INIT_WAIT;
  cond1 = cond2 = cond3 = cond4 = 0;
  result = judg = result_loss = result_gain = error_flag = fail_flag =0;
  trimvalue[I_IMONI_FLASH_NOTEMP] = TARGET_IREF_F_25C;
  vs[0] = 3300;
  vs[1] = vs[2] = 500;
  Ffpt = INIT_FFPT;
// ************************************************************************************
// ************************************************************************************
      stack0 = GetValue(IN_AREA,param);
      stack1 = GetValue(IN_TOPADDR0,param);
      stack2 = GetValue(IN_TESTSIZE0,param);
      stack3 = GetValue(IN_TOPADDR1,param);
      stack4 = GetValue(IN_TESTSIZE1,param);
      stack5 = GetValue(IN_TOPADDR2,param);
      stack6 = GetValue(IN_TESTSIZE2,param);
      stack7 = SetValue(IN_EXTAL1,param,31);
      stack8 = GetValue(IN_REG,param);
      etlr_25 = GetValue(IN_ETLR25,param);
      InitialValue(param);
// ************************************************************************************
// **   Initialization                                                               **
// ************************************************************************************
        DeviceSpecificPowerUp();

        SetValue(IN_REG,param,CHIPDATA);
        SetValue(IN_TESTSEL,param,0x00);
        if ( PASS == CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC,param,CreatePatFileName("devicefunction_at"),"DEBUG PASS") ) {
          SetValue(IN_ETLR00,param,GetValue(OUT_ETLR00,param)); SetValue(IN_ETLR01,param,GetValue(OUT_ETLR01,param)); SetValue(IN_ETLR02,param,GetValue(OUT_ETLR02,param));
          SetValue(IN_ETLR03,param,GetValue(OUT_ETLR03,param)); SetValue(IN_ETLR04,param,GetValue(OUT_ETLR04,param)); SetValue(IN_ETLR05,param,GetValue(OUT_ETLR05,param));
          SetValue(IN_ETLR06,param,GetValue(OUT_ETLR06,param)); SetValue(IN_ETLR07,param,GetValue(OUT_ETLR07,param)); SetValue(IN_ETLR08,param,GetValue(OUT_ETLR08,param));
          SetValue(IN_ETLR09,param,GetValue(OUT_ETLR09,param)); SetValue(IN_ETLR10,param,GetValue(OUT_ETLR10,param)); SetValue(IN_ETLR11,param,GetValue(OUT_ETLR11,param));
          SetValue(IN_ETLR12,param,GetValue(OUT_ETLR12,param)); SetValue(IN_ETLR13,param,GetValue(OUT_ETLR13,param)); SetValue(IN_ETLR14,param,GetValue(OUT_ETLR14,param));
          SetValue(IN_ETLR15,param,GetValue(OUT_ETLR15,param)); SetValue(IN_ETLR16,param,GetValue(OUT_ETLR16,param)); SetValue(IN_ETLR17,param,GetValue(OUT_ETLR17,param));
          SetValue(IN_ETLR18,param,GetValue(OUT_ETLR18,param)); SetValue(IN_ETLR19,param,GetValue(OUT_ETLR19,param)); SetValue(IN_ETLR20,param,GetValue(OUT_ETLR20,param));
          SetValue(IN_ETLR21,param,GetValue(OUT_ETLR21,param)); SetValue(IN_ETLR22,param,GetValue(OUT_ETLR22,param)); SetValue(IN_ETLR23,param,GetValue(OUT_ETLR23,param));
          SetValue(IN_ETLR24,param,GetValue(OUT_ETLR24,param)); SetValue(IN_ETLR25,param,GetValue(OUT_ETLR25,param)); SetValue(IN_ETLR26,param,GetValue(OUT_ETLR26,param));
          SetValue(IN_ETLR27,param,GetValue(OUT_ETLR27,param)); SetValue(IN_ETLR28,param,GetValue(OUT_ETLR28,param)); SetValue(IN_ETLR29,param,GetValue(OUT_ETLR29,param));
          SetValue(IN_ETLR30,param,GetValue(OUT_ETLR30,param)); SetValue(IN_ETLR31,param,GetValue(OUT_ETLR31,param)); SetValue(IN_ETLR32,param,GetValue(OUT_ETLR32,param));
          SetValue(IN_ETLR33,param,GetValue(OUT_ETLR33,param)); SetValue(IN_ETLR34,param,GetValue(OUT_ETLR34,param)); SetValue(IN_ETLR35,param,GetValue(OUT_ETLR35,param));
          SetValue(IN_ETLR36,param,GetValue(OUT_ETLR36,param)); SetValue(IN_ETLR37,param,GetValue(OUT_ETLR37,param)); SetValue(IN_ETLR38,param,GetValue(OUT_ETLR38,param));
          SetValue(IN_ETLR39,param,GetValue(OUT_ETLR39,param));
          printf("************************************");
          printf("***   INIT PASS                  ***");
          printf("************************************");
          printf("");
        } else {
          printf("------------------------------------");
          printf("---   INIT FAIL                  ---");
          printf("------------------------------------");
          printf("");
	         return(FAIL);
        }

// ************************************************************************************
// **   Chip-Infomation Read                                                         **
// ************************************************************************************
      stack3 = SetValue(IN_TESTSEL,param,0);
      // VthRead count mode
      stack4 = SetValue(IN_BDATA00,param,0);
      stack0 = SetValue(IN_AREA,param,5);
      // Extra5
      stack1 = SetValue(IN_TOPADDR0,param,0);
      stack2 = SetValue(IN_TESTSIZE0,param,EXTRAS_SIZE);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("readdump_code_at"),"CPU Vth-READ ALL");
      info[0] = ReadEcrMode8(FBM_READTOP+0x0020,SIZE_B);
      info[1] = ReadEcrMode8(FBM_READTOP+0x0021,SIZE_B);
      info[2] = ReadEcrMode8(FBM_READTOP+0x0022,SIZE_B);
      info[3] = ReadEcrMode8(FBM_READTOP+0x0023,SIZE_B);
      info[4] = ReadEcrMode8(FBM_READTOP+0x0024,SIZE_B);
      info[5] = ReadEcrMode8(FBM_READTOP+0x0025,SIZE_B);
      info[10]= ReadEcrMode8(FBM_READTOP+0x002A,SIZE_B);
      info[11]= ReadEcrMode8(FBM_READTOP+0x002C,SIZE_B);
      info[12]= ReadEcrMode8(FBM_READTOP+0x002E,SIZE_B);

      printf("");
      printf("*** L# : %c%c%c%c%c%c",info[0],info[1],info[2],info[3],info[4],info[5]);
      printf("*** W# : %3d",info[10]);
      printf("***  X : %3d",info[11]);
      printf("***  Y : %3d",info[12]);

      SetValue(IN_AREA,param,stack0);
      SetValue(IN_TOPADDR0,param,stack1);
      SetValue(IN_TESTSIZE0,param,stack2);
      SetValue(IN_TESTSEL,param,stack3);
      SetValue(IN_BDATA00,param,stack4);

      result = Set_Register(info[11],info[12],info[10],param);
      if(result == FAIL) return(FAIL);
      stack0 = SetValue(IN_TESTSEL,param,0);
      stack1 = SetValue(IN_BDATA00,param,1);
      stack2 = SetValue(IN_BDATA01,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("settrimingdata_code_at"),"Write Triming REGISTER");
      // t@[ŕύX\
      SetValue(IN_TESTSEL,param,stack0);
      SetValue(IN_BDATA00,param,stack1);
      SetValue(IN_BDATA01,param,stack2);
      
	  SetValue(IN_AREA,param,0);
	  SetValue(IN_TOPADDR0,param,FLP0_TOP);
	  SetValue(IN_TESTSIZE0,param,FLP0_SIZE);
	  SetValue(IN_TOPADDR1,param,0);
	  SetValue(IN_TESTSIZE1,param,0);
	  SetValue(IN_TOPADDR2,param,0);
	  SetValue(IN_TESTSIZE2,param,0);

      sprintf(sample_name,"%c%c%c%c%c%c_W%02d_X%03dY%03d_",info[0],info[1],info[2],info[3],info[4],info[5],info[10],info[11],info[12]);
      strcpy(item_name,sample_name);
      strcat(item_name,"VthRead");

     if(judge == 0){
	  SetValue(IN_TESTSEL,param,0);
      CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
      // t@[ŕύX\

        if(info[11] % 2 == 0){
          SetValue(IN_TESTSEL,param,0);
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("programall_code_at"),"Program ALL");
          // t@[ŕύX\
        }else if(info[11] % 2 == 1){
	      SetValue(IN_TESTSEL,param,0);
          CpuModeFunc(MNORMAL,MSNORMAL,&vs[0],W1SEC*waittime,param,CreatePatFileName("erase_code_at"),"Erase");
          // t@[ŕύX\
        }
	  }
 
      SetValue(IN_EXTAL1,param,VTH_FREQ);
      tempo2 = 50;
      bit_count = (GetValue(IN_TESTSIZE0,param)+GetValue(IN_TESTSIZE1,param)+GetValue(IN_TESTSIZE2,param)) * 8 * 1;
      /      	// Log Headder
      	// Log Headder
