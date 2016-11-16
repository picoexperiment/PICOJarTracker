#ifndef PICOWRITER_HPP_INCLUDED
#define PICOWRITER_HPP_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <opencv2/opencv.hpp>



class OutputWriter{

    private:

        int camera;
        int StatusCode;
        std::string OutputDir;
        std::string run_number;
        std::string abubOutFilename;

        std::ofstream OutFile;
        std::stringstream _StreamOutput;
        std::stringstream _StreamOutputMatrix;


    protected:


    public:

        struct TrackedObjectData{
            std::vector<cv::Point2f> TrackerObjectData;
            cv::Mat transformMatrix;
            int StatusCode;
            int event;
            TrackedObjectData();
        };

        TrackedObjectData TrackedObjectsCam0;
        TrackedObjectData TrackedObjectsCam1;
        TrackedObjectData TrackedObjectsCam2;
        TrackedObjectData TrackedObjectsCam3;



        /*Trainer instance initilized with the camera number*/
        OutputWriter(std::string, std::string );
        ~OutputWriter(void );

        /*Compute the mean and std*/
        void writeHeader(int, int, int, int );
        void stageMarkerOutput(std::vector<cv::Point2f>& , int , int, cv::Mat& );
        void stageMarkerOutputError(int, int, int);

        void formEachBubbleOutput(int, int& );
        void writeCameraOutput(std::string );
        int CalculateNBubCamera(int );
        void formPerspectiveMatrixOutput(int);



};


#endif // ANALYZERUNIT_HPP_INCLUDED
