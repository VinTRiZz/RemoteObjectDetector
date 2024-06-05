#include "typesholder.hpp"

TypesHolder::TypesHolder(cv::Ptr<cv::BackgroundSubtractor>& pBSub) :
    m_pBackSub { pBSub }
{

}

cv::Mat TypesHolder::loadImage(const std::string &filepath)
{
    // Read image
    cv::Mat imageRead = cv::imread(filepath);
    if (imageRead.empty())
    {
        std::cout << "Can't read template by path: %s" << filepath << std::endl;
        return {};
    }
//    cv::imwrite("image.png", imageRead);

    // Able morpho filter
    cv::Mat resultImage;
//    cv::adaptiveThreshold(imageRead, resultImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);
//    cv::imwrite("result_aThreshold.png", resultImage);

//    cv::Mat tempImage;
//    cv::morphologyEx(imageRead, resultImage, cv::MORPH_GRADIENT, tempImage);
//    cv::imwrite("result_morpho.png", resultImage);

//    cv::Laplacian(imageRead, resultImage, 5);
//    cv::imwrite("result_lapla.png", resultImage);

//    cv::Canny(imageRead, resultImage, 200, 100, 3);
//    cv::imwrite("result_canny.png", resultImage);

//    cv::Mat markersImage = cv::imread("shapedM.png");
//    cv::watershed(imageRead, markersImage);
//    cv::imwrite("result_markers.png", imageRead);
    return imageRead;
}


MomentsType TypesHolder::createHuMoments(const cv::Mat& image)
{
    cv::Mat grayImage;
    if ((image.channels() > 2) && (image.type() != CV_8UC1))
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    else
        grayImage = image;

    // Get Hu moments
    MomentsType momentsVect;
    cv::HuMoments(cv::moments(grayImage), momentsVect);
    return momentsVect;
}


std::vector<cv::Mat> TypesHolder::getObjects(const cv::Mat &targetImage)
{
    try {
        // Apply background erase
        cv::Mat objectsOnImage;
        m_pBackSub->apply(targetImage, objectsOnImage, 0);

        // Search for objects
        std::vector<ContourType> contours;
        addContours(objectsOnImage, contours);

        std::vector<cv::Mat> result (contours.size());
        size_t currentIndex = 0;

        auto targetCopy = targetImage;

        // Get objects array from contours
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);
            if (boundingRect.area() < (30 * 30)) continue;

            cv::rectangle(targetCopy, boundingRect, cv::Scalar(0, 0, 255), 4);

            auto& image = result[0];
            cv::Mat mask = cv::Mat::zeros(image.size(), image.type());
            cv::drawContours(mask, contour, -1, 255, 0);
            cv::bitwise_not(mask, mask);

            cv::Mat resultMat;
            image.copyTo(resultMat, mask);
            cv::imwrite("result.jpg", resultMat);

            std::cout << __FUNCTION__ << std::endl; exit(1);

            auto img = targetImage(boundingRect); // Crop image
            result[currentIndex] = img;
        }

        return result;

    } catch (std::exception& ex) {
        std::cout << "[getObjects()] Got OpenCV exception: %s" << ex.what() << std::endl;
    }
    return {};
}

void TypesHolder::addContours(const cv::Mat &img, std::vector<ContourType> &imageContours)
{
    cv::Mat grayImage;
    if ((img.channels() > 2) && (img.type() != CV_8UC1))
        cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);
    else
        grayImage = img;

    // Search into temporary array
    std::vector<ContourType> tempRes;
    cv::findContours(grayImage, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // If nothing found, return
    if (!tempRes.size()) return;

    std::sort(tempRes.begin(), tempRes.end(), [](auto& cont1, auto& cont2){ return (cv::boundingRect(cont1).area() > cv::boundingRect(cont2).area()); });
    imageContours.push_back(*tempRes.begin());
}

