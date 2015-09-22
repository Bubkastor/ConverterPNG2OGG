#include "PNG_Converter.h"

boost::filesystem::path PNG_EXTENSION = ".png";

PNG_Converter::PNG_Converter(std::string path) :
	m_pathFolder(path)
{
	m_pathAllFile = GetAllPathInFolder(path);
}

PNG_Converter::~PNG_Converter()
{
}

static unsigned char clamp(int d)
{
	if (d < 0)
		return 0;

	if (d > 255)
		return 255;

	return d;
}

static void rgb_to_yuv(png_bytep *png, th_ycbcr_buffer ycbcr, unsigned int w, unsigned int h, int chroma_format)
{
	unsigned int x;
	unsigned int y;

	unsigned int x1;
	unsigned int y1;

	unsigned long yuv_w;

	unsigned char *yuv_y;
	unsigned char *yuv_u;
	unsigned char *yuv_v;

	yuv_w = ycbcr[0].width;

	yuv_y = ycbcr[0].data;
	yuv_u = ycbcr[1].data;
	yuv_v = ycbcr[2].data;

	/*This ignores gamma and RGB primary/whitepoint differences.
	It also isn't terribly fast (though a decent compiler will
	strength-reduce the division to a multiplication).*/

	if (chroma_format == TH_PF_420) {
		for (y = 0; y < h; y += 2) {
			y1 = y + (y + 1 < h);
			for (x = 0; x < w; x += 2) {
				x1 = x + (x + 1 < w);
				png_byte r0 = png[y][3 * x + 0];
				png_byte g0 = png[y][3 * x + 1];
				png_byte b0 = png[y][3 * x + 2];
				png_byte r1 = png[y][3 * x1 + 0];
				png_byte g1 = png[y][3 * x1 + 1];
				png_byte b1 = png[y][3 * x1 + 2];
				png_byte r2 = png[y1][3 * x + 0];
				png_byte g2 = png[y1][3 * x + 1];
				png_byte b2 = png[y1][3 * x + 2];
				png_byte r3 = png[y1][3 * x1 + 0];
				png_byte g3 = png[y1][3 * x1 + 1];
				png_byte b3 = png[y1][3 * x1 + 2];

				yuv_y[x + y * yuv_w] = clamp((65481 * r0 + 128553 * g0 + 24966 * b0 + 4207500) / 255000);
				yuv_y[x1 + y * yuv_w] = clamp((65481 * r1 + 128553 * g1 + 24966 * b1 + 4207500) / 255000);
				yuv_y[x + y1 * yuv_w] = clamp((65481 * r2 + 128553 * g2 + 24966 * b2 + 4207500) / 255000);
				yuv_y[x1 + y1 * yuv_w] = clamp((65481 * r3 + 128553 * g3 + 24966 * b3 + 4207500) / 255000);

				yuv_u[(x >> 1) + (y >> 1) * ycbcr[1].stride] =
					clamp(((-33488 * r0 - 65744 * g0 + 99232 * b0 + 29032005) / 4 +
						(-33488 * r1 - 65744 * g1 + 99232 * b1 + 29032005) / 4 +
						(-33488 * r2 - 65744 * g2 + 99232 * b2 + 29032005) / 4 +
						(-33488 * r3 - 65744 * g3 + 99232 * b3 + 29032005) / 4) / 225930);
				yuv_v[(x >> 1) + (y >> 1) * ycbcr[2].stride] =
					clamp(((157024 * r0 - 131488 * g0 - 25536 * b0 + 45940035) / 4 +
						(157024 * r1 - 131488 * g1 - 25536 * b1 + 45940035) / 4 +
						(157024 * r2 - 131488 * g2 - 25536 * b2 + 45940035) / 4 +
						(157024 * r3 - 131488 * g3 - 25536 * b3 + 45940035) / 4) / 357510);
			}
		}
	}
	else if (chroma_format == TH_PF_444) {
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				png_byte r = png[y][3 * x + 0];
				png_byte g = png[y][3 * x + 1];
				png_byte b = png[y][3 * x + 2];

				yuv_y[x + y * yuv_w] = clamp((65481 * r + 128553 * g + 24966 * b + 4207500) / 255000);
				yuv_u[x + y * yuv_w] = clamp((-33488 * r - 65744 * g + 99232 * b + 29032005) / 225930);
				yuv_v[x + y * yuv_w] = clamp((157024 * r - 131488 * g - 25536 * b + 45940035) / 357510);
			}
		}
	}
	else {  /* TH_PF_422 */
		for (y = 0; y < h; y += 1) {
			for (x = 0; x < w; x += 2) {
				x1 = x + (x + 1 < w);
				png_byte r0 = png[y][3 * x + 0];
				png_byte g0 = png[y][3 * x + 1];
				png_byte b0 = png[y][3 * x + 2];
				png_byte r1 = png[y][3 * x1 + 0];
				png_byte g1 = png[y][3 * x1 + 1];
				png_byte b1 = png[y][3 * x1 + 2];

				yuv_y[x + y * yuv_w] = clamp((65481 * r0 + 128553 * g0 + 24966 * b0 + 4207500) / 255000);
				yuv_y[x1 + y * yuv_w] = clamp((65481 * r1 + 128553 * g1 + 24966 * b1 + 4207500) / 255000);

				yuv_u[(x >> 1) + y * ycbcr[1].stride] =
					clamp(((-33488 * r0 - 65744 * g0 + 99232 * b0 + 29032005) / 2 +
						(-33488 * r1 - 65744 * g1 + 99232 * b1 + 29032005) / 2) / 225930);
				yuv_v[(x >> 1) + y * ycbcr[2].stride] =
					clamp(((157024 * r0 - 131488 * g0 - 25536 * b0 + 45940035) / 2 +
						(157024 * r1 - 131488 * g1 - 25536 * b1 + 45940035) / 2) / 357510);
			}
		}
	}

}

