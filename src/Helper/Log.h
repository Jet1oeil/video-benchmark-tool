#ifndef HELPER_LOG_H_
#define HELPER_LOG_H_

#ifdef __GNUC__
  #define FORMAT(X, Y) __attribute__ ((format (printf, X, Y)))
#else
  #define FORMAT(X, Y)
#endif

#include <cstdarg>
#include <string>

namespace helper {
	class Log {
	public:
		Log() = delete;

		enum Level : int {
			Debug,
			Info,
			Warn,
			Error,
			Fatal,
		};

		static void setLevel(Level level);

		static void debug(const char *fmt, ...) FORMAT(1, 2);
		static void info(const char *fmt, ...) FORMAT(1, 2);
		static void warning(const char *fmt, ...) FORMAT(1, 2);
		static void error(const char *fmt, ...) FORMAT(1, 2);
		static void fatal(const char *fmt, ...) FORMAT(1, 2);

	private:
		static void log(Level level, const char *fmt, va_list ap);

	private:
		static Level s_level;
	};
}

#endif // HELPER_LOG_H_
