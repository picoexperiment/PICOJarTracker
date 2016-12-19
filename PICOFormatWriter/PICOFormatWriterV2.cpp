#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>


#include <opencv2/opencv.hpp>
#include "PICOFormatWriterV2.hpp"
#include "../common/UtilityFunctions.hpp"
#include "../NumMarks.hpp"



//#include "ImageEntropyMethods/ImageEntropyMethods.hpp"
//#include "LBP/LBPUser.hpp"




OutputWriter::OutputWriter(std::string OutDir, std::string run_number)
{
    /*Give the properties required to make the object - the identifiers i.e. the camera number, and location*/
    this->OutputDir = OutDir;
    this->run_number = run_number;

    this->abubOutFilename = this->OutputDir+"glasses_"+this->run_number+".txt";


    this->TrackedObjectsCam0.StatusCode=-2;
    this->TrackedObjectsCam1.StatusCode=-2;
    this->TrackedObjectsCam2.StatusCode=-2;
    this->TrackedObjectsCam3.StatusCode=-2;
    //this->OutFile.open(this->abubOutFilename);

    this->thisFrameFailedAnalysis=false;

}

OutputWriter::~OutputWriter(void ) {}

OutputWriter::TrackedObjectData::TrackedObjectData(){};

/*! \brief Function writes headers for the output file
 *
 * \param void
 * \return void
 *
 * This function writes the header file for abub3 output
 */

void OutputWriter::writeHeader(int NMarkCam0, int NMarkCam1, int NMarkCam2, int NMarkCam3 ){

    std::stringstream Line1;
    std::stringstream Line2;
    this->OutFile.open(this->abubOutFilename);
    this->OutFile<<"Output of P60Optometrist v2 - the automatic fiducial key point finder code by Pitam, using OpenCV.\n";
    this->OutFile<<"run ev ";

    Line1<<"key_point0("<<NMarkCam0<<",2) ";
    for (int i=0; i<NMarkCam0; i++){
        Line2<<"%f %f ";
    }

    Line1<<"key_point1("<<NMarkCam1<<",2) ";
    for (int i=0; i<NMarkCam1; i++){
        Line2<<"%f %f ";
    }

    Line1<<"key_point2("<<NMarkCam2<<",2) ";
    for (int i=0; i<NMarkCam2; i++){
        Line2<<"%f %f ";
    }

    Line1<<"key_point3("<<NMarkCam3<<",2) ";
    for (int i=0; i<NMarkCam3; i++){
        Line2<<"%f %f ";
    }

    Line1<<"perspective_transform0(3,3) perspective_transform1(3,3) perspective_transform2(3,3) perspective_transform3(3,3) ";
    for (int i=0; i<4*9; i++){
        Line2<<"%.9f ";
    }


    this->OutFile<<Line1.rdbuf()<<"\n";
    this->OutFile<<"%12s %5d "<<Line2.rdbuf()<<"\n2\n\n\n";
    this->OutFile.close();
}

/*! \brief Function stages and sorts the Rects as they come from the bubble identifier if there was no error int he process
 *
 * \param std::vector<cv::Rect> BubbleData
 * \param int camera
 * \return void
 *
 * This function writes the header file for abub3 output
 */

void OutputWriter::stageMarkerOutput(std::vector<cv::Point2f>& MarkerList, int camera, int event, cv::Mat& perspectiveMatrix){

    int tempStatus;
    if (MarkerList.size()==0) tempStatus = -1;
    else tempStatus = 0;

    TrackedObjectData* currentlyWorkingTrackPointer;

    if (camera==0) currentlyWorkingTrackPointer = &this->TrackedObjectsCam0;
    else if (camera==1) currentlyWorkingTrackPointer = &this->TrackedObjectsCam1;
    else if (camera==2) currentlyWorkingTrackPointer = &this->TrackedObjectsCam2;
    else if (camera==3) currentlyWorkingTrackPointer = &this->TrackedObjectsCam3;


    currentlyWorkingTrackPointer->TrackerObjectData = MarkerList;
    currentlyWorkingTrackPointer->StatusCode = tempStatus;
    currentlyWorkingTrackPointer->event = event;
    currentlyWorkingTrackPointer->transformMatrix=perspectiveMatrix;

}


/*! \brief Function stages error
 *
 * \param int error
 * \param int event
 * \param int camera
 * \return void
 *
 * This function writes the header file for abub3 output
 */


void OutputWriter::stageMarkerOutputError(int camera, int error, int event){

    TrackedObjectData* currentlyWorkingTrackPointer;

    if (camera==0) currentlyWorkingTrackPointer = &this->TrackedObjectsCam0;
    else if (camera==1) currentlyWorkingTrackPointer = &this->TrackedObjectsCam1;
    else if (camera==2) currentlyWorkingTrackPointer = &this->TrackedObjectsCam2;
    else if (camera==3) currentlyWorkingTrackPointer = &this->TrackedObjectsCam3;

    currentlyWorkingTrackPointer->StatusCode = error;
    currentlyWorkingTrackPointer->event = event;

}



