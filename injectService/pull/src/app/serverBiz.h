
#ifndef __SERVER_BIZ_H__
#define __SERVER_BIZ_H__

#include <stdio.h>
#include <string>
#include <queue>
#include "common.h"

using namespace std;

typedef int (*_action_process)(_process_info &param);
typedef int (*_reload_process)(string &sid);

typedef struct _WorkerContext
{
    _WorkerContext()
    {
	running = true;
    }
    
    _action_process fn_spawn;
    _action_process fn_remove;
    _reload_process fn_reload;
    _action_process fn_terminate;

    std::queue<_event> *queue;
    bool running;
    int tid;
}WorkerContext;

class ServerBiz
{
public:
    static ServerBiz *Instance()
    {
        if(NULL == _instance)
		{
		    _instance = new ServerBiz;
		}
 	
		return _instance;
    }

    static void  Destroy()
    {
		if(NULL != _instance)
		{
		    delete _instance;

		    _instance = NULL;
		}
    }

    bool init(int worker_num);
    int put_data(_event &ev);
    bool host_register(string host, int port);
    int remove_host();
    inline void Schedule() {create_schedule_thread();}

    static _process_info *get_process_info(string sid);
    static int remove_process_by_stream(string sid);
    static int remove_process_by_id(int id);
    static bool is_process_exist(string sid, _process_info &process);
    static bool is_process_exist(int pid, _process_info &process);
    static int add_process_to_cache(_process_info &process);
    static int update_process_info(_process_info &process);
    static int parser(string item, _process_info *process);
    static bool is_host_exist(string host);
    static void set_mongodb(const char *db);
    static int register_host_to_cache(string host, int port);
    static int find_orphan_process(vector<_process_info> &process_vec);
	static int find_all_process(vector<_process_info> &processes);
	
private:
    int create_worker_thread(int worker_num);
    int process_alive_detect();
    int create_schedule_thread();

    static void *worker_loop(void*);
    static void *schedule(void *ptr);
	
private:
    ServerBiz() {}
    ~ServerBiz() {}
	
private:
    static ServerBiz *_instance;
    static string db_address;
    static string _host;
    static int _port;
	
    pthread_mutex_t _mutex;
    vector<WorkerContext*> worker_list;

    int worker_index;
    int worker_max;
};
#endif
