#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <theora/theoraenc.h>
#include <ogg/ogg.h>

class YCbCr_ConverterToOgg
{
public:
	YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer, std::string outFile);
	~YCbCr_ConverterToOgg();

	void Convert();
	void setKeyFrameInterval(int keyFrameInterval);
	void setFrameRate(int frameRate);
	void setQuality(int quality);
	void setOutputFile(std::string fileName);
	void end();
private:
	void InitTheora();
	void WriteHeaders();
	void EncodeFrame(th_ycbcr_buffer ycbcr);
	void WriteFrame(th_ycbcr_buffer ycbcr, int dupCount = 0);
	int width, height, quality, frameRate, keyFrameInterval;
	unsigned long frameCount;

	FILE *ogg_fp;
	th_info ti;
	th_enc_ctx *td;
	th_comment tc;
	ogg_packet op;
	ogg_page og;
	ogg_stream_state *ogg_os;
	std::string outputFileName;
	

	std::vector<th_img_plane*> arrayYcbcrBuffer;
};

