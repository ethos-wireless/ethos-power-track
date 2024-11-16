cmake_minimum_required(VERSION 3.10)
#Allow project version
cmake_policy(SET CMP0048 NEW)
project(ETHOS-power VERSION 0.0.1)

# find_package(CppLinuxSerial)
# if (NOT CppLinuxSerial_FOUND)
# 	message(FATAL_ERROR "CppLinuxSerial library not found")
# endif()

if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
  message(STATUS "Using GNU compiler, compiler ID ${CMAKE_C_COMPILER_ID}")
  #For Ubuntu 1804 need to keep the c11 std for thread check
  set(CMAKE_C_FLAGS "-std=c11 -Wall")
  set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -march=native")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
else()
  message(FATAL_ERROR "Unsupported version of compiler")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()


set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(NVML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/cuda-12.2")

# Collect all .cpp and .h files in the src directory and add main.cpp,
# excluding files in include/CppLinuxSerial and GpuTracker.cpp
file(GLOB_RECURSE SRC_FILES 
    "${SRC_DIR}/*.cpp" 
    "${SRC_DIR}/*.h" 
    "${CMAKE_SOURCE_DIR}/main.cpp" 
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/*.cpp"
)
# list(FILTER SRC_FILES EXCLUDE REGEX ".*CppLinuxSerial/.*")
# list(FILTER SRC_FILES EXCLUDE REGEX ".*/GpuTracker.h")  # Exclude GpuTracker.cpp from main sources

# Add the main executable target excluding GpuTracker.cpp
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include directories, adding src for main.cpp and excluding CppLinuxSerial
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${NVML_INCLUDE_DIR} ${SRC_DIR})

# Add GpuTracker.cpp as a separate target object library
# add_library(GpuTrackerObj OBJECT ${SRC_DIR}/GpuTracker.h)

set(NVML_LIB "/usr/lib32/libnvidia-ml.so")

target_link_libraries(${PROJECT_NAME} -L$(NVML_LIB) -lnvidia-ml -lCppLinuxSerial)
