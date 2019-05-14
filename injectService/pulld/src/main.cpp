
#include "sys_headers.h"
#include <vector>

#include "global_conf.h"
#include "string/p_string.h"
#include "global.h"
#include "mongo_adaptor.h"
#include "common.h"
#include "config/iniReader.h"


#define ADMINISTRATOR ""
#define IP_SIZE     16

#define MONGODB_DATABASE "pull_stream"
#define MONGODB_COLLECTION_PROCESS "process"
#define MONGODB_COLLECTION_HOSTINFO "host"

#define DEFAULT_CONF_PATH "../conf/1.conf"
#define DEFAULT_LOG_PATH "../log/"

/*global time struct*/
CLog* g_psyslog;	
CLog* g_perrlog;	
G_CONF_S * g_parg;

bool g_restart_sig;
bool g_reload_sig;

bool is_running = false;

void write_pid_to_file(char *file);
bool is_host_exist(string &host, const char *mdb);
int remove_host(string host, const char*mdb);

bool check_process_alive(char *file, string host, const char *mdb)
{
	//pid_t pid = getpid();
	s32 m_fd = -1;
	
	if((m_fd=open(file,O_RDWR|O_CREAT,0644))==-1) 
	{
		g_psyslog->prt(LOG_CRIT,"wrtie pid : Open %s Error:%s",file,strerror(errno));
	
		exit(0);
	}
	
	s32 ret = lockf(m_fd, F_TLOCK, 0);

	close(m_fd);
	
	if(ret < 0)
	{
		//g_psyslog->prt(LOG_CRIT, "record has runned");
		//g_psyslog->prt(LOG_CRIT,"try lock file %s failed: %s", file, strerror(errno));
		
		close(m_fd);

		return true;
	}
	else
	{
		ret = lockf(m_fd, F_ULOCK, 0);

		g_psyslog->prt(LOG_CRIT, "pull2push does not running, host = %s", host.c_str());

		if(is_host_exist(host, mdb)) {

		    remove_host(host, mdb);		    
		}

		close(m_fd);
		
	    	return false;
	}
}

bool is_host_exist(string &host, const char *mdb)
{
    int ret = REC_ERROR_OK;

    g_psyslog->prt(LOG_INFO, "mongo address: %s", mdb);

    string db = MONGODB_DATABASE;
    string tb = MONGODB_COLLECTION_HOSTINFO;

    void *connect = MongoAdaptor::connect(mdb);
    void *collection = MongoAdaptor::collection(connect, db, tb);

    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "host", host.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(REC_ERROR_OK == ret) {

                g_psyslog->prt(LOG_INFO, "cache host: %s exist", host.c_str());
        
                return true;

    }
    else
    {
         g_psyslog->prt(LOG_INFO, "cache host: %s not exist", host.c_str());
        
         return false;
    }
}

int remove_host(string host, const char *mdb)
{
    int ret = REC_ERROR_OK;

    g_psyslog->prt(LOG_INFO, "remove remain host %s", host.c_str());
    
    string db = MONGODB_DATABASE;
    string tb = MONGODB_COLLECTION_HOSTINFO;
	
    void *connect = MongoAdaptor::connect(mdb);
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "host", host.c_str());

    if(!MongoAdaptor::remove(collection, query)) {

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int daemon(CLog *log, pid_t &pid)
{
    int  fd;

    pid_t ppid = fork();

    switch (ppid) {
    case -1:
	log->prt(LOG_CRIT,"fork() failed");
        return -1;

    case 0:
        break;

    default:
        exit(0);
    }
   
    write_pid_to_file(g_parg->pid_path);
 
    pid = getpid();

    log->prt(LOG_CRIT,"daemon pid = %d", pid);

    if (setsid() == -1) {
	log->prt(LOG_CRIT,"setsid() failed");
        return -1;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
	log->prt(LOG_CRIT,"open(\"/dev/null\") failed");
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
	log->prt(LOG_CRIT,"dup2(STDIN) failed");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
	log->prt(LOG_CRIT,"dup2(STDOUT) failed");
        return -1;
    }

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
	    log->prt(LOG_CRIT,"close() failed");
            return -1;
        }
    }

    return 0;
}

void write_pid_to_file(char *file)
{
        pid_t pid = getpid();
        s32 m_fd = -1;

        if((m_fd=open(file,O_RDWR|O_CREAT,0644))==-1)
        {
                g_psyslog->prt(LOG_CRIT,"wrtie pid : Open %s Error:%s",file,strerror(errno));

                exit(0);
        }

        int flags = fcntl(m_fd, F_GETFD);
        flags |= FD_CLOEXEC;
        fcntl(m_fd, F_SETFD, flags);

        s32 ret = lockf(m_fd,F_TLOCK,0);

        if(ret<0)
        {
                g_psyslog->prt(LOG_CRIT,"try lock file %s failed: %s", file, strerror(errno));
                printf("record has runned\n");
                exit(0);
        }

        s8 buffer[64] = {0};

        s32 len = rn_snprintf(buffer,64,"%d\n",pid);

        if(write(m_fd,buffer,len) < 0)
        {
                g_psyslog->prt(LOG_CRIT,"wrtie pid failed: %s", strerror(errno));
                exit(0);
        }

	printf("[%d]+\n", pid);

        return;
}

