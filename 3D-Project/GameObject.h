#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <iostream>

#include "BaseObject.h"

namespace DX = DirectX;

class GameObject : public BaseObject
{
public:
    // Add a matching constructor to fix E0289
    GameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "", const bool flipUVy = true, const bool tesselate = false)
        : BaseObject(device, transform, folderPath, objectName, textureFolder, flipUVy, tesselate) {}

    void Update(float deltaTime) override;
};
