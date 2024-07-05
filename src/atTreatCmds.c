
#include "global.h"
#include "FakeModem.h"
#include "helper.h"



void f_trata_OK(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...) {
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
	
    debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    
	if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( debugLVL, "\rOK\r\n");
    }
	
};


void f_trata_ATI(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...){
    char **p;
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
     debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
     for( p = atCmd->reg_value.urv.pStrings ; *p ; ++p ) {
         debug_printf( 25, "\r%s\r\n", *p );
     }
     if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( 25, "\rOK\r\n");
     }
     
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
    debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( 25, "\rCONNECT\r\n");
    }
    
};


void f_trata_ATA(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
     debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
     if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( 25, "\rCONNECTED\rOK\r\n");
    }
};


void f_trata_ATO(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
     debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
     if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( 25, "\rCONNECTED\rOK\r\n");
    }
};

void f_trata_ATH(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        debug_printf( 25, "\rOK\r\n");
    }
};


void f_trata_ATS(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    debug_printf( 25, "%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
};

void f_trata_ATE(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        printf("\rOK\r\n");
    }
};

void f_trata_ATZ(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        printf("\rOK\r\n");
    }
};

void f_trata_ERROR(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s ; %s\n",__func__, __LINE__, cmdIn->cmdBase , cmdIn->cmdFull );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        printf("\rERROR\r\n");
    };
}

void f_trata_ABORT(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...){
    printf("%s ; %d -- Processando ::: %s\n",__func__, __LINE__, cmdIn->cmdBase );
    if( cmdIn->p_next == NULL ) {
        // É o ultimo comando da linha, 
        // Reorna o OK se chegou aqui., 
        printf("\rERROR\r\n");
    };
};

