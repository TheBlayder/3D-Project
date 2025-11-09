#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include "Transform.h"
#include "SimpleVertex.h"

/// <summary>
/// Class used for testing purposes.
/// </summary>
class TestObject
{
private:
	Transform m_transform;
	std::vector<SimpleVertex> m_vertices;


public:
	TestObject();
	~TestObject() = default;

	
};