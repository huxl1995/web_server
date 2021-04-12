#include "http_conn.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
Http_conn::Http_conn()
{

    memset(read_buf, '\0', BUFFER_SIZE);
}
Http_conn::~Http_conn()
{
    //delete[] read_buf;
}
void Http_conn::run(int sockfd)
{
    reset();
    init(sockfd);
    process();
}
// void Http_conn::run(int sockfd, const sockaddr_in &addr)
// {
//     reset();
//     init(sockfd, addr);
//     process();
// }
void Http_conn::init(const int sockfd, const sockaddr_in &addr)
{
    assert(sockfd > 0);
    fd_ = sockfd;
    addr_ = addr;
}
void Http_conn::init(const int sockfd)
{
    fd_ = sockfd;
}
ssize_t Http_conn::read_data()
{
    ssize_t len = -1;
    //memset(read_buf, '\0', sizeof(read_buf));
    len = read(fd_, read_buf, BUFFER_SIZE);
    //printf("%s\n", read_buf);
    return len;
}

void Http_conn::process()
{
    //m_.lock();、
    reset();
    read_data();
    request.parse(read_buf);
    http_infos = request.get_http_infos();
    response.init(http_infos, fd_);
    response.process();
    //response.make_response(iv, iv_count);
    response.make_response();
    //write();
    //reset();
    //m_.unlock();
    return;
}

ssize_t Http_conn::write_()
{
    response.write_response();
    //ssize_t len = -1;
    //len = writev(fd_, iv, iv_count);
    return 0;
}

void Http_conn::closes()
{
    close(fd_);
    memset(&addr_, 0, sizeof(addr_));
}

void Http_conn::reset()
{
    // if (read_buf)
    // {
    //     memset(read_buf, '\0', sizeof(read_buf));
    // }

    //memset(iv, 0, sizeof(iv));
    //iv_count = 0;
    http_infos.method = Http_infos::METHOD::GET;
    http_infos.file_name = "";
    http_infos.head_state.clear();
    http_infos.password = "";
    http_infos.username = "";
    request.reset();
}
void Http_conn::init_test(const int sockfd)
{
    fd_ = sockfd;
}
void Http_conn::read_test()
{
    char readbuf[BUFFER_SIZE];
    memset(readbuf, 0, BUFFER_SIZE);
    read(fd_, readbuf, BUFFER_SIZE);
    //printf("%s\n", readbuf);
    //delete[] readbuf;
}
void Http_conn::write_test()
{
    char writebuf[BUFFER_SIZE];
    memset(writebuf, 0, BUFFER_SIZE);
    sprintf(writebuf, "%s\n", "helloword");
    write(fd_, writebuf, BUFFER_SIZE);
    // printf("%s\n", "write over");
    //delete[] writebuf;
}