#include "PNG_ConverterToYCbCr.h"
#include "YCbCr_ConverterToOgg.h"

#include <lodepng.h>

void main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cout << "Low parametr" << std::endl;
	}
	{
		using namespace std;
		string inFolder = argv[1];
		string ouFolder = argv[2];
		PNG_ConverterToYCbCr convert = PNG_ConverterToYCbCr(inFolder);
		convert.MakeYcbcr();
		std::vector<th_img_plane*> yCbCr = convert.GetYcbcr();

		YCbCr_ConverterToOgg convOgg = YCbCr_ConverterToOgg(yCbCr, ouFolder);
	}
}