void user_read_data(png_structp png_p, png_bytep data, png_size_t length)
{
	png_voidp tmp = png_get_io_ptr(png_p);
	reinterpret_cast<std::istream *>(tmp)->read(reinterpret_cast<char *>(data), length);
}

void PNG_Converter::MakeYcbcr()
{
	for (auto it : m_pathAllFile)
	{
		
		char header[4];
		png_structp png_ptr;
		png_infop info_ptr;
		png_infop end_ptr;
		png_bytep row_data;
		png_bytep *row_pointers;
		png_color_16p bkgd;
		png_uint_32 width;
		png_uint_32 height;
		unsigned long yuv_w;
		unsigned long yuv_h;
		int bit_depth;
		int color_type;
		int interlace_type;
		int compression_type;
		int filter_method;
		png_uint_32 y;
		unsigned int *w = 0;
		unsigned int *h = 0;
		th_ycbcr_buffer ycbcr;

		static int chroma_format = TH_PF_420;

		std::ifstream file;

		file.open(it.string(), std::ios::binary);
		file.read(header, 4);

		
		png_sig_cmp(reinterpret_cast<png_const_bytep>(header), 0, 4);

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		info_ptr = png_create_info_struct(png_ptr);
		end_ptr = png_create_info_struct(png_ptr);

		if (!png_ptr && !info_ptr && !end_ptr)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			std::cout << it << "couldn't create png info structure" << std::endl;
		}

		png_set_read_fn(png_ptr, reinterpret_cast<png_voidp>(&file), user_read_data);

		png_set_sig_bytes(png_ptr, 4);

		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, &compression_type, &filter_method);
		png_set_expand(png_ptr);

		

		if (bit_depth < 8)png_set_packing(png_ptr);
		if (bit_depth == 16)png_set_strip_16(png_ptr);
		if (!(color_type&PNG_COLOR_MASK_COLOR))png_set_gray_to_rgb(png_ptr);
		if (png_get_bKGD(png_ptr, info_ptr, &bkgd)) {
			png_set_background(png_ptr, bkgd, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		}

		png_set_strip_alpha(png_ptr);

		row_data = (png_bytep)png_malloc(png_ptr, 3 * height*width*sizeof(*row_data));
		row_pointers = (png_bytep *)png_malloc(png_ptr, height*sizeof(*row_pointers));
		for (y = 0; y < height; y++) {
			row_pointers[y] = row_data + y*(3 * width);
		}
		png_read_image(png_ptr, row_pointers);
		png_read_end(png_ptr, end_ptr);

		*w = width;
		*h = height;
		/* Must hold: yuv_w >= w */
		yuv_w = (*w + 15) & ~15;
		/* Must hold: yuv_h >= h */
		yuv_h = (*h + 15) & ~15;


		//			chroma_format = TH_PF_444;
		//			chroma_format = TH_PF_422;

		if (!ycbcr[0].data) {
			ycbcr[0].width = yuv_w;
			ycbcr[0].height = yuv_h;
			ycbcr[0].stride = yuv_w;
			ycbcr[1].width = (chroma_format == TH_PF_444) ? yuv_w : (yuv_w >> 1);
			ycbcr[1].stride = ycbcr[1].width;
			ycbcr[1].height = (chroma_format == TH_PF_420) ? (yuv_h >> 1) : yuv_h;
			ycbcr[2].width = ycbcr[1].width;
			ycbcr[2].stride = ycbcr[1].stride;
			ycbcr[2].height = ycbcr[1].height;

			ycbcr[0].data = (unsigned char*)malloc(ycbcr[0].stride * ycbcr[0].height);
			ycbcr[1].data = (unsigned char*)malloc(ycbcr[1].stride * ycbcr[1].height);
			ycbcr[2].data = (unsigned char*)malloc(ycbcr[2].stride * ycbcr[2].height);

		}
		else {
			if ((ycbcr[0].width != yuv_w) || (ycbcr[0].height != yuv_h)) {
				fprintf(stderr, "Input size %lux%lu does not match %dx%d\n", yuv_w, yuv_h, ycbcr[0].width, ycbcr[0].height);
				exit(1);
			}
		}

		rgb_to_yuv(row_pointers, ycbcr, *w, *h, chroma_format);
		//m_arrayYcbcrBuffer.push_back(ycbcr);
		png_free(png_ptr, row_pointers);
		png_free(png_ptr, row_data);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		
		file.close();
	}
	std::cout << m_arrayYcbcrBuffer.size();
}

std::vector<th_ycbcr_buffer> PNG_Converter::GetYcbcr()
{
	return std::vector<th_ycbcr_buffer>();
}

std::vector<boost::filesystem::path> PNG_Converter::GetAllPathInFolder(std::string pathRead)
{
	using namespace boost::filesystem;
	std::vector<boost::filesystem::path> result;

	if (is_directory(m_pathFolder))
	{
		for (auto x : directory_iterator(m_pathFolder))
			CheckExtension(x.path())?			
			result.push_back(x.path()) :
			ErrorFile(x.path().string());
	}
	return result;
}

void PNG_Converter::ErrorFile(std::string path)
{
	std::cout << path << " not png" << std::endl;
}

bool PNG_Converter::CheckExtension(boost::filesystem::path path)
{
	return path.extension() == PNG_EXTENSION;
}
