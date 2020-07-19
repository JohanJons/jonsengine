#include "Utils.h"

#include "FreeImage.h"
#include <sstream>

namespace JonsAssetImporter
{
    static std::string gLog;


    void Log(const std::string& msg)
    {
		gLog.append("-JonsAssetImporter: ");
		gLog.append(msg);
		gLog.append("\n");
    }

	std::string FlushLog()
	{
		std::string ret = gLog;
		gLog.clear();

		return ret;
	}
}