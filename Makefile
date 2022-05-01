all:
	cd bin && g++ ../http/http_conn_response.cpp ../http/http_conn_request.cpp ../http/http_conn.cpp ../server/server.cpp \
	../pool/mysql_pool.cpp ../epoll/epoller.cpp ../timer/time_heap.cpp ../webserver.cpp -o web -g -std=c++17 -O0 -Wall -pthread -lmysqlclient
