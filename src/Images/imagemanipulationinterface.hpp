#ifndef IMAGEMANIPULATIONINTERFACE_H
#define IMAGEMANIPULATIONINTERFACE_H

#include <memory>
#include <list>
#include <string>

struct DetectedObject
{
    std::string name {"Unknown"};
    float percent {0};
};

struct ObjectType
{
    std::string name;
    std::string imagePath;
};

class ImageManipulationInterface
{
public:
    ImageManipulationInterface();
    ~ImageManipulationInterface();

    void init();
    bool isReady() const;

    void setInitTime(uint64_t newInitTime);
    uint64_t getInitTime() const;

    void setCamera(const std::string& cameraFile);
    std::string getCamera() const;

    bool startDetectObjects();
    void pollDetecting();
    std::list<DetectedObject> detectedObjects();

    std::string lastErrorText() const;

    void processTemplatesDirectory(const std::string& templatesDirectoryPath);
    bool addType(const ObjectType& _ot);
    void removeType(const std::string& _otName);
    bool renameType(const std::string& _otOldName, const std::string& _otNewName);
    std::list<ObjectType> availableTypes() const;

private:
    struct ImageManipulationInterfacePrivate;
    std::unique_ptr<ImageManipulationInterfacePrivate> d;
};

#endif // IMAGEMANIPULATIONINTERFACE_H
