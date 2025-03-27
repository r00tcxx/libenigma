project(libenigma)

file(GLOB_RECURSE SOURCE
    *.h
    *.hpp
    *.cpp
    *.c
    *.cc
    *.cxx
)

file(GLOB_RECURSE HEADER
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "source" FILES ${SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include PREFIX "include" FILES ${HEADER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(${PROJECT_NAME} STATIC ${HEADER} ${SOURCE})