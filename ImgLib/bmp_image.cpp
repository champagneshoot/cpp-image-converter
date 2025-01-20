#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader
{
    char sign1 = 'B'; 
    char sign2 = 'M'; 
    uint32_t file_size = 0; 
    uint32_t reserved_space = 0; 
    uint32_t indention = 0; // 
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader 
{
    uint32_t header_size = 40; 
    int32_t width = 0; 
    int32_t height = 0;
    uint16_t plane = 1;
    uint16_t bit_per_pixel = 24; 
    uint32_t compression_type = 0;
    uint32_t bytes_at_data = 0; 
    int32_t horizontal_pixel_per_meter = 11811; 
    int32_t vertical_pixel_per_meter = 11811; 
    uint32_t colors_in_use = 0; 
    uint32_t colors = 0x1000000; 
}
PACKED_STRUCT_END

static int GetBMPStride(int w) 
{
    return 4 * ((w * 3 + 3) / 4);
}

bool SaveBMP(const Path& file, const Image& image) 
{
    ofstream out(file, ios::binary);
  
    int width = image.GetWidth();
    int height = image.GetHeight();
  
    const int bmp_stride = GetBMPStride(width);
  
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
  
    file_header.indention = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header.file_size = file_header.indention + bmp_stride * height;
  
    info_header.width = width;
    info_header.height = height;
    info_header.bytes_at_data = bmp_stride * height;
  
    out.write(reinterpret_cast<const char*>(&file_header), sizeof(BitmapFileHeader));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(BitmapInfoHeader));
  
    std::vector<char> buff(bmp_stride);
    for (int i = height - 1; i >= 0; --i) 
    {
        const Color* line = image.GetLine(i);
        for (int j = 0; j < width; ++j)
        {
            buff[3 * j] = static_cast<char>(line[j].b);
            buff[1 + 3 * j] = static_cast<char>(line[j].g);
            buff[2 + 3 * j] = static_cast<char>(line[j].r);
        }
        std::fill(buff.begin() + 3 * width, buff.end(), 0);
        out.write(buff.data(), bmp_stride);
    }
    return out.good();
}

 
Image LoadBMP(const Path& file) 
{
    
    ifstream input(file, ios::binary);
 
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
 
    input.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    input.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));
 
    int width = info_header.width;
    int height = info_header.height;
    auto color = Color::Black();
 
    const int bmp_stride = GetBMPStride(width);
    
    std::vector<char> buff(bmp_stride);
    Image result(width, height, color);
 
    for (int i = height - 1; i >= 0; --i) {
        
        input.read(buff.data(), bmp_stride);
        Color* line = result.GetLine(i);
        
        for (int j = 0; j != width; ++j) {
            line[j].b = static_cast<byte>(buff[j * 3]);
            line[j].g = static_cast<byte>(buff[1 + j * 3]);
            line[j].r = static_cast<byte>(buff[2 + j * 3]);
        }
    }
 
    return result;
}
}
