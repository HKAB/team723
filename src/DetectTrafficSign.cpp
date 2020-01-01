#include "DetectTrafficSign.h"
#include <chrono>

DetectTrafficSign::DetectTrafficSign()
{
    int H_MIN = B_HUE_MIN; //80;
    int H_MAX = B_HUE_MAX; // 150;
    int S_MIN = B_SAT_MIN; // 150;
    // createTrackbars();
}

DetectTrafficSign::~DetectTrafficSign(){ }

void DetectTrafficSign::on_trackbar(int, void *)
{
    // B_HUE_MIN = H_MIN;
    // B_HUE_MAX = H_MAX;
    // B_SAT_MIN = S_MIN;

    // // R_HUE_MIN = H_MIN;
    // // R_HUE_MAX = H_MAX;
    // // R_SAT_MIN = S_MIN;

    // for (int i = 0; i < rgb_image.rows; ++i)
    // {
    //     const uchar* rgb_data = rgb_image.ptr<uchar> (i);
    //     uchar* ihls_data = ihls_image.ptr<uchar> (i);

    //     for (int j = 0; j < rgb_image.cols; ++j)
    //     {
    //         unsigned int b = *rgb_data++;
    //         unsigned int g = *rgb_data++;
    //         unsigned int r = *rgb_data++;
    //         *ihls_data++ = (uchar) get_H(r, g, b);
    //         *ihls_data++ = (uchar) get_L(r, g, b);
    //         *ihls_data++ = (uchar) get_S(r, g, b);
    //     }
    // }

    // nhs_image = blue_nhs(ihls_image);
    // // nhs_image = red_nhs(ihls_image);
    // cv::imshow("nhs_image", nhs_image);
}

float DetectTrafficSign::get_S(unsigned int r, unsigned int g, unsigned int b)
{
    float s;
    s = std::max(r, std::max(g, b)) - std::min(r, std::min(g, b));
    return s;
}

float DetectTrafficSign::get_L(unsigned int r, unsigned int g, unsigned int b)
{
    return (0.212f * r) + (0.715f * g) + (0.072f * b);
}

float DetectTrafficSign::get_H(unsigned int r, unsigned int g, unsigned int b)
{
    float H;
    float num = r - (g * 0.5) - (b * 0.5);
    float deno = (r * r) + (g * g) + (b * b) - (r * g) - (r * b) - (g * b);

    float t = num/sqrtf(deno);
    H = acos(t)*180/M_PI;

    if (b > g)
    {
        H = 360 - H;
    }
    return H;
}

// cv::Mat DetectTrafficSign::red_nhs(cv::Mat ihls_input)
// {
//     cv::Mat nhs_image(ihls_input.rows, ihls_input.cols, CV_8UC1);
//     // cv::imshow("ihls_input", ihls_input);
//     // cv::waitKey(0);
//     for (int i = 0; i < ihls_input.rows; ++i)
//     {
//         const uchar* ihsl_data = ihls_input.ptr<uchar> (i);
//         uchar* nhs_data = nhs_image.ptr<uchar> (i);

//         for (int j = 0; j < ihls_input.cols; ++j)
//         {
//             unsigned int h = *ihsl_data++;
//             unsigned int l = *ihsl_data++;
//             unsigned int s = *ihsl_data++;
            
//             if (h > R_HUE_MIN || h < R_HUE_MAX && s > R_SAT_MIN)
//                 *nhs_data++ = 255;
//             else
//                 *nhs_data++ = 0;
//         }
//     }

//     return nhs_image;
// }

