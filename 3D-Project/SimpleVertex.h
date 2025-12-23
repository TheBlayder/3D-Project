#pragma once
#include "OBJ_Loader.h"
#include <array>


struct SimpleVertex
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 2> uv;

	SimpleVertex() = default;

	SimpleVertex(const std::array<float, 3>& pos, const std::array<float, 3>& norm, const std::array<float, 2>& tex)
		: position(pos), normal(norm), uv(tex) {}

	SimpleVertex(objl::Vertex& vertex, bool flipUVy = false)
		: position{ vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
		  normal{ vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z },
		  uv{ vertex.TextureCoordinate.X, flipUVy ? (1.0f - vertex.TextureCoordinate.Y) : vertex.TextureCoordinate.Y }
	{}
};