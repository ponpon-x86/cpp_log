file(GLOB_RECURSE SOURCES *.cpp)

add_library(ultralib ${SOURCES})

target_include_directories(ultralib PUBLIC ../include)
