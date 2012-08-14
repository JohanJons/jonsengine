#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	EngineSettings::EngineSettings() : mLogToFile(false), mLogToSTDOut(false), mLogToFileLocation("")
	{
	}

	/*
	 * Logging
	 */
	void EngineSettings::SetLogToFile(bool res)
	{
		mLogToFile = res;
	}

	void EngineSettings::SetLogToFile(bool res, const std::string& fileLocation)
	{
		mLogToFileLocation = fileLocation;

		SetLogToFile(res);
	}

	void EngineSettings::SetLogToSTDOut(bool res)
	{
		mLogToSTDOut = res;
	}

	bool EngineSettings::GetLogToFile() const { return mLogToFile; }
	std::string EngineSettings::GetLogToFileLocation() const { return mLogToFileLocation; }
	bool EngineSettings::GetLogToSTDOut() const { return mLogToSTDOut; }
}