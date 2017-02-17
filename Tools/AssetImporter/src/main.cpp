#include "include/AssetImporter.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <iostream>

using namespace JonsAssetImporter;


Command ParseCommand(const std::string& command)
{
    if (!command.compare("import"))
        return Command::IMPORT;
    else
        return Command::UNKNOWN;
}

int _tmain(int argc, _TCHAR* cmds[])
{
    Command command(Command::UNKNOWN);
    std::vector<std::string> parameters;
    int i = 0;

    #ifdef _DEBUG
        std::cout << "-JonsAssetImporter: Attach debugger then/or press a key...";
        std::cin.get();
    #endif

    // increment before due to program name counts as parameter aswell
    if (++i < argc)
        command = ParseCommand(cmds[i]);

    while (++i < argc)
        parameters.push_back(std::string(cmds[i]));

    ParseResult result = ParseCommands(command, parameters);
    
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