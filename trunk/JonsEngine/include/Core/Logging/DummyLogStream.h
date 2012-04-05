#ifndef _DUMMY_LOG_STREAM_H
#define _DUMMY_LOG_STREAM_H

#include <iostream>

namespace JonsEngine
{

	class DummyLogStreamBuf : public std::streambuf
	{
	public:
		inline DummyLogStreamBuf() : std::streambuf()
		{

		}

		inline ~DummyLogStreamBuf()
		{

		}
	};

}


#endif