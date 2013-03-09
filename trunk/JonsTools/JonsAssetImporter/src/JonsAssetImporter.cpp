#include "include/AssetImporter.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <iostream>

int _tmain(int argc, _TCHAR* cmds[])
{
    std::vector<std::string> commands;
    int i = 0;

    #ifdef _DEBUG
        std::cout << "-JonsAssetImporter: Attach debugger then/or press a key...";
        std::cin.get();
    #endif

    while (++i < argc)
        commands.push_back(std::string(cmds[i]));

    std::string result = JonsAssetImporter::ParseCommands(commands);
    std::cout << result << std::endl;

	return 0;
}