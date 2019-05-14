#ifndef _GLOBAL_CONF_H_
#define _GLOBAL_CONF_H_

#include "basic_type.h"


#define MAX_PATH_LENGTH 256

	typedef struct global_conf
	{
		/*os title*/
		s8 ** argv;
		u8 argc;
		s8 * os_argv;
		s8 * last;

		/*path*/
		s8 pid_path[MAX_PATH_LENGTH];
		s8 sbin_path[MAX_PATH_LENGTH];
		s8 log_path[MAX_PATH_LENGTH];
		s8 conf_path[MAX_PATH_LENGTH];


		/*configuration file node_head*/
		
	}G_CONF_S;


	/*get current sbin path,usually from argv[0]*/
	void get_cur_path(s8 * str,char * o_path,s32 i_l);
	void set_process_titile(const s8 * i_title,G_CONF_S * i_conf);
	void init_process_title(G_CONF_S * i_conf);

#endif
