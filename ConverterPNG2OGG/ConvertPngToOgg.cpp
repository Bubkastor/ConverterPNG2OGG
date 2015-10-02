#include "ConvertPngToOgg.h"

boost::filesystem::path PNG_EXTENSION = ".png";

struct RGB
{
	int R;
	int G;
	int B;

	RGB& operator+(const RGB right)
	{
		RGB result;
		result.R = this->R + right.R;
		result.G = this->G + right.G;
		result.B = this->B + right.B;
		return result;
	};

	RGB& operator/(const int right)
	{
		RGB result;
		result.R = this->R / right;
		result.G = this->G / right;
		result.B = this->B / right;
		return result;
	};

};

ConvertPngToOgg::ConvertPngToOgg(std::string folderPng, std::string outputFile, bool onlyAlpha) :
	outputFile(outputFile), converter(0, 0), onlyAlpha(onlyAlpha), resize(1)
{
	pngArray = GetAllPathInFolder(folderPng);
	resize = 2;
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
		//unsigned char* newSize = ResizePng(rgb);

		converter.NewFrame(rgb);
		percent++;
		LogPercentConverter(percent, size);
		image.clear();
		png.clear();
		//delete[] newSize;
		delete[] rgb;
		//newSize = NULL;
		rgb = NULL;
	}

	converter.End();
}



std::vector<std::vector<RGB>> SSAA2(std::vector<std::vector<RGB>> RgbArray)
{
	int resize = 2;
	std::vector<std::vector<RGB>> resizeArrayRgb;
	int width = RgbArray[0].size();
	int height = RgbArray.size();

	for (size_t i = 0; i < RgbArray.size(); i += resize)
	{
		std::vector<RGB> tempWidth;
		for (size_t j = 0; j < RgbArray[0].size(); j += resize)
		{
			RGB tRgb;
			if ((j + 1 < width) && (i + 1 < height))
			{
				tRgb = (RgbArray[i][j] + RgbArray[i + 1][j] + RgbArray[i + 1][j + 1] + RgbArray[i][j + 1]) / (resize * resize);			
			}
			else
			{
				if ((j + 1 > width) && (i + 1 > height))
				{
					tRgb = RgbArray[i][j];
				}
				else
				{
					if (i + 1 > height)
					{
						tRgb = (RgbArray[i][j] + RgbArray[i][j + 1]) / (resize * resize);
					}
					if (j + 1 > width)
					{
						tRgb = (RgbArray[i][j] + RgbArray[i + 1][j]) / (resize * resize);
					}
				}
			}
			tempWidth.push_back(tRgb);
		}

		resizeArrayRgb.push_back(tempWidth);
		tempWidth.clear();
	}
	return resizeArrayRgb;
}



unsigned char* ConvertPngToOgg::ResizePng(unsigned char* rgb)
{
	std::vector<std::vector<RGB>> RgbArray;
	int resize = 2;
	size_t sizeRgb = width* height * 3;
	
	std::vector<RGB> tempWidth;
	for (size_t i = 0; i < sizeRgb; i += 3)
	{
		RGB tRgb = {
			rgb[i],
			rgb[i + 1],
			rgb[i + 2]
		};
		tempWidth.push_back(tRgb);
		if (tempWidth.size() == width)
		{
			RgbArray.push_back(tempWidth);
			tempWidth.clear();
		}
	}

	std::vector<std::vector<RGB>> resizeArrayRgb = SSAA2(RgbArray);


	unsigned char* rgbResult = new unsigned char[resizeArrayRgb.size() * resizeArrayRgb[0].size() * 3];

	size_t index = 0;
	for (auto it : resizeArrayRgb)
	{
		for (auto element : it)
		{
			rgbResult[index] = element.R;
			rgbResult[index + 1] = element.G;
			rgbResult[index + 2] = element.B;
			index += 3;
		}
	}
	resizeArrayRgb.clear();
	return rgbResult;
	
}


void ConvertPngToOgg::InitYCbCr_conver()
{
	lodepng::load_file(png, pngArray[0].string());
	unsigned error = lodepng::decode(image, width, height, png);

	converter = ConverterOgg(width, height);
	converter.SetOutputFile(outputFile);
	converter.SetBlackWhiteImage(onlyAlpha);
	png.clear();
	image.clear();
}

void ConvertPngToOgg::LogPercentConverter(int i, int size)
{
	std::cout.precision(2);
	
	if (i != size)
	{
		system("cls");
		std::cout << "Precent converting: " << ((float)i / (float)size) * 100 << std::endl;
	}
	else
		std::cout << "Precent converting : 100" << std::endl;
	
}

unsigned char* ConvertPngToOgg::RGBA_To_RGB(std::vector<unsigned char> rgba)
{
	std::vector<unsigned char> tempRgb;
	
	for (int i = 0; i < rgba.size(); i += 4 ) {
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
