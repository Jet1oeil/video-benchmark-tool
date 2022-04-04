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
