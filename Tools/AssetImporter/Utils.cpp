#include "Utils.h"

#include "FreeImage.h"
#include <sstream>

namespace JonsAssetImporter
{
    static std::string gLog;


    void Log( const std::string& msg )
    {
		gLog.append("-JonsAssetImporter: ");
		gLog.append(msg);
		gLog.append("\n");
    }

	void Log( const std::exception& e )
	{
		std::string error( "ERROR: " );
		error.append( e.what() );
		Log( error );
	}

	std::string FlushLog()
	{
		std::string ret = gLog;
		gLog.clear();

		return ret;
	}
}