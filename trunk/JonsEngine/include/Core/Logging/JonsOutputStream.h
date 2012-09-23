#pragma once

#include <iostream>

#include "include/Core/Logging/JonsStreamBuf.h"

namespace JonsEngine
{
	class JonsOutputStream : public std::ostream
	{
	public:
		JonsOutputStream(JonsStreamBuf* buf);
		~JonsOutputStream();

	};
}
