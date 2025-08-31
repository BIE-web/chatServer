#include "chatservice.hpp"
#include "public.hpp"
#include "user.hpp"

#include "map"
#include <vector>
#include <muduo/base/Logging.h>

using namespace muduo;

//获取单例对象的接口函数
ChatService *ChatService::instance() 
{
    static ChatService service;
    return &service;
}

// 单例模式
// 注册消息以及对应的处理方法
ChatService::ChatService() 
{
    // 登陆操作的回调函数
    _msgHandlerMap.insert(
        {LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});

    // 注册事件的回调函数绑定
    _msgHandlerMap.insert(
        {REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});

    //一对一
    _msgHandlerMap.insert(
        {ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});

    //添加好友
    _msgHandlerMap.insert(
        {ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    //创建群组
    _msgHandlerMap.insert(
        {CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});

    //加入群组
    _msgHandlerMap.insert(
        {ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});

    //群组聊天
    _msgHandlerMap.insert(
        {GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    //用户登出
    _msgHandlerMap.insert(
        {LOGOUT_MSG, std::bind(&ChatService::logout, this, _1, _2, _3)});
}

// 处理登陆业务 id pwd
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) 
{
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = _usermodel.query(id);
    if(user.getId()==id &&user.getPassword()==pwd)
    {
        if(user.getState()=="online")
        {
            //用户已经登陆，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已经登陆，请重新输入账号";
            conn->send(response.dump());
        }
        else
        {
            {
                //登陆成功记录用户连接信息
                std::lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }
            // 登陆成功，更新用户状态信息 state offline-》online
            user.setState("online");
            _usermodel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            //查询用户是否有离线消息
            std::vector<std::string> vec = _offlineMsgModel.query(user.getId());
            if(!vec.empty())
            {
                response["offlinemsg"] = vec;
                //读取该用户的离线消息之后,把用户的离线消息删除掉
                _offlineMsgModel.remove(user.getId());
            }


            //查询用户的好友信息并返回
            std::vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                std::vector<std::string> vec2;
                for(User &u:userVec)
                {
                    json js;
                    js["id"] = u.getId();
                    js["name"] = u.getName();
                    js["state"] = u.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            // 查询群组信息并返回
            std::vector<Group> groupvec = _groupModel.queryGroups(id);
            if(!groupvec.empty())
            {
                std::vector<std::string> vec3;
                for (Group &group : groupvec) 
                {
                    json js;
                    js["id"] = group.getId();
                    js["name"] = group.getName();
                    js["desc"] = group.getDesc();
                    vec3.push_back(js.dump());
                }
                response["groups"] = vec3;
            }

            conn->send(response.dump());
        }
        
    }
    else
    {
        //登陆失败
        //用户不存在，用户存在但是密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误";
        conn->send(response.dump());
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) 
{
    // LOG_INFO << "do reg service!";
    std::string name = js["name"];
    std::string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPassword(pwd);
    bool state = _usermodel.insert(user);
    if(state)
    {
        //注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } 
    else {
        //注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid) 
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) 
    {
        //返回一个默认的处理器
        return [=](const TcpConnectionPtr &, json &, Timestamp) {
            LOG_ERROR << "msgid " << msgid << " can not find handler!!! ";
        };
    } else {
        return _msgHandlerMap[msgid];
    }
}

//处理服务器异常,业务重置方法
void ChatService::reset()
{
    //把online状态的用户,设置成offline
    _usermodel.resetState();
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end();++it)
        {
            if(it->second == conn)
            {
                //从map表删除用户的连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
        //lock_guard出作用域解锁
    }
    //更新用户状态,下线
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _usermodel.updateState(user);
    }
}


//一对一聊天业务方法
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toId = js["to"].get<int>();
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if(it != _userConnMap.end())
        { 
            //在线, 转发消息, 服务器主动推送消息给toId用户
            it->second->send(js.dump());
            return;
        }
    }
    //toid不在线,存储离线消息
    _offlineMsgModel.insert(toId, js.dump());
}

//添加好友 msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    _friendModel.insert(userid, friendid);
}

//创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    //存储新创建的群组信息
    Group group(-1, name, desc);
    if(_groupModel.createGroup(group))
    {
        //存储群组创建者的信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
    else
    {
        LOG_ERROR << " create group : " << name << "fault !!! ";
    }
}

//加入群组
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

//群组聊天
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> userIdVec;
    userIdVec = _groupModel.queryGroupUsers(id, groupid);
    std::lock_guard<std::mutex> lock(_connMutex);
    for(int id : userIdVec)
    {
        auto it = _userConnMap.find(id);
        if(it!=_userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            _offlineMsgModel.insert(id, js.dump());
        }
    }
}


void ChatService::logout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end();++it)
        {
            if(it->second == conn)
            {
                //从map表删除用户的连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
        //lock_guard出作用域解锁
    }
    //更新用户状态,下线
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _usermodel.updateState(user);
    }
}