#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <vector>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat DeteccaoDoRostoOlhos(Mat frame);

CascadeClassifier rosto_cascade;
CascadeClassifier olhos_cascade;

int main(){

    //Procura os arquivos xml
    if(!rosto_cascade.load("./haarcascades/haarcascade_frontalface_default_cuda.xml")){
        cout << "Erro ao carregar o arquivo haarcascade_frontalface_default_cuda.xml \n\n";
        getchar();
        return 1;
    }
    if(!olhos_cascade.load("./haarcascades/haarcascade_eye.xml")){
        cout << "Erro ao carregar o arquivo haarcascade_eye.xml \n\n";
        getchar();
        return 1;
    }

    VideoCapture camera(0); //Pega a imagem da câmera padrão do computador (0)

    if(!camera.isOpened()){
        cout << "Erro ao abrir a camera \n\n";
        getchar();
        return 1;
    }

    //É uma variavevel matriz
    Mat frame_camera;

    while(1){
        camera.read(frame_camera); //Joga a imagem da câmera na variável entre parenteses

        if(frame_camera.empty()){
            frame_camera = imread("./Images/Erro.jpg");
        }

        imshow("Detecção Facial", DeteccaoDoRostoOlhos(frame_camera));

        //Tecla <Esc>, usa a tabela ASCII
        if(waitKey(10) == 27){
            break;
        }
    }

    return 0;
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
    //rosto_cascade.detectMultiScale(frame_cinza, rosto, 1.1, 10, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING,
    //                             cvSize(0, 0), CvSize(300, 300)); //Outro codigo de detecção facial

    rosto_cascade.detectMultiScale( frame_cinza, rosto, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    //face_cascade.detectMultiScale( frame_cinza, rosto, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) ); também funciona
    for(int i = 0; i < rosto.size(); i ++){
        //Desenha um quadrado em volta do rosto
        Point rosto_final   (rosto[i].x + rosto[i].width, rosto[i].y + rosto[i].height);
        Point rosto_inicial (rosto[i].x, rosto[i].y);
        Mat faceROI = frame_cinza(rosto[i]);
        olhos_cascade.detectMultiScale(faceROI, olhos, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

        for(size_t j = 0; j < olhos.size(); j ++){
            //Desenha um quadrado em volta do olho
            Point olhos_final   (rosto[i].x + (olhos[j].x + olhos[j].width), rosto[i].y + (olhos[j].y + olhos[j].height));
            Point olhos_inicial (rosto[i].x + olhos[j].x, rosto[i].y + olhos[j].y);
            rectangle(frame, olhos_final, olhos_inicial, cvScalar(0, 0, 255), 1, 8, 0);

            //Desenha um circulo envolta do olho
            //Point olhos_centro ((rosto[i].x + olhos[j].x + olhos[j].width/2), (rosto[i].y + olhos[j].y + olhos[j].height/2);
            //int raio = cvRound ((olhos[j].width + olhos[j].height)*0.25 );
            //circle(frame, olhos_centro, raio, cvScalar(0, 0, 255), 1, 8, 0);
        }

        rectangle(frame, rosto_final, rosto_inicial, cvScalar(0, 255, 0), 1, 8, 0);
    }

    return frame;
}
