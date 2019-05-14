
#include "event/event.h"
#include "event/evhttp.h"
#include "event/event2/http.h"
#include "json/json.h"
#include "rest_api.h"
#include "processCommonApi.h"
#include <string.h>
#include <assert.h>
#include "global.h"
#include "mongoCommonApi.h"
#include "log4cplus/log.h"
#include "serverBiz.h"

using namespace std;

CRestApi::CRestApi() 
{

}

CRestApi::~CRestApi() 
{

}

int CRestApi::init(CIniReader &ini_reader, char *host) 
{
	MongoAdaptor::init();
	
	app_data.port = ini_reader.GetInt("system", "listen");
	
	if(0 == app_data.port) {
		
		Log::Error("listen port unspecified");

		exit(0);
	}
		
	app_data.host = host;

	char *address = ini_reader.GetStr("mongodb", "address");
	
	if (NULL == address) {

		Log::Error("mongdb address unspecified");

		exit(0);
	}
	
	Log::Info("get mongo address %s", address);

	ServerBiz::Instance()->set_mongodb(address);
	
	if(!ServerBiz::Instance()->host_register(app_data.host, app_data.port)) {

		Log::Error("duplicate host register");

		exit(0);
	}
	
	return 0;
}
	
int CRestApi::run() 
{	
	event_init();
	
	struct evhttp *httpd = evhttp_start("0.0.0.0", app_data.port);
	
	if(NULL == httpd) {

		Log::Error("bind port: %s failed", app_data.port);

		exit(0);
	}

	evhttp_set_cb(httpd, "/pull/inject/start", start, (void*)(&app_data));
	
	Log::Info("[/pull/inject/start] register");
	
	evhttp_set_cb(httpd, "/pull/inject/stop", stop, (void*)(&app_data));
	
	Log::Info("[/pull/inject/stop] register");
	
	evhttp_set_cb(httpd, "/pull/inject/i/start", start_i, (void*)(&app_data));
	
	Log::Info("[/pull/inject/i/start] register");
	
	evhttp_set_cb(httpd, "/pull/inject/i/stop", stop_i, (void*)(&app_data));
	
	Log::Info("[/pull/inject/i/stop] register");
	
	evhttp_set_cb(httpd, "/health/live", check_alive_i, (void*)(&app_data));
	
	Log::Info("[/health/live] register");
	
	Log::Info("listen %d ...", app_data.port);

	event_dispatch();
	
	evhttp_free(httpd);
	
	return 0;
}

int CRestApi::shutdown() 
{
	event_loopbreak();

	Log::Warn("libevent terminal");
	
	exit(0);

	return 0;
}