cv::Mat DetectTrafficSign::blue_nhs(cv::Mat ihls_input)
{
    cv::Mat nhs_image(ihls_input.rows, ihls_input.cols, CV_8UC1);
    for (int i = 0; i < ihls_input.rows; ++i)
    {
        const uchar* ihsl_data = ihls_input.ptr<uchar> (i);
        uchar* nhs_data = nhs_image.ptr<uchar> (i);

        for (int j = 0; j < ihls_input.cols; ++j)
        {
            unsigned int h = *ihsl_data++;
            unsigned int l = *ihsl_data++;
            unsigned int s = *ihsl_data++;

            if (h < B_HUE_MAX && h > B_HUE_MIN && s > B_SAT_MIN)
            {
                *nhs_data++ = 255;
            }
            else
                *nhs_data++ = 0;
        }
    }

    return nhs_image;
}

string doubleToString(double number)
{

    std::stringstream ss;
    ss << number;
    return ss.str();
}

void DetectTrafficSign::createTrackbars()
{

}

Mat DetectTrafficSign::cleanImageOps(Mat thresh)
{

    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle

    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

    dilate(thresh, thresh, dilateElement);
    Mat contour_filled_pic(thresh.rows, thresh.cols, CV_8UC3);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


    for (size_t i = 0; i < contours.size(); i++)
    {
        // rectangle(contour_filled_pic, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2);
        drawContours( contour_filled_pic, contours, i, Scalar(255, 255, 255), -1, 8, hierarchy, 0, Point() );
    }

    erode(contour_filled_pic, contour_filled_pic, erodeElement);

    medianBlur(contour_filled_pic, contour_filled_pic, 3);

    return contour_filled_pic;
}

Mat DetectTrafficSign::objElimination(Mat &cleanedImage)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat contour_filled_pic_gray(cleanedImage.rows, cleanedImage.cols, CV_8UC1);
    cvtColor(cleanedImage, contour_filled_pic_gray, CV_BGR2GRAY);
    findContours(contour_filled_pic_gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    vector<Rect> boundRect( contours.size() );

    for (size_t i = 0; i < contours.size(); i++)
    {
        boundRect[i] = boundingRect(contours[i]);
    }

    
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) < cleanedImage.cols*cleanedImage.rows/500 || 
            contourArea(contours[i]) > cleanedImage.cols*cleanedImage.rows/100 ||
            (boundRect[i].width/boundRect[i].height < 0.8 || boundRect[i].width/boundRect[i].height > 1.2) ||
            (contourArea(contours[i])/(boundRect[i].width*boundRect[i].height) < 0.6))
            {
                rectangle(cleanedImage, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2);
                drawContours( cleanedImage, contours, i, Scalar(0, 0, 0), -1, 8, hierarchy, 0, Point() );
            }
    }
    // imshow("contour_filled_pic_gray", contour_filled_pic_gray);
    cvtColor(cleanedImage, contour_filled_pic_gray, CV_BGR2GRAY);
    return contour_filled_pic_gray;
}