void OutputWriter::formEachBubbleOutput(int camera, int nMarkThisCamera){




    TrackedObjectData* currentlyWorkingTrackPointer;

    if (camera==0) currentlyWorkingTrackPointer = &this->TrackedObjectsCam0;
    else if (camera==1) currentlyWorkingTrackPointer = &this->TrackedObjectsCam1;
    else if (camera==2) currentlyWorkingTrackPointer = &this->TrackedObjectsCam2;
    else if (camera==3) currentlyWorkingTrackPointer = &this->TrackedObjectsCam3;


    std::stringstream OutLine;

   if (currentlyWorkingTrackPointer->StatusCode !=0) {

        /*Write zero outputs for the objects*/
        for (int i=0; i<nMarkThisCamera; i++){
            OutLine<<-9998<<" "<<-9998<<" ";
        }

        /*Set the flag to not write perspective matrices*/
        this->thisFrameFailedAnalysis=true;

    } else {
    /*Write all outputs here*/
        for (int i=0; i<currentlyWorkingTrackPointer->TrackerObjectData.size(); i++){

            OutLine<<currentlyWorkingTrackPointer->TrackerObjectData[i].x<<" "<<currentlyWorkingTrackPointer->TrackerObjectData[i].y<<" ";
        }
    }
    this->_StreamOutput<<OutLine.rdbuf();
    OutLine.clear();
}

void OutputWriter::formPerspectiveMatrixOutput(int camera){

    TrackedObjectData* currentlyWorkingTrackPointer;

    if (camera==0) currentlyWorkingTrackPointer = &this->TrackedObjectsCam0;
    else if (camera==1) currentlyWorkingTrackPointer = &this->TrackedObjectsCam1;
    else if (camera==2) currentlyWorkingTrackPointer = &this->TrackedObjectsCam2;
    else if (camera==3) currentlyWorkingTrackPointer = &this->TrackedObjectsCam3;


    std::stringstream TransformMatrix;


    if (currentlyWorkingTrackPointer->StatusCode !=0) {

        for(int i=0; i<9; i++) TransformMatrix<<0.0<<" ";

    } else {
        for (int i=0; i<3; i++){
            for (int j=0; j<3; j++){
                TransformMatrix<<currentlyWorkingTrackPointer->transformMatrix.at<double>(i,j);
                TransformMatrix<<" ";
            }
        }
    }

    this->_StreamOutputMatrix<<TransformMatrix.rdbuf();
}


void OutputWriter::formPerspectiveMatrixError(void ){

    std::stringstream TransformMatrix;

    for (int i=0; i<9; i++) TransformMatrix<<-9999<<" ";

    this->_StreamOutputMatrix<<TransformMatrix.rdbuf();
}




void OutputWriter::writeCameraOutput(std::string EventName){

    int ibubImageStart = 1;

    /*Set up the stream*/
    this->_StreamOutput.clear();
    this->_StreamOutput.precision(2);
    this->_StreamOutput.setf(std::ios::fixed, std::ios::floatfield);
    this->_StreamOutput<<this->run_number<<" "<<EventName<<"    ";




    /*Add to the stream*/
    this->formEachBubbleOutput(0, NMark0);
    this->formEachBubbleOutput(1, NMark1);
    this->formEachBubbleOutput(2, NMark2);
    this->formEachBubbleOutput(3, NMark3);

    /*Add transform matrix*/
    this->_StreamOutputMatrix.clear();
    this->_StreamOutputMatrix.precision(5);
    this->_StreamOutputMatrix.setf(std::ios::fixed, std::ios::floatfield);

    if (this->thisFrameFailedAnalysis){
        this->formPerspectiveMatrixError(); /*Camera 0*/
        this->formPerspectiveMatrixError(); /*Camera 1*/
        this->formPerspectiveMatrixError(); /*Camera 2*/
        this->formPerspectiveMatrixError(); /*Camera 3*/
    } else {
        this->formPerspectiveMatrixOutput(0);
        this->formPerspectiveMatrixOutput(1);
        this->formPerspectiveMatrixOutput(2);
        this->formPerspectiveMatrixOutput(3);
    }

    /*write*/
    this->OutFile.open(this->abubOutFilename, std::fstream::out | std::fstream::app);
    this->OutFile<<this->_StreamOutput.rdbuf()<<" "<<_StreamOutputMatrix.rdbuf()<<"\n";
    this->OutFile.close();


    /*Reset*/
    this->thisFrameFailedAnalysis=false;

}


