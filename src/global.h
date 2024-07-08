#ifndef __GLOBAL_HINC
#define __GLOBAL_HINC


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>


#include "jsmninc.h"
#include "uthash.h"
#include "enums.h"

// #define PCT_S(c) "%" c
// #define DEBUG_HDR      PCT_S("s") "->" PCT_S("s") "(" PCT_S("d") ")::  "
#define DEBUG_HDR      "%s->%s(%d)::  "
#define DEBUG_PRINTF( LVL , FMT , ... )  debug_printf( LVL, DEBUG_HDR FMT , __FILE__, __func__, __LINE__, ##__VA_ARGS__ )

#define SZ_ATCMD_HASHKEY    5
#define RMSZ  9
#define ERRBFSZ   1024


// Shared 
#define MAXMM_MDMNAME   128


//  2 (baseCommand) , 5 (flags) ,  7 (action) , 8 (value)
typedef struct s_CMD_In {
    struct s_CMD_In *p_next;
    char *cmdFull;
    char *cmdBase;
    char *cmdFlag;
    char *cmdAction; //  ? or =
    char *cmdValue;
    char bf;
} s_CMD_IN_t;

typedef struct mdmDISPLAY {
    unsigned char RX;
    unsigned char TX;
    unsigned char DCD;
    unsigned char RTS;
    unsigned char CTS;
    unsigned char DTR;
    unsigned char DSR;
    enum mdmSTATE mdmSt;
    char    mdmname[MAXMM_MDMNAME];
}  mdmDISPLAY;


//  https://troydhanson.github.io/uthash/userguide.html
#define SZ_REGVCHAR  sizeof(unsigned long int)
typedef union u_regv {
        char ch[SZ_REGVCHAR];
        unsigned long int  int_v;
        char **pStrings;
} u_regv;        

typedef struct {
    enum tpREGV tpx;
    union u_regv urv;
} u_regv_t;        
#define SZ_VLCMDUNION sizeof(u_regv)

typedef struct atCmds atCmds;

#define SZ_ATCMD_HASHKEY    5
struct atCmds {
    char CMDpfx[SZ_ATCMD_HASHKEY];            /* key */
                               /* AT commands are prefix+CMD+MOD :: charALT + charCMDCODE + charMODIFIER */
                               /* Ex.:  DT ;; &C ;; %K  */ 
    void (*func)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
    enum EcmdRC RC; 
    // void (*cmdProcess)();
    u_regv_t reg_value;        
    //
    int  id;
    char **s_responses;
    //
    UT_hash_handle hh;         /* makes this structure hashable */
};

typedef void (*v_func_t)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...)  ;  
void dummyProc(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);



extern jsmn_parser jsmn_config_SRC;
#define CONFIGTOKS   2048
extern jsmntok_t jsmn_configTOKs[];


extern s_CMD_IN_t *regexProc(char *p0);
extern char **strArray(  int K );


#define SILENCETIMEOUT  1000

extern char *filename; // /etc/rc.conf";
//
extern char s_atResponsesSTR[];
extern char s_atResponsesOK[];
extern char s_atDFLReg[];

void configResponseOK(int K);
void configResponseSTR(int K );
void configRegisters(int K);

extern char *bfConfig;

extern regmatch_t rm[];

extern char errBuff[];
extern char line[];
extern regex_t re;
extern struct atCmds *cmdsTBL;

#endif