
#include "serverBiz.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <dirent.h>  
#include <unistd.h>
#include <memory.h>

#include "processCommonApi.h"
#include "mongoCommonApi.h"
#include "json/json.h"
#include <sys/socket.h> 
#include "global.h"
#include "log4cplus/log.h"

#define MONGODB_DATABASE "pull_stream"
#define MONGODB_COLLECTION_PROCESS "process"
#define MONGODB_COLLECTION_HOSTINFO "host"

ServerBiz *ServerBiz::_instance = NULL;
string ServerBiz::_host = "";
string ServerBiz::db_address;
int ServerBiz::_port = 0;

bool ServerBiz::init(int worker_num)
{
    signal(SIGCHLD, SIG_IGN);

    pthread_mutex_init(&_mutex, NULL);

    worker_index = 0;

    worker_max = worker_num;

    create_worker_thread(worker_max);

    //create_schedule_thread();
 
    return ERROR_OK;
}

bool ServerBiz::host_register(string host, int port)
{
    bool ret = false;
   
    Log::Info("host: %s register", host.c_str());
 
    if(!is_host_exist(host)) {

	register_host_to_cache(host, port);

	ret = true;
    }

    return ret;
}

void ServerBiz::set_mongodb(const char *db)
{
    db_address = db;
}

int ServerBiz::update_process_info(_process_info &param)
{
    int ret = ERROR_OK;
 
    void *connect = MongoAdaptor::connect(db_address.c_str());

    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);

    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "sid", param.sid.c_str());

    bson_t *update = bson_new();
    bson_t *child = bson_new();

    bson_append_document_begin(update, "$set", -1, child);
    BSON_APPEND_INT32(child, "pid", param.pid);
    bson_append_document_end(update, child);

    Log::Info("update_process_info new pid = %d", param.pid);

    if(!MongoAdaptor::update(collection, query, update)) {

		Log::Error("mongodb update failed");

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(child);
    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::put_data(_event &ev)
{
    pthread_mutex_lock(&_mutex);

    WorkerContext *context = worker_list[worker_index];

    context->queue->push(ev);
    
    worker_index = (worker_index + 1) % worker_max;

    pthread_mutex_unlock(&_mutex);

    return ERROR_OK;
}

void *ServerBiz::schedule(void *ptr)
{
    ServerBiz *p = (ServerBiz*)ptr;

    while(1)
    {
        Log::Info("schedule detection ...");

	p->process_alive_detect();

        sleep(10);
    }

    return NULL;
}


int ServerBiz::process_alive_detect()
{
    int ret = ERROR_OK;

    vector<_process_info> processes;

    ret = find_all_process(processes);

    Log::Info("find processes size = %d", processes.size());

    vector<_process_info>::iterator iter;
    for(iter = processes.begin(); iter != processes.end(); iter++) 
    {	
		if(!ProcessCtlModule::check_system_process_exist(iter->pid)) 
		{
			Log::Warn("system process pid = %d not exist", iter->pid);

			_process_info info;

			if(is_process_exist(iter->pid, info)) {

				ProcessCtlModule::spawn_process_i(info);
			}
		}
		else
		{
			//printf("system process pid = %d exist\n", iter->pid);
		}
    }

    return ret;
}

void *ServerBiz::worker_loop(void *ptr)
{
    _event ev;
    int rtn = ERROR_OK;
	
    WorkerContext *context = (WorkerContext*)ptr;

    queue<_event> *q = context->queue;

    Log::Info("worker thread: %d", context->tid);

    while(context->running)
    {
		while(!q->empty())
		{
			ev = q->front();
			
			switch(ev.type)
			{
			case EV_CREATE_PROCESS:
			{
				Log::Info("spawn process sid: %s, tid: %d", ev.process.sid.c_str(), context->tid);

				rtn = context->fn_spawn(ev.process);

				if (ERROR_OK != rtn) {

					Log::Error("spawn process error: %d", rtn);
				}
			}
			break;

			case EV_REMOVE_PROCESS:
			{
				Log::Info("remove process sid: %s, tid: %d", ev.process.sid.c_str(), context->tid);

				rtn = context->fn_remove(ev.process);

				if (ERROR_OK != rtn) {

					Log::Error("remove process error: %d", rtn);
				}
			}
			break;
			
			case EV_RELOAD_PROCESS:
			{
				Log::Info("reload process sid = %s", ev.process.sid.c_str());
				
				rtn = context->fn_reload(ev.process.sid);

				if (ERROR_OK != rtn) {

					Log::Error("reload process error: %d", rtn);
				}
			}
			break;

			default:
			{
				Log::Error("mainstage: undefined message type");
				
				assert(0);
			}
			break;

			}

			q->pop();
		}
	
		usleep(20000);
    }

    return NULL;
}

int ServerBiz::create_schedule_thread()
{
    pthread_t id;
    
    int ret = pthread_create(&id, NULL, schedule, this);
    
    if(ret) {

        Log::Error("create schedule error");
        
	return 1;
    }

    Log::Info("create schedule thread");

    return ERROR_OK;
}

int ServerBiz::create_worker_thread(int worker_num)
{
	for(int i = 0; i < worker_num; i++) {

	    WorkerContext *context = new WorkerContext;
		
	    context->fn_spawn = ProcessCtlModule::spawn_process;
	    context->fn_remove = ProcessCtlModule::remove_process;
		context->fn_reload = ProcessCtlModule::process_reload;
		context->fn_terminate = ProcessCtlModule::terminate_process;
		
	    context->queue = new std::queue<_event>;
	    context->tid = i;
		
	    pthread_t id;
	    int ret = pthread_create(&id, NULL, worker_loop, context);

	    if(ret) {

	        Log::Error("create thread %d error", i);

	        exit(1);
	    }
 	    
	    worker_list.push_back(context);
	}

    return ERROR_OK;
}

int ServerBiz::add_process_to_cache(_process_info &info)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "sid", info.sid.c_str());
    BSON_APPEND_INT32(query, "pid", info.pid);
    BSON_APPEND_INT32(query, "ppid", getpid());
    BSON_APPEND_UTF8(query, "src_url", info.src_url.c_str());
    BSON_APPEND_UTF8(query, "dest_url", info.dest_url.c_str());
    BSON_APPEND_INT32(query, "type", (int)info.type);
    BSON_APPEND_UTF8(query, "status", "active");
    BSON_APPEND_UTF8(query, "host", info.host.c_str());

    if(!MongoAdaptor::insert(collection, query)) {

		Log::Error("mongodb insert sid: %s failed", info.sid.c_str());
		
		ret = REC_ERROR_MONGODB;
    }
    
    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::register_host_to_cache(string host, int port)
{
    int ret = ERROR_OK;

    _host = host;
    _port = port;

    void *connect = MongoAdaptor::connect(db_address.c_str());

    if(NULL == connect) {

		Log::Error("mongodb get connect: %s failed", db_address.c_str());

		return REC_ERROR_MONGODB;
    }

    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_HOSTINFO);
    
    if(NULL == collection) {

		Log::Error("mongodb get collection: %s failed", MONGODB_COLLECTION_HOSTINFO);

		return REC_ERROR_MONGODB;
    }

    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "host", host.c_str());
    BSON_APPEND_INT32(query, "port", port);

    if(!MongoAdaptor::insert(collection, query)) {
                
		Log::Error("mongodb insert host: %s failed", host.c_str());

		ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::find_orphan_process(vector<_process_info> &process_vec)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();
    bson_t *child = bson_new();

    int pid = getpid();
    bson_append_document_begin(query, "ppid", -1, child);
    BSON_APPEND_INT32(child, "$ne", pid);
    bson_append_document_end(query, child);

    vector<string> vec;
    ret = MongoAdaptor::find(collection, query, vec);
    
    _process_info item;
    vector<string>::iterator iter;
    for(iter = vec.begin(); iter != vec.end(); iter++) {

    	parser(*iter, &item);

		if (item.ppid != pid) {
			
		    process_vec.push_back(item);
		}
    }

    bson_destroy(query);
    bson_destroy(child);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::find_all_process(vector<_process_info> &processes)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();

    vector<string> vec;
    ret = MongoAdaptor::find(collection, query, vec);
    
    _process_info item;
    vector<string>::iterator iter;
    for(iter = vec.begin(); iter != vec.end(); iter++) {

    	parser(*iter, &item);

		processes.push_back(item);
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

bool ServerBiz::is_process_exist(int pid, _process_info &info)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();
    BSON_APPEND_INT32(query, "pid", pid);

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(ERROR_OK == ret) {

        parser(item, &info);

        return true;
    }
    else
    {
        return false;
    }
}

bool ServerBiz::is_process_exist(string sid, _process_info &info)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "sid", sid.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);


    if(ERROR_OK == ret) {

        parser(item, &info);

        return true;
    }
    else
    {
        return false;
    }

}

