#include "ConvertPngToOgg.h"

boost::filesystem::path PNG_EXTENSION = ".png";

ConvertPngToOgg::ConvertPngToOgg(std::string folderPng, std::string outputFile) :
	outputFile(outputFile), converter(0, 0)
{
	pngArray = GetAllPathInFolder(folderPng);
}


std::vector<boost::filesystem::path> ConvertPngToOgg::GetAllPathInFolder(std::string pathRead)
{
	using namespace boost::filesystem;
	std::vector<boost::filesystem::path> result;

	if (is_directory(pathRead))
	{
		for (auto x : directory_iterator(pathRead))
			CheckExtension(x.path()) ?
			result.push_back(x.path()) :
			ErrorFile(x.path().string());
	}
	return result;
}

void ConvertPngToOgg::ErrorFile(std::string path)
{
	throw path + " not open";
}

bool ConvertPngToOgg::CheckExtension(boost::filesystem::path path)
{
	return path.extension() == PNG_EXTENSION;
}

ConvertPngToOgg::~ConvertPngToOgg()
{
	pngArray.clear();
	image.clear();
	png.clear();
}

void ConvertPngToOgg::Run()
{
	InitYCbCr_conver();
	int percent = 0;;
	size_t size = pngArray.size();
	for (auto element : pngArray)
	{
		lodepng::load_file(png, element.string());
		unsigned error = lodepng::decode(image, width, height, png);
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		unsigned char* rgb = RGBA_To_RGB(image);
		converter.NewFrame(rgb);
		percent++;
		LogPercentConverter(percent, size);
		image.clear();
		png.clear();
		delete[] rgb;
		rgb = NULL;
	}

	converter.End();
}

void ConvertPngToOgg::LogPercentConverter(int i, int size)
{
	std::cout.precision(2);
	
	if (i != size)
	{
		std::cout << "Precent converting: " << ((float)i / (float)size) * 100 << std::endl;
	}
	else
		std::cout << "Precent converting : 100" << std::endl;
	
}

void ConvertPngToOgg::InitYCbCr_conver()
{
	lodepng::load_file(png, pngArray[0].string());
	unsigned error = lodepng::decode(image, width, height, png);
	converter = YCbCr_ConverterToOgg(width, height);
	converter.SetOutputFile(outputFile);
	png.clear();
	image.clear();
}

unsigned char* ConvertPngToOgg::RGBA_To_RGB(std::vector<unsigned char> rgba)
{
	std::vector<unsigned char> tempRgb;
	for (int i = 0; i < rgba.size(); i += 4) {
		tempRgb.push_back(rgba[i]);
		tempRgb.push_back(rgba[i + 1]);
		tempRgb.push_back(rgba[i + 2]);
	}

	unsigned char* rgb = new unsigned char[tempRgb.size()];
	for (size_t i = 0; i < tempRgb.size(); i++)
		rgb[i] = tempRgb[i];
	tempRgb.clear();
	tempRgb.~vector();
	return rgb;
}
