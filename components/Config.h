#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Vertex data representation
struct Vertex
{
	glm::vec3 pos; // Vertex Position (x, y, z)
	glm::vec3 col; // Vertex Colour (r, g, b)
	glm::vec2 tex; // Texture Coords (u, v)
};

const int MAX_OBJECTS = 20;
const int MAX_FRAME_DRAWS = 2;



