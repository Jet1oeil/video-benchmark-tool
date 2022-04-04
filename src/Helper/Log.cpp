/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
