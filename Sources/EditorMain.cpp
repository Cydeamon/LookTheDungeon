#define STB_IMAGE_IMPLEMENTATION

#include <Engine/GlobalMacro.h>
#include <CMakeConfig.h>
#include <iostream>
#include <exception>
#include <signal.h>
#include "Game/Game.h"
#include "Editor/Editor.h"

#if (IS_WINDOWS)
#include <windows.h>
#elif (IS_LINUX)
#include <gtk/gtk.h>
#endif

int main()
{
    #if (IS_WINDOWS)
    {
        SetConsoleOutputCP(CP_UTF8);

        // EXE meta information
        HMODULE hModule = GetModuleHandle(NULL);
        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
        HGLOBAL hResourceData = LoadResource(hModule, hResource);
        LPVOID pResourceData = LockResource(hResourceData);
        DWORD dwSize = SizeofResource(hModule, hResource);
    }
    #endif

    Editor *editor = nullptr;

    try
    {
        editor = new Editor();
        editor->Run();
    }
    catch (const std::exception &e)
    {
        if (editor)
            delete editor;

        #if(IS_WINDOWS)
        {
            std::string title = std::string(PROJECT_LABEL) + ". Fatal error";
            MessageBoxA(NULL, e.what(), title.c_str(), MB_OK | MB_ICONERROR);
        }
        #elif (IS_LINUX)
        {
            gtk_init(NULL, NULL);
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s. Fatal error", PROJECT_LABEL);
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", e.what());
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            gtk_main_quit();
        }
        #endif

        std::cerr << e.what() << '\n';
        return 1;
    }

    delete editor;
    return 0;
}
