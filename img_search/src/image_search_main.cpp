#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <cstdint>
#include <cstdlib>
#include <string>
#include <iostream>
#include <atomic>
#include <vector>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ocv_threshold_functions.h"
#include "get_directory_listing.h"
#include "file_ops.h"

#include "image_tile.h"
#include "get_tiled_images.h"

//-----------------------------------------------------------------------------
inline double get_distance(cv::Point p1, cv::Point p2)
{
    
    cv::Point2d tmp_p = p1 - p2;
    double d = tmp_p.ddot(tmp_p);
    
    return sqrt(d);
}

inline double get_distance(cv::Rect r1, cv::Rect r2)
{
    return get_distance(r1.tl(), r2.tl());
}

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

    //std::string img_filename = std::string(argv[1]);
    std::string image_directory = std::string(argv[1]);
    //image_directory = path_check(image_directory);

    //std::vector<std::string> img_filter = { "png", "jpg", "tif", "tiff" };
    //std::vector<std::string> img_listing = get_directory_listing(image_directory, img_filter);


    //img = cv::imread(image_directory + img_listing[0], cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_GRAYSCALE);
    //img.convertTo(img, CV_32FC1);


    //std::string pix_size = std::to_string(pow(2, 50));
    //auto r1 = setenv("OPENCV_IO_MAX_IMAGE_PIXELS", pix_size.c_str(), 1);
    //auto r1 = _putenv_s("CV_IO_MAX_IMAGE_PIXELS", pix_size.c_str());
    //std::cout << cv::CV_IO_MAX_IMAGE_PIXELS << std::endl;
    
    // read in the image
    //img = cv::imread(img_filename, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_GRAYSCALE);
    //img.convertTo(img, CV_32FC1);

    // threshold image
    cv::Mat img_threshold1;
    int threshold_val;
    double energy_value = 0.8;


    std::vector<image_tile> it;
    //uint64_t tile_h = 2000, tile_w = 2000;
    //uint64_t overlap_x = 200, overlap_y = 200;
    //generate_tiles(img, tile_h, tile_w, overlap_x, overlap_y, it);

    load_image_tiles(image_directory, it);

    //cv::Rect test_rect = cv::Rect(2000, 2000, 2000, 2000);
    cv::Mat test_roi = it[10].img;

    //img.release();
    //img_threshold1 = test_roi.clone();

    //cv::Rect search_rect = cv::Rect(2065, 2047, 145, 55);
    uint64_t cell_x, cell_y, cell_w, cell_h;

    //cv::Rect search_rect2 = cv::Rect(309, 529, 35, 56);

    uint32_t cell_type = 0;

    switch(cell_type)
    {
    case 0:
        cell_x = 223;
        cell_y = 64;
        cell_w = 284;
        cell_h = 116;
        break;

    case 1:
        cell_x = 1297;
        cell_y = 67;
        cell_w = 73;
        cell_h = 116;
        break;


    default:
        cell_x = 223;
        cell_y = 64;
        cell_w = 284;
        cell_h = 116;
        break;
    }

    cv::Rect search_rect1(cell_x, cell_y, cell_w, cell_h);        // for [11] - x=265, y=247, for [71] x=256, y=167


    advanced_threshold(test_roi, img_threshold1, 110.0f, -1.0f, 1.0f);
    cv::Mat SE3_rect = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::morphologyEx(img_threshold1, img_threshold1, cv::MORPH_CLOSE, SE3_rect);

    //energy_threshold(img, img_threshold1, energy_value, threshold_val, 1);
    //energy_threshold(img, img_threshold2, energy_value, threshold_val, 2);



    cv::Mat img_roi = img_threshold1(search_rect1);
    //cv::Mat img_roi2 = img_threshold1(search_rect2);

    cv::Mat match_res1, img_roi1;

    //cv::filter2D(img_threshold1, match_res1, CV_32FC1, img_roi1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    //cv::filter2D(img_threshold1, match_res2, CV_32FC1, img_roi2, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    //cv::Mat match_threshold1, match_threshold2;
    //advanced_threshold(match_res1, match_threshold1, 0.82f * search_rect1.area(), 0.0f, 1.0f);
    //advanced_threshold(match_res2, match_threshold2, 0.82f * search_rect2.area(), 0.0f, 1.0f);

    bp = 1;

    uint64_t count = 0;
    bool match_found = true;

    cv::Point match_point;
    double max_val;
    double match_thresh = 0.90 * search_rect1.area();
    uint64_t x, y, w, h;
    uint64_t min_x, max_x, min_kx, max_kx;
    uint64_t min_y, max_y, min_ky, max_ky;

    cv::Rect match_rect(0, 0, cell_w, cell_h);

    cv::Mat clear_mat = cv::Mat::zeros(cell_h>>2, cell_w>>2, CV_32FC1);


    for (idx = 0; idx < 4; ++idx)
    {
        match_found = true;

        switch (idx)
        {
        case 0:
            img_roi1 = img_roi.clone();
            break;

        case 1:
            cv::flip(img_roi, img_roi1, 0);
            break;

        case 2:
            cv::flip(img_roi, img_roi1, 1);

            break;

        case 3:
            cv::flip(img_roi, img_roi1, -1);

            break;
        }


        cv::filter2D(img_threshold1, match_res1, CV_32FC1, img_roi1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

        while (match_found)
        {
            cv::minMaxLoc(match_res1, NULL, &max_val, NULL, &match_point);

            if (max_val >= match_thresh)
            {

                // calculate the bounds of where copying the clear window happens
                min_x = std::max((int64_t)0, (int64_t)(match_point.x - (cell_w >> 1)));
                max_x = std::min((int64_t)img_threshold1.cols, (int64_t)(match_point.x + (cell_w >> 1)));

                min_y = std::max((int64_t)0, (int64_t)(match_point.y - (cell_h >> 1)));
                max_y = std::min((int64_t)img_threshold1.rows, (int64_t)(match_point.y + (cell_h >> 1)));

                min_kx = std::max((int64_t)0, (int64_t)(match_point.x - (clear_mat.cols >> 1)));
                max_kx = std::min((int64_t)img_threshold1.cols, (int64_t)(match_point.x + (clear_mat.cols >> 1)));

                min_ky = std::max((int64_t)0, (int64_t)(match_point.y - (clear_mat.rows >> 1)));
                max_ky = std::min((int64_t)img_threshold1.rows, (int64_t)(match_point.y + (clear_mat.rows >> 1)));

                clear_mat(cv::Range(0, max_ky - min_ky), cv::Range(0, max_kx - min_kx)).copyTo(match_res1(cv::Range(min_ky, max_ky), cv::Range(min_kx, max_kx)));

                // draw the bounding box
                match_rect = cv::Rect(min_x, min_y, max_x - min_x, max_y - min_y);
                cv::rectangle(test_roi, match_rect, cv::Scalar(255), 2, cv::LINE_8);


            }
            else
            {
                match_found = false;
            }

        }

        cv::imshow(cv_window, test_roi / 255.0);
        cv::waitKey(0);
    }
    bp = 2;

    std::cout << "Press Enter to close..." << std::endl;

    std::cin.ignore();

    cv::destroyAllWindows();

    return 0;
}
