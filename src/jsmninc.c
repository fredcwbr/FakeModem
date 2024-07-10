
#define JSMN_PARENT_LINKS 1
//  https://github.com/zserge/jsmn
#include "jsmn.h"

#include "jsmninc.h"
#include "helper.h"


char *bfConfig;

int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
    
  DEBUG_PRINTF( 50, "called with rest %d : Indent %d >> ndx :%d; type %X : sz %d ; len = %d\n",
                    count,
                    indent, 
                    (t - jsmn_configTOKs), 
                    t->type, 
                    t->size ,
                    t->end - t->start );
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    DEBUG_PRINTF( 50, "JSMN_PRIMITIVE ndx %d : %.*s\n", (t - jsmn_configTOKs), t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    DEBUG_PRINTF( 50, "JSMN_STRING  ndx %d : '%.*s'\n", (t - jsmn_configTOKs) , t->end - t->start, js + t->start, (t - jsmn_configTOKs));
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    DEBUG_PRINTF( 50, "Object  ndx %d : SZ %d\n",(t - jsmn_configTOKs) ,t->size);
    j = 0;
    for (i = 0; i < t->size; i++) {
      key = t + 1 + j;
      DEBUG_PRINTF( 50, "Entrando no dump para este obj ndx=%d.\n", (t - jsmn_configTOKs) );
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        DEBUG_PRINTF( 70, "recorrencia... : \n");
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
      DEBUG_PRINTF( 50, "\n");
    }
    DEBUG_PRINTF( 25, "Saindo do Objeto., \n");
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    DEBUG_PRINTF( 50, "Array SZ %d\n",t->size);
    for (i = 0; i < t->size; i++) {
      j += dump(js, t + 1 + j, count - j, indent + 1);
      DEBUG_PRINTF( 50, "J=> %d\n", j );
    }
    return j + 1;
  } 
  DEBUG_PRINTF( 25, "Saindo pelo ELSE com type = %d, count = %d , indent=%d\n", t->type , count, indent );
  return 0;
}
