#ifndef LoadTemplates_HPP_INCLUDED
#define LoadTemplates_HPP_INCLUDED

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>

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


void LoadTemplatesCam0(std::vector<FiducialMark>& );
void LoadTemplatesCam1(std::vector<FiducialMark>& );
void LoadTemplatesCam2(std::vector<FiducialMark>& );
void LoadTemplatesCam3(std::vector<FiducialMark>& );

#endif // LoadTemplates_HPP_INCLUDED
