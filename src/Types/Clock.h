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
