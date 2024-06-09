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

#define DEBUG_MODE

#define DEBUG_EXIT          std::cout << "DEBUG EXIT FROM " << __FILE__ << " : " << __LINE__ << std::endl; exit(1)
#define DEBUG_TRY(FUNC)     try { FUNC; } catch (std::exception& ex) { std::cout << __FUNCTION__ << "() Exception got: " << ex.what() << std::endl; exit(2); };

#define DEBUG_BASEPATH "../DATA"

/*
chess
photo1
photo2
*/
#define DEBUG_DIRECTORY "chess"

/*
***************** CHESS
figures
black_knight_distort
black_knight_rotates
distorts

***************** Photos
object_1 (GUM or EAR)
object_2 (PEN or FLASH)
*/
#define DEBUG_TARGET_DIRECTORY "figures"

#endif // COMMON_H
