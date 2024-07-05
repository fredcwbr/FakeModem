
#include "global.h"

#ifndef __HELPER_HINC
#define __HELPER_HINC

extern int debugLVL ;
extern int printDebugLVL ;
extern FILE *fdebug;

char *pNullDeref( char *p );
void debug_printf( int debugLvl, char *format, ...);
void pprintAtCmds( const char* f, int ln , atCmds *p );


#endif
