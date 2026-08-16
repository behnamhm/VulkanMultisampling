#pragma once

#include <string>
#include <iostream>
#include <stdexcept>

#include "components/window.h"
#include "components/meshModel.h"
#include "Renderer.h"



class Application
{
public:
    void Run();
    bool Init(int width, int height);
private:
    Renderer renderer;
    GLFWwindow* thisWindow = nullptr;
    Window window;


};