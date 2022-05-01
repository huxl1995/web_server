//创建套接字
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
//#include <mutex>
//#include <atomic>
#include <string>
#include <sys/uio.h>
//#include <memory>
#include <fcntl.h>
#include <functional>
#include <vector>
#include <iostream>
#include<filesystem>
#include "pool/mysql_pool.h"
#include "./http/http_conn.h"
#include "./pool/threadPool.h"
#include "./server/server.h"
using namespace std;
#define PORT 8080
#define LISTENQ 5
#define MYSQL_NAME "test"     //数据库账户名
#define MYSQL_PASSWORD "0"    //数据库密码
#define TABLE_NAME "userinfo" //数据库名（其中需要包含一个名为userinfo的表）
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        cout<<"参数数量错误"<<endl;
        return 1;
    }
    string source=string(argv[1]);
    int threads=atoi(argv[2]);
    sockaddr_in servaddr;
    //准备地址端口信息
    //常量 INADDR_ANY 表示任意网卡，位于netinet/in.h
    bzero(&servaddr, sizeof(servaddr));           //将servaddr前x位设置为0
    servaddr.sin_family = AF_INET;                //服务器地址zu设置为ipv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //服务器地址为本机任意网卡
    servaddr.sin_port = htons(PORT);

    Mysql_conn_pool *mcp = Mysql_conn_pool::instance();
    mcp->init("localhost", 3306, MYSQL_NAME, MYSQL_PASSWORD, TABLE_NAME, 1024);
    Web_server wb(source);
    wb.init(servaddr,threads);
    wb.run();

    return 0;
}
