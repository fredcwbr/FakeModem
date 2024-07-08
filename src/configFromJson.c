
#include "configFromJson.h"
#include "atTreatCmds.h"


void kyCfg(){
    // const char *js, jsmntok_t *t, size_t count, int indent;
    jsmntok_t *t,*key_t,*tokx;
    int K=1,J=0;
    
    t = jsmn_configTOKs;
    J = t->size;
    DEBUG_PRINTF( 20, "Raiz %d\n" , t->size );
    do {        
        tokx = jsmn_configTOKs + K;
        char *sstart = bfConfig + tokx->start;
        int lenx = tokx->end - tokx->start;
        if( tokx->end < tokx->start ) {
            DEBUG_PRINTF( 25, "%s, %d : End < Start K=%d... End:%d  Start:%d \n" , __func__, __LINE__, K ,tokx->end , tokx->start);
            K += 1;
        } else if( tokx->type & (JSMN_PRIMITIVE | JSMN_STRING )  == (JSMN_PRIMITIVE | JSMN_STRING )  ) {
            // 'atResponsesSTR'
            // 'atResponsesOK'
            // 'atDFLReg'
            if( strncmp(s_atResponsesOK,sstart,strlen(s_atResponsesOK) ) == 0 ){
                //
                DEBUG_PRINTF( 35, "-> ADD OK --> K=%d :: Sz %d : '%.*s'\n",  K, tokx->size, lenx, sstart  );
                configResponseOK(K);
            } else if( strncmp(s_atDFLReg,sstart,strlen(s_atDFLReg) ) == 0 ){
                DEBUG_PRINTF( 35, "-> ADD REG --> K=%d :: Sz %d : '%.*s'\n",  K, tokx->size, lenx, sstart  );
                configRegisters(K);
            } else if( strncmp(s_atResponsesSTR ,sstart,strlen(s_atResponsesSTR) ) == 0 ) {
                DEBUG_PRINTF( 35, "-> ADD STR --> K=%d :: Sz %d : '%.*s'\n",  K, tokx->size, lenx, sstart  );
                configResponseSTR(K);
            }
            DEBUG_PRINTF( 25, "-> K=%d :: Primitiva ..  Sz %d : '%.*s'\n",  K, tokx->size, lenx, sstart  );
            --J;  
            ++K;
        } else if (t->type == JSMN_OBJECT) {
            DEBUG_PRINTF( 25, "%s, %d : Inside object K=%d... \n" , __func__, __LINE__, K );
            DEBUG_PRINTF( 25, "-> K=%d  :: Sz %d : '%.*s'\n",  K,  tokx->size, lenx, sstart  );
            K += 1;
        } else if( tokx->type == JSMN_ARRAY ) {
            DEBUG_PRINTF( 25, "%s, %d : Inside ARRAY ... \n" , __func__, __LINE__ );
            DEBUG_PRINTF( 25, "-> K=%d  :: Sz %d : '%.*s'\n",  K,  tokx->size, lenx, sstart  );
            K += 2;
        } else {       
            DEBUG_PRINTF( 25, "%s, %d : Skipping ... \n" , __func__, __LINE__ );
            DEBUG_PRINTF( 25, "->%d  :: Sz %d : '%.*s'\n",  K,  tokx->size, lenx, sstart  );
            jsmn_nested_skip(jsmn_configTOKs,jsmn_config_SRC.toknext,&K);
        }
    } while( K < jsmn_config_SRC.toknext  && J > 0 );
    
}



/* return next token, ignoring descendants */
jsmntok_t *skip_token(jsmntok_t *token)
{
	int pending = 1;
	do {
		pending += token->size - 1;
		token++;
	} while (pending);
	return token;
}

// `*i` will be the index of the next non-nested token, or the last token.
// works for any type of token
void jsmn_nested_skip(const jsmntok_t* tok, int num_tokens, int* i)
{
    DEBUG_PRINTF( 20, "\n Iniciando: Limit %d :: [K=%d] -> nextstart %d, end %d\n", num_tokens, *i, tok[*i].end, tok[*i].start );
    
    for (int char_end = tok[*i].end; *i < num_tokens && tok[*i].start < char_end; (*i)++) {
       // DEBUG_PRINTF( debugLVL, " Loop: Limit %d :: [K=%d] -> nextstart %d, end %d\n", num_tokens, *i, char_end, tok[*i].start );
    }  // pass
}


void add_CMDOK(const char *cmd) {
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    strncpy( wkey, cmd, SZ_ATCMD_HASHKEY );
    HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    if (s == NULL) {
        DEBUG_PRINTF( 25, "Added new key %s na hash table \n",wkey);
        s = malloc(sizeof *s);
        strncpy( s->CMDpfx, cmd, SZ_ATCMD_HASHKEY );
        HASH_ADD_STR(cmdsTBL, CMDpfx, s);  /* id: name of key field */
    } else {
        DEBUG_PRINTF( 25, "Key FOUND %s na hash table \n",wkey);
    }
    // s->cmdProcess = dummyProc;
    s->reg_value.urv.int_v = -1;
    s->RC = ATRC_OK;
    s->func =  f_trata_OK;
    
    pprintAtCmds( __func__, __LINE__ , s );
}



