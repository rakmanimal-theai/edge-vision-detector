cmake_minimum_required(VERSION 4.3.2)
set(CMAKE_CXX_STANDARD 17)
project(edge-vision-detector VERSION 0.1.0)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Stream.cpp
    src/Detector.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${OpenCV_INCLUDE_DIRS}
    include/   # tells compiler where to find your .h files
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})