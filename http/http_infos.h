#ifndef HTTP_INFOS_H
#define HTTP_INFOS_H
struct Http_infos
{
    enum METHOD
    {
        GET = 0,
        POST
    } method;
    std::unordered_map<std::string, std::string> head_state;
    std::string file_name;
    std::string username;
    std::string password;
};
#endif