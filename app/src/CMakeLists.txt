file(GLOB_RECURSE SOURCES *.cpp)

add_executable(cpp_log ${SOURCES})

target_include_directories(cpp_log PRIVATE ../include)
