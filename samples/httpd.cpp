#include <iostream>
using namespace std;
#include "../include/httpserver.h"
#include "../include/thread.h"
#include "../include/file.h"
using namespace net;

class Httpd : public Thread {
public:
	HttpServer server;
	int task() {
		server.start(8080);
		server.wait();
	}
};

int main() {

	Httpd s;

	s.server.handler("/file", [] (HTTPResponse &res,HttpRequest &req){
		cout << "path:" << req.path << endl;
		File f("httpd_index.html");
		string s;
		f.load(s);
		res.contentType = "text/html";
		res.write(s);
		return false;
	});

	s.server.handler("/test", [] (HTTPResponse &res,HttpRequest &req){
		cout << "path:" << req.path << endl;
		res.contentType = "text/plain";
		res.write("Hello, world!\n");
		res.write(" method:" + req.method);
		res.write(" path:" + req.path);
		res.write(" query:" + req.query_string);
		return false;
	});

	s.start();
	s.join();

	return 0;
}

