
#include "utils.h"
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "log4cplus/log.h"
#include <net/if.h>
#include "app/common.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#define FILE_PATH_LEN_MAX 256

void CUtils::multi_mkdir(const char *dir)   
{
    char *str = new char[FILE_PATH_LEN_MAX];
	
    memset(str, 0x00, FILE_PATH_LEN_MAX);
	
    strncpy(str, dir, FILE_PATH_LEN_MAX - 1);
	
    int len = strlen(str);
	
    for(int i = 0; i < len; i++)
    {
        if(str[i] == '/')
        {  
            str[i] = '\0';
			
            if(access(str, 0) != 0)
            {  
                mkdir(str, 0777);
            }
			
            str[i] = '/';
        }
    }
	
    if(len > 0 && access(str, 0) != 0)  
    {  
        mkdir(str, 0777);  
    }

    delete [] str;
}

int CUtils::get_cur_path(char *str, char *o_path, int i_l)
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

	int len = last_slash - str;
	if(len>i_l-1)
	{
		//LOG_TRACE("path too length");
		
		return -1;
	}
	strncpy(o_path, str, len);
	return 0;
}

int CUtils::get_local_ip(const char *eth_inf, char *ip)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    Log::Info("eth_inf: %s", eth_inf);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        Log::Error("socket error: %s\n", strerror(errno));

        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        Log::Error("ioctl error: %s\n", strerror(errno));

        close(sd);

        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));

    close(sd);

    return 0;
}

int CUtils::get_pid_from_file(char *file, pid_t *o_pid)
{
	int m_fd = -1;
	
	if((m_fd=open(file,O_RDONLY,S_IRUSR))==-1) 
	{
		Log::Error("get pid : Open %s Error:%s\n",file,strerror(errno));

		return ERROR_OPEN_PID_FILE;
	}

	int flags = fcntl(m_fd, F_GETFD); 
	flags |= FD_CLOEXEC; 
	fcntl(m_fd, F_SETFD, flags); 

	char buffer[64] = {0};

	if(read(m_fd,buffer,64)<0)
	{
		Log::Error("get pid : Read %s Error:%s\n",file,strerror(errno));

		return ERROR_OPEN_PID_FILE;
	}

	*o_pid = atoi(buffer);
	
	return ERROR_OK;

}

int CUtils::daemon(const char *pid_path, pid_t &pid)
{
    int  fd;

    pid_t ppid = fork();

    switch (ppid) {
    case -1:
		Log::Error("fork() failed");
        return REC_ERROR_INTERNAL;

    case 0:
        break;

    default:
        exit(0);
    }
    
    write_pid_to_file(pid_path);

    pid = getpid();

    if (setsid() == -1) {
		
		Log::Error("setsid() failed");
		
        return REC_ERROR_INTERNAL;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
	
    if (fd == -1) {
		Log::Error("open(\"/dev/null\") failed");
        return REC_ERROR_INTERNAL;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
		Log::Error("dup2(STDIN) failed");
        return REC_ERROR_INTERNAL;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
		Log::Error("dup2(STDOUT) failed");
        return REC_ERROR_INTERNAL;
    }

#if 0
    if (dup2(fd, STDERR_FILENO) == -1) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno, "dup2(STDERR) failed");
        return NGX_ERROR;
    }
#endif

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
	    	Log::Error("close() failed");
            return REC_ERROR_INTERNAL;
        }
    }

    return ERROR_OK;
}

void CUtils::write_pid_to_file(const char *file)
{
	pid_t pid = getpid();
	int m_fd = -1;
	
	if((m_fd = open(file, O_RDWR|O_CREAT, 0644)) == -1) 
	{
		Log::Error("wrtie pid : Open %s Error:%s",file,strerror(errno));
	
		exit(0);
	}
	
	int flags = fcntl(m_fd, F_GETFD); 
	flags |= FD_CLOEXEC; 
	fcntl(m_fd, F_SETFD, flags); 

	int ret = lockf(m_fd, F_TLOCK, 0);
	
	if(ret < 0)
	{
		Log::Error("try lock file %s failed: %s", file, strerror(errno));
		Log::Error("instance has runned\n");

		exit(0);
	}
	
	char buffer[64] = {0};
	
	int len = snprintf(buffer, 64, "%d\n", pid);
	
	if(write(m_fd, buffer, len) < 0)
	{
		Log::Error("wrtie pid failed: %s", strerror(errno));

		exit(0);
	}

	//close(m_fd);
	printf("[%d]+\n", pid);
}

bool CUtils::try_lock_file(char *file, int &fd)
{
	Log::Info("try lock file = %s\n", file);

    if((fd = open(file, O_RDWR|O_CREAT, 0644)) == -1)
    {
        Log::Error("wrtie pid : Open %s Error:%s",file,strerror(errno));

        exit(0);
    }       
            
    int flags = fcntl(fd, F_GETFD); 
    flags |= FD_CLOEXEC; 
    fcntl(fd, F_SETFD, flags); 
            
    int ret = lockf(fd, F_TLOCK, 0);  
            
    if(ret == 0)
    { 
		Log::Info("lock file succeed\n");
		
		return true;
    }

	close(fd);
	
	Log::Warn("lock file failed\n");
	
	return false;
}


