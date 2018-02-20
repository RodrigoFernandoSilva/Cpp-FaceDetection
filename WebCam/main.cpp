#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int main() {

    String title;

    VideoCapture camera(0);
    Mat image;

    while(1){

        camera.read(image);

        //Just show the camera image if the variable has some thing
        if (image.empty()) {
            image = imread("./Images/Erro.jpg");
        }

        //Show the image on the windows
        imshow("WebCam", image);

        //Exit the windows if press 'Esc' key
        if(waitKey(10) == 27){
            break;
        }

    }

    return 0;
}
