
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

#define JSMN_PARENT_LINKS 1
//  https://github.com/zserge/jsmn
#include "jsmn/jsmn.h"
char *bfConfig;
int dump(const char *js, jsmntok_t *t, size_t count, int indent);
void kyCfg();    
    

//  Ref:: https://pokristensson.com/strmap.html
//  https://troydhanson.github.io/uthash/userguide.html
#include "uthash/src/uthash.h"

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

//  2 (baseCommand) , 5 (flags) ,  7 (action) , 8 (value)
typedef struct s_CMD_In {
    struct s_CMD_In *p_next;
    char *cmdBase;
    char *cmdFlag;
    char *cmdAction; //  ? or =
    char *cmdValue;
    char bf;
} s_CMD_IN_t;

s_CMD_IN_t *regexProc(char *p0);



#define tofind    " ?((([&\\+%]?) ?([a-zA-Z]+))([0-9|;]*|[;]?)(([=?])?([0-9]*))?)"

// Returns always 8 tokens... 
#define RMSZ  9
regmatch_t rm[RMSZ];

#define ERRBFSZ   1024
char errBuff[ERRBFSZ];
char line[1024];
regex_t re;



//  Funcion / states  
// response group
#define  cmd_OK         0x0001
#define  cmd_IDLE       0x0002
#define  cmd_CONNECT    0x0008
// external line disc group
#define  cmd_TRAINING   0x0010
#define  cmd_RING       0x0020
// information/config group
#define  cmd_STRING     0x0100
#define  cmd_REGISTER   0x0200
#define  cmd_ERROR      0x8000


enum EcmdRC  {
    ATRC_OK     = cmd_OK,
    ATRC_IDLE   = cmd_IDLE,
    ATRC_TRAIN  = cmd_TRAINING,
    ATRC_RING   = cmd_RING,
    ATRC_CNNCT  = cmd_CONNECT,
    ATRC_STR    = cmd_STRING,
    ATRC_REG    = cmd_REGISTER,
    ATRC_ERROR  = cmd_ERROR
    };

