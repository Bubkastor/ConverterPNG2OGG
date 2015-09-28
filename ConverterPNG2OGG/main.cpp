#include <iostream>
#include "ConvertPngToOgg.h"


bool StringToBool(std::string str)
{
	return str == "1"? true : false;
}

void Using()
{
	std::cout << "first argument folder png\n" <<
		"second parametr output file\n" <<
		"additional parametr only alpha channel 1 true define false 0\n";
}


void main(int argc, char *argv[])
{

	if (argc < 3) {
		std::cout << "Low parametr" << std::endl;
		Using();
	}
	{
		using namespace std;
		string pathPng = argv[1];
		string pathOggFile = argv[2];
		bool onlyAlpha = false;
		if (argc == 4)
			onlyAlpha = StringToBool(argv[3]);
		
		ConvertPngToOgg convert = ConvertPngToOgg(pathPng, pathOggFile, onlyAlpha);
		convert.Run();
	}
}
