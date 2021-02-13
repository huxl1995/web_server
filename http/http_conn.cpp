#include "http_conn.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
Http_conn::Http_conn()
{
    read_buf = new char[BUFFER_SIZE];
    memset(read_buf, '\0', BUFFER_SIZE);
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
    //memset(read_buf, '\0', sizeof(read_buf));
    len = read(fd_, read_buf, BUFFER_SIZE);
    //printf(read_buf);
    //printf("\n");
    return len;
}

void Http_conn::process()
{
    //m_.lock();、
    reset();
    read_data();
    request.parse(read_buf);
    http_infos = request.get_http_infos();
    response.init(http_infos);
    response.process();
    response.make_response(iv, iv_count);

    //write();
    //reset();
    //m_.unlock();
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
    memset(read_buf, '\0', BUFFER_SIZE * sizeof(char));
    memset(iv, 0, sizeof(iv));
    iv_count = 0;
    http_infos.method = Http_infos::METHOD::GET;
    http_infos.file_name = "";
    http_infos.head_state.clear();
    http_infos.password = "";
    http_infos.username = "";
    request.reset();
}