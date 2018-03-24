cmake_minimum_required(VERSION 2.6)
project(yuvtransform)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(base_dir ${CMAKE_CURRENT_LIST_DIR})

set(src_dir ${base_dir})

file(GLOB src_files ${src_dir}/*.cpp)

add_executable(yuvtransform ${src_files})

target_link_libraries(yuvtransform GL GLEW glfw)

#install(TARGETS yuvtransform RUNTIME DESTINATION bin)
