#ifndef SQL_H
#define SQL_H
#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
class Mysql_conn_pool
{
public:
    static Mysql_conn_pool *instance();
    void free_conn(MYSQL *conn);
    void init(const char *host, int port, const char *user,
              const char *pwd, const char *db_name, int connsize);
    MYSQL *get_conn();
    void close_pool();
    ~Mysql_conn_pool();

private:
    Mysql_conn_pool();

    std::mutex m;
    std::queue<MYSQL *> mysql_pool;
    int max_conn;
    int used_count;
    int free_count;
    sem_t sem_id;
};

#endif