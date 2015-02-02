#include <stdio.h>

#include "Bootstrap.h"

#pragma comment(lib, "shlwapi.lib")

int main(int argc, char * argv[])
{
    Bootstrap bootstrap;

    try {
        bootstrap.Init();
    }
    catch (std::domain_error& ex) {
        //Log
        exit(-1);
    }

    bootstrap.Run();

    for(;;) {
        ::Sleep(1000);
    }

    bootstrap.Exit();

    return 0;
}

