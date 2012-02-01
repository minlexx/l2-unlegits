#include "stdafx.h"
#if 0
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include "PngJpegImage.h"


PngJpegImage::PngJpegImage() { initnull(); }
PngJpegImage::~PngJpegImage() { freeself(); }

bool PngJpegImage::readFile_PNG( const char *fileName )
{
	if( !fileName ) return FALSE;
	
	// try to open file 1st
	FILE *fp = fopen( fileName, "rb" );
    if( !fp )
    {
		SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }
    
    // check is it png?
    fread( b_header, 1, number, fp );
    int is_png = !png_sig_cmp( b_header, 0, number );
    if( !is_png )
    {
		SetLastError( ERROR_INVALID_DATA );
		fclose( fp );
        return FALSE;
    }
	
	memset( m_filename, 0, sizeof(m_filename) );
	strncpy( m_filename, fileName, sizeof(m_filename)-1 );
    
    // now free possible previous data
    freeself();
    
    // and read new file
    
    // allocate libPNG structs
    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if( !png_ptr )
	{
		SetLastError( ERROR_NOT_ENOUGH_MEMORY );
		png_ptr = NULL;
		fclose( fp );
		return FALSE;
	}
	
	info_ptr = png_create_info_struct( png_ptr );
	if( !info_ptr )
	{
		png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
		SetLastError( ERROR_NOT_ENOUGH_MEMORY );
		png_ptr = NULL;
		info_ptr = NULL;
		fclose( fp );
		return FALSE;
	}
	
	png_init_io( png_ptr, fp );
	png_set_sig_bytes( png_ptr, number );
	png_read_info( png_ptr, info_ptr );
	
	width = png_get_image_width( png_ptr, info_ptr );
	height = png_get_image_height( png_ptr, info_ptr );
	bit_depth = png_get_bit_depth( png_ptr, info_ptr );
	rowbytes = png_get_rowbytes( png_ptr, info_ptr );
	channels = png_get_channels( png_ptr, info_ptr );
	color_type = png_get_color_type( png_ptr, info_ptr );

	/**  // color types.  Note that not all combinations are legal
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
// aliases 
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA  PNG_COLOR_TYPE_GRAY_ALPHA **/
	
	// allocate space for image data
	row_pointers = (unsigned char **)png_malloc( png_ptr, height*sizeof(png_bytep) );
	if( !row_pointers )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
		SetLastError( ERROR_NOT_ENOUGH_MEMORY );
		png_ptr = NULL;
		info_ptr = NULL;
		fclose( fp );
		return FALSE;
	}
	
	int i = 0;
	for( i=0; i<height; i++ )
	{
		row_pointers[i] = (unsigned char *)png_malloc( png_ptr, rowbytes );
		if( !row_pointers[i] )
		{
			png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
			SetLastError( ERROR_NOT_ENOUGH_MEMORY );
			png_ptr = NULL;
			info_ptr = NULL;
			fclose( fp );
			return false;
		}
	}
	
	png_read_image( png_ptr, row_pointers );
	png_read_end( png_ptr, info_ptr );
	
	// we no longer need all libPNG stuff, we just need image data row_pointers // REALLY DON'T NEED??
	//png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
	//png_ptr = NULL;
	//info_ptr = NULL;
	
	fclose( fp );
	fp = NULL;
	image_is_png = 1;
	image_is_jpeg = 0;
	return true;
}


bool PngJpegImage::readFile_JPEG( const char *fileName )
{
	if( !fileName ) return false;

	// try to open file 1st
	FILE *fp = fopen( fileName, "rb" );
    if( !fp )
    {
		SetLastError( ERROR_FILE_NOT_FOUND );
        return false;
    }

	// init libjpeg decompressor
	jpeg_cinfo.err = jpeg_std_error( &jpeg_err );
	jpeg_create_decompress( &jpeg_cinfo );
	// set file as data source
	jpeg_stdio_src( &jpeg_cinfo, fp );
	// read image header
	jpeg_read_header( &jpeg_cinfo, TRUE );

	width = jpeg_cinfo.image_width;
	height = jpeg_cinfo.image_height;
	bit_depth = 8 * jpeg_cinfo.num_components;
	color_type = jpeg_cinfo.jpeg_color_space;
	rowbytes = width * jpeg_cinfo.num_components * 8;
	//printf( "JPEG: %d x %d @ %d (color space: %d)\n", width, height, bit_depth, color_type );
	/* Known color spaces. */
//typedef enum {
//	JCS_UNKNOWN,		/* error/unspecified */
//	JCS_GRAYSCALE,		/* monochrome */
//	JCS_RGB,		/* red/green/blue */
//	JCS_YCbCr,		/* Y/Cb/Cr (also known as YUV) */
//	JCS_CMYK,		/* C/M/Y/K */
//	JCS_YCCK		/* Y/Cb/Cr/K */
//} J_COLOR_SPACE; */
	// we do not set any transforms, so output parameters are like in original image

	// allocate row pointers array
	row_pointers = (unsigned char **)malloc( height * sizeof(void *) );
	if( !row_pointers )
	{
		jpeg_abort( (j_common_ptr)&jpeg_cinfo );
		jpeg_destroy_decompress( &jpeg_cinfo );
		SetLastError( ERROR_NOT_ENOUGH_MEMORY );
		return false;
	}

	// allocate each row
	int i = 0;
	for( i=0; i<height; i++ )
	{
		row_pointers[i] = (unsigned char *)malloc( rowbytes );
		if( !row_pointers[i] )
		{
			jpeg_abort( (j_common_ptr)&jpeg_cinfo );
			jpeg_destroy_decompress( &jpeg_cinfo );
			SetLastError( ERROR_NOT_ENOUGH_MEMORY );
			free( row_pointers );
			return false;
		}
	}

	// start
	jpeg_start_decompress( &jpeg_cinfo );

	//dummy array
	unsigned char *one_row = (unsigned char *)malloc( rowbytes );
	unsigned char **dummy_array_of_one_row = &one_row;

	/*JSAMPARRAY unsigned char **
		JSAMPROW unsigned char *
		JSAMPLE unsigned char*/
	i = 0;
	int iread = 0;
	while( jpeg_cinfo.output_scanline < jpeg_cinfo.output_height )
	{
		//jpeg_read_scanlines( &jpeg_cinfo, (JSAMPARRAY)row_pointers, height );
		//jpeg_read_scanlines( &jpeg_cinfo, (JSAMPARRAY)row_pointers[jpeg_cinfo.output_scanline], 1 );
		iread = jpeg_read_scanlines( &jpeg_cinfo, (JSAMPARRAY)dummy_array_of_one_row, 1 );
		//printf( "read: %d (i = %d)\n", iread, i );
		memcpy( row_pointers[i], one_row, rowbytes );
		i++;
	}

	free( one_row ); one_row = NULL;

	// end decompress free libjpeg's mem
	jpeg_finish_decompress( &jpeg_cinfo );
	jpeg_destroy_decompress( &jpeg_cinfo );

	fclose( fp );
	image_is_jpeg = 1;
	image_is_png = 0;
	return true;
}

