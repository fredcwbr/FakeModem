#ifndef __ATTREATCMDS_INC__
#define __ATTREATCMDS_INC__

#include "global.h"

void f_trata_OK(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...); 
void f_trata_ATI(s_CMD_IN_t*cmdIn, atCmds *atCmd , ...);
void f_trata_ATD(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATA(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATO(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...); 
void f_trata_ATH(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATS(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATE(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ATZ(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ERROR(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);
void f_trata_ABORT(s_CMD_IN_t *cmdIn, atCmds *atCmd , ...);

#endif