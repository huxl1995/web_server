#include "server.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <functional>
#include <unistd.h>

void Web_server::init(sockaddr_in &addr)
{
    tp_ = new ThreadPool(4);
    addr_ = addr_;
    //epollfd_ = epoll_create(5);
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    //set_noblocking_fd(sockfd_);
    bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr));
    assert(sockfd_ > 0);
    int ret = listen(sockfd_, 5);
    assert(ret != -1);
}
void Web_server::del(int &fd)
{
    epoller.del_fd(fd);
}
void Web_server::run()
{
    uint32_t sockfd_event = EPOLLIN;
    add_fd(sockfd_, sockfd_event);
    //epoll_event *events = new epoll_event[MAX_EVENTS];
    while (1)
    {
        int ret = epoller.wait();
        for (int i = 0; i < ret; i++)
        {
            epoll_event *ev = epoller.get_epoll_event(i);
            int fd = ev->data.fd;
            uint32_t event = ev->events;
            if (fd == sockfd_)
            {
                int connfd = accept(fd, NULL, NULL);
                if (event == EPOLLIN)
                {
                    uint32_t connfd_event = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
                    add_fd(connfd, connfd_event);
                    //Http_conn hc;

                    hcs[connfd] = new Http_conn;
                }
            }
            else
            {
                if (event == EPOLLIN)
                {
                    //hc.init(fd);
                    hcs[fd]->init(fd);
                    auto task = std::bind(&Http_conn::process, hcs[fd]);
                    //task();
                    tp_->add(std::function<void()>(task));
                }
                else if (event == EPOLLHUP || event == EPOLLRDHUP)
                {
                    close(fd);
                    epoller.del_fd(fd);
                }
                else
                {
                    /* code */
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
