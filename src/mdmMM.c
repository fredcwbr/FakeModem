
#include "mdmMM.h"


char *mmDisplayFileName = { "mmMDMx.dsply" };
char *modemFileName;
int maxndxmdm  = 8;
mdmDISPLAY *mdms;   // mmap modem displays array on file 
                    // NDXMDM shows this modem ., 


void f_mdmDsplyInit() {     
    
    char wkBf[128];
    int mmMDMDisplay;
    mdmDISPLAY *this_mdm;
    
    
    mmMDMDisplay = open(mmDisplayFileName,
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
        debug_printf( debugLVL, "Erro no mm %s\n",strerror(errno) );
    }
    ftruncate(mmMDMDisplay,sizeof(mdmDISPLAY)*maxndxmdm);
    close(mmMDMDisplay);
    
    //  Init modem display structures
    for( int i = 0; i < maxndxmdm ; ++i) {
        sprintf(wkBf,"/dev/tnt/mdmTeste%d",i);
        this_mdm = mdms + i;
        mdmDspUpdate(this_mdm, wkBf);
    }
    
    
}



void mdmDspUpdate(mdmDISPLAY *this_mdm, char *mdmFN )  {
    
    this_mdm->RX  = '0';
    this_mdm->TX  = '0';
    this_mdm->DCD = '0';
    this_mdm->RTS = '0';
    this_mdm->CTS = '0';
    this_mdm->DTR = '0';
    this_mdm->DSR = '0';
    this_mdm->mdmSt = '0';
    strncpy( this_mdm->mdmname , mdmFN, MAXMM_MDMNAME -1 );

}
    
