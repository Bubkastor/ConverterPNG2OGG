#include "YCbCr_ConverterToOgg.h"


static int ilog(unsigned _v) {
	int ret;
	for (ret = 0; _v; ret++)_v >>= 1;
	return ret;
}

YCbCr_ConverterToOgg::YCbCr_ConverterToOgg(std::vector<th_img_plane*> YcbcrBuffer, std::string outFile):
	m_arrayYcbcrBuffer(YcbcrBuffer), outputFile(outFile)
{
	static th_enc_ctx*td;
	static th_info ti;

	int width = 1152;
	int height = 648;

	ti.frame_width = ((width + 15) >> 4) << 4;  // 800 = w
	ti.frame_height = ((height + 15) >> 4) << 4; // 600 = h
	ti.pic_width = width;  // W
	ti.pic_height = height; // H 
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

	if (!th_encode_packetout(td, 5, &m_op)) 
		std::cout << "-E- [theora_write_frame] Error: could not read packets\n";	

	int ret = 0;

	FILE * fout;
	fopen_s(&fout, outputFile.c_str(), "wb");
	for (;;)
	{
		ret = th_encode_flushheader(td, &tc, &m_op);
		if (ret<0)
		{
			std::cout << "-E- Internal Theora library error.";
			if (td != NULL)
			{
				th_encode_free(td);
				td = NULL;
			}
			th_comment_clear(&tc);
			th_info_clear(&ti);
			
		}
		else if (!ret)
		{
			fwrite(m_og.header, 1, m_og.header_len, fout);
			fwrite(m_og.body, 1, m_og.body_len, fout);
			break;
		}
		ogg_stream_packetin(&ogg_os, &m_op);
	}
	int result;
	for (;;) {
		int result = ogg_stream_flush(&ogg_os, &m_og);
		if (result<0) {
			printf("-E- Internal Ogg library error.");
		}

		if (result == 0)break;
		fwrite(m_og.header, 1, m_og.header_len, fout);
		fwrite(m_og.body, 1, m_og.body_len, fout);
	}

	unsigned char *buffer;
	int bytes = th_encode_ctl(td, TH_ENCCTL_2PASS_OUT, &buffer, sizeof(buffer));
	if (bytes < 0) {
		fprintf(stderr, "Could not read two-pass summary data from encoder.\n");
		exit(1);
	}
	if (fseek(fout, 0, SEEK_SET) < 0) {
		fprintf(stderr, "Unable to seek in two-pass data file.\n");
		exit(1);
	}
	if (fwrite(buffer, 1, bytes, fout) < bytes) {
		fprintf(stderr, "Unable to write to two-pass data file.\n");
		exit(1);
	}
	fflush(fout);

}


	YCbCr_ConverterToOgg::~YCbCr_ConverterToOgg()
	{
	}

	void YCbCr_ConverterToOgg::SetOutputFile(std::string path)
	{
		outputFile = path;
	}
