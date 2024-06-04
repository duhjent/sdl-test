cmake_minimum_required(VERSION 3.5)
project(SDLTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
file(COPY ${PROJECT_BINARY_DIR}/vendor/SDL/SDL3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH})

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_executable(SDLTest src/main.cpp)

# target_include_directories(SDLTest PRIVATE SDL3::SDL3)

target_link_libraries(SDLTest PRIVATE SDL3::SDL3)

