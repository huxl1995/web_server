#用C++实现的高性能web服务器

#功能
*利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
*利用正则表达式与有限状态机解析HTTP请求报文，实现处理静态资源的请求；

*利用单例模式实现数据库连接池，增加登录注册功能

#环境要求
*Linux
*C++11
*mysql
#目录树
、、、
.
├── bin
│   └── web
├── epoll
│   ├── epoller.cpp
│   └── epoller.h
├── http
│   ├── http_conn.cpp
│   ├── http_conn.h
│   ├── http.h
│   ├── http_infos.h
│   ├── http_request.cpp
│   ├── http_request.h
│   ├── http_response.cpp
│   └── http_response.h
├── Makefile
├── pool
│   ├── mysql_pool.cpp
│   ├── mysql_pool.h
│   ├── taskQueue.h
│   ├── threadpool.h
│   └── threadPool.h
├── readme.md
├── server
│   ├── server.cpp
│   └── server.h
├── sources
│   ├── 404.html
│   ├── error.html
│   ├── index.html
│   ├── login_failure.html
│   ├── login.html
│   ├── login_success.html
│   ├── picture.jpg
│   ├── register_failure.html
│   ├── register.html
│   ├── register_success.html
│   ├── video.html
│   └── video.mp4
└── webserver.cpp



、、、


#项目启动
创建一个数据库，新建一个名为userinfo的表，字段设置为username,password
、、、bash
make
./bin/web
、、、