#ifndef ANALYZERUNIT_HPP_INCLUDED
#define ANALYZERUNIT_HPP_INCLUDED

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include "LoadTemplates.hpp"


class AnalyzerUnit{

    private:

        /*Error handling stuff*/
        int StatusCode=0;
        std::vector<cv::Point2f> ReferencePoints;




    protected:
        /*Event identification and location*/
        std::string ImageDir;
        std::string EventID;

        /*List of all the frames belonging to the particular event in question*/
        int CameraNumber;




    public:
        /*Constructor and deconstructor*/
        AnalyzerUnit(std::string, std::string, int, std::vector<FiducialMark>&);
        ~AnalyzerUnit(void );

        /*Function to parse and sort the triggers from the folder and the directory where the images are stored*/
        void LoadFrameForFiducialTracking( void );
        void LoadFrameForReferenceTracking(void);
        /*Functions to do the tracking*/
        void TrackAllFiducialMarks(void);
        void CalculatePerspectiveShift(void);
        void TrackAFeature(FiducialMark&, cv::Point2f& );




        /*Variable holding the RotatedRect bubble array and the trigger frame*/
        std::vector<cv::Point2f> TemplatePos;
        std::vector<cv::Point2f> TemplatePosReference;
        cv::Mat HomographyMatrix;


        /*Templates*/
        std::vector<FiducialMark> Templates;

        /*Frames to be loaded for analysis*/
        std::string LoadFrameName;
        cv::Mat AnalysisFrame;
        cv::Mat ReferenceFrame;
        int AnalyzeFrame = 30;

        /*Status checks*/
        bool okToProceed = true;

};

/*Helper functions*/
bool frameSortFunc(std::string , std::string );


#endif // ANALYZERUNIT_HPP_INCLUDED
