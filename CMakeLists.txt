#only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

#executable name
project(canClient CXX)

#configurations
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#subdirectory for the application
add_subdirectory(src)

#location where the "main.cpp" is hosted
add_executable(${PROJECT_NAME} src/canClient.cpp)
