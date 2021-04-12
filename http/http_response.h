#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <unordered_map>
#include <sys/stat.h>
#include <sys/uio.h>
#include <string>
#include "../pool/mysql_pool.h"
//#include "../buffer/buffer.h"
#include "http_infos.h"
class Http_response
{
public:
    Http_response();
    ~Http_response();
    void init(Http_infos &http_infos, int connfd_);
    void process();
    void make_response(iovec *iv, int &iv_count);
    void make_response();
    void write_response();

private:
    void process_GET();
    void process_POST();
    void process_error();
    void open_file(std::string &file_name);
    void unmap();
    int connfd;
    Http_infos http_infos_;
    int code;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    Mysql_conn_pool *mcp;

private:
    std::string source = "./sources";
    char head[1024];
    char *mm_file;
    struct stat mystat;
    struct iovec iv[2];
    int ivcount;
};
#endif