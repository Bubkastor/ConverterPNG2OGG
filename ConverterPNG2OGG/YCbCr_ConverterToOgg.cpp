#include "YCbCr_ConverterToOgg.h"
#include <cmath>

YCbCr_ConverterToOgg::YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer, std::string outputFileName) :
	outputFileName(outputFileName), arrayYcbcrBuffer(YcbcrBuffer),
	quality(1),
	frameRate(1), 
	keyFrameInterval(32),
	ogg_fp(NULL),
	td(NULL), 
	ogg_os(NULL), 
	frameCount(0)
{
	auto el = YcbcrBuffer[0];
	width = el->width;
	height = el->height;
	//el = nullptr;
}


YCbCr_ConverterToOgg::~YCbCr_ConverterToOgg()
{

}



void YCbCr_ConverterToOgg::Convert()
{

	for (auto it : arrayYcbcrBuffer)
	{
		th_ycbcr_buffer ycbcr = {it[0], it[1], it[2]};
		EncodeFrame(ycbcr);
	}

}

void YCbCr_ConverterToOgg::setKeyFrameInterval(int keyFrameInterval)
{
	this->keyFrameInterval = keyFrameInterval;
}

void YCbCr_ConverterToOgg::setFrameRate(int frameRate)
{
	this->frameCount = frameRate;
}

void YCbCr_ConverterToOgg::setQuality(int quality)
{
	this->quality = quality;
}

void YCbCr_ConverterToOgg::setOutputFile(std::string fileName)
{
	this->outputFileName = fileName;
}

void YCbCr_ConverterToOgg::EncodeFrame(th_ycbcr_buffer ycbcr)
{
	if (!frameCount)
	{
		if(outputFileName.empty())
			throw "No output means was set. Use setOutputFile to set it.";
		fopen_s(&ogg_fp, outputFileName.c_str(), "w+");
		if (!ogg_fp)
		{
			std::string errorMsg;
			std::cout << "Could not open " << outputFileName << " Error:" << errorMsg;
			throw errorMsg;
		}

		InitTheora();
		WriteHeaders();
	}
	WriteFrame(ycbcr);
	frameCount++;
}

void YCbCr_ConverterToOgg::WriteFrame(th_ycbcr_buffer ycbcr, int dupCount)
{
	ogg_packet op;
	ogg_page og;

	if (dupCount > 0) {
		int ret = th_encode_ctl(td, TH_ENCCTL_SET_DUP_COUNT, &dupCount, sizeof(int));
		if (ret)
			throw "th_encode_ctl failed for dupCount>0";
	}

	if (th_encode_ycbcr_in(td, ycbcr))
		throw "th_encode_ycbcr_in failed in WriteFrame";

	while (int ret = th_encode_packetout(td, 0, &op))
	{
		if (ret < 0)
			throw "th_encode_packetout failed in WriteFrame";
		ogg_stream_packetin(ogg_os, &op);
		while (ogg_stream_pageout(ogg_os, &og))
		{
			fwrite(og.header, og.header_len, 1, ogg_fp);
			fwrite(og.body, og.body_len, 1, ogg_fp);
		}
	}

	//ogg_stream_flush(ogg_os, &og);
	//fwrite(og.header, og.header_len, 1, ogg_fp);
	//fwrite(og.body, og.body_len, 1, ogg_fp);
}

void YCbCr_ConverterToOgg::end()
{
	if (ogg_fp) fclose(ogg_fp);
	if (td) th_encode_free(td);
	if (ogg_os) ogg_stream_clear(ogg_os);
	ogg_fp = NULL;
	td = NULL;
	ogg_os = NULL;
}

void YCbCr_ConverterToOgg::WriteHeaders()
{
	th_comment_init(&tc);
	if (th_encode_flushheader(td, &tc, &op) <= 0)
		throw "th_encode_flushheader failed in WriteHeaders";
	th_comment_clear(&tc);

	ogg_stream_packetin(ogg_os, &op);
	if (ogg_stream_pageout(ogg_os, &og) != 1)
		throw "ogg_stream_pageout failed in WriteHeaders";

	fwrite(og.header, 1, og.header_len, ogg_fp);
	fwrite(og.body, 1, og.body_len, ogg_fp);

	for (;;)
	{
		int ret = th_encode_flushheader(td, &tc, &op);
		if (ret < 0 )
			throw "th_encode_flushheader failed in WriteHeaders";
		else if (ret == 0)
			break;
		ogg_stream_packetin(ogg_os, &op);
	}

	for (;;) {
		int ret = ogg_stream_flush(ogg_os, &og);
		if (ret < 0)
			throw "ogg_stream_flush failed in WriteHeaders";
		else if (ret == 0)
			break;
		fwrite(og.header, 1, og.header_len, ogg_fp);
		fwrite(og.body, 1, og.body_len, ogg_fp);
	}
}
void YCbCr_ConverterToOgg::InitTheora()
{
	th_info_init(&ti);

	ti.frame_width = ((width + 15) >> 4) << 4;
	ti.frame_height = ((height + 15) >> 4) << 4;
	ti.pic_width = width;
	ti.pic_height = height;
	ti.pic_x = 0;
	ti.pic_y = 0;

	ti.fps_numerator = frameRate;
	ti.fps_denominator = 0;
	ti.aspect_numerator = 0;
	ti.aspect_denominator = 0;
	ti.colorspace = TH_CS_UNSPECIFIED;
	ti.target_bitrate = 0;
	ti.quality = 10;// quality;
	ti.keyframe_granule_shift = 6;// (int)log2(keyFrameInterval);
	ti.pixel_fmt = TH_PF_420;

	td = th_encode_alloc(&ti);

	th_info_clear(&ti);

	int comp = 1;
	th_encode_ctl(td, TH_ENCCTL_SET_VP3_COMPATIBLE, &comp, sizeof(comp));

	ogg_os = (ogg_stream_state*)malloc(sizeof(ogg_stream_state));
	if (!ogg_os)
		throw "malloc failed in InitTheora for ogg_stream_state";
	if (ogg_stream_init(ogg_os,rand()))
		throw "ogg_stream_init failed in InitTheora";

}
