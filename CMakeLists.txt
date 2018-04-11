cmake_minimum_required(VERSION 2.8)
project(c_plus_example)

set(CMAKE_CXX_STANDARD 11)

#SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_BUILD_TYPE Debug)

#配置编译选项
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -Wall")
#配置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive -pthread -DMG_ENABLE_THREADS")

#配置头文件目录
include_directories(
        include
)

set(SRC
        main.cpp
)

add_executable(c_plus_example
        ${SRC}

        )

# 可以对源代码中宏定义进行声明
#add_definitions("-DCMCC")

link_directories("/usr/local/lib/")
link_directories(/usr/lib/x86_64-linux-gnu)


set(LIBRARIES
    m pthread
)

target_link_libraries(c_plus_example ${LIBRARIES})