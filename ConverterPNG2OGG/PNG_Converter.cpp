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
	for (size_t i = 0; i < m_pathAllFile.size(); i++)
	{
		std::string filename = m_pathAllFile.at(i).string();

		std::vector<unsigned char> png;
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned int yuv_w;
		unsigned int yuv_h;

		lodepng::load_file(png, filename);
		unsigned error = lodepng::decode(image, width, height, png);

		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		auto arrayRGBA = RGBAtoRGBArray(image, width);
		yuv_w = (width + 15) & ~15;
		yuv_h = (height + 15) & ~15;

		
 		th_img_plane *ycbcr_ptr = new th_img_plane[3];
				{
			ycbcr_ptr[0].width = yuv_w;
			ycbcr_ptr[0].height = yuv_h;
			ycbcr_ptr[0].stride = yuv_w;
			ycbcr_ptr[1].width = yuv_w;
			ycbcr_ptr[1].stride = ycbcr_ptr[1].width;
			ycbcr_ptr[1].height = yuv_h;
			ycbcr_ptr[2].width = ycbcr_ptr[1].width;
			ycbcr_ptr[2].stride = ycbcr_ptr[1].stride;
			ycbcr_ptr[2].height = ycbcr_ptr[1].height;

			ycbcr_ptr[0].data = (unsigned char*)malloc(ycbcr_ptr[0].stride * ycbcr_ptr[0].height);
			ycbcr_ptr[1].data = (unsigned char*)malloc(ycbcr_ptr[1].stride * ycbcr_ptr[1].height);
			ycbcr_ptr[2].data = (unsigned char*)malloc(ycbcr_ptr[2].stride * ycbcr_ptr[2].height);
		}
		
		RGBToYUV(arrayRGBA, ycbcr_ptr, width, height);

		
		
		m_arrayYcbcrBuffer.push_back(ycbcr_ptr);				
	}
	
}

std::vector<th_img_plane*> PNG_Converter::GetYcbcr()
{
	return m_arrayYcbcrBuffer;
}

std::vector<std::vector<RGB>> PNG_Converter::RGBAtoRGBArray(std::vector<unsigned char> input, unsigned int width) const
{
	std::vector<std::vector<RGB>> result;
	std::vector<RGB> line;	

	for (size_t i = 0; i < input.size(); i += 4)
	{
		RGB rgb = {
			input.at(i),
			input.at(i + 1),
			input.at(i + 2),
		};
		line.push_back(rgb);
		if (line.size() == width)
		{
			result.push_back(line);
			line.clear();
		}
	}
	return result;
}

static unsigned char clamp(int d)
{
	if (d < 0)
		return 0;

	if (d > 255)
		return 255;

	return d;
}
void PNG_Converter::RGBToYUV(std::vector<std::vector<RGB>> arrayRGB, th_img_plane *ycbcr_ptr, unsigned int w, unsigned int h)
{
	unsigned int x;
	unsigned int y;
	unsigned long yuv_w;
	
	yuv_w = ycbcr_ptr[0].width;
	
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			unsigned char r = arrayRGB[y][x].R;
			unsigned char g = arrayRGB[y][x].G;
			unsigned char b = arrayRGB[y][x].B;

			ycbcr_ptr[0].data[x + y * yuv_w] = clamp((65481 * r + 128553 * g + 24966 * b + 4207500) / 255000);
			ycbcr_ptr[1].data[x + y * yuv_w] = clamp((-33488 * r - 65744 * g + 99232 * b + 29032005) / 225930);
			ycbcr_ptr[2].data[x + y * yuv_w] = clamp((157024 * r - 131488 * g - 25536 * b + 45940035) / 357510);
		}
	}
	
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
