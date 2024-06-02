#include <iostream>
#include <signal.h>

#include "appsetup.hpp"

using namespace std;
int main(int argc, char* argv[])
{
    AppSetup::independentSetup();

    Components::MainApp app(argc, argv);

    AppSetup::setupApp(app);

    app.init();

    return app.exec();
}


//#include <opencv2/opencv.hpp>
//#include <list>
//#include <experimental/filesystem>
//namespace stdfs = std::experimental::filesystem;

//#include <iostream>
//#include <cstring>

//void loadObjects(const std::string &path, std::list<cv::Mat> &typeList)
//{
//    // Check if directory exist and it's directory
//    if (!stdfs::exists(path) || !stdfs::is_directory(path))
//    {
//        std::cout << "Error: " << std::strerror(errno) << std::endl;
//        return;
//    }

//    // Iterate in directory
//    std::vector<std::string> paths;
//    for (const auto& dirent : stdfs::directory_iterator(path))
//    {
//        // If a file, try to get image from it
//        if (stdfs::is_regular_file(dirent.path()))
//        {
//            std::string newTypeName = dirent.path().filename();
//            newTypeName.erase(newTypeName.find_last_of('.'), newTypeName.size() -1);

//            // Setup type as a name of file
//            paths.push_back(dirent.path().string());
//        }
//    }

//    std::sort(paths.begin(), paths.end());

//    for (auto& path : paths)
//    {
//        std::cout << "Loading file: " << path << std::endl;
//        auto img = cv::imread(path);
//        if (img.empty())
//        {
//            std::cout << "ERROR!" << std::endl;
//            continue;
//        }
//        typeList.push_back(img);
//    }
//}

//#include <thread>

//int main()
//{
//    std::cout << "Loading background studied" << std::endl;
//    std::list<cv::Mat> backgrounds;
//    loadObjects("./temp/TestPhotos/background", backgrounds);

//    cv::Ptr<cv::BackgroundSubtractor> pBS;
//    pBS = cv::createBackgroundSubtractorMOG2(0);
////    pBS = cv::createBackgroundSubtractorKNN(0);

//    cv::Mat result;
//    for (auto& backgrd : backgrounds) pBS->apply(backgrd, result);

//    std::cout << "Loading objects" << std::endl;
//    std::list<cv::Mat> objects;
//    loadObjects("./temp/TestPhotos/object_2", objects);

//    pBS->getBackgroundImage(result);
//    imwrite("bkgnd.jpg", result);

//    int readNo = 1;
//    for (auto& obj : objects)
//    {
//        pBS->apply(obj, result);
//        imwrite(std::string("result_") + std::to_string(readNo++) + ".jpg", result);
//    }

//    return 0;
//}
