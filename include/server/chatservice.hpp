#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "offlinemessagemodel.hpp"
#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"

using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;

using namespace std::placeholders;

//表示处理消息的回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &, json &, Timestamp)>;

//聊天服务器业务类
class ChatService {
   public:
    //获取单例对象的接口函数
    static ChatService *instance();

    //处理登陆业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //处理服务器异常,业务重置方法
    void reset();

    //一对一聊天业务方法
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //创建群组
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //加入群组
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //群组聊天
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //用户登出
    void logout(const TcpConnectionPtr &conn, json &js, Timestamp time);

   private:
    ChatService();                                       //单例模式
    std::unordered_map<int, MsgHandler> _msgHandlerMap;  //存储消息id和其对应的业务处理方法

    UserModel _usermodel;                                //数据库操作类
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    std::unordered_map<int, TcpConnectionPtr> _userConnMap; //存储在线用户的通信连接
    std::mutex _connMutex; //互斥锁,用来保证_userConnMap的线程安全问题

};

#endif