#pragma once
#include <string>
#include <vector>
#include <theora\theoraenc.h>
#include <boost\filesystem.hpp>
#include <iostream>
#include <lodepng.h>

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

class PNG_Converter
{
public:
	PNG_Converter(std::string path);
	~PNG_Converter();

	void MakeYcbcr();
	std::vector<th_img_plane*> GetYcbcr();

private:
	std::vector<std::vector<RGB>> RGBAtoRGBArray(std::vector<unsigned char> input, unsigned int width)const;

	void RGBToYUV(
		std::vector<std::vector<RGB>> arrayRGB,
		th_img_plane *ycbcr_ptr,
		unsigned int w, unsigned int h);

	std::vector<boost::filesystem::path> GetAllPathInFolder(std::string pathRead);
	void ErrorFile(std::string path);
	bool CheckExtension(boost::filesystem::path path);
	
	boost::filesystem::path m_pathFolder;
	std::vector<boost::filesystem::path> m_pathAllFile;
	std::vector<th_img_plane*> m_arrayYcbcrBuffer;
};

