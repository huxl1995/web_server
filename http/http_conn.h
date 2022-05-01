#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <arpa/inet.h>
#include <sys/uio.h>
#include <mutex>
#include <sys/uio.h>
#include <sys/stat.h>
#include <string.h>
//#include "./http_request.h"
//#include "./http_response.h"
#include "./http_base.h"
#include "../pool/mysql_pool.h"
class Http_conn
{
public:
    Http_conn():fd_(0),isclose_(false),read_idx_(0),check_idx_(0),start_line_(0),check_state_(CHECK_STATE_REQUESTLINE),read_buf(new char[BUFFER_SIZE])
    {
        mcp = Mysql_conn_pool::instance();
        memset(read_buf,0,BUFFER_SIZE);
    };
    ~Http_conn(){delete[] read_buf;};
    //void init(const int sockfd);
    void init(const int sockfd, const sockaddr_in &addr,std::string sources);
    void process();
    void run(const int sockfd,const sockaddr_in& addr,std::string sources);
    bool write_();
    Http_infos http_infos_;
    /*void read_test();
    void write_test();
    void init_test(const int sockfd);*/

private:
    LINE_STATUS parse_line();
    PARSE_RESULT parse_requestline(const char *temp);
    PARSE_RESULT parse_headline(const char *temp);
    PARSE_RESULT parse_bodyline(const char *temp);
    PARSE_RESULT parse_content();
    CHECK_STATE check_state_;
    int read_idx_;
    int check_idx_;
    int start_line_;
private:
    //void write_response();
    void make_response(iovec *iv, int &iv_count);
    void make_response();
    void process_GET();
    void process_POST();
    void process_error();
    void open_file(std::string &file_name);
    void unmap();
    void process_response();
    //int connfd;
    int code;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    Mysql_conn_pool *mcp;
    std::string source_;
    char head[1024];
    char *mm_file;
    struct stat mystat;
    struct iovec iv[2];
    int ivcount;
    int size_to_response;

private:
    bool read_data();
    void reset();
    void closes();
    //std::mutex m_;
    int fd_;
    sockaddr_in addr_;
    bool isclose_;
    char* read_buf;

    
};

#endif
