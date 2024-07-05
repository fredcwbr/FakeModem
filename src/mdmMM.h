#ifndef __MDMMM_INT_H
#define __MDMMM_INT_H

#include "global.h"
#include "helper.h"




extern char *mmDisplayFileName;
extern char *modemFileName;
extern int maxndxmdm;
extern mdmDISPLAY *mdms;   // mmap modem displays array on file 
                    // NDXMDM shows this modem ., 

void f_mdmDsplyInit() ;
void mdmDspUpdate(mdmDISPLAY *this_mdm, char *mdmFN );

#endif
