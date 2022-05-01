#include "server.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <functional>
#include <unistd.h>
#include <iostream>
#include <signal.h>
using namespace std;
bool Web_server::is_stop = false;

void Web_server::init(sockaddr_in &addr,int threads_num)
{
    tp_ = new ThreadPool(threads_num);
    timer_heap_ = new Timer_heap(1024);
    addr_ = addr;
    epollfd_ = epoll_create(5);
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr));
    assert(sockfd_ > 0);
    int ret = listen(sockfd_, 5);
    assert(ret != -1);
    listen_event = EPOLLRDHUP | EPOLLIN;
    connfd_event = EPOLLONESHOT | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    add_fd(sockfd_, listen_event);
    signal(SIGHUP,stop);
    signal(SIGINT,stop);
}
void Web_server::del(int &fd)
{
    epoller.del_fd(fd);
}
void Web_server::run()
{

    while (!Web_server::is_stop)
    {
        timer_heap_->tick();

        int ret = epoller.wait(0);
        if (ret <= 0)
        {
            sleep(0.1);
        }
        for (int i = 0; i < ret; i++)
        {
            epoll_event *ev = epoller.get_epoll_event(i);
            int fd = ev->data.fd;
            uint32_t event = ev->events;

            if (fd == sockfd_)
            {
                deal_listen();
            }
            else if (event & EPOLLRDHUP || event & EPOLLHUP)
            {
                auto task = std::bind(&Web_server::close_, this, fd);
                tp_->add(std::function<void()>(task));
                //close_(fd);
            }
            else if (event & EPOLLIN)
            {

                auto task = std::bind(&Web_server::deal_read, this, fd);
                tp_->add(std::function<void()>(task));
            }
            else if (event & EPOLLOUT)
            {

                auto task = std::bind(&Web_server::deal_write, this, fd);
                tp_->add(std::function<void()>(task));
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
    if (connfd < 0)
    {
        perror("accept");
        close(connfd);
        return;
    }
    mtx.lock();
    if(users.find(connfd)!=users.end())
    {
        close(connfd);
        mtx.unlock();
        return;
    }
    users[connfd].init(connfd,addr,sources_);
    mtx.unlock();
    uint32_t events = connfd_event | EPOLLIN;
    add_fd(connfd, events);
    
}
void Web_server::deal_read(int fd)
{
    mtx.lock();
    Http_conn* p_http_conn=nullptr;
    if(users.find(fd)!=users.end())
    {
        p_http_conn=&users[fd];
    }
    mtx.unlock();
    if(p_http_conn!=nullptr)
    {
        p_http_conn->process();

        if (p_http_conn->http_infos_.head_state.find("Connection") != p_http_conn->http_infos_.head_state.end() && p_http_conn->http_infos_.head_state["Connection"] == "keep-alive")
        {
            if (timers.find(fd) != timers.end())
            {
                timers[fd]->expire = time(NULL) + 5;
                timer_heap_->adjust(timers[fd]);
            }
            else
            {
                Timer *p = nullptr;

                try
                {
                    p = new Timer;
                }
                catch (bad_alloc &e)
                {
                    cout << e.what() << endl;
                }
                timers[fd] = p;
                timers[fd]->init(5);
                timers[fd]->cb_func = function<void()>(bind(&Web_server::close_, this, fd));
                timer_heap_->add_timer(timers[fd]);
            }
        }
        reset_oneshot(fd);
    }
}
void Web_server::deal_write(int fd)
{   mtx.lock();
    Http_conn* p=nullptr;
    if(users.find(fd)!=users.end())
    {
        p=&users[fd];
    }
    mtx.unlock();
    bool write_res=false;
    if(p!=nullptr)
    {
        write_res=p->write_();
    }
    write_res = users[fd].write_();
    if(!write_res)
    {
        if(errno==EAGAIN){
            reset_oneshot_write(fd);
        }
        else
        {
            close_(fd);
        }
        return;
    }
    if(timers.find(fd)==timers.end())
    {
        //close_(fd);
    }
    else
    {
        reset_oneshot_read(fd);
    }
}
void Web_server::close_(int fd)
{

    close(fd);

    epoller.del_fd(fd);
    if(timers.find(fd)!=timers.end())
    {
        timers[fd]->cb_func = NULL;
    }
    mtx.lock();
    users.erase(fd);
    mtx.unlock();
}
void Web_server::reset_oneshot_read(int fd)
{
    uint32_t ev = connfd_event | EPOLLIN;
    epoller.mod_fd(fd, ev);
}
void Web_server::reset_oneshot_write(int fd)
{
    uint32_t ev = connfd_event | EPOLLOUT;
    epoller.mod_fd(fd, ev);
}

void Web_server::reset_oneshot(int fd)
{
    uint32_t ev=connfd_event | EPOLLIN|EPOLLOUT;
    epoller.mod_fd(fd,ev);
}

void Web_server::stop(int sig)
{    
    is_stop=true;
}