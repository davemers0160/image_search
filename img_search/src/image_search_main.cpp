#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <atomic>
#include <vector>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ocv_threshold_functions.h"


//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx, jdx;

    char key = 0;

    uint32_t img_w, img_h, img_c;
    double threshold = 30.0;

    std::string lib_filename;

    if (argc < 2)
    {
        std::cout << "Enter the location of the image..." << std::endl;
        std::cin.ignore();
    }


    for (idx = 0; idx < argc; ++idx)
    {
        std::cout << std::string(argv[idx]) << std::endl;
    }

    std::string cv_window = "Image";
    cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    int bp = 0;

    cv::Mat img;

    std::string img_filename = std::string(argv[1]);

/*
    // load in the library
#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#if defined(_DEBUG)
    lib_filename = "../../blob_detector/build/Debug/blob_detector.dll";
#else
    lib_filename = "../../blob_detector/build/Release/blob_detector.dll";
#endif

    HINSTANCE blob_detector_lib = LoadLibrary(lib_filename.c_str());

    if (blob_detector_lib == NULL)
    {
        throw std::runtime_error("error loading library");
    }

    lib_blob_detector blob_detector = (lib_blob_detector)GetProcAddress(blob_detector_lib, "blob_detector");

#else
    lib_filename = "../../fusion_lib/build/libms_fuser.so";
    void* blob_detector_lib = dlopen(lib_filename.c_str(), RTLD_NOW);

    if (blob_detector_lib == NULL)
    {
        throw std::runtime_error("error loading blob_detector_lib library");
    }

    lib_blob_detector blob_detector = (lib_blob_detector)dlsym(blob_detector_lib, "blob_detector");

#endif
*/
    //std::string pix_size = std::to_string(pow(2, 50));
    //auto r1 = setenv("OPENCV_IO_MAX_IMAGE_PIXELS", pix_size.c_str(), 1);
    //auto r1 = _putenv_s("CV_IO_MAX_IMAGE_PIXELS", pix_size.c_str());


    //std::cout << cv::CV_IO_MAX_IMAGE_PIXELS << std::endl;
    // read in the image
    img = cv::imread(img_filename, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_GRAYSCALE);
    img.convertTo(img, CV_32FC1);

    // threshold image
    cv::Mat img_threshold1, img_threshold2;
    int threshold_val;
    double energy_value = 0.8;

    cv::Rect test_rect = cv::Rect(2000, 2000, 2000, 2000);
    cv::Mat test_roi = img(test_rect);

    img.release();
    //img_threshold1 = test_roi.clone();

    advanced_threshold(test_roi, img_threshold1, 110.0f, -1.0f, 1.0f);
    cv::Mat SE3_rect = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::morphologyEx(img_threshold1, img_threshold1, cv::MORPH_CLOSE, SE3_rect);

    //energy_threshold(img, img_threshold1, energy_value, threshold_val, 1);
    //energy_threshold(img, img_threshold2, energy_value, threshold_val, 2);


    //cv::Rect search_rect = cv::Rect(2065, 2047, 145, 55);
    cv::Rect search_rect1 = cv::Rect(65, 47, 145, 55);
    cv::Rect search_rect2 = cv::Rect(109, 329, 35, 56);

    cv::Mat img_roi1 = img_threshold1(search_rect1);
    cv::Mat img_roi2 = img_threshold1(search_rect2);

    cv::Mat match_res1, match_res2;

    cv::filter2D(img_threshold1, match_res1, CV_32FC1, img_roi1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    cv::filter2D(img_threshold1, match_res2, CV_32FC1, img_roi2, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    cv::Mat match_threshold1, match_threshold2;
    advanced_threshold(match_res1, match_threshold1, 6500.0f, 0.0f, 1.0f);
    advanced_threshold(match_res2, match_threshold2, 1800.0f, 0.0f, 1.0f);

    bp = 1;

    cv::destroyAllWindows();

    //std::cout << "Press Enter to close..." << std::endl;

    //std::cin.ignore();
    
    return 0;
}
