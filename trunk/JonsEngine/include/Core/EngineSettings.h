#ifndef _ENGINE_SETTINGS_H
#define _ENGINE_SETTINGS_H

#include <string>

#include "../../interface/EngineDefs.h"

namespace JonsEngine
{

	class EngineSettings
	{
	public:
		EngineSettings();

		// Logging
		void SetLogToFile(bool res);		// default: false
		void SetLogToFile(bool res,std::string fileLocation);
		void SetLogToSTDOut(bool res);		// default: false

		bool GetLogToFile();
		std::string GetLogToFileLocation();
		bool GetLogToSTDOut();

		// Memory
		void SetAllocatorBackEnd(Allocator_BackEnd allocatorBackend);		// default: Allocator_BackEnd.DLMALLOC

		Allocator_BackEnd GetAllocatorBackEnd();		

		// GameObject


		// Render


		// Audio


		// Input


	private:
		bool mLogToFile;
		bool mLogToSTDOut;
		std::string mLogToFileLocation;

		Allocator_BackEnd mAllocatorBackEnd;

	};

}

#endif