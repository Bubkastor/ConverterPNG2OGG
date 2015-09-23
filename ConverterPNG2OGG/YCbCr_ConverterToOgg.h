#pragma once
#include <iostream>
#include <vector>
#include <theora/theoraenc.h>
#include <ogg/ogg.h>

class YCbCr_ConverterToOgg
{
public:
	YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer);
	~YCbCr_ConverterToOgg();
private:
	
	ogg_packet m_op;
	ogg_page m_og;
	std::vector<th_img_plane*> m_arrayYcbcrBuffer;
};

