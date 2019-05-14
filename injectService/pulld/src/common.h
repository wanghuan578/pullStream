
#ifndef __ACC_INTERNAL_H__
#define __ACC_INTERNAL_H__

#include "basic_type.h"
#include <string>

using namespace std;


//#define MONGODB_CLUSTER "mongodb://migurecord:qazwsx2018edc@10.200.43.63:30000,10.200.43.32:30000,10.200.43.37:30000/rec_distribute"
#define MONGODB_CLUSTER "mongodb://10.200.41.89:27017"

typedef enum event_t
{
    EV_CREATE_PROCESS,
    EV_REMOVE_PROCESS,
}_event_t;

typedef struct process_info
{
    string sid;
    int pid;
    int ppid;
    string src_url;
    string m3u8_path;
    string type;
    string status;
    string host;
}_process_info;

typedef struct rec_event
{
    process_info process;
    _event_t type;
}_event;

enum error_code
{
	REC_ERROR_OK = 0,
	REC_ERROR_STREAM_EXIST,
	REC_ERROR_PORCESS_INFO_NONE,
	REC_ERROR_GET_COLLECTION_FAILED,
	REC_ERROR_FORK_FAILED,
	REC_ERROR_STREAM_NOT_EXIST,
	REC_ERROR_MONGODB,
	REC_ERROR_JSON_PARSER,
	REC_ERROR_INTERNAL,
	REC_ERROR_PORT_UNSPECIFIED,
	REC_ERROR_ROOT_PATH_UNSPECIFIED,
	REC_ERROR_M3U8_PATH_EXIST,
	REC_ERROR_JSON_PARSER_EXCEPTION,
};

typedef struct app_data
{
	char *host;
        char *port;
        char *root_path;
	char *ts_duration;
	char *log_tank;
}_app_data;

typedef struct m3u8_param
{
	string path;
	string sid;
	string file_path;
}_m3u8_param;


#endif
