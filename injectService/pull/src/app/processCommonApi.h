
#ifndef __PROCESS_CTL_MODULE_H__
#define __PROCESS_CTL_MODULE_H__

#include <stdlib.h>
#include <string>
#include "common.h"

//typedef enum message_type
//{
//    MSG_TYPE_SIGCHIL_NOTIFY,
//}_message_type;

//typedef struct message
//{
//    _message_type type;
//    void *data;
//}_message;

using namespace std;

class ProcessCtlModule
{
public:
    ProcessCtlModule() {}
    ~ProcessCtlModule() {}

    static int spawn_process(_process_info &process);
    static int process_reload(string &sid);
    static int spawn_process_i(_process_info &process);
    static int remove_process(_process_info &process);
    static int terminate_process(_process_info &info);
    static bool check_system_process_exist(int pid); 

private:
    static int process_reload_i(_process_info &process);
};

#endif
