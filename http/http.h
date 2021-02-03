
#ifndef HTTP_H
#define HTTP_H
#include <sys/uio.h>
#include <string>
void process(int connfd)
{
    int BUFFER_SIZE = 1024;
    char head[BUFFER_SIZE];
    memset(head, '\0', sizeof(head));
    char *file;
    struct stat st;
    std::string doc = "./source/web.html";
    struct iovec iv[2];
    stat(doc.data(), &st);
    int fd = open(doc.data(), O_RDONLY);
    file = new char[st.st_size + 1];
    memset(file, '\0', st.st_size + 1);
    read(fd, file, st.st_size);
    iv[1].iov_base = file;
    iv[1].iov_len = st.st_size;
    int len = 0;
    int ret = snprintf(head, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", "200");
    len += ret;
    //ret = snprintf(head + len, BUFFER_SIZE - 1 - len, "Content-Lenght: %d\r\n", st.st_size);
    //len += ret;
    ret = snprintf(head + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");

    iv[0].iov_base = head;
    iv[0].iov_len = strlen(head);

    writev(connfd, iv, 2);
}
#endif