#pragma once
#include "OBJ_Loader.h"
#include <array>


struct SimpleVertex
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 2> uv;

	SimpleVertex(objl::Vertex& vertex) : position{ vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
		normal{ vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z },
		uv{ vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y }
	{}
};