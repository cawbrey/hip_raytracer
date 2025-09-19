#ifndef RTCUDA_BITMAPIMAGE_H
#define RTCUDA_BITMAPIMAGE_H

#include <png.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Image {
class Bitmap {
   public:
    uint width;
    uint height;
    Color* data;

    __device__ __host__ Bitmap( Bitmap& img )
        : width( img.width ), height( img.height ), data( new Color[width * height] ) {
        for ( int i = 0; i < width * height; i++ ) {
            data[i] = img.data[i];
        }
    }

    __device__ __host__ Bitmap( std::vector<Color>& imgBuffer, uint width, uint height )
        : width( width ), height( height ), data( new Color[width * height] ) {
        for ( int i = 0; i < width * height; i++ ) {
            data[i] = imgBuffer[i];
        }
    }

    __device__ __host__ Bitmap( Color* imgBuffer, uint width, uint height )
        : width( width ), height( height ), data( new Color[width * height] ) {
        for ( int i = 0; i < width * height; i++ ) {
            data[i] = imgBuffer[i];
        }
    }

    __device__ __host__ Color getPixel( uint x, uint y ) const {
        return data[y * width + x];
    }
};

void readPPM( const std::string& ppmFile,
              std::vector<Color>& imgBuffer,
              uint* width,
              uint* height ) {
    std::ifstream file( ppmFile );

    if ( !file.is_open() ) {
        std::cerr << "Error: Could not open file " << ppmFile << std::endl;
        return;
    }

    std::string magicNumber;
    file >> magicNumber;

    if ( magicNumber != "P3" ) {
        std::cerr << "Error: Invalid magic number " << magicNumber << std::endl;
        return;
    }

    file >> *width >> *height;

    float maxColor;
    file >> maxColor;

    imgBuffer.clear();

    for ( int i = 0; i < ( *width ) * ( *height ); i++ ) {
        float r, g, b;
        file >> r >> g >> b;
        imgBuffer.emplace_back( r / maxColor, g / maxColor, b / maxColor );
    }

    file.close();
}

void readPNG( const std::string& pngFile,
              std::vector<Color>& imgBuffer,
              uint* width,
              uint* height ) {
    FILE* fp = fopen( pngFile.c_str(), "rb" );
    if ( !fp )
        return;

    png_structp png = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
    png_infop info = png_create_info_struct( png );
    if ( setjmp( png_jmpbuf( png ) ) )
        return;

    png_init_io( png, fp );
    png_read_info( png, info );

    *width = (uint)png_get_image_width( png, info );
    *height = png_get_image_height( png, info );
    png_byte color_type = png_get_color_type( png, info );
    png_byte bit_depth = png_get_bit_depth( png, info );

    if ( bit_depth == 16 )
        png_set_strip_16( png );

    if ( color_type == PNG_COLOR_TYPE_PALETTE )
        png_set_palette_to_rgb( png );

    if ( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
        png_set_expand_gray_1_2_4_to_8( png );

    if ( png_get_valid( png, info, PNG_INFO_tRNS ) )
        png_set_tRNS_to_alpha( png );

    if ( color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
         color_type == PNG_COLOR_TYPE_PALETTE )
        png_set_filler( png, 0xFF, PNG_FILLER_AFTER );

    if ( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        png_set_gray_to_rgb( png );

    png_read_update_info( png, info );

    auto* row_pointers = (png_bytep*)malloc( sizeof( png_bytep ) * *height );
    for ( int y = 0; y < *height; y++ )
        row_pointers[y] = (png_byte*)malloc( png_get_rowbytes( png, info ) );

    png_read_image( png, row_pointers );

    imgBuffer.clear();

    for ( int y = 0; y < *height; y++ ) {
        png_bytep row = row_pointers[y];
        for ( int x = 0; x < *width; x++ ) {
            png_bytep px = &( row[x * 4] );
            imgBuffer.emplace_back( px[0] / 255.0, px[1] / 255.0, px[2] / 255.0 );
        }
    }

    for ( int y = 0; y < *height; y++ )
        free( row_pointers[y] );

    free( row_pointers );
    fclose( fp );
    png_destroy_read_struct( &png, &info, nullptr );
}

void writePPM( const Color* imgBuffer,
               const uint width,
               const uint height,
               const std::string& fileName ) {
    std::ofstream file( fileName );

    if ( !file.is_open() ) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
    }

    file << "P3 " << width << " " << height << " " << USHRT_MAX << std::endl;

    for ( int i = 0; i < width * height; i++ ) {
        file << (int)( imgBuffer[i].x * USHRT_MAX ) << ' ';
        file << (int)( imgBuffer[i].y * USHRT_MAX ) << ' ';
        file << (int)( imgBuffer[i].z * USHRT_MAX ) << ' ';
    }

    file.close();
}

void writePNG( const Color* imgBuffer,
               const uint width,
               const uint height,
               const std::string& fileName ) {
    FILE* fp = fopen( fileName.c_str(), "wb" );
    if ( !fp )
        return;

    png_structp png = png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
    png_infop info = png_create_info_struct( png );
    if ( setjmp( png_jmpbuf( png ) ) )
        return;

    png_init_io( png, fp );

    png_set_IHDR( png, info, width, height, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

    png_write_info( png, info );
    png_set_swap( png );

    std::vector<png_uint_16> temp_image;
    temp_image.reserve( height * width * 3 );

    for ( uint i = 0; i < height * width; i++ ) {
        Color color = imgBuffer[i];
        temp_image.push_back( (png_uint_16)( color.x * USHRT_MAX ) );
        temp_image.push_back( (png_uint_16)( color.y * USHRT_MAX ) );
        temp_image.push_back( (png_uint_16)( color.z * USHRT_MAX ) );
    }

    std::vector<png_uint_16p> rows( height );

    for ( int y = 0; y < height; y++ ) {
        rows[y] = &temp_image[y * width * 3];
    }

    png_write_image( png, (png_bytepp)rows.data() );
    png_write_end( png, nullptr );

    fclose( fp );
    png_destroy_write_struct( &png, &info );
}

Bitmap* readPPM( const std::string& ppmFile ) {
    std::vector<Color> imgBuffer;
    uint width, height;
    readPPM( ppmFile, imgBuffer, &width, &height );
    return new Bitmap{ imgBuffer, width, height };
}

Bitmap* readPNG( const std::string& pngFile ) {
    std::vector<Color> imgBuffer;
    uint width, height;
    readPNG( pngFile, imgBuffer, &width, &height );
    return new Bitmap{ imgBuffer, width, height };
}

void writePPM( const Bitmap& image, const std::string& fileName ) {
    writePPM( image.data, image.width, image.height, fileName );
}

void writePNG( const Bitmap& image, const std::string& fileName ) {
    writePNG( image.data, image.width, image.height, fileName );
}
}  // namespace Image

#endif  // RTCUDA_BITMAPIMAGE_H