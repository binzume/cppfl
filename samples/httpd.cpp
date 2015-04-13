#include <iostream>
using namespace std;
#include "../include/httpserver.h"
#include "../include/thread.h"
#include "../include/file.h"
using namespace Net;

class QTree {
public:
	QTree *child[4];
	unsigned long long data;

	int get(long x,long y) {
		return 0;
	}
	void set(long x,long y,int d) {
	}
};


bool handler(HTTPResponse &res,HttpRequest &req) {

	if (req.path=="/") {
		File f("index.html");
		string s;
		f.load(s);
		
		res.contentType = "text/html";
		res.write(s);
	} else {
		res.write("Hello, world!\n");
		res.write(" method:" + req.method);
		res.write(" path:" + req.path);
		res.write(" query:" + req.query_string);
	}

	return false;
}

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
	
	s.server.handler("/", [] (HTTPResponse &res,HttpRequest &req){
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

