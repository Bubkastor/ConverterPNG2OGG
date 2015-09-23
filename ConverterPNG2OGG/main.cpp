#include "PNG_Converter.h"
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
		PNG_Converter convert = PNG_Converter(inFolder);

		convert.MakeYcbcr();
	}
}