#ifndef H_PNGIMAGE
#define H_PNGIMAGE

class PngJpegImage
{
public:
	PngJpegImage();
	PngJpegImage( const char *fileName );
	~PngJpegImage();
	bool readFile_PNG( const char *fileName );
	bool readFile_JPEG( const char *fileName );
	bool freeImage();
	bool displayOn( HDC hdc, int x0, int y0 );
	bool getParams( int *w, int *h, int *bpp, int *ch ) const;
	bool getFileName( char *out, size_t cchMax ) const;
protected:
	void initnull();
	void freeself();
protected:
	int number;
	unsigned char b_header[12];
	char          m_filename[512];
	// lib PNG
		png_structp   png_ptr;
		png_infop     info_ptr;
		// info
		int width;
		int height;
		int bit_depth;
		int rowbytes;
		int channels;
		int color_type; // depends on image type - png or jpeg have own constants :(
		// image data
		unsigned char     **row_pointers;
	// lib JPEG
		jpeg_decompress_struct jpeg_cinfo;
		jpeg_error_mgr         jpeg_err;
	// image type
	int image_is_png;
	int image_is_jpeg;
};

#endif
