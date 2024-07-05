#ifndef _FAKEMODEM_H
#define _FAKEMODEM_H


#include "global.h"

void processInput( s_CMD_IN_t *p_cmd );

// Fake Modem Function IMPLEMENTATIONS
//
void f_trata_OK(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATI(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATD(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATA(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATO(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATH(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATS(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATE(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATZ(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ERROR(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ABORT(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);

extern char **strArray(  int K );

//  Ref:: https://pokristensson.com/strmap.html
//  https://troydhanson.github.io/uthash/userguide.html
#include "uthash.h"

/*
 * Standard POSIX EXTENDED REGEX
 *
 * Examples of regex returns::
    <<&K3B0 S9?S21=354>>
    Line: <<&K3>>
        Text[1]: <<&K3>>
        Text[2]: <<&K>>
        Text[3]: <<&>>
        Text[4]: <<K>>
        Text[5]: <<3>>
        Text[6]: <<>>
    ndx 7 == -1
        Text[8]: <<>>
    ndx 9 == -1
    ndx 10 == -1
    << S9?S21=354>>
    Line: << S9?>>
        Text[1]: <<S9?>>
        Text[2]: <<S>>
        Text[3]: <<>>
        Text[4]: <<S>>
        Text[5]: <<9>>
        Text[6]: <<?>>
        Text[7]: <<?>>
        Text[8]: <<>>
    ndx 9 == -1
    ndx 10 == -1
    <<S21=354>>
    Line: <<S21=354>>
        Text[1]: <<S21=354>>
        Text[2]: <<S>>
        Text[3]: <<>>
        Text[4]: <<S>>
        Text[5]: <<21>>
        Text[6]: <<=354>>
        Text[7]: <<=>>
        Text[8]: <<354>>
    ndx 9 == -1
    ndx 10 == -1
    


 *   1 - FullCMD (no Query or Setters)
 *   2 - Command
 *   3 - Modifier
 *   4 - Command/SubCommand
 *   5 - CommandIndex/Value
 *   6 - Query/Variable expression
 *   7 - Query/Variable Flag (?|=)
 *   8 - Register Set Value
*/

s_CMD_IN_t *regexProc(char *p0);

#define tofind    " ?((([&\\+%]?) ?([a-zA-Z]+))([0-9|;]*|[;]?)(([=?])?([0-9]*))?)"

// Returns always 8 tokens... 
#define RMSZ  9
extern regmatch_t rm[];

#define ERRBFSZ   1024
extern char errBuff[];
extern char line[];
extern regex_t re;

void dummyProc(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
    

extern enum mdmSTATE modemState;

      
typedef void (*v_func_t)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...)  ;                 
typedef struct  {
    enum E_FUNC_AT ky;
    char *nome;
    // void (*func)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
     v_func_t func;
    } k_trata_dict;

//  <KTYPE of processing> <KCONFIGKEY> <function implementation>
#define K_TRATA_CMDNDX   0
#define K_TRATA_CMDKEY   1
#define K_TRATA_CMDFUNC  2

void processInput( s_CMD_IN_t *p_cmd );

// Fake Modem Function IMPLEMENTATIONS
//
void f_trata_OK(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATI(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATD(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATA(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATO(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATH(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATS(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATE(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATZ(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ERROR(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ABORT(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);

                 
typedef void (*v_func_t)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...)  ;                 

//  <KTYPE of processing> <KCONFIGKEY> <function implementation>
#define K_TRATA_CMDNDX   0
#define K_TRATA_CMDKEY   1
#define K_TRATA_CMDFUNC  2

extern k_trata_dict K_TRATA_CALL[] ; 
         
// Silence timeout for detecting AT +++ break.
//
#define SILENCETIMEOUT  1000

// Config Sections
extern char s_atResponsesSTR[];
extern char s_atResponsesOK[];
extern char s_atDFLReg[];
void configResponseSTR(int K_cfg );
void configResponseOK(int K);
void configRegisters(int K);

extern  jsmn_parser jsmn_config_SRC;
#define CONFIGTOKS   2048
extern  jsmntok_t jsmn_configTOKs[]; /* We expect <<CONFIGTOKS>> JSON tokens */

//  Tabela de comandos implementados.
extern struct atCmds *cmdsTBL;    /* important! initialize to NULL */



#endif