void add_CMDRegister(const char *cmd, u_regv_t Vl ) {
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    strncpy( wkey, cmd, SZ_ATCMD_HASHKEY );
    HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    if (s == NULL) {
        s = malloc(sizeof *s);
        strncpy( s->CMDpfx, cmd, SZ_ATCMD_HASHKEY );
        HASH_ADD_STR(cmdsTBL, CMDpfx, s);  /* id: name of key field */
    }
    // s->cmdProcess = dummyProc;
	s->func = dummyProc;
    s->reg_value = Vl ;
    s->RC = ATRC_OK;
    DEBUG_PRINTF( 15, "->: Added key %s ;", wkey);
    pprintAtCmds( __func__, __LINE__ , s );
}



void add_CMDResponseSTR(const char *cmd, v_func_t func, char **Vls ) {
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    strncpy( wkey, cmd, SZ_ATCMD_HASHKEY );
    HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    if (s == NULL) {
        s = malloc(sizeof *s);
        strncpy( s->CMDpfx, cmd, SZ_ATCMD_HASHKEY );
        HASH_ADD_STR(cmdsTBL, CMDpfx, s);  /* id: name of key field */
    }
    // s->cmdProcess = dummyProc;
	s->func = func;
    s->reg_value.urv.pStrings = Vls;
    s->RC = ATRC_OK;
    DEBUG_PRINTF( 15, "Added STR key %s na hash table Vls = %p \n", wkey, Vls );
    pprintAtCmds( __func__, __LINE__ , s );
}




void delete_CMD(atCmds *s_cmd) {
    HASH_DEL(cmdsTBL, s_cmd);  /* user: pointer to deletee */
    free(s_cmd);             /* optional; it's up to you! */
}



s_CMD_IN_t *regexProc(char *p0) {
    s_CMD_IN_t *p_next;
    s_CMD_IN_t *p_ini = NULL;
    
    int retval = 0;
    s_CMD_IN_t *pwk;
    char *px;
    int  tsz;
    int  flg_OK = 0;
    //  Interested only on tokens
    //  2 (baseCommand) , 5 (flags) ,  7 (action) , 8 (value)
    //  always copy up to bufer., 
    //
    
    while( ((retval = regexec(&re, p0, RMSZ, rm, 0)) == 0) ) 
    {
        flg_OK = 1;
        DEBUG_PRINTF( 25, "<<%s>>\n", p0 );
        // Complete match
        DEBUG_PRINTF( 25, "Line: <<%.*s>>\n", (int)(rm[0].rm_eo - rm[0].rm_so), p0 + rm[0].rm_so);
        // Match captured in (...) - the \( and \) match literal parenthesis
        // espaco o bastante para copiar cada coisa no seu lugar
        pwk = (s_CMD_IN_t *)malloc(sizeof(s_CMD_IN_t) + ((int)(rm[0].rm_eo - rm[0].rm_so) *2) + 9 );
        memset(pwk,0,sizeof(s_CMD_IN_t));
        px = &pwk->bf;
        pwk->p_next = NULL;
        if( p_ini == NULL ) {
            p_ini=pwk; // inicio da lista.
            p_next = pwk;
        } else {
            p_next->p_next = pwk;
            p_next = pwk;                    
        }
                
        for( int i = 1; i < RMSZ; ++i ){
            if( rm[i].rm_so == -1 ) {
                DEBUG_PRINTF( debugLVL, "ndx %d == -1\n",i);
            } else {
                tsz = rm[i].rm_eo - rm[i].rm_so;
                switch(i){
                    case 1:
                        pwk->cmdFull = px;
                        break;
                    case 2:
                        pwk->cmdBase = px;
                        break;
                    case 5:
                        pwk->cmdFlag = px;
                        break;
                    case 7:
                        pwk->cmdAction = px;
                        break;
                    case 8:
                        pwk->cmdValue = px;
                        break;
                }
                switch(i){
                    case 1:
                    case 2:
                    case 5:
                    case 7:
                    case 8:
                        px = stpncpy(px, p0 + rm[i].rm_so, tsz);
                        *px++ = 0;
                }
                DEBUG_PRINTF( 25, "\tText[%d]: <<%.*s>>\n", i, tsz, p0 + rm[i].rm_so);
            }
        }
        p0 = p0 + (int)(rm[0].rm_eo - rm[0].rm_so);
    } 
    if( !flg_OK ) {
        regerror( retval , &re , errBuff, ERRBFSZ );
        DEBUG_PRINTF( 20,  "%s %d  : Erro no regex %s\n", __func__, __LINE__, errBuff );
    }
    return p_ini;
}
