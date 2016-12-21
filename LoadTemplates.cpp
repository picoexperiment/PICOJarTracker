/*Code to load search templates*/

#include <opencv2/opencv.hpp>
#include "LoadTemplates.hpp"
#include <stdio.h>
#include <vector>
#include "NumMarks.hpp"



void LoadTemplatesSingleCam(std::vector<cv::Mat>& SearchTemplates, int camera, int HowManyTemplates){

    std::string FileLoc = "cam"+std::to_string(camera)+"/";
    cv::Mat _tempReadTemplate;

    for (int i=1; i<=HowManyTemplates; i++){

        std::string TemplateFName=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate=cv::imread(TemplateFName,0);
        SearchTemplates.push_back(_tempReadTemplate);
    }


}


FiducialMark::FiducialMark(int camera, float TX, float TY, float cX, float cY, std::string FileLocAndName, cv::Mat TImage):
    camera(camera), TemplateX(TX), TemplateY(TY), correctionsX(cX), correctionsY(cY), TemplateFileName(FileLocAndName), TemplateImage(TImage){}



void LoadTemplatesCam0(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam0/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;


    for (int i=1; i<=NMark0; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    FiducialMark templ1 = FiducialMark(0,  576, 424, 4, 7, "templ1.png", _templateDataImg[0]);
    FiducialMark templ2 = FiducialMark(0,  845, 419, 5, 5, "templ2.png", _templateDataImg[1]);
    FiducialMark templ3 = FiducialMark(0, 1032, 417, 5, 4, "templ3.png", _templateDataImg[2]);
    FiducialMark templ4 = FiducialMark(0, 1235, 415, 4, 5, "templ4.png", _templateDataImg[3]);
    FiducialMark templ5 = FiducialMark(0, 1462, 418, 5, 6, "templ5.png", _templateDataImg[4]);
    FiducialMark templ6 = FiducialMark(0,  507, 923, 5, 2, "templ6.png", _templateDataImg[5]);
    FiducialMark templ7 = FiducialMark(0,  871, 924, 6, 2, "templ7.png", _templateDataImg[6]);
    FiducialMark templ8 = FiducialMark(0, 1121, 912, 7, 6, "templ8.png", _templateDataImg[7]);
    FiducialMark templ9 = FiducialMark(0,  281, 557, 0, 0, "templ9.png", _templateDataImg[8]);
    FiducialMark templ10 = FiducialMark(0,  142, 312, 0, 0, "templ10.png", _templateDataImg[9]);

    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10};

}



void LoadTemplatesCam1(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam1/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;


    for (int i=1; i<=NMark1; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    FiducialMark templ1 = FiducialMark(1,  142, 464, 3, 5, "templ1.png", _templateDataImg[0]);
    FiducialMark templ2 = FiducialMark(1,  480, 450, 5, 4, "templ2.png", _templateDataImg[1]);
    FiducialMark templ3 = FiducialMark(1,  658, 444, 3, 4, "templ3.png", _templateDataImg[2]);
    FiducialMark templ4 = FiducialMark(1,  888, 443, 5, 6, "templ4.png", _templateDataImg[3]);
    FiducialMark templ5 = FiducialMark(1,  1123, 459, 3, 5, "templ5.png", _templateDataImg[4]);
    FiducialMark templ6 = FiducialMark(1,  361, 869, 7, 5, "templ6.png", _templateDataImg[5]);
    FiducialMark templ7 = FiducialMark(1,  595, 886, 6, 5, "templ7.png", _templateDataImg[6]);
    FiducialMark templ8 = FiducialMark(1,  903, 893, 6, 4, "templ8.png", _templateDataImg[7]);
    FiducialMark templ9 = FiducialMark(1,  1207, 881, 5, 4, "templ9.png", _templateDataImg[8]);
    FiducialMark templ10 = FiducialMark(1,  439, 170, 7, 2, "templ10.png", _templateDataImg[9]);
    FiducialMark templ11 = FiducialMark(1,  922, 153, 9, 3, "templ11.png", _templateDataImg[10]);



    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11};

}

void LoadTemplatesCam2(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam2/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;


    for (int i=1; i<=NMark2; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    FiducialMark templ1 = FiducialMark(2,  610.0, 706.0, 7, 6, "templ1.png", _templateDataImg[0]);
    FiducialMark templ2 = FiducialMark(2,  872.0, 709.0, 4, 6, "templ2.png", _templateDataImg[1]);
    FiducialMark templ3 = FiducialMark(2,  1053.0, 708.0, 5, 10, "templ3.png", _templateDataImg[2]);
    FiducialMark templ4 = FiducialMark(2,  1248.0, 702.0, 4, 7, "templ4.png", _templateDataImg[3]);
    FiducialMark templ5 = FiducialMark(2,  1466.0, 691.0, 5, 6, "templ5.png", _templateDataImg[4]);
    FiducialMark templ6 = FiducialMark(2,  1336.0, 234.0, 5, 4, "templ6.png", _templateDataImg[5]);
    FiducialMark templ7 = FiducialMark(2,  1590.0, 265.0, 6, 3, "templ7.png", _templateDataImg[6]);
    FiducialMark templ8 = FiducialMark(2,  1654.0, 677.0, 2, 6, "templ8.png", _templateDataImg[7]);
    FiducialMark templ9 = FiducialMark(2,  354.0, 872.0, 7, 2, "templ9.png", _templateDataImg[8]);
    FiducialMark templ10 = FiducialMark(2,  903.0, 886.0, 9, 3, "templ10.png", _templateDataImg[9]);
    FiducialMark templ11 = FiducialMark(2,  1288.0, 877.0, 6, 2, "templ11.png", _templateDataImg[10]);




    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11};

}


void LoadTemplatesCam3(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam3/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;


    for (int i=1; i<=NMark3; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    FiducialMark templ1 = FiducialMark(3, 1393, 793, 8, 4, "templ1.png", _templateDataImg[0]);
    FiducialMark templ2 = FiducialMark(3,  880, 675, 6, 6, "templ2.png", _templateDataImg[1]);
    FiducialMark templ3 = FiducialMark(3, 1111, 661, 4, 5, "templ3.png", _templateDataImg[2]);
    FiducialMark templ4 = FiducialMark(3,  649, 684, 4, 7, "templ4.png", _templateDataImg[3]);
    FiducialMark templ5 = FiducialMark(3,  468, 687, 5, 9, "templ5.png", _templateDataImg[4]);
    FiducialMark templ6 = FiducialMark(3,  319, 685, 4, 5, "templ6.png", _templateDataImg[5]);
    FiducialMark templ7 = FiducialMark(3,  127, 681, 4, 5, "templ7.png", _templateDataImg[6]);
    FiducialMark templ8 = FiducialMark(3,  429, 838, 7, 2, "templ8.png", _templateDataImg[7]);
    FiducialMark templ9 = FiducialMark(3,  576, 236, 3, 3, "templ9.png", _templateDataImg[8]);
    FiducialMark templ10 = FiducialMark(3,  920, 823, 8, 1, "templ10.png", _templateDataImg[9]);
    FiducialMark templ11 = FiducialMark(3,  860, 221, 7, 4, "templ11.png", _templateDataImg[10]);



    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11};

}


