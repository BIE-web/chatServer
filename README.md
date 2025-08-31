需要的环境:
mysql
muduo
json
nginx
redis

编译方式:
cd build
rm -rf *
cmake ..
make
如果编译出现问题请检查各个CMakeLists.txt文件

各个文件夹介绍:
bin-可执行文件所在文件夹
src-源码
include-源码(头文件)
build-编译相关中间文件保存路径
test-测试(可不用)
