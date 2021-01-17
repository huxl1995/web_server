#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include <unordered_map>
#include <string>
#include "http_infos.h"
//#include "../buffer/buffer.h"
#define BUFFER_SIZE 4096
//主状态机的两种可能状态，分别表示：当前正在分析请求行，当前正在分析头部字段
enum CHECK_STATE
{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_BODY
};
//从状态机的三种可能状态，即行的读取状态，分别表示，读取到一个完整的行，行出错和行数据尚且不完整
enum LINE_STATUS
{
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN
};
//服务器处理HTTP请求的结果：NO_REQUEST表示请求不完整，需要继续读取客户数据：GET_REQUEST表示获得了一个完整的客户请求；BAD_REQUEST表示客户请求有语法错误；FORBIDDEN_REQUEST表示
//客户对资源没有足够的访问权限；INTERNAL_ERROR表示服务器内部错误；CLOSED_CONNECTION表示客户端已经关闭连接
enum HTTP_CODE
{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

class Http_request
{
public:
    void parse( char *read_buf);
    Http_infos get_http_infos();

private:
    LINE_STATUS parse_line( char *buffer, int &check_index, int &read_index);
    HTTP_CODE parse_requestline(const char *temp, CHECK_STATE &checkstate);
    HTTP_CODE parse_headline(const char *temp, CHECK_STATE &checkstate);
    HTTP_CODE parse_bodyline(const char *temp);
    HTTP_CODE parse_content( char *buffer, int &checked_index, CHECK_STATE &checkstate, int &read_index, int &start_line);
    Http_infos http_infos;
};
#endif // !HTTP_REQUEST_H
