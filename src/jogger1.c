/**
 * regexp (License: AGPL3 or higher)
 * @param re extended POSIX regular expression
 * @param nmatch maximum number of matches
 * @param str string to match
 * @return An array of char pointers. You have to free() the first element (string storage). the second element is the string matching the full regex, then come the submatches.
*/
char **regexp(char *re, int nmatch, char *str) {
  char **result;
  char *string;
  regex_t regex;
  regmatch_t *match;
  int i;

  match=malloc(nmatch*sizeof(*match));
  if (!result) {
    fprintf(stderr, "Out of memory !");
    return NULL;
  }

  if (regcomp(&regex, re, REG_EXTENDED)!=0) {
    fprintf(stderr, "Failed to compile regex '%s'\n", re);
    return NULL;
  }

  string=strdup(str);
  if (regexec(&regex,string,nmatch,match,0)) {
#ifdef DEBUG
    fprintf(stderr, "String '%s' does not match regex '%s'\n",str,re);
#endif
    free(string);
    return NULL;
  }

  result=malloc(sizeof(*result));
  if (!result) {
    fprintf(stderr, "Out of memory !");
    free(string);
    return NULL;
  }

  for (i=0; i<nmatch; ++i) {
    if (match[i].rm_so>=0) {
      string[match[i].rm_eo]=0;
      ((char**)result)[i]=string+match[i].rm_so;
#ifdef DEBUG
      printf("%s\n",string+match[i].rm_so);
#endif                                                                                                                                                                                                                                                   
    } else {                             
      ((char**)result)[i]="";            
    }
  }

  result[0]=string;                      

  return result;                         

}
