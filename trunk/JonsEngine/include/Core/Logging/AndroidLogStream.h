#ifndef _ANDROID_LOG_STREAM_H
#define _ANDROID_LOG_STREAM_H

#ifdef ANDROID

#include <iostream>
#include <android/log.h>

#include "../../../Interface/EngineDefs.h"

namespace JonsEngine
{

	class AndroidLogStreamBuf : public std::streambuf
	{
	public:
		inline AndroidLogStreamBuf()
		{

		}

		inline ~AndroidLogStreamBuf()
		{

		}

		/* Currently writes everything to LOG_DEBUG... parsing string to check for
		 * log-level is probably too expensive.
		 * Will think of something later */
		inline std::streamsize xsputn (const char* s, std::streamsize n)
		{
			__android_log_print(ANDROID_LOG_DEBUG, EngineLogTag.c_str(), s);
			return n;
		}
		

	};

}

#endif

#endif
