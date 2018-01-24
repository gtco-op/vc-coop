#pragma once
#include "VCCoopConfig.h"

#include <Windows.h>
#include <regex>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace Logger
{
	extern const std::string path;
	extern std::ofstream out;

	void flush() { out.flush(); }

	namespace // detail
	{
		char buffer[256], cstr[256]{};
		char* timeFormat = DEFAULT_TIME_FORMAT;
		auto now = std::time(nullptr);
		va_list args;

		std::string GetExecutablePath()
		{
			char buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, MAX_PATH);
			std::string::size_type pos = std::string(buffer).find_last_of("\\/");
			return std::string(buffer).substr(0, pos);
		}
		std::string time_stamp()
		{
			now = std::time(nullptr);
			return std::strftime(cstr, sizeof(cstr), timeFormat, std::localtime(&now)) ? cstr : "";
		}
		std::string path_to_session_log_file()
		{
			std::string exePath = GetExecutablePath();
			exePath.append("\\");

			static const std::string log_dir = exePath;
			static const std::string log_file_name = "VCCOOP_log.txt";

			return log_dir + time_stamp() + '_' + log_file_name;
		}
		void Log(char * format, ...)
		{
			va_start(args, format);
			vsnprintf(buffer, 255, format, args);
			va_end(args);

			timeFormat = LOGGER_TIME_FORMAT;

			std::string buf("[");
			buf.append(time_stamp());
			buf.append("]");
			buf.append(buffer);
			out << buf;
			flush();

			timeFormat = DEFAULT_TIME_FORMAT;

#ifdef VCCOOP_DEBUG
			printf(buf.c_str());
#endif
		}
	}

	const std::string path = path_to_session_log_file();
	std::ofstream out = std::ofstream(path);
}