/*
    muduo网络库给用户提供了两个主要的类
    TcpServer用于编写服务器程序
    TcpClient用于编写客户端程序

    epoll+线程池
    好处：能够把网络IO的代码和业务区分开
    业务代码的暴露：用户的连接和断开，用户的可读写事件
*/

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <functional>
#include <iostream>
#include <string>

using namespace ::muduo;
using namespace muduo::net;
// using namespace placeholders;
using namespace std;

/*
基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop时间循环的指针
3.明确TcpServer构造函数需要什么参数
4.在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配IO线程和work线程
*/
class ChatServer {
   public:
    ChatServer(EventLoop* loop,                //事件循环
               const InetAddress& listenAddr,  // IP地址加端口
               const string& nameArg)          //服务器的名字
        : _server(loop, listenAddr, nameArg), _loop(loop) 
    {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection, this, placeholders::_1));


        //给服务器用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));


        //设置服务器端的线程数量
        _server.setThreadNum(4);

    }

    //开启事件循环
    void start() {
        _server.start();
    }

   private:

    //专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr& conn) 
    {
        if (conn->connected()) 
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "status::online" << endl;
        } 
        else 
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "status::offline" << endl;
            conn->shutdown();
            // _loop->quit();
        }
    }


    void onMessage(const TcpConnectionPtr& conn,  //连接
                   Buffer* buffer,                //缓冲区
                   Timestamp time)                //事件
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        conn ->send(buf);
    }


    TcpServer _server;
    EventLoop* _loop;  //可以看做epoll事件循环
};

int main()
{
    EventLoop loop;    //epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();    //listenfd_epoll_ctl=>epoll
    loop.loop();    //epoll_wait一阻塞方式等待新用户的连接，已连接用户的读写事件

    return 0;
}