#ifndef DETECTTRAFFICSIGN_H
#define DETECTTRAFFICSIGN_H
#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
class DetectTrafficSign 
{
    public:
        // Red color
        int R_HUE_MAX = 11;
        int R_HUE_MIN = 230;
        int R_SAT_MIN = 30;

        // The values for the blue colour.
        int B_HUE_MAX = 225;
        int B_HUE_MIN = 209;
        int B_SAT_MIN = 10;

        DetectTrafficSign();
        ~DetectTrafficSign();
        void detect(Mat Img);
        // Mat rgb_image; //= imread("asign.png", IMREAD_COLOR);
        // Mat hls_image;//(rgb_image.rows, rgb_image.cols, CV_8UC3);
        // Mat ihls_image;//(rgb_image.rows, rgb_image.cols, CV_8UC3);
        // Mat nhs_image;//(rgb_image.rows, rgb_image.cols, CV_8UC1);

    private: 
        void createTrackbars();
        void on_trackbar(int, void *);
        Mat blue_nhs(cv::Mat);
        float get_H(unsigned int, unsigned int, unsigned int);
        float get_S(unsigned int, unsigned int, unsigned int);
        float get_L(unsigned int, unsigned int, unsigned int);
        Mat cleanImageOps(Mat);
        Mat objElimination(Mat &);
        vector<Point> contourExtraction(Mat);
        Mat detectSign(vector<Point>, Mat&);

        Mat rgb_to_ihls(Mat);

        int H_MIN;// = B_HUE_MIN; //80;
        int H_MAX;// = B_HUE_MAX; // 150;
        int S_MIN;// = B_SAT_MIN; // 150;

};

#endif