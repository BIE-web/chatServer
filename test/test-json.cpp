#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

// json序列化示例
string func() {
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "lisi";
    js["msg"] = "Hello, what are you doing?";
    string sendBuf = js.dump();
    std::cout << sendBuf.c_str() << std::endl;
    return sendBuf;
}
// json序列化示例
void func2() {
    json js;
    js["msg"]["zhangsan"] = "hello,i am zhang san";
    js["nums"] = {1, 2, 3, 4, 5};
    map<int, string> m;
    m.insert({1, "nihao1"});
    m.insert({2, "nihao2"});
    m.insert({3, "nihao3"});
    js["path"] = m;
    string buf = js.dump();
    cout << buf << endl;
}

// int main() {
//     string recvBuf = func();
//     json buf = json::parse(recvBuf);
//     cout << buf["msg"] << endl;
    
// }
