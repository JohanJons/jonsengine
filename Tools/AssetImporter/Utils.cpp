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

    const std::string& GetLog()
    {
        return gLog;
    }
}