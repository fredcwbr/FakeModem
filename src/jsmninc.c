

//  https://github.com/zserge/jsmn
#include "jsmn.h"

#include "jsmninc.h"
#include "helper.h"


char *bfConfig;

int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    debug_printf( 50, "%.*s", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    debug_printf( 50, "'%.*s'", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    debug_printf( 50, "Object SZ %d\n",t->size);
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
        debug_printf( 50, "  ");
      }
      key = t + 1 + j;
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        debug_printf( 50, ": ");
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
      debug_printf( 50, "\n");
    }
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    debug_printf( 50, "Array SZ %d\n",t->size);
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
        debug_printf( 50, "  ");
      }
      debug_printf( 50, "   - ");
      j += dump(js, t + 1 + j, count - j, indent + 1);
      debug_printf( 50, "\n");
    }
    return j + 1;
  }
  return 0;
}
