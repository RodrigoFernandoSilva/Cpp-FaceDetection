#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

Mat RotateImage(Mat image, double angle);

int main(){

    double angle = 0;

    Mat src = imread("./Image/Female.jpg");


    while(1){

        imshow("Rotate Image", RotateImage(src, angle));

        angle --;

        if(waitKey(30) == 27){
            break;
        }

    }

    return 0;
}

Mat RotateImage(Mat image, double angle) {
    // get rotation matrix for rotating the image around its center
    Point2f center(image.cols/2.0, image.rows/2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);

    // determine bounding rectangle
    Rect bbox = RotatedRect(center,image.size(), angle).boundingRect();
    // adjust transformation matrix

    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    Mat dst;
    warpAffine(image, dst, rot, bbox.size());

    return dst;
}
