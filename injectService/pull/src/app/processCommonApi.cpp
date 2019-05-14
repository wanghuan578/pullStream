
#include <sys/wait.h>
#include <unistd.h>
#include "processCommonApi.h"
#include "global.h"
#include "log4cplus/log.h"
#include "serverBiz.h"

#define FFMPEG_PARAM_MAX_LEN 2048

enum _fork_mode
{
    FORK_MODE_SPAWN,
    FORK_MODE_RELOAD,
};

/*
void ProcessCtlModule::sig_reap_chld(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
 	Log::Trace("reap child pid = %d", pid);
  	
	//_message msg;

	//msg.type = MSG_TYPE_SIGCHIL_NOTIFY;
	//msg.data = (void*)(&pid);

	//printf("wanghuan sig_reap_chld ----------------- pid = %d\n", pid);

	//send_message(msg);
    }
}

int ProcessCtlModule::send_message(_message &msg)
{
    int ret = ERROR_OK;

    //boost::shared_ptr<_message> m(msg);

    switch(msg.type)
    {
    case MSG_TYPE_SIGCHIL_NOTIFY:
    {
   	int pid = *(static_cast<int*>(msg.data));
	
	_process_info info;

	if(ServerBiz::is_process_exist(pid, info)) {

            spawn_process_i(info);
    	}
    }
	break;

    default:
	break;
    }

    return ret;
}
*/

int ProcessCtlModule::process_reload(string &sid)
{
    int ret = ERROR_OK;

    _process_info info;

    if(!ServerBiz::is_process_exist(sid, info)) {

		Log::Trace("process_reload stream: %s not exist", sid.c_str());

		return REC_ERROR_STREAM_NOT_EXIST; 
    }

    spawn_process_i(info);

    return ret;
}

bool ProcessCtlModule::check_system_process_exist(int pid)
{
    int ret = kill(pid, 0);

    Log::Trace("check orphan process exist ret = %d", ret);

    if(0 == ret) {

		return true;	
    }

    return false;
}

int ProcessCtlModule::spawn_process(_process_info &param)
{
    int ret = ERROR_OK;
    pid_t pid;

    _process_info temp;
    if(ServerBiz::is_process_exist(param.sid, temp)) {

        Log::Error("-------------------stream %s exist", param.sid.c_str());

	return ERROR_STREAM_EXIST;
    }

    pid = fork();

    if(pid < 0)
    {
        Log::Error("create process error");

        return REC_ERROR_FORK_FAILED;
    }
    else if(pid > 0)
    {
	param.pid = pid;		
	
	ServerBiz::add_process_to_cache(param);

	return ERROR_OK;		
    }
    else if (pid == 0)
    {
	char cmd[FFMPEG_PARAM_MAX_LEN] = {0};
/*
    	if (TP_RTMP_2_RTMP == param.type) {

        	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -c copy -f flv %s", param.
src_url.c_str(), param.dest_url.c_str());

        	Log::Info("exec [rtmp 2 rtmp] ffmpeg cmd = %s", cmd);
    	}
    	else if (TP_RTSP_2_RTMP == param.type)
    	{
        	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -f flv -r 25 -s 640x480 -an %s", param.src_url.c_str(), param.dest_url.c_str());

        	Log::Info("exec [rtsp 2 rtmp] ffmpeg cmd = %s", cmd);
    	}
	else 
	{
	    assert(false);

            Log::Error("!!!!!!!!!!!!!!!!!!!!!!!!!1ffmpeg type unexcepted");

            return ERROR_TRANSLATE_TYPE_UNEXCEPTED;
	}
*/
	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -c copy -f flv %s", param.
src_url.c_str(), param.dest_url.c_str());

    	int ret = execl("/bin/sh", "sh", "-c", cmd, (char*)0);

    	if (ret < 0)
    	{
        	Log::Warn("exec ffmpeg failed, stream %s, pid=%d", param.sid.c_str(), pid);
    	}	
    }
}

int ProcessCtlModule::spawn_process_i(_process_info &param)
{
    pid_t pid;
    pid = fork();

    if(pid < 0)
    {
        Log::Error("spawn_process_i fork process error");

        return REC_ERROR_FORK_FAILED;
    }
    else if(pid > 0)
    {
        param.pid = pid;

        ServerBiz::update_process_info(param);

	return ERROR_OK;
    }
    else if (pid == 0)
    {
	char cmd[FFMPEG_PARAM_MAX_LEN] = {0};
/*
    	if (TP_RTMP_2_RTMP == param.type) {

        	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -c copy -f flv %s", param.
src_url.c_str(), param.dest_url.c_str());

        	Log::Info("exec [rtmp 2 rtmp] ffmpeg cmd = %s", cmd);
    	}
    	else if (TP_RTSP_2_RTMP == param.type)
    	{
        	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -f flv -r 25 -s 640x480 -an %s", param.src_url.c_str(), param.dest_url.c_str());

        	Log::Info("exec [rtsp 2 rtmp] ffmpeg cmd = %s", cmd);
    	}
	else 
	{
	    assert(false);

	    Log::Error("###################################3ffmpeg type unexcepted");

	    return ERROR_TRANSLATE_TYPE_UNEXCEPTED; 
	}
*/
	snprintf(cmd, FFMPEG_PARAM_MAX_LEN - 1, "ffmpeg -v quiet -i %s -c copy -f flv %s", param.
src_url.c_str(), param.dest_url.c_str());

    	int ret = execl("/bin/sh", "sh", "-c", cmd, (char*)0);

    	if (ret < 0)
    	{
        	Log::Warn("exec ffmpeg failed, stream %s, pid=%d", param.sid.c_str(), pid);
    	}
    }
}

int ProcessCtlModule::remove_process(_process_info &info)
{
    _process_info *p = ServerBiz::get_process_info(info.sid);

    if(NULL == p) {

		Log::Warn("kill sid: %s not exist", info.sid.c_str());

		return REC_ERROR_STREAM_NOT_EXIST;
    }

    ServerBiz::remove_process_by_stream(p->sid);

    kill(p->pid, SIGKILL);

    delete p;

    return ERROR_OK;
}

int ProcessCtlModule::terminate_process(_process_info &info)
{
    Log::Info("kill/i pid: %d", info.pid);

    kill(info.pid, SIGKILL);

    return ERROR_OK;
}
