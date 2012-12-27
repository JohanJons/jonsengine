#include "include/Core/Engine.h"

// memory leak detector
#include <vld.h>

// google unit test framework
#include "gtest/gtest.h"

#include "UnitTest/TestFixtures/RunningEngineFixture.h"

#include <iostream>
#include <fstream>
#include <tchar.h>

#ifndef JONSENGINE_RUN_UNIT_TEST
#define JONSENGINE_RUN_UNIT_TEST 0
#endif


using namespace JonsEngine;


/*
 * To perform simple test and verification of the modules of JonsEngine 
 */


int32_t _tmain(int32_t argc, _TCHAR* argv[])
{
	std::cout << "Press key to begin" << std::endl;
	getchar();

	if (JONSENGINE_RUN_UNIT_TEST)
	{
		::testing::AddGlobalTestEnvironment(new RunningEngineFixture());

		::testing::InitGoogleTest(&argc,argv);
		if (!RUN_ALL_TESTS())
			std::cout << "TESTING SUCCESSFULL" << std::endl;
		else
			std::cout << "TESTING FAILED" << std::endl;
	}



	std::cout << "Press key to terminate" << std::endl;
	getchar();
	return 0;
}