void CRestApi::start_i(struct evhttp_request *req, void *arg)
{
	int ret = 0;
	string output;
	string path;
	struct evbuffer *buffer = NULL;
	_process_info cmd_param;
	Json::Reader reader;
	Json::Value root;
	char tmp[256] = {0};
	char *body = NULL;
	_event ev;

	_app_data *data = (_app_data*) arg;

	assert(NULL != data);

	buffer = evbuffer_new();

	memset(tmp, 0x00, sizeof(tmp));
	body = (char *) EVBUFFER_DATA(req->input_buffer);

	if(NULL == body) {
		    
		Log::Info("create/i pull body null");

		goto HTTP_BODY_NULL;
	}

	snprintf(tmp, sizeof(tmp), "body = %s", body);
	Log::Info("%s", tmp);
	
	try
	{
		if (!reader.parse(body, root, false))
		{
		    Log::Error("create/i pull body parser failed");

			goto INPUT_NO_MATCH;
		}

		cmd_param.sid = root["sid"].asString();
		cmd_param.type = root["type"].asInt();

	}
	catch(std::exception &ex)
	{
		Log::Error("create/i pull parser exception detail = %s\n", ex.what());
		
		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", 403);
		output.append("{\"ret\":");
		output.append(tmp);
		output.append(",\"");
		output.append("msg\":\"");
		output.append(ex.what());
		output.append("\"}");

		evbuffer_add_printf(buffer, "%s", output.c_str());
		evhttp_send_reply(req, 403, "%s", buffer);
		evbuffer_free(buffer);
		return;
	}
	
	Log::Info("create/i process sid = %s", cmd_param.sid.c_str());

	ev.process = cmd_param;
	ev.type = EV_RELOAD_PROCESS;

	ret = ServerBiz::Instance()->put_data(ev);

	output.append("{\"ret\":");
	output.append("0");
	output.append(",\"");
	output.append("msg\":\"");
	output.append("ok\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, HTTP_OK, "OK", buffer);
	evbuffer_free(buffer);
	return;

INPUT_NO_MATCH:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("input not match\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "paramter not match", buffer);
	evbuffer_free(buffer);
	return;

HTTP_BODY_NULL:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", 400);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("http post body null\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "http body null", buffer);
	evbuffer_free(buffer);
	return;
}

void CRestApi::stop_i(struct evhttp_request *req, void *arg)
{
	int ret = 0;
	string output;
	string path;
	struct evbuffer *buffer = NULL;
	_process_info cmd_param;
	Json::Reader reader;
	Json::Value root;
	char tmp[256] = {0};
	char *body = NULL;
	_event ev;

	_app_data *data = (_app_data*) arg;
	assert(NULL != data);

	buffer = evbuffer_new();

	memset(tmp, 0x00, sizeof(tmp));
	body = (char *) EVBUFFER_DATA(req->input_buffer);

	if(NULL == body) {
		
		Log::Error("stop/i pull body null");

		goto HTTP_BODY_NULL;
	}

	snprintf(tmp, sizeof(tmp), "body = %s", body);
	Log::Info("%s", tmp);
	
	try
	{
		if (!reader.parse(body, root, false))
		{
			Log::Error("stop/i record body parser failed");

			goto INPUT_NO_MATCH;
		}
		
        cmd_param.pid = root["pid"].asInt();
	}
	catch(std::exception &ex)
    {
		Log::Error("stop/i record parser exception detail = %s\n", ex.what());

		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", 403);
		output.append("{\"ret\":");
		output.append(tmp);
		output.append(",\"");
		output.append("msg\":\"");
		output.append(ex.what());
		output.append("\"}");

		evbuffer_add_printf(buffer, "%s", output.c_str());
		evhttp_send_reply(req, 403, "%s", buffer);
		evbuffer_free(buffer);
		return;
    }
	
	Log::Info("stop/i record terminal process pid = %d", cmd_param.pid);

	ev.process = cmd_param;
	ev.type = EV_TERMINATE_PROCESS;

	ret = ServerBiz::Instance()->put_data(ev);

	output.append("{\"ret\":");
	output.append("0");
	output.append(",\"");
	output.append("msg\":\"");
	output.append("ok\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, HTTP_OK, "OK", buffer);
	evbuffer_free(buffer);
	return;

INPUT_NO_MATCH:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("input not match\"}");
	
	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "paramter not match", buffer);
	evbuffer_free(buffer);
	return;

HTTP_BODY_NULL:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", 400);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("http post body null\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "http body null", buffer);
	evbuffer_free(buffer);
	return;
}

void CRestApi::start(struct evhttp_request *req, void *arg)
{
	int ret = 0;
	string output;
	string path;
	struct evbuffer *buffer = NULL;
	_process_info cmd_param;
	Json::Reader reader;
    Json::Value root;
	char tmp[256] = {0};
	char *body = NULL;
	_event ev;

	_app_data *data = (_app_data*) arg;

	assert(NULL != data);

	if (NULL == data) {

		goto INTERNAL_SERVER_ERROR;
	}

	buffer = evbuffer_new();

	memset(tmp, 0x00, sizeof(tmp));
	
	body = (char *) EVBUFFER_DATA(req->input_buffer);
		
	if(NULL == body) {
	
		Log::Info("start pull: body null");

		goto HTTP_BODY_NULL;
	}

    snprintf(tmp, sizeof(tmp), "body = %s", body);
	
	Log::Info("%s", tmp);

	try
	{
		if (!reader.parse(body, root, false))
		{
		    Log::Error("start pull: post body parser failed");

		    goto INPUT_NO_MATCH;
		}

		cmd_param.src_url = root["src"].asString();
		cmd_param.dest_url = root["dest"].asString();
		cmd_param.sid = root["sid"].asString();
		cmd_param.type = root["type"].asInt();

		if(cmd_param.src_url.empty()) {
		
		    Log::Error("start pull: src null");

		    goto INPUT_NO_MATCH;
		}

		if(cmd_param.dest_url.empty()) {
		
		    Log::Error("start pull: dest null");

		    goto INPUT_NO_MATCH;
		}

		if(cmd_param.sid.empty()) {
		
		    Log::Error("start pull: sid null");

		    goto INPUT_NO_MATCH;
		}

		
		if(TP_RTMP_2_RTMP != cmd_param.type && 
		   TP_RTSP_2_RTMP != cmd_param.type) {
				
			Log::Info("start pull: translate type unsupport");

			goto TRANSLATE_TYPE_UNSUPPORT;
		}

 		cmd_param.status = "active";
		cmd_param.host = data->host;
	}
	catch(std::exception &ex)
	{
		Log::Error("start pull parser exception detail = %s\n", ex.what());
		
		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", 403);
		output.append("{\"ret\":");
		output.append(tmp);
		output.append(",\"");
		output.append("msg\":\"");
		output.append(ex.what());
		output.append("\"}");

		evbuffer_add_printf(buffer, "%s", output.c_str());
		evhttp_send_reply(req, 403, "%s", buffer);
		evbuffer_free(buffer);
		return;
	}

	Log::Info("start pull sid = %s", cmd_param.sid.c_str());
		
	ev.process = cmd_param;
	ev.type = EV_CREATE_PROCESS;
	
	ret = ServerBiz::Instance()->put_data(ev);
		
	if(ERROR_STREAM_EXIST == ret) {
		
	    Log::Error("create process failed, sid %s exist", cmd_param.sid.c_str());   

	    goto STREAM_EXIST;
	}

	output.append("{\"ret\":");
	output.append("0");
	output.append(",\"");
	output.append("msg\":\"");
	output.append("SUCCESS\"}");
		
	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, HTTP_OK, "OK", buffer);
	evbuffer_free(buffer);
	return;

