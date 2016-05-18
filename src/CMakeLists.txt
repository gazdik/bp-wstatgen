set(CMAKE_INCLUDE_CURRENT_DIR ON)

file( GLOB wstatgen_SOURCES *.cc )

add_executable (wstatgen ${wstatgen_SOURCES})

if(WIN32)
  target_link_libraries(wstatgen ws2_32)
endif(WIN32)