vector<Point> DetectTrafficSign::contourExtraction(Mat contour_filled_pic)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    Mat contour_filled_pic_gray(contour_filled_pic.rows, contour_filled_pic.cols, CV_8UC1);
    // cvtColor(contour_filled_pic, contour_filled_pic_gray, CV_BGR2GRAY);
    findContours(contour_filled_pic_gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    vector<int> hull_index(contours.size());
    vector<Point> hull_point;
    vector<int> contour_add_index;
    vector<Point> hull;
    // Mat hull_pic(rgb_image.rows, rgb_image.cols, CV_8UC3);
    cv::Mat hull_pic(contour_filled_pic.rows, contour_filled_pic.cols, CV_BGR2GRAY);

    int maxAreaIndex;
    double maxArea;
    if (contours.size() < 1) return hull;
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) > maxArea)
        {
            maxAreaIndex = i;
        }
    }

    convexHull(contours[maxAreaIndex], hull_index, true);
    convexHull(contours[maxAreaIndex], hull_point, true);

    for (size_t j = 0; j < hull_index.size() - 1; j++)
    {
        double a = norm(contours[maxAreaIndex][hull_index[j]] - contours[maxAreaIndex][hull_index[j + 1]]);
        // circle(contour_filled_pic, contours[i][hull[j]], 1, Scalar(0, 0, 255));
        for (size_t k = hull_index[j] + 1; k < hull_index[j + 1]; k++)
        {
            double b = norm(contours[maxAreaIndex][hull_index[j]] - contours[maxAreaIndex][k]);
            double c = norm(contours[maxAreaIndex][hull_index[j + 1]] - contours[maxAreaIndex][k]);
            double s = (a + b + c)/2;
            double alt = 2*sqrt(s*(s - a)*(s - b)*(s - c))/a;
            if (alt < 2)
            {
                contour_add_index.push_back(k);
            }
        }       
    }
    for (size_t j = 0; j < contour_add_index.size(); j++)
    {
        hull_point.push_back(contours[maxAreaIndex][contour_add_index[j]]);
    }
    
    vector<Point> hull_point_sorted;
    convexHull(hull_point, hull, true);

    return hull;

        // for (size_t i = 0; i < hull.size(); i++)
        // {
        //  drawContours(hull_pic, hull, i, Scalar(255, 255, 255), 1);
        //  Moments mu = moments(hull[i]);
        //  circle(rgb_image, Point2f(mu.m10/mu.m00 , mu.m01/mu.m00), sqrtf(mu.m00/M_PI), Scalar(0, 0, 255));
        // }
}

Mat DetectTrafficSign::detectSign(vector<Point> hull, Mat &pic_to_draw)
{
    Mat sign_mask(pic_to_draw.rows, pic_to_draw.cols, CV_8UC1);
        // drawContours(hull_pic, hull, i, Scalar(255, 255, 255), 1);
    Moments mu = moments(hull);
    circle(pic_to_draw, Point2f(mu.m10/mu.m00 , mu.m01/mu.m00), sqrtf(mu.m00/M_PI), Scalar(0, 0, 255));
    circle(sign_mask, Point2f(mu.m10/mu.m00 , mu.m01/mu.m00), sqrtf(mu.m00/M_PI), Scalar(255, 255, 255), -1);

    // imshow("sign mask", sign_mask);
    return sign_mask;
}

Mat DetectTrafficSign::rgb_to_ihls(Mat rgb_image)
{
    Mat ihls_image(rgb_image.rows, rgb_image.cols, CV_8UC3);
    for (int i = 0; i < rgb_image.rows; ++i)
    {
        const uchar* rgb_data = rgb_image.ptr<uchar> (i);
        uchar* ihls_data = ihls_image.ptr<uchar> (i);

        for (int j = 0; j < rgb_image.cols; ++j)
        {
            unsigned int b = *rgb_data++;
            unsigned int g = *rgb_data++;
            unsigned int r = *rgb_data++;
            *ihls_data++ = (uchar) get_H(r, g, b);
            *ihls_data++ = (uchar) get_L(r, g, b);
            *ihls_data++ = (uchar) get_S(r, g, b);
        }
    }
    return ihls_image;
}
void DetectTrafficSign::detect(Mat rgb_image)
{
    Mat hsv_image(rgb_image.rows, rgb_image.cols, CV_8UC3);
    Mat hsv_range_image(rgb_image.rows, rgb_image.cols, CV_8UC3);
    cvtColor(rgb_image, hsv_image, CV_BGR2HSV);
	inRange(hsv_image, Scalar(100, 100, 40), Scalar(110, 140, 50), hsv_range_image);

    // Mat ihls_image = rgb_to_ihls(rgb_image);
    // Mat nhs_image = blue_nhs(ihls_image);
    // Mat cleaned_image = cleanImageOps(nhs_image);

    // Mat obj_elimination_pic = objElimination(cleaned_image);

    // vector<Point> hull = contourExtraction(obj_elimination_pic);
    // if (hull.size() < 1)
    //     cout << "not found!!";
    // else
    // {
    //     detectSign(hull, rgb_image);        
    // }
    // imshow("hsv_range_image", hsv_range_image);
}