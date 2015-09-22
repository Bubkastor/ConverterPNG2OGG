#include "PNG_Converter.h"


void main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cout << "Low parametr" << std::endl;
	}
	{
		using namespace std;
		string inFolder = argv[1];
		string ouFolder = argv[2];
		cout << "input: " << inFolder << std::endl;
		cout << "output: " << ouFolder << std::endl;
		PNG_Converter convert = PNG_Converter(inFolder);
	}
}