#ifndef _IMAGE_TILE_CLASS_H_
#define _IMAGE_TILE_CLASS_H_

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

//-----------------------------------------------------------------------------
class image_tile
{
public:
    cv::Mat img;
    cv::Rect r;

    image_tile(cv::Mat img_, cv::Rect r_) : img(img_), r(r_) {}


    //-----------------------------------------------------------------------------
    inline double get_distance(cv::Point p1)
    {
        cv::Point2d tmp_p = r.tl() - p1;
        double d = tmp_p.ddot(tmp_p);

        return sqrt(d);
    }

    //-----------------------------------------------------------------------------
    inline double get_distance(cv::Rect r1)
    {
        return get_distance(r1.tl());
    }

private:

};

#endif	// _IMAGE_TILE_CLASS_H_
