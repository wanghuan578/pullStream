
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "sys_headers.h"
#include "basic_type.h"
#include "log/pure_log.h"
#include "global_conf.h"




#define rec_log g_psyslog->prt

extern CLog* g_psyslog;
extern CLog* g_perrlog;
extern G_CONF_S * g_parg;
	
extern bool g_restart_sig;
extern bool g_reload_sig;

/*file index*/
//extern FITH_CT* work_tasks;
extern int workthreads;
extern char * g_tspath;


#endif /* _GLOBAL_H_ */
