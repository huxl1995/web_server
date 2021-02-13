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
    void init(Http_infos &http_infos);
    void process();

    void make_response(iovec *iv, int &iv_count);

private:
    void process_GET();
    void process_POST();
    void process_error();
    void open_file(std::string &file_name);
    Http_infos http_infos_;
    int code;
    struct stat mystat;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    std::string source = "./sources";
    char *mm_file;
    struct stat mm_file_stat;
    Mysql_conn_pool *mcp;
    //std::unordered_map<int, std::string> status_code;
};
#endif