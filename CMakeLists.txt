######################################################################################
### 0) Set your application properties
######################################################################################
# check cmake version compatibility
cmake_minimum_required(VERSION 3.2)

# set a language for the entire project.
enable_language(CXX)

# sends the -std=c++17 flag to the gcc compiler
set(CMAKE_CXX_STANDARD 17)

# Set the project name
SET(PROJ_NAME "Flowshop NEH-based heuristic recommendation")

# Here define the name and the version of your package
set(PACKAGE_NAME ${PROJ_NAME} CACHE STRING "Flowshop NEH-based heuristic recommendation" FORCE)
set(PACKAGE_VERSION "1" CACHE STRING "1" FORCE)

# Define project
project(${PROJ_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Define path to find Paradiseo cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

add_definitions(-DDATA_FOLDER="${CMAKE_SOURCE_DIR}/data")
add_definitions(-DDEPRECATED_MESSAGES="")

#set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lpthread -g -fPIC -v -Wall -Wextra -Wno-logical-op-parentheses -Wno-deprecated-register -Wno-pragma-once-outside-header")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ ")

# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -header-filter=.;
#   -checks=-*,clang-diagnostic-*,clang-analyzer-*,-clang-diagnostic-c++98-compat,performance-*,bugprone-*,modernize-*;)


#####################################################################################
### include required modules & utilities
#####################################################################################
include(CMakeBackwardCompatibilityCXX)
include(CheckLibraryExists)

#####################################################################################
### Manage the build type
#####################################################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

######################################################################################
### 2) include the sources
######################################################################################
# Prepare environment to find Paradiseo lib files

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/module")

find_package(Paradiseo COMPONENTS moeo eoutils eo)

include_directories(SYSTEM ${PARADISEO_INCLUDE_DIR})

######################################################################################
### 3) Link the libraries for your executable
######################################################################################

set(FSP_SOURCES
  src/flowshop-solver/FSPProblemFactory.cpp
  src/flowshop-solver/MHParamsSpecsFactory.cpp
  src/flowshop-solver/global.cpp
)

add_library(flowshop_solver_lib STATIC ${FSP_SOURCES})

set_property(TARGET flowshop_solver_lib PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(flowshop_solver_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

######################################################################################
# Create tests and docs
######################################################################################

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/test)
