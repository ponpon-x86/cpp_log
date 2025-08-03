file(GLOB_RECURSE SOURCES *.cpp)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(ultralib ${SOURCES})

target_include_directories(ultralib PUBLIC ../include)
