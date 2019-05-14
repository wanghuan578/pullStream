
#include <log4cplus/logger.h>    
#include <log4cplus/consoleappender.h>    
#include <log4cplus/fileappender.h>    
#include <log4cplus/layout.h>    
#include <log4cplus/configurator.h>    
    
#include "log.h"    

#define  LOG_MAX_PARA_STR_LEN   512


Logger Log::logger;
    
void Log::Init(string config, bool daemonized)    
{    
    if (daemonized) {

  	config.append("/log4cplus/log-d.conf");
        PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(config.c_str()));
    }    
    else {

	config.append("/log4cplus/log.conf");    
        PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(config.c_str()));
    }

	logger = Logger::getInstance(LOG4CPLUS_TEXT("logmain"));
}

void Log::Trace(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};
	
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
	va_end(argptr);

	LOG4CPLUS_TRACE(logger, parameters);
}

void Log::Debug(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
        va_end(argptr);

        LOG4CPLUS_DEBUG(logger, parameters);
}

void Log::Info(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
        va_end(argptr);

        LOG4CPLUS_INFO(logger, parameters);
}

void Log::Warn(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
        va_end(argptr);

        LOG4CPLUS_WARN(logger, parameters);
}

void Log::Error(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
        va_end(argptr);

        LOG4CPLUS_ERROR(logger, parameters);
}

void Log::Fatal(const char *fmt,...)
{
	char parameters[LOG_MAX_PARA_STR_LEN] = {0};

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(parameters, LOG_MAX_PARA_STR_LEN, fmt, argptr);
        va_end(argptr);

        LOG4CPLUS_FATAL(logger, parameters);
}


