#include "ObjectParser.h"

ObjectParser::ObjectParser()
{
}

ObjectParser::~ObjectParser()
{
}

bool ObjectParser::LoadObjectFromFile(const std::string& filePath)
{

	using namespace DirectX;
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;

	FILE* file;
	errno_t err = fopen_s(&file, filePath.c_str(), "r"); // Open the .obj file
	if (!file)
	{
		std::cerr << "Error opening file: " << filePath << std::endl;
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf_s(file, "%s", lineHeader, (unsigned)_countof(lineHeader));
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			
			//Normalize
			float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			if (length == 0)
			{
				std::cerr << "Error: Normal length is 0!" << std::endl;
				return false;
			}
			else if (length != 1)
			{
				normal.x /= length;
				normal.y /= length;
				normal.z /= length;
			}

			normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &normalIndex[0], &uvIndex[0],
				&vertexIndex[1], &normalIndex[1], &uvIndex[1],
				&vertexIndex[2], &normalIndex[2], &uvIndex[2]);
			if (matches != 9)
			{
				std::cerr << "File can't be read by parser!" << std::endl;
				return false;
			}
		}
	}

	fclose(file);
	return true;

	// COMMENT TO SELF
	// Still need to output and handle the data
}
