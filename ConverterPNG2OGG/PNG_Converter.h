#pragma once
#include <string>
#include <vector>
#include <theora\theoraenc.h>
#include <boost\filesystem.hpp>
#include <iostream>

class PNG_Converter
{
public:
	PNG_Converter(std::string path);
	~PNG_Converter();

	void MakeYcbcr();
	std::vector<th_ycbcr_buffer> GetYcbcr();

private:
	std::vector<boost::filesystem::path> GetAllPathInFolder(std::string pathRead);
	void ErrorFile(std::string path);
	bool CheckExtension(boost::filesystem::path path);

	boost::filesystem::path m_pathFolder;
	std::vector<boost::filesystem::path> m_pathAllFile;
	std::vector<th_ycbcr_buffer>  m_arrayYcbcrBuffer;
};