void TypesHolder::createRotationSet(std::vector<cv::Mat> &result, size_t &currentIndex)
{
    auto& baseImage = result[currentIndex];
    currentIndex++;
    auto* pImage = &result[currentIndex];

    // Rotate 90
    cv::transpose(baseImage, *pImage);
    cv::flip(*pImage, *pImage, 1);

    // Rotate -90
    currentIndex++;
    pImage = &result[currentIndex];
    cv::transpose(baseImage, *pImage);
    cv::flip(*pImage, *pImage, 0);

    // Rotate 180
    currentIndex++;
    cv::flip(baseImage, result[currentIndex], -1);

    currentIndex++;
}

std::vector<cv::Mat> TypesHolder::createRotations(const cv::Mat &image)
{
    std::vector<cv::Mat> result;

    // Setup first index and reserve memory for other
    result.push_back(image);
    result.resize(3 * 4);

    // Temp values
    size_t currentIndex = 0;

    // Rotations for normal image
    createRotationSet(result, currentIndex);

    // Mirror image vertical
    cv::flip(result[0], result[currentIndex], 0);

    // Rotations for mirrored vertical
    createRotationSet(result, currentIndex);

    // Mirror image horizontal
    cv::flip(result[0], result[currentIndex], 1);

    // Rotations for mirrored horizontal
    createRotationSet(result, currentIndex);

    // Remove empty rotation results if exist
    result.erase(std::remove_if(result.begin(), result.end(), [](auto& matr){ return matr.empty(); }), result.end());

    return result;
}

std::vector<MomentsType > TypesHolder::createHuMomentsSet(const cv::Mat &image)
{
    // Setup first index and reserve memory for other
    auto rotations = createRotations(image);
    std::vector<MomentsType > result(rotations.size());
    std::transform(rotations.begin(), rotations.end(), result.begin(), [this](auto& img){ return createHuMoments(img); });

    // Remove empty rotation results if exist
    std::remove_if(result.begin(), result.end(), [](auto& mom){ return mom.empty(); });

    return result;
}

std::list<cv::Mat> TypesHolder::createHistograms(const std::vector<cv::Mat> &imageRotations)
{
    std::list<cv::Mat> histograms;

    // Setup histogram configuration
    cv::Mat templateHist;               // Histogram
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    // Calculate histograms
    for (auto& templateRotation : imageRotations)
    {
        if (templateRotation.channels() != 0)
            continue;

        cv::calcHist(&templateRotation, 1, 0, cv::Mat(), templateHist, 1, &histSize, &histRange);

        // Get edges
        //        cv::Canny(templateRotation, templateHist, 100, 200);

        // Calc PGH
        //        cv::calcPGH(templateHist, templateHist, 1, 0, cv::Size(4,4));
        histograms.push_back(templateHist);
    }
    return histograms;
}

void TypesHolder::setupInfoHolder(TypeInfoHolder &imageIHolder)
{
    // Setup object things
    imageIHolder.image          = loadImage(imageIHolder.imagePath);

    auto objects                = getObjects(imageIHolder.image);
    if (objects.size())
    {
        imageIHolder.image = objects[0];
        std::cout << "Inserted object found: " << imageIHolder.typeName << std::endl;
        cv::imwrite(imageIHolder.typeName + ".png", imageIHolder.image);
    }

    imageIHolder.imageRotations = createRotations(imageIHolder.image);
    imageIHolder.histograms         = createHistograms(imageIHolder.imageRotations);
    imageIHolder.huMoments          = createHuMoments(imageIHolder.image);
    addContours(imageIHolder.image, imageIHolder.contours);
}

bool TypesHolder::addType(TypeInfoHolder &imageIHolder)
{
    // Check if type already exist
    auto exist = std::find_if(typeList.begin(), typeList.end(), [&imageIHolder](auto& tih){ return (tih.typeName == imageIHolder.typeName); });
    if (exist != typeList.end()) return false;

    // Add if not exist
    setupInfoHolder(imageIHolder);
    typeList.push_front(imageIHolder);
    return true;
}
