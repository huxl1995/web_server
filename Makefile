all:
	cd bin && g++ .././http/http_request.cpp .././http/http_response.cpp .././http/http_conn.cpp .././server/server.cpp \
	.././pool/mysql_pool.cpp ../epoll/epoller.cpp ../webserver.cpp -o web -g -pthread -lmysqlclient
