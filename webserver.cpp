//创建套接字
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <mutex>
//#include <atomic>
#include <string>
#include <sys/uio.h>
//#include <memory>
#include <fcntl.h>
#include <functional>
#include <vector>
#include <iostream>
#include "./http/http_conn.h"
#include "./pool/threadPool.h"
#include "./server/server.h"
using namespace std;
#define PORT 8080
#define LISTENQ 5
#define MYSQL_NAME "root"
#define MYSQL_PASSWORD "0"
#define TABLE_NAME "userinfo"
//相应报文头

int main()
{

    //int listenfd, connfd;

    sockaddr_in servaddr;

    //准备地址端口信息
    //常量 INADDR_ANY 表示任意网卡，位于netinet/in.h
    bzero(&servaddr, sizeof(servaddr));           //将servaddr前x位设置为0
    servaddr.sin_family = AF_INET;                //服务器地址zu设置为ipv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //服务器地址为本机任意网卡
    servaddr.sin_port = htons(PORT);

    Web_server wb;
    wb.init(servaddr);
    wb.run();

    return 0;
}
