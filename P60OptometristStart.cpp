/* OpenCV code to find fiducial marks for
 * PICO Dark Matter search - the Geyser
 *
 * Written by Pitam Mitra for PICO Collaboration
 *
 * Latest Edit: 2014-07-11. See the HG Changelog
 */


#include <opencv2/opencv.hpp>
//#include "opencv2/cudaimgproc.hpp"
//#include <opencv/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>

/*C++ Stuff*/
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */

/*Geyser Image Analysis Stuff*/
#include "ParseFolder/ParseFolder.hpp"
//#include "common/CommonDatatypes.h"
//#include "SQLiteDBstorage/imageAnalysisResults.hpp"

#include "AnalyzerUnit.hpp"
#include "PICOFormatWriter/PICOFormatWriterV2.hpp"
#include "common/UtilityFunctions.hpp"
#include "LoadTemplates.hpp"

#include "ParseFolder/Parser.hpp"
#include "ParseFolder/RawParser.hpp"
#include "ParseFolder/ZipParser.hpp"

/*Number of Marks in Each Camera*/
#include "NumMarks.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

const int evalEntropyThresholdFrames = 2;
std::vector<int> badEvents;




/*Workaround because fermi grid is using old gcc*/
bool BubblePosZsort(cv::RotatedRect a, cv::RotatedRect b)
{
    return a.center.y<b.center.y;
}
bool eventNameOrderSort(std::string a, std::string b)
{
    return std::stoi(a)<std::stoi(b);
}



void RunEachCameraAnalysisMachine(OutputWriter** P60Output, std::string AEventNumber, std::string ImgDir, int camera, std::vector<FiducialMark>& CameraTrackObjects, Parser* FileParser){

    OutputWriter* Pico60Writer = *P60Output;



    //AnalyzerUnit *AnalyzerC0 = new AnalyzerUnit(EventList[evi], imageDir, 0, CameraTrackObjects0); /*EventID, imageDir and camera number*/
    AnalyzerUnit *AnalyzerCGeneric = new AnalyzerUnit(AEventNumber, ImgDir, camera, CameraTrackObjects, FileParser); /*EventID, imageDir and camera number*/

    /*We need the actual event number in case folders with events are missing*/
    int actualEventNumber = atoi(AEventNumber.c_str());

    /*Fancy coursors!*/
    advance_cursor();


    /*Exception handling - per camera*/
    try
    {
        AnalyzerCGeneric->LoadFrameForFiducialTracking();
        if (AnalyzerCGeneric->okToProceed) AnalyzerCGeneric->TrackAllFiducialMarks();
        if (AnalyzerCGeneric->okToProceed)
        {
            AnalyzerCGeneric->CalculatePerspectiveShift();
            Pico60Writer->stageMarkerOutput(AnalyzerCGeneric->TemplatePos, camera, actualEventNumber, AnalyzerCGeneric->HomographyMatrix);
        } else  {
            Pico60Writer->stageMarkerOutputError(camera, -5, actualEventNumber);
            Pico60Writer->thisFrameFailedAnalysis[camera]=true;
        }

    /*The exception block for camera specific crashes. outputs -6 for the error*/
    }
    catch (...)
    {
        Pico60Writer->stageMarkerOutputError(camera,-6, actualEventNumber);
        Pico60Writer->thisFrameFailedAnalysis[camera]=true;
    }

    delete AnalyzerCGeneric;
    advance_cursor();


}


std::string usage(){
    std::string msg(
                "Usage: P60Optometrist[-hz] [-D data_series] [-t template_dir] -d data_dir -r run_ID -o out_dir\n"
                "Run the AutoBub3hs bubble finding algorithm on a PICO run\n\n"
                "Required arguments:\n"
                "  -d, --data_dir = Dir\t\tpath to the directory in which the run folder/file is stored\n"
                "  -r, --run_id = Str\t\trun ID, formatted as YYYYMMDD_*\n"
                "  -o, --out_dir = Dir\t\tdirectory to write the output file to\n"
                "  -t, --template_dir = Dir\tdirectory containing subfolders with template images per camera. Subdirectories should be names \"cam0\", \"cam1\", etc.\n\n"
                "Optional arguments:\n"
                "  -h, --help\t\t\tgive this help message\n"
                "  -z, --zip\t\t\tindicate the run is stored as a zip file; otherwise assumed to be in a directory\n"
                "  -D, --data_series = Str\tname of the data series, e.g. 40l-19, 30l-16, etc.\n"
    );
    return msg;
}


