#pragma once

#include <vector>
#include <string>
#include <fstream>

class FileSystem
{
public:
	static std::vector<char> FileSystem::readFile(const std::string& filename);
};