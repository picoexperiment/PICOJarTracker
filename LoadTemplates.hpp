#ifndef LoadTemplates_HPP_INCLUDED
#define LoadTemplates_HPP_INCLUDED

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>

void IncreaseContrast(cv::Mat& tempReadTemplate_raw, double contrast);
void GammaCorrection(cv::Mat& src, float fGamma);
void ProcessImage(cv::Mat& image);

void LoadTemplatesSingleCam(std::vector<cv::Mat>&, int, int);


/*ReferenceTemplates*/

struct FiducialMark{

    int camera;

    float TemplateX;
    float TemplateY;

    float correctionsX;
    float correctionsY;

    std::string TemplateFileName;

    cv::Mat TemplateImage;

    FiducialMark(int , float , float , float , float , std::string , cv::Mat );


};


int LoadTemplatesCam0(std::vector<FiducialMark>&, std::string );
int LoadTemplatesCam1(std::vector<FiducialMark>&, std::string );
int LoadTemplatesCam2(std::vector<FiducialMark>&, std::string );
int LoadTemplatesCam3(std::vector<FiducialMark>&, std::string );

#endif // LoadTemplates_HPP_INCLUDED
