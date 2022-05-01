
#include <unordered_map>
#include "../http/http_conn.h"
#include "../epoll/epoller.h"
#include "../pool/threadPool.h"
#include "../timer/time_heap.h"
#include <sys/socket.h>
#include<unistd.h>
#include<iostream>
#define MAX_EVENTS 65535
class Web_server
{
public:
    Web_server(std::string sources):sources_(sources){}
    ~Web_server()
    {
        tp_->close();
        for(auto& pair:timers)
        {   
            delete pair.second;
        }
        delete timer_heap_;
        delete tp_;
    }
    void init(sockaddr_in &addr,int threads_num);
    void run();
    void del(int &fd);
    static bool is_stop;
    static void stop(int sig);

private:
    void deal_listen();
    void deal_write(int fd);
    void deal_read(int fd);
    void close_(int fd);
    void add_fd(int &fd, uint32_t &events);
    int set_noblocking_fd(int &fd);
    sockaddr_in addr_;

private:
    void reset_oneshot_write(int fd);
    void reset_oneshot_read(int fd);
    void reset_oneshot(int fd);
    int epollfd_;
    epoll_event events[MAX_EVENTS];
    uint32_t listen_event;
    uint32_t connfd_event;
    Epoller epoller;

private:
    int sockfd_;
    ThreadPool *tp_;
    Timer_heap *timer_heap_;
    std::unordered_map<int, Timer *> timers;
    std::unordered_map<int, Http_conn> users;
    std::string sources_;
    std::mutex mtx;
};