bool PngJpegImage::freeImage() { freeself(); return TRUE; }

bool PngJpegImage::displayOn( HDC hdc, int x0, int y0 )
{
	if( !row_pointers ) return FALSE;
	if( height < 1 ) return FALSE;
	if( width < 1 ) return FALSE;
	int i, j;
	unsigned char **p = row_pointers;
	COLORREF crColor = 0; // pixel color in RGB mode
	unsigned char alpha = 0; // alpha channel ;) only for RGBA color mode // not used ><
	// TODO: other color types or set translations while reading
	for( i=0; i<height; i++ )
	{
		for( j=0; j<width; j++ )
		{
			if( image_is_png )
			{
				if( (bit_depth == 8) && (channels == 3) && (color_type == PNG_COLOR_TYPE_RGB) )
					crColor = RGB( p[i][j*3 + 0], p[i][j*3 + 1], p[i][j*3 + 2] );
				if( (bit_depth == 8) && (channels == 4) && (color_type == PNG_COLOR_TYPE_RGBA) )
				{
					crColor = RGB( p[i][j*4 + 0], p[i][j*4 + 1], p[i][j*4 + 2] );
					alpha = p[i][j*4 + 3]; // alpha channel ;) only for RGBA color mode // not used ><
				}
			}
			else if( image_is_jpeg )
			{
				if( jpeg_cinfo.out_color_space == JCS_RGB )
				{
					crColor = RGB( p[i][j*3], p[i][j*3+1], p[i][j*3+2] );
				}
			}
			SetPixel( hdc, x0+j, y0+i, crColor );
		}
	}
	return TRUE;
}

bool PngJpegImage::getParams( int *w, int *h, int *bpp, int *ch ) const
{
	if( w ) *w = width;
	if( h ) *h = height;
	if( bpp ) *bpp = bit_depth;
	if( ch ) *ch = channels;
	//if( ctype ) *ctype = color_type;
	return TRUE;
}

bool PngJpegImage::getFileName( char *out, size_t cchMax ) const
{
	if( !out || (cchMax < 1) ) return FALSE;
	strncpy( out, m_filename, cchMax );
	return TRUE;
}

void PngJpegImage::initnull()
{
	number = 6;
	memset( b_header, 0, sizeof(b_header) );
	m_filename[0] = 0;
	png_ptr = NULL;
	info_ptr = NULL;
	width = height = bit_depth = rowbytes = channels = 0;
	// image data
	row_pointers = NULL;
	//
	image_is_png = image_is_jpeg = 0;
}

void PngJpegImage::freeself()
{
	if( image_is_png )
	{
		int i = 0;
		if( row_pointers )
		{
			for( i=0; i<height; i++ )
			{
				if( row_pointers[i] )
				{
					png_free( png_ptr, row_pointers[i] );
				}
			}
			png_free( png_ptr, row_pointers );
#ifdef _DEBUG_LOG
			log_error( LOG_DEBUG, "png.freeself() freed row_pointers\n" );
#endif
		}
		if( png_ptr && info_ptr )
		{
			png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
#ifdef _DEBUG_LOG
			log_error( LOG_DEBUG, "png.freeself() freed read_struct 0x%p, 0x%p\n", png_ptr, info_ptr );
#endif
		}
	} // image_is_png
	else if( image_is_jpeg )
	{
		int i = 0;
		if( row_pointers )
		{
			for( i=0; i<height; i++ )
			{
				if( row_pointers[i] ) free( row_pointers[i] );
			}
			free( row_pointers );
#ifdef _DEBUG_LOG
			log_error( LOG_DEBUG, "jpeg.freeself() freed row_pointers\n" );
#endif
		}
	}
	initnull();
}

