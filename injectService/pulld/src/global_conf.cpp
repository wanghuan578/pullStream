#include "global_conf.h"

#include "string/p_string.h"
//#include "sys_headers.h"

extern s8 ** environ;



void get_cur_path(char * str,char * o_path,s32 i_l)
{
	char * pos =str;
	char * last_slash = NULL;
	do
	{
		pos = strchr(pos,'/');
		if(pos)
			last_slash = pos;
		else
		{
			break;
		}
		pos++;
	}
	while(pos&&(*pos != 0) );

	s32 len = last_slash - str;
	if(len>i_l-1)
	{
		printf("path too long\n");
		exit(0);
	}
	rc_strncpy(o_path,i_l,str,len);
	return;
}

void set_process_titile(const s8 * i_title,G_CONF_S * i_conf)
{
	s32 len = i_conf->last - i_conf->os_argv;
	memset(i_conf->os_argv,0,len);
	rc_strncpy(i_conf->os_argv,len,i_title,strlen(i_title));
	
	return;
}

void init_process_title(G_CONF_S * i_conf)
{
	s8 * last = NULL;
	u32       size;
    s32   i;
	
    size = 0;

    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }


    last = i_conf->argv[0];

    for (i = 0; i_conf->argv[i]; i++) {
        if (last == i_conf->argv[i]) {
            last = i_conf->argv[i] + strlen(i_conf->argv[i]) + 1;
        }
    }

    for (i = 0; environ[i]; i++) {
        if (last == environ[i]) {

            size = strlen(environ[i]) + 1;
            last = environ[i] + size;

        }
    }

    last --;
	i_conf->last = last;
}


