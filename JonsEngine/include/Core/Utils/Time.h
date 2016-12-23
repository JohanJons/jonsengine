#pragma once

#include <chrono>

namespace JonsEngine
{
    typedef std::chrono::microseconds Microseconds;
    typedef std::chrono::milliseconds Milliseconds;
    typedef std::chrono::seconds Seconds;
    typedef std::chrono::minutes Minutes;
    typedef std::chrono::hours Hours;

	template <typename TIME_FROM, typename TIME_TO>
	TIME_TO CastTimeUnit(const TIME_FROM from)
	{
		return std::chrono::duration_cast<TIME_TO>(from);
	}

	template <typename TIME_FROM>
	Microseconds TimeInMicroseconds(const TIME_FROM from)
	{
		return CastTimeUnit<TIME_FROM, Microseconds>(from);
	}

	template <typename TIME_FROM>
	Milliseconds TimeInMilliseconds(const TIME_FROM from)
	{
		return CastTimeUnit<TIME_FROM, Milliseconds>(from);
	}

	template <typename TIME_FROM>
	Seconds TimeInSeconds(const TIME_FROM from)
	{
		return CastTimeUnit<TIME_FROM, Seconds>(from);
	}

	template <typename TIME_FROM>
	Minutes TimeInMinutes(const TIME_FROM from)
	{
		return CastTimeUnit<TIME_FROM, Minutes>(from);
	}

	template <typename TIME_FROM>
	Hours TimeInHours(const TIME_FROM from)
	{
		return CastTimeUnit<TIME_FROM, Hours>(from);
	}

	template <typename TIME_UNIT>
	float ConvertTimeUnitToFloat(const TIME_UNIT duration)
	{
		return std::chrono::duration<float, TIME_UNIT::period>(duration).count();
	}
}