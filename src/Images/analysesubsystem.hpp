#ifndef ANALYSESUBSYSTEM_HPP
#define ANALYSESUBSYSTEM_HPP

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>
#include <list>

enum class BGR_ERASE_METHOD
{
    NO_METHOD,
    METHOD_KNN,
    METHOD_MOG2,
};

class AnalyseSubsystem
{
public:
    AnalyseSubsystem();
    ~AnalyseSubsystem();

    void setBackgroundEraseMethod(int historySize = 0, BGR_ERASE_METHOD method = BGR_ERASE_METHOD::METHOD_KNN);
    void setCameraFile(const std::string& cameraDevicePath);
    std::string getCameraFile() const;

    bool addTemplate(const std::string& path);
    bool renameTemplate(const std::string& oldName, const std::string& newName);
    void removeTemplate(const std::string& typeName);

    void setInitTime(uint64_t newTime);
    uint64_t getInitTime() const;

    void enableHardAnalyse(bool isEnabled = true);

    void init();
    bool isReady() const;

    bool startAnalyse(std::list<std::pair<std::string, double>>& analyseResultBuffer);
    void pollAnalyse();

    std::string errorText() const;

private:
    void studyBackground();

    struct AnalyseSubsystemPrivate;
    std::unique_ptr<AnalyseSubsystemPrivate> d;
};

#endif // ANALYSESUBSYSTEM_HPP