/*The main autobub code starts here*/
int main(int argc, char** argv)
{
    std::string dataLoc;
    std::string run_number;
    std::string out_dir;
    std::string tem_dir;
    std::string data_series;
    bool zipped = false;

    // generic options
    po::options_description generic("Arguments");
    generic.add_options()
        ("help,h", "produce help message")
        ("data_series,D", po::value<std::string>(&data_series)->default_value(""), "data series name, e.g. 30l-16, 40l-19, etc.")
        ("zip,z", po::bool_switch(&zipped), "run is stored as a zip file")
        ("data_dir,d", po::value<std::string>(&dataLoc), "directory in which the run is stored")
        ("run_num,r", po::value<std::string>(&run_number), "run ID, formatted as YYYYMMDD_")
        ("out_dir,o", po::value<std::string>(&out_dir), "directory to write the output file to")
        ("template_dir,t", po::value<std::string>(&tem_dir), "directory containing the camera mask pictures")
    ;

    // Parsing arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(generic).run(), vm);
    po::notify(vm);

    if (vm.count("help") | argc == 1){
        std::cout << usage() << std::endl;
        return 1;
    }

    if (dataLoc.compare("") == 0 | run_number.compare("") == 0 | out_dir.compare("") == 0){
        std::cerr << "Insufficient required arguments; use \"P60Optometrist -h\" to view required arguments" << std::endl;
        return -1;
    }

    printf("This is P60Optometrist perspective tracker, the bubble chamber or the camera movement tracker.\n");

    if (dataLoc.find_last_of("/") != dataLoc.size()-1) dataLoc+="/";
    if (out_dir.find_last_of("/") != out_dir.size()-1) out_dir+="/";
    if (tem_dir.find_last_of("/") != tem_dir.size()-1) tem_dir+="/";

    std::string eventDir=dataLoc+run_number+"/";

    /*I anticipate the object to become large with many bubbles, so I wanted it on the heap*/
    OutputWriter *PICO60Output = new OutputWriter(out_dir, run_number);
    PICO60Output->writeHeader(NMark0,NMark1,NMark2,NMark3);

    /* File parser stuff */
    std::string imageFormat = "cam%d_image%u.png";
    std::string imageFolder = "/Images/";
    Parser *FileParser;
    /* The Parser reads directories/zip files and retreives image data */
    if (zipped){
        FileParser = new ZipParser(eventDir, imageFolder, imageFormat);
    }
    else {
        FileParser = new RawParser(eventDir, imageFolder, imageFormat);
    }

    /*Construct list of events*/
    std::vector<std::string> EventList;
    int* EVstatuscode = 0;
    std::cout<<eventDir<<"\n";
    try
    {
        //GetEventDirLists(eventDir.c_str(), EventList, EVstatuscode);
        FileParser->GetEventDirLists(EventList);

    /*Crash handler at the begining of the program - writes -5 if the folder could not be read*/
    }
    catch (...)
    {
        std::cout<<"Failed to read the images from the run. Optometrist cannot continue.\n";
        //PICO60Output->stageMarkerOutputError(CameraNumber,-5, -1);
        //PICO60Output->writeMarkerOutput();
        return -5;
    }

    /*A sort is unnecessary at this level, but it is good practice and does not cost extra resources*/
    std::sort(EventList.begin(), EventList.end(), eventNameOrderSort);
    /*Event list is now constructed*/





    /*Declare the memory to load the templates in*/
    //std::vector<cv::Mat> CameraTrackObjects0;
    //std::vector<cv::Mat> CameraTrackObjects1;
    //std::vector<cv::Mat> CameraTrackObjects2;
    //std::vector<cv::Mat> CameraTrackObjects3;
    //LoadTemplatesSingleCam(CameraTrackObjects0, 0, NMark0);
    //LoadTemplatesSingleCam(CameraTrackObjects1, 1, NMark1);
    //LoadTemplatesSingleCam(CameraTrackObjects2, 2, NMark2);
    //LoadTemplatesSingleCam(CameraTrackObjects3, 3, NMark3);

    /*New templates*/
    std::vector<FiducialMark> CameraTrackObjects0;
    std::vector<FiducialMark> CameraTrackObjects1;
    std::vector<FiducialMark> CameraTrackObjects2;
    std::vector<FiducialMark> CameraTrackObjects3;
    LoadTemplatesCam0(CameraTrackObjects0, tem_dir);
    LoadTemplatesCam1(CameraTrackObjects1, tem_dir);
    LoadTemplatesCam2(CameraTrackObjects2, tem_dir);
    LoadTemplatesCam3(CameraTrackObjects3, tem_dir);





    /*Detect mode
     *Iterate through all the events in the list and detect bubbles in them one by one
     *A seprate procedure will store them to a file at the end
     */

    for (int evi = 0; evi < EventList.size(); evi++)
    {
        std::string imageDir=eventDir+EventList[evi]+"/Images/";
        printf("\nProcessing event: %s / %d  ... ", EventList[evi].c_str(), EventList.size()-1);


/*
        for (int icam = 0; icam < 4; icam++){
            Parser *tp = FileParser->clone();
            RunEachCameraAnalysisMachine(&PICO60Output, EventList[evi], imageDir, icam, CameraTrackObjects0, tp);
            delete tp;
        }
*/
        Parser *tp = FileParser->clone();
        RunEachCameraAnalysisMachine(&PICO60Output, EventList[evi], imageDir, 0, CameraTrackObjects0, tp);//FileParser);
        RunEachCameraAnalysisMachine(&PICO60Output, EventList[evi], imageDir, 1, CameraTrackObjects1, tp);//FileParser);
        RunEachCameraAnalysisMachine(&PICO60Output, EventList[evi], imageDir, 2, CameraTrackObjects2, tp);//FileParser);
        RunEachCameraAnalysisMachine(&PICO60Output, EventList[evi], imageDir, 3, CameraTrackObjects3, tp);//FileParser);
        delete tp;



        /*Write and commit output after each iteration, so in the event of a crash, its not lost*/

        PICO60Output->writeCameraOutput(EventList[evi]);


    }

    delete PICO60Output;
    delete FileParser;







    printf("P60Optometrist done analyzing this sequence. Thank you.\n");
    return 0;

}

