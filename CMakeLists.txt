cmake_minimum_required(VERSION 3.0)
project(ECALC)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)
#CMAKE_C_FLAGS：gcc编译选项
#CMAKE_CXX_FLAGS：g++编译选项

##LLVM scope
find_package(LLVM REQUIRED CONFIG)
message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
##LLVM scope

# 在CMAKE_CXX_FLAGS编译选项后追加 -std=c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O0")
set(CMAKE_CXX_STANDARD 14)

#CMAKE_BUILD_TYPE：编译类型(Debug, Release)
# 设定编译类型为debug，因为在调试时需要选择debug
set(CMAKE_BUILD_TYPE Debug)
# 设定编译类型为release，因为在发布时需要选择release
#set(CMAKE_BUILD_TYPE release)

add_compile_options("-D_DEBUG")
add_definitions(${LLVM_DEFINITIONS})  ##LLVM SCOPE

#定义SRC变量，其值为main.cpp hello.cpp
#set(SRC ./src)

# 将/usr/include/myincludefolder 和 ./include 添加到头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/../include ./include)
include_directories(${CMAKE_SOURCE_DIR}/src/3rd/include ./include)
include_directories(${LLVM_INCLUDE_DIRS}) ##LLVM SCOPE

# 将/usr/lib/mylibfolder 和 ./lib 添加到库文件搜索路径
link_directories(/usr/local/lib64 ./lib)

# 通过变量 SRC 生成 libhello.so 共享库
# SHARED代表动态库，STATIC代表静态库
#add_library(hello SHARED ${SRC})

# 添加编译参数 -wall -std=c++11
#add_compile_options(-wall -std=c++11 -o2)

# 编译main.cpp生成可执行文件main
#add_executable(main main.cpp ${SRC})



# 添加src子目录，src中需要有一个CMakeLists.txt
#add_subdirectory(src)

#语法：aux_source_directory(文件夹路径 变量名)
# 定义SRC变量，其值为当前目录下所有的源代码文件
aux_source_directory(./src SRC)
aux_source_directory(./src/3rd/uuid UUID)
aux_source_directory(./src/Algorithm ALGORITHM)
aux_source_directory(./src/emisc EMISC)
aux_source_directory(./src/frames FRAMES)
# 编译SRC变量所代表的源代码文件，生成main可执行文件
add_executable(main main.cpp test.cpp ${SRC} ${EMISC} ${ALGORITHM} ${UUID} ${FRAMES})

# 将hello动态库文件链接到可执行文件main
# libevent链接到主程序
set(LINK_LIBRARY "event.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "z.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "curl.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "crypto.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "hiredis.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "spdlog.a")
set(LINK_LIBRARY ${LINK_LIBRARY} "pthread")
set(LINK_LIBRARY ${LINK_LIBRARY} "dl")


target_link_libraries(main ${LINK_LIBRARY})

