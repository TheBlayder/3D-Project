#pragma once

#include <fstream>
#include <iostream>
#include <string>

/*
* @breif Reads a CSO file and returns its content as a string.
* 
* @param filePath The path to the CSO file to read.
* @param output A reference to a string where the content of the CSO file will be stored.
* 
* @return Returns true if the file was read successfully, false otherwise.
*/
namespace CSOReader 
{
	inline static bool ReadCSO(const std::string& filePath, std::string& output) 
	{
		output.clear();
		std::ifstream reader;
		reader.open(filePath, std::ios::binary | std::ios::ate);
		if (!reader.is_open()) 
		{
			std::cerr << "Error opening file: " << filePath << std::endl;
			return false;
		}

		reader.seekg(0, std::ios::end);
		output.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);

		output.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
		reader.close();

		return true;
	}
}