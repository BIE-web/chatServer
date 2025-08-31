#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"

#include <vector>
#include <string>

// Group表的ORM类
class Group 
{
public:
    //构造函数
    Group(int id = -1, std::string name = "", std::string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
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

    //设置状态
    void setDesc(std::string desc)
    {
        this->desc = desc;
    }

    //获得相应的成员变量
    int getId(){return this->id;}
    std::string getName(){return this->name;}
    std::string getDesc(){return this->desc;}
    std::vector<GroupUser>& getUsers(){return this->users;}

private:
    int id;
    std::string name;
    std::string desc;
    std::vector<GroupUser> users;
};

#endif