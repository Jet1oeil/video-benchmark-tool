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

#ifndef TYPES_CLOCK_H_
#define TYPES_CLOCK_H_

#include <chrono>

namespace types {
	template <typename DefaultDurationType = std::chrono::milliseconds,
			typename ClockType = std::chrono::high_resolution_clock>
	class Clock {
		using TimePoint = typename ClockType::time_point;

	public:
		Clock()
		: m_start(ClockType::now())
		{

		}

		template <typename DurationType = DefaultDurationType>
		DurationType restart()
		{
			auto duration = getDuration();

			m_start = ClockType::now();

			return duration;
		}

		template <typename DurationType = DefaultDurationType>
		DurationType getDuration()
		{
			return std::chrono::duration_cast<DurationType>(ClockType::now() - m_start);
		}

	private:
		TimePoint m_start;
	};
}

#endif // TYPES_CLOCK_H_
