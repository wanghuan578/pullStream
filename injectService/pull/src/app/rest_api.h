
#ifndef __RESTFUL_API_H__
#define __RESTFUL_API_H__


#include "common.h"
#include "config/iniReader.h"
//#include "global_conf.h"

	class CRestApi
	{
	public:
		CRestApi();
		~CRestApi();
		
		int init(CIniReader &ini_reader, char *host);
		int run();
		int shutdown();

		static void start(struct evhttp_request *req, void *arg);
		static void stop(struct evhttp_request *req, void *arg);
		static void start_i(struct evhttp_request *req, void *arg);
		static void stop_i(struct evhttp_request *req, void *arg);
		static void check_alive_i(struct evhttp_request *req, void *arg);
		
	private:
		_app_data app_data;
	};


#endif
