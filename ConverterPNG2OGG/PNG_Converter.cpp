#include "PNG_Converter.h"

boost::filesystem::path PNG_EXTENSION = ".png";

PNG_Converter::PNG_Converter(std::string path) :
	m_pathFolder(path)
{
	m_pathAllFile = GetAllPathInFolder(path);
}

PNG_Converter::~PNG_Converter()
{
}


void PNG_Converter::MakeYcbcr()
{

}

std::vector<th_ycbcr_buffer> PNG_Converter::GetYcbcr()
{
	return std::vector<th_ycbcr_buffer>();
}

std::vector<boost::filesystem::path> PNG_Converter::GetAllPathInFolder(std::string pathRead)
{
	using namespace boost::filesystem;
	std::vector<boost::filesystem::path> result;

	if (is_directory(m_pathFolder))
	{
		for (auto x : directory_iterator(m_pathFolder))
			CheckExtension(x.path()) ?
				result.push_back(x.path()) :
				ErrorFile(x.path().string());
	}
	return result;
}

void PNG_Converter::ErrorFile(std::string path)
{
	std::cout << path << " not png" << std::endl;
}

bool PNG_Converter::CheckExtension(boost::filesystem::path path)
{
	return path.extension() == PNG_EXTENSION;
}
