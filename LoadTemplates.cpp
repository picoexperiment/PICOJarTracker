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
    std::vector<int> templ_nums;
    SearchTemplates.clear();
    std::ifstream fin;
    std::string ConfigFileLoc = temLocations+"templ.cfg";
    fin.open(ConfigFileLoc);
    if (!fin.is_open()){
        std::cout << "Templates config file not found at " << ConfigFileLoc << std::endl;
        return -1;
    }
    int templ;
    double X, Y, cX, cY;
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
        templ_nums.push_back(templ);
        Xs.push_back(X);
        Ys.push_back(Y);
        cXs.push_back(cX);
        cYs.push_back(cY);
    }
    
    std::string TemplFileLoc = temLocations+"cam"+std::to_string(camera)+"/";
    cv::Mat _tempReadTemplate;

    int NMark = Xs.size();

    for (int i=0; i<NMark; i++){
        std::string templName = "templ"+std::to_string(templ_nums[i])+".png";
        std::string _tfilename=TemplFileLoc+templName;
        _tempReadTemplate = cv::imread(_tfilename, 0);
        if (_tempReadTemplate.empty()){
            std::cout << "Failed to load template " << _tfilename << std::endl;
            return -2;
        }
        ProcessImage(_tempReadTemplate);
        FiducialMark templ  = FiducialMark(camera,  Xs[i], Ys[i], cXs[i], cYs[i], templName,  _tempReadTemplate);
        if (SAVE_DEBUG_IMAGES) cv::imwrite("cam"+std::to_string(camera)+"_"+templName,_tempReadTemplate);
        SearchTemplates.push_back(templ);
        _tempReadTemplate.release();
    }
    
    return 0;
}

