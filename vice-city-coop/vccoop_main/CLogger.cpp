#include "main.h"

CLogger::CLogger(bool debug)
{
	this->debugLog = debug;
	this->TimeFormat = DEFAULT_TIME_FORMAT;
	
	if(debug)
		this->path = path_to_session_log_file().append("_debug.txt");
	else
		this->path = path_to_session_log_file();
	
	this->out = std::ofstream(path);
}
CLogger::~CLogger()
{
	this->Log("[CLogger] CLogger shutting down\n");
	exit(-1);
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

#ifdef VCCOOP_DEBUG
	if(!debugLog)
		gChat->AddChatMessage(buf.c_str());	printf(buf.c_str());
#endif

#ifdef VCCOOP_DEBUG_ENGINE
	if(gRender->gDebugScreen->gDevConsole != nullptr && debugLog)
		gRender->gDebugScreen->gDevConsole->AddLog(buf.c_str());
#endif
}
std::string GetExecutablePath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
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
	exePath.append("\\scripts\\vccoop\\Logs\\");

	static const std::string log_dir = exePath;
	static const std::string log_file_name = "VCCOOP_log.txt";

	return log_dir + time_stamp(DEFAULT_TIME_FORMAT) + '_' + log_file_name;
}
