#include "http_conn.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
Http_conn::Http_conn()
{
    read_buf = new char[BUFFER_SIZE];
}
Http_conn::~Http_conn()
{
    delete read_buf;
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
    len = read(fd_, read_buf, BUFFER_SIZE);
    return len;
}

void Http_conn::process()
{
    read_data();
    request.parse(read_buf);
    Http_infos hi = request.get_http_infos();
    response.init(hi);
    response.process();
    response.make_response(iv, iv_count);
    write();
}

ssize_t Http_conn::write() const
{
    ssize_t len = -1;
    len = writev(fd_, iv, iv_count);
    return len;
}

void Http_conn::closes()
{
    close(fd_);
    memset(&addr_, 0, sizeof(addr_));
}

void Http_conn::reset()
{
    memset(read_buf, '\0', sizeof(read_buf));
    memset(iv, 0, sizeof(iv));
    iv_count = 0;
}