#ifndef _ENGINE_SETTINGS_H
#define _ENGINE_SETTINGS_H

#include <string>

class EngineSettings
{
public:
	// Logging
	void SetLogToFile(bool res);		// default: false
	void SetLogToFile(bool res,std::string fileLocation);
	void SetLogToSTDOut(bool res);		// default: false

	bool GetLogToFile();
	std::string GetLogToFileLocation();
	bool GetLogToSTDOut();

	// Memory
	void SetUseDLMalloc(bool res);		// default: true

	bool GetUseDLMalloc();		

	// GameObject


	// Render


	// Audio


	// Input


private:
	bool mLogToFile;
	bool mLogToSTDOut;
	std::string mLogToFileLocation;

	bool mUseDLMalloc;

};


#endif