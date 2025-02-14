#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

namespace DX = DirectX;

class ObjectParser
{
private:


public:
	ObjectParser();
	~ObjectParser();

	bool LoadObjectFromFile(const std::string* filePath, std::vector<DX::XMFLOAT3>& verticies, std::vector<DX::XMFLOAT3>& normals, std::vector<DX::XMFLOAT2>& UVs);
};

