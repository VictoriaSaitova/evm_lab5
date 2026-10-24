cmake_minimum_required(VERSION 3.20)
project(evm_lab5)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "C:/msys64/ucrt64/lib/cmake/opencv5")
find_package(OpenCV REQUIRED)

add_executable(evm_lab5 main.cpp)
target_link_libraries(evm_lab5 ${OpenCV_LIBS})