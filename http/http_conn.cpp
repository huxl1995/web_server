#include "http_conn.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
using namespace std;
void Http_conn::run(int sockfd,const sockaddr_in& addr,string sources)
{
    reset();
    init(sockfd,addr,sources);
    process();
}
// void Http_conn::run(int sockfd, const sockaddr_in &addr)
// {
//     reset();
//     init(sockfd, addr);
//     process();
// }
void Http_conn::init(const int sockfd, const sockaddr_in &addr,string sources)
{
    assert(sockfd > 0);
    fd_ = sockfd;
    addr_ = addr;
    source_=sources;
    /*
    int reuse=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    */
}
/*
void Http_conn::init(const int sockfd)
{
    fd_ = sockfd;
    int reuse=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
}*/
bool Http_conn::read_data()
{
    int bytes_read=0;
    while(true)
    {
        bytes_read=recv(fd_,read_buf+read_idx_,BUFFER_SIZE-read_idx_,0);
        if(bytes_read==-1)
        {
            if(errno==EAGAIN || errno==EWOULDBLOCK)
            {
                break;
            }
            return false;
        }
        else if(bytes_read==0)
        {
            return false;
        }
        read_idx_+=bytes_read;
    }

    return true;
}

void Http_conn::process()
{
    //m_.lock();、
    reset();
    read_data();
    parse_content();
    //http_infos = request.get_http_infos();
    //response.init(http_infos, fd_);
    process_response();
    make_response();

    return;
}

bool Http_conn::write_()
{
    int w_len=0;
    int have_send = 0;
    while(1)
    {
        try
        {
            w_len = writev(fd_, iv, ivcount);

        }
        catch(...)
        {
            return false;
        }
        if(w_len<=-1)
        {
            if(errno==EAGAIN){
                
                return false;
            }
            unmap();
            return false;
        }
 
        have_send+=w_len;
        if(have_send>=size_to_response){
            break;
        }
    }

    unmap();
    return true;
}

void Http_conn::closes()
{
    close(fd_);
    memset(&addr_, 0, sizeof(addr_));
}

void Http_conn::reset()
{

    http_infos_.method = Http_infos::METHOD::GET;
    http_infos_.file_name = "";
    http_infos_.head_state.clear();
    http_infos_.password = "";
    http_infos_.username = "";
}
/*
void Http_conn::init_test(const int sockfd)
{
    fd_ = sockfd;
}
void Http_conn::read_test()
{
    char readbuf[BUFFER_SIZE];
    memset(readbuf, 0, BUFFER_SIZE);
    read(fd_, readbuf, BUFFER_SIZE);
}
void Http_conn::write_test()
{
    char writebuf[BUFFER_SIZE];
    memset(writebuf, 0, BUFFER_SIZE);
    sprintf(writebuf, "%s\n", "helloword");
    write(fd_, writebuf, BUFFER_SIZE);
}
*/