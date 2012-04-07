#include "EngineSettings.h"

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
void EngineSettings::SetUseDLMalloc(bool res)
{
	mUseDLMalloc = res; 
}

bool EngineSettings::GetUseDLMalloc() { return mUseDLMalloc; }