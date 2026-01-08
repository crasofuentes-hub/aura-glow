cmake_minimum_required(VERSION 3.16)
project(auraglow_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(auraglow STATIC
  src/auraglow.cpp
)
target_include_directories(auraglow PUBLIC include)

add_executable(auraglow_demo src/demo.cpp)
target_link_libraries(auraglow_demo PRIVATE auraglow)