int get_local_ip(const char *eth_inf, char *ip)  
{  
    int sd;  
    struct sockaddr_in sin;  
    struct ifreq ifr;  
  
    sd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (-1 == sd)  
    {  
        printf("socket error: %s\n", strerror(errno));  

        return -1;        
    }  
  
    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);  
    ifr.ifr_name[IFNAMSIZ - 1] = 0;  
      
    // if error: No such device  
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)  
    {  
        printf("ioctl error: %s\n", strerror(errno));  

        close(sd);  

        return -1;  
    }  
                                           
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
    snprintf(ip, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));  
                                    
    close(sd);  

    return 0;  
}

static void sig_terminal(int signo)
{
    is_running = false;

    g_psyslog->prt(LOG_INFO, "\nbye");

    return;
}

int main(int argc, char *const *argv)
{
	const char *conf_p = DEFAULT_CONF_PATH;
	char *dae = NULL;
	char *child_bin_dir = NULL;
  	string net_type;
	char rec_bin_dir[1024] = {0};
	char rec_pid_dir[1024] = {0};
	bool is_daemon = false;
	char bin_dir[256] = {0};
	char local_ip[IP_SIZE] = {0};
	CIniReader ini_reader;
	string mongo_addr;

	signal(SIGINT, sig_terminal);
	signal(SIGTERM, sig_terminal);

	MongoAdaptor::init();

	realpath(argv[0], bin_dir);

	/*get current path*/
	g_parg = new G_CONF_S;
	assert(g_parg);
	memset(g_parg,0,sizeof(G_CONF_S));
	g_parg->argv = (s8 **)argv;
	g_parg->argc = argc;
	g_parg->os_argv = argv[0];
	get_cur_path(argv[0],g_parg->sbin_path,MAX_PATH_LENGTH);
	
	/*initialize environment*/
	snprintf(g_parg->conf_path, MAX_PATH_LENGTH, "%s/%s", g_parg->sbin_path, conf_p);
	snprintf(g_parg->log_path, MAX_PATH_LENGTH, "%s/%s", g_parg->sbin_path, DEFAULT_LOG_PATH);
	snprintf(g_parg->pid_path, MAX_PATH_LENGTH, "%s/%s", g_parg->sbin_path, "../log/pid");

	ini_reader.Load(g_parg->conf_path, "r");
	
	dae = ini_reader.GetStr("system", "daemon");
                if(NULL != dae) {

			if(strcmp("on", dae) == 0)
			    is_daemon = true;
			else if(strcmp("off", dae) == 0)
			    is_daemon = false;
                }

	child_bin_dir = ini_reader.GetStr("child", "bin_dir");
                if(NULL != child_bin_dir) {
		
			snprintf(rec_bin_dir, sizeof(rec_bin_dir), "%s%s%s", ADMINISTRATOR, child_bin_dir, "sbin/pull");
			snprintf(rec_pid_dir, sizeof(rec_bin_dir), "%s%s", child_bin_dir, "log/pid");
                }
		else
		{
			printf("child_bin_dir unspecified\n");

			exit(0);
		}
	
	net_type = ini_reader.GetStr("system", "net_type");
                if(net_type.empty()) {

			printf("net_type unspacified\n");

			exit(0);
                }
	
	mongo_addr = ini_reader.GetStr("mongodb", "address");
	
	if (mongo_addr.empty()) {

		rec_log(LOG_INFO, "mongdb address unspecified");

		exit(0);
	}

	/*create CLOG*/
	g_psyslog = new CLog;
	
	assert(g_psyslog);
	
	if (is_daemon)
	    g_psyslog->set_log_tank("file");
	else 
	    g_psyslog->set_log_tank("console");
 	
	g_psyslog->set_log_level(LOG_INFO);
	g_psyslog->set_log_out_by_time(-1);	//one file per hour 
	g_psyslog->init(g_parg->log_path,"error");

	if (is_daemon) {
	    
	    pid_t tmp;
	    if(0 != daemon(g_psyslog, tmp)) {

		rec_log(LOG_INFO, "daemon error");
	
		goto EXIT;	
	    }

	}

	get_local_ip(net_type.c_str(), local_ip);

	is_running = true;

	while(is_running) {

		if(!check_process_alive(rec_pid_dir, local_ip, mongo_addr.c_str())) {
		
			rec_log(LOG_INFO, "run bin dir: %s", rec_bin_dir);

			system(rec_bin_dir);
		}

		sleep(5);
	}

EXIT:
	delete g_parg;
	delete g_psyslog;
}

//end
