//
//
//#include "stdafx.h"   
//#include <iostream>   
//#include "VCodec_Theora.h"   
//int frame_num = 0;
//
//BOOL APIENTRY DllMain(HANDLE hModule,
//	DWORD  ul_reason_for_call,
//	LPVOID lpReserved
//	)
//{
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//	return TRUE;
//}
//
//VCodec_Theora::VCodec_Theora(void)
//	:frame_w(0), frame_h(0), pic_w(0), pic_h(0), pic_x(0), pic_y(0),
//	video_fps_n(16), video_fps_d(1), video_par_n(1), video_par_d(1),
//	src_c_dec_h(2), src_c_dec_v(2), dst_c_dec_h(2), dst_c_dec_v(2),
//	y4m_dst_buf_sz(0), y4m_dst_buf_read_sz(0), y4m_aux_buf_sz(0),
//	y4m_aux_buf_read_sz(0), encoder(NULL), decoder(NULL), ts(NULL),
//	bit_rate(0), bit_count(0), frame_rate(0), video_rate(-1), video_quality(48),
//	theora_p(0), theora_processing_headers(0)
//{
//}
//VCodec_Theora::~VCodec_Theora(void)
//{
//	th_comment_clear(&tc);
//	ogg_stream_clear(&to);
//	th_info_clear(&ti);
//	if (encoder != NULL)
//	{
//		th_encode_free(encoder);
//		encoder = NULL;
//	}
//	if (decoder != NULL)
//	{
//		th_decode_free(decoder);
//		decoder = NULL;
//	}
//}
//void VCodec_Theora::SetFrameParameter(int width, int height, float frameratezz, unsigned long bitrate, int bitcount)
//{
//	this->pic_w = width;
//	this->pic_h = height;
//	this->frame_rate = frameratezz;
//	this->bit_rate = bitrate;
//	this->bit_count = bitcount;
//	/* Theora has a divisible-by-sixteen restriction for the encoded frame size */
//	/* scale the picture size up to the nearest /16 and calculate offsets */
//	this->frame_w = pic_w + 15 & ~0xF;
//	this->frame_h = pic_h + 15 & ~0xF;
//	/*Force the offsets to be even so that chroma samples line up like we
//	expect.*/
//	this->pic_x = frame_w - pic_w >> 1 & ~1;
//	this->pic_y = frame_h - pic_h >> 1 & ~1;
//}
//
//bool VCodec_Theora::EncodeInit(BYTE *out, int &outSize)
//{
//	int ret;
//	th_info_init(&ti);
//	ti.frame_width = frame_w;
//	ti.frame_height = frame_h;
//	ti.pic_width = pic_w;
//	ti.pic_height = pic_h;
//	ti.pic_x = pic_x;
//	ti.pic_y = pic_y;
//	ti.fps_numerator = video_fps_n;
//	ti.fps_denominator = video_fps_d;
//	ti.aspect_numerator = video_par_n;
//	ti.aspect_denominator = video_par_d;
//
//	ti.colorspace = TH_CS_UNSPECIFIED;
//	ti.target_bitrate = video_rate;
//	ti.quality = video_quality;
//	ti.keyframe_granule_shift = 6;
//	ti.pixel_fmt = TH_PF_420;
//
//	encoder = th_encode_alloc(&ti);
//	if (encoder == NULL)
//	{
//		std::cout << "negative return code initializing encoder!" << std::endl;
//		th_info_clear(&ti);
//		return false;
//	}
//
//	th_comment_init(&tc);
//	ogg_stream_init(&to, rand());
//	if (th_encode_flushheader(encoder, &tc, &op) <= 0)
//	{
//		std::cout << "Internal Theora Library Error!" << std::endl;
//		return false;
//	}
//	ogg_stream_packetin(&to, &op);
//	if (ogg_stream_pageout(&to, &og) != 1)
//	{
//		std::cout << "Internal Ogg library Error!" << std::endl;
//		return false;
//	}
//	memcpy(out + outSize, &og.header_len, sizeof(long));
//	memcpy(out + outSize + sizeof(long), &og.body_len, sizeof(long));
//	memcpy(out + outSize + 2 * sizeof(long), og.header, og.header_len);
//	memcpy(out + outSize + 2 * sizeof(long) + og.header_len, og.body, og.body_len);
//	outSize += og.header_len + og.body_len + 2 * sizeof(long);
//	for (;;)
//	{
//		ret = th_encode_flushheader(encoder, &tc, &op);
//		if (ret<0)
//		{
//			std::cout << "Internal Theora library error." << std::endl;
//			if (encoder != NULL)
//			{
//				th_encode_free(encoder);
//				encoder = NULL;
//			}
//			th_comment_clear(&tc);
//			th_info_clear(&ti);
//			return false;
//		}
//		else if (!ret)break;
//		ogg_stream_packetin(&to, &op);
//	}
//	for (;;) {
//		int result = ogg_stream_flush(&to, &og);
//		if (result<0) {
//			std::cout << "Internal Ogg library error." << std::endl;
//			return false;
//		}
//		if (result == 0)break;
//		memcpy(out + outSize, &og.header_len, sizeof(long));
//		memcpy(out + outSize + sizeof(long), &og.body_len, sizeof(long));
//		memcpy(out + 2 * sizeof(long) + outSize, og.header, og.header_len);
//		memcpy(out + 2 * sizeof(long) + outSize + og.header_len, og.body, og.body_len);
//		outSize += og.header_len + og.body_len + 2 * sizeof(long);
//	}
//	return true;
//}
//void VCodec_Theora::QueuePage(ogg_page *page)
//{
//	if (theora_p)
//		ogg_stream_pagein(&to, page);
//}
//
//bool VCodec_Theora::DecodeInit(const BYTE *in, int inSize, int &offset)
//{
//	int ret;
//	th_info_init(&ti);
//	th_comment_init(&tc);
//	while (inSize - offset)
//	{
//		memcpy(&og.header_len, in + offset, sizeof(long));
//		memcpy(&og.body_len, in + offset + sizeof(long), sizeof(long));
//		og.header = (unsigned char*)in + offset + 2 * sizeof(long);
//		og.body = (unsigned char*)in + offset + 2 * sizeof(long) + og.header_len;
//		offset += og.header_len + og.body_len + 2 * sizeof(long);
//		ogg_stream_state test;
//		if (!ogg_page_bos(&og))
//		{
//			QueuePage(&og);
//			break;
//		}
//		ogg_stream_init(&test, ogg_page_serialno(&og));
//		ogg_stream_pagein(&test, &og);
//		ogg_stream_packetpeek(&test, &op);
//		if (!theora_p && (theora_processing_headers =
//			th_decode_headerin(&ti, &tc, &ts, &op)) >= 0)
//		{
//			memcpy(&to, &test, sizeof(test));
//			theora_p = 1;
//			/*Advance past the successfully processed header.*/
//			if (theora_processing_headers) ogg_stream_packetout(&to, NULL);
//		}
//		else
//		{
//			/* whatever it is, we don't care about it */
//			ogg_stream_clear(&test);
//		}
//
//	}
//
//	/* we're expecting more header packets. */
//
//	while (theora_p && theora_p<3)
//	{
//		int ret;
//		/* look for further theora headers */
//		while (theora_processing_headers && (ret = ogg_stream_packetpeek(&to, &op)))
//		{
//			if (ret<0)continue;
//			theora_processing_headers = th_decode_headerin(&ti, &tc, &ts, &op);
//			if (theora_processing_headers<0)
//			{
//				th_info_clear(&ti);
//				th_comment_clear(&tc);
//				std::cout << "Error parsing Theora stream headers; corrupt stream" << std::endl;
//				return false;
//			}
//			else if (theora_processing_headers>0)
//			{
//				/*Advance past the successfully processed header.*/
//				ogg_stream_packetout(&to, NULL);
//			}
//			theora_p++;
//			if (theora_p == 3)break;
//			if (inSize - offset)
//			{
//				memcpy(&og.header_len, in + offset, sizeof(long));
//				memcpy(&og.body_len, in + offset + sizeof(long), sizeof(long));
//				og.header = (unsigned char*)in + offset + 2 * sizeof(long);
//				og.body = (unsigned char*)in + offset + 2 * sizeof(long) + og.header_len;
//				offset += og.header_len + og.body_len + 2 * sizeof(long);
//				QueuePage(&og);
//			}
//		}
//		/*Stop now so we don't fail if there aren't enough pages in a short
//		stream.*/
//		if (!(theora_p && theora_processing_headers))break;
//	}
//
//	if (theora_p)
//	{
//		this->decoder = th_decode_alloc(&ti, ts);
//		if (this->decoder == NULL)
//		{
//			std::cout << "negative return code initializing decoder!" << std::endl;
//			th_info_clear(&ti);
//			th_comment_clear(&tc);
//			return false;
//		}
//	}
//	else
//	{
//		th_info_clear(&ti);
//		th_comment_clear(&tc);
//		return false;
//	}
//	th_setup_free(ts);
//	return true;
//}
//bool VCodec_Theora::EncodeFrame(const BYTE *in, int inSize, BYTE *out, int &outSize, bool &bKeyFrame)
//{
//	outSize = 0;
//	const BYTE     *yuvframe;
//	th_ycbcr_buffer     ycbcr;
//	int                      pic_sz;
//	int                      frame_c_w;
//	int                      frame_c_h;
//	int                      c_w;
//	int                      c_h;
//	int                      c_sz;
//	int                      ret;
//	yuvframe = in;
//	pic_sz = pic_w*pic_h;
//	frame_c_w = frame_w / dst_c_dec_h;
//	frame_c_h = frame_h / dst_c_dec_v;
//	c_w = (pic_w + dst_c_dec_h - 1) / dst_c_dec_h;
//	c_h = (pic_h + dst_c_dec_v - 1) / dst_c_dec_v;
//	c_sz = c_w*c_h;
//	//yuvframe=new BYTE[1.5*frame_h*frame_w];   
//	if (encoder == NULL)
//	{
//		bool ret;
//		ret = EncodeInit(out, outSize);
//		if (!ret)
//		{
//			std::cout << "initial encoder failed!" << std::endl;
//			return false;
//		}
//	}
//	/*
//	while (true)
//	{
//	if (ogg_stream_pageout(&to,&og)>0)
//	{
//	memcpy(out+outSize,&og.header_len,sizeof(long));
//	memcpy(out+outSize+sizeof(long),&og.body_len,sizeof(long));
//	memcpy(out+outSize+2*sizeof(long),og.header,og.header_len);
//	memcpy(out+outSize+2*sizeof(long)+og.header_len,og.body,og.body_len);
//	outSize+=og.header_len+og.body_len+2*sizeof(long);
//	return true;
//	}
//	if(generate_dummy_i420(yuvframe,(unsigned int)this->frame_w,(unsigned int)this->frame_h))
//	{
//	ycbcr[0].width=frame_w;
//	ycbcr[0].height=frame_h;
//	ycbcr[0].stride=pic_w;
//	ycbcr[0].data=(unsigned char*)yuvframe-pic_x-pic_y*pic_w;
//	ycbcr[1].width=frame_c_w;
//	ycbcr[1].height=frame_c_h;
//	ycbcr[1].stride=c_w;
//	ycbcr[1].data=(unsigned char*)yuvframe+pic_sz;
//	ycbcr[2].width=frame_c_w;
//	ycbcr[2].height=frame_c_h;
//	ycbcr[2].stride=c_w;
//	ycbcr[2].data=ycbcr[1].data+(pic_sz>>2);
//	ret=th_encode_ycbcr_in(encoder,ycbcr);
//	for (;;)
//	{
//	ret=th_encode_packetout(encoder,0,&op);
//	if (!ret) break;
//	else    if (ret<0)
//	{
//	std::cout<<"failed to retrieve compressed frame!"<<std::endl;
//	return false;
//	}
//	ogg_stream_packetin(&to,&op);
//	}
//	}
//
//	}
//	*/
//	ycbcr[0].width = frame_w;
//	ycbcr[0].height = frame_h;
//	ycbcr[0].stride = pic_w;
//	ycbcr[0].data = (unsigned char*)yuvframe - pic_x - pic_y*pic_w;
//	ycbcr[1].width = frame_c_w;
//	ycbcr[1].height = frame_c_h;
//	ycbcr[1].stride = c_w;
//	ycbcr[1].data = (unsigned char*)yuvframe + pic_sz;
//	ycbcr[2].width = frame_c_w;
//	ycbcr[2].height = frame_c_h;
//	ycbcr[2].stride = c_w;
//	ycbcr[2].data = ycbcr[1].data + (pic_sz >> 2);
//
//
//	ret = th_encode_ycbcr_in(encoder, ycbcr);
//	if (ret<0)
//	{
//		std::cout << "negative error code submitting frame for compression!" << std::endl;
//		return false;
//	}
//	for (;;)
//	{
//		ret = th_encode_packetout(encoder, 0, &op);
//		if (!ret) break;
//		else    if (ret<0)
//		{
//			std::cout << "failed to retrieve compressed frame!" << std::endl;
//			return false;
//		}
//		ogg_stream_packetin(&to, &op);
//	}
//	for (;;)
//	{
//		ret = ogg_stream_flush(&to, &og);
//		if (ret)
//		{
//
//			memcpy(out + outSize, &og.header_len, sizeof(long));
//			memcpy(out + outSize + sizeof(long), &og.body_len, sizeof(long));
//			memcpy(out + outSize + 2 * sizeof(long), og.header, og.header_len);
//			memcpy(out + outSize + 2 * sizeof(long) + og.header_len, og.body, og.body_len);
//			outSize += og.header_len + og.body_len + 2 * sizeof(long);
//			return true;
//
//		}
//		else if (!ret) return true;
//	}
//
//	return false;
//
//}
//bool VCodec_Theora::DecodeFrame(const BYTE *in, int inSize, BYTE *out, int &outSize)
//{
//	outSize = 0;
//	th_ycbcr_buffer            y_buffer;
//	int offset = 0;
//	memset(&y_buffer[0], 0, sizeof(th_img_plane));
//	memset(&y_buffer[1], 0, sizeof(th_img_plane));
//	memset(&y_buffer[2], 0, sizeof(th_img_plane));
//	if (decoder == NULL)
//	{
//		bool ret;
//		ret = DecodeInit(in, inSize, offset);
//		if (!ret)
//		{
//			std::cout << "initial decoder failed!" << std::endl;
//			return false;
//		}
//	}
//	while (inSize - offset)
//	{
//		memcpy(&og.header_len, in + offset, sizeof(long));
//		memcpy(&og.body_len, in + offset + sizeof(long), sizeof(long));
//		og.header = (unsigned char*)in + offset + 2 * sizeof(long);
//		og.body = (unsigned char*)in + offset + 2 * sizeof(long) + og.header_len;
//		offset += og.header_len + og.body_len + 2 * sizeof(long);
//		QueuePage(&og);
//	}
//	while (theora_p)
//	{
//		if (ogg_stream_packetout(&to, &op)>0)
//			if (th_decode_packetin(decoder, &op, NULL) >= 0)
//			{
//				int ret;
//				if (ret = th_decode_ycbcr_out(decoder, y_buffer))
//				{
//					std::cout << "output decoded Y'CbCr data error!" << std::endl;
//					return false;
//				}
//			}
//			else
//			{
//				std::cout << "submit packet to decoder error!" << std::endl;
//				return false;
//			}
//		else break;
//
//	}
//
//	for (int j = 0; j<y_buffer[0].height; j++)
//	{
//		memcpy(out + j*y_buffer[0].width, y_buffer[0].data + y_buffer[0].stride*j, y_buffer[0].width);
//	}
//	for (int i = 1; i<3; i++)
//	{
//		for (int j = 0; j<y_buffer[i].height; j++)
//		{
//			memcpy(out + y_buffer[0].height*y_buffer[0].width + (i - 1)*y_buffer[i].height*y_buffer[i].width + j*y_buffer[i].width
//				, y_buffer[i].data + y_buffer[i].stride*j, y_buffer[i].width);
//		}
//	}
//	outSize = frame_h*frame_w + 2 * (frame_h / dst_c_dec_h)*(frame_w / dst_c_dec_v);
//	return true;
//}
//void VCodec_Theora::Release()
//{
//	delete this;
//}
//int VCodec_Theora::CheckCodecParam(int width, int height, float framerate, unsigned long bitrate, int bitcount)
//{
//	int compre = -1;//initialization no change  
//	if (!((width == this->pic_w) && (height == this->pic_h) && (framerate == this->frame_rate)))
//		compre = 1;//parameter changed  
//
//	if (bitrate != this->bit_rate)
//	{
//		if (compre != 1)
//			compre = 2;//bit rate changed  
//		else
//			compre = 3;//bit rate and parameter changed  
//	}
//
//	return compre;//return resul   
//}
//
//IVCodec* IVCodec_Creater()
//{
//	return new VCodec_Theora;
//}
//bool VCodec_Theora::generate_dummy_i420(BYTE* buffer, const unsigned int width, const unsigned int height)
//{
//	const int nbytes = width*height;
//	BYTE* Y = buffer;
//	BYTE* U = Y + nbytes;
//	BYTE* V = U + (nbytes >> 2);
//	int Y_line = width;
//	int U_line = width >> 1;
//	int V_line = width >> 1;
//
//
//	/* Y */
//	for (unsigned int y = 0; y<height; y++) {
//		for (unsigned int x = 0; x<width; x++) {
//			Y[y * Y_line + x] = (BYTE)(x + y + frame_num * 3);
//		}
//	}
//
//	/* Cb and Cr */
//	for (unsigned int y = 0; y<height / 2; y++) {
//		for (unsigned int x = 0; x<width / 2; x++) {
//			U[y * U_line + x] = (BYTE)(128 + y + frame_num * 2);
//			V[y * V_line + x] = (BYTE)(64 + x + frame_num * 5);
//		}
//	}
//
//	frame_num++;
//	return true;
//}
