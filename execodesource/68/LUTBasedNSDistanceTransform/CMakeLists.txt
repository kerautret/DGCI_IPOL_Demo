# $Id: CMakeLists.txt 153 2014-02-26 10:50:16Z Nicolas.Normand $

project (LUTBasedNSDistanceTransform CXX)
cmake_minimum_required (VERSION 2.6)

option(WITH_PNG "png" TRUE)
option(WITH_NETPBM "netpbm" TRUE)

#link_directories("/usr/local/netpbm/lib")

# Look for modules
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories("/usr/include/libpng")


add_library(sequence CumulativeSequence.cpp RationalBeattySequence)

add_executable(CumulativeSequenceTest CumulativeSequenceTest.cpp)
target_link_libraries(CumulativeSequenceTest sequence)

add_executable(RationalBeattySequenceTest RationalBeattySequenceTest.cpp)
target_link_libraries(RationalBeattySequenceTest sequence)

add_executable(LUTBasedNSDistanceTransform LUTBasedNSDistanceTransform.cpp ImageFilter.cpp ImageWriter.cpp BaseDistanceDT.cpp D4DistanceDT.cpp D8DistanceDT.cpp RatioNSDistanceDT.cpp PeriodicNSDistanceDT.cpp)

target_link_libraries(LUTBasedNSDistanceTransform sequence)

if (WITH_NETPBM)
    find_package(NetPBM REQUIRED)
endif (WITH_NETPBM)

if (NETPBM_FOUND)
    include_directories(${NETPBM_INCLUDE_DIR})
    add_library(pbmio PBMImageReader.cpp PGMImageWriter.cpp)
    target_link_libraries(LUTBasedNSDistanceTransform ${NETPBM_LIBRARIES})
    target_link_libraries(LUTBasedNSDistanceTransform pbmio)

    add_executable(ImageFeeder ImageFeeder.cpp)
    target_link_libraries(ImageFeeder ${NETPBM_LIBRARIES})
endif (NETPBM_FOUND)

if (WITH_PNG)
    find_package(png REQUIRED)
    if (PNG_FOUND)
	include_directories(${PNG_INCLUDE_DIRS})
	add_library(pngio PNGImageReader.cpp PNGImageWriter.cpp)
	target_link_libraries(LUTBasedNSDistanceTransform ${PNG_LIBRARIES})
	target_link_libraries(LUTBasedNSDistanceTransform pngio)
    endif (PNG_FOUND)
endif (WITH_PNG)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/LUTBasedNSDistanceTransformConfig.h.in"
  "${PROJECT_BINARY_DIR}/LUTBasedNSDistanceTransformConfig.h"
)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

IF(WITH_NETPBM) 
SET (LIST_OPTION ${LIST_OPTION} [NETPBM]\ ) 
message(STATUS "      WITH_NETPBM       true")
ELSE(WITH_NETPBM)
message(STATUS "      WITH_NETPBM       false")
ENDIF(WITH_NETPBM)

IF(WITH_PNG) 
SET (LIST_OPTION ${LIST_OPTION}    [PNG]\ ) 
message(STATUS "      WITH_PNG          true")
ELSE(WITH_PNG)
message(STATUS "      WITH_PNG          false")
ENDIF(WITH_PNG)
