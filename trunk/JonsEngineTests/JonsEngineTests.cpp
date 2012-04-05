// JonsEngineTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestManager.h"
#include <iostream>
#include <fstream>

using namespace JonsEngine;


int32_t _tmain(int32_t argc, _TCHAR* argv[])
{
	TestManager* testMgr = new TestManager(20);
	testMgr->RunAllTests();

	getchar();
	return 0;
}

