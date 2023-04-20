cmake_minimum_required(VERSION 2.6)
project( CountParticulates )


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} .)

find_package(OpenCV REQUIRED)
# It seems that boost isn't installed properly on graham. The single-threaded
# libraries are always loaded, even if the multi-threaded libraries are the
# default. Since we don't need multi-threading here, it's fine to just turn it
# off.
set(Boost_USE_MULTITHREADED 0)
find_package(Boost REQUIRED COMPONENTS regex filesystem program_options QUIET)
execute_process(COMMAND git submodule update --init --recursive)

include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/minizip-ng/)
execute_process(COMMAND git submodule update --init --recursive
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  RESULT_VARIABLE SUBMOD_RESULT)

# Minizip-ng options override - Only compile Zlib support
set(MZ_COMPAT OFF CACHE BOOL "Enable compatibility layer with minizip
(non-ng)")
set(MZ_BZIP2 OFF CACHE BOOL "Compile bzip2 support")
set(MZ_LIBBSD OFF CACHE BOOL "Compile libbsd support")
set(MZ_LZMA OFF CACHE BOOL "Compile LZMA support")
set(MZ_OPENSSL OFF CACHE BOOL "Enable OpenSSL support for encryption")
set(MZ_PKCRYPT OFF CACHE BOOL "Enable PKWARE support for encryption")
set(MZ_SIGNING OFF CACHE BOOL "Enable zip signing")
set(MZ_WZAES OFF CACHE BOOL "Enable WinZIP AES encryption")
set(MZ_ZSTD OFF CACHE BOOL "Enable ZSTD compression")

add_subdirectory(minizip-ng)

# Required for minizip-ng
find_package(ZLIB QUIET)


set(CMAKE_CXX_FLAGS "-I${OpenCV_INCLUDE_DIRS}/ -I${CMAKE_CURRENT_SOURCE_DIR}/minizip-ng/")
set (CMAKE_EXE_LINKER_FLAGS "-Wno-write-strings")

#utility and folder parsing
set(UtilitySources
    ${CMAKE_CURRENT_SOURCE_DIR}/common/UtilityFunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ParseFolder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/RawParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ZipParser.cpp
)
set(UtilityHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/common/UtilityFunctions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ParseFolder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/Parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/RawParser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParseFolder/ZipParser.hpp
)
add_library(AbubUtility SHARED ${UtilitySources})
target_include_directories(AbubUtility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/minizip-ng/)

target_link_libraries(AbubUtility ${OpenCV_LIBS} ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${LIBLZMA_LIBRARIES}
    ${ZSTD_LIBRARIES} ${OPENSSL_LIBRARIES})
set_property(TARGET AbubUtility PROPERTY PUBLIC_HEADER ${UtilityHeaders})


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
)

#file(GLOB minizip_headers ${CMAKE_CURRENT_SOURCE_DIR}/minizip-ng/*h)
#list(APPEND HEADERS ${minizip_headers})

add_executable( P60Optometrist P60OptometristStart.cpp )
target_include_directories(P60Optometrist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/minizip-ng/)
target_link_libraries( P60Optometrist ${OpenCV_LIBS} ${SOURCE} ${HEADERS} AbubUtility
    minizip)

if(NOT SUBMOD_RESULT EQUAL "0")
  message("\n\nFailed to retrieve submodules with error code ${SUBMOD_RESULT}.\n\n")
endif()
