#include "PNG_ConverterToYCbCr.h"
#include "YCbCr_ConverterToOgg.h"

#include <lodepng.h>
boost::filesystem::path PNG_EXTENSION = ".png";

void ErrorFile(std::string path)
{
	std::cout << path << " not png" << std::endl;
}
bool CheckExtension(boost::filesystem::path path)
{
	return path.extension() == PNG_EXTENSION;
}

std::vector<boost::filesystem::path> GetAllPathInFolder(std::string pathRead)
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

unsigned char* RGBA_To_RGB(std::vector<unsigned char> rgba) {
	//unsigned char* rgb  = new unsigned char[rgba.size()/4];
	int i, j;
	std::vector<unsigned char> tempRgb;
	for (i = 0; i < rgba.size(); i += 4) {
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

void main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cout << "Low parametr" << std::endl;
	}
	{
		using namespace std;
		string inFolder = argv[1];
		string ouFolder = argv[2];

		
		
		auto path = GetAllPathInFolder(inFolder);

		std::vector<unsigned char> png;
		std::vector<unsigned char> image;

		lodepng::load_file(png, path[0].string());
		unsigned int width;
		unsigned int height;

		unsigned error = lodepng::decode(image, width, height, png);

		YCbCr_ConverterToOgg convOgg = YCbCr_ConverterToOgg(width, height);
		convOgg.SetOutputFile(ouFolder);
		image.clear();
		for (auto element : path)
		{
			lodepng::load_file(png, element.string());

			unsigned error = lodepng::decode(image, width, height, png);
			if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			
			
			unsigned char* rgb = RGBA_To_RGB(image);
			convOgg.NewFrame(rgb);
			image.clear();
			delete[] rgb;
			rgb = NULL;
		}

		convOgg.End();
	}
}
