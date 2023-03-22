#ifndef _GET_TILED_IMAGES_H_
#define _GET_TILED_IMAGES_H_

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

#include "get_directory_listing.h"
#include "file_ops.h"
#include "file_parser.h"

#include "image_tile.h"


void load_image_tiles(std::string folder, std::vector<image_tile>& img_tiles)
{
	uint64_t idx;
	cv::Mat img;
	cv::Rect r;

	folder = path_check(folder);

	//std::vector<std::string> img_filter = { ".png", ".jpg", ".tif", ".tiff" };
	//std::vector<std::string> img_listing = get_directory_listing(folder, img_filter);

	std::vector<std::string> txt_filter = { ".txt"};
	std::vector<std::string> txt_listing = get_directory_listing(folder, txt_filter);

	std::vector<std::vector<std::string>> params;
	parse_csv_file(folder + txt_listing[0], params);

	for (idx = 0; idx < params.size(); ++idx)
	{
		img = cv::imread(params[idx][0], cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_GRAYSCALE);
		img.convertTo(img, CV_32FC1);

		r = cv::Rect(std::stoi(params[idx][1]), std::stoi(params[idx][2]), std::stoi(params[idx][3]), std::stoi(params[idx][4]));

		img_tiles.push_back(image_tile(img, r));
	}


}

#endif	// _GET_TILED_IMAGES_H_
