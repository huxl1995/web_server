#include "http_conn.h"
#include <regex>
#include <string.h>
using namespace std;
LINE_STATUS Http_conn::parse_line()
{
    char temp='0';
    //check_index指向buffer中当前正在分析的字节，read_index指向buffer中客户数据的尾部下一字节，buffer中第0～check_index字节都已分析完毕，第check_index~(read_index-1)
    //的字节由下面的循环分析
    int temp_check_idx=check_idx_;
    for (; temp_check_idx < read_idx_; ++temp_check_idx)
    {
        //获得当前要分析的字节
        temp = read_buf[temp_check_idx];
        //如果当前的字节是"\r",说明可能读到一个完整的行
        if (temp == '\r')
        {
            //如果‘\r'碰巧是buffer中最后一个已经被读入的客户数据，那么这次分析没有读取到一个完整的行，返回LINE_OPEN表示还需读取客户数据
            if (temp_check_idx + 1 == read_idx_)
            {
                return LINE_OPEN;
            }
            else if (read_buf[temp_check_idx + 1] == '\n')
            {
                read_buf[temp_check_idx++] = '\0';
                read_buf[temp_check_idx++] = '\0';
                check_idx_=temp_check_idx;
                return LINE_OK;
            }
            //否则，说明HTTP请求存在语法问题
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if ((temp_check_idx > 1) && read_buf[temp_check_idx - 1] == '\r')
            {
                read_buf[temp_check_idx - 1] = '\0';
                read_buf[temp_check_idx++] = '\0';
                check_idx_=temp_check_idx;
                return LINE_OK;
            }
            check_idx_=temp_check_idx+1;
            return LINE_BAD;
        }
    }
    //如果所有内容都分析完毕也没遇到'\r'字符，返回LINE_OPEN,表示还需要继续读取客户数据
    return LINE_OPEN;
}
PARSE_RESULT Http_conn::parse_requestline(const char *temp)
{

    string requestline = string(temp);
    regex r("^([A-Z]*) ([^ ]*) HTTP/[0-2].[0-9]$");
    std::smatch m;
    if (regex_match(requestline, m, r))
    {
        if (m[1] == "GET")
        {
            http_infos_.method = Http_infos::METHOD::GET;
        }
        else if (m[1] == "POST")
        {
            http_infos_.method = Http_infos::METHOD::POST;
        }
        else
        {
            http_infos_.method = Http_infos::METHOD::GET;
            check_state_ = CHECK_STATE_HEADER;
        }
        if (m[2] == "/")
        {
            http_infos_.file_name = "/index.html";
        }
        else
        {
            string file = m[2];
            if (file.find(".") == file.npos)
            {
                file = file + ".html";
            }

            http_infos_.file_name = file;
        }

        check_state_ = CHECK_STATE_HEADER;
    }
    else
    {
        check_state_ = CHECK_STATE_REQUESTLINE;
    }
    return PARSE_END;

}
PARSE_RESULT Http_conn::parse_headline(const char *temp)
{
    string headline = string(temp);
    if (headline == "")
    {
        if(http_infos_.method==Http_infos::METHOD::GET)
        {
            //HTTP请求报文解析结束
            check_state_= CHECK_STATE_END;
        }
        else
        {
            //HTTP请求头部处理完毕，状态转移到请求体字段分析
            check_state_= CHECK_STATE_BODY;
        }
        return PARSE_END;
        
    }

    regex r("^([^:]*): ?(.*)$");
    std::smatch m;
    if (regex_match(headline, m, r))
    {
        http_infos_.head_state[m[1]] = m[2];
        return PARSE_OK;
    }
    return PARSE_FAIL;
}
PARSE_RESULT Http_conn::parse_bodyline(const char *temp)
{
    string bodyline = string(temp);
    regex r("^username=(.*)&password=(.*)$");
    std::smatch m;
    if (regex_match(bodyline, m, r))
    {
        http_infos_.username = m[1];
        http_infos_.password = m[2];
        PARSE_END;
    }
    return PARSE_FAIL;
}
PARSE_RESULT Http_conn::parse_content()
{
    LINE_STATUS linestatus = LINE_OK; //记录当前行读取状态
    PARSE_RESULT retcode = PARSE_FAIL;   //记录HTTP请求的处理结果
    bool parse=true;
    //主状态机，用于从buffer中取所有完整的行
    while (parse && (linestatus = parse_line()) == LINE_OK)
    {
        char *temp = read_buf + start_line_; //start_line是行在buffer中的启始位置
        start_line_ = check_idx_;       //记录下一行启始位置

        //checkstate记录主状态机当前的状态
        switch (check_state_)
        {
        case CHECK_STATE_REQUESTLINE: //第一个状态，分析请求行
        {
            retcode = parse_requestline(temp);
            break;
        }
        case CHECK_STATE_HEADER:
        {

            retcode = parse_headline(temp);
            if (retcode == PARSE_END)
            {
                memcpy(read_buf,read_buf+check_idx_,read_idx_-check_idx_+1);
                read_idx_=read_idx_-check_idx_;
                check_idx_=0;
                start_line_=0;
                parse=false;
                check_state_=CHECK_STATE_REQUESTLINE;
            }
            break;
        }
        case CHECK_STATE_BODY:
        {
            retcode = parse_bodyline(temp);
            memcpy(read_buf,read_buf+check_idx_,read_idx_-check_idx_+1);
            read_idx_=read_idx_-check_idx_;
            check_idx_=0;
            start_line_=0;
            parse=false;
            check_state_=CHECK_STATE_REQUESTLINE;

            break;
        }
        default:
        {
            return PARSE_FAIL;
        }
        }
    }
    //若没有读取到一个完整的行，则表示还需要读取客户数据才能进一步分析
    if (linestatus == LINE_OPEN)
    {
        return PARSE_END;
    }

}
