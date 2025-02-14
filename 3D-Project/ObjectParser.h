#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

class ObjectParser
{
private:


public:
	ObjectParser();
	~ObjectParser();

	bool LoadObjectFromFile(const std::string& filePath);
};

