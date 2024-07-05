#ifndef _JSMNINC_H
#define _JSMNINC_H

#include "global.h"

#define JSMN_HEADER
#define JSMN_PARENT_LINKS 1
//  https://github.com/zserge/jsmn
#include "jsmn.h"

extern char **strArray(  int K );

extern char *bfConfig;
int dump(const char *js, jsmntok_t *t, size_t count, int indent);
void kyCfg();    
    

#endif
