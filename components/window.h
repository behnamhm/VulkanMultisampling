#pragma once


#include <GL/glew.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include <iostream>


class Window
{
public:

	bool init(GLFWwindow*& thisWindow, int width, int height);

};
