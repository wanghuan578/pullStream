
#ifndef __LOG4CPULS_H__
#define __LOG4CPULS_H__

#include <log4cplus/logger.h>    
#include <log4cplus/loggingmacros.h>    
#include <string>

using namespace std;
using namespace log4cplus;    
using namespace log4cplus::helpers;    


class Log
{
public:
	static void Init(string config, bool daemonized);
	
	static void Trace(const char *fmt,...);
	static void Debug(const char *fmt,...);
	static void Info(const char *fmt,...);
	static void Warn(const char *fmt,...);
	static void Error(const char *fmt,...);
	static void Fatal(const char *fmt,...);

protected:
	Log();
	~Log();

private:
	static Logger logger;
};

#endif
