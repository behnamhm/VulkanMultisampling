#include <iostream>
#include "Application.h"


int main()
{
    Application app;

    if (!app.Init(1920, 1080))
    {
        return EXIT_FAILURE;
    }

    app.Run();

    return EXIT_SUCCESS;
}


