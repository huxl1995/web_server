#ifndef EPOLLER_H
#define EPOLLER_H
#include <sys/epoll.h>
class Epoller
{
public:
    Epoller(int max_events = 1024);
    ~Epoller();
    void add_fd(int &fd, uint32_t &event);
    void mod_fd(int &fd, uint32_t &event);
    void del_fd(int &fd);
    int wait(int timeout_ms = -1);
    epoll_event *get_epoll_event(int i);

private:
    int epoll_fd;
    epoll_event *events;
    int max_events_;
};
#endif