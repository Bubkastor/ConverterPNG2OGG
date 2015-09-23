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
	std::string filename = m_pathAllFile.at(0).string();
	
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
	th_ycbcr_buffer ycbcr;
	ycbcr[0].data = 0;
	if (!ycbcr[0].data)
	{
		ycbcr[0].width = yuv_w;
		ycbcr[0].height = yuv_h;
		ycbcr[0].stride = yuv_w;
		ycbcr[1].width = yuv_w;
		ycbcr[1].stride = ycbcr[1].width;
		ycbcr[1].height = yuv_h;
		ycbcr[2].width = ycbcr[1].width;
		ycbcr[2].stride = ycbcr[1].stride;
		ycbcr[2].height = ycbcr[1].height;

		ycbcr[0].data = (unsigned char*)malloc(ycbcr[0].stride * ycbcr[0].height);
		ycbcr[1].data = (unsigned char*)malloc(ycbcr[1].stride * ycbcr[1].height);
		ycbcr[2].data = (unsigned char*)malloc(ycbcr[2].stride * ycbcr[2].height);
	}

	RGBToYUV(arrayRGBA, ycbcr, width, height);

	//m_arrayYcbcrBuffer.push_back(ycbcr);

	std::cout << "complete RGBtoYUV" << std::endl;
}

std::vector<th_ycbcr_buffer> PNG_Converter::GetYcbcr()
{
	return std::vector<th_ycbcr_buffer>();
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
void PNG_Converter::RGBToYUV(std::vector<std::vector<RGB>> arrayRGB, th_ycbcr_buffer ycbcr, unsigned int w, unsigned int h)
{
	unsigned int x;
	unsigned int y;

	unsigned int x1;
	unsigned int y1;

	unsigned long yuv_w;

	unsigned char *yuv_y;
	unsigned char *yuv_u;
	unsigned char *yuv_v;

	yuv_w = ycbcr[0].width;

	yuv_y = ycbcr[0].data;
	yuv_u = ycbcr[1].data;
	yuv_v = ycbcr[2].data;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			unsigned char r = arrayRGB[y][x].R;
			unsigned char g = arrayRGB[y][x].G;
			unsigned char b = arrayRGB[y][x].B;

			yuv_y[x + y * yuv_w] = clamp((65481 * r + 128553 * g + 24966 * b + 4207500) / 255000);
			yuv_u[x + y * yuv_w] = clamp((-33488 * r - 65744 * g + 99232 * b + 29032005) / 225930);
			yuv_v[x + y * yuv_w] = clamp((157024 * r - 131488 * g - 25536 * b + 45940035) / 357510);
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
