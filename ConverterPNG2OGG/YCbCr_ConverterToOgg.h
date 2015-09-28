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
	YCbCr_ConverterToOgg(int width, int height);
	~YCbCr_ConverterToOgg();

	void SetKeyFrameInterval(int keyFrameInterval);
	void SetFrameRate(int frameRate);
	void SetQuality(int quality);
	void SetOutputFile(std::string fileName);
	void End();
	void NewFrame(const unsigned char *data);

private:
	void InitTheora();
	void WriteHeaders();
	void WriteFrame(const unsigned char *rgb, int dupCount = 0);
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

