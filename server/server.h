
#include <unordered_map>
#include "../http/http_conn.h"
#include "../epoll/epoller.h"
#include "../pool/threadPool.h"
#include <sys/socket.h>
#define MAX_EVENTS 65535
class Web_server
{
public:
    void init(sockaddr_in &addr);
    void run();
    void del(int &fd);

private:
    sockaddr_in addr_;
    int set_noblocking_fd(int &fd);
    int epollfd_;
    int sockfd_;
    void add_fd(int &fd, uint32_t &events);
    ThreadPool *tp_;
    Http_conn hc;
    std::unordered_map<int, Http_conn *> hcs;
    Epoller epoller;
};
