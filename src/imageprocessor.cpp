#include "imageprocessor.h"


// Terminal data output
#include <iostream>

#define PRINT_LOG(what) std::cout << "[ " << __FUNCTION__ << " ] " << what << std::endl
#define PRINT_SUC(what) std::cout << "[\033[32m OK! \033[0m] " << what << std::endl
#define PRINT_ERR(what) std::cout << "[\033[31mERROR\033[0m] [ " << __PRETTY_FUNCTION__ << " ] " << what << std::endl

struct ImageAnalyse::Processor::AnalysatorPrivate
{
    std::string m_configDirPath;


    // Init in case of setting config dir path
    void init()
    {
        PRINT_SUC("Init complete");
    }


    // Deinit in case of setting config dir path
    void deinit()
    {
        PRINT_SUC("Deinit complete");
    }
};

ImageAnalyse::Processor::Processor() :
    d {new AnalysatorPrivate()}
{

}

ImageAnalyse::Processor::~Processor()
{

}

void ImageAnalyse::Processor::setConfigDirPath(const std::string& path)
{
    d->m_configDirPath = path;
}
