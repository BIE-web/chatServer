#ifndef USER_H
#define USER_H

#include <string>

// User表的ORM类
class User 
{
public:
    //构造函数
    User(int id = -1,std::string name = "",std::string pwd = "",std::string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }

    //设置id
    void setId(int id)
    {
        this->id = id;
    }

    //设置名称
    void setName(std::string name)
    {
        this->name = name;
    }

    //设置密码
    void setPassword(std::string pwd)
    {
        this->password = pwd;
    }

    //设置状态
    void setState(std::string state)
    {
        this->state = state;
    }

    //获得相应的成员变量
    int getId(){return this->id;}
    std::string getName(){return this->name;}
    std::string getPassword(){return this->password;}
    std::string getState(){return this->state;}

private:
    int id;
    std::string name;
    std::string password;
    std::string state;
};

#endif