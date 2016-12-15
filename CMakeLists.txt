cmake_minimum_required(VERSION 2.6)
project( CountParticulates )


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} .)

set(OpenCV_LIBS_DIR "/grid/fermiapp/products/coupp/prd/opencv/lib")
link_directories(${OpenCV_LIBS_DIR})
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_calib3d opencv_video opencv_features2d opencv_ml opencv_highgui opencv_objdetect opencv_contrib opencv_legacy opencv_gpu)


set(CMAKE_CXX_FLAGS "-I/grid/fermiapp/products/coupp/prd/opencv/include/ -std=gnu++1y -march=native -m64")
set (CMAKE_EXE_LINKER_FLAGS "-Wno-write-strings")



set(SOURCE
   ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ParseFolder.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/common/UtilityFunctions.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/PICOFormatWriter/PICOFormatWriterV2.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/AnalyzerUnit.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LoadTemplates.cpp
)
set(HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ParseFolder.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/PICOFormatWriter/PICOFormatWriterV2.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/AnalyzerUnit.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LoadTemplates.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/common/UtilityFunctions.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/NumMarks.hpp
)



add_executable( P60Optometrist P60OptometristStart.cpp )
target_link_libraries( P60Optometrist ${OpenCV_LIBS} ${SOURCE} ${HEADERS})
