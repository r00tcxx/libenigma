project(test)
file(GLOB_RECURSE SOURCE
    *.h
    *.hpp
    *.cpp
    *.c
    *.cc
    *.cxx
)

message(${CMAKE_SOURCE_DIR}/3rd/gtest/${PLATFORM}/lib/debug/)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd/gtest/${PLATFORM}/include)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "source" FILES ${SOURCE})

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADER})
target_link_directories(${PROJECT_NAME} PRIVATE
    $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/3rd/gtest/${PLATFORM}/lib/debug/,${CMAKE_SOURCE_DIR}/3rd/gtest/${PLATFORM}/lib/release/>
)
target_link_libraries(${PROJECT_NAME} PRIVATE libenigma gtest gmock)

add_dependencies(${PROJECT_NAME} libenigma)
