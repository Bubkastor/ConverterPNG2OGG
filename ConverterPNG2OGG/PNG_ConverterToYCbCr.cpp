//#include "PNG_ConverterToYCbCr.h"
//
//
//PNG_ConverterToYCbCr::PNG_ConverterToYCbCr(std::string path) :
//	m_pathFolder(path)
//{
//	m_pathAllFile = GetAllPathInFolder(path);
//}
//
//PNG_ConverterToYCbCr::~PNG_ConverterToYCbCr()
//{
//}
//
//
//
//
//void PNG_ConverterToYCbCr::MakeYcbcr()
//{
//	for (size_t i = 0; i < m_pathAllFile.size(); i++)
//	{
//		std::string filename = m_pathAllFile.at(i).string();
//
//		std::vector<unsigned char> png;
//		std::vector<unsigned char> image;
//
//		unsigned width, height;
//		unsigned char *yuv;
//		unsigned int yuv_w;
//		unsigned int yuv_h;
//
//		unsigned char *yuv_y;
//		unsigned char *yuv_u;
//		unsigned char *yuv_v;
//
//		unsigned int x;
//		unsigned int y;
//
//		lodepng::load_file(png, filename);
//		unsigned error = lodepng::decode(image, width, height, png);
//
//		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
//
//
//		yuv = rgb_to_yuv(image, width, height);
//
//		yuv_w = (width + 15) & ~15;
//		yuv_h = (height + 15) & ~15;
//
//
//		th_img_plane *ycbcr_ptr = new th_img_plane[3];
//		ycbcr_ptr[0].width = yuv_w;
//		ycbcr_ptr[0].height = yuv_h;
//		ycbcr_ptr[0].stride = yuv_w;
//		ycbcr_ptr[1].width = (chroma_format == TH_PF_444) ? yuv_w : (yuv_w >> 1);
//		ycbcr_ptr[1].stride = ycbcr_ptr[1].width;
//		ycbcr_ptr[1].height = (chroma_format == TH_PF_420) ? (yuv_h >> 1) : yuv_h;
//		ycbcr_ptr[2].width = ycbcr_ptr[1].width;
//		ycbcr_ptr[2].stride = ycbcr_ptr[1].stride;
//		ycbcr_ptr[2].height = ycbcr_ptr[1].height;
//
//
//		yuv_y = (unsigned char*)malloc(ycbcr_ptr[0].stride * ycbcr_ptr[0].height);
//		if (!yuv_y)
//			throw "malloc failed in WriteFrame for yuv_y";
//
//		yuv_u = (unsigned char*)malloc(ycbcr_ptr[1].stride * ycbcr_ptr[1].height);
//		if (!yuv_u)
//			throw "malloc failed in WriteFrame for yuv_u";
//
//		yuv_v = (unsigned char*)malloc(ycbcr_ptr[2].stride * ycbcr_ptr[2].height);
//		if (!yuv_u)
//			throw "malloc failed in WriteFrame for yuv_v";
//
//		ycbcr_ptr[0].data = yuv_y;
//		ycbcr_ptr[1].data = yuv_u;
//		ycbcr_ptr[2].data = yuv_v;	
//
//		for (y = 0; y < height; y++) {
//			for (x = 0; x < width; x++) {
//				yuv_y[x + y * yuv_w] = yuv[3 * (x + y * width) + 0];
//			}
//		}
//
//		if (chroma_format == TH_PF_420) {
//			for (y = 0; y < height; y += 2) {
//				for (x = 0; x < width; x += 2) {
//					yuv_u[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * width) + 1];
//					yuv_v[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * width) + 2];
//				}
//			}
//		}
//		else if (chroma_format == TH_PF_444) {
//			for (y = 0; y < height; y++) {
//				for (x = 0; x < width; x++) {
//					yuv_u[x + y * ycbcr_ptr[1].stride] = yuv[3 * (x + y * width) + 1];
//					yuv_v[x + y * ycbcr_ptr[2].stride] = yuv[3 * (x + y * width) + 2];
//				}
//			}
//		}
//		else {  // TH_PF_422 
//			for (y = 0; y < height; y += 1) {
//				for (x = 0; x < width; x += 2) {
//					yuv_u[(x >> 1) + y * ycbcr_ptr[1].stride] = yuv[3 * (x + y * width) + 1];
//					yuv_v[(x >> 1) + y * ycbcr_ptr[2].stride] = yuv[3 * (x + y * width) + 2];
//				}
//			}
//		}
//
//		
//		m_arrayYcbcrBuffer.push_back(ycbcr_ptr);
//	}
//
//}
//
//
//
//
//std::vector<th_img_plane*> PNG_ConverterToYCbCr::GetYcbcr()
//{
//	return m_arrayYcbcrBuffer;
//}
//std::vector<boost::filesystem::path> PNG_ConverterToYCbCr::GetAllPathInFolder(std::string pathRead)
//{
//	using namespace boost::filesystem;
//	std::vector<boost::filesystem::path> result;
//
//	if (is_directory(m_pathFolder))
//	{
//		for (auto x : directory_iterator(m_pathFolder))
//			CheckExtension(x.path()) ?
//			result.push_back(x.path()) :
//			ErrorFile(x.path().string());
//	}
//	return result;
//}
//
//
//
