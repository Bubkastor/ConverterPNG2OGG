#include "YCbCr_ConverterToOgg.h"



YCbCr_ConverterToOgg::YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer, std::string outFile) :
	m_arrayYcbcrBuffer(YcbcrBuffer), m_outputFile(outFile)
{
}




YCbCr_ConverterToOgg::~YCbCr_ConverterToOgg()
{


}

void YCbCr_ConverterToOgg::SetOutputFile(std::string path)
{
	m_outputFile = path;
}

void YCbCr_ConverterToOgg::Convert()
{
	auto element = m_arrayYcbcrBuffer[0];

	SetFrameParameter(element->width, element->height);

	//std::ofstream ofs(m_outputFile, std::ofstream::binary);
	FILE* outFil;

	fopen_s(&ogg_fp, m_outputFile.c_str(), "w+");

	for (size_t i = 0; i < m_arrayYcbcrBuffer.size(); i++)
	{
		th_img_plane* it = m_arrayYcbcrBuffer[i];

		th_ycbcr_buffer ycbcr;
		ycbcr[0] = it[0];
		ycbcr[1] = it[1];
		ycbcr[2] = it[2];
		
		
		if (!EncodeFrame(ycbcr))
			std::cout << "frame not encoder (((" << std::endl;
	}
	//ofs.close();
	fclose(ogg_fp);
}

bool YCbCr_ConverterToOgg::EncodeFrame(th_ycbcr_buffer ycbcr)
{
	int ret;
	if (m_encoder == NULL)
	{
		bool ret;
		ret = InitEncode();
		if (!ret)
		{
			std::cout << "initial encoder failed!" << std::endl;
			return false;
		}
	}

	ret = th_encode_ycbcr_in(m_encoder, ycbcr);

	for (;;)
	{
		ret = th_encode_packetout(m_encoder, 0, &m_op);
		if (!ret) break;
		else    if (ret < 0)
		{
			std::cout << "failed to retrieve compressed frame!" << std::endl;
			return false;
		}
		ogg_stream_packetin(&m_ogg_os, &m_op);
	}

	for (;;)
	{
		ret = ogg_stream_flush(&m_ogg_os, &m_og);
		if (ret)
		{
			fwrite(m_og.header, 1, m_og.header_len, ogg_fp);
			fwrite(m_og.body, 1, m_og.body_len, ogg_fp);

			return true;

		}
		else if (!ret) return true;
	}
}
void YCbCr_ConverterToOgg::SetFrameParameter(int width, int height)
{
	m_pic_w = width;
	m_pic_h = height;
	m_frame_w = m_pic_w + 15 & ~0xF;
	m_frame_h = m_pic_h + 15 & ~0xF;

	m_pic_x = m_frame_w - m_pic_w >> 1 & ~1;
	m_pic_y = m_frame_h - m_pic_h >> 1 & ~1;
}
bool YCbCr_ConverterToOgg::InitEncode(std::ofstream& ofs)
{
	int ret;
	th_info_init(&m_ti);

	m_ti.frame_width = m_frame_w;
	m_ti.frame_height = m_frame_h;
	m_ti.pic_width = m_pic_w;
	m_ti.pic_height = m_pic_h;
	m_ti.pic_x = m_pic_x;
	m_ti.pic_y = m_pic_y;
	m_ti.fps_numerator = video_fps_numerator;
	m_ti.fps_denominator = video_fps_denominator;
	m_ti.aspect_numerator = video_aspect_numerator;
	m_ti.aspect_denominator = video_aspect_denominator;
	m_ti.colorspace = TH_CS_UNSPECIFIED;
	m_ti.target_bitrate = video_rate;
	m_ti.quality = video_quality;
	m_ti.keyframe_granule_shift = 6;
	m_ti.pixel_fmt = TH_PF_444;

	m_encoder = th_encode_alloc(&m_ti);

	if (m_encoder == NULL)
	{
		std::cout << "negative return code initializing encoder!" << std::endl;
		th_info_clear(&m_ti);
		return false;
	}
	if (ogg_stream_init(&m_ogg_os, rand()) < 0)
	{
		std::cout << "ogg stream not created" << std::endl;
		return false;
	}

	th_comment_init(&m_tc);

	if (th_encode_flushheader(m_encoder, &m_tc, &m_op) <= 0)
	{
		std::cout << "Internal Theora Library Error!" << std::endl;
		return false;
	}
	ogg_stream_packetin(&m_ogg_os, &m_op);
	if (ogg_stream_pageout(&m_ogg_os, &m_og) != 1)
	{
		std::cout << "Internal Ogg library Error!" << std::endl;
		return false;
	}
	// todo https://github.com/pkrumins/node-video/blob/master/src/video_encoder.cpp

		
	for (;;)
	{
		ret = th_encode_flushheader(m_encoder, &m_tc, &m_op);
		if (ret < 0)
		{
			std::cout << "Internal Theora library error." << std::endl;
			if (m_encoder != NULL)
			{
				th_encode_free(m_encoder);
				m_encoder = NULL;
			}
			th_comment_clear(&m_tc);
			th_info_clear(&m_ti);
			return false;
		}
		else if (!ret)break;
		ogg_stream_packetin(&m_ogg_os, &m_op);
	}
	for (;;) {
		int result = ogg_stream_flush(&m_ogg_os, &m_og);
		if (result < 0) {
			std::cout << "Internal Ogg library error." << std::endl;
			return false;
		}
		if (result == 0)break;

		fwrite(m_og.header, m_og.header_len, 1, ogg_fp);
		fwrite(m_og.body, og.body_len, 1, ogg_fp);
	}
	return true;

}
