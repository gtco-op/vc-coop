#include "server.h"

CLogger::CLogger()
{
	TimeFormat = DEFAULT_TIME_FORMAT;
	path = path_to_session_log_file();
	out = std::ofstream(path);

	this->Log("[CCore] %s %s loaded.\n", VCCOOP_NAME, VCCOOP_VER);
	this->Log("[CCore] Build: %s\n", GIT_BUILD);
}
CLogger::~CLogger()
{

}
void CLogger::Out(const char * szBuf)
{
	out << szBuf;
	FlushBuffer();
}
void CLogger::FlushBuffer()
{
	out.flush();
}
void CLogger::Log(char * format, ...)
{
	char buffer[256];

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);


	// Timestamp
	std::string buf("[");
	buf.append(time_stamp(LOGGER_TIME_FORMAT));
	buf.append("]");

	// Application Name
	buf.append("[");
	buf.append(VCCOOP_NAME);
	buf.append("]");

	// Message
	buf.append(buffer);
	Out(buf.c_str());

#if defined(VCCOOP_DEBUG) || !defined(_MSC_VER)
	printf(buf.c_str());
#endif
}
std::string GetExecutablePath()
{
	char buffer[MAX_PATH];
	
	// TODO: Find replacement
#if defined(_MSC_VER)
	GetModuleFileName(NULL, buffer, MAX_PATH);
#endif

	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
std::string time_stamp(char* TimeFormat)
{ 
	auto now = std::time(nullptr);
	char cstr[256]{};
	return std::strftime(cstr, sizeof(cstr), TimeFormat, std::localtime(&now)) ? cstr : "";
}
std::string path_to_session_log_file()
{
	std::string exePath = GetExecutablePath();
#if defined(_MSC_VER)
	exePath.append("\\Logs\\");
#else
	exePath.append("./Logs/");
#endif

	static const std::string log_dir = exePath;
	static const std::string log_file_name = "VCCOOPServer_log.txt";

	return log_dir + time_stamp(DEFAULT_TIME_FORMAT) + '_' + log_file_name;
}
