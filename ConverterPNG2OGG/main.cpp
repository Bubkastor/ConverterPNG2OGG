#include <iostream>
#include "ConvertPngToOgg.h"

void main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cout << "Low parametr" << std::endl;
	}
	{
		using namespace std;
		string pathPng = argv[1];
		string pathOggFile = argv[2];
		ConvertPngToOgg convert = ConvertPngToOgg(pathPng, pathOggFile);
		convert.Run();
	}
}
