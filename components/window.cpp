#include "window.h"


bool Window::init(GLFWwindow*& thisWindow, int width, int height)
{
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


    thisWindow = glfwCreateWindow(width, height, "test App", nullptr, nullptr);

    if (thisWindow == nullptr)
    {
        std::cout << "Error Creating window" << std::endl;
        glfwTerminate();
        return false;
    }


    return true;
}

