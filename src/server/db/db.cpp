#include "db.h"

#include <muduo/base/Logging.h>

using namespace muduo;

//数据库配置信息
static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "041222";
static std::string dbname = "chat";

//初始化数据库连接
//初始化数据库连接
MySql::MySql()
{
    _conn = mysql_init(nullptr);
}

//释放数据库连接资源
MySql::~MySql() 
{
    if (_conn != nullptr) {
        mysql_close(_conn);
    }
}

//连接数据库
bool MySql::connect() 
{
    MYSQL* p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr) 
    {
        // c和c++代码默认的编码字符是ASCII，如果不设置，从MySql上拉下来的中文显示问号
        mysql_query(_conn, "set names gbk");
        LOG_INFO << " connect mysql success !!! ";
    }
    else
    {
        LOG_INFO << " connect mysql failed !!! ";
    }
    return p;
}

//更新操作
bool MySql::update(std::string sql) 
{
    if (mysql_query(_conn, sql.c_str())) 
    {
        LOG_INFO << __FILE__ << " : " << __LINE__ << ":" << sql << "  更新失败!!!  ";
        return false;
    }
    return true;
}

//查询操作
MYSQL_RES* MySql::query(std::string sql) 
{
    if (mysql_query(_conn, sql.c_str())) 
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败!!!!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

//获取连接
MYSQL* MySql::getConnection()
{
    return _conn;
}