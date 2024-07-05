
#include "global.h"


extern char **strArray(  int K );


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

s_CMD_IN_t *regexProc(char *p0);

#define tofind    " ?((([&\\+%]?) ?([a-zA-Z]+))([0-9|;]*|[;]?)(([=?])?([0-9]*))?)"

#define SILENCETIMEOUT  1000

char *filename = "testcmd.atcmd"; // /etc/rc.conf";
//
char s_atResponsesSTR[] = "atResponsesSTR";
char s_atResponsesOK[] = "atResponsesOK";
char s_atDFLReg[] = "atDFLReg";

