#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>


#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/calib3d/calib3d_c.h>


#include "AnalyzerUnit.hpp"
#include "common/UtilityFunctions.hpp"
#include "LoadTemplates.hpp"


#define SubPixelAveragingGrid 1



AnalyzerUnit::AnalyzerUnit(std::string EventID, std::string ImageDir, int CameraNumber, std::vector<FiducialMark>& Templates)
{
    /*Give the properties required to make the object - the identifiers i.e. the camera number, and location*/
    this->ImageDir=ImageDir;
    this->CameraNumber=CameraNumber;
    this->EventID=EventID;

    this->Templates = Templates;


}

AnalyzerUnit::~AnalyzerUnit(void ){
}



/*! \brief Load a frame for fiducial tracking
 *
 * \param void
 * \return loads the frame to the class variable
 *
 */

void AnalyzerUnit::LoadFrameForFiducialTracking(void){

    LoadFrameName = this->ImageDir+"cam"+std::to_string(this->CameraNumber)+"_image"+std::to_string(this->AnalyzeFrame)+".png";


    if (getFilesize(LoadFrameName) < 1000000) {
        this->okToProceed=false;
        std::cout<<"Malformed image: "<<LoadFrameName<<"\n";
        std::cout<<"Skipping on this event and continue...\n";
        throw -10;
    } else {

        this->AnalysisFrame=cv::imread(LoadFrameName,0);
        this->okToProceed = true;

    }


}





/*! \brief Track Fiducial marks
 *
 * \param void
 * \return loads fiducial marks in cv::point vector
 *
 */

void AnalyzerUnit::TrackAllFiducialMarks(void){

    /*How many Markers to track*/
    int nMarkersToTrack = this->Templates.size();



    /*Track Templates*/
    cv::Point2f _storeEachTemplateBestMatchLoc;
    cv::Point2f _thisReferencePoint;

    for (int i=0; i<nMarkersToTrack; i++){
        this->TrackAFeature(this->Templates[i], _storeEachTemplateBestMatchLoc);
        /*Position matches*/
        this->TemplatePos.push_back(_storeEachTemplateBestMatchLoc);
        /*Reference points*/
        _thisReferencePoint = cv::Point2f(this->Templates[i].TemplateX, this->Templates[i].TemplateY);
        this->ReferencePoints.push_back(_thisReferencePoint);

        //std::cout<<"Template X: "<<this->TemplatePos[i].x<<" Y: "<<this->TemplatePos[i].y<<"\n";
        /*GC*/
        _thisReferencePoint = cv::Point2f(0.0,0.0);
        _storeEachTemplateBestMatchLoc = cv::Point2f(0.0,0.0);

    }
    this->okToProceed = true;

}

/*! \brief Track a keypoint
 *
 * \param cv::Mat template to track, TBI:
 * \return cv::Point x,y of the template
 *
 */

void AnalyzerUnit::TrackAFeature(FiducialMark& Template, cv::Point2f& BestMatchLoc){

    cv::Mat result;

    /*Restrict the ROT of the analysisFrame*/
    cv::Rect TemplateSearchZone = cv::Rect(Template.TemplateX-45, Template.TemplateY-45, 90, 90);
    cv::Mat _AnaFrameSmallROI = cv::Mat(this->AnalysisFrame, TemplateSearchZone);



    // Create the result matrix
    int result_cols =  _AnaFrameSmallROI.cols - Template.TemplateImage.cols + 1;
    int result_rows = _AnaFrameSmallROI.rows - Template.TemplateImage.rows + 1;
    result.create( result_rows, result_cols, CV_32F );


    // Do the Matching and Normalize
    int match_method=CV_TM_SQDIFF;
    cv::matchTemplate( _AnaFrameSmallROI, Template.TemplateImage, result, match_method );
    cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );



    // Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;
    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
        { matchLoc = minLoc; }
    else
        { matchLoc = maxLoc; }

    /*Append result to return var*/

    cv::Point2f BestMatchSubPixel;
    BestMatchSubPixel = cv::Point2f(0.0,0.0);

    float total_mass=0.0;
    float inv_mass=0.0;
    float pixel_value=0.0;

    for (int i=-SubPixelAveragingGrid; i<=SubPixelAveragingGrid; i++){
        for (int j=-SubPixelAveragingGrid; j<=SubPixelAveragingGrid; j++){
            pixel_value = result.at<float>(matchLoc.y+j, matchLoc.x+i);
            BestMatchSubPixel+=cv::Point2f(matchLoc.x+i,matchLoc.y+j)*pixel_value;
            total_mass +=pixel_value;
        }
    }

    inv_mass = 1.0/total_mass;
    BestMatchSubPixel *= inv_mass;



    /*Offset correction*/
    BestMatchLoc = cv::Point2f(BestMatchSubPixel.x+TemplateSearchZone.x+Template.correctionsX, BestMatchSubPixel.y+Template.correctionsY+TemplateSearchZone.y);
    //std::cout<<BestMatchLoc<<"\n";
    result.release();
}


/*! \brief Calculate perspective transform homotopy
 *
 * \param void
 * \return perspectivetransform
 *
 */

void AnalyzerUnit::CalculatePerspectiveShift(void){


    this->HomographyMatrix = cv::findHomography(this->TemplatePos, this->ReferencePoints, CV_RANSAC, 3);

}



/*Misc functions*/

/*! \brief Sorting function for camera frames
 *
 * To be used by std::sort for sorting files associated with
 * the camera frames. Not to be used otherwise.
 */

bool frameSortFunc(std::string i, std::string j)
{

    unsigned int sequence_i, camera_i;
    int got_i = sscanf(i.c_str(),  "cam%d_image%u.png",
                       &camera_i, &sequence_i
                      );


    assert(got_i == 2);

    unsigned int  sequence_j, camera_j;
    int got_j = sscanf(j.c_str(),  "cam%d_image%u.png",
                       &camera_j, &sequence_j
                      );
    assert(got_j == 2);

    return sequence_i < sequence_j;

}