// Fake Modem AT commands configuration
enum tpREGV {REGV_INVALID, REGV_CHAR, REGV_INT, REGV_STRARR};
//  https://troydhanson.github.io/uthash/userguide.html
#define SZ_REGVCHAR  sizeof(unsigned long int)
union u_regv {
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

void dummyProc(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
    

// State machine ..
enum mdmSTATE  {COMMAND,DIALING,TRAINING,RINGING,ONLINE,ERROR};
enum mdmSTATE modemState;


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

enum E_FUNC_AT { F_CMD_ATI,
                 F_CMD_ATD,
                 F_CMD_ATA,
                 F_CMD_ATO,
                 F_CMD_ATH,
                 F_CMD_ATS,
                 F_CMD_ATE,
                 F_CMD_ATZ,
                 F_CMD_OK,
                 F_CMD_ERROR,
                 F_INVALIDA,
                 F_NULL = -1}; // End of list Marker...
typedef struct  {
    enum E_FUNC_AT ky;
    char *nome;
    void (*func)(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
    } k_trata_dict;

//  <KTYPE of processing> <KCONFIGKEY> <function implementation>
#define K_TRATA_CMDNDX   0
#define K_TRATA_CMDKEY   1
#define K_TRATA_CMDFUNC  2
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
 

// chat(8) return codes emulation
#define chatRC_OK           0
#define chatRC_INVALIDPRM   1
#define chatRC_FAIL         2
#define chatRC_TIMEOUT      3
#define chatRC_ABORT        4

// chatABORT accepts multiple reasons., 
//  
enum chatRC  {
    eChatRC_OK = chatRC_OK,
    eChatRC_INVLD = chatRC_INVALIDPRM,
    eChatRC_FAIL = chatRC_FAIL,
    eChatRC_TO = chatRC_TIMEOUT,
    eChatRC_ABRT = chatRC_ABORT
    };

// Chat Abort extended reasons... 
enum eChatABORTreason {
    caNOCARRY = 1,
    caNOSIGNAL = 2,
    caTIMEOUT = 3,
    caBUSY = 4,
    // Anything else
    caERROR = 5
    };

// Shared 
#define MAXMM_MDMNAME   128
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


void mdmDspUpdate(mdmDISPLAY *this_mdm);
char *modemFileName;
int maxndxmdm  = 8;
mdmDISPLAY *mdms;   // mmap modem displays array on file 
                    // NDXMDM shows this modem ., 

// Silence timeout for detecting AT +++ break.
//
#define SILENCETIMEOUT  1000

const char *filename = "x"; // /etc/rc.conf";

//
char s_atResponsesSTR[] = "atResponsesSTR";
char s_atResponsesOK[] = "atResponsesOK";
char s_atDFLReg[] = "atDFLReg";
void configResponseSTR(int K_cfg );
void configResponseOK(int K);
void configRegisters(int K);

jsmn_parser jsmn_config_SRC;
#define CONFIGTOKS   2048
jsmntok_t jsmn_configTOKs[CONFIGTOKS]; /* We expect <<CONFIGTOKS>> JSON tokens */


struct atCmds *cmdsTBL = NULL;    /* important! initialize to NULL */


void pprintAtCmds( const char* f, int ln , atCmds *p ){
    char *w;
    char *px;

    printf("->%s(%d): vindo de :: F(%s).L(%d) :\n",__func__,__LINE__,f,ln);
    printf("Comando : %s\n",p->CMDpfx);
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
                // printf("->%s(%d): I=> \n",__func__,__LINE__, I);                
            }
            *px++ = '\'';
            *px = 0;
            // printf("->%s(%d):\n",__func__,__LINE__);
            puts(w);
            free(w);
            break;

        case REGV_INT:
            printf("\tINT:: %ld\n", p->reg_value.urv.int_v);
            break;
                   
        case REGV_STRARR:
            printf("\tSTRARR:: at %p\n", p->reg_value.urv.pStrings );
            for( char **px = p->reg_value.urv.pStrings ; *px ; px++ ) {
                putchar('\t');
                puts(*px);
            }                   
            break;
            
            
        // case REGV_INVALID:
        default:
            printf("\tREG:: INVALID VALUE\n");
    }
    for( k_trata_dict *p_kc = &K_TRATA_CALL[0]; p_kc->ky != F_NULL ; ++p_kc){
        // lista a funcao que trata esse comando
        if( p_kc->func == p->func ) {
            printf("comando tratado por %s \n",p_kc->nome );            
        }
    }
    
}


void add_CMDOK(const char *cmd) {
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    strncpy( wkey, cmd, SZ_ATCMD_HASHKEY );
    HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    if (s == NULL) {
        printf("Added new key %s na hash table \n",wkey);
        s = malloc(sizeof *s);
        strncpy( s->CMDpfx, cmd, SZ_ATCMD_HASHKEY );
        HASH_ADD_STR(cmdsTBL, CMDpfx, s);  /* id: name of key field */
    } else {
        printf("Key FOUND %s na hash table \n",wkey);
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
    printf("->%s(%d): Added key %s ;",__func__,__LINE__, wkey);
    pprintAtCmds( __func__, __LINE__ , s );
}

void add_CMDResponseSTR(const char *cmd, char **Vls ) {
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
    s->reg_value.urv.pStrings = Vls;
    s->RC = ATRC_OK;
    printf("Added key %s na hash table \n",wkey);
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
    
    //  Interested only on tokens
    //  2 (baseCommand) , 5 (flags) ,  7 (action) , 8 (value)
    //  always copy up to bufer., 
    //
    
    while( ((retval = regexec(&re, p0, RMSZ, rm, 0)) == 0) ) 
    {
        printf("<<%s>>\n", p0 );
        // Complete match
        printf("Line: <<%.*s>>\n", (int)(rm[0].rm_eo - rm[0].rm_so), p0 + rm[0].rm_so);
        // Match captured in (...) - the \( and \) match literal parenthesis
        // espaco o bastante para copiar cada coisa no seu lugar
        pwk = (s_CMD_IN_t *)malloc(sizeof(s_CMD_IN_t) + (int)(rm[0].rm_eo - rm[0].rm_so) + 9 );
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
                printf("ndx %d == -1\n",i);
            } else {
                tsz = rm[i].rm_eo - rm[i].rm_so;
                switch(i){
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
                    case 2:
                    case 5:
                    case 7:
                    case 8:
                        px = stpncpy(px, p0 + rm[i].rm_so, tsz);
                        *px++ = 0;
                }
                printf("\tText[%d]: <<%.*s>>\n", i, tsz, p0 + rm[i].rm_so);
            }
        }
        p0 = p0 + (int)(rm[0].rm_eo - rm[0].rm_so);
    } 
    regerror( retval , &re , errBuff, ERRBFSZ );
    printf( "Erro no regex %s\n", errBuff );
    return p_ini;
}

