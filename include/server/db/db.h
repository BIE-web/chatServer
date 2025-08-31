#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

#include <string>

//数据库操作类
class MySql {
   public:
    //初始化数据库连接
    MySql();

    // 释放数据库连接资源
    ~MySql();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(std::string sql);

    // 查询操作
    MYSQL_RES* query(std::string sql);

    //获取连接
    MYSQL* getConnection();

   private:
    MYSQL* _conn;
};

#endif