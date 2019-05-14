
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "global.h"
#include "config/iniReader.h"
#include "app/rest_api.h"
#include "app/serverBiz.h"
#include "app/processCommonApi.h"
#include "log4cplus/log.h"
#include "utils/utils.h"

void sig_handler_terminal(int signo)
{
    Log::Trace("exit signal");
 
    ServerBiz::Instance()->remove_host();

    Log::Trace("bye");

    exit(0);
}

int main(int argc, char *const *argv)
{
	const char *conf_p = DEFAULT_CONF_PATH;
	char *dae = NULL;
	char *net_type = NULL;
	int worker_num = 0;
	bool is_daemon = false;
	char local_ip[IP_SIZE] = {0};
	CIniReader ini_reader;	
	CRestApi api;
	char sbin_path[MAX_PATH_LENGTH] = {0};
	char pid_path[MAX_PATH_LENGTH] = {0};
	char conf_path[MAX_PATH_LENGTH] = {0};
	char log_conf_path[MAX_PATH_LENGTH] = {0};

	signal(SIGTERM, sig_handler_terminal);
    	signal(SIGINT, sig_handler_terminal);

	CUtils::get_cur_path(argv[0], sbin_path, MAX_PATH_LENGTH);
	
	snprintf(conf_path, MAX_PATH_LENGTH, "%s/%s", sbin_path, conf_p);
	snprintf(pid_path, MAX_PATH_LENGTH, "%s/%s", sbin_path, "../log/pid");
	snprintf(log_conf_path, MAX_PATH_LENGTH, "%s/%s", sbin_path, "../conf");
	
	ini_reader.Load(conf_path, "r");

	dae = ini_reader.GetStr("system", "daemon");
	
	if(NULL != dae) {
		
	    if(strcmp("on", dae) == 0)
		is_daemon = true;
	    else if(strcmp("off", dae) == 0)
    		is_daemon = false;
    	}
	
	if (is_daemon) {
	    
	    pid_t tmp;
	    if(ERROR_OK != CUtils::daemon(pid_path, tmp)) {

		Log::Trace("daemon error");
		
		exit(0);	
	    }
	}

	Log::Init(log_conf_path, is_daemon);
	Log::Trace("%s","wanghuan log test");

	net_type = ini_reader.GetStr("system", "net_type");

	if(NULL == net_type) {
	
	    printf("net_type unspecified\n");

	    exit(0);
    	}

	CUtils::get_local_ip(net_type, local_ip);

	worker_num = ini_reader.GetInt("system", "worker");

        if(0 == worker_num) {

            printf("worker_num unspecified\n");

            exit(0);
    	}
	
	ServerBiz::Instance()->init(worker_num);
	
	api.init(ini_reader, local_ip);

	ServerBiz::Instance()->Schedule();

	api.run();

	unlink(pid_path);

}

//end
