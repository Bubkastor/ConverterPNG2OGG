#include "YCbCr_ConverterToOgg.h"



static int ilog(unsigned _v) {
	int ret;
	for (ret = 0; _v; ret++)_v >>= 1;
	return ret;
}

YCbCr_ConverterToOgg::YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer) :
	m_arrayYcbcrBuffer(YcbcrBuffer)
{
	static th_enc_ctx*td;
	static th_info ti;


	ti.frame_width = ((800 + 15) >> 4) << 4;  // 800 = w
	ti.frame_height = ((600 + 15) >> 4) << 4; // 600 = h
	ti.pic_width = 800;  // W
	ti.pic_height = 600; // H 
	ti.pic_x = 0;
	ti.pic_y = 0;
	ti.fps_numerator = 24;  // video_fps_numerator
	ti.fps_denominator = 1; // video_fps_denominator
	ti.aspect_numerator = 0; // video_aspect_numerator
	ti.aspect_denominator = 0; //video_aspect_denominator
	ti.colorspace = TH_CS_UNSPECIFIED;
	ti.pixel_fmt = TH_PF_444;
	ti.target_bitrate = 1; // video_rate
	ti.quality = -1; // video_quality
	ti.keyframe_granule_shift = ilog(0 - 1); //keyframe_frequency


	td = th_encode_alloc(&ti);
	th_info_clear(&ti);
	if (td == NULL)
	{
		std::cout << "E- negative return code initializing encoder!";
		th_info_clear(&ti);

	}

	th_comment tc;
	th_comment_init(&tc);

	static ogg_stream_state ogg_os;
	ogg_stream_init(&ogg_os, rand());

	if (th_encode_flushheader(td, &tc, &m_op) <= 0)
		std::cout << "-E- Internal Theora Library Error!";

	if (th_encode_flushheader(td, &tc, &m_op) <= 0)
		std::cout << "-E- Internal Theora Library Error!";

	/* Theora is a one-frame-in,one-frame-out system; submit a frame
	for compression and pull out the packet */
	//while may be this add ycbcr
	for (auto it : m_arrayYcbcrBuffer)
	{
		th_ycbcr_buffer ycbcr;
		ycbcr[0] = it[0];
		ycbcr[1] = it[1];
		ycbcr[2] = it[2];

		if (th_encode_ycbcr_in(td, ycbcr))
			std::cout << "-E- Error: could not encode frame\n";

	}
	if (!th_encode_packetout(td, 1, &m_op)) {
		std::cout << "-E- [theora_write_frame] Error: could not read packets\n";


	}
}


	YCbCr_ConverterToOgg::~YCbCr_ConverterToOgg()
	{
	}