STREAM_EXIST:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("stream exist\"}");
	evbuffer_add_printf(buffer, "%s", output.c_str());

	evhttp_send_reply(req, 403, "stream exist", buffer);
	evbuffer_free(buffer);
	return;

INPUT_NO_MATCH:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("input not match\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "paramter not match", buffer);
	evbuffer_free(buffer);
	return;

TRANSLATE_TYPE_UNSUPPORT:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("translate type unsupport\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "paramter not match", buffer);
	evbuffer_free(buffer);
	return;

HTTP_BODY_NULL:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", 400);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("http post body null\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 403, "http body null", buffer);
	evbuffer_free(buffer);
	return;

INTERNAL_SERVER_ERROR:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", 500);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("internal server error\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 500, "internal server error", buffer);
	evbuffer_free(buffer);
	return;
}

void CRestApi::stop(struct evhttp_request *req, void *arg)
{
	int ret = 0;
	string output;
	struct evbuffer *buffer;
	_process_info cmd_param;
	Json::Reader reader;
	Json::Value root;
	char tmp[256];
	char *body = NULL;
	_event ev;

	//_app_data *data = (_app_data*) arg;

	buffer = evbuffer_new();

	memset(tmp, 0x00, sizeof(tmp));
	body = (char *) EVBUFFER_DATA(req->input_buffer);

	if(NULL == body) {

	    Log::Error("stop record body null");

	    goto HTTP_BODY_NULL; 
	}

	snprintf(tmp, sizeof(tmp), "body = %s", body);
	Log::Info("%s", tmp);

	try
	{
		if (!reader.parse(body, root, false))
		{
			Log::Error("stop pull body parser failed");

			goto INPUT_NO_MATCH;
		}

		cmd_param.sid = root["sid"].asString();
		
		if(cmd_param.sid.empty()) {

		    Log::Error("stop pull sid null");

		    goto INPUT_NO_MATCH;
		}
	}
	catch(std::exception &ex)
	{
		Log::Error("stop pull json parser exception: %s", ex.what());

		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", 403);
		output.append("{\"ret\":");
		output.append(tmp);
		output.append(",\"");
		output.append("msg\":\"");
		output.append(ex.what());
		output.append("\"}");

		evbuffer_add_printf(buffer, "%s", output.c_str());
		evhttp_send_reply(req, 403, "%s", buffer);
		evbuffer_free(buffer);
		return;
	}

	Log::Info("stop process sid = %s", cmd_param.sid.c_str());
	
	ev.process = cmd_param;
	ev.type = EV_REMOVE_PROCESS;
	ret = ServerBiz::Instance()->put_data(ev);

	output.append("{\"ret\":");
	output.append("0");
	output.append(",\"");
	output.append("msg\":\"");
	output.append("SUCCESS\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, HTTP_OK, "OK", buffer);
	evbuffer_free(buffer);
	return;

INPUT_NO_MATCH:
	memset(tmp, 0x00, sizeof(tmp));
 	snprintf(tmp, sizeof(tmp), "%d", ret);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("input not match\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 400, "paramter not match", buffer);
	evbuffer_free(buffer);
	return;
        
HTTP_BODY_NULL:
	memset(tmp, 0x00, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", 403);
	output.append("{\"ret\":");
	output.append(tmp);
	output.append(",\"");
	output.append("msg\":\"");
	output.append("http post body null\"}");

	evbuffer_add_printf(buffer, "%s", output.c_str());
	evhttp_send_reply(req, 403, "http body null", buffer);
	evbuffer_free(buffer);
	return;
}

void CRestApi::check_alive_i(struct evhttp_request *req, void *arg)
{
    string output;
    struct evbuffer *buffer = NULL;

    output.append("{\"ret\":");
    output.append("0");
    output.append(",\"");
    output.append("msg\":\"");
    output.append("alive\"}");

    evbuffer_add_printf(buffer, "%s", output.c_str());
    evhttp_send_reply(req, HTTP_OK, "OK", buffer);
    evbuffer_free(buffer);    
}
