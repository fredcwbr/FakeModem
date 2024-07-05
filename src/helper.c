
#include "helper.h"
#include "global.h"
#include "FakeModem.h"
#include <ctype.h>
#include <stdarg.h>


int debugLVL = 0;
int printDebugLVL = 99;
FILE *fdebug;


char *pNullDeref( char *p ) {
    if( p ) {
        return p;
    }
    else {
        return "NULO";
    }
}



void debug_printf( int debugLvl, char *format, ...){
    if( debugLvl < printDebugLVL ){
        va_list ap;
        va_start(ap, format);
        
        vfprintf( fdebug, format , ap );
        
        va_end(ap);
    }
}


void pprintAtCmds( const char* f, int ln , atCmds *p ){
    char *w;
    char *px;

    debug_printf( 25, "->%s(%d): vindo de :: F(%s).L(%d) :\n",__func__,__LINE__,f,ln);
    debug_printf( 15, "Comando : %s\n",p->CMDpfx);
    switch( p->reg_value.tpx) {        
        case REGV_CHAR: 

            w = malloc(1024);  
            px = w;
            strcpy(px,"\tCHAR:: '");
            px = strlen(w) + w +1;
            for( int I ; I < SZ_REGVCHAR ; ++I ) {
                if( isprint(p->reg_value.urv.ch[I] ) )
                   *px++ = p->reg_value.urv.ch[I];                 
                else {
                   int x = sprintf(px,"<0x%2.2X>", p->reg_value.urv.ch[I] );
                   px +=x+1; 
                }
                // debug_printf( debugLVL, "->%s(%d): I=> \n",__func__,__LINE__, I);                
            }
            *px++ = '\'';
            *px = 0;
            // debug_printf( debugLVL, "->%s(%d):\n",__func__,__LINE__);
            puts(w);
            free(w);
            break;

        case REGV_INT:
            debug_printf( 20, "\tINT:: %ld\n", p->reg_value.urv.int_v);
            break;
                   
        case REGV_STRARR:
            debug_printf( 20, "\tSTRARR:: at %p\n", p->reg_value.urv.pStrings );
            for( char **px = p->reg_value.urv.pStrings ; *px ; px++ ) {
                putchar('\t');
                puts(*px);
            }                   
            break;
            
            
        // case REGV_INVALID:
        default:
            debug_printf( 20, "\tREG:: INVALID VALUE\n");
    }
    for( k_trata_dict *p_kc = &K_TRATA_CALL[0]; p_kc->ky != F_NULL ; ++p_kc){
        // lista a funcao que trata esse comando
        if( p_kc->func == p->func ) {
            debug_printf( 15, "comando tratado por %s \n",p_kc->nome );            
        }
    }
    
}

