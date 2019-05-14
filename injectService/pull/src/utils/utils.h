
#ifndef __UTILS_H__
#define __UTILS_H__

#include <sys/types.h>

#define IP_SIZE 16

class CUtils
{
public:
    CUtils();
    ~CUtils();

    static void multi_mkdir(const char *dir);
    static int get_cur_path(char *str, char *o_path, int i_l);
	static bool try_lock_file(char *file, int &fd);
	static void write_pid_to_file(const char *file);
	static int daemon(const char *pid_path, pid_t &pid);
	static int get_pid_from_file(char *file, pid_t *o_pid);
	static int get_local_ip(const char *eth_inf, char *ip);
};
#endif
