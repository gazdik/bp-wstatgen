set(CMAKE_INCLUDE_CURRENT_DIR ON)

file( GLOB wstatgen_SOURCES *.cc )

add_executable (wstatgen ${wstatgen_SOURCES})

