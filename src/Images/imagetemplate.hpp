#ifndef IMAGETEMPLATE_H
#define IMAGETEMPLATE_H

#include <memory>
#include <string>

namespace Analyse
{

class ImageTemplate
{
public:
    ImageTemplate();
    ImageTemplate(const ImageTemplate& imgTemplate);
    ~ImageTemplate();

    ImageTemplate& operator =(const ImageTemplate& imgTemplate);

    void setTemplate(const std::string& filepath);
    std::string getTemplate() const;

    void setName(const std::string& name);
    std::string getName() const;

    double match(const std::string& filepath);

private:
    void setupRotations();

    struct ImageTemplatePrivate;
    std::unique_ptr<ImageTemplatePrivate> d;
};

}
#endif // IMAGETEMPLATE_H
