#define STB_IMAGE_IMPLEMENTATION

#include <Engine/GlobalMacro.h>
#include <CMakeConfig.h>
#include <iostream>
#include <exception>
#include <signal.h>
#include "Game/Game.h"

#if (IS_WINDOWS)
#include <windows.h>
#endif

int main()
{
    #if (IS_WINDOWS)
        SetConsoleOutputCP(CP_UTF8);

        // EXE meta information
        HMODULE hModule = GetModuleHandle(NULL);
        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
        HGLOBAL hResourceData = LoadResource(hModule, hResource);
        LPVOID pResourceData = LockResource(hResourceData);
        DWORD dwSize = SizeofResource(hModule, hResource);
    #endif

    Game *game = nullptr;

    try
    {
        game = new Game();
        game->Run();
    }
    catch (const std::exception &e)
    {
        if (game)
            delete game;

        #if(IS_WINDOWS)
            std::string title = std::string(PROJECT_LABEL) + ". Fatal error";
            MessageBoxA(NULL, e.what(), title.c_str(), MB_OK | MB_ICONERROR);
        #endif

        std::cerr << e.what() << '\n';
        return 1;
    }

    delete game;
    return 0;
}
