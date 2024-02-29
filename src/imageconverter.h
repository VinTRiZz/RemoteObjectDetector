#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

namespace ImageAnalyse
{
    
enum class ImageTypes
{
    UNKNOWN,
    PNG,
    JPG,
    JPEG,
    BMP
};

class Converter
{
public:
    Converter(ImageTypes type = ImageTypes::UNKNOWN);
    ~Converter();
    
    bool convert(std::shared_ptr<uint8_t> source, std::shared_ptr<uint8_t> destination);
    
private:
    struct ConverterPrivate;
    std::unique_ptr<ConverterPrivate> d;
};

}

#endif // IMAGE_CONVERTER_H
