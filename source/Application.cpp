#include "Application.h"

bool Application::Init(int width,int height)
{
    if (!window.init(thisWindow, width, height))
    {
        return false;
    }

    if (!renderer.init(thisWindow))
    {
        return false;
    }

    return true;
}

void Application::Run()
{

    while (!glfwWindowShouldClose(thisWindow))
    {
        glfwPollEvents();

        renderer.update();

        renderer.draw();
    }

    glfwDestroyWindow(thisWindow);
    glfwTerminate();

 }

