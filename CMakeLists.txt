cmake_minimum_required (VERSION 2.8)

project (wstatgen)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

if (UNIX)
	add_definitions( -std=c++11 )
endif (UNIX)

SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)
