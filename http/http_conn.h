#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <arpa/inet.h>
#include <sys/uio.h>
#include <mutex>
#include "./http_request.h"
#include "./http_response.h"

class Http_conn
{
public:
    Http_conn();
    ~Http_conn();
    void init(const int sockfd);
    void process();
    //void run(const int sockfd, const sockaddr_in &addr);
    void run(const int sockfd);

private:
    void init(const int sockfd, const sockaddr_in &addr);

    ssize_t read_data();
    ssize_t write() const;

    void reset();
    void closes();
    std::mutex m_;
    int fd_;
    sockaddr_in addr_;
    bool isclose_;
    char *read_buf;
    struct iovec iv[2];
    int iv_count;
    Http_request request;
    Http_response response;
};

#endif
