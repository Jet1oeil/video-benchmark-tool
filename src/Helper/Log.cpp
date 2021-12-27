#include "Log.h"

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <locale>
#include <sstream>
#include <thread>

namespace helper {
#if DEBUG
	Log::Level Log::s_level = Log::Debug;
#else
	Log::Level Log::s_level = Log::Info;
#endif

	namespace {
		const char *getStringFromLevel(Log::Level level) {
			switch (level) {
			case Log::Debug:
				return "Debug";

			case Log::Info:
				return "Info";

			case Log::Warn:
				return "Warn";

			case Log::Error:
				return "Error";

			case Log::Fatal:
				return "Fatal";
			}

			assert(false);
			return "?";
		}

		inline std::string getThreadID()
		{
			std::ostringstream oss;
			oss << std::this_thread::get_id();
			return oss.str();
		}
	}

	void Log::setLevel(Level level)
	{
		s_level = level;
	}

	void Log::debug(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		log(Level::Debug, fmt, ap);
		va_end(ap);
	}

	void Log::info(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		log(Level::Info, fmt, ap);
		va_end(ap);
	}

	void Log::warning(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		log(Level::Warn, fmt, ap);
		va_end(ap);
	}

	void Log::error(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		log(Level::Error, fmt, ap);
		va_end(ap);
	}

	void Log::fatal(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		log(Level::Fatal, fmt, ap);
		va_end(ap);

		std::abort();
	}

	void Log::log(Level level, const char *fmt, va_list ap) {
		if (level < s_level) {
			return;
		}

		std::time_t integerPart = std::time(nullptr);

		static constexpr std::size_t BufferSize = 1024;
		char buffer[BufferSize];
		std::strftime(buffer, BufferSize, "%F %T", std::localtime(&integerPart));

		std::fprintf(stderr, "[%s][%s][%s] ", getStringFromLevel(level), buffer, getThreadID().c_str());
		std::vfprintf(stderr, fmt, ap);
		std::fprintf(stderr, "\n");
	}
}
