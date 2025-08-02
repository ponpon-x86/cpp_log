file(GLOB_RECURSE SOURCES *.cpp)

add_executable(socket_app ${SOURCES})

target_include_directories(socket_app PRIVATE ../include)
