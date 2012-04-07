#include "../../../Include/Core/Utils/JonsTime.h"

namespace JonsEngine
{

	uint64_t GetTimeSinceEpoch()
	{
		#if defined _WIN32 || _WIN64
			return (uint64_t)timeGetTime();
		#else
			struct timeval tv;
			gettimeofday(&tv, 0); 
			return (((uint64_t)tv.tv_sec)*(uint64_t)1000) + (((uint64_t)tv.tv_usec)/(uint64_t)1000);
		#endif
	}

	int32_t GetSecond()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_sec;
	}

	int32_t GetMinute()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_min;
	}

	int32_t GetHour()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_hour;
	}

	int32_t GetDay()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_mday;

	}

	int32_t GetMonth()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_mon+1;
	}


	int32_t GetYear()
	{
		time_t t;
		tm* now;

		t = time(NULL);
		now = localtime(&t);

		return now->tm_year+1900;
	}

}