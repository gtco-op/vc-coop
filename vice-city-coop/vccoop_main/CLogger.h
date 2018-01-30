#ifndef _CLOGGER_H_DEFINED
#define _CLOGGER_H_DEFINED
#include "VCCoopConfig.h"

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <ctime>
#include <time.h>

std::string GetExecutablePath();
std::string time_stamp(char* TimeFormat);
std::string path_to_session_log_file();

class CLogger
{
public:
	std::string path;
	std::ofstream out;

	va_list args;

	char* TimeFormat;

	void FlushBuffer();
	void Out(const char * szBuf);
	void Log(char * format, ...);

	CLogger();
	~CLogger();
};
#endif