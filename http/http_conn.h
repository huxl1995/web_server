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
    void init(const int sockfd, const sockaddr_in &addr);
    void process();
    //void run(const int sockfd, const sockaddr_in &addr);
    void run(const int sockfd);
    ssize_t write_();
    Http_infos http_infos;
    void read_test();
    void write_test();
    void init_test(const int sockfd);

private:
    ssize_t read_data();
    void reset();
    void closes();
    std::mutex m_;
    int fd_;
    sockaddr_in addr_;
    bool isclose_;
    char read_buf[BUFFER_SIZE];

    Http_request request;
    Http_response response;
};

#endif
