#ifndef _ENGINE_SETTINGS_H
#define _ENGINE_SETTINGS_H

#include <string>

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{

	class EngineSettings
	{
	public:
		EngineSettings();

		// Logging
		void SetLogToFile(bool res);		// default: false
		void SetLogToFile(bool res, const std::string& fileLocation);
		void SetLogToSTDOut(bool res);		// default: false

		bool GetLogToFile() const;
		std::string GetLogToFileLocation() const;
		bool GetLogToSTDOut() const;

		// Memory	

		// GameObject


		// Render


		// Audio


		// Input


	private:
		bool mLogToFile;
		bool mLogToSTDOut;
		std::string mLogToFileLocation;

	};

}

#endif