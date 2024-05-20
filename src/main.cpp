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

//using namespace cv;

//int main()
//{
//    // Load the two images
//    cv::Mat image = cv::imread("target.jpg");
//    cv::Mat themeImage = cv::imread("theme.jpg");
//    cv::Mat result = cv::Mat(image.size(), image.type(), cv::Scalar(0, 0, 0));;

//    cv::bitwise_xor(image, themeImage, result);

//    // Display the modified image
//    imwrite("result.jpg", result);
//    return 0;
//}
