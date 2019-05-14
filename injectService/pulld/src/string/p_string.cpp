#include "p_string.h"


//dest_len 参数均为数组大小，即包括'\0'的大小长度,src_len为复制长度 strlen(src);
char * rc_strncpy(char * dest,int d_l,const char * src,int s_l)
{
	if(s_l<d_l-1)
	{
		strncpy(dest,src,s_l);
		dest[s_l] = '\0';
	}
	else
	{
		strncpy(dest,src,d_l-1);
		dest[d_l-1] = '\0';
	}

	return dest;
}

//dest_len 参数均为数组大小，即包括'\0'的大小长度，src_len为复制长度
int rn_strncpy(char * dest,int d_l,const char * src,int s_l)
{

	if(s_l<d_l-1)
	{
		strncpy(dest,src,s_l-1);
		dest[s_l-1] = '\0';
		return s_l-1;
	}
	else
	{
		strncpy(dest,src,d_l-1);
		dest[d_l-1] = '\0';
		return d_l-1;
	}
	
}

int rn_snprintf(char * buf,int n,const char * fmt,...)
{
	 va_list argptr;
	 int cnt;
	 va_start(argptr, fmt);
	 cnt = vsnprintf(buf,n, fmt, argptr);
	 va_end(argptr);
	 
	 if(cnt<0)
	 {
	 	return cnt;
	 }
	 
	 if(cnt < n)
	 {
	 	return cnt;
	 }
	 else
	 {
	 	return n;
	 }
	 
}

int p_atoof(char *line, size_t n)
{
    int  value;

    if (n == 0) {
        return -1;
    }

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return -1;
        }

        value = value * 10 + (*line - '0');
    }

    if (value < 0) {
        return -1;

    } else {
        return value;
    }
}


