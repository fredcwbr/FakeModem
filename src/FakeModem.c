#include "global.h"
#include "FakeModem.h"
#include "mdmMM.h" 
#include "configFromJson.h"
#include "atTreatCmds.h"

#include "jsmninc.h"

//  Ref:: https://pokristensson.com/strmap.html
//  https://troydhanson.github.io/uthash/userguide.html
#include "uthash.h"

// Returns always 8 tokens... 

regmatch_t rm[RMSZ];

char errBuff[ERRBFSZ];
char line[1024];
regex_t re;

jsmn_parser jsmn_config_SRC;
#define CONFIGTOKS   2048
jsmntok_t jsmn_configTOKs[CONFIGTOKS]; /* We expect <<CONFIGTOKS>> JSON tokens */


void dummyProc(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
    

// State machine ..
enum mdmSTATE modemState;
         
k_trata_dict K_TRATA_CALL[] = { 
        { F_CMD_ATI, "F_ATI",  f_trata_ATI },
        { F_CMD_ATD, "F_ATD",  f_trata_ATD },
        { F_CMD_ATA, "F_ATA",  f_trata_ATA },
        { F_CMD_ATO, "F_ATO",  f_trata_ATO },
        { F_CMD_ATH, "F_ATH",  f_trata_ATH },
        { F_CMD_ATS, "F_ATS",  f_trata_ATS },
        { F_CMD_ATE, "F_ATE",  f_trata_ATE },
        { F_CMD_ATZ, "F_ATZ",  f_trata_ATZ },
        { F_CMD_ATZ, "F_OK",   f_trata_OK },
        { F_CMD_ERROR, "F_ERROR",   f_trata_ERROR },
        { F_INVALIDA, "F_+++", f_trata_ABORT },  // Keep at end of list
        { F_NULL , "", f_trata_ERROR }
    }  ;
 

// Silence timeout for detecting AT +++ break.
//

jsmn_parser jsmn_config_SRC;
jsmntok_t jsmn_configTOKs[CONFIGTOKS]; /* We expect <<CONFIGTOKS>> JSON tokens */

struct atCmds *cmdsTBL = NULL;    /* important! initialize to NULL */


int main(int argc, char **argv)
{
    FILE *fp;
    int  fconfig;

    char *p1;
    char *p0;
    char wkBf[128];
    int r;
    int src;
    struct stat finfo;
    s_CMD_IN_t *pxR ;
    char c;
    
    int arg_ndx;
    
    while((c=getopt( argc,argv,"d:s:")) != -1)
        switch(c) {
            case 'd':  // Set debug level 
                printDebugLVL = atoi(optarg);
                break;

            case 's':  // Set memory mapped Display  sharing file name
                mmDisplayFileName = optarg;
                break;
            default:

            abort();
        }
   
    fdebug = stderr;
 
    f_mdmDsplyInit();  // Init memory mapped IPC for modem GUI/STATUS
    
    fconfig = open("config.json", O_RDONLY );
    fstat(fconfig, &finfo);
    bfConfig = malloc(finfo.st_size+1);
    src = read(fconfig, bfConfig ,finfo.st_size);
    bfConfig[src] = 0;
    close(fconfig);
    
    //  https://zserge.com/jsmn/
    jsmn_init(&jsmn_config_SRC);
    // "src" is the char array holding the json content
    r = jsmn_parse(&jsmn_config_SRC, bfConfig, strlen(bfConfig), jsmn_configTOKs, CONFIGTOKS); 
    DEBUG_PRINTF( 10, "config read %d Tokens\n", r);
    kyCfg();
    dump(bfConfig, jsmn_configTOKs, jsmn_config_SRC.toknext, 0);
    
    if (optind < argc )
        filename = argv[optind];

    if (regcomp(&re, tofind, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex '%s'\n", tofind);
        return EXIT_FAILURE;
    }
    DEBUG_PRINTF( 15, "Regex: %s\n", tofind);
    DEBUG_PRINTF( 15, "Number of captured expressions: %zu\n", re.re_nsub);

    fp = fopen(filename, "r");
    if (fp == 0)
    {
        fprintf(stderr, "Failed to open file %s (%d: %s)\n", filename, errno, strerror(errno));
        return EXIT_FAILURE;
    }
    DEBUG_PRINTF( 5, "Arqin %s\n" , filename);

    while ((fgets(line, 1024, fp)) != NULL)
    {
        int le = strlen(line);
        // Le ate um \n ...  clear all \r(s) .. 
        for( p1 = line; *p1 ; ++p1 ) { 
            if( *p1 == '\r' ) *p1 = ' ';
        }
        // Se comecar com AT ... , interessa., 
        if( strncasecmp(line , "AT", 2) != 0 ) {
          // Todo comando comeca com  AT ...   
          continue;   
        }
        for( int ndx = 2 ; ndx < le ; ) {
            // Termos caracteres nessa linha... .
            int ndx2=strcspn( &line[ndx], "\n");
            line[ndx+ndx2] = 0;
            for( pxR = regexProc( &line[ndx] ); pxR ; )  {
                // Processar o comando ... 
                DEBUG_PRINTF( 15, "%s %d : Comando interpretado : %s : %s :: %s ; %s\n",
                    __func__,
                    __LINE__,
                    pxR->cmdBase,
                    pNullDeref( pxR->cmdFlag ) ,
                    pNullDeref( pxR->cmdAction ),
                    pNullDeref( pxR->cmdValue )
                    );
                    
                processInput( pxR );
     
                s_CMD_IN_t *pxW = pxR;
                pxR=pxR->p_next;
                free(pxW);
            };           
            
            //
            ndx += ndx2+1;
        }
    } 
    return EXIT_SUCCESS;
}



//  https://troydhanson.github.io/uthash/userguide.html
//typedef struct atCmds {
//    int cmd_id;                    /* key */
//    char CMDpfx[5];            /* AT commands are prefix+CMD+MOD :: charALT + charCMDCODE + charMODIFIER */
//                               /* Ex.:  DT ;; &C ;; %K  */ /
//    enum EcmdRC RC; 
//    int  cmdProcess;/
//    int  reg_value;
//    char **s_responses;
//    //
//    UT_hash_handle hh;         /* makes this structure hashable */
//} atCmds;

void dummyProc(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...) {
    DEBUG_PRINTF( 10, "DummyProcess  called\n");
};

/*
    ATRC_OK     = cmd_OK,
    ATRC_IDLE   = cmd_IDLE,
    ATRC_TRAIN  = cmd_TRAINING,
    ATRC_RING   = cmd_RING,
    ATRC_CNNCT  = cmd_CONNECT,
    ATRC_STR    = cmd_STRING,
    ATRC_REG    = cmd_REGISTER,
    ATRC_ERROR  = cmd_ERROR

    ->RC = 
*/


v_func_t configResponseFunction( char *F_type, ... ) {
     
	/* 
	 * typedef struct  {
     * enum E_FUNC_AT ky;
     * char *nome;
     * void (*func)(atCmds *atCmd , ...);
	 */
    k_trata_dict *p;
        
    for( p = K_TRATA_CALL; 
		  strcmp(p->nome, F_type ) && p->ky != F_INVALIDA ;
		  ++p ) {} ;
    return p->func;
}


void configResponseSTR(int K ){
    // Incluir respostas com funcoes atribuidas.,
    // e resultados que alterem o estado da maquina.
    // char *F_type, atCmds *vl 
    
    jsmntok_t *tokx,*tokf;
    int J, M, N, T;
    char wx[SZ_ATCMD_HASHKEY];
    char **Vls;
    v_func_t func;
    char *F_type;
    
    T = (jsmn_configTOKs + K + 1 )->size + 2;
    DEBUG_PRINTF( 10, "configResponseSTR K %d  \n", T );

    M  = K+2;
    for( J=0; J < T ; ++J ) {
        tokx = jsmn_configTOKs + M;  
        strncpy(wx, bfConfig + tokx->start, SZ_ATCMD_HASHKEY );
        if((tokx->end - tokx->start) >= SZ_ATCMD_HASHKEY ){
            DEBUG_PRINTF( 15, "Erro no ATCMD_OK  HashKey %d > %d\n", tokx->end - tokx->start, SZ_ATCMD_HASHKEY );
        } else {
            wx[tokx->end - tokx->start] = 0;
            
            Vls = strArray( M+2 );  // primeiro array com os dados de resposta
            // Segundo elemento é o tipo de funcao que trata o comando
            N = M+2;
            jsmn_nested_skip(jsmn_configTOKs, jsmn_config_SRC.toknext, &N ) ; 
            
            tokf = jsmn_configTOKs+N;
            F_type = malloc( (tokf->end - tokf->start) + 1);
            memset(F_type,0,(tokf->end - tokf->start) + 1);
            strncpy( F_type, bfConfig + tokf->start, (tokf->end - tokf->start) );
            func = configResponseFunction( F_type );
            
            add_CMDResponseSTR(wx, func, Vls  );
            // add_CMDResponseSTR(const char *cmd, v_func_t func, char **Vls  );
            free(F_type);
        }

        DEBUG_PRINTF( 15, "CFGTok : '%s' :: %d :: Sz %d \n",wx, J,  strlen(wx)   );
        ++M;
        jsmn_nested_skip(jsmn_configTOKs, jsmn_config_SRC.toknext, &M ) ; 
    }

    // configResponseFunction( F_type, vl );
    
        
};

void configResponseOK(int K){
    jsmntok_t *tokx;
    int J, T;
    char wx[SZ_ATCMD_HASHKEY];
    
    T = (jsmn_configTOKs + K + 1 )->size + 2;
    DEBUG_PRINTF( 10, "ConfigResponseOK K %d  \n", T );

    for( J=0; J < T ; ++J ) {
        tokx = jsmn_configTOKs + K + J;  
        strncpy(wx, bfConfig + tokx->start, SZ_ATCMD_HASHKEY );
        if((tokx->end - tokx->start) >= SZ_ATCMD_HASHKEY ){
            DEBUG_PRINTF( 15, "Erro no ATCMD_OK  HashKey %d > %d\n", tokx->end - tokx->start, SZ_ATCMD_HASHKEY );
        } else {
            wx[tokx->end - tokx->start] = 0;
            add_CMDOK( wx );
        }
        DEBUG_PRINTF( 15, "CFGTok : '%s' :: %d :: Sz %d \n",wx, J,  strlen(wx)   );
    }

};


char **strArray(  int K ){
    char **pArr;
    char *wp0;
    int  w_sz;
    int  pdspl;
    jsmntok_t *itok;
    jsmntok_t *ntok;
    
    ntok =  jsmn_configTOKs + K;
    pdspl = (ntok->size+1)*sizeof(void *);
    w_sz = (ntok->end - ntok->start) + ntok->size + pdspl;
    pArr = (char**)malloc(w_sz);    // Area interna
    // Copia o buffer para a area interna.
    wp0 =( (char *)pArr + pdspl);  // start of internal work area
    DEBUG_PRINTF( 35, "Alloc em %p ; Adicionando .. %d strings %.*s\n", pArr, ntok->size ,(ntok->end - ntok->start), bfConfig + ntok->start );
    for( int J=0 ; J < ntok->size ; ++J ){
        // Para cada string.
        pArr[J] = wp0;
        itok = jsmn_configTOKs + K + J + 1;
        int isz = itok->end - itok->start ;
        memcpy( wp0 , bfConfig + itok->start, isz ); // Buffer copiado., 
        *(wp0+isz)=0;
        DEBUG_PRINTF( 35, "Copiado %s em %p\n",wp0,wp0);        
        wp0+=isz+1;        
    } 
    pArr[ntok->size] = NULL;
    return( pArr );
}

u_regv_t cvtVl( int K) {
    static u_regv_t vl;
    char *p;
    char tok[32];
    int  ntoksz;
    jsmntok_t *ntok;
    char *wk; // workarea for string splitting, dictionary build, 
    
    ntok = jsmn_configTOKs + K;
    // Value type. .
    // 
    if(ntok->type == JSMN_ARRAY) {
        // Multiple string arrays., 
        //  n*P + strLen + delim
        //
        // Allocate n X (char *array) + sz of strings via malloc., 
        // cpy strings to internal area.,
        // 
        // for( int k=0; k < t->size ; ++k ) {
        vl.urv.pStrings = strArray( K );  
        vl.tpx = REGV_STRARR;
        
        int sz = ntok->end - ntok->start;
        DEBUG_PRINTF( 25, "JSMN_ARRAY %d,%d:%d em %p => %.*s \n",
                            ntok->start ,
                            ntok->end, 
                            sz, vl.urv.pStrings , 
                            sz, bfConfig + ntok->start );
        // }
        // 
    } else {
        // Int or n x CHAR
        // tok = jsmn_configTOKs + ntok;
        ntoksz = ntok->end - ntok->start;
        for( int i=0 ; i < 32; tok[i++] = 0 );
        strncpy( tok , bfConfig + ntok->start, (ntoksz>SZ_VLCMDUNION? SZ_VLCMDUNION : ntoksz )  );

        vl.urv.int_v = strtol( tok , &p, 0 );
        vl.tpx = REGV_INT;
        DEBUG_PRINTF( 25, "-> cvtVL strtol --> %ld ; errno = %d ; %s\n", __func__, __LINE__, vl.urv.int_v , errno, tok , (p == tok?"True":"False") );
        if( vl.urv.int_v == 0 && p == tok ) {
          // Verify .  if array of tokens,   
          // or string of chars., 
          strncpy( vl.urv.ch , tok , (32>SZ_VLCMDUNION? SZ_VLCMDUNION : 32 ) );
          vl.tpx = REGV_CHAR;
          DEBUG_PRINTF( 25, "->  cvtVL to %%%ds ..-->>> %.*s\n",__func__, __LINE__,  SZ_VLCMDUNION, SZ_VLCMDUNION, vl.urv.ch );
        }
    }  
   
    return vl;
}


void configRegisters(int K){
    jsmntok_t *tokx, *vltok;
    int J, T, N;
    char wx[32];
    char wx0[32];

    
    T = (jsmn_configTOKs + K + 1 )->size + 2;
    DEBUG_PRINTF( 25, "configRegisters K %d  \n", T );

    N = K + 2;
    for( J=0; J < T ; ++J ) {
        tokx = jsmn_configTOKs + N;  
        vltok = jsmn_configTOKs + N + 1; 
        strncpy(wx, bfConfig + tokx->start, 32 );
        wx[tokx->end - tokx->start] = 0;
        
        strncpy(wx0, bfConfig + vltok->start, 32 );
        wx0[vltok->end - vltok->start] = 0;
        
        DEBUG_PRINTF( 25, "-> CFGTok : '%s[%d;%d]' :: [N=%d] J=%d :: Sz %d ;; vl=>%s <<\n", wx,tokx->start, tokx->end, N, J,  strlen(wx) ,  wx0  );
        
        if((tokx->end - tokx->start) >= SZ_ATCMD_HASHKEY ){
            DEBUG_PRINTF( 25, "-> Erro no ATCMD_OK  HashKey %d > %d\n",   tokx->end - tokx->start, SZ_ATCMD_HASHKEY );
        } else {
            // convert whatever to Vl .,
            //  
            // Value can be: int ; or 0x00000000  (up to 4 hex bytes) ; or up to 4 char. 
            // or even an array of strings., 
            // 
            //
            add_CMDRegister( wx, cvtVl( N+1 ) );
        }
        int M = N+1;
        DEBUG_PRINTF( 25,  "-> K %d: T %d; Sz %d -->> M, N before %d ; %d  >>>  ",   K, T, vltok->size, M, N);
        if( vltok->size > 0 ) {   
           DEBUG_PRINTF( 25, "   vltokSZ .. %d ,  ", vltok->size );
           jsmn_nested_skip(jsmn_configTOKs, jsmn_config_SRC.toknext, &M ) ;  
           N=M;
        } else {
            N+=2;
        }
        DEBUG_PRINTF( 25,  "M, N after %d ; %d \n", M, N );
    }

};


void processInput( s_CMD_IN_t *p_cmd ){
    // lookup command and pass input to treat
    //
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    HASH_FIND_STR(cmdsTBL,p_cmd->cmdBase, s);  /* id already in the hash? */
    if( s == NULL ) {
        // tenta usando o modificador
        strncpy( wkey, p_cmd->cmdBase ,SZ_ATCMD_HASHKEY );
        strcat( wkey, p_cmd->cmdFlag );
        DEBUG_PRINTF( debugLVL, "%s ; %d -- %s\n",__func__, __LINE__, p_cmd->cmdBase, wkey );
        // strncpy( wkey, p_cmd->cmdBase, SZ_ATCMD_HASHKEY );
        HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    }
    
    if( s  == NULL ) {
        f_trata_ERROR( p_cmd, NULL );
    } else  {
        DEBUG_PRINTF( 25, "%s ; %d -- %s -> %p\n",__func__, __LINE__, s->CMDpfx ,s->func );
        s->func(p_cmd, s );
    }
    
};

