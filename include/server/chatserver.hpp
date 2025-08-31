#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>



using namespace muduo;
using namespace muduo::net;

//聊天服务器主类
class ChatServer 
{
public:
    //构造函数,初始化聊天服务器对象
    ChatServer(EventLoop* loop, //
            const InetAddress& listenAddr,
            const string& nameArg);

    //start，启动服务
    void start();
private:
    //上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr& );

    //上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr& conn,Buffer* buffer,Timestamp time);

    TcpServer _server;//结合muduo库实现服务器功能的类对象
};

#endif