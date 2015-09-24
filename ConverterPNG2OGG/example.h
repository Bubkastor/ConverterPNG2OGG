//#pragma once
////#pragma once 
//#ifndef _VCODERC_THEORA_H_ 
//#define _VCODERC_THEORA_H_ 
//
//
//#ifdef VCODEC_THEORA_EXPORTS
//#define VCODEC_THEORA_API __declspec(dllexport)
//#else
//#define VCODEC_THEORA_API __declspec(dllimport)
//#endif
//
//#include "IVCodec.h"
//extern "C"
//{
//#include "theora/codec.h" 
//#include "theora/theora.h" 
//#include "theora/theoraenc.h" 
//#include "theora/theoradec.h" 
//#include "ogg/ogg.h" 
//};
//class VCodec_Theora :public IVCodec
//{
//public:
//	VCodec_Theora(void);
//	virtual ~VCodec_Theora(void);
//
//	void Release();
//	bool EncodeFrame(const BYTE *in, int inSize, BYTE *out, int &outSize, bool &bKeyFrame);
//	bool DecodeFrame(const BYTE *in, int inSize, BYTE *out, int &outSize);
//	void SetFrameParameter(int width, int height, float frameratezz, unsigned long bitrate, int bitcount);
//	int CheckCodecParam(int width, int height, float framerate, unsigned long bitrate, int bitcount);
//private:
//	bool EncodeInit(BYTE *out, int &outSize);
//	bool DecodeInit(const BYTE *in, int inSize, int &offset);
//	void QueuePage(ogg_page *page);
//	bool generate_dummy_i420(BYTE* buffer, const unsigned int width, const unsigned int height);
//	int frame_w;
//	int frame_h;
//	int pic_w;
//	int pic_h;
//	int pic_x;
//	int pic_y;
//	int video_fps_n;
//	int video_fps_d;
//	int video_par_n;
//	int video_par_d;
//	int src_c_dec_h;
//	int src_c_dec_v;
//	int dst_c_dec_h;
//	int dst_c_dec_v;
//	/*The size of each converted frame buffer.*/
//	size_t y4m_dst_buf_sz;
//	/*The amount to read directly into the converted frame buffer.*/
//	size_t y4m_dst_buf_read_sz;
//	/*The size of the auxilliary buffer.*/
//	size_t y4m_aux_buf_sz;
//	/*The amount to read into the auxilliary buffer.*/
//	size_t y4m_aux_buf_read_sz;
//
//	ogg_stream_state to; /* take physical pages, weld into a logical
//						 stream of packets */
//	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
//	ogg_packet       op; /* one raw packet of data for decode */
//	th_enc_ctx      *encoder;
//	th_dec_ctx      *decoder;
//	th_info             ti;
//	th_comment    tc;
//	th_setup_info  *ts;
//	int bit_rate;
//	int bit_count;
//	float frame_rate;
//	int video_rate;
//	int video_quality;
//	int theora_p;
//	int theora_processing_headers;
//};
//#endif