#include "include/Core/Logging/JonsStreamBuf.h"

namespace JonsEngine
{
	JonsStreamBuf::JonsStreamBuf() : std::streambuf()
	{
		
	}

	JonsStreamBuf::~JonsStreamBuf()
	{
		mStreamBufs.clear();
		mLine.clear();
	}

	void JonsStreamBuf::AddStream(std::streambuf* sb)
	{
		if (sb)
			mStreamBufs.push_back(sb);
	}

	void JonsStreamBuf::RemoveStream(std::streambuf* sb)
	{
		if (sb)
			mStreamBufs.remove(sb);
	}

	bool JonsStreamBuf::IsStreamAdded(std::streambuf* sb) const
	{
		if (sb)
			return (std::find(mStreamBufs.begin(),mStreamBufs.end(),sb) != mStreamBufs.end());
		else
			return false;
	}

	int32_t JonsStreamBuf::overflow(int32_t c)
	{
		int32_t r1 = 0, r2 = 0;

		if (c == EOF)
		   return !EOF;
		else
		{
			mLine += c;
			return r1 == EOF || r2 == EOF ? EOF : c;
		}
	}

	int32_t JonsStreamBuf::sync()
	{
		int32_t res = 0;

		for(std::list<std::streambuf*>::iterator it = mStreamBufs.begin(); it != mStreamBufs.end(); ++it)
		{
			if (*it)
			{
				(*it)->sputn(mLine.c_str(),mLine.length());
				res &= (*it)->pubsync();
			}
		}				

		mLine.clear();

		return res == 0 ? 0 : -1;
	}
}
