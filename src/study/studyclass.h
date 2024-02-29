#ifndef STUDYCLASS_H
#define STUDYCLASS_H

#include <memory>

#include <vector>

namespace Study
{

class ObjectNeuralNet
{
public:
    // Args: path to configuration file (saved neural net) and switch to load or not from it
    ObjectNeuralNet(const std::string& configFile, bool loadFromFile = false);
    ~ObjectNeuralNet();

    // Study on files in directories (one or more), start and stop
    void startStudy(const std::vector<std::string>& studyDirs);
    void stopStudy();

    // Set name for object net must detect
    void setObject(const std::string& name);

    // Ask if image by path contains object neural net study to find to
    bool isObject(const std::string& imagePath);

private:
    struct ObjectNeuralNetPrivate;
    std::shared_ptr<ObjectNeuralNetPrivate> d;
};

}

#endif // STUDYCLASS_H
