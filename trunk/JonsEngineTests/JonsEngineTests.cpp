#include "include/Core/Engine.h"

// memory leak detector
#include <vld.h>

// google unit test framework
#include "gtest/gtest.h"

#include "UnitTest/TestFixtures/RunningEngineFixture.h"


#include <iostream>
#include <fstream>
#include <tchar.h>


using namespace JonsEngine;

int32_t _tmain(int32_t argc, _TCHAR* argv[])
{
	std::cout << "Press key to begin" << std::endl;
	getchar();

	::testing::AddGlobalTestEnvironment(new RunningEngineFixture());

	// start basic unit testing
	::testing::InitGoogleTest(&argc,argv);
	if (!RUN_ALL_TESTS())
		std::cout << "TESTING SUCCESSFULL" << std::endl;
	else
		std::cout << "TESTING FAILED" << std::endl;

	// run an example usage/integrated test


	std::cout << "Press key to terminate" << std::endl;
	getchar();
	return 0;
}

