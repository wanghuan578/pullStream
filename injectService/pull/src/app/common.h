
#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

#include <string>

using namespace std;


typedef enum event_t
{
    EV_CREATE_PROCESS = 0,
    EV_RELOAD_PROCESS,
    EV_REMOVE_PROCESS,
    EV_TERMINATE_PROCESS,
}_event_t;

typedef enum translate_type
{
    TP_RTMP_2_RTMP = 0,
    TP_RTSP_2_RTMP,
}_translate_type_t;

typedef struct process_info
{
	process_info()
	{
		type = TP_RTMP_2_RTMP;
	}
    string sid;
    int pid;
    int ppid;
    string src_url;
    string dest_url;
    int type;
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
	ERROR_OK = 0,
	ERROR_STREAM_EXIST,
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
	ERROR_OPEN_PID_FILE,
	ERROR_TRANSLATE_TYPE_UNEXCEPTED,
};

typedef struct app_data
{
	string host;
    int port;
}_app_data;

#endif