bool ServerBiz::is_host_exist(string host)
{
    int ret = ERROR_OK;
	
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_HOSTINFO);

    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "host", host.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(ERROR_OK == ret) {

		Log::Info("is_host_exist host: %s exist", host.c_str());

        return true;
    }
    else
    {
		Log::Info("host %s not exist", host.c_str());

        return false;
    }
}

int ServerBiz::parser(string item, _process_info *info)
{
    try
    {
    	Json::Reader reader;
    	Json::Value root;

    	if (!reader.parse(item.c_str(), root, false)) {

	    	return REC_ERROR_JSON_PARSER;
    	}

    	info->src_url = root["src_url"].asString();
    	info->dest_url = root["dest_url"].asString();
    	info->sid = root["sid"].asString();
    	info->type = root["type"].asInt();
    	info->status = root["status"].asString();
    	info->host = root["host"].asString();
    	info->pid = root["pid"].asInt();
    	info->ppid = root["ppid"].asInt();
    }
    catch(std::exception &ex)
    {
		Log::Error("json parser exception detail = %s\n", ex.what());

		return REC_ERROR_JSON_PARSER_EXCEPTION; 
    }

    return ERROR_OK;
}

_process_info *ServerBiz::get_process_info(string sid)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "sid", sid.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(ERROR_OK == ret) {

		_process_info *info = new _process_info;

		parser(item, info);

		return info;
    }
    else
    {
		return NULL;
    }
}

int ServerBiz::remove_process_by_stream(string sid)
{
    int ret = ERROR_OK;

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_PROCESS);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "sid", sid.c_str());
   
    if(!MongoAdaptor::remove(collection, query)) {

		Log::Error("mongodb remove sid: %s failed", sid.c_str());	

		ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::remove_process_by_id(int id)
{
    int ret = ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_INT32(query, "pid", id);

    if(!MongoAdaptor::remove(collection, query)) {

		Log::Error("mongodb remove pid: %d failed", id);

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ServerBiz::remove_host()
{
    int ret = ERROR_OK;

    Log::Info("remove host %s", _host.c_str());

    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, MONGODB_DATABASE, MONGODB_COLLECTION_HOSTINFO);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "host", _host.c_str());

    if(!MongoAdaptor::remove(collection, query)) {

		Log::Error("mongodb remove host: %s failed", _host.c_str());

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

