#ifndef __CONFIGFROMJSON_H__
#define __CONFIGFROMJSON_H__

#include "global.h"
#include "helper.h"

jsmntok_t *skip_token(jsmntok_t *token);
void jsmn_nested_skip(const jsmntok_t* tok, int num_tokens, int* i);

void kyCfg();

s_CMD_IN_t *regexProc(char *p0);
void delete_CMD(atCmds *s_cmd);
void add_CMDResponseSTR(const char *cmd, v_func_t func, char **Vls ) ;
void add_CMDRegister(const char *cmd, u_regv_t Vl ) ;
void add_CMDOK(const char *cmd) ;
    

#endif
