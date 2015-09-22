#include "PNG_Converter.h"
#include <lodepng.h>

unsigned int yuv_w;
unsigned int yuv_h;

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};
void decodeTwoSteps(const char* filename)
{
	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	lodepng::load_file(png, filename);
	unsigned error = lodepng::decode(image, width, height, png);

	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	std::vector<RGB>


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
		//PNG_Converter convert = PNG_Converter(inFolder);
		const char* filename = "C:/Users/albert.gubkin/Desktop/PNG/test.png";
		

		decodeTwoSteps(filename);
	}
}