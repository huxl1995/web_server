#include "http_conn.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
using namespace std;

const unordered_map<int, string> Http_conn::CODE_STATUS{
    {200, "OK"},
    {400, "Bad Request"}};
void Http_conn::process_response()
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
void Http_conn::open_file(string &file_name)
{
    if (file_name=="" || stat((source_ + file_name).data(), &mystat) < 0)
    {
        code = 400;
        file_name = "/404.html";
    }
    else
    {
        code = 200;
    }
    int file_fd = open((source_ + file_name).data(), O_RDONLY);
    mm_file = (char *)mmap(0, mystat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    close(file_fd);
}
void Http_conn::process_GET()
{
    open_file(http_infos_.file_name);
}
void Http_conn::process_POST()
{
    MYSQL *mysql = mcp->get_conn();

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

            if ((mysql_row = mysql_fetch_row(mysql_res)))
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
void Http_conn::process_error()
{
}
void Http_conn::make_response(iovec *iv, int &iv_count)
{
    string status;
    status = CODE_STATUS.find(code)->second;
    string temp = "HTTP/1.1 " + to_string(code) + " " + status + "\r\n";
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
void Http_conn::make_response()
{
    string status = CODE_STATUS.find(code)->second;
    string heads = "HTTP/1.1 " + to_string(code) + " " + status + "\r\n";
    heads = heads + "Content-Length: " + to_string(mystat.st_size) + "\r\n\r\n";
    iv[0].iov_len = snprintf(head, 1024, "%s", heads.data());
    iv[0].iov_base = head;
    iv[1].iov_base = mm_file;
    iv[1].iov_len = mystat.st_size;
    ivcount = 2;
    size_to_response=iv[0].iov_len+iv[1].iov_len;
}
/*
bool Http_conn::write_response()
{
    int w_len=0;
    int have_send = 0;
    while(1)
    {
        w_len = writev(fd_, iv, ivcount);
        if(w_len<=-1)
        {
            if(errno==EAGAIN){
                
                return;
            }
            
            unmap();
            
        }
 
        have_send+=w_len;
        if(have_send>=size_to_response){
            break;
        }
    }

    unmap();
}*/

void Http_conn::unmap()
{
    if (mm_file)
    {
        munmap(mm_file, mystat.st_size);
        mm_file = 0;
    }
}