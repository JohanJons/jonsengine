// JonsEngineTests.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "TestManager.h"
#include <iostream>
#include <fstream>

#include <vld.h>

using namespace JonsEngine;


int32_t _tmain(int32_t argc, _TCHAR* argv[])
{
	std::cout << "Press key to begin" << std::endl;
	getchar();

	TestManager testMgr(10);
	testMgr.RunAllTests();

	std::cout << "Press key to terminate" << std::endl;
	getchar();
	return 0;
}

