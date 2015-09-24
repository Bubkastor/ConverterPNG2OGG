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
	void SetOutputFile(std::string path);
	void Convert();
	bool EncodeFrame(th_ycbcr_buffer buffer, std::ofstream& ofs);
	void SetFrameParameter(int width, int height);
private:
	bool InitEncode(std::ofstream& ofs);

	std::string m_outputFile;
	th_enc_ctx *m_encoder;
	th_info m_ti;
	int m_frame_w;
	int m_frame_h;
	int m_pic_w;
	int m_pic_h;
	int m_pic_x;
	int m_pic_y;
	ogg_uint32_t video_fps_numerator = 1;
	ogg_uint32_t video_fps_denominator = 1;
	ogg_uint32_t video_aspect_numerator = 0;
	ogg_uint32_t video_aspect_denominator = 0;
	int video_rate = 1;
	int video_quality = 0;
	int keyframe_frequency = 0;

	th_comment m_tc;

	ogg_packet m_op;
	ogg_page m_og;
	ogg_stream_state m_ogg_os;

	std::vector<th_img_plane*> m_arrayYcbcrBuffer;
};

