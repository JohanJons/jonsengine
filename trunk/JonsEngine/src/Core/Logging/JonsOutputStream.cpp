#include "../../../Include/Core/Logging/JonsOutputStream.h"

namespace JonsEngine
{
	JonsOutputStream::JonsOutputStream(JonsStreamBuf* buf)
			: std::ostream(buf)
	{
		
	}

	JonsOutputStream::~JonsOutputStream()
	{
		
	}
}
