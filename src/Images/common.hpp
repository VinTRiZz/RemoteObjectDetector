#ifndef COMMON_H
#define COMMON_H

// OpenCV header
#include <opencv2/opencv.hpp>

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

// Data containers
#include <string>
#include <list>
#include <map>
#include <vector>

// STD algorithms
#include <algorithm>

// Directory processing
#include <dirent.h>

// Compare speed increasing
#include <thread>
#include <mutex>

// C++ 14 problems
#if (__cplusplus > 201402L)
#include <filesystem>
namespace stdfs = std::filesystem;
#else
#include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#endif // C++ 14 problems

#endif // COMMON_H
