#include "include/AssetImporter.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <iostream>

using namespace JonsAssetImporter;


AssetImporter::Command ParseCommand(const std::string& command)
{
    if (!command.compare("import"))
        return AssetImporter::Command::IMPORT;
    else
        return AssetImporter::Command::UNKNOWN_COMMAND;
}

int _tmain(int argc, _TCHAR* cmds[])
{
    AssetImporter::Command command(AssetImporter::Command::UNKNOWN_COMMAND);
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

    AssetImporter assetImporter;
    assetImporter.ParseCommands(command, parameters);
        
    std::cout << assetImporter.GetErrorLog() << std::endl;

	return 0;
}