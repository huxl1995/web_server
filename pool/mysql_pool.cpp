#include "mysql_pool.h"
#include <assert.h>
Mysql_conn_pool::~Mysql_conn_pool()
{
    close_pool();
}
Mysql_conn_pool::Mysql_conn_pool()
{
}
void Mysql_conn_pool::init(const char *host, int port, const char *user,
                           const char *pwd, const char *db_name, int connsize)
{
    for (int i = 0; i < connsize; i++)
    {
        MYSQL *mysql = nullptr;
        assert(connsize > 0);
        mysql = mysql_init(mysql);
        mysql = mysql_real_connect(mysql, host, user, pwd, db_name, port, nullptr, 0);
        mysql_pool.push(mysql);
    }
    max_conn = connsize;
    sem_init(&sem_id, 0, connsize);
}
MYSQL *Mysql_conn_pool::get_conn()
{
    MYSQL *mysql = nullptr;
    if (mysql_pool.empty())
    {
        return nullptr;
    }
    sem_wait(&sem_id);
    m.lock();
    mysql = mysql_pool.front();
    mysql_pool.pop();
    m.unlock();
    return mysql;
}
void Mysql_conn_pool::free_conn(MYSQL *mysql)
{
    assert(mysql);
    m.lock();
    mysql_pool.push(mysql);
    m.unlock();
    sem_post(&sem_id);
}
Mysql_conn_pool *Mysql_conn_pool::instance()
{

    static Mysql_conn_pool mcp;
    return &mcp;
}
void Mysql_conn_pool::close_pool()
{
    m.lock();
    while (!mysql_pool.empty())
    {
        MYSQL *mysql = mysql_pool.front();
        mysql_pool.pop();
        mysql_close(mysql);
    }
    mysql_library_end();
    m.unlock();
}