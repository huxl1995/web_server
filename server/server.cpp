#include "server.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <functional>
#include <unistd.h>
using namespace std;
void Web_server::init(sockaddr_in &addr)
{
    tp_ = new ThreadPool(4);
    timer_heap_ = new Timer_heap(1024);
    addr_ = addr;
    //epollfd_ = epoll_create(5);
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    //set_noblocking_fd(sockfd_);
    bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr));
    assert(sockfd_ > 0);
    int ret = listen(sockfd_, 5);
    assert(ret != -1);
    listen_event = EPOLLRDHUP | EPOLLIN;
    connfd_event = EPOLLONESHOT | EPOLLRDHUP;
    add_fd(sockfd_, listen_event);
}
void Web_server::del(int &fd)
{
    epoller.del_fd(fd);
}
void Web_server::run()
{
    // uint32_t sockfd_event = EPOLLIN;
    // add_fd(sockfd_, sockfd_event);
    //epoll_event *events = new epoll_event[MAX_EVENTS];
    while (1)
    {
        timer_heap_->tick();
        //printf("before wait\n");
        int ret = epoller.wait();
        //printf("after wait\n");
        for (int i = 0; i < ret; i++)
        {
            epoll_event *ev = epoller.get_epoll_event(i);
            int fd = ev->data.fd;
            uint32_t event = ev->events;
            if (fd == sockfd_)
            {
                deal_listen();
            }
            else
            {
                if (event == EPOLLIN)
                {

                    //printf("EPOLLIN:%d\n", fd);
                    auto task = std::bind(&Web_server::deal_read, this, fd);
                    tp_->add(std::function<void()>(task));
                    //task();
                }
                else if (event == EPOLLOUT)
                {
                    //printf("EPOLLOUT:%d\n", fd);
                    auto task = std::bind(&Web_server::deal_write, this, fd);
                    //task();
                    tp_->add(std::function<void()>(task));
                }
                else
                {
                    close_(fd);
                    timer_heap_->del_timer(timers[fd]);
                }
            }
        }
    }
}
int Web_server::set_noblocking_fd(int &fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
void Web_server::add_fd(int &fd, uint32_t &events)
{

    epoller.add_fd(fd, events);
    set_noblocking_fd(fd);
}
void Web_server::deal_listen()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = accept(sockfd_, (struct sockaddr *)&addr, &len);
    users[connfd] = new Http_conn;
    users[connfd]->init(connfd, addr);
    uint32_t events = connfd_event | EPOLLIN;
    add_fd(connfd, events);
    //
    timers[connfd] = new Timer(5);
    //timers[connfd]->cb_func = function<void()>(bind(&Web_server::close_, this, connfd));
    timer_heap_->add_timer(timers[connfd]);
}
void Web_server::deal_read(int fd)
{
    users[fd]->process();
    uint32_t events = connfd_event | EPOLLOUT;
    epoller.mod_fd(fd, events);
    //
    if (users[fd]->http_infos.head_state.find("Connection") != users[fd]->http_infos.head_state.end() && users[fd]->http_infos.head_state["Connection"] == "keep-alive")
    {
        timers[fd]->expire = time(NULL) + 5;
        timer_heap_->adjust(timers[fd]);
        // printf("alive\n");
    }
    else
    {
        timers[fd]->expire = time(NULL);
        timer_heap_->adjust(timers[fd]);
        //printf("Connetcion: close\n");
    }
}
void Web_server::deal_write(int fd)
{
    users[fd]->write();
    uint32_t events = connfd_event | EPOLLIN;
    epoller.mod_fd(fd, events);
    //close_(fd);
}
void Web_server::close_(int fd)
{
    //printf("main colse fd :%d,%d\n", fd, time(NULL));
    close(fd);
    epoller.del_fd(fd);
    users.erase(fd);
}