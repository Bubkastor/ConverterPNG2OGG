#pragma once
#include <string>
#include <vector>
#include <boost\filesystem.hpp>
#include "YCbCr_ConverterToOgg.h"

#include <iostream>
#include <lodepng.h>

class ConvertPngToOgg
{
public:
	ConvertPngToOgg(std::string folderPng, std::string outputFile, bool onlyAlpha);
	~ConvertPngToOgg();
	void Run();
private:
	void InitYCbCr_conver();
	std::vector<boost::filesystem::path> GetAllPathInFolder(std::string pathRead);
	void ErrorFile(std::string path);
	bool CheckExtension(boost::filesystem::path path);
	unsigned char* RGBA_To_RGB(std::vector<unsigned char> rgba);
	void LogPercentConverter(int i, int size);
	
	bool onlyAlpha;
	YCbCr_ConverterToOgg converter;
	std::string outputFile;
	std::vector<boost::filesystem::path> pngArray;
	std::vector<unsigned char> png;
	std::vector<unsigned char> image;
	unsigned int width;
	unsigned int height;
	
};

