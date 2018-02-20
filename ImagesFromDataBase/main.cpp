#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <string>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

using namespace cv;
using namespace std;

void GenerateDataBase();
Mat DeteccaoDoRostoOlhos(Mat frame);
Mat RotationImage(Mat image, double angle);
Mat ResizeImage(Mat image, int altura);

CascadeClassifier rosto_cascade;
CascadeClassifier olhos_cascade;

int main(int argc, char* argv[]){

    GenerateDataBase();

    //Procura os arquivos xml
    if(!rosto_cascade.load("./haarcascades/haarcascade_frontalface_alt2.xml")){
        cout << "Erro ao carregar o arquivo haarcascade_frontalface_alt2.xml \n\n";
        return 1;
    }
    if(!olhos_cascade.load("./haarcascades/haarcascade_eye.xml")){
        cout << "Erro ao carregar o arquivo haarcascade_eye.xml \n\n";
        return 1;
    }

    //É uma variavevel matriz
    Mat rosto_1, rosto_2;

    string linha;
    ifstream bancoDeDados ("./Image/Image.Image"); // ifstream = padrão ios:in

    int timer;
    if(bancoDeDados.is_open()){
        while(!bancoDeDados.eof()){
            getline(bancoDeDados, linha);

            //Mostra na tela a imagem carregada do banco de dados
            rosto_1 = imread("./Image/" + linha, CV_LOAD_IMAGE_COLOR);
            if(!rosto_1.empty()){
                //Se a imagem for maior q 650px seu tamanho é mudado para caber melhor na tela
                if(rosto_1.rows > 650){
                    rosto_1 = ResizeImage(rosto_1, 650);
                }
                imshow("Detecção Facial", DeteccaoDoRostoOlhos(rosto_1));
                timer = 0;

                cout << linha << endl;
            } else{
                timer = 651;
            }



            while(1){
                if(waitKey(1) == 13 || timer > 100){
                    break;
                }
                if(waitKey(10) == 27){
                    exit(0);
                }
                timer++;
                //cout << timer << endl;
            }
        }
        bancoDeDados.close();
    }

    remove("./Image/Image.image");

    return 0;
}

void GenerateDataBase() {
    DIR *dir;
    struct dirent *lsdir;
    char *str;

    dir = opendir("./Image");
    ofstream criarBD ("./Image/Image.image");

    //Percorre toda a pasta "Image"
    while ( ( lsdir = readdir(dir) ) != NULL ) {
        str = lsdir->d_name;
        criarBD << str << "\n";
    }

    closedir(dir);
    criarBD.close();
}

Mat ResizeImage(Mat image, int altura){
    Size tamanho(((image.cols * altura) / image.rows), altura);
    cv::resize(image, image, tamanho, 0, 0, CV_INTER_LINEAR);
    return image;
}

Mat RotationImage(Mat image, double angle){

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

//Faz o reconhecimento facilal com a imagem cinza, porém, ele ainda funcoina sem a imgaem cinza, só que
//com ela acinzentada fica melhor o reconhecimento, telvez!!!
Mat DeteccaoDoRostoOlhos(Mat frame){
    Mat frame_cinza;

    //Vecores para desenhar onde esta o rosto e os olhos
    vector<Rect> rosto;
    vector<Rect> olhos;

    //pega o frame e deixa ele cinza, tem que fazer isso porque o OpenCV exige isso
    cvtColor(frame, frame_cinza, COLOR_BGR2GRAY);
    equalizeHist(frame_cinza, frame_cinza); //Equiliza o histograma da imagem

    //Detequita o rosto
    rosto_cascade.detectMultiScale(frame_cinza, rosto, 1.1, 10, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING,
                                   Size(0, 0), Size(300, 300));

    //face_cascade.detectMultiScale( frame_cinza, rosto, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) ); também funciona
    for(int i = 0; i < rosto.size(); i ++){
        Point rosto_final   (rosto[i].x + rosto[i].width, rosto[i].y + rosto[i].height);
        Point rosto_inicial (rosto[i].x, rosto[i].y);
        Mat faceROI = frame_cinza(rosto[i]);
        olhos_cascade.detectMultiScale(faceROI, olhos, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

        for(size_t j = 0; j < olhos.size(); j ++){
            Point olhos_final   (rosto[i].x + (olhos[j].x + olhos[j].width), rosto[i].y + (olhos[j].y + olhos[j].height));
            Point olhos_inicial (rosto[i].x + olhos[j].x, rosto[i].y + olhos[j].y);
            rectangle(frame, olhos_final, olhos_inicial, cvScalar(0, 0, 255), 1, 8, 0);
        }

        rectangle(frame, rosto_final, rosto_inicial, cvScalar(0, 255, 0), 1, 8, 0);
    }
    return frame;
}
