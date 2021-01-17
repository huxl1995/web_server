#include "http_request.h"
#include <regex>
#include <string.h>
using namespace std;
LINE_STATUS Http_request::parse_line(char *buffer, int &check_index, int &read_index)
{
    char temp;
    //check_index指向buffer中当前正在分析的字节，read_index指向buffer中客户数据的尾部下一字节，buffer中第0～check_index字节都已分析完毕，第check_index~(read_index-1)
    //的字节由下面的循环分析
    for (; check_index < read_index; ++check_index)
    {
        //获得当前要分析的字节
        temp = buffer[check_index];
        //如果当前的字节是"\r",说明可能读到一个完整的行
        if (temp == '\r')
        {
            //如果‘\r'碰巧是buffer中最后一个已经被读入的客户数据，那么这次分析没有读取到一个完整的行，返回LINE_OPEN表示还需读取客户数据
            if (check_index + 1 == read_index)
            {
                return LINE_OPEN;
            }
            else if (buffer[check_index + 1] == '\n')
            {
                buffer[check_index++] = '\0';
                buffer[check_index++] = '\0';
                return LINE_OK;
            }
            //否则，说明HTTP请求存在语法问题
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if ((check_index > 1) && buffer[check_index - 1] == '\r')
            {
                buffer[check_index - 1] = '\0';
                buffer[check_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    //如果所有内容都分析完毕也没遇到'\r'字符，返回LINE_OPEN,表示还需要继续读取客户数据
    return LINE_OPEN;
}
HTTP_CODE Http_request::parse_requestline(const char *temp, CHECK_STATE &checkstate)
{

    string requestline = string(temp);
    regex r("^([A-Z]*) ([^ ]*) HTTP/1.1$");
    std::smatch m;
    if (regex_match(requestline, m, r))
    {
        if (m[1] == "GET")
        {
            http_infos.method = Http_infos::METHOD::GET;
        }
        else if (m[1] == "POST")
        {
            http_infos.method = Http_infos::METHOD::POST;
        }
        else
        {
            checkstate = CHECK_STATE_HEADER;
            return BAD_REQUEST;
        }
        if (m[2] == "/")
        {
            http_infos.file_name = "/index.html";
        }
        else
        {
            http_infos.file_name = m[2];
        }

        checkstate = CHECK_STATE_HEADER;
        return NO_REQUEST;
    }
    else
    {
        checkstate = CHECK_STATE_HEADER;
        return BAD_REQUEST;
    }
    //HTTP请求行处理完毕，状态转移到头部字段分析
    checkstate = CHECK_STATE_HEADER;
    return BAD_REQUEST;
}
HTTP_CODE Http_request::parse_headline(const char *temp, CHECK_STATE &checkstate)
{
    string headline = string(temp);
    regex r("^([^:]*): ?(.*)$");
    std::smatch m;
    if (regex_match(headline, m, r))
    {
        http_infos.head_state[m[1]] = m[2];
        return NO_REQUEST;
    }
    //HTTP请求头部处理完毕，状态转移到请求体字段分析
    checkstate = CHECK_STATE_BODY;
    return BAD_REQUEST;
}
HTTP_CODE Http_request::parse_bodyline(const char *temp)
{
    string bodyline = string(temp);
    regex r("^username=(.*)&&password=(.*)$");
    std::smatch m;
    if (regex_match(bodyline, m, r))
    {
        http_infos.username = m[1];
        http_infos.password = m[2];
        return GET_REQUEST;
    }
    return BAD_REQUEST;
}
HTTP_CODE Http_request::parse_content(char *buffer, int &checked_index, CHECK_STATE &checkstate, int &read_index, int &start_line)
{
    LINE_STATUS linestatus = LINE_OK; //记录当前行读取状态
    HTTP_CODE retcode = NO_REQUEST;   //记录HTTP请求的处理结果
    //主状态机，用于从buffer中取所有完整的行
    while ((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK)
    {
        char *temp = buffer + start_line; //start_line是行在buffer中的启始位置
        start_line = checked_index;       //记录下一行启始位置
        //checkstate记录主状态机当前的状态
        switch (checkstate)
        {
        case CHECK_STATE_REQUESTLINE: //第一个状态，分析请求行
        {
            retcode = parse_requestline(temp, checkstate);
            if (retcode == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            break;
        }
        case CHECK_STATE_HEADER:
        {
            retcode = parse_headline(temp, checkstate);
            if (retcode == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            else if (retcode == GET_REQUEST)
            {
                return GET_REQUEST;
            }
            break;
        }
        case CHECK_STATE_BODY:
        {
            retcode = parse_bodyline(temp);
            if (retcode == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            else if (retcode == GET_REQUEST)
            {
                return GET_REQUEST;
            }
            break;
        }
        default:
        {
            return INTERNAL_ERROR;
        }
        }
    }
    //若没有读取到一个完整的行，则表示还需要读取客户数据才能进一步分析
    if (linestatus == LINE_OPEN)
    {
        return NO_REQUEST;
    }
    else
    {
        return BAD_REQUEST;
    }
}
void Http_request::parse(char *read_buf)
{
    int checked_index = 0;
    CHECK_STATE check_state = CHECK_STATE_REQUESTLINE;
    int read_index = strlen(read_buf);
    int startline = 0;
    parse_content(read_buf, checked_index, check_state, read_index, startline);
}
Http_infos Http_request::get_http_infos()
{
    return http_infos;
}
