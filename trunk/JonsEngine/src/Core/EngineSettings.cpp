#include "EngineSettings.h"

namespace JonsEngine
{
	EngineSettings::EngineSettings() : mLogToFile(false), mLogToSTDOut(false), mLogToFileLocation(""),
										mAllocatorBackEnd(DLMALLOC)
	{
	}

	/*
	 * Logging
	 */
	void EngineSettings::SetLogToFile(bool res)
	{
		mLogToFile = res;
	}

	void EngineSettings::SetLogToFile(bool res, std::string fileLocation)
	{
		mLogToFileLocation = fileLocation;

		SetLogToFile(res);
	}

	void EngineSettings::SetLogToSTDOut(bool res)
	{
		mLogToSTDOut = res;
	}

	bool EngineSettings::GetLogToFile() { return mLogToFile; }
	std::string EngineSettings::GetLogToFileLocation() { return mLogToFileLocation; }
	bool EngineSettings::GetLogToSTDOut() { return mLogToSTDOut; }


	/*
	 * Memory
	 */
	void EngineSettings::SetAllocatorBackEnd(Allocator_BackEnd allocatorBackend)
	{
		mAllocatorBackEnd = allocatorBackend; 
	}

	Allocator_BackEnd EngineSettings::GetAllocatorBackEnd() { return mAllocatorBackEnd; }
}