char *pNullDeref( char *p ) {
    if( p ) {
        return p;
    }
    else {
        return "NULO";
    }
}

int main(int argc, char **argv)
{
    FILE *fp;
    int  fconfig;
    int  mmMDMDisplay;
    mdmDISPLAY *this_mdm;
    char *p1;
    char *p0;
    char wkBf[128];
    int r;
    int src;
    struct stat finfo;
    s_CMD_IN_t *pxR ;
    
    modemFileName = "";
 
    mmMDMDisplay = open("mmMDMx.dsply",
                            O_RDWR|O_CREAT ,
                            S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP );
    lseek(mmMDMDisplay, sizeof(mdmDISPLAY)*8,SEEK_SET);
    mdms = (mdmDISPLAY *)mmap(
            NULL,
            sizeof(mdmDISPLAY)*maxndxmdm,
            PROT_READ|PROT_WRITE,
            MAP_SHARED|MAP_FILE|MAP_POPULATE,
            mmMDMDisplay,
            0);
    if( mdms == (void *)-1 ){
        printf("Erro no mm %s\n",strerror(errno) );
    }
    ftruncate(mmMDMDisplay,sizeof(mdmDISPLAY)*maxndxmdm);
    close(mmMDMDisplay);
    
    modemFileName = wkBf;
    for( int i = 0; i < maxndxmdm ; ++i) {
        sprintf(wkBf,"/dev/tnt/mdmTeste%d",i);
        this_mdm = mdms + i;
        mdmDspUpdate(this_mdm);
    }
    
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
    printf("config read %d Tokens\n", r);
    kyCfg();
    dump(bfConfig, jsmn_configTOKs, jsmn_config_SRC.toknext, 0);
    
    if (argc > 1)
        filename = argv[1];

    if (regcomp(&re, tofind, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex '%s'\n", tofind);
        return EXIT_FAILURE;
    }
    printf("Regex: %s\n", tofind);
    printf("Number of captured expressions: %zu\n", re.re_nsub);

    fp = fopen(filename, "r");
    if (fp == 0)
    {
        fprintf(stderr, "Failed to open file %s (%d: %s)\n", filename, errno, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("Arqin %s\n" , filename);

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
                printf("Comando interpretado : %s : %s :: %s ; %s\n",
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
jsmntok_t *skip_token(jsmntok_t *token);
void jsmn_nested_skip(const jsmntok_t* tok, int num_tokens, int* i);

void kyCfg(){
    // const char *js, jsmntok_t *t, size_t count, int indent;
    jsmntok_t *t,*key_t,*tokx;
    int K=1,J=0;
    
    t = jsmn_configTOKs;
    J = t->size;
    printf("Raiz %d\n" , t->size );
    do {        
        tokx = jsmn_configTOKs + K;
        if( tokx->type == JSMN_PRIMITIVE || tokx->type == JSMN_STRING ) {
            // 'atResponsesSTR'
            // 'atResponsesOK'
            // 'atDFLReg'
            char *sstart = bfConfig + tokx->start;
            int lenx = tokx->end - tokx->start;
            if( strncmp(s_atResponsesOK,sstart,strlen(s_atResponsesOK) ) == 0 ){
                //
                configResponseOK(K);
            } else if( strncmp(s_atDFLReg,sstart,strlen(s_atDFLReg) ) == 0 ){
                configRegisters(K);
            }
            printf("->%s(%d)%d :: Sz %d : '%.*s'\n", __func__,__LINE__, K, tokx->size, lenx, sstart  );
            --J;            
        }
        jsmn_nested_skip(jsmn_configTOKs,jsmn_config_SRC.toknext,&K);
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
void
jsmn_nested_skip(const jsmntok_t* tok, int num_tokens, int* i)
{
    printf("\n%s(%d) Iniciando: Limit %d :: [K=%d] -> nextstart %d, end %d\n",__func__,__LINE__, num_tokens, *i, tok[*i].end, tok[*i].start );
    
    for (int char_end = tok[*i].end; *i < num_tokens && tok[*i].start < char_end; (*i)++) {
       // printf("%s(%d) Loop: Limit %d :: [K=%d] -> nextstart %d, end %d\n",__func__,__LINE__, num_tokens, *i, char_end, tok[*i].start );
    }  // pass
}

int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    printf("%.*s", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    printf("'%.*s'", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    printf("Object SZ %d\n",t->size);
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
        printf("  ");
      }
      key = t + 1 + j;
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        printf(": ");
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
      printf("\n");
    }
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    printf("Array SZ %d\n",t->size);
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
        printf("  ");
      }
      printf("   - ");
      j += dump(js, t + 1 + j, count - j, indent + 1);
      printf("\n");
    }
    return j + 1;
  }
  return 0;
}


void mdmDspUpdate(mdmDISPLAY *this_mdm)  {
    
    this_mdm->RX  = '0';
    this_mdm->TX  = '0';
    this_mdm->DCD = '0';
    this_mdm->RTS = '0';
    this_mdm->CTS = '0';
    this_mdm->DTR = '0';
    this_mdm->DSR = '0';
    this_mdm->mdmSt = '0';
    strncpy( this_mdm->mdmname , modemFileName, MAXMM_MDMNAME -1 );

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
    printf("DummyProcess  called\n");
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


void configResponseFunction( char *F_type, atCmds *vl ) {
     
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
    vl->func =  p->func;

}

void configResponseSTR(int K_cfg ){
    // Incluir respostas com funcoes atribuidas.,
    // e resultados que alterem o estado da maquina.
    // char *F_type, atCmds *vl 
    
    // configResponseFunction( F_type, vl );
        
};

void configResponseOK(int K){
    jsmntok_t *tokx;
    int J, T;
    char wx[SZ_ATCMD_HASHKEY];
    
    T = (jsmn_configTOKs + K + 1 )->size + 2;
    printf("ConfigResponseOK K %d  \n", T );

    for( J=0; J < T ; ++J ) {
        tokx = jsmn_configTOKs + K + J;  
        strncpy(wx, bfConfig + tokx->start, SZ_ATCMD_HASHKEY );
        if((tokx->end - tokx->start) >= SZ_ATCMD_HASHKEY ){
            printf("Erro no ATCMD_OK  HashKey %d > %d\n", tokx->end - tokx->start, SZ_ATCMD_HASHKEY );
        } else {
            wx[tokx->end - tokx->start] = 0;
            add_CMDOK( wx );
        }
        printf("CFGTok : '%s' :: %d :: Sz %d \n",wx, J,  strlen(wx)   );
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
    wp0 = (char *)(pArr + pdspl);  // start of internal work area
    printf("Alloc em %p ; Adicionando .. %d strings %.*s\n", pArr, ntok->size ,(ntok->end - ntok->start), bfConfig + ntok->start );
    for( int J=0 ; J < ntok->size ; ++J ){
        // Para cada string.
        pArr[J] = wp0;
        itok = jsmn_configTOKs + K + J + 1;
        int isz = itok->end - itok->start ;
        memcpy( wp0 , bfConfig + itok->start, isz ); // Buffer copiado., 
        *(wp0+isz)=0;
        printf("Copiado %s em %p\n",wp0,wp0);        
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
        printf("JSMN_ARRAY %d,%d:%d em %p => %.*s \n",
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
        printf("->%s(%d) cvtVL strtol --> %ld ; errno = %d ; %s\n", __func__, __LINE__, vl.urv.int_v , errno, tok , (p == tok?"True":"False") );
        if( vl.urv.int_v == 0 && p == tok ) {
          // Verify .  if array of tokens,   
          // or string of chars., 
          strncpy( vl.urv.ch , tok , (32>SZ_VLCMDUNION? SZ_VLCMDUNION : 32 ) );
          vl.tpx = REGV_CHAR;
          printf("->%s(%d)  cvtVL to %%%ds ..-->>> %.*s\n",__func__, __LINE__,  SZ_VLCMDUNION, SZ_VLCMDUNION, vl.urv.ch );
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
    printf("configRegisters K %d  \n", T );

    N = K + 2;
    for( J=0; J < T ; ++J ) {
        tokx = jsmn_configTOKs + N;  
        vltok = jsmn_configTOKs + N + 1; 
        strncpy(wx, bfConfig + tokx->start, 32 );
        wx[tokx->end - tokx->start] = 0;
        
        strncpy(wx0, bfConfig + vltok->start, 32 );
        wx0[vltok->end - vltok->start] = 0;
        
        printf("->%s(%d) CFGTok : '%s[%d;%d]' :: [N=%d] J=%d :: Sz %d ;; vl=>%s <<\n", __func__,__LINE__,wx,tokx->start, tokx->end, N, J,  strlen(wx) ,  wx0  );
        
        if((tokx->end - tokx->start) >= SZ_ATCMD_HASHKEY ){
            printf("->%s(%d) Erro no ATCMD_OK  HashKey %d > %d\n",  __func__,__LINE__, tokx->end - tokx->start, SZ_ATCMD_HASHKEY );
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
        printf( "->%s(%d) K %d: T %d; Sz %d -->> M, N before %d ; %d  >>>  ",  __func__,__LINE__, K, T, vltok->size, M, N);
        if( vltok->size > 0 ) {   
           printf("   vltokSZ .. %d ,  ", vltok->size );
           jsmn_nested_skip(jsmn_configTOKs, jsmn_config_SRC.toknext, &M ) ;  
           N=M;
        } else {
            N+=2;
        }
        printf( "M, N after %d ; %d \n", M, N );
    }

};


void processInput( s_CMD_IN_t *p_cmd ){
    // lookup command and pass input to treat
    //
    char   wkey[SZ_ATCMD_HASHKEY];
    struct atCmds *s;
    
    printf("%s ; %d -- %s\n",__func__, __LINE__, p_cmd->cmdBase );
    strncpy( wkey, p_cmd->cmdBase, SZ_ATCMD_HASHKEY );
    HASH_FIND_STR(cmdsTBL,wkey, s);  /* id already in the hash? */
    
    if( s  == NULL ) {
        f_trata_ERROR( p_cmd, NULL );
    } else  {
        printf("%s ; %d -- %s -> %p\n",__func__, __LINE__, s->CMDpfx ,s->func );
        s->func(p_cmd, s );
    }
    
};


void f_trata_OK(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...){
    // Devolve um OK ., 
    // pode receber um valor no comando 
    // que é a variavel tipo 0 / 1
    // e modifica o comando como TRUE ou False.,
    // num modem verdadeiro isso modifica o 
    // comportamento, no nosso fake ., 
    // alguns casos vamos tratar, 
    // especialmente o V0/1 ., que
    // altera o modo verbose., dos codigos
    // de retorno ., 
    //
    // ** do ponto de vista da maquina de estados,. 
    // nao tem alteracao 
	
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    
	if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        printf("\rOK\r\n");
    }
	
};

void f_trata_ATI(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...){
    // Devolve as strings de identificacao., 
    // de acordo com  o padrao HAEYES de 
    // a
    // b
    // r1|3 ., 
    // Essas strings sao bitmaps codificados.,
    // para simplificar., vem do arquivo de configuracao
    // e servem para auto deteccao/identificacao  do modem., 
    // ** do ponto de vista da maquina de estados,. 
    // nao tem alteracao 
     printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
     
};
void f_trata_ATD(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    //
    //  ocorre em um SHM IPC comum, 
    //  que tem a sinalizacao entre processos distintos., 
    //   
    // ** do ponto de vista da maquina de estados,. 
    // entra nos modos de 
    // DIALING, TRAINING , 
    //   e eventualmente  pode terminar em :
    //     CONNECT - se houver resposta
    //     NO-CARRIER - se nao houver resposta (time-out)...
    //     BUSY  - se a resposta nao for ACK . 
    // ==
    //  Neste modelo de implementacao virtual/emulada,
    //   o VFakeModem da contra parte recebe a variavel 
    //   do ATD ,  devolve uma das tres respostas;
    //     CONNECT <<--  se estiver disponivel (pode ser por ATA ou Auto Atendimento 
    //     NO-CARRIER  <<-- se a contra-parte nao existir., pois sairá por timeout
    //     BUSY <<-- se a contra-parte ja estiver conectada.,
    // == 
    //  Cada contra-parte é um vMdm<NNN> onde NNN é o numero de destino ., 
    //  o numero de vMDMs, é instanciado pelo arquivo de configuracao 
    //      **** TODO ****  --?? por IOCTL ??? (quem sabe)
     printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    
};
void f_trata_ATA(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
     printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
void f_trata_ATO(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
     printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
 
void f_trata_ATH(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
void f_trata_ATS(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
void f_trata_ATE(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
void f_trata_ATZ(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};
void f_trata_ERROR(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    ;
}
void f_trata_ABORT(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};

