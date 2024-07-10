#ifndef __CONFIGFROMJSON_H__
#define __CONFIGFROMJSON_H__

#include "global.h"
#include "helper.h"

jsmntok_t *skip_token(jsmntok_t *token);
void jsmn_nested_skip(const jsmntok_t* tok, int num_tokens, int* i);

void kyCfg();

s_CMD_IN_t *regexProc(char *p0);
void delete_CMD(atCmds *s_cmd);
int add_CMDResponseSTR(const char *cmd, v_func_t func, char **Vls ) ;
int add_CMDRegister(const char *cmd, u_regv_t Vl ) ;
int add_CMDOK(const char *cmd) ;
    
    
int configResponseOK(int K);
int  configRegisters(int K);
int configResponseSTR(int K);

#endif
