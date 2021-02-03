#include "http_response.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
using namespace std;

const unordered_map<int, string> Http_response::CODE_STATUS{
    {200, "OK"},
    {400, "Bad Request"}};
Http_response::Http_response()
{
    mcp = Mysql_conn_pool::instance();
}

void Http_response::init(Http_infos &http_infos)
{
    http_infos_ = http_infos;
}
void Http_response::process()
{
    if (http_infos_.method == Http_infos::METHOD::GET)
    {
        process_GET();
    }
    else if (http_infos_.method == Http_infos::METHOD::POST)
    {
        process_POST();
    }
    else
    {
        process_error();
    }
}
void Http_response::open_file(string &file_name)
{
    if (stat((source + file_name).data(), &mystat) < 0)
    {
        code = 400;
        file_name = "/404.html";
    }
    else
    {
        code = 200;
    }
    int file_fd = open((source + file_name).data(), O_RDONLY);
    mm_file = new char[mystat.st_size + 1];
    memset(mm_file, '\0', sizeof(mm_file));
    int len = read(file_fd, mm_file, mystat.st_size);
    close(file_fd);
}
void Http_response::process_GET()
{
    open_file(http_infos_.file_name);
}
void Http_response::process_POST()
{
    MYSQL *mysql = mcp->get_conn();
    MYSQL_FIELD *mysql_field;
    MYSQL_RES *mysql_res;
    MYSQL_ROW mysql_row;
    string username = http_infos_.username;
    string password = http_infos_.password;
    char *query = new char[256];
    string file_name;
    if (http_infos_.file_name == "/login.html")
    {
        snprintf(query, 256, "SELECT password FROM userinfo WHERE username='%s'", username.data());
        int ret = mysql_query(mysql, query);
        if (ret)
        {
            file_name = "/error.html";
            open_file(file_name);
        }
        else
        {
            mysql_res = mysql_store_result(mysql);

            if (mysql_row = mysql_fetch_row(mysql_res))
            {
                if (mysql_row[0] == password)
                {
                    file_name = "/login_success.html";
                    open_file(file_name);
                }
                else
                {
                    file_name = "/login_failure.html";
                    open_file(file_name);
                }
            }
            else
            {
                file_name = "/login_failure.html";
                open_file(file_name);
            }
        }
        mysql_free_result(mysql_res);
    }
    else
    {
        snprintf(query, 256, "INSERT INTO userinfo values('%s','%s')", username.data(), password.data());
        int ret = mysql_query(mysql, query);
        if (ret)
        {
            file_name = "/register_failure.html";
            open_file(file_name);
        }
        else
        {
            file_name = "/register_success.html";
            open_file(file_name);
        }
    }
    mcp->free_conn(mysql);
}
void Http_response::process_error()
{
}
void Http_response::make_response(iovec *iv, int &iv_count)
{
    string status;
    status = CODE_STATUS.find(code)->second;
    string temp = "HTTP/1.1 " + to_string(code) + "\r\n";
    temp = temp + "Content-Length: " + to_string(mystat.st_size) + "\r\n\r\n";
    char head[1024];
    memset(head, '\0', sizeof(head));
    snprintf(head, 1024, "%s", temp.data());
    iv[0].iov_base = head;
    iv[0].iov_len = strlen(head);
    iv[1].iov_base = mm_file;
    iv[1].iov_len = mystat.st_size;
    iv_count = 2;
}
