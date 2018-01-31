#pragma once
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