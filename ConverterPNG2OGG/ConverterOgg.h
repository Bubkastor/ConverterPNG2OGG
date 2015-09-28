#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <theora/theoraenc.h>
#include <ogg/ogg.h>
#include <cmath>

class ConverterOgg
{
public:
	ConverterOgg(int width, int height);
	~ConverterOgg();

	void SetKeyFrameInterval(int keyFrameInterval);
	void SetFrameRate(int frameRate);
	void SetQuality(int quality);
	void SetOutputFile(std::string fileName);
	void SetBlackWhiteImage(bool onlyAlbph);
	void End();
	void NewFrame(const unsigned char *data);

private:
	void InitTheora();
	void WriteHeaders();
	void WriteFrame(const unsigned char *rgb, int dupCount = 0);
	unsigned char *rgb_to_yuv(const unsigned char *rgb, size_t size);

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

	bool onlyAlphaChannel;
	

	std::vector<th_img_plane*> arrayYcbcrBuffer;
};

