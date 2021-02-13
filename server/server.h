
#include <unordered_map>
#include "../http/http_conn.h"
#include "../epoll/epoller.h"
#include "../pool/threadPool.h"
#include "../timer/time_heap.h"
#include <sys/socket.h>
#define MAX_EVENTS 65535
class Web_server
{
public:
    void init(sockaddr_in &addr);
    void run();
    void del(int &fd);
    bool isclose;

private:
    void deal_listen();
    void deal_write(int fd);
    void deal_read(int fd);
    void close_(int fd);
    void add_fd(int &fd, uint32_t &events);
    int set_noblocking_fd(int &fd);
    sockaddr_in addr_;
    int epollfd_;
    uint32_t listen_event;
    uint32_t connfd_event;
    Epoller epoller;
    int sockfd_;
    ThreadPool *tp_;
    Timer_heap *timer_heap_;
    //Http_conn hc;
    std::unordered_map<int, Timer *> timers;
    std::unordered_map<int, Http_conn *> users;
    //std::unordered_map<int, Timer *> times;
};
