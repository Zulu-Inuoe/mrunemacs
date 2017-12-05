/*
 * mrunemacs - emacs runner
 * Written in 2017 by Wilfredo Velázquez-Rodríguez <zulu.inuoe@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>

#define EMACS_CLASS_NAME L"Emacs"
#define HOME_VAR_NAME L"HOME"

wchar_t* GetEnv(const wchar_t* name)
{
    DWORD size = GetEnvironmentVariableW(name, NULL, 0);

    if(size == 0 && GetLastError() == ERROR_ENVVAR_NOT_FOUND)
    {
        return NULL;
    }

    wchar_t* ret = malloc((size) * sizeof(wchar_t));

    GetEnvironmentVariableW(name, ret, size);

    return ret;
}

int VariableExists(const wchar_t* name)
{
    return GetEnvironmentVariableW(name, NULL, 0) != 0;
}

void EnsureHome()
{
    if(VariableExists(HOME_VAR_NAME))
    {//Already set;
        return;
    }
    else if(1)
    {
        wchar_t* userProfileVar = GetEnv(L"USERPROFILE");

        SetEnvironmentVariableW(HOME_VAR_NAME, userProfileVar);

        wprintf(L"Setting home to: %ls", userProfileVar);

        free(userProfileVar);

        return;
    }
    else
    {
        wchar_t process[1024];
        wchar_t directory[1024];
        wchar_t upTwoDirs[1024];
        wchar_t* p1 = directory;
        wchar_t* p2 = process;
        wchar_t* directoryEnd = process;

        GetModuleFileNameW(NULL, process, 1024);

        while(*p2)
        {
            if(*p2 == '/' || *p2 == '\\')
            {
                directoryEnd = p2;
            }

            ++p2;
        }

        p2 = process;

        while(p2 <= directoryEnd)
        {
            *p1++ = *p2++;
        }

        *p1 = '\0';

        //Go up twice from directory
        wcscpy(upTwoDirs, directory);

        p2 = upTwoDirs;
        directoryEnd = upTwoDirs;

        while(*p2)
        {
            if((*p2 == '/' || *p2 == '\\')
               && *(p2+1))
            {
                directoryEnd = p2;
            }
            ++p2;
        }

        *directoryEnd = '\0';

        p2 = upTwoDirs;
        directoryEnd = upTwoDirs;

        while(*p2)
        {
            if((*p2 == '/' || *p2 == '\\')
               && *(p2+1))
            {
                directoryEnd = p2;
            }
            ++p2;
        }

        *(directoryEnd+1) = '\0';

        SetEnvironmentVariableW(HOME_VAR_NAME, upTwoDirs);

        wprintf(L"Setting home to: %ls", upTwoDirs);
    }
    //Already set
}

int main(int argcIgnore, char** argvIgnore)
{
    wchar_t process[1024];
    wchar_t directory[1024];
    wchar_t* p1 = directory;
    wchar_t* p2 = process;
    wchar_t* directoryEnd = process;
    HWND emacs_window = FindWindowW(EMACS_CLASS_NAME, NULL);

    wchar_t* commandLine = GetCommandLineW();

    int argc;
    wchar_t** argv;

    EnsureHome();

    argv = CommandLineToArgvW(commandLine, &argc);

    GetModuleFileNameW(NULL, process, 1024);

    while(*p2)
    {
        if(*p2 == '/' || *p2 == '\\')
        {
            directoryEnd = p2;
        }

        ++p2;
    }

    p2 = process;

    while(p2 <= directoryEnd)
    {
        *p1++ = *p2++;
    }

    *p1 = '\0';

    if(argc == 1)
    {//No args, just want to run emacs
        if(!emacs_window)
        {
            wchar_t exec[2024];
            wchar_t command[2024];
            STARTUPINFOW siStartupInfo;
            PROCESS_INFORMATION piProcessInfo;

            memset(&siStartupInfo, 0, sizeof(siStartupInfo));
            siStartupInfo.cb = sizeof(siStartupInfo);

            memset(&piProcessInfo, 0, sizeof(piProcessInfo));

            wsprintfW(exec, L"%lsemacsclientw.exe", directory);
            wsprintfW(command, L"\"%ls\" -n -a \"\" -c", exec);

            CreateProcessW(exec, command, 0, 0, 0, 0, 0, 0, &siStartupInfo, &piProcessInfo);
        }
    }
    else
    {
        int i;
        wchar_t exec[2024];
        wchar_t command[2024];

        STARTUPINFOW siStartupInfo;
        PROCESS_INFORMATION piProcessInfo;

        memset(&siStartupInfo, 0, sizeof(siStartupInfo));
        siStartupInfo.cb = sizeof(siStartupInfo);

        memset(&piProcessInfo, 0, sizeof(piProcessInfo));

        wsprintfW(exec, L"%lsemacsclientw.exe", directory);
        wsprintfW(command, L"\"%ls\" -n -a \"\"", exec, directory);

        for(i = 1; i < argc; ++i)
        {
            wcscat(command, L" \"");
            wcscat(command, argv[i]);
            wcscat(command, L"\"");
        }

        wprintf(L"Running command line: %ls", command);

        CreateProcessW(exec, command, 0, 0, 0, 0, 0, 0, &siStartupInfo, &piProcessInfo);

        if(!emacs_window)
        {//Try again
            //Wait a little bit to give the window time to pop up
            Sleep(2000);

            emacs_window = FindWindowW(EMACS_CLASS_NAME, NULL);
        }
    }

    if(emacs_window)
    {
        if(IsIconic(emacs_window))
        {
            ShowWindow(emacs_window, SW_RESTORE);
        }
        SetForegroundWindow(emacs_window);
    }

    return 0;
}
