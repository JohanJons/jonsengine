#ifndef _JONS_OUTPUT_STREAM_H
#define _JONS_OUTPUT_STREAM_H

#include <iostream>

#include "JonsStreamBuf.h"

namespace JonsEngine
{
	class JonsOutputStream : public std::ostream
	{
	public:
		JonsOutputStream(JonsStreamBuf* buf);
		~JonsOutputStream();

	private:
		JonsStreamBuf* mStreamBuf;

	};
}

#endif