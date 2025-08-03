file(GLOB_RECURSE SOURCES *.cpp)

add_executable(cpp_log ${SOURCES})
target_link_libraries(cpp_log PRIVATE ultralib)

target_include_directories(cpp_log PRIVATE ../include)