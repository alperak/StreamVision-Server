cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0146 OLD)
project(StreamVision-Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)
if(CPPZMQ_INCLUDE_DIR)
    message(STATUS "Found cppzmq at ${CPPZMQ_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "cppzmq not found!")
endif()
find_package(tinyxml2 REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCES
    src/main.cpp
    src/PipelineController.cpp
    src/InferenceEngine.cpp
    src/ConfigXML.cpp
    src/NetworkManager.cpp
)

add_executable(streamvision-server ${SOURCES})

target_include_directories(streamvision-server
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${ZMQ_INCLUDE_DIRS}
        ${CPPZMQ_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(streamvision-server
    PRIVATE
        ${OpenCV_LIBS}
        ${ZMQ_LIBRARIES}
        nlohmann_json::nlohmann_json
        tinyxml2::tinyxml2
        spdlog::spdlog
)

target_compile_options(streamvision-server PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic -Werror -Wimplicit-fallthrough -Wsign-conversion>
    $<$<CONFIG:Release>:-O3>
)

# Doxygen Documentation
option(BUILD_DOCS "Build Doxygen documentation" OFF)
find_package(Doxygen QUIET)
if(BUILD_DOCS AND DOXYGEN_FOUND)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
elseif(NOT BUILD_DOCS)
    message(STATUS "BUILD_DOCS is OFF.")
else()
    message(STATUS "Doxygen not found. Documentation target will not be available.")
endif()
