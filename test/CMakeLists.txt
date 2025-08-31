

#设置可执行文件最终存储路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
# link_directories()

#设置需要编译的源文件列表
set(SRC_LIST ./test-muduo.cpp)

#把.指定路径下的所有源文件名字放入变量名SRC_LIST
aux_source_directory(. SRC_LIST)

#表示生成可执行文件server，有SRC_LIST变量所定义的源文件编译而来
add_executable(server test-muduo.cpp)

#表示server这个目标程序需要链接下面这三个库
target_link_libraries(server muduo_net muduo_base pthread)