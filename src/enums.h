#ifndef __ENUMS_INC_H
#define __ENUMS_INC_H



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


// Fake Modem AT commands configuration
enum tpREGV {REGV_INVALID, REGV_CHAR, REGV_INT, REGV_STRARR};



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


// State machine ..
enum mdmSTATE  {COMMAND,DIALING,TRAINING,RINGING,ONLINE,ERROR};


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








#endif