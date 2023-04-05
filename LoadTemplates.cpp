/*Code to load search templates*/

#include <opencv2/opencv.hpp>
#include "LoadTemplates.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#define SAVE_DEBUG_IMAGES false

// Currently doesn't seem to help much, so disabled.
void ProcessImage(cv::Mat& image){
    //IncreaseContrast(image,255./86);
    //GammaCorrection(image,0.1);
}

void IncreaseContrast(cv::Mat& tempReadTemplate_raw, double contrast){
    cv::Mat tempReadTemplate = cv::Mat::zeros(tempReadTemplate_raw.size(),tempReadTemplate_raw.type());
    tempReadTemplate_raw.convertTo(tempReadTemplate, -1, contrast, 0);
    tempReadTemplate_raw = tempReadTemplate;
}

void GammaCorrection(cv::Mat& src, float fGamma)
{
  unsigned char lut[256];
  for (int i = 0; i < 256; i++)
  {
    lut[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
  }

  const int channels = src.channels();
  switch (channels)
  {
    case 1:
    {
      cv::MatIterator_<uchar> it, end;
      for (it = src.begin<uchar>(), end = src.end<uchar>(); it != end; it++)
      *it = lut[(*it)];
      break;
    }
    case 3:
    {
      cv::MatIterator_<cv::Vec3b> it, end;
      for (it = src.begin<cv::Vec3b>(), end = src.end<cv::Vec3b>(); it != end; it++)
      {
        (*it)[0] = lut[((*it)[0])];
        (*it)[1] = lut[((*it)[1])];
        (*it)[2] = lut[((*it)[2])];
      }
      break;
    }
  }
}


void LoadTemplatesSingleCam(std::vector<cv::Mat>& SearchTemplates, int camera, int HowManyTemplates){

    std::string FileLoc = "cam"+std::to_string(camera)+"/";
    cv::Mat _tempReadTemplate;

    for (int i=1; i<=HowManyTemplates; i++){

        std::string TemplateFName=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate=cv::imread(TemplateFName,0);
        ProcessImage(_tempReadTemplate);
        
        SearchTemplates.push_back(_tempReadTemplate);
    }


}


FiducialMark::FiducialMark(int camera, float TX, float TY, float cX, float cY, std::string FileLocAndName, cv::Mat TImage):
    camera(camera), TemplateX(TX), TemplateY(TY), correctionsX(cX), correctionsY(cY), TemplateFileName(FileLocAndName), TemplateImage(TImage){}

bool LoadTemplatesConfig(int camera, std::vector<FiducialMark>& SearchTemplates, std::string temLocations){
    std::vector<double> Xs;
    std::vector<double> Ys;
    std::vector<double> cXs;
    std::vector<double> cYs;
    SearchTemplates.clear();
    std::ifstream fin;
    std::string ConfigFileLoc = temLocations+"templ.cfg";
    fin.open(ConfigFileLoc);
    if (!fin.is_open()){
        std::cout << "Templates config file not found at " << ConfigFileLoc << std::endl;
        return -1;
    }
    double templ, X, Y, cX, cY;
    std::string head;
    bool head_found = false;
    while (!fin.eof()){
        std::string line;
        getline(fin,line);
        std::stringstream ss(line);
        if (ss.str()[0] == '#' || ss.str() == "" || ss.str()[0] == ' '){
            //std::cout << "Ignoring: " << line << std::endl;
            continue;
        }
        if (!head_found){   //Find camera header
            ss >> head;
            if (head == "cam"+std::to_string(camera)){
                head_found = true;
            }
            continue;
        }
        if (ss.str()[0] == 'c') break;  //At next header, so leave now
        ss >> templ >> X >> Y >> cX >> cY;
        if (fin.eof()) break;
        Xs.push_back(X);
        Ys.push_back(Y);
        cXs.push_back(cX);
        cYs.push_back(cY);
    }
    
    std::string TemplFileLoc = temLocations+"cam"+std::to_string(camera)+"/";
    cv::Mat _tempReadTemplate;

    int NMark = Xs.size();

    for (int i=1; i<=NMark; i++){
        std::string templName = "templ"+std::to_string(i)+".png";
        std::string _tfilename=TemplFileLoc+templName;
        _tempReadTemplate = cv::imread(_tfilename, 0);
        if (_tempReadTemplate.empty()){
            std::cout << "Failed to load template " << _tfilename << std::endl;
            return -2;
        }
        ProcessImage(_tempReadTemplate);
        FiducialMark templ  = FiducialMark(camera,  Xs[i-1], Ys[i-1], cXs[i-1], cYs[i-1], templName,  _tempReadTemplate);
        if (SAVE_DEBUG_IMAGES) cv::imwrite("cam"+std::to_string(camera)+"_"+templName,_tempReadTemplate);
        SearchTemplates.push_back(templ);
        _tempReadTemplate.release();
    }
    
    return 0;
}

void LoadTemplatesCam0(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam0/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;

    int NMark0 = 6;//10 //11

    for (int i=1; i<=NMark0; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        ProcessImage(_tempReadTemplate);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    // FiducialMark templ1 = FiducialMark(0,  576, 424, 4, 7, "templ1.png", _templateDataImg[0]);
    // FiducialMark templ2 = FiducialMark(0,  845, 419, 5, 5, "templ2.png", _templateDataImg[1]);
    // FiducialMark templ3 = FiducialMark(0, 1032, 417, 5, 4, "templ3.png", _templateDataImg[2]);
    // FiducialMark templ4 = FiducialMark(0, 1235, 415, 4, 5, "templ4.png", _templateDataImg[3]);
    // FiducialMark templ5 = FiducialMark(0, 1462, 418, 5, 6, "templ5.png", _templateDataImg[4]);
    // FiducialMark templ6 = FiducialMark(0,  507, 923, 5, 2, "templ6.png", _templateDataImg[5]);
    // FiducialMark templ7 = FiducialMark(0,  871, 924, 6, 2, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8 = FiducialMark(0, 1121, 912, 7, 6, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9 = FiducialMark(0,  281, 557, 0, 0, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(0,  142, 312, 0, 0, "templ10.png", _templateDataImg[9]);

    FiducialMark templ1  = FiducialMark(0,  350, 749, 0, 0, "templ1.png",  _templateDataImg[0]);    //lower front 1
    FiducialMark templ2  = FiducialMark(0,  940, 716, 0, 0, "templ2.png",  _templateDataImg[1]);    //lower front 3
    FiducialMark templ3  = FiducialMark(0,  460, 660, 0, 0, "templ3.png",  _templateDataImg[2]);    //middle back 1
    FiducialMark templ4  = FiducialMark(0,  792, 652, 0, 0, "templ4.png",  _templateDataImg[3]);    //middle back 3
    FiducialMark templ5  = FiducialMark(0,  645, 310, 0, 0, "templ5.png",  _templateDataImg[4]);    //upper front 0
    FiducialMark templ6  = FiducialMark(0,  1002, 346, 0, 0, "templ6.png",  _templateDataImg[5]);   //upper back 4
    
    // FiducialMark templ7  = FiducialMark(0,  , , 0, 0, "templ7.png",  _templateDataImg[6]);
    // FiducialMark templ8  = FiducialMark(0,  , , 0, 0, "templ8.png",  _templateDataImg[7]);
    // FiducialMark templ9  = FiducialMark(0,  , , 0, 0, "templ9.png",  _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(0,  , , 0, 0, "templ10.png", _templateDataImg[9]);

    for (int i = 0; SAVE_DEBUG_IMAGES && i < _templateDataImg.size(); i++)
        cv::imwrite("cam0_templ"+std::to_string(i+1)+".png",_templateDataImg[i]);

    // SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10};
    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6};

}



void LoadTemplatesCam1(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam1/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;

    int NMark1 = 6;//11 //10

    for (int i=1; i<=NMark1; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        ProcessImage(_tempReadTemplate);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    // FiducialMark templ1 = FiducialMark(1,  142, 464, 3, 5, "templ1.png", _templateDataImg[0]);
    // FiducialMark templ2 = FiducialMark(1,  480, 450, 5, 4, "templ2.png", _templateDataImg[1]);
    // FiducialMark templ3 = FiducialMark(1,  658, 444, 3, 4, "templ3.png", _templateDataImg[2]);
    // FiducialMark templ4 = FiducialMark(1,  888, 443, 5, 6, "templ4.png", _templateDataImg[3]);
    // FiducialMark templ5 = FiducialMark(1,  1123, 459, 3, 5, "templ5.png", _templateDataImg[4]);
    // FiducialMark templ6 = FiducialMark(1,  361, 869, 7, 5, "templ6.png", _templateDataImg[5]);
    // FiducialMark templ7 = FiducialMark(1,  595, 886, 6, 5, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8 = FiducialMark(1,  903, 893, 6, 4, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9 = FiducialMark(1,  1207, 881, 5, 4, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(1,  439, 170, 7, 2, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(1,  922, 153, 9, 3, "templ11.png", _templateDataImg[10]);

    FiducialMark templ1  = FiducialMark(1,  275, 728, 0, 0, "templ1.png", _templateDataImg[0]); //lower front 1
    FiducialMark templ2  = FiducialMark(1,  593, 712, 0, 0, "templ2.png", _templateDataImg[1]); //lower front 2
    FiducialMark templ3  = FiducialMark(1,  918, 710, 0, 0, "templ3.png", _templateDataImg[2]); //lower front 3
    FiducialMark templ4  = FiducialMark(1, 1304, 718, 0, 0, "templ4.png", _templateDataImg[3]); //lower front 4
    FiducialMark templ5  = FiducialMark(1,  490, 634, 0, 0, "templ5.png", _templateDataImg[4]); //middle back 2
    FiducialMark templ6  = FiducialMark(1,  746, 257, 0, 0, "templ6.png", _templateDataImg[5]); //upper back -3
    // FiducialMark templ7  = FiducialMark(1,  , , 0, 0, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8  = FiducialMark(1,  , , 0, 0, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9  = FiducialMark(1,  , , 0, 0, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(1,  , , 0, 0, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(1,  , , 0, 0, "templ11.png", _templateDataImg[10]);

    for (int i = 0; SAVE_DEBUG_IMAGES && i < _templateDataImg.size(); i++)
        cv::imwrite("cam1_templ"+std::to_string(i+1)+".png",_templateDataImg[i]);


    // SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11};
    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6};

}

void LoadTemplatesCam2(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam2/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;

    int NMark2 = 6;//14 //9

    for (int i=1; i<=NMark2; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        ProcessImage(_tempReadTemplate);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    // FiducialMark templ1 = FiducialMark(2,  610.0, 706.0, 7, 6, "templ1.png", _templateDataImg[0]);
    // FiducialMark templ2 = FiducialMark(2,  872.0, 709.0, 4, 6, "templ2.png", _templateDataImg[1]);
    // FiducialMark templ3 = FiducialMark(2,  1053.0, 708.0, 5, 10, "templ3.png", _templateDataImg[2]);
    // FiducialMark templ4 = FiducialMark(2,  1248.0, 702.0, 4, 7, "templ4.png", _templateDataImg[3]);
    // FiducialMark templ5 = FiducialMark(2,  1466.0, 691.0, 5, 6, "templ5.png", _templateDataImg[4]);
    // FiducialMark templ6 = FiducialMark(2,  1336.0, 234.0, 5, 4, "templ6.png", _templateDataImg[5]);
    // FiducialMark templ7 = FiducialMark(2,  1590.0, 265.0, 6, 3, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8 = FiducialMark(2,  1654.0, 677.0, 2, 6, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9 = FiducialMark(2,  354.0, 872.0, 7, 2, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(2,  903.0, 886.0, 9, 3, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(2,  1288.0, 877.0, 6, 2, "templ11.png", _templateDataImg[10]);
    // FiducialMark templ12 = FiducialMark(2,  549.0, 215.0, 3, 5, "templ12.png", _templateDataImg[11]);
    // FiducialMark templ13 = FiducialMark(2,  873.0, 209.0, 2, 3, "templ13.png", _templateDataImg[12]);
    // FiducialMark templ14 = FiducialMark(2,  1099.0, 217.0, 3, 2, "templ14.png", _templateDataImg[13]);

    FiducialMark templ1  = FiducialMark(2,  582, 704, 0, 0, "templ1.png", _templateDataImg[0]); //lower front 1
    FiducialMark templ2  = FiducialMark(2, 1198, 654, 0, 0, "templ2.png", _templateDataImg[1]); //lower front 2
    FiducialMark templ3  = FiducialMark(2,  318, 398, 0, 0, "templ3.png", _templateDataImg[2]); //upper back 1
    FiducialMark templ4  = FiducialMark(2,  546, 247, 0, 0, "templ4.png", _templateDataImg[3]); //upper front 1
    FiducialMark templ5  = FiducialMark(2,  859, 240, 0, 0, "templ5.png", _templateDataImg[4]); //upper front 2
    FiducialMark templ6  = FiducialMark(2, 1148, 232, 0, 0, "templ6.png", _templateDataImg[5]); //upper front 3
    // FiducialMark templ7  = FiducialMark(2,  0, 0, 0, 0, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8  = FiducialMark(2,  0, 0, 0, 0, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9  = FiducialMark(2,  0, 0, 0, 0, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(2,  0, 0, 0, 0, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(2,  0, 0, 0, 0, "templ11.png", _templateDataImg[10]);
    // FiducialMark templ12 = FiducialMark(2,  0, 0, 0, 0, "templ12.png", _templateDataImg[11]);
    // FiducialMark templ13 = FiducialMark(2,  0, 0, 0, 0, "templ13.png", _templateDataImg[12]);
    // FiducialMark templ14 = FiducialMark(2,  0, 0, 0, 0, "templ14.png", _templateDataImg[13]);

    for (int i = 0; SAVE_DEBUG_IMAGES && i < _templateDataImg.size(); i++)
        cv::imwrite("cam2_templ"+std::to_string(i+1)+".png",_templateDataImg[i]);


    // SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11, templ12, templ13, templ14};
    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6};

}


void LoadTemplatesCam3(std::vector<FiducialMark>& SearchTemplates, std::string temLocations){

    std::string FileLoc = temLocations+"cam3/";
    cv::Mat _tempReadTemplate;
    std::vector<cv::Mat> _templateDataImg;

    int NMark3 = 6;//11 //9

    for (int i=1; i<=NMark3; i++){
        std::string _tfilename=FileLoc+"templ"+std::to_string(i)+".png";
        _tempReadTemplate = cv::imread(_tfilename, 0);
        ProcessImage(_tempReadTemplate);
        _templateDataImg.push_back(_tempReadTemplate);
        _tempReadTemplate.release();
    }

    // FiducialMark templ1 = FiducialMark(3, 1393, 793, 8, 4, "templ1.png", _templateDataImg[0]);
    // FiducialMark templ2 = FiducialMark(3,  880, 675, 6, 6, "templ2.png", _templateDataImg[1]);
    // FiducialMark templ3 = FiducialMark(3, 1111, 661, 4, 5, "templ3.png", _templateDataImg[2]);
    // FiducialMark templ4 = FiducialMark(3,  649, 684, 4, 7, "templ4.png", _templateDataImg[3]);
    // FiducialMark templ5 = FiducialMark(3,  468, 687, 5, 9, "templ5.png", _templateDataImg[4]);
    // FiducialMark templ6 = FiducialMark(3,  319, 685, 4, 5, "templ6.png", _templateDataImg[5]);
    // FiducialMark templ7 = FiducialMark(3,  127, 681, 4, 5, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8 = FiducialMark(3,  429, 838, 7, 2, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9 = FiducialMark(3,  576, 236, 3, 3, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(3,  920, 823, 8, 1, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(3,  860, 221, 7, 4, "templ11.png", _templateDataImg[10]);

    FiducialMark templ1  = FiducialMark(3,  543, 735, 0, 0, "templ1.png", _templateDataImg[0]);
    FiducialMark templ2  = FiducialMark(3, 1235, 761, 0, 0, "templ2.png", _templateDataImg[1]);
    FiducialMark templ3  = FiducialMark(3,  270, 439, 0, 0, "templ3.png", _templateDataImg[2]);
    FiducialMark templ4  = FiducialMark(3,  450, 439, 0, 0, "templ4.png", _templateDataImg[3]);
    FiducialMark templ5  = FiducialMark(3,  197, 247, 0, 0, "templ5.png", _templateDataImg[4]);
    FiducialMark templ6  = FiducialMark(3,  1240, 247, 0, 0, "templ6.png", _templateDataImg[5]);
    // FiducialMark templ7  = FiducialMark(3,  , , 0, 0, "templ7.png", _templateDataImg[6]);
    // FiducialMark templ8  = FiducialMark(3,  , , 0, 0, "templ8.png", _templateDataImg[7]);
    // FiducialMark templ9  = FiducialMark(3,  , , 0, 0, "templ9.png", _templateDataImg[8]);
    // FiducialMark templ10 = FiducialMark(3,  , , 0, 0, "templ10.png", _templateDataImg[9]);
    // FiducialMark templ11 = FiducialMark(3,  , , 0, 0, "templ11.png", _templateDataImg[10]);

    for (int i = 0; SAVE_DEBUG_IMAGES && i < _templateDataImg.size(); i++)
        cv::imwrite("cam3_templ"+std::to_string(i+1)+".png",_templateDataImg[i]);


    // SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6, templ7, templ8, templ9, templ10, templ11};
    SearchTemplates = {templ1, templ2, templ3, templ4, templ5, templ6};

}


