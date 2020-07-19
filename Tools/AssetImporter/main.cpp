#include "AssetImporter.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <iostream>

using namespace JonsAssetImporter;

int _tmain(int argc, _TCHAR* cmds[])
{
    std::vector<std::string> parameters;

    #ifdef _DEBUG
        std::cout << "-JonsAssetImporter: Attach debugger then/or press a key...";
        std::cin.get();
    #endif

	int i = 0;
	while ( ++i < argc )
		parameters.push_back( std::string( cmds[ i ] ) );

    // increment before due to program name counts as parameter aswell
    ParseResult result = ParseCommands( parameters );
    
    // echo to console
    std::cout << result.mLog << std::endl;
    if (!result.mSuccess)
        std::cout << "-JonsAssetImporter: FAILED!" << std::endl;

	#ifdef _DEBUG
		std::cout << "-JonsAssetImporter: Finished!" << std::endl;
		std::cin.get();
	#endif

	return result.mSuccess ? 0 : -1;
}