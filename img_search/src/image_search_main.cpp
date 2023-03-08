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
typedef struct image_tile
{
    cv::Mat img;
    cv::Rect r;

    image_tile(cv::Mat img_, cv::Rect r_) : img(img_), r(r_) {}

} image_tile;

//-----------------------------------------------------------------------------
void generate_tiles(cv::Mat& src, uint64_t tile_h, uint64_t tile_w, uint64_t overlap_x, uint64_t overlap_y, std::vector<image_tile>& dst)
{
    uint64_t row, col;
    uint64_t t_w, t_h;

    cv::Rect r;
    cv::Mat img;

    uint64_t img_h = src.rows;
    uint64_t img_w = src.cols;

    dst.clear();

    row = 0;
    while (row < img_h)
    {
        col = 0;
        while (col < img_w)
        {
            
            t_w = (col + tile_w >= img_w) ? img_w - col : tile_w;
            t_h = (row + tile_h >= img_h) ? img_h - row : tile_h;

            r = cv::Rect(col, row, t_w, t_h);
            img = src(r).clone();

            image_tile tmp(img, r);

            dst.push_back(tmp);

            col += (tile_w - overlap_x);
        }

        row += (tile_h - overlap_y);
    }

}   // end of generate_tiles

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


    std::vector<image_tile> it;
    uint64_t tile_h = 2000, tile_w = 2000;
    uint64_t overlap_x = 200, overlap_y = 200;
    generate_tiles(img, tile_h, tile_w, overlap_x, overlap_y, it);


    cv::Rect test_rect = cv::Rect(2000, 2000, 2000, 2000);
    cv::Mat test_roi = it[11].img;

    img.release();
    //img_threshold1 = test_roi.clone();

    advanced_threshold(test_roi, img_threshold1, 110.0f, -1.0f, 1.0f);
    cv::Mat SE3_rect = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::morphologyEx(img_threshold1, img_threshold1, cv::MORPH_CLOSE, SE3_rect);

    //energy_threshold(img, img_threshold1, energy_value, threshold_val, 1);
    //energy_threshold(img, img_threshold2, energy_value, threshold_val, 2);

    //cv::Rect search_rect = cv::Rect(2065, 2047, 145, 55);
    cv::Rect search_rect1(265, 247, 145, 55);
    cv::Rect search_rect2 = cv::Rect(309, 529, 35, 56);

    cv::Mat img_roi1 = img_threshold1(search_rect1);
    cv::Mat img_roi2 = img_threshold1(search_rect2);

    cv::Mat match_res1, match_res2;

    cv::filter2D(img_threshold1, match_res1, CV_32FC1, img_roi1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    cv::filter2D(img_threshold1, match_res2, CV_32FC1, img_roi2, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    cv::Mat match_threshold1, match_threshold2;
    advanced_threshold(match_res1, match_threshold1, 0.82f * search_rect1.area(), 0.0f, 1.0f);
    advanced_threshold(match_res2, match_threshold2, 0.82f * search_rect2.area(), 0.0f, 1.0f);

    bp = 1;

    uint64_t count = 0;
    bool match_found = true;

    cv::Point match_point;
    double max_val;
    double match_thresh = 0.82 * search_rect2.area();
    uint64_t x, y, w, h;
    uint64_t min_x, max_x, min_kx, max_kx;
    uint64_t min_y, max_y, min_ky, max_ky;

    cv::Rect match_rect(0, 0, search_rect1.width >> 2, search_rect1.height >> 2);

    while (match_found)
    {
        cv::minMaxLoc(match_res1, NULL, &max_val, NULL, &match_point);

        if (max_val >= match_thresh)
        {

            // calculate the bounds of where copying the filter kernel happens
            //min_x = std::max((int64_t)0, (int64_t)(x)-(blur_cols >> 1));
            //max_x = std::min((int64_t)N, (int64_t)(x)+(blur_cols >> 1) + 1);

            //min_y = std::max((int64_t)0, (int64_t)(y)-(blur_rows >> 1));
            //max_y = std::min((int64_t)N, (int64_t)(y)+(blur_rows >> 1) + 1);


            min_x = std::max((int64_t)0, (int64_t)(match_point.x - (match_rect.width >> 1)));
            max_x = std::min((int64_t), (int64_t));

            x = match_point.x - (match_rect.width >> 1);
            x = x < 0 ? 0 : x;

            y = match_point.y - (match_rect.height >> 1);
            y = (y < 0) ? 0 : y;

            match_rect.x = x;
            match_rect.y = y;
            match_res1(match_rect) = cv::Mat::zeros(match_rect.height, match_rect.width, CV_32FC1);

            // draw the bounding box
            cv::rectangle(test_roi, match_rect, cv::Scalar(255), 2, cv::LINE_8);

        }
        else
        {
            match_found = false;
        }

    }


    //match_res1(cv::Rect(136, 142, 10, 10)) = cv::Mat::zeros(10, 10, CV_32FC1);



    cv::destroyAllWindows();

    //std::cout << "Press Enter to close..." << std::endl;

    //std::cin.ignore();
    
    return 0;
}
