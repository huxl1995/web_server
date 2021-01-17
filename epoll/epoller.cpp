#include "epoller.h"
#include <fcntl.h>
Epoller::Epoller(int max_events)
{
    epoll_fd = epoll_create(5);
    max_events_ = max_events;
    events = new epoll_event[max_events_];
}
Epoller::~Epoller()
{
    delete[] events;
}
void Epoller::add_fd(int &fd, uint32_t &event)
{
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = event;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

void Epoller::mod_fd(int &fd, uint32_t &event)
{
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = event;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void Epoller::del_fd(int &fd)
{
    epoll_event ev = {0};
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}
int Epoller::wait(int timeoutms)
{
    return epoll_wait(epoll_fd, events, max_events_, timeoutms);
}

epoll_event *Epoller::get_epoll_event(int i)
{
    return &events[i];
}