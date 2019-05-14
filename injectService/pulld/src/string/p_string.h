#ifndef _PURE_STRING_H_
#define _PURE_STRING_H_

#include "sys_headers.h"


#define STRMATCH(av1, av2) (strlen(av1) == strlen(av2) && !memcmp((av1),(av2),strlen(av1)))

//safe strcpy
char * rc_strncpy(char * dest,int d_l,const char * src,int s_l);

int rn_strncpy(char * dest,int d_l,const char * src,int s_l);

int rn_snprintf(char * buf,int n,const char * fmt,...);

int p_atoof(char *line, size_t n);


#endif

