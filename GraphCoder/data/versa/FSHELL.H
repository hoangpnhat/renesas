// Filename:Shell.h Header file for rev 2.0 shell program. 9/23/92
extern  void    PowerDown(int bin);
// Power down routine
extern  int     ovrhead;
// timer variables
extern  int     curtime;
extern  int     forced_flow;

// mode option variables
extern  int     device;
extern  int     dlog;
extern  int     lya;
extern  int     var1;
extern  int     var2;

// Tester configuration
extern  int     io_process;
extern  int     test_site;
extern  int     test_mode;
extern  int     test_flow;
extern  int     test_type;
extern  int     forced_flow;
extern  int     leakage_test_flag;
extern  int     apc_test_flag;
extern  int     xy_location[2];
extern  char    lot_name[0x20];
extern  char    wafer_name[0x20];
extern  char    device_name[0x20];
extern  char    oper_name[0x20];
extern  char    station_name[0x20];
extern  char    tester_name[0x20];
extern  char    probe_name[0x20